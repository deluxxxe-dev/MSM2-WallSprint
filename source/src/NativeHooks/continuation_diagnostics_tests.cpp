// Synthetic serializer/comparison tests only; no watch state or API output injection.
#include "NativeHooks/continuation_diagnostics.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
namespace d=ws::continuation_diagnostics;
d::Report fixture(bool active,bool unequal) {
  d::Report r;r.syntheticInputs=true;r.attempted=true;
  r.saved=d::Registers{1,2,3,4,0xffff0ff0u,0x410};r.before=*r.saved;r.after=*r.saved;
  if(unequal)(*r.after)[4]^=0x8000000000000001ull;
  r.activeBefore=true;r.activeAfter=active;r.pollReturned=false;
  r.canRetireBefore=true;r.pendingBefore=0;r.pendingAfter=0;
  return r;
}
int main(int argc,char** argv) {
  if(argc==2&&!std::strcmp(argv[1],"--json-fixture")) {
    d::Report empty;empty.syntheticInputs=true;
    std::printf("{\"synthetic_diagnostic_fixture\":true,\"reports\":[%s,%s,%s,%s,%s]}\n",d::json(fixture(false,false)).c_str(),d::json(fixture(true,false)).c_str(),d::json(fixture(false,true)).c_str(),d::json(fixture(true,true)).c_str(),d::json(empty).c_str());return 0;
  }
  assert(argc==1);
  for(bool active:{false,true})for(bool unequal:{false,true}) {
    auto r=fixture(active,unequal);const auto* kind=d::classification(active,*r.saved,*r.after);
    const char* expected=active?(unequal?"LEASE_AND_DR_FAILURE":"LEASE_ONLY_FAILURE"):(unequal?"DR_ONLY_FAILURE":"ALL_RESTORED");
    assert(!std::strcmp(kind,expected));assert(d::same(*r.saved,*r.after)==!unequal);
    assert(d::json(r).find(expected)!=std::string::npos);
  }
  std::puts("PASS: c43 synthetic all-restored, lease-only, DR-only and combined classifications remain distinct");
  for(unsigned i=0;i<6;++i)for(auto bit:{1ull,0x8000000000000000ull}) {
    auto r=fixture(false,false);(*r.after)[i]^=bit;
    assert(!d::same(*r.saved,*r.after));assert(!std::strcmp(d::classification(false,*r.saved,*r.after),"DR_ONLY_FAILURE"));
    assert(d::json(r).find(d::hex(bit))!=std::string::npos);
  }
  std::puts("PASS: c43 each of six registers detects low/high-bit differences with exact unmasked comparison");
  auto r=fixture(false,false);r.activeAfter.reset();assert(d::json(r).find("NOT_OBSERVED")!=std::string::npos);
  r=fixture(false,false);r.after.reset();assert(d::json(r).find("\"final_predicate\":null")!=std::string::npos);
  std::puts("PASS: c43 missing lease or snapshot observations cannot be serialized as successful restoration");
  r=fixture(false,false);(*r.before)[0]^=0x80;auto before=*r.before;auto saved=*r.saved;
  auto text=d::json(r);assert(text.find("\"before_equal\":false")!=std::string::npos);assert(text.find("ALL_RESTORED")!=std::string::npos);
  assert(*r.before==before&&*r.saved==saved);
  std::puts("PASS: c43 before-poll divergence is independent from after-poll acceptance and serialization preserves inputs");
  r=fixture(false,false);r.rechecks[0]=*r.saved;r.rechecks[1]=*r.saved;(*r.rechecks[1])[5]^=1;
  text=d::json(r);assert(text.find("\"same_saved\":true")!=std::string::npos&&text.find("\"same_saved\":false")!=std::string::npos&&text.find("\"captured\":false")!=std::string::npos);
  std::puts("PASS: c43 bounded verification readbacks retain exact mismatches and missing-capture distinction");
  assert(d::hex(0)=="0x0000000000000000"&&d::hex(UINT64_MAX)=="0xffffffffffffffff");
  std::puts("PASS: c43 full-width hexadecimal values and XOR diagnostics preserve all64 bits without normalization");
}
