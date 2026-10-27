// SYNTHETIC only: these are predicate inputs, never fabricated native snapshots.
#include "NativeHooks/vch_search_identity.hpp"
#include <cassert>
#include <cstdio>
namespace q=ws::vch_search_identity;
q::Evidence fixture(bool unrelated=false){
 q::Evidence e;e.observed=e.stableSnapshot=e.search=e.ripPreserved=true;e.registersPreserved.fill(true);
 e.entryCause=5;e.postCause=4;e.delta=1;e.worker=42;e.usedTotal=e.recordCount=unrelated?2:1;e.expectedUp=0x1000;
 e.legacyWindowKnown=true;e.legacyWindowId=1;e.legacyWindowGeneration=149;
 for(unsigned i=0;i<e.recordCount;++i)e.leases[i]={i,i?42u:unrelated?43u:42u,0,1,3,149,0x1000,true,true,true,true,true,false};
 return e;
}
int main(int argc,char** argv){
 auto e=fixture(true);const auto r=q::evaluate(e);assert(r.onlyGlobalCountFailure&&r.uniqueMatching&&r.diagnosticAdmissible&&!r.legacyAggregate&&r.selected==1);
 if(argc==2&&std::string(argv[1])=="--json-fixture"){std::printf("{\"synthetic_identity_fixture\":true,\"native_execution\":false,\"evidence\":%s}\n",q::json(e).c_str());return 0;}
 assert(argc==1);
 std::puts("PASS: c50 synthetic unrelated global lease plus unique worker/context obligation separates old FAIL from named candidate");
 {const auto x=q::evaluate(fixture());assert(x.legacyAggregate&&x.uniqueMatching&&x.diagnosticAdmissible);}
 std::puts("PASS: c50 synthetic exact single matching debt preserves all old conjunctions");
 for(unsigned own:{1u,2u,3u}){auto x=e;x.leases[0].tid=42;x.leases[0].ownMask=own;const auto y=q::evaluate(x);assert(!y.uniqueMatching&&!y.diagnosticAdmissible&&y.selected==-1&&y.ownedWorkerCount==2);}
 std::puts("PASS: c50 synthetic duplicate/full or partial competing worker obligations refuse");
 {auto x=e;x.worker=44;assert(!q::evaluate(x).uniqueMatching);}
 std::puts("PASS: c50 synthetic wrong worker tid refuses");
 for(unsigned mode=0;mode<2;++mode){auto x=e;if(mode)x.leases[1].windowIndex=1;else x.leases[1].windowId=2;assert(!q::evaluate(x).diagnosticAdmissible);}
 std::puts("PASS: c50 synthetic wrong window id or index/id relation refuses");
 {auto x=e;++x.leases[1].generation;assert(!q::evaluate(x).diagnosticAdmissible);}
 std::puts("PASS: c50 synthetic wrong generation refuses");
 for(unsigned own:{0u,1u,2u}){auto x=e;x.leases[1].ownMask=own;assert(!q::evaluate(x).diagnosticAdmissible);}
 {auto x=e;++x.leases[1].up;assert(!q::evaluate(x).diagnosticAdmissible);}
 std::puts("PASS: c50 synthetic partial/missing ownership or wrong up refuses");
 for(unsigned mode=0;mode<4;++mode){auto x=e;if(mode==0)x.search=false;if(mode==1)x.entryCause^=8;if(mode==2)x.postCause^=4;if(mode==3)x.delta^=0x10800;assert(!q::evaluate(x).diagnosticAdmissible);}
 std::puts("PASS: c50 synthetic disposition/cause/delta conflict stays FAIL with no status mask");
 for(unsigned reg=0;reg<5;++reg){auto x=e;x.registersPreserved[reg]=false;assert(!q::evaluate(x).diagnosticAdmissible);}
 {auto x=e;x.ripPreserved=false;assert(!q::evaluate(x).diagnosticAdmissible);}
 std::puts("PASS: c50 synthetic each Dr0/1/2/3/7 and RIP preservation conflict refuses");
 for(unsigned mode=0;mode<6;++mode){auto x=e;if(mode==0)x.stableSnapshot=false;if(mode==1)--x.recordCount;if(mode==2)x.leases[1].metadataKnown=false;if(mode==3)x.leases[1].statusKnown=false;if(mode==4)x.leases[1].installed=false;if(mode==5)x.leases[1].suspended=true;assert(!q::evaluate(x).diagnosticAdmissible);}
 std::puts("PASS: c50 synthetic incomplete/unstable metadata or unavailable installation status refuses");
 {auto x=e;x.leases[0].ownMask=0;const auto y=q::evaluate(x);assert(y.uniqueMatching&&!y.onlyGlobalCountFailure&&!y.diagnosticAdmissible);}
 {auto x=fixture();x.leases[0].windowIndex=1;x.leases[0].windowId=2;const auto y=q::evaluate(x);assert(y.legacyAggregate&&!y.uniqueMatching&&!y.diagnosticAdmissible);}
 for(unsigned mode=0;mode<3;++mode){auto x=e;if(mode==0)x.legacyWindowKnown=false;if(mode==1)x.legacyWindowId=2;if(mode==2)x.legacyWindowGeneration=150;assert(!q::evaluate(x).diagnosticAdmissible);}
 {auto x=e;x.recordCount=x.usedTotal=q::capacity+1;assert(!q::evaluate(x).diagnosticAdmissible);}
 {q::Evidence x;assert(!q::evaluate(x).diagnosticAdmissible);}
 std::puts("PASS: c50 synthetic additional legacy conjunct failure, unknown capture and cap overflow cannot be silently relaxed");
 {auto text=q::json(e);for(auto name:q::conjunctNames)assert(text.find(name)!=std::string::npos);assert(text.find("\"legacy_c49_aggregate\":false")!=std::string::npos);}
 std::puts("PASS: c50 synthetic every old conjunct and historical aggregate FAIL is serializable independently");
}
