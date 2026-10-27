// Explicitly SYNTHETIC bridge contract tests, never native VCH evidence.
#include "NativeHooks/vch_post_return_witness.hpp"
#include <cassert>
#include <cstdio>
namespace q=ws::vch_post_return_witness;
q::Identity id(){return {42,99,0x1000,0x2000,1,1,149,1,0x140001000};}
q::Ledger reserved(){q::Ledger l;assert(l.reserve(id(),3,4));return l;}
q::Ledger seen(){auto l=reserved();assert(l.observeFinal(id(),6,7,true,true,true));return l;}
q::Ledger published(){auto l=seen();assert(l.publish(id(),0x41000001,true,true,true,true));return l;}
q::Ledger redirected(){auto l=published();assert(l.redirect(id(),true,true));return l;}
q::Configuration config(){
 q::Configuration f;f.nativeWindows=f.intel=f.admitted=f.boundAndAcknowledged=f.pollFalse=f.leasesInactive=f.stable=f.complete=true;
 f.saved={0,0,0x1000,0,0,0xd000010};f.entry={0x1000,0x1008,0x1000,0,0xffff0ff5,0xd990415};f.afterWS=f.entry;f.afterWS[4]=0xffff0ff4;f.finalVch=f.afterWS;f.finalVch[4]=0xffff0ff0;f.postReturn=f.finalVch;
 f.after=f.saved;f.after[4]=0xffff0ff0;f.after[5]=0xd000410;f.readbacks.fill(f.after);return f;
}
int main(int argc,char** argv){
 auto f=config();auto l=redirected();assert(l.acknowledge(id(),0x41000001,true)&&l.pending()==0&&l.canCleanup());
 assert(q::configurationRestored(f));
 if(argc==2&&std::string(argv[1])=="--json-fixture"){
  std::printf("{\"synthetic_bridge_contract\":true,\"native_execution\":false,\"expected_token\":%u,\"pending_after_model_ack\":%u,\"model_configuration_restored\":true,\"RAW_EXACT\":false,\"ARCHITECTURAL_CANONICAL\":false,\"live_storage_reclaimable\":false}\n",l.token,l.pending());return 0;
 }
 assert(argc==1);std::puts("PASS: c51 synthetic reservation -> final VCH -> prepared publication -> RIP-only -> exact token ordering");
 {q::Ledger x;assert(!x.reserve(id(),4,4)&&x.poison);}
 {auto x=reserved();assert(!x.reserve(id(),5,6)&&x.poison&&x.pending()==1);}
 std::puts("PASS: c51 synthetic reservation-before-SEARCH and single-debt cap are mandatory");
 for(unsigned mode=0;mode<3;++mode){auto x=reserved();assert(!x.observeFinal(id(),6,7,mode!=0,mode!=1,mode!=2)&&x.poison&&x.pending()==1);}
 std::puts("PASS: c51 synthetic multiple matching obligation, final-context mismatch or absent callback refuses");
 for(unsigned order:{5u,6u}){auto x=reserved();assert(!x.observeFinal(id(),6,order,true,true,true));}
 {auto x=seen();assert(!x.observeFinal(id(),6,8,true,true,true));}
 std::puts("PASS: c51 synthetic VCH-before-final, ambiguous order or duplicate VCH refuses");
 for(unsigned mode=0;mode<4;++mode){auto x=seen();assert(!x.publish(id(),0x41000001,mode!=0,mode!=1,mode!=2,mode!=3)&&x.poison&&x.pending()==1);}
 std::puts("PASS: c51 synthetic preparation/current-code/range/mitigation failure keeps sticky debt");
 {auto x=seen();assert(!x.redirect(id(),true,true)&&x.poison);}
 {auto x=published();assert(!x.redirect(id(),false,true)&&x.poison);}
 {auto x=published();assert(!x.redirect(id(),true,false)&&x.poison);}
 std::puts("PASS: c51 synthetic missing publication, edits beyond RIP or changed final context refuses");
 for(unsigned field=0;field<9;++field){auto x=redirected();auto wrong=id();switch(field){case 0:++wrong.thread;break;case 1:++wrong.epoch;break;case 2:++wrong.context;break;case 3:++wrong.exception;break;case 4:++wrong.lease;break;case 5:++wrong.window;break;case 6:++wrong.generation;break;case 7:++wrong.sequence;break;case 8:++wrong.originalRip;break;}assert(!x.acknowledge(wrong,0x41000001,true)&&x.pending()==1&&x.poison);}
 std::puts("PASS: c51 synthetic wrong thread/epoch/context/exception/lease/window/generation/sequence/original-RIP token refuses");
 {auto x=redirected();assert(!x.acknowledge(id(),0x41000002,true)&&x.pending()==1);}
 {auto x=redirected();assert(x.acknowledge(id(),0x41000001,true));assert(!x.acknowledge(id(),0x41000001,true)&&!x.canCleanup());}
 {auto x=published();assert(!x.acknowledge(id(),0x41000001,true)&&x.pending()==1);}
 std::puts("PASS: c51 synthetic stale/duplicate token or marker before redirection cannot discharge debt");
 {auto x=redirected();assert(x.pending()==1&&!x.canCleanup());assert(x.acknowledge(id(),0x41000001,true)&&x.pending()==0&&x.canCleanup());assert(!x.storageReclaimable(false)&&x.storageReclaimable(true));}
 std::puts("PASS: c51 synthetic cleanup waits for exact token; acknowledgment is not code exit/storage reclamation");
 {const auto old=ws::debug_register_comparison::compare(ws::debug_register_comparison::Vendor::Intel,f.saved,f.after);assert(!old.raw&&!old.canonical&&q::configurationRestored(f));}
 std::puts("PASS: c51 synthetic full same-event configuration property keeps old raw/c44 failures independent; c47 profile untouched");
 for(unsigned bit=0;bit<64;++bit){auto x=f;x.after[4]^=std::uint64_t(1)<<bit;x.readbacks.fill(x.after);assert(!q::configurationRestored(x));}
 std::puts("PASS: c51 synthetic every DR6 bit, including BLD/RTM/unknown, must equal the complete authenticated final-VCH word");
 for(unsigned bit=0;bit<64;++bit){auto x=f;x.after[5]^=std::uint64_t(1)<<bit;x.readbacks.fill(x.after);assert(!q::configurationRestored(x));}
 for(unsigned reg=0;reg<4;++reg){auto x=f;++x.after[reg];x.readbacks.fill(x.after);assert(!q::configurationRestored(x));}
 std::puts("PASS: c51 synthetic every final DR7 bit violation and each Dr0-Dr3/foreign-address mismatch refuses");
 for(unsigned mode=0;mode<8;++mode){auto x=f;if(mode==0)x.nativeWindows=false;if(mode==1)x.intel=false;if(mode==2)x.admitted=false;if(mode==3)x.boundAndAcknowledged=false;if(mode==4)x.pollFalse=false;if(mode==5)x.leasesInactive=false;if(mode==6)x.stable=false;if(mode==7)x.complete=false;assert(!q::configurationRestored(x));}
 {auto x=f;x.readbacks[2][4]^=1;assert(!q::configurationRestored(x));}
 std::puts("PASS: c51 synthetic unknown vendor/environment/policy/debt/poll/lease/stability/missing readback refuses");
 for(unsigned mode=0;mode<5;++mode){auto x=f;if(mode==0)x.entry[4]^=4;if(mode==1)x.afterWS[4]^=1;if(mode==2)x.finalVch[4]^=0x10800;if(mode==3)x.postReturn[0]^=1;if(mode==4)x.saved[4]=4;assert(!q::configurationRestored(x));}
 std::puts("PASS: c51 synthetic wrong mixed-cause timeline, post-return configuration or pre-existing foreign cause refuses");
}
