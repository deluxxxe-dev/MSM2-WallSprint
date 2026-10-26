#pragma once
// TEST ONLY. Observations and necessary-condition audit, NEVER an acceptance certificate.
#include "NativeHooks/debug_register_comparison.hpp"

namespace ws::dr6_lifecycle {
namespace d=ws::continuation_diagnostics;
namespace c=ws::debug_register_comparison;
enum class Phase : unsigned {
  Baseline,ForeignSet,ForeignResumeSaved,WatchSet,DbEntry,DbAfterEngine,
  PostReturn,AfterPoll,Verify1,Verify2,Verify3,Count
};
constexpr unsigned count=static_cast<unsigned>(Phase::Count);
inline constexpr const char* names[]={
  "baseline_pre_foreign","post_foreign_set_readback","post_foreign_resume_saved",
  "post_watch_set_readback_pre_resume","outer_db_handler_entry",
  "outer_db_handler_after_engine","post_return_pre_final_poll","after_final_poll",
  "verification_readback_1","verification_readback_2","verification_readback_3"
};
inline unsigned index(Phase p){return static_cast<unsigned>(p);}
inline bool exceptionPhase(unsigned p){return p==index(Phase::DbEntry)||p==index(Phase::DbAfterEngine);}
struct Observation {d::Registers registers{};unsigned sequence{},thread{};bool suspendedApi{};};
struct Timeline {
  std::array<std::optional<Observation>,count> stages{};
  unsigned worker{};bool observerError{};
};
inline bool completeOrdered(const Timeline& t) {
  if(!t.worker||t.observerError)return false;
  unsigned previous=0;
  for(unsigned i=0;i<count;++i) {
    if(!t.stages[i])return false;
    const auto& s=*t.stages[i];
    if(s.thread!=t.worker||s.sequence<=previous||s.suspendedApi==exceptionPhase(i))return false;
    previous=s.sequence;
  }
  return true;
}
struct Transition {
  const char* classification="UNRESOLVED_OR_MULTIPLE";
  const char* reason="incomplete_or_unordered_timeline";
  int first=-1;unsigned changes{};bool unique{},zeroToOne{};
};
inline Transition classify(const Timeline& t,unsigned bit) {
  Transition r;if(bit!=11&&bit!=16){r.reason="unsupported_status_bit";return r;}
  for(unsigned i=1;i<count;++i)if(t.stages[i-1]&&t.stages[i]) {
    const bool a=(t.stages[i-1]->registers[4]>>bit)&1,b=(t.stages[i]->registers[4]>>bit)&1;
    if(a!=b){if(r.first<0){r.first=int(i);r.zeroToOne=!a&&b;}++r.changes;}
  }
  if(!completeOrdered(t))return r;
  if(r.changes==0){r.classification="UNCHANGED";r.reason="all_observed_values_equal";r.unique=true;return r;}
  if(r.changes!=1){r.reason="multiple_observed_transitions";return r;}
  switch(static_cast<Phase>(r.first)) {
    case Phase::ForeignSet:case Phase::WatchSet:r.classification="SETTHREADCONTEXT_ROUNDTRIP";break;
    case Phase::DbEntry:r.classification="HARDWARE_DB_ENTRY";break;
    case Phase::DbAfterEngine:r.classification="HANDLER_EDIT";break;
    case Phase::PostReturn:r.classification="OS_RETURN_POST_CONTEXT";break;
    default:r.reason="unisolated_resume_or_post_poll_boundary";return r;
  }
  r.reason="unique_first_observed_boundary_not_underlying_causality";r.unique=true;return r;
}
// Necessary conditions for a FUTURE proof only. No RETIREMENT_STATUS_SAFE comparator
// is added/enabled in c45: native lifecycle/OS-view equivalence is still unmeasured.
struct Facts {
  c::Vendor vendor=c::Vendor::Unknown;
  bool mitigationAdmitted{},ordinaryOwnedDataDb{},engineClaimed{},canRetire{};
  bool activeAfter=true,pollReturned=true;
  unsigned pendingBefore{1},pendingAfter{1};
  d::Registers saved{};
};
struct Audit {bool necessaryConditions{};const char* reason="native_lifecycle_proof_pending";};
inline Audit audit(const Timeline& t,const Facts& f) {
  if(f.vendor!=c::Vendor::Intel)return {false,"Intel_profile_required"};
  if(!completeOrdered(t))return {false,"incomplete_or_unordered_timeline"};
  if(!f.mitigationAdmitted||!f.ordinaryOwnedDataDb||!f.engineClaimed)return {false,"ordinary_owned_DB_or_mitigation_unproven"};
  if(!f.canRetire||f.activeAfter||f.pollReturned||f.pendingBefore||f.pendingAfter)return {false,"lease_or_continuation_debt_not_retired"};
  if(!d::same(f.saved,t.stages[index(Phase::ForeignResumeSaved)]->registers)||!f.saved[2]||!(f.saved[5]&0x10))return {false,"saved_foreign_snapshot_identity_unproven"};
  for(const auto& s:t.stages)if(s->registers[5]&0x800)return {false,"Dr7_RTM_not_disabled"};
  for(const auto& s:t.stages)if(s->registers[4]&0xe00c)return {false,"foreign_debug_cause_in_lifecycle"};
  const auto entry=t.stages[index(Phase::DbEntry)]->registers[4];
  if(!(entry&3)||(entry&0x10800)!=0x10800)return {false,"owned_ordinary_DB_entry_status_contract_unproven"};
  if(t.stages[index(Phase::DbAfterEngine)]->registers[4]&3)return {false,"engine_did_not_clear_owned_cause"};
  for(unsigned bit:{11u,16u}) {
    const auto transition=classify(t,bit);
    if(!transition.unique||(transition.changes&&!transition.zeroToOne))return {false,"unresolved_multiple_or_wrong_direction_status_transition"};
  }
  const auto& after=t.stages[index(Phase::AfterPoll)]->registers;
  for(unsigned i=index(Phase::AfterPoll);i<count;++i) {
    const auto& current=t.stages[i]->registers;
    if(!d::same(current,after))return {false,"post_poll_raw_readbacks_not_stable"};
    for(unsigned reg=0;reg<4;++reg)if(current[reg]!=f.saved[reg])return {false,"Dr0_Dr3_or_foreign_Dr2_not_exact"};
    const auto m7=c::masks(c::Vendor::Intel,7),m6=c::masks(c::Vendor::Intel,6);
    if(((current[5]^f.saved[5])&(m7.meaningful|m7.osExact))||((current[5]|f.saved[5])&m7.unknown)||
       (current[5]&m7.one)!=m7.one||(current[5]&m7.zero))return {false,"Dr7_c44_contract_failed"};
    if((current[4]&0xe00f)||(f.saved[4]&0xe00f))return {false,"owned_or_foreign_debug_cause_present"};
    if((current[4]&0x10800)!=0x10800)return {false,"BLD_or_RTM_post_event_no_cause_contract_failed"};
    if((current[4]&m6.one)!=m6.one||(current[4]&m6.zero)||((current[4]|f.saved[4])&m6.unknown))return {false,"Dr6_fixed_or_unknown_contract_failed"};
  }
  return {true,"necessary_conditions_only_not_a_RETIREMENT_STATUS_SAFE_certificate"};
}
inline std::string transitionJson(const Timeline& t,unsigned bit) {
  const auto r=classify(t,bit);
  std::string out="{\"classification\":\""+std::string(r.classification)+"\",\"reason\":\""+r.reason+"\",\"unique\":"+d::boolean(r.unique)+",\"changes_observed\":"+std::to_string(r.changes)+",\"first_observed_stage\":";
  out+=r.first<0?"null":"\""+std::string(names[r.first])+"\"";
  out+=",\"transitions\":[";bool first=true;
  for(unsigned i=1;i<count;++i)if(t.stages[i-1]&&t.stages[i]) {
    const auto a=(t.stages[i-1]->registers[4]>>bit)&1,b=(t.stages[i]->registers[4]>>bit)&1;
    if(a==b)continue;
    if(!first)out+=',';
    first=false;
    out+="{\"from\":\""+std::string(names[i-1])+"\",\"to\":\""+names[i]+"\",\"before\":"+std::to_string(a)+",\"after\":"+std::to_string(b)+"}";
  }
  return out+"]}";
}
inline std::string json(const Timeline& t,const Facts& f) {
  const auto a=audit(t,f);
  std::string out="{\"revision\":\"c45-v1\",\"complete_ordered\":"+d::boolean(completeOrdered(t))+",\"observer_error\":"+d::boolean(t.observerError)+",\"worker\":"+std::to_string(t.worker);
  out+=",\"RETIREMENT_STATUS_SAFE_added\":false,\"necessary_conditions_only\":"+d::boolean(a.necessaryConditions)+",\"audit_reason\":\""+a.reason+"\",\"ordinary_owned_data_DB\":"+d::boolean(f.ordinaryOwnedDataDb)+",\"engine_claimed\":"+d::boolean(f.engineClaimed)+",\"stages\":[";
  for(unsigned i=0;i<count;++i) {
    if(i)out+=',';
    out+="{\"phase\":\""+std::string(names[i])+"\",\"captured\":"+d::boolean(bool(t.stages[i]));
    if(t.stages[i]) {
      const auto& s=*t.stages[i];out+=",\"sequence\":"+std::to_string(s.sequence)+",\"thread\":"+std::to_string(s.thread)+",\"origin\":\""+(s.suspendedApi?"suspended_GetThreadContext":"saved_exception_context")+"\",\"Dr6_bit11\":"+std::to_string((s.registers[4]>>11)&1)+",\"Dr6_bit16\":"+std::to_string((s.registers[4]>>16)&1)+",\"registers\":{";
      for(unsigned n=0;n<6;++n){if(n)out+=',';out+="\""+std::string(d::names[n])+"\":\""+d::hex(s.registers[n])+"\"";}out+='}';
    }
    out+='}';
  }
  return out+"],\"bit11\":"+transitionJson(t,11)+",\"bit16\":"+transitionJson(t,16)+"}";
}
} // namespace ws::dr6_lifecycle
