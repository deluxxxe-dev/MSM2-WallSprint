#pragma once
// TEST ONLY. Conservative vendor contract; never linked to production restoration.
// Sources/limitations: analysis/V01C44_DR_CANONICALIZATION.md. BLD/RTM are NEVER masked.
#include "NativeHooks/continuation_diagnostics.hpp"
#include <cstring>

namespace ws::debug_register_comparison {
namespace d=ws::continuation_diagnostics;
enum class Vendor { Unknown, Intel, AMD };
inline const char* name(Vendor v) {
  return v==Vendor::Intel?"GenuineIntel":v==Vendor::AMD?"AuthenticAMD":"UNKNOWN";
}
inline Vendor vendor(const char* bytes) {
  if(!std::memcmp(bytes,"GenuineIntel",12))return Vendor::Intel;
  if(!std::memcmp(bytes,"AuthenticAMD",12))return Vendor::AMD;
  return Vendor::Unknown;
}
inline bool supportedCpuProfile(Vendor v,bool leaf7Known,std::uint32_t leaf7B) {
  if(v==Vendor::Intel)return true; // BLD/RTM fields remain exact, not feature-masked.
  // The checked AMD edition does not certify an RTM-capable AMD DR7 layout.
  return v==Vendor::AMD&&leaf7Known&&(leaf7B&(1u<<11))==0;
}
struct Masks { std::uint64_t meaningful{},one{},zero{},osExact{},unknown{UINT64_MAX}; };
inline Masks masks(Vendor v,unsigned reg) {
  if(v==Vendor::Intel&&reg==6)return {0x1e80f,0xfffe07f0,0xffffffff00000000ull,0,0x1000};
  if(v==Vendor::AMD&&reg==6)return {0xe00f,0xfffe07f0,0xffffffff00001000ull,0x10800,0};
  if(v==Vendor::Intel&&reg==7)return {0xffff2b55,0x400,0xffffffff00000000ull,0xaa,0xd000};
  if(v==Vendor::AMD&&reg==7)return {0xffff2355,0x400,0xffffffff0000d800ull,0xaa,0};
  return {};
}
inline const char* bitClass(Vendor v,unsigned reg,unsigned bit) {
  if(bit>=64)return "UNKNOWN_REFUSE";
  const auto m=masks(v,reg);const auto b=std::uint64_t(1)<<bit;
  if(m.meaningful&b)return "MEANINGFUL_EXACT";
  if(m.one&b)return "FIXED_ONE";
  if(m.zero&b)return "FIXED_ZERO";
  if(m.osExact&b)return "OS_CONTROLLED_EXACT_OR_REFUSE";
  return "UNKNOWN_REFUSE";
}
struct Result {
  bool raw{},canonical{},addressesExact{},meaningfulExact{},osExact{},unknownClear{},fixedValid{},statusBoundaryValid{};
  const char* reason="unknown_vendor";
};
inline Result compare(Vendor v,const d::Registers& saved,const d::Registers& current) {
  Result r;r.raw=d::same(saved,current);
  if(v!=Vendor::Intel&&v!=Vendor::AMD)return r;
  r.addressesExact=true;r.meaningfulExact=true;r.osExact=true;r.unknownClear=true;r.fixedValid=true;
  for(unsigned i=0;i<4;++i)r.addressesExact&=saved[i]==current[i];
  for(unsigned i=4;i<6;++i) {
    const auto m=masks(v,i==4?6:7);const auto delta=saved[i]^current[i];
    r.meaningfulExact&=(delta&m.meaningful)==0;
    r.osExact&=(delta&m.osExact)==0;
    r.unknownClear&=((saved[i]|current[i])&m.unknown)==0;
    r.fixedValid&=(current[i]&m.one)==m.one&&(current[i]&m.zero)==0;
  }
  // Narrow candidate boundary, NOT a fixed-bit mask or an assertion that BLD/
  // RTM are universally fixed. Without a certified feature/OS contract, reject
  // their active-low current0 states even if raw-equal. Still require EXACT
  // saved/current equality above: a saved0/current1 transition never becomes OK.
  r.statusBoundaryValid=(current[4]&0x10800)==0x10800;
  r.reason=!r.addressesExact?"Dr0_Dr3_not_exact":!r.meaningfulExact?"meaningful_status_or_control_not_exact":!r.osExact?"OS_or_potential_status_not_exact":!r.unknownClear?"unknown_bits_present":!r.fixedValid?"current_fixed_bits_not_documented_value":!r.statusBoundaryValid?"BLD_RTM_current_status_outside_supported_boundary":"architectural_equivalent";
  r.canonical=r.addressesExact&&r.meaningfulExact&&r.osExact&&r.unknownClear&&r.fixedValid&&r.statusBoundaryValid;
  return r;
}
inline std::string maskJson(Masks m) {
  return "{\"MEANINGFUL_EXACT\":\""+d::hex(m.meaningful)+"\",\"FIXED_ONE\":\""+d::hex(m.one)+"\",\"FIXED_ZERO\":\""+d::hex(m.zero)+"\",\"OS_CONTROLLED_EXACT_OR_REFUSE\":\""+d::hex(m.osExact)+"\",\"UNKNOWN_REFUSE\":\""+d::hex(m.unknown)+"\"}";
}
inline std::string resultJson(Result r) {
  return "{\"RAW_EXACT\":"+d::boolean(r.raw)+",\"ARCHITECTURAL_CANONICAL\":"+d::boolean(r.canonical)+",\"Dr0_Dr3_exact\":"+d::boolean(r.addressesExact)+",\"meaningful_exact\":"+d::boolean(r.meaningfulExact)+",\"OS_controlled_exact\":"+d::boolean(r.osExact)+",\"unknown_bits_clear\":"+d::boolean(r.unknownClear)+",\"current_fixed_bits_valid\":"+d::boolean(r.fixedValid)+",\"status_boundary_valid\":"+d::boolean(r.statusBoundaryValid)+",\"reason\":\""+r.reason+"\"}";
}
struct Stability {
  bool complete{},rawAll{},canonicalAll{},rawStable{},canonicalResultsStable{};
};
inline Stability stability(Vendor v,const d::Report& report) {
  Stability s;
  if(!report.saved||!report.after)return s;
  for(const auto& r:report.rechecks)if(!r)return s;
  s.complete=true;const auto initial=compare(v,*report.saved,*report.after);
  s.rawAll=initial.raw;s.canonicalAll=initial.canonical;s.rawStable=true;s.canonicalResultsStable=true;
  for(const auto& r:report.rechecks) {
    const auto next=compare(v,*report.saved,*r);
    s.rawAll&=next.raw;s.canonicalAll&=next.canonical;
    s.rawStable&=d::same(*report.after,*r);
    s.canonicalResultsStable&=next.canonical==initial.canonical;
  }
  return s;
}
inline std::string reportJson(Vendor v,const d::Report& report) {
  const auto s=stability(v,report);
  std::string out="{\"revision\":\"c44-v1\",\"documentation\":\"Intel_SDM_075_2021_AMD_24593_3.30_2018_conservative\",\"cpu_vendor\":\"";
  out+=name(v);out+="\",\"vendor_supported\":"+d::boolean(v==Vendor::Intel||v==Vendor::AMD);
  out+=",\"supported_Dr6_current_status_must_one\":\"0x0000000000010800\"";
  out+=",\"masks\":{\"Dr6\":"+maskJson(masks(v,6))+",\"Dr7\":"+maskJson(masks(v,7))+"}";
  out+=",\"before\":";out+=report.saved&&report.before?resultJson(compare(v,*report.saved,*report.before)):"null";
  out+=",\"after\":";out+=report.saved&&report.after?resultJson(compare(v,*report.saved,*report.after)):"null";
  out+=",\"verification_readbacks\":[";
  for(unsigned i=0;i<3;++i){if(i)out+=',';out+=report.saved&&report.rechecks[i]?resultJson(compare(v,*report.saved,*report.rechecks[i])):"null";}
  out+="],\"readbacks_complete\":"+d::boolean(s.complete);
  out+=",\"raw_all_exact\":"+d::boolean(s.complete?std::optional<bool>(s.rawAll):std::nullopt);
  out+=",\"architectural_all_equivalent\":"+d::boolean(s.complete?std::optional<bool>(s.canonicalAll):std::nullopt);
  out+=",\"raw_readbacks_stable\":"+d::boolean(s.complete?std::optional<bool>(s.rawStable):std::nullopt);
  out+=",\"architectural_results_stable\":"+d::boolean(s.complete?std::optional<bool>(s.canonicalResultsStable):std::nullopt);
  return out+"}";
}
} // namespace ws::debug_register_comparison
