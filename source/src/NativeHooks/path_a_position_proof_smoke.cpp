#include "NativeHooks/path_a_position_proof.hpp"
#include <array>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdint>
int main(){
 std::array<unsigned char,584> w;w.fill(0x90);std::array<unsigned char,168> c;c.fill(0x90);
 unsigned char copy[]={0x0f,0x10,0x42,0x30,0x0f,0x11,0x41,0x30,0xc3};std::memcpy(w.data(),copy,sizeof(copy));
 unsigned char stack[]={0x48,0x83,0xec,0x48,0x48,0x8d,0x54,0x24,0x20};std::memcpy(c.data(),stack,sizeof(stack));c[158]=0xe8;std::int32_t delta=0x2cfc580-0x2cfc8f3;std::memcpy(c.data()+159,&delta,4);c[163]=0x48;c[164]=0x83;c[165]=0xc4;c[166]=0x48;c[167]=0xc3;
 auto r=ws::position_proof::inspect(w.data(),w.size(),c.data(),c.size());assert(!r.admitted&&r.writerDecoded&&r.callerDecoded&&r.callVerified&&r.stackRelation&&r.directMask==15&&r.sourceReads==1&&r.targetStores==1);
 assert(!std::strcmp(r.reason,"position_W_semantics_and_exclusive_effects_unproven"));
 // Identity-shaped layout/copy candidates never turn into Position permission.
 auto changed=c;changed[159]^=1;r=ws::position_proof::inspect(w.data(),w.size(),changed.data(),changed.size());assert(!r.admitted&&!r.callVerified);
 changed=c;changed[8]=0x30;r=ws::position_proof::inspect(w.data(),w.size(),changed.data(),changed.size());assert(!r.admitted&&r.callVerified&&!r.stackRelation);
 auto computed=w;unsigned char arithmetic[]={0x0f,0x10,0x42,0x30,0x0f,0x58,0xc1,0x0f,0x11,0x41,0x30,0xc3};std::memcpy(computed.data(),arithmetic,sizeof(arithmetic));r=ws::position_proof::inspect(computed.data(),computed.size(),c.data(),c.size());assert(!r.admitted&&r.directMask==0&&r.targetStores==1);
 auto branch=w;unsigned char conditional[]={0x75,0x08,0x0f,0x10,0x42,0x30,0x0f,0x11,0x41,0x30,0xc3};std::memcpy(branch.data(),conditional,sizeof(conditional));r=ws::position_proof::inspect(branch.data(),branch.size(),c.data(),c.size());assert(!r.admitted&&r.branches==1&&r.accesses[0].afterControlFlow);
 auto bad=w;bad.back()=0xc4;r=ws::position_proof::inspect(bad.data(),bad.size(),c.data(),c.size());assert(!r.admitted&&!r.writerDecoded&&r.failureOffset==583);
 r=ws::position_proof::inspect(w.data(),583,c.data(),c.size());assert(!r.admitted&&!r.writerDecoded);
 auto many=w;many.fill(0x90);unsigned char aliases[]={0x48,0x89,0xcb,0x48,0x89,0xd6};std::memcpy(many.data(),aliases,sizeof(aliases));unsigned char row[]={0x0f,0x10,0x46,0x30,0x0f,0x11,0x43,0x30};for(unsigned i=0;i<60;++i)std::memcpy(many.data()+6+i*8,row,8);r=ws::position_proof::inspect(many.data(),many.size(),c.data(),c.size());assert(!r.admitted&&r.accessCount==24&&r.suppressed>0);
 many.fill(0x90);for(unsigned i=0;i<50;++i){many[2*i]=0x75;many[2*i+1]=0;}r=ws::position_proof::inspect(many.data(),many.size(),c.data(),c.size());assert(!r.admitted&&r.controlCount==16&&r.suppressedControls==34);
 std::puts("PASS: complete Capstone Position candidate inspection on owned584/168 fixtures; exact caller/stack relation, direct/computed/conditional/truncated/mutated negatives; copy/W/exclusive-effects gaps never admit Position writes; no game bytes");
}
