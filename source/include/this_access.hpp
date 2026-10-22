#pragma once
#include <array>
#include <vector>
#include <deque>
#include <map>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <climits>
#include "hde64.h"
namespace ws::native {
// Conservative one-level provenance, not a native C++ type/ABI reconstruction.
enum class Origin {Unknown,This,Child};
struct Alias {Origin kind=Origin::Unknown;int field{},delta{};bool operator==(const Alias& b)const{return kind==b.kind&&field==b.field&&delta==b.delta;}};
using State=std::array<Alias,16>;
struct Access {unsigned pc{};int field{},offset{};Origin base{};unsigned width{};const char* operation="UNKNOWN";bool pointerLoad{},indirectCall{};};
struct Result {std::vector<Access> accesses;std::vector<unsigned> directCalls;bool converged=true;unsigned decoded{},unknownInstructions{};};
struct Step {hde64s h{};State next{};std::vector<unsigned> edges;Access access{};bool hasAccess{},unknown{};};
inline void clobberVolatile(State& s){for(unsigned r:{0u,1u,2u,8u,9u,10u,11u})s[r]={};}
inline int displacement(const hde64s& h){return h.flags&F_DISP8?int(static_cast<std::int8_t>(h.disp.disp8)):h.flags&F_DISP32?static_cast<std::int32_t>(h.disp.disp32):0;}
inline int memoryBase(const hde64s& h){
 if(!(h.flags&F_MODRM)||h.modrm_mod==3||h.p_67||h.p_seg)return -1;
 if(h.modrm_rm==4){if(h.sib_index!=4||h.rex_x||(!h.modrm_mod&&h.sib_base==5))return -1;return h.sib_base+8*h.rex_b;}
 if(!h.modrm_mod&&h.modrm_rm==5)return -1;
 return h.modrm_rm+8*h.rex_b;
}
inline unsigned operandWidth(const hde64s& h){return h.rex_w?8:h.p_66?2:4;}
inline Step interpret(const std::vector<unsigned char>& code,unsigned pc,const State& in){
 Step out;out.next=in;unsigned char padded[16]{};auto remain=std::min(std::size_t(16),code.size()-pc);std::memcpy(padded,code.data()+pc,remain);hde64_disasm(padded,&out.h);
 auto& h=out.h;if(!h.len||(h.flags&F_ERROR)||h.len>code.size()-pc){out.unknown=true;return out;}
 auto op=h.opcode,op2=h.opcode2;unsigned reg=h.modrm_reg+8*h.rex_r,rm=h.modrm_rm+8*h.rex_b;int base=memoryBase(h);auto width=operandWidth(h);bool known=false;
 if(base>=0&&in[base].kind!=Origin::Unknown){auto a=in[base];long long sum=static_cast<long long>(a.delta)+displacement(h);if(sum>=INT_MIN&&sum<=INT_MAX){out.hasAccess=true;out.access={pc,a.field,int(sum),a.kind,width,"UNKNOWN",false,false};}}
 auto byteReg=[&](unsigned r){return !h.rex&&r>=4&&r<8?r-4:r;};
 auto setOp=[&](const char* name,unsigned bytes){if(out.hasAccess){out.access.operation=name;out.access.width=bytes;}};
 if(op==0x8b||op==0x8a){known=true;setOp("READ",op==0x8a?1:width);out.next[op==0x8a?byteReg(reg):reg]={};
  if(h.modrm_mod==3&&op==0x8b&&h.rex_w)out.next[reg]=in[rm];
  else if(op==0x8b&&h.rex_w&&out.hasAccess&&out.access.base==Origin::This){out.next[reg]={Origin::Child,out.access.offset,0};out.access.pointerLoad=true;}
 }else if(op==0x89||op==0x88){known=true;setOp("WRITE",op==0x88?1:width);if(h.modrm_mod==3)out.next[op==0x88?byteReg(rm):rm]=op==0x89&&h.rex_w?in[reg]:Alias{};
 }else if(op==0x8d){known=true;out.hasAccess=false;out.next[reg]={};if(h.rex_w&&base>=0&&in[base].kind!=Origin::Unknown){out.next[reg]=in[base];long long sum=static_cast<long long>(out.next[reg].delta)+displacement(h);if(sum<INT_MIN||sum>INT_MAX)out.next[reg]={};else out.next[reg].delta=int(sum);}
 }else if(op==0xc6||op==0xc7){known=true;setOp("WRITE",op==0xc6?1:width);if(h.modrm_mod==3)out.next[op==0xc6?byteReg(rm):rm]={};
 }else if(op==0x39||op==0x3b||op==0x38||op==0x3a||op==0x85||op==0x84){known=true;setOp("READ",op==0x38||op==0x3a||op==0x84?1:width);
 }else if(op==0x81||op==0x83||op==0x80){known=true;setOp(h.modrm_reg==7?"READ":"READ_WRITE",op==0x80?1:width);if(h.modrm_mod==3&&h.modrm_reg!=7)out.next[op==0x80?byteReg(rm):rm]={};
 }else if(op<=0x33&&(op&7)<=3){known=true;bool regDestination=(op&2)!=0;setOp(regDestination?"READ":"READ_WRITE",(op&1)?width:1);if(regDestination)out.next[(op&1)?reg:byteReg(reg)]={};else if(h.modrm_mod==3)out.next[(op&1)?rm:byteReg(rm)]={};
 }else if(op>=0xb0&&op<=0xbf){known=true;out.next[op<=0xb7?byteReg((op&7)+8*h.rex_b):(op&7)+8*h.rex_b]={};
 }else if(op>=0x50&&op<=0x57){known=true;out.next[4]={};
 }else if(op>=0x58&&op<=0x5f){known=true;out.next[(op&7)+8*h.rex_b]={};out.next[4]={};
 }else if(op==0x90||op==0x9c||op==0x9d){known=true;if(op==0x90&&h.rex_b){out.next[0]={};out.next[8]={};}
 }else if(op==0xff){setOp(h.modrm_reg==0||h.modrm_reg==1?"READ_WRITE":"READ",h.modrm_reg==2||h.modrm_reg==4?8:width);known=true;out.access.indirectCall=h.modrm_reg==2;if(h.modrm_reg==2)clobberVolatile(out.next);else if(h.modrm_reg==6)out.next[4]={};else if(h.modrm_mod==3&&(h.modrm_reg==0||h.modrm_reg==1))out.next[rm]={};else if(h.modrm_reg!=4&&h.modrm_reg!=6)out.next={};
 }else if(op==0xe8){known=true;clobberVolatile(out.next);
 }else if(op==0xe9||op==0xeb||(op>=0x70&&op<=0x7f)||op==0xc3||op==0xc2||op==0xcc){known=true;
 }else if(op==0x0f){
  if(op2>=0x80&&op2<=0x8f){known=true;}
  else if(op2==0xb6||op2==0xb7||op2==0xbe||op2==0xbf){known=true;setOp("READ",op2==0xb6||op2==0xbe?1:2);out.next[reg]={};}
  else if(op2==0x1f){known=true;out.hasAccess=false;}
  else if((op2>=0x10&&op2<=0x17)||(op2>=0x28&&op2<=0x2f)||(op2>=0x50&&op2<=0x76)||op2==0x7e||op2==0x7f||op2==0xc2||op2==0xc6||op2>=0xd1){
   // Only known SSE families that cannot modify GPRs are retained. Others kill provenance.
   bool store=op2==0x11||op2==0x29||op2==0x7f;bool gp=op2==0x2c||op2==0x2d||op2==0x50||op2==0x7e||op2==0xc4||op2==0xc5||op2==0xd7;
   if(!gp){
    known=true;unsigned memoryWidth=16;store=store||op2==0x13||op2==0x17||op2==0x2b||op2==0xd6||op2==0xe7;
    if(op2==0x10||op2==0x11||op2==0x51||op2==0x52||op2==0x53||(op2>=0x58&&op2<=0x5f))memoryWidth=h.p_rep==0xf3?4:h.p_rep==0xf2?8:16;
    if(op2==0x12||op2==0x16)memoryWidth=h.p_rep==0xf3?16:8;
    if(op2==0x13||op2==0x17||op2==0xd6)memoryWidth=8;
    if(op2==0x2a)memoryWidth=h.p_rep?operandWidth(h):8;
    if(op2==0x2e||op2==0x2f)memoryWidth=h.p_66?8:4;
    if(op2==0x5a)memoryWidth=h.p_rep==0xf3?4:h.p_rep==0xf2?8:h.p_66?16:8;
    if(op2==0x5b)memoryWidth=16;
    if((op2>=0x60&&op2<=0x76)||op2==0x7f||op2>=0xd1)memoryWidth=h.p_66||h.p_rep?16:8;
    if(op2==0x6e)memoryWidth=operandWidth(h);
    if(op2==0xd6)memoryWidth=8;
    setOp(store?"WRITE":"READ",memoryWidth);
   }
  }
 }
 if(!known){out.next={};out.unknown=true;if(out.hasAccess){out.access.width=0;out.access.operation="UNKNOWN";}}
 auto add=[&](long long target){if(target>=0&&target<static_cast<long long>(code.size()))out.edges.push_back(unsigned(target));};
 if(op==0xc3||op==0xc2||op==0xcc||(op==0xff&&(h.modrm_reg==4||h.modrm_reg==5)))return out;
 if(op==0xe9||op==0xeb||(op>=0x70&&op<=0x7f)||(op==0x0f&&op2>=0x80&&op2<=0x8f)){
  long long rel=(h.flags&F_IMM8)?static_cast<std::int8_t>(h.imm.imm8):static_cast<std::int32_t>(h.imm.imm32);add(static_cast<long long>(pc)+h.len+rel);if(op==0xe9||op==0xeb)return out;
 }
 add(static_cast<long long>(pc)+h.len);return out;
}
inline Result analyze(const std::vector<unsigned char>& code){
 Result result;if(code.empty()||code.size()>0x4000){result.converged=false;return result;}
 std::vector<State> states(code.size());std::vector<bool> seen(code.size());std::deque<unsigned> queue;states[0][1]={Origin::This,0,0};seen[0]=true;queue.push_back(0);unsigned visits=0;
 while(!queue.empty()){
  if(++visits>131072){result.converged=false;return result;}auto pc=queue.front();queue.pop_front();auto step=interpret(code,pc,states[pc]);
  for(auto edge:step.edges){if(!seen[edge]){states[edge]=step.next;seen[edge]=true;queue.push_back(edge);}else {bool changed=false;for(unsigned r=0;r<16;++r)if(!(states[edge][r]==step.next[r])&&states[edge][r].kind!=Origin::Unknown){states[edge][r]={};changed=true;}if(changed)queue.push_back(edge);}}
 }
 // Reject overlapping instruction entries; never interpret a branch into an instruction middle.
 std::vector<int> owner(code.size(),-1);
 for(unsigned pc=0;pc<code.size();++pc)if(seen[pc]){auto step=interpret(code,pc,states[pc]);if(!step.h.len||(step.h.flags&F_ERROR)||step.h.len>code.size()-pc)continue;
  for(unsigned i=pc;i<pc+step.h.len;++i){if(owner[i]>=0&&owner[i]!=int(pc)){result.converged=false;result.accesses.clear();return result;}owner[i]=int(pc);}}
 for(unsigned pc=0;pc<code.size();++pc)if(seen[pc]){auto step=interpret(code,pc,states[pc]);++result.decoded;if(step.unknown)++result.unknownInstructions;
  if(step.hasAccess){result.accesses.push_back(step.access);}
  if(step.h.opcode==0xe8)result.directCalls.push_back(pc);
 }
 return result;
}
}
