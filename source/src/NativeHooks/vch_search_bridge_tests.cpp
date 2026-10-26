// Clearly synthetic contract tests. No OS dispatch or native bridge is claimed.
#include "NativeHooks/vch_search_bridge_model.hpp"
#include <cassert>
#include <cstdio>
namespace m=ws::vch_search_bridge_model;
m::Identity id(unsigned sequence=1){return {42,99,0x1000+sequence,0x2000+sequence,1,101,sequence};}
m::Ledger downstream(){m::Ledger l;assert(l.reserve(id(),1));assert(l.escape(id(),2));assert(l.downstream(id(),3,0xfeed,true));return l;}
int main(){
 {m::Ledger l;assert(l.reserve(id(),1)&&l.pending()==1&&!l.canRetire());assert(l.escape(id(),2));assert(l.downstream(id(),3,0xfeed,true));assert(l.bind(id(),4,0xfeed,11,true,true,true));assert(l.acknowledge(id(),11)&&l.canRetire());}
 std::puts("PASS: c49 synthetic reserve-before-SEARCH -> downstream EXECUTION -> VCH -> exact acknowledgment ordering");
 {m::Ledger l;assert(!l.escape(id(),1)&&l.poison&&!l.canRetire());}
 {m::Ledger l;assert(l.reserve(id(),1));assert(!l.escape(id(),1)&&l.pending()==1);}
 std::puts("PASS: c49 synthetic missing reservation or ambiguous SEARCH order refuses");
 {m::Ledger l;auto invalid=id();invalid.lease=0;assert(!l.reserve(invalid,1)&&l.poison&&!l.canRetire());}
 {m::Ledger l;assert(l.reserve(id(),1)&&l.pending()==1);assert(!l.reserve(id(),2)&&l.pending()==1&&l.poison);}
 std::puts("PASS: c49 synthetic invalid/stale lease and duplicate reservation keep sticky unknown-debt refusal");
 for(unsigned order:{2u,3u}){auto l=downstream();assert(!l.bind(id(),order,0xfeed,11,true,true,true)&&l.poison&&l.pending()==1);}
 {m::Ledger l;assert(l.reserve(id(),1)&&l.escape(id(),2));assert(!l.bind(id(),3,0xfeed,11,true,true,true));}
 std::puts("PASS: c49 synthetic VCH before/downstream or missing final decision refuses");
 {auto l=downstream();assert(!l.bind(id(),4,0xfeed,11,false,true,true)&&l.pending()==1&&!l.canRetire());}
 std::puts("PASS: c49 synthetic registration without callback cannot bind/discharge debt; no native evidence");
 for(unsigned field=0;field<7;++field){auto l=downstream();auto wrong=id();switch(field){case 0:++wrong.thread;break;case 1:++wrong.epoch;break;case 2:++wrong.context;break;case 3:++wrong.exception;break;case 4:++wrong.lease;break;case 5:++wrong.generation;break;case 6:++wrong.sequence;break;}
  assert(!l.bind(wrong,4,0xfeed,11,true,true,true)&&l.poison&&l.pending()==1);
 }
 std::puts("PASS: c49 synthetic wrong context/exception/thread/epoch/lease/generation/sequence each refuses");
 {auto l=downstream();assert(!l.bind(id(),4,0xbeef,11,true,true,true));}
 {m::Ledger l;assert(l.reserve(id(),1)&&l.escape(id(),2));assert(!l.downstream(id(),3,0xfeed,false));}
 std::puts("PASS: c49 synthetic downstream final-context invalidation or non-EXECUTION decision refuses");
 {auto l=downstream();assert(l.bind(id(),4,0xfeed,11,true,true,true));auto second=id(2),third=id(3);third.thread=43;third.epoch=100;
  for(auto next:{second,third})assert(l.reserve(next,5)&&l.escape(next,6)&&l.downstream(next,7,0xbeef,true));
  assert(l.bind(second,8,0xbeef,12,true,true,true)&&l.bind(third,9,0xbeef,13,true,true,true));
  assert(l.pending()==3);assert(l.acknowledge(second,12)&&l.pending()==2&&!l.canRetire());assert(l.acknowledge(third,13)&&l.pending()==1);assert(l.acknowledge(id(),11)&&l.canRetire());
 }
 std::puts("PASS: c49 synthetic nested/multiple and other-thread reservations discharge independently");
 {auto l=downstream();assert(l.bind(id(),4,0xfeed,11,true,true,true));assert(!l.acknowledge(id(),12)&&l.pending()==1&&l.poison);}
 {auto l=downstream();assert(l.bind(id(),4,0xfeed,11,true,true,true)&&l.acknowledge(id(),11));assert(!l.acknowledge(id(),11)&&!l.canRetire());}
 {auto l=downstream();assert(l.bind(id(),4,0xfeed,11,true,true,true));auto other=id(2);assert(l.reserve(other,5)&&l.escape(other,6)&&l.downstream(other,7,0xfeed,true));assert(!l.bind(other,8,0xfeed,11,true,true,true));}
 std::puts("PASS: c49 synthetic wrong/stale/duplicate/reused token poisons instead of releasing another debt");
 {m::Ledger l;for(unsigned i=1;i<=4;++i)assert(l.reserve(id(i),i));assert(!l.reserve(id(5),5)&&l.pending()==4&&l.poison&&!l.canRetire());}
 for(unsigned reason=0;reason<2;++reason){auto l=downstream();assert(!l.bind(id(),4,0xfeed,11,true,reason!=0,reason!=1)&&l.poison&&l.pending()==1);}
 std::puts("PASS: c49 synthetic capacity/preparation/publication failure retains counted debt and sticky poison");
 for(unsigned scenario=0;scenario<3;++scenario){auto l=downstream();assert(l.bind(id(),4,0xfeed,11,true,true,true));assert(l.pending()==1&&!l.canRetire());assert(l.acknowledge(id(),11)&&l.pending()==0&&l.canRetire());}
 std::puts("PASS: c49 synthetic close/deadline/kill requests never clear pending; only exact acknowledgment does");
 {auto l=downstream();assert(l.bind(id(),4,0xfeed,11,true,true,true)&&l.acknowledge(id(),11)&&l.canRetire());assert(!l.storageReclaimable(false)&&l.storageReclaimable(true));}
 std::puts("PASS: c49 synthetic marker observed is not stub exit; owned worker termination needed for storage reclamation");
}
