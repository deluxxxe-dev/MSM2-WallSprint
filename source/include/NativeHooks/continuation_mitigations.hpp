#pragma once
// Owned standalone-probe classifier only. No production watch admission.
// Layout: checked Microsoft WinSDK winnt.h; semantics/evidence in
// analysis/V01C42_MITIGATION_BOUNDARY.md. Unknown fields ALWAYS refuse.
#include <array>
#include <cstdint>
#include <string>

namespace ws::continuation_mitigations {
enum class Policy : unsigned { DynamicCode, ControlFlowGuard, UserShadowStack };
enum class Decision { RelevantRefuse, Irrelevant, UnknownRefuse };
struct Rule {
  std::uint32_t mask;
  const char* name;
  Decision decision;
  const char* reason;
};
inline constexpr Rule dynamicRules[] = {
  {0x1, "ProhibitDynamicCode", Decision::RelevantRefuse, "candidate_allocates_and_makes_owned_code_executable"},
  {0x2, "AllowThreadOptOut", Decision::Irrelevant, "candidate_never_calls_ThreadDynamicCodePolicy_opt_out"},
  {0x4, "AllowRemoteDowngrade", Decision::UnknownRefuse, "remote_policy_transition_contract_not_established"},
  {0x8, "AuditProhibitDynamicCode", Decision::UnknownRefuse, "SDK_field_present_documented_semantics_insufficient"},
};
inline constexpr Rule cfgRules[] = {
  {0x1, "EnableControlFlowGuard", Decision::RelevantRefuse, "whole_probe_has_indirect_callbacks_and_API_calls_not_CFG_certified"},
  {0x2, "EnableExportSuppression", Decision::RelevantRefuse, "governs_exported_indirect_API_targets_complete_path_not_certified"},
  {0x4, "StrictMode", Decision::RelevantRefuse, "loaded_image_CFG_compatibility_not_certified"},
  {0x8, "EnableXfg", Decision::UnknownRefuse, "SDK_field_present_XFG_contract_not_established"},
  {0x10, "EnableXfgAuditMode", Decision::UnknownRefuse, "SDK_field_present_XFG_audit_contract_not_established"},
};
inline constexpr Rule shadowRules[] = {
  {0x1, "EnableUserShadowStack", Decision::RelevantRefuse, "hardware_return_and_context_compatibility_not_certified"},
  {0x2, "AuditUserShadowStack", Decision::UnknownRefuse, "audit_requires_unproved_shadow_stack_boundary"},
  {0x4, "SetContextIpValidation", Decision::RelevantRefuse, "candidate_redirects_saved_RIP_context_IP_validation_unproved"},
  {0x8, "AuditSetContextIpValidation", Decision::UnknownRefuse, "audit_does_not_certify_context_restoration"},
  {0x10, "EnableUserShadowStackStrictMode", Decision::RelevantRefuse, "strict_hardware_return_compatibility_not_certified"},
  {0x20, "BlockNonCetBinaries", Decision::RelevantRefuse, "complete_probe_image_CET_compatibility_not_certified"},
  {0x40, "BlockNonCetBinariesNonEhcont", Decision::RelevantRefuse, "probe_has_no_EH_continuation_metadata_contract"},
  {0x80, "AuditBlockNonCetBinaries", Decision::UnknownRefuse, "audit_does_not_certify_image_continuation_compatibility"},
  {0x100, "CetDynamicApisOutOfProcOnly", Decision::Irrelevant, "candidate_uses_neither_restricted_CET_dynamic_API"},
  {0x200, "SetContextIpValidationRelaxedMode", Decision::UnknownRefuse, "relaxed_validation_is_not_an_execution_certificate"},
};
inline const char* name(Policy p) {
  switch(p) {
    case Policy::DynamicCode: return "ProcessDynamicCodePolicy";
    case Policy::ControlFlowGuard: return "ProcessControlFlowGuardPolicy";
    case Policy::UserShadowStack: return "ProcessUserShadowStackPolicy";
  }
  return "UnknownPolicy";
}
inline const char* name(Decision d) {
  switch(d) {
    case Decision::RelevantRefuse: return "RELEVANT_REFUSE";
    case Decision::Irrelevant: return "IRRELEVANT_TO_C41_CANDIDATE";
    case Decision::UnknownRefuse: return "UNKNOWN_REFUSE";
  }
  return "UNKNOWN_REFUSE";
}
inline Rule rule(Policy p, unsigned bit) {
  if(bit >= 32) return {0, "InvalidBit", Decision::UnknownRefuse, "bit_outside_DWORD"};
  const auto mask=std::uint32_t(1)<<bit;
  const Rule* rules=nullptr;
  unsigned count=0;
  switch(p) {
    case Policy::DynamicCode: rules=dynamicRules; count=4; break;
    case Policy::ControlFlowGuard: rules=cfgRules; count=5; break;
    case Policy::UserShadowStack: rules=shadowRules; count=10; break;
  }
  for(unsigned i=0;i<count;++i) if(rules[i].mask==mask) return rules[i];
  return {mask, "ReservedOrUnknown", Decision::UnknownRefuse, "no_supported_bit_contract"};
}
struct Value { bool known; std::uint32_t flags; };
struct Result {
  Policy policy;
  Value input;
  std::uint32_t relevant{}, irrelevant{}, unknown{};
  bool admitted{};
};
inline Result classify(Policy p, Value input) {
  Result r{p,input};
  if(!input.known) return r;
  if(static_cast<unsigned>(p)>2) { r.unknown=input.flags; return r; }
  for(unsigned bit=0;bit<32;++bit) {
    const auto q=rule(p,bit);
    if(!(input.flags&q.mask)) continue;
    switch(q.decision) {
      case Decision::RelevantRefuse: r.relevant|=q.mask; break;
      case Decision::Irrelevant: r.irrelevant|=q.mask; break;
      case Decision::UnknownRefuse: r.unknown|=q.mask; break;
    }
  }
  r.admitted=(r.relevant|r.unknown)==0;
  return r;
}
using Inputs=std::array<Value,3>;
inline bool admitted(const Inputs& values) {
  for(unsigned i=0;i<3;++i) if(!classify(static_cast<Policy>(i),values[i]).admitted) return false;
  return true;
}
// Only fixed ASCII rule strings and DWORD/boolean values enter this JSON.
inline std::string json(const Inputs& values) {
  std::string out="{\"revision\":\"c42-v1\",\"admitted\":";
  out+=admitted(values)?"true":"false";
  out+=",\"policies\":[";
  for(unsigned i=0;i<3;++i) {
    const auto p=static_cast<Policy>(i);
    const auto r=classify(p,values[i]);
    if(i) out+=',';
    out+="{\"policy\":\""; out+=name(p); out+="\",\"query_known\":";
    out+=r.input.known?"true":"false";
    out+=",\"flags\":"+std::to_string(r.input.flags);
    out+=",\"decision\":\"";
    out+=!r.input.known?"QUERY_UNKNOWN_REFUSE":r.admitted?"ADMIT":"REFUSE";
    out+="\",\"relevant_refuse_mask\":"+std::to_string(r.relevant);
    out+=",\"irrelevant_mask\":"+std::to_string(r.irrelevant);
    out+=",\"unknown_refuse_mask\":"+std::to_string(r.unknown);
    out+=",\"enabled_bits\":[";
    bool first=true;
    for(unsigned bit=0;r.input.known&&bit<32;++bit) {
      const auto q=rule(p,bit);
      if(!(r.input.flags&q.mask)) continue;
      if(!first) out+=',';
      first=false;
      out+="{\"mask\":"+std::to_string(q.mask)+",\"name\":\""+q.name;
      out+="\",\"classification\":\""; out+=name(q.decision);
      out+="\",\"reason\":\""; out+=q.reason; out+="\"}";
    }
    out+="]}";
  }
  out+="]}";
  return out;
}
} // namespace ws::continuation_mitigations
