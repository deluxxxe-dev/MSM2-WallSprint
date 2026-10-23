#include "NativeHooks/path_a_position_proof.hpp"
#include <capstone/capstone.h>
#include <cstring>
#include <algorithm>
#include <cstdint>
namespace ws::position_proof {
namespace {
struct Decode {
 csh handle{};cs_insn* ins{};std::size_t count{};
 ~Decode(){if(ins)cs_free(ins,count);if(handle)cs_close(&handle);}
 bool run(const unsigned char* bytes,std::size_t size,unsigned& decoded,unsigned& failure){
  if(!bytes||!size||size>1024||cs_open(CS_ARCH_X86,CS_MODE_64,&handle)!=CS_ERR_OK)return false;
  if(cs_option(handle,CS_OPT_DETAIL,CS_OPT_ON)!=CS_ERR_OK||cs_option(handle,CS_OPT_SKIPDATA,CS_OPT_OFF)!=CS_ERR_OK)return false;
  count=cs_disasm(handle,bytes,size,0,0,&ins);decoded=0;
  for(std::size_t i=0;i<count;++i){auto& p=ins[i];if(!p.id||!p.detail||!p.size||p.address!=decoded)break;decoded+=p.size;}
  failure=decoded;return decoded==size;
 }
};
int reg(unsigned r){switch(r){
 case X86_REG_RAX:case X86_REG_EAX:case X86_REG_AX:case X86_REG_AL:case X86_REG_AH:return 0;case X86_REG_RCX:case X86_REG_ECX:case X86_REG_CX:case X86_REG_CL:case X86_REG_CH:return 1;
 case X86_REG_RDX:case X86_REG_EDX:case X86_REG_DX:case X86_REG_DL:case X86_REG_DH:return 2;case X86_REG_RBX:case X86_REG_EBX:case X86_REG_BX:case X86_REG_BL:case X86_REG_BH:return 3;
 case X86_REG_RSP:case X86_REG_ESP:case X86_REG_SP:case X86_REG_SPL:return 4;case X86_REG_RBP:case X86_REG_EBP:case X86_REG_BP:case X86_REG_BPL:return 5;
 case X86_REG_RSI:case X86_REG_ESI:case X86_REG_SI:case X86_REG_SIL:return 6;case X86_REG_RDI:case X86_REG_EDI:case X86_REG_DI:case X86_REG_DIL:return 7;
 case X86_REG_R8:case X86_REG_R8D:case X86_REG_R8W:case X86_REG_R8B:return 8;case X86_REG_R9:case X86_REG_R9D:case X86_REG_R9W:case X86_REG_R9B:return 9;
 case X86_REG_R10:case X86_REG_R10D:case X86_REG_R10W:case X86_REG_R10B:return 10;case X86_REG_R11:case X86_REG_R11D:case X86_REG_R11W:case X86_REG_R11B:return 11;
 case X86_REG_R12:case X86_REG_R12D:case X86_REG_R12W:case X86_REG_R12B:return 12;case X86_REG_R13:case X86_REG_R13D:case X86_REG_R13W:case X86_REG_R13B:return 13;
 case X86_REG_R14:case X86_REG_R14D:case X86_REG_R14W:case X86_REG_R14B:return 14;case X86_REG_R15:case X86_REG_R15D:case X86_REG_R15W:case X86_REG_R15B:return 15;default:return -1;}}
int vector(unsigned r){if(r>=X86_REG_XMM0&&r<=X86_REG_XMM31)return int(r-X86_REG_XMM0);if(r>=X86_REG_YMM0&&r<=X86_REG_YMM31)return int(r-X86_REG_YMM0);return r>=X86_REG_ZMM0&&r<=X86_REG_ZMM31?int(r-X86_REG_ZMM0):-1;}
enum Root {Unknown,Source,Target,Stack,Loaded};
struct Fact {Root root{};long long off{};unsigned bytes{};};
struct State {std::array<Fact,16> gpr{};std::array<Fact,32> xmm{};};
Fact memory(const cs_x86_op& p,const State& s){if(p.type!=X86_OP_MEM||p.mem.segment!=X86_REG_INVALID||p.mem.index!=X86_REG_INVALID)return {};auto r=reg(p.mem.base);if(r<0)return {};auto f=s.gpr[r];if(f.root!=Source&&f.root!=Target&&f.root!=Stack)return {};f.off+=p.mem.disp;return f;}
bool group(const cs_insn& p,unsigned id){for(unsigned i=0;i<p.detail->groups_count;++i)if(p.detail->groups[i]==id)return true;return false;}
bool move(unsigned id){return id==X86_INS_MOV||id==X86_INS_MOVAPS||id==X86_INS_MOVUPS||id==X86_INS_VMOVAPS||id==X86_INS_VMOVUPS||id==X86_INS_MOVSS||id==X86_INS_VMOVSS||id==X86_INS_MOVDQU||id==X86_INS_VMOVDQU;}
Fact value(const cs_x86_op& p,const State& s){if(p.type!=X86_OP_REG)return {};auto g=reg(p.reg);if(g>=0)return s.gpr[g];auto v=vector(p.reg);return v>=0?s.xmm[v]:Fact{};}
void step(Decode& d,const cs_insn& p,State& s){
 auto old=s;auto& x=p.detail->x86;cs_regs reads{},writes{};std::uint8_t nr{},nw{};
 if(cs_regs_access(d.handle,&p,reads,&nr,writes,&nw)!=CS_ERR_OK){s={};return;}
 for(unsigned i=0;i<nw;++i){auto g=reg(writes[i]);if(g>=0)s.gpr[g]={};auto v=vector(writes[i]);if(v>=0)s.xmm[v]={};}
 if(p.id==X86_INS_PUSH||p.id==X86_INS_POP){auto f=old.gpr[4];if(f.root==Stack){f.off+=p.id==X86_INS_PUSH?-8:8;s.gpr[4]=f;}}
 if(!x.op_count)return;
 auto& dest=x.operands[0];auto g=dest.type==X86_OP_REG?reg(dest.reg):-1;
 if(g>=0&&dest.size==8&&x.op_count==2&&(p.id==X86_INS_SUB||p.id==X86_INS_ADD)&&x.operands[1].type==X86_OP_IMM){auto f=old.gpr[g];if(f.root==Stack){f.off+=(p.id==X86_INS_SUB?-1:1)*x.operands[1].imm;s.gpr[g]=f;}}
 if(g>=0&&dest.size==8&&p.id==X86_INS_LEA&&x.op_count==2)s.gpr[g]=memory(x.operands[1],old);
 if(move(p.id)&&dest.type==X86_OP_REG&&x.op_count==2){auto& src=x.operands[1];Fact f{};
  if(src.type==X86_OP_REG){f=value(src,old);if(f.root!=Loaded&&(dest.size!=8||src.size!=8))f={};}
  else {auto a=memory(src,old);if(a.root==Source)f={Loaded,a.off,src.size};}
  if(g>=0)s.gpr[g]=f;else {auto v=vector(dest.reg);if(v>=0)s.xmm[v]=f;}
 }
 if(group(p,CS_GRP_CALL)){for(unsigned n:{0u,1u,2u,8u,9u,10u,11u})s.gpr[n]={};s.xmm={};}
}
Result analyze(const unsigned char* writer,std::size_t ws,const unsigned char* caller,std::size_t cs){
 Result r{};if(ws!=584||cs!=168){r.reason="original_function_span_mismatch";return r;}
 Decode w,c;r.writerDecoded=w.run(writer,ws,r.writerBytes,r.failureOffset);if(!r.writerDecoded){r.reason="writer_complete_decode_failed";return r;}
 r.callerDecoded=c.run(caller,cs,r.callerBytes,r.failureOffset);if(!r.callerDecoded){r.reason="caller_complete_decode_failed";return r;}
 State state{};state.gpr[1]={Target,0,8};state.gpr[2]={Source,0,8};state.gpr[4]={Stack,0,8};bool conditional=false;
 for(std::size_t i=0;i<w.count;++i){auto& p=w.ins[i];auto& x=p.detail->x86;
  bool jump=group(p,CS_GRP_JUMP),call=group(p,CS_GRP_CALL);
  if(jump){++r.branches;conditional=true;}if(call){++r.calls;conditional=true;}
  if(jump||call){bool direct=x.op_count==1&&x.operands[0].type==X86_OP_IMM;Control item{unsigned(p.address),direct?0x2cfc580+x.operands[0].imm:-1,call,direct};if(r.controlCount<r.controls.size())r.controls[r.controlCount++]=item;else ++r.suppressedControls;}
  for(unsigned j=0;j<x.op_count;++j){auto& op=x.operands[j];auto a=memory(op,state);if((a.root!=Source&&a.root!=Target)||a.off>=64||a.off+op.size<=48)continue;
   bool store=(op.access&CS_AC_WRITE)||(j==0&&move(p.id));Fact f{};if(store&&j==0&&x.op_count==2)f=value(x.operands[1],state);
   bool direct=move(p.id)&&f.root==Loaded&&f.bytes>=op.size;
   if(a.root==Source&&!store)++r.sourceReads;
   if(a.root==Target&&store)++r.targetStores;
   if(a.root==Target&&store&&direct&&f.off==a.off)for(unsigned k=0;k<4;++k)if(a.off<=48+4*k&&a.off+op.size>=52+4*k)r.directMask|=1u<<k;
   Access item{unsigned(p.address),op.size,a.off,direct?f.off:-1,store,a.root==Target,direct,conditional};
   if(r.accessCount<r.accesses.size())r.accesses[r.accessCount++]=item;else ++r.suppressed;
  }
  step(w,p,state);
 }
 state={};state.gpr[4]={Stack,0,8};
 for(std::size_t i=0;i<c.count;++i){auto& p=c.ins[i];
  if(0x2cfc850+p.address+p.size==0x2cfc8f3){std::int32_t delta{};if(p.size==5&&caller[p.address]==0xe8){std::memcpy(&delta,caller+p.address+1,4);r.callVerified=std::int64_t(0x2cfc8f3)+delta==0x2cfc580;}
   auto sp=state.gpr[4],source=state.gpr[2];r.stackRelation=r.callVerified&&sp.root==Stack&&source.root==Stack&&source.off==sp.off+0x20;break;
  }
  if(group(p,CS_GRP_CALL)||group(p,CS_GRP_JUMP)||group(p,CS_GRP_RET))break;
  step(c,p,state);
 }
 // Linear aliases/copy candidates are NOT a control-flow or downstream side-
 // effect proof. Even a simple16-byte copy does not prove Position W semantics.
 r.reason=!r.callVerified?"exact_direct_caller_dataflow_unproven":!r.stackRelation?"caller_stack_relation_unproven":r.branches||r.calls?"position_control_flow_and_downstream_effects_unproven":r.directMask!=15?"position_copy_map_incomplete_or_computed":"position_W_semantics_and_exclusive_effects_unproven";
 return r;
}
}
Result inspect(const unsigned char* w,std::size_t ws,const unsigned char* c,std::size_t cs) noexcept {try{return analyze(w,ws,c,cs);}catch(...){Result r{};r.reason="position_inspection_internal_failure";return r;}}
}
namespace ws::position_proof {
Causal causal(const unsigned char* bytes,std::size_t n,const unsigned char* caller,std::size_t cn) noexcept {
 Causal r{};try{
  auto base=inspect(bytes,n,caller,cn);if(!base.writerDecoded||!base.callerDecoded||!base.callVerified||!base.stackRelation){r.reason=base.reason;return r;}
  Decode d;unsigned failure{};r.complete=d.run(bytes,n,r.decoded,failure);if(!r.complete){r.reason="complete_decode_failed";return r;}
  bool loaded=false,stored=false,returned=false;unsigned vectorReg{};
  for(std::size_t i=0;i<d.count;++i){auto& p=d.ins[i];auto& x=p.detail->x86;if(p.id==X86_INS_NOP)continue;
   bool load=!returned&&!loaded&&p.id==X86_INS_MOVUPS&&x.op_count==2&&x.operands[0].type==X86_OP_REG&&x.operands[0].size==16&&x.operands[1].type==X86_OP_MEM&&x.operands[1].size==16&&x.operands[1].mem.base==X86_REG_RDX&&x.operands[1].mem.index==X86_REG_INVALID&&x.operands[1].mem.segment==X86_REG_INVALID&&x.operands[1].mem.disp==48;
   if(load){loaded=true;vectorReg=x.operands[0].reg;continue;}
   bool store=!returned&&loaded&&!stored&&p.id==X86_INS_MOVUPS&&x.op_count==2&&x.operands[0].type==X86_OP_MEM&&x.operands[0].size==16&&x.operands[0].mem.base==X86_REG_RCX&&x.operands[0].mem.index==X86_REG_INVALID&&x.operands[0].mem.segment==X86_REG_INVALID&&x.operands[0].mem.disp==48&&x.operands[1].type==X86_OP_REG&&x.operands[1].reg==vectorReg;
   if(store){stored=true;continue;}
   if(!returned&&stored&&p.id==X86_INS_RET&&x.op_count==0){returned=true;continue;}
   r.firstUnsupported=unsigned(p.address);r.reason="whole_function_effect_or_control_not_certified";return r;
  }
  r.directRowOnly=loaded&&stored&&returned;r.reason=r.directRowOnly?"complete_direct_Vec4_copy_only_certificate":"incomplete_direct_row";
 }catch(...){r.reason="causal_inspection_failed";}return r;
}
Store positionStore(const unsigned char* bytes,std::size_t n,unsigned afterOffset) noexcept {
 Store r{};try{Decode d;unsigned decoded{},failure{};r.decoded=n<=4096&&d.run(bytes,n,decoded,failure);if(!r.decoded)return r;
 for(std::size_t i=0;i<d.count;++i){auto& p=d.ins[i];auto& x=p.detail->x86;if(group(p,CS_GRP_CALL))++r.calls;if(group(p,CS_GRP_JUMP))++r.branches;
  if(p.address+p.size!=afterOffset)continue;
  for(unsigned j=0;j<x.op_count;++j){auto& op=x.operands[j];if(op.type!=X86_OP_MEM||op.mem.index!=X86_REG_INVALID||op.mem.segment!=X86_REG_INVALID||!((op.access&CS_AC_WRITE)||(j==0&&move(p.id))))continue;
   auto g=reg(op.mem.base);if(g<0||op.mem.base==X86_REG_RIP)continue;
   r.matched=true;r.instruction=unsigned(p.address);r.width=op.size;r.displacement=op.mem.disp;r.baseRegister=unsigned(g);if(x.op_count==2&&x.operands[1].type==X86_OP_REG)r.sourceRegister=x.operands[1].reg;break;
  }
 }
 }catch(...){r.decoded=false;}return r;
}
}
