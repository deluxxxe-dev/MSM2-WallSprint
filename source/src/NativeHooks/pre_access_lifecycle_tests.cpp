// Synthetic pre-access boundary models; never native evidence or acceptance.
#include "NativeHooks/pre_access_lifecycle.hpp"
#include <cassert>
#include <cstdio>
namespace p=ws::pre_access_lifecycle;namespace l=ws::dr6_lifecycle;namespace c=ws::debug_register_comparison;
p::Evidence fixture(unsigned first=5) {
 p::Evidence e;e.prior.worker=42;e.expectedLease=1;e.expectedGeneration=101;
 auto& g=e.gate;g.reached=g.capturedSuspended=g.capturedBeforeRelease=g.releaseUnsignaled=g.resumedAfterCapture=g.activeLease=g.ownedSlots=true;
 g.arrivalThread=42;g.lease=1;g.generation=101;g.dbAtArrival=g.dbBeforeSuspend=g.dbAtCapture=g.dbAfterRead=g.machineEntriesAtCapture=g.ticketsAtCapture=g.pendingAtCapture=0;g.writesAtWatch=g.writesAtCapture=1;
 for(unsigned i=0;i<p::count;++i){
  ws::continuation_diagnostics::Registers r{0,0,i?0x140000000ull:0,0,i>=first?0xffff0ff0ull:0,i?0x10ull:0};
  if(i>=3&&i<8){r[0]=0x1000;r[1]=0x1008;r[5]=0x990415;}
  if(i==5)r[4]|=1;
  if(i>=8)r[5]|=0x400;
  auto s=l::Observation{r,i+1,42,i!=5&&i!=6};
  if(i==4)e.preAccess=s;else e.prior.stages[i-(i>4)]=s;
 }
 return e;
}
l::Facts facts(const p::Evidence& e){l::Facts f;f.vendor=c::Vendor::Intel;f.mitigationAdmitted=f.ordinaryOwnedDataDb=f.engineClaimed=f.canRetire=true;f.activeAfter=f.pollReturned=false;f.pendingBefore=f.pendingAfter=0;f.saved=e.prior.stages[2]->registers;return f;}
int main(int argc,char** argv){
 if(argc==2&&!std::strcmp(argv[1],"--json-fixture")){auto a=fixture(4),b=fixture(5);std::printf("{\"synthetic_pre_access_fixture\":true,\"pre_access\":%s,\"delivery\":%s}\n",p::json(a,facts(a)).c_str(),p::json(b,facts(b)).c_str());return 0;}
 assert(argc==1);
 auto e=fixture();assert(p::gateProven(e)&&p::completeOrdered(e));
 for(unsigned bit:{11u,16u}){
  const auto a=p::classify(fixture(4),bit),b=p::classify(fixture(5),bit);
  assert(a.unique&&!std::strcmp(a.classification,"WATCH_ENABLED_RESUME_PRE_ACCESS")&&a.first==4);
  assert(b.unique&&!std::strcmp(b.classification,"DEBUG_EXCEPTION_DELIVERY_BOUNDARY")&&b.first==5);
 }
 std::puts("PASS: c46 zero-DB pre-access gate separates watch-enabled Resume from exception-delivery first observation");
 for(unsigned i:{1u,3u,6u,7u})for(unsigned bit:{11u,16u}){auto t=fixture(i);auto r=p::classify(t,bit);assert(r.unique&&!std::strcmp(r.classification,p::boundary(i)));}
 std::puts("PASS: c46 preserves pre-Resume Set, handler-only and post-return-only classifications");
 for(unsigned mode=0;mode<12;++mode){auto t=fixture();auto& g=t.gate;
  if(mode==0)g.reached=false;
  if(mode==1)g.capturedSuspended=false;
  if(mode==2)g.capturedBeforeRelease=false;
  if(mode==3)g.releaseUnsignaled=false;
  if(mode==4)g.resumedAfterCapture=false;
  if(mode==5)g.arrivalThread=43;
  if(mode==6)g.lease=2;
  if(mode==7)g.generation=102;
  if(mode==8)g.activeLease=false;
  if(mode==9)g.ownedSlots=false;
  if(mode==10)g.machineEntriesAtCapture=1;
  if(mode==11)g.ticketsAtCapture=1;
  assert(!p::gateProven(t)&&!p::classify(t,11).unique&&!p::audit(t,facts(t)).necessaryConditions);
 }
 std::puts("PASS: c46 ambiguous gate/access ordering, early machine/ticket and wrong thread/lease/generation/live-watch proof refuse");
 for(unsigned mode=0;mode<6;++mode){auto t=fixture();
  if(mode==0)t.gate.dbAtArrival=1;
  if(mode==1)t.gate.dbBeforeSuspend=1;
  if(mode==2)t.gate.dbAtCapture=1;
  if(mode==3)t.gate.dbAfterRead=1;
  if(mode==4)t.gate.dbAtCapture.reset();
  if(mode==5)t.gate.pendingAtCapture=1;
  assert(!p::gateProven(t)&&!p::classify(t,16).unique);
 }
 std::puts("PASS: c46 early DB at arrival/before suspend/during read, unknown counts and early pending debt refuse");
 for(unsigned mode=0;mode<8;++mode){auto t=fixture();
  if(mode==0)t.preAccess.reset();
  if(mode==1)t.preAccess->sequence=3;
  if(mode==2)t.preAccess->sequence=6;
  if(mode==3)t.preAccess->suspendedApi=false;
  if(mode==4)t.prior.stages[10].reset();
  if(mode==5)t.prior.observerError=true;
  if(mode==6)t.gate.writesAtCapture=2;
  if(mode==7)t.gate.writesAtWatch.reset();
  assert(!p::completeOrdered(t)&&!p::classify(t,11).unique);
 }
 std::puts("PASS: c46 missing/reordered/wrong-origin observations, unknown or extra context write and partial readbacks refuse");
 auto multiple=fixture();multiple.prior.stages[5]->registers[4]&=~0x10800ull;
 for(unsigned bit:{11u,16u}){auto r=p::classify(multiple,bit);assert(!r.unique&&r.changes==3&&!std::strcmp(r.firstBoundary,"DEBUG_EXCEPTION_DELIVERY_BOUNDARY"));}
 assert(!p::audit(multiple,facts(multiple)).necessaryConditions);
 std::puts("PASS: c46 multiple status transitions keep earliest observed boundary but never become unique acceptance");
 for(unsigned bit:{0u,1u,2u,3u,12u,13u,14u,15u,63u}){auto t=fixture();t.preAccess->registers[4]|=std::uint64_t(1)<<bit;assert(!p::gateProven(t));}
 for(unsigned bit:{0u,1u,2u,3u,13u,14u,15u}){auto t=fixture();auto f=facts(t);for(unsigned i=7;i<l::count;++i)t.prior.stages[i]->registers[4]|=std::uint64_t(1)<<bit;assert(!p::audit(t,f).necessaryConditions);}
 std::puts("PASS: c46 pre-access and final owned B0/B1, foreign B2/B3/BD/BS/BT, unknown/high DR6 bits refuse");
 for(unsigned reg=0;reg<4;++reg){auto t=fixture();t.preAccess->registers[reg]^=1;assert(!p::gateProven(t));t=fixture();auto f=facts(t);for(unsigned i=7;i<l::count;++i)t.prior.stages[i]->registers[reg]^=1;assert(!p::audit(t,f).necessaryConditions);}
 std::puts("PASS: c46 pre-access and final Dr0-Dr3 including synthetic foreign Dr2 remain exact");
 for(unsigned bit:{0u,1u,4u,5u,11u,12u,13u,16u,18u,63u}){auto t=fixture();t.preAccess->registers[5]^=std::uint64_t(1)<<bit;assert(!p::gateProven(t));t=fixture();auto f=facts(t);for(unsigned i=7;i<l::count;++i)t.prior.stages[i]->registers[5]^=std::uint64_t(1)<<bit;assert(!p::audit(t,f).necessaryConditions);}
 std::puts("PASS: c46 Dr7 meaningful/global/control/RTM/unknown fields stay conservative at gate and final readbacks");
 for(unsigned mode=0;mode<9;++mode){auto t=fixture();auto f=facts(t);
  if(mode==0)f.vendor=c::Vendor::Unknown;
  if(mode==1)f.mitigationAdmitted=false;
  if(mode==2)f.ordinaryOwnedDataDb=false;
  if(mode==3)f.engineClaimed=false;
  if(mode==4)f.canRetire=false;
  if(mode==5)f.activeAfter=true;
  if(mode==6)f.pollReturned=true;
  if(mode==7)f.pendingBefore=1;
  if(mode==8)f.pendingAfter=1;
  assert(!p::audit(t,f).necessaryConditions);
 }
 std::puts("PASS: c46 negative guard retains Intel/mitigation/DB/lease/pending/continuation and final-poll prerequisites");
 assert(!p::classify(e,10).unique);assert(p::json(e,facts(e)).find("\"RETIREMENT_CONFIGURATION_SAFE_added\":false")!=std::string::npos);
 std::puts("PASS: c46 unsupported status field refuses and no configuration-safe comparator or synthetic acceptance is added");
}
