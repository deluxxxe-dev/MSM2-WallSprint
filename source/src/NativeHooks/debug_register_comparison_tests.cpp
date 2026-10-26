// Owned synthetic inputs only, never an injected native register/API result.
#include "NativeHooks/debug_register_comparison.hpp"
#include <cassert>
#include <cstdio>
namespace c=ws::debug_register_comparison;namespace d=ws::continuation_diagnostics;
d::Registers valid(){return {0,0,0x140000000,0,0xffff0ff0,0x410};}
int main(int argc,char** argv) {
  const auto good=valid();auto saved=good;saved[5]&=~0x400ull;
  d::Registers observedSaved{0,0,0,0,0,0x10},observedCurrent{0,0,0,0,0xffff0ff0,0x410};
  if(argc==2&&!std::strcmp(argv[1],"--json-fixture")) {
    std::printf("{\"synthetic_comparator_fixture\":true,\"fixed_only\":%s,\"observed_c43_Intel\":%s,\"observed_c43_AMD\":%s,\"unknown_vendor\":%s}\n",c::resultJson(c::compare(c::Vendor::Intel,saved,good)).c_str(),c::resultJson(c::compare(c::Vendor::Intel,observedSaved,observedCurrent)).c_str(),c::resultJson(c::compare(c::Vendor::AMD,observedSaved,observedCurrent)).c_str(),c::resultJson(c::compare(c::Vendor::Unknown,good,good)).c_str());return 0;
  }
  assert(argc==1);
  assert(c::vendor("GenuineIntel")==c::Vendor::Intel&&c::vendor("AuthenticAMD")==c::Vendor::AMD&&c::vendor("UnknownVendr")==c::Vendor::Unknown);
  assert(c::supportedCpuProfile(c::Vendor::Intel,false,0));
  assert(c::supportedCpuProfile(c::Vendor::AMD,true,0));
  assert(!c::supportedCpuProfile(c::Vendor::AMD,false,0)&&!c::supportedCpuProfile(c::Vendor::AMD,true,1u<<11));
  assert(!c::supportedCpuProfile(c::Vendor::Unknown,true,0));
  for(auto v:{c::Vendor::Intel,c::Vendor::AMD}) {
    auto r=c::compare(v,good,good);assert(r.raw&&r.canonical);
    for(unsigned reg:{6u,7u}) {
      auto m=c::masks(v,reg);const std::uint64_t groups[]={m.meaningful,m.one,m.zero,m.osExact,m.unknown};std::uint64_t total{};
      for(auto g:groups){assert(!(total&g));total|=g;}assert(total==UINT64_MAX);
      for(unsigned bit=0;bit<64;++bit)assert(std::strcmp(c::bitClass(v,reg,bit),"UNKNOWN_REFUSE")||((std::uint64_t(1)<<bit)&m.unknown));
    }
  }
  std::puts("PASS: c44 vendor identification, exact valid snapshots and complete disjoint128-bit vendor classifications");
  for(auto v:{c::Vendor::Intel,c::Vendor::AMD}) {
    auto r=c::compare(v,observedSaved,observedCurrent);assert(!r.raw&&!r.canonical);
  }
  std::puts("PASS: c44 synthetic observed c43 pair refuses; BLD/potential-status11 and RTM/potential-status16 are never normalized");
  for(auto v:{c::Vendor::Intel,c::Vendor::AMD}) {
    auto r=c::compare(v,saved,good);assert(!r.raw&&r.canonical);
    auto old=good;old[4]^=0x10;assert(c::compare(v,old,good).canonical);
  }
  std::puts("PASS: c44 raw-exact false and architectural true only for documented fixed-one representation differences");
  for(auto v:{c::Vendor::Intel,c::Vendor::AMD})for(unsigned i:{4u,5u}) {
    auto m=c::masks(v,i==4?6:7);
    for(unsigned bit=0;bit<64;++bit)if((m.one|m.zero)&(std::uint64_t(1)<<bit)) {
      auto wrong=good;wrong[i]^=std::uint64_t(1)<<bit;
      assert(!c::compare(v,good,wrong).canonical);auto exact=c::compare(v,wrong,wrong);assert(exact.raw&&!exact.canonical);
    }
  }
  for(auto v:{c::Vendor::Intel,c::Vendor::AMD})for(unsigned bit:{11u,16u}) {
    auto active=good;active[4]&=~(std::uint64_t(1)<<bit);auto r=c::compare(v,active,active);
    assert(r.raw&&r.fixedValid&&r.meaningfulExact&&r.osExact&&!r.statusBoundaryValid&&!r.canonical);
  }
  std::puts("PASS: c44 every current fixed-bit violation or uncertified active-low BLD/RTM state refuses even when raw exact is true");
  for(auto v:{c::Vendor::Intel,c::Vendor::AMD})for(unsigned i:{4u,5u}) {
    auto m=c::masks(v,i==4?6:7);
    for(unsigned bit=0;bit<64;++bit)if((m.meaningful|m.osExact)&(std::uint64_t(1)<<bit)) {
      auto wrong=good;wrong[i]^=std::uint64_t(1)<<bit;assert(!c::compare(v,good,wrong).canonical);
    }
  }
  // Golden architectural positions, independent of the helper's declared masks.
  // Put the changed bit in SAVED so accidental fixed-value normalization would
  // otherwise hide the loss when CURRENT has the documented baseline value.
  for(auto v:{c::Vendor::Intel,c::Vendor::AMD}) {
    for(unsigned bit:{0u,1u,2u,3u,11u,13u,14u,15u,16u}) {
      auto old=good;old[4]^=std::uint64_t(1)<<bit;
      assert(!c::compare(v,old,good).canonical);
    }
    for(unsigned bit:{0u,1u,4u,5u,8u,9u,13u,16u,18u,31u}) {
      auto old=good;old[5]^=std::uint64_t(1)<<bit;
      assert(!c::compare(v,old,good).canonical);
    }
    assert(!std::strcmp(c::bitClass(v,6,0),"MEANINGFUL_EXACT"));
    assert(!std::strcmp(c::bitClass(v,7,0),"MEANINGFUL_EXACT"));
    assert(!std::strcmp(c::bitClass(v,7,1),"OS_CONTROLLED_EXACT_OR_REFUSE"));
  }
  std::puts("PASS: c44 all meaningful status/control, BLD/RTM, global/local enables, GD and type/length mismatches refuse");
  for(unsigned i=0;i<4;++i){auto wrong=good;wrong[i]^=0x8000000000000001ull;assert(!c::compare(c::Vendor::Intel,good,wrong).canonical);assert(!c::compare(c::Vendor::AMD,good,wrong).canonical);}
  std::puts("PASS: c44 Dr0-Dr3 require exact full64bit address preservation");
  assert(!c::compare(c::Vendor::Unknown,good,good).canonical);
  assert(!c::compare(static_cast<c::Vendor>(99),good,good).canonical);
  auto unknown=good;unknown[4]|=0x1000;assert(!c::compare(c::Vendor::Intel,unknown,unknown).canonical);
  assert(!std::strcmp(c::bitClass(c::Vendor::Intel,6,64),"UNKNOWN_REFUSE"));
  std::puts("PASS: c44 unknown vendor/policy and nonzero unknown-reserved bits refuse even when equally present");
  d::Report report;report.syntheticInputs=true;report.saved=saved;report.after=good;
  for(auto& r:report.rechecks)r=good;
  auto s=c::stability(c::Vendor::Intel,report);assert(s.complete&&!s.rawAll&&s.canonicalAll&&s.rawStable&&s.canonicalResultsStable);
  (*report.rechecks[1])[0]^=1;s=c::stability(c::Vendor::Intel,report);assert(s.complete&&!s.canonicalAll&&!s.rawStable&&!s.canonicalResultsStable);
  report.rechecks[2].reset();assert(!c::stability(c::Vendor::Intel,report).complete);
  std::puts("PASS: c44 three immutable readbacks must all agree; drift or missing capture cannot certify stability");
}
