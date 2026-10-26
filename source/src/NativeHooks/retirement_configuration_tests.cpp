// SYNTHETIC models only. No native execution, vendor or mitigation query is forged.
#include "NativeHooks/retirement_configuration.hpp"
#include <cassert>
#include <cstdio>
namespace q=ws::retirement_configuration;namespace p=ws::pre_access_lifecycle;namespace l=ws::dr6_lifecycle;namespace c=ws::debug_register_comparison;
q::Inputs fixture(){
 q::Inputs f;f.profile=q::Profile::OwnerC46IntelPositionMultipleV1;f.vendor=c::Vendor::Intel;
 f.nativeWindows=f.prerequisitesKnown=f.cpuidKnown=f.leaf7Known=f.debuggerAbsent=f.positionMultiple=true;f.signature=1;f.poisoned=false;
 f.mitigations={{{true,0},{true,0},{true,256}}};
 auto& e=f.lifecycle;e.prior.worker=42;e.expectedLease=1;e.expectedGeneration=101;
 auto& g=e.gate;g.reached=g.capturedSuspended=g.capturedBeforeRelease=g.releaseUnsignaled=g.resumedAfterCapture=g.activeLease=g.ownedSlots=true;
 g.arrivalThread=42;g.lease=1;g.generation=101;g.dbAtArrival=g.dbBeforeSuspend=g.dbAtCapture=g.dbAfterRead=g.machineEntriesAtCapture=g.ticketsAtCapture=g.pendingAtCapture=0;g.writesAtWatch=g.writesAtCapture=1;
 for(unsigned i=0;i<p::count;++i){
  ws::continuation_diagnostics::Registers r{0,0,i?0x140000000ull:0,0,i<4?0:i==5?0xffff0ff1ull:0xffff0ff0ull,i?0x10ull:0};
  if(i>=3&&i<=7){r[0]=0x1000;r[1]=0x1008;r[5]=i==3?0x990015:0x990415;}
  if(i>=8)r[5]|=0x400;
  auto s=l::Observation{r,i+1,42,i!=5&&i!=6};if(i==4)e.preAccess=s;else e.prior.stages[i-(i>4)]=s;
 }
 auto& r=f.restoration;r.syntheticInputs=true;r.attempted=true;r.activeBefore=true;r.activeAfter=r.pollReturned=false;r.canRetireBefore=true;r.pendingBefore=r.pendingAfter=0;
 r.saved=e.prior.stages[2]->registers;r.before=e.prior.stages[6]->registers;r.after=e.prior.stages[7]->registers;
 for(unsigned i=0;i<3;++i)r.rechecks[i]=e.prior.stages[8+i]->registers;
 f.ticketsCaptured=true;f.ticketCount=3;for(unsigned i=0;i<3;++i)f.tickets[i]={42,i+1,1,101,true,true};
 f.watchWrites=f.outerDbEvents=f.machineEntries=f.watchHits=f.finalPollWriteAttempts=1;f.totalWriteAttempts=2;f.unscopedWriteAttempts=f.failedWriteAttempts=0;f.ordinaryOwnedDataDb=f.engineClaimed=true;
 return f;
}
void finalReg(q::Inputs& f,unsigned reg,std::uint64_t x){for(unsigned i=7;i<l::count;++i)f.lifecycle.prior.stages[i]->registers[reg]=x;(*f.restoration.after)[reg]=x;for(auto& r:f.restoration.rechecks)(*r)[reg]=x;}
int main(int argc,char** argv){
 auto f=fixture();const auto old=c::stability(c::Vendor::Intel,f.restoration);
 assert(q::compare(f).safe&&!old.rawAll&&!old.canonicalAll);
 if(argc==2&&!std::strcmp(argv[1],"--json-fixture")){std::printf("{\"synthetic_configuration_fixture\":true,\"native_execution\":false,\"RAW_EXACT\":false,\"ARCHITECTURAL_CANONICAL\":false,\"model\":%s}\n",q::json(f).c_str());return 0;}
 assert(argc==1);std::puts("PASS: c47 exact synthetic c46 owner model accepts separately while unchanged RAW_EXACT and c44 canonical remain false");
 for(unsigned bit:{11u,16u}){auto t=fixture();t.lifecycle.prior.stages[3]->registers[4]|=std::uint64_t(1)<<bit;assert(!q::compare(t).safe);}
 std::puts("PASS: c47 either status bit first changing at SetThreadContext roundtrip refuses");
 for(unsigned mode=0;mode<4;++mode){auto t=fixture();
  if(mode==0)t.lifecycle.preAccess->registers[4]&=~0x10800ull;
  if(mode==1)t.lifecycle.prior.stages[5]->registers[4]&=~0x10800ull;
  if(mode==2)finalReg(t,4,0xfffe07f0);
  if(mode==3)t.lifecycle.preAccess.reset();
  assert(!q::compare(t).safe);
 }
 std::puts("PASS: c47 DB-entry-only, handler reversal/multiple, final reversal and missing pre-access evidence refuse");
 for(unsigned mode=0;mode<9;++mode){auto t=fixture();
  if(mode==0)t.lifecycle.gate.dbAtArrival=1;
  if(mode==1)t.lifecycle.gate.dbBeforeSuspend=1;
  if(mode==2)t.lifecycle.gate.dbAtCapture=1;
  if(mode==3)t.lifecycle.gate.dbAfterRead=1;
  if(mode==4)t.lifecycle.gate.machineEntriesAtCapture=1;
  if(mode==5)t.lifecycle.gate.writesAtCapture=2;
  if(mode==6)t.lifecycle.gate.releaseUnsignaled=false;
  if(mode==7)t.lifecycle.gate.pendingAtCapture.reset();
  if(mode==8)t.lifecycle.preAccess->sequence=6;
  assert(!q::compare(t).safe);
 }
 std::puts("PASS: c47 extra pre-access DB/access/write, missing/ambiguous gate/order/counts refuse");
 for(unsigned reg=0;reg<4;++reg){auto t=fixture();finalReg(t,reg,(*t.restoration.saved)[reg]^1);assert(!q::compare(t).safe);}
 std::puts("PASS: c47 Dr0, Dr1, synthetic foreign Dr2 and Dr3 mismatches each refuse");
 for(unsigned bit:{0u,1u,2u,3u,4u,5u,6u,7u,10u,11u,12u,13u,16u,18u,20u,22u,63u}){auto t=fixture();finalReg(t,5,(*t.restoration.after)[5]^(std::uint64_t(1)<<bit));assert(!q::compare(t).safe);}
 std::puts("PASS: c47 Dr7 owned enable/type/length, every global/foreign enable, RTM, fixed and unknown controls refuse on mismatch");
 for(unsigned bit:{0u,1u,2u,3u,11u,12u,13u,14u,15u,16u,63u}){auto t=fixture();finalReg(t,4,(*t.restoration.after)[4]^(std::uint64_t(1)<<bit));assert(!q::compare(t).safe);}
 for(unsigned bit:{4u,10u,17u,31u}){auto t=fixture();finalReg(t,4,(*t.restoration.after)[4]^(std::uint64_t(1)<<bit));assert(!q::compare(t).safe);}
 std::puts("PASS: c47 owned B0/B1, foreign B2/B3/BD/BS/BT, wrong final BLD/RTM, unknown/high and fixed DR6 violations refuse");
 for(unsigned mode=0;mode<7;++mode){auto t=fixture();
  if(mode==0)t.restoration.pendingBefore=1;
  if(mode==1)t.restoration.pendingAfter=1;
  if(mode==2)t.restoration.activeAfter=true;
  if(mode==3)t.restoration.canRetireBefore=false;
  if(mode==4)t.restoration.pollReturned=true;
  if(mode==5)t.restoration.activeBefore.reset();
  if(mode==6)t.poisoned=true;
  assert(!q::compare(t).safe);
 }
 std::puts("PASS: c47 pending debt, retained/missing lease, canRetire, poll and poisoned unknown debt refuse");
 for(unsigned mode=0;mode<8;++mode){auto t=fixture();
  if(mode==0)t.tickets[0].acknowledged=false;
  if(mode==1)t.tickets[1].currentCode=false;
  if(mode==2)t.tickets[2].sequence=2;
  if(mode==3)t.tickets[1].thread=43;
  if(mode==4)t.tickets[0].lease=2;
  if(mode==5)t.tickets[0].generation=102;
  if(mode==6)t.ticketsCaptured=false;
  if(mode==7)t.ticketCount=2;
  assert(!q::compare(t).safe);
 }
 std::puts("PASS: c47 unacknowledged, stale-code, wrong/duplicate thread-sequence-lease-generation and missing tickets refuse");
 for(unsigned mode=0;mode<6;++mode){auto t=fixture();
  if(mode==0)t.restoration.rechecks[1].reset();
  if(mode==1)(*t.restoration.rechecks[2])[0]^=1;
  if(mode==2)t.lifecycle.prior.stages[10]->registers[0]^=1;
  if(mode==3)t.restoration.before.reset();
  if(mode==4)t.lifecycle.prior.stages[6].reset();
  if(mode==5)t.restoration.saved.reset();
  assert(!q::compare(t).safe);
 }
 std::puts("PASS: c47 missing final readback, raw drift, independent evidence disagreement and incomplete timeline refuse");
 for(unsigned mode=0;mode<11;++mode){auto t=fixture();
  if(mode==0)t.vendor=c::Vendor::Unknown;
  if(mode==1)t.vendor=c::Vendor::AMD;
  if(mode==2)t.nativeWindows=false;
  if(mode==3)t.profile=q::Profile::Unknown;
  if(mode==4)t.leaf7Known=false;
  if(mode==5)t.prerequisitesKnown=false;
  if(mode==6)t.signature=0;
  if(mode==7)t.debuggerAbsent=false;
  if(mode==8)t.positionMultiple=false;
  if(mode==9)t.mitigations[0].flags=1;
  if(mode==10)t.mitigations[2].known=false;
  assert(!q::compare(t).safe);
 }
 std::puts("PASS: c47 unknown/AMD/Wine/unsupported CPU or scenario/debugger and refused/unknown mitigation profiles refuse");
 for(unsigned mode=0;mode<9;++mode){auto t=fixture();
  if(mode==0)t.outerDbEvents=2;
  if(mode==1)t.totalWriteAttempts=3;
  if(mode==2)t.finalPollWriteAttempts=2;
  if(mode==3)t.unscopedWriteAttempts=1;
  if(mode==4)t.failedWriteAttempts=1;
  if(mode==5)t.watchWrites.reset();
  if(mode==6)t.ordinaryOwnedDataDb=false;
  if(mode==7)t.engineClaimed=false;
  if(mode==8)t.mitigations[2].flags=0;
  assert(!q::compare(t).safe);
 }
 std::puts("PASS: c47 extra/missing/unscoped/failed context writes, extra DB, wrong event claim and alternate admitted tuple refuse");
}
