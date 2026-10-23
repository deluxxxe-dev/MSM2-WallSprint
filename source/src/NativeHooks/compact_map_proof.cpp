#include "NativeHooks/compact_map_proof.hpp"
#include <capstone/capstone.h>
#include <algorithm>
#include <array>
#include <bitset>
#include <cstring>
#include <vector>
namespace ws::compact_proof {
namespace {
struct Decoder {
 csh handle{};cs_insn* instructions{};std::size_t count{};
 ~Decoder(){if(instructions)cs_free(instructions,count);if(handle)cs_close(&handle);}
 bool decode(const unsigned char* bytes,std::size_t size,Result& result){
  if(!bytes||!size||size>1024){result.reason="invalid_function_span";return false;}
  auto status=cs_open(CS_ARCH_X86,CS_MODE_64,&handle);result.decoderStatus=status;
  if(status!=CS_ERR_OK){result.reason="decoder_open_failed";return false;}
  if(cs_option(handle,CS_OPT_DETAIL,CS_OPT_ON)!=CS_ERR_OK||cs_option(handle,CS_OPT_SKIPDATA,CS_OPT_OFF)!=CS_ERR_OK){result.reason="decoder_configuration_failed";return false;}
  count=cs_disasm(handle,bytes,size,0,0,&instructions);result.decoderStatus=cs_errno(handle);
  unsigned end=0;
  for(std::size_t i=0;i<count;++i){const auto& in=instructions[i];
   if(in.address!=end||!in.size||!in.id||!in.detail){result.reason="non_contiguous_decode_or_missing_detail";result.failureOffset=end;return false;}
   end+=in.size;
  }
  result.decodedBytes=end;result.decodedInstructions=unsigned(count);result.failureOffset=end;
  if(end!=size){result.reason="undecodable_or_truncated_instruction";return false;}
  return true;
 }
};
enum Kind {unknown,source,target,stack,value};
struct Fact {Kind kind{};std::int64_t offset{};};
int gpr(unsigned reg){
 switch(reg){
  case X86_REG_RAX:case X86_REG_EAX:case X86_REG_AX:case X86_REG_AL:case X86_REG_AH:return 0;
  case X86_REG_RCX:case X86_REG_ECX:case X86_REG_CX:case X86_REG_CL:case X86_REG_CH:return 1;
  case X86_REG_RDX:case X86_REG_EDX:case X86_REG_DX:case X86_REG_DL:case X86_REG_DH:return 2;
  case X86_REG_RBX:case X86_REG_EBX:case X86_REG_BX:case X86_REG_BL:case X86_REG_BH:return 3;
  case X86_REG_RSP:case X86_REG_ESP:case X86_REG_SP:case X86_REG_SPL:return 4;
  case X86_REG_RBP:case X86_REG_EBP:case X86_REG_BP:case X86_REG_BPL:return 5;
  case X86_REG_RSI:case X86_REG_ESI:case X86_REG_SI:case X86_REG_SIL:return 6;
  case X86_REG_RDI:case X86_REG_EDI:case X86_REG_DI:case X86_REG_DIL:return 7;
  case X86_REG_R8:case X86_REG_R8D:case X86_REG_R8W:case X86_REG_R8B:return 8;
  case X86_REG_R9:case X86_REG_R9D:case X86_REG_R9W:case X86_REG_R9B:return 9;
  case X86_REG_R10:case X86_REG_R10D:case X86_REG_R10W:case X86_REG_R10B:return 10;
  case X86_REG_R11:case X86_REG_R11D:case X86_REG_R11W:case X86_REG_R11B:return 11;
  case X86_REG_R12:case X86_REG_R12D:case X86_REG_R12W:case X86_REG_R12B:return 12;
  case X86_REG_R13:case X86_REG_R13D:case X86_REG_R13W:case X86_REG_R13B:return 13;
  case X86_REG_R14:case X86_REG_R14D:case X86_REG_R14W:case X86_REG_R14B:return 14;
  case X86_REG_R15:case X86_REG_R15D:case X86_REG_R15W:case X86_REG_R15B:return 15;
  default:return -1;
 }
}
int simd(unsigned reg){
 if(reg>=X86_REG_XMM0&&reg<=X86_REG_XMM31)return int(reg-X86_REG_XMM0);
 if(reg>=X86_REG_YMM0&&reg<=X86_REG_YMM31)return int(reg-X86_REG_YMM0);
 if(reg>=X86_REG_ZMM0&&reg<=X86_REG_ZMM31)return int(reg-X86_REG_ZMM0);
 return -1;
}
struct State {std::array<Fact,16> regs{};std::array<bool,32> products{};std::bitset<256> locals{};};
Fact address(const cs_x86_op& op,const State& state){
 if(op.type!=X86_OP_MEM||op.mem.segment!=X86_REG_INVALID||op.mem.index!=X86_REG_INVALID)return {};
 auto base=gpr(op.mem.base);if(base<0)return {};
 auto fact=state.regs[base];if(fact.kind!=source&&fact.kind!=target&&fact.kind!=stack)return {};
 fact.offset+=op.mem.disp;return fact;
}
bool group(const cs_insn& in,unsigned wanted){for(unsigned i=0;i<in.detail->groups_count;++i)if(in.detail->groups[i]==wanted)return true;return false;}
bool transfer(const cs_insn& in){return group(in,CS_GRP_JUMP)||group(in,CS_GRP_CALL)||group(in,CS_GRP_RET)||group(in,CS_GRP_INT)||group(in,CS_GRP_IRET);}
// Model exact moves/LEA/stack adjustments; every other register write loses its
// fact via Capstone's explicit+implicit write set. Unrelated stack saves and
// VEX arithmetic are decoded, never skipped or mistaken for RDX/RCX accesses.
bool step(Decoder& decoder,const cs_insn& in,State& state,Result& result){
 auto before=state;auto& x=in.detail->x86;cs_regs read{},written{};std::uint8_t nr{},nw{};
 if(cs_regs_access(decoder.handle,&in,read,&nr,written,&nw)!=CS_ERR_OK){result.reason="register_effects_unavailable";return false;}
 for(unsigned i=0;i<nw;++i){auto r=gpr(written[i]);if(r>=0)state.regs[r]={};auto v=simd(written[i]);if(v>=0)state.products[v]=false;}
 if(in.id==X86_INS_PUSH||in.id==X86_INS_POP){auto sp=before.regs[4];if(sp.kind==stack){sp.offset+=in.id==X86_INS_PUSH?-8:8;state.regs[4]=sp;}}
 if(!x.op_count)return true;
 auto& dest=x.operands[0];auto dr=dest.type==X86_OP_REG?gpr(dest.reg):-1;
 if((in.id==X86_INS_ADD||in.id==X86_INS_SUB)&&dr>=0&&dest.size==8&&x.op_count==2&&x.operands[1].type==X86_OP_IMM){auto f=before.regs[dr];if(f.kind==stack){f.offset+=(in.id==X86_INS_ADD?1:-1)*x.operands[1].imm;state.regs[dr]=f;}}
 if(in.id==X86_INS_LEA&&dr>=0&&dest.size==8&&x.op_count==2)state.regs[dr]=address(x.operands[1],before);
 if(in.id==X86_INS_MOV&&dr>=0&&x.op_count==2){auto& src=x.operands[1];
  if(src.type==X86_OP_REG){auto sr=gpr(src.reg);if(sr>=0&&((dest.size==8&&src.size==8)||(dest.size==4&&src.size==4&&before.regs[sr].kind==value)))state.regs[dr]=before.regs[sr];}
  else if(src.type==X86_OP_MEM&&dest.size==4&&src.size==4){auto a=address(src,before);if(a.kind==source)state.regs[dr]={value,a.offset};}
 }
 auto vr=dest.type==X86_OP_REG?simd(dest.reg):-1;
 if(vr>=0&&(in.id==X86_INS_VMULSS||in.id==X86_INS_MULSS))state.products[vr]=true;
 return true;
}
bool fail(Result& r,const cs_insn& in,const char* reason){r.failureOffset=unsigned(in.address);r.reason=reason;return false;}
Result proveWriter(const unsigned char* bytes,std::size_t size){
 Result r{};Decoder d;if(!d.decode(bytes,size,r))return r;
 State state{};state.regs[1]={target,0};state.regs[2]={source,0};state.regs[4]={stack,0};std::bitset<9> seen;
 for(std::size_t i=0;i<d.count;++i){auto& in=d.instructions[i];auto& x=in.detail->x86;
  if(transfer(in)){if(r.pairs!=9)fail(r,in,"control_flow_before_complete_copy_map");break;}
  if(in.id==X86_INS_MOV&&x.op_count==2&&x.operands[0].type==X86_OP_MEM){
   auto dest=address(x.operands[0],state);auto& src=x.operands[1];auto sr=src.type==X86_OP_REG?gpr(src.reg):-1;
   if(dest.kind==target&&dest.offset>=0&&dest.offset<48){
    bool padding=dest.offset%16==12;
    if(!padding){
     if(src.size!=4||x.operands[0].size!=4||sr<0||state.regs[sr].kind!=value){fail(r,in,"orientation_store_not_direct_dword_source");return r;}
     auto offset=state.regs[sr].offset;
     if(offset<0||offset>32||offset%4){fail(r,in,"source_offset_outside_compact_payload");return r;}
     auto index=unsigned(offset)/4;auto expected=(index/3)*16+(index%3)*4;
     if(dest.offset!=expected){fail(r,in,"destination_offset_mismatch");return r;}
     if(seen[index]){fail(r,in,"duplicate_or_ambiguous_copy_pair");return r;}
     seen.set(index);r.map[index]={unsigned(in.address),unsigned(offset),unsigned(dest.offset)};++r.pairs;
    }
   }else if(dest.kind==target&&sr>=0&&state.regs[sr].kind==value){fail(r,in,"direct_source_store_outside_orientation_region");return r;}
  }
  if(!step(d,in,state,r)){r.failureOffset=unsigned(in.address);return r;}
 }
 r.admitted=r.pairs==9;r.reason=r.admitted?"full_decode_nine_anchored_direct_copies":"missing_compact_copy_pairs";
 if(r.admitted)r.failureOffset=unsigned(size);
 return r;
}
Result proveCaller(const unsigned char* bytes,std::size_t size,std::uintptr_t begin,std::uintptr_t targetRva,std::uintptr_t returnRva){
 Result r{};if(size!=128||returnRva<=begin||returnRva>begin+size){r.reason="caller_span_or_return_invalid";return r;}
 Decoder d;if(!d.decode(bytes,size,r))return r;
 State state{};state.regs[4]={stack,0};
 for(std::size_t i=0;i<d.count;++i){auto& in=d.instructions[i];auto& x=in.detail->x86;
  if(begin+in.address+in.size==returnRva){
   if(in.id!=X86_INS_CALL||in.size!=5||bytes[in.address]!=0xe8||x.op_count!=1||x.operands[0].type!=X86_OP_IMM){fail(r,in,"caller_return_not_exact_direct_call");return r;}
   std::int32_t displacement{};std::memcpy(&displacement,bytes+in.address+1,4);
   if(std::int64_t(returnRva)+displacement!=std::int64_t(targetRva)){fail(r,in,"caller_direct_target_mismatch");return r;}
   auto sp=state.regs[4],rdx=state.regs[2];
   if(sp.kind!=stack||rdx.kind!=stack||rdx.offset!=sp.offset+0x20){fail(r,in,"caller_rdx_not_callee_rsp_plus_0x28");return r;}
   for(unsigned k=0;k<9;++k){auto off=rdx.offset+k*4;if(off< -512||off>=512||off%4||!state.locals[unsigned(off+512)/4]){fail(r,in,"caller_compact_scalar_product_not_initialized");return r;}}
   r.admitted=true;r.pairs=9;r.reason="full_decode_caller_nine_products_stack_relation_and_call";r.failureOffset=unsigned(size);return r;
  }
  if(transfer(in)){fail(r,in,"unexpected_control_flow_before_exact_call");return r;}
  for(unsigned j=0;j<x.op_count;++j){auto& op=x.operands[j];
   // Capstone 5.0.6 marks VMOVSS memory destinations CS_AC_READ. The
   // decoded opcode and operand position, rather than that access flag,
   // establish direction for this known store. Other memory writes still
   // invalidate the tracked local; a memory read never initializes it.
   bool scalarStore=j==0&&(in.id==X86_INS_VMOVSS||in.id==X86_INS_MOVSS);
   if(op.type!=X86_OP_MEM||(!(op.access&CS_AC_WRITE)&&!scalarStore))continue;
   auto a=address(op,state);if(a.kind!=stack)continue;
   if(a.offset< -512||a.offset+op.size>512||a.offset%4||op.size%4){fail(r,in,"caller_local_store_outside_tracked_span");return r;}
   for(unsigned k=0;k<op.size;k+=4)state.locals.reset(unsigned(a.offset+512+k)/4);
   if((in.id==X86_INS_VMOVSS||in.id==X86_INS_MOVSS)&&j==0&&op.size==4&&x.op_count==2&&x.operands[1].type==X86_OP_REG){auto v=simd(x.operands[1].reg);if(v>=0&&state.products[v])state.locals.set(unsigned(a.offset+512)/4);}
  }
  if(!step(d,in,state,r)){r.failureOffset=unsigned(in.address);return r;}
 }
 r.reason="exact_caller_return_not_instruction_boundary";return r;
}
}
Result writer(const unsigned char* bytes,std::size_t size) noexcept {try{return proveWriter(bytes,size);}catch(...){Result r{};r.reason="proof_allocation_or_internal_failure";return r;}}
Result callerProof(const unsigned char* bytes,std::size_t size,std::uintptr_t begin,std::uintptr_t target,std::uintptr_t ret) noexcept {try{return proveCaller(bytes,size,begin,target,ret);}catch(...){Result r{};r.reason="proof_allocation_or_internal_failure";return r;}}
bool caller(const unsigned char* bytes,std::size_t size,std::uintptr_t begin,std::uintptr_t target,std::uintptr_t ret) noexcept {return callerProof(bytes,size,begin,target,ret).admitted;}
}
