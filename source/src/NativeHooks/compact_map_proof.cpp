#include <algorithm>
#include "NativeHooks/compact_map_proof.hpp"
extern "C" {
#include <hde64.h>
}
#include <cstring>
namespace ws::compact_proof {
namespace {struct Fact {unsigned kind{},offset{};};constexpr unsigned source=1,target=2,value=3;}
Result writer(const unsigned char* code,std::size_t size) noexcept {
 Result r{};r.reason="instruction_or_dataflow_not_proven";if(!code||size<16||size>1024)return r;
 Fact regs[16]{};regs[1]={target,0};regs[2]={source,0};bool seen[9]{};
 for(unsigned at=0;at<size;){
  hde64s h{};unsigned char bounded[16]{};auto available=std::min<std::size_t>(16,size-at);std::memcpy(bounded,code+at,available);
  unsigned len=hde64_disasm(bounded,&h);if(!len||len>available||(h.flags&F_ERROR))return r;
  ++r.decodedInstructions;r.decodedBytes=at+len;
  unsigned op=h.opcode,rg=h.modrm_reg+(h.rex_r?8:0),rm=h.modrm_rm+(h.rex_b?8:0);
  auto memory=[&](Fact& f)->bool {
   if(h.modrm_mod==3||h.p_67||h.p_seg)return false;
   unsigned base=rm;int displacement=0;
   if(h.modrm_rm==4){if(h.sib_index!=4||h.rex_x)return false;base=h.sib_base+(h.rex_b?8:0);if(h.modrm_mod==0&&h.sib_base==5)return false;}
   if(h.modrm_mod==0&&h.modrm_rm==5)return false;
   if(h.modrm_mod==1)displacement=static_cast<std::int8_t>(h.disp.disp8);
   else if(h.modrm_mod==2)displacement=static_cast<std::int32_t>(h.disp.disp32);
   f=regs[base];if(!f.kind||displacement<0)return false;f.offset+=unsigned(displacement);return true;
  };
  if((op==0x89||op==0x8b)&&!h.p_66&&!h.p_rep){
   if(h.modrm_mod==3){unsigned dest=op==0x8b?rg:rm,src=op==0x8b?rm:rg;regs[dest]=h.rex_w?regs[src]:regs[src].kind==value?regs[src]:Fact{};}
   else {Fact m{};if(!memory(m))return r;
    if(op==0x8b){regs[rg]=!h.rex_w&&m.kind==source&&m.offset<=32&&m.offset%4==0?Fact{value,m.offset}:Fact{};}
    else if(m.kind==target){
     auto v=regs[rg];if(h.rex_w||v.kind!=value){at+=len;continue;}if(v.offset>32||v.offset%4)return r;
     auto index=v.offset/4;unsigned dest=(index/3)*16+(index%3)*4;
     if(m.offset!=dest||seen[index])return r;
     seen[index]=true;r.map[index]={at,v.offset,m.offset};++r.pairs;
    }
   }
  }else {
   // Unknown instructions cannot establish a pair. Conservatively invalidate
   // their explicit ModRM destination without interpreting game semantics.
   if(h.flags&F_MODRM){
    bool registerDestination=(op==0x8d||op==0x8b||op==0x03||op==0x23||op==0x33||op==0x0f);
    if(registerDestination)regs[rg]={};
    else if(h.modrm_mod==3)regs[rm]={};
   }
  }
  at+=len;
 }
 r.admitted=r.pairs==9;
 if(r.admitted)r.reason="nine_anchored_direct_copies_full_function_decoded";
 return r;
}
bool caller(const unsigned char* code,std::size_t size,std::uintptr_t begin,std::uintptr_t targetRva,std::uintptr_t returnRva) noexcept {
 if(!code||size!=128||returnRva<=begin||returnRva>begin+size)return false;
 bool found=false;
 for(unsigned at=0;at<size;){unsigned char bounded[16]{};auto n=std::min<std::size_t>(16,size-at);std::memcpy(bounded,code+at,n);hde64s h{};unsigned len=hde64_disasm(bounded,&h);
  if(!len||len>n||(h.flags&F_ERROR))return false;
  if(begin+at+len==returnRva){if(h.opcode!=0xe8||len!=5)return false;std::int32_t displacement{};std::memcpy(&displacement,code+at+1,4);if(std::int64_t(returnRva)+displacement!=std::int64_t(targetRva))return false;found=true;}
  at+=len;
 }
 return found;
}
}
