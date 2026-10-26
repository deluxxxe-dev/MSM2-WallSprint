// Synthetic DWORD classifier tests only: never a native mitigation runtime claim.
#include "NativeHooks/continuation_mitigations.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
namespace m=ws::continuation_mitigations;
int main(int argc,char** argv) {
  m::Inputs zero{{{true,0},{true,0},{true,0}}};
  if(argc==2&&std::strcmp(argv[1],"--json-fixture")==0) {
    auto observed=zero;observed[2].flags=0x100;
    auto mixed=observed;mixed[2].flags|=0x80000001u;
    auto unknown=observed;unknown[2].known=false;
    std::printf("{\"synthetic_classifier_fixture\":true,\"reports\":[%s,%s,%s]}\n",m::json(observed).c_str(),m::json(mixed).c_str(),m::json(unknown).c_str());return 0;
  }
  assert(argc==1);
  assert(m::admitted(zero));
  for(unsigned i=0;i<3;++i) {auto r=m::classify(static_cast<m::Policy>(i),zero[i]);assert(r.admitted&&!r.relevant&&!r.irrelevant&&!r.unknown);}
  std::puts("PASS: c42 known all-zero policy boundary admits without inventing runtime evidence");
  auto observed=zero;observed[2].flags=0x100;assert(m::admitted(observed));
  auto r=m::classify(m::Policy::UserShadowStack,observed[2]);assert(r.irrelevant==0x100&&!r.relevant&&!r.unknown);
  assert(m::rule(m::Policy::UserShadowStack,8).decision==m::Decision::Irrelevant);
  std::puts("PASS: c42 synthetic observed tuple [0,0,0x100] admits only its documented unused-API restriction");
  const std::uint32_t relevant[]={0x1,0x7,0x75};
  const std::uint32_t irrelevant[]={0x2,0,0x100};
  const std::uint32_t known[]={0xf,0x1f,0x3ff};
  for(unsigned i=0;i<3;++i)for(unsigned bit=0;bit<32;++bit) {
    auto p=static_cast<m::Policy>(i);auto mask=std::uint32_t(1)<<bit;auto q=m::rule(p,bit);auto one=m::classify(p,{true,mask});
    assert(q.mask==mask);assert((one.relevant|one.irrelevant|one.unknown)==mask);
    assert((one.relevant&one.irrelevant)==0&&(one.relevant&one.unknown)==0&&(one.irrelevant&one.unknown)==0);
    if(mask&relevant[i])assert(q.decision==m::Decision::RelevantRefuse&&one.relevant==mask&&!one.admitted);
    else if(mask&irrelevant[i])assert(q.decision==m::Decision::Irrelevant&&one.irrelevant==mask&&one.admitted);
    else assert(q.decision==m::Decision::UnknownRefuse&&one.unknown==mask&&!one.admitted);
  }
  std::puts("PASS: c42 every materially relevant known protection bit refuses individually; all96 single-bit decisions partition their raw flags");
  for(unsigned i=0;i<3;++i)for(unsigned bit=0;bit<32;++bit)if(!(known[i]&(std::uint32_t(1)<<bit)))assert(!m::classify(static_cast<m::Policy>(i),{true,std::uint32_t(1)<<bit}).admitted);
  assert(!m::classify(m::Policy::DynamicCode,{true,8}).admitted);
  assert(!m::classify(m::Policy::ControlFlowGuard,{true,0x18}).admitted);
  std::puts("PASS: c42 all reserved positions and semantically unknown Audit/XFG fields fail closed, including the top DWORD bit");
  for(unsigned i=0;i<3;++i) {
    auto mixed=observed;mixed[i].flags|=relevant[i];assert(!m::admitted(mixed));
    mixed=observed;mixed[i].flags|=0x80000000u;assert(!m::admitted(mixed));
    auto all=m::classify(static_cast<m::Policy>(i),{true,0xffffffffu});assert(!all.admitted&&all.relevant==relevant[i]&&all.irrelevant==irrelevant[i]);
  }
  assert(m::classify(m::Policy::UserShadowStack,{true,0x105}).relevant==5);
  std::puts("PASS: c42 irrelevant bits cannot hide relevant/unknown bits within a policy or across policies");
  auto opt=observed;opt[0].flags=2;assert(m::admitted(opt));opt[0].flags|=1;assert(!m::admitted(opt));opt[0].flags=4;assert(!m::admitted(opt));
  std::puts("PASS: c42 unused thread opt-out permission alone is irrelevant; prohibition and unproved remote transition still refuse");
  for(unsigned i=0;i<3;++i)for(auto flags:{0u,0x100u,0xffffffffu}) {auto q=observed;q[i]={false,flags};assert(!m::admitted(q));auto unknown=m::classify(static_cast<m::Policy>(i),q[i]);assert(!unknown.admitted&&!unknown.irrelevant&&!unknown.relevant&&!unknown.unknown);}
  std::puts("PASS: c42 unknown query always refuses regardless of raw/zero/observed flags, with no Wine or native bypass");
  assert(!m::classify(static_cast<m::Policy>(99),{true,0}).admitted);
  assert(m::rule(m::Policy::UserShadowStack,32).decision==m::Decision::UnknownRefuse);
  std::puts("PASS: c42 invalid policy/bit inputs cannot silently become admitted zero masks");
  auto text=m::json(observed);assert(text.find("CetDynamicApisOutOfProcOnly")!=std::string::npos&&text.find("IRRELEVANT_TO_C41_CANDIDATE")!=std::string::npos&&text.find("\"irrelevant_mask\":256")!=std::string::npos);
  auto q=observed;q[2].known=false;auto unknownText=m::json(q);assert(unknownText.find("QUERY_UNKNOWN_REFUSE")!=std::string::npos&&unknownText.find("CetDynamicApisOutOfProcOnly")==std::string::npos);
  std::puts("PASS: c42 auditable JSON preserves raw flags, per-policy decisions and enabled-bit evidence without interpreting failed queries");
}
