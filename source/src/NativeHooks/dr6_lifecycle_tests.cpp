// Synthetic lifecycle and NECESSARY-condition audit only; never native evidence/certificate.
#include "NativeHooks/dr6_lifecycle.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
namespace l=ws::dr6_lifecycle;namespace d=ws::continuation_diagnostics;namespace c=ws::debug_register_comparison;
l::Timeline fixture(unsigned first=l::index(l::Phase::DbEntry)) {
  l::Timeline t;t.worker=42;
  for(unsigned i=0;i<l::count;++i) {
    d::Registers r{0,0,i?0x140000000ull:0,0,0,i?0x10ull:0};
    if(i>=first)r[4]=0xffff0ff0;
    if(i>=l::index(l::Phase::WatchSet)&&i<l::index(l::Phase::AfterPoll)){r[0]=0x1000;r[1]=0x1008;r[5]=0x990415;}
    if(i==l::index(l::Phase::DbEntry))r[4]|=1;
    if(i>=l::index(l::Phase::AfterPoll))r[5]|=0x400;
    t.stages[i]=l::Observation{r,i+1,42,!l::exceptionPhase(i)};
  }
  return t;
}
l::Facts facts(const l::Timeline& t) {
  l::Facts f;f.vendor=c::Vendor::Intel;f.mitigationAdmitted=f.ordinaryOwnedDataDb=f.engineClaimed=f.canRetire=true;
  f.activeAfter=f.pollReturned=false;f.pendingBefore=f.pendingAfter=0;
  f.saved=t.stages[l::index(l::Phase::ForeignResumeSaved)]->registers;return f;
}
int main(int argc,char** argv) {
  if(argc==2&&!std::strcmp(argv[1],"--json-fixture")) {
    auto t=fixture();auto f=facts(t);auto multiple=t;multiple.stages[l::index(l::Phase::DbAfterEngine)]->registers[4]&=~0x10800ull;
    std::printf("{\"synthetic_lifecycle_fixture\":true,\"unique\":%s,\"multiple\":%s}\n",l::json(t,f).c_str(),l::json(multiple,f).c_str());return 0;
  }
  assert(argc==1);
  for(auto phase:{l::Phase::ForeignSet,l::Phase::WatchSet})for(unsigned bit:{11u,16u}) {
    auto t=fixture(l::index(phase));assert(!std::strcmp(l::classify(t,bit).classification,"SETTHREADCONTEXT_ROUNDTRIP"));
  }
  std::puts("PASS: c45 synthetic foreign/watch SetThreadContext-before-Resume boundaries classify distinctly");
  for(unsigned bit:{11u,16u}){auto r=l::classify(fixture(),bit);assert(r.unique&&r.changes==1&&r.first==int(l::index(l::Phase::DbEntry))&&!std::strcmp(r.classification,"HARDWARE_DB_ENTRY"));}
  std::puts("PASS: c45 synthetic ordinary owned DB-entry first transition preserves exact stage and direction");
  for(auto pair:{std::pair{l::Phase::DbAfterEngine,"HANDLER_EDIT"},std::pair{l::Phase::PostReturn,"OS_RETURN_POST_CONTEXT"}}) {
    auto t=fixture(l::index(pair.first));for(unsigned bit:{11u,16u})assert(!std::strcmp(l::classify(t,bit).classification,pair.second));
  }
  auto multiple=fixture();multiple.stages[l::index(l::Phase::DbAfterEngine)]->registers[4]&=~0x10800ull;
  for(unsigned bit:{11u,16u})assert(!l::classify(multiple,bit).unique&&l::classify(multiple,bit).changes==3);
  assert(!l::audit(multiple,facts(multiple)).necessaryConditions);
  std::puts("PASS: c45 isolated handler/OS-return boundaries and entry-restored-old-returned-new multiple transitions never collapse into a certificate");
  for(unsigned mode=0;mode<5;++mode) {
    auto t=fixture();auto f=facts(t);
    if(mode==0)t.stages[2].reset();
    if(mode==1)t.stages[3]->sequence=1;
    if(mode==2)t.stages[4]->thread=43;
    if(mode==3)t.stages[4]->suspendedApi=true;
    if(mode==4)t.observerError=true;
    assert(!l::completeOrdered(t)&&!l::audit(t,f).necessaryConditions);
  }
  auto resumed=fixture(l::index(l::Phase::ForeignResumeSaved));assert(!l::classify(resumed,11).unique);
  std::puts("PASS: c45 incomplete, duplicate/error, reordered, foreign-thread, wrong-origin and unisolated Resume-view evidence refuses");
  for(unsigned mode=0;mode<5;++mode) {
    auto t=fixture();auto f=facts(t);
    if(mode==0)f.vendor=c::Vendor::AMD;
    if(mode==1)f.mitigationAdmitted=false;
    if(mode==2)f.ordinaryOwnedDataDb=false;
    if(mode==3)f.engineClaimed=false;
    if(mode==4)t.stages[3]->registers[5]|=0x800;
    assert(!l::audit(t,f).necessaryConditions);
  }
  std::puts("PASS: c45 only Intel, admitted mitigation, real owned data-DB/engine claim and disabled Dr7.RTM can satisfy necessary conditions");
  for(unsigned bit:{11u,16u}) {
    auto t=fixture();auto f=facts(t);for(unsigned i=l::index(l::Phase::AfterPoll);i<l::count;++i)t.stages[i]->registers[4]&=~(std::uint64_t(1)<<bit);
    assert(!l::audit(t,f).necessaryConditions);
  }
  std::puts("PASS: c45 wrong BLD11 or RTM16 post-event status refuses without blanket masking");
  for(unsigned bit:{0u,1u,2u,3u,13u,14u,15u}) {
    auto t=fixture();auto f=facts(t);for(unsigned i=l::index(l::Phase::AfterPoll);i<l::count;++i)t.stages[i]->registers[4]|=std::uint64_t(1)<<bit;
    assert(!l::audit(t,f).necessaryConditions);
  }
  std::puts("PASS: c45 owned B0/B1 residue and foreign B2/B3/BD/BS/BT causes all refuse");
  for(unsigned reg=0;reg<4;++reg) {
    auto t=fixture();auto f=facts(t);for(unsigned i=l::index(l::Phase::AfterPoll);i<l::count;++i)t.stages[i]->registers[reg]^=1;
    assert(!l::audit(t,f).necessaryConditions);
  }
  for(unsigned bit:{0u,1u,4u,5u,10u,12u,13u,16u,18u,63u}) {
    auto t=fixture();auto f=facts(t);for(unsigned i=l::index(l::Phase::AfterPoll);i<l::count;++i)t.stages[i]->registers[5]^=std::uint64_t(1)<<bit;
    assert(!l::audit(t,f).necessaryConditions);
  }
  std::puts("PASS: c45 Dr0-Dr3/foreign Dr2 and Dr7 meaningful/global/fixed/unknown fields remain protected");
  for(unsigned mode=0;mode<7;++mode) {
    auto t=fixture();auto f=facts(t);
    if(mode==0)f.canRetire=false;
    if(mode==1)f.activeAfter=true;
    if(mode==2)f.pollReturned=true;
    if(mode==3)f.pendingBefore=1;
    if(mode==4)f.pendingAfter=1;
    if(mode==5)t.stages[l::count-1].reset();
    if(mode==6)t.stages[l::count-1]->registers[0]=1;
    assert(!l::audit(t,f).necessaryConditions);
  }
  std::puts("PASS: c45 lease, pending debt, canRetire, poll and incomplete/unstable three-readback conditions refuse");
  auto t=fixture();assert(l::completeOrdered(t)&&l::audit(t,facts(t)).necessaryConditions);
  assert(l::json(t,facts(t)).find("\"RETIREMENT_STATUS_SAFE_added\":false")!=std::string::npos);
  assert(!l::classify(t,10).unique);
  std::puts("PASS: c45 all synthetic necessary conditions can hold while RETIREMENT_STATUS_SAFE remains explicitly unimplemented");
}
