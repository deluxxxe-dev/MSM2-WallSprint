#pragma once
// TEST ONLY. Refines c45 observations; never grants retirement acceptance.
#include "NativeHooks/dr6_lifecycle.hpp"

namespace ws::pre_access_lifecycle {
namespace l=ws::dr6_lifecycle;namespace d=ws::continuation_diagnostics;namespace c=ws::debug_register_comparison;
constexpr unsigned count=l::count+1,preIndex=4;
inline const char* name(unsigned i){return i==preIndex?"post_watch_resume_pre_access":l::names[i-(i>preIndex)];}
struct Gate {
  bool reached{},capturedSuspended{},capturedBeforeRelease{},releaseUnsignaled{},resumedAfterCapture{},activeLease{},ownedSlots{};
  unsigned arrivalThread{},lease{};std::uint64_t generation{};
  std::optional<unsigned> dbAtArrival,dbBeforeSuspend,dbAtCapture,dbAfterRead;
  std::optional<unsigned> machineEntriesAtCapture,ticketsAtCapture,pendingAtCapture,writesAtWatch,writesAtCapture;
};
struct Evidence {l::Timeline prior;std::optional<l::Observation> preAccess;Gate gate;unsigned expectedLease{};std::uint64_t expectedGeneration{};};
inline const std::optional<l::Observation>& stage(const Evidence& e,unsigned i){return i==preIndex?e.preAccess:e.prior.stages[i-(i>preIndex)];}
inline const char* gateReason(const Evidence& e) {
  const auto& g=e.gate;
  if(!g.reached||!g.capturedSuspended||!g.capturedBeforeRelease||!g.releaseUnsignaled||!g.resumedAfterCapture)return "gate_order_or_suspended_capture_unproven";
  if(!e.prior.worker||g.arrivalThread!=e.prior.worker||!e.expectedLease||g.lease!=e.expectedLease||!e.expectedGeneration||g.generation!=e.expectedGeneration)return "thread_lease_generation_mismatch";
  if(!g.activeLease||!g.ownedSlots)return "owned_watch_not_live_at_gate";
  for(const auto& v:{g.dbAtArrival,g.dbBeforeSuspend,g.dbAtCapture,g.dbAfterRead,g.machineEntriesAtCapture,g.ticketsAtCapture,g.pendingAtCapture})if(!v||*v)return "zero_DB_access_and_ticket_counts_unproven";
  if(!g.writesAtWatch||!g.writesAtCapture||*g.writesAtWatch!=1||*g.writesAtCapture!=*g.writesAtWatch)return "extra_or_unknown_context_write_before_gate";
  if(!e.preAccess||!e.prior.stages[3])return "pre_access_or_watch_readback_missing";
  const auto& a=*e.prior.stages[3];const auto& b=*e.preAccess;
  if(b.thread!=e.prior.worker||!b.suspendedApi||b.sequence<=a.sequence||e.prior.observerError)return "pre_access_identity_order_or_origin_unproven";
  for(unsigned i=0;i<4;++i)if(a.registers[i]!=b.registers[i])return "pre_access_address_or_foreign_Dr2_changed";
  const auto m=c::masks(c::Vendor::Intel,7);
  if(((a.registers[5]^b.registers[5])&(m.meaningful|m.osExact))||((a.registers[5]|b.registers[5])&(m.unknown|m.zero|0x800)))return "pre_access_Dr7_controls_or_profile_changed";
  if(b.registers[4]&(0xe00full|c::masks(c::Vendor::Intel,6).unknown|c::masks(c::Vendor::Intel,6).zero))return "pre_access_owned_foreign_or_unknown_Dr6_cause";
  return "none";
}
inline bool gateProven(const Evidence& e){return !std::strcmp(gateReason(e),"none");}
inline bool completeOrdered(const Evidence& e) {
  if(!gateProven(e)||!l::completeOrdered(e.prior))return false;
  unsigned previous=0;
  for(unsigned i=0;i<count;++i){const auto& s=stage(e,i);if(!s||s->sequence<=previous)return false;previous=s->sequence;}
  return true;
}
struct Transition {const char* classification="UNRESOLVED_OR_MULTIPLE";const char* reason="incomplete_or_ambiguous_pre_access_evidence";const char* firstBoundary="UNRESOLVED_OR_MULTIPLE";int first=-1;unsigned changes{};bool unique{};};
inline const char* boundary(unsigned i) {
  switch(i){case 1:case 3:return "SETTHREADCONTEXT_ROUNDTRIP";case 4:return "WATCH_ENABLED_RESUME_PRE_ACCESS";case 5:return "DEBUG_EXCEPTION_DELIVERY_BOUNDARY";case 6:return "HANDLER_EDIT";case 7:return "OS_RETURN_POST_CONTEXT";default:return "UNRESOLVED_OR_MULTIPLE";}
}
inline Transition classify(const Evidence& e,unsigned bit) {
  Transition r;if(bit!=11&&bit!=16){r.reason="unsupported_status_bit";return r;}
  for(unsigned i=1;i<count;++i)if(stage(e,i-1)&&stage(e,i)){
    const auto a=(stage(e,i-1)->registers[4]>>bit)&1,b=(stage(e,i)->registers[4]>>bit)&1;
    if(a!=b){if(r.first<0)r.first=int(i);++r.changes;}
  }
  if(!completeOrdered(e))return r;
  if(r.first>=0)r.firstBoundary=boundary(unsigned(r.first));
  if(!r.changes){r.classification="UNCHANGED";r.reason="all_observed_values_equal";r.unique=true;return r;}
  if(r.changes!=1){r.reason="multiple_observed_transitions";return r;}
  if(!std::strcmp(r.firstBoundary,"UNRESOLVED_OR_MULTIPLE")){r.reason="unisolated_resume_or_post_poll_boundary";return r;}
  const auto before=(stage(e,unsigned(r.first)-1)->registers[4]>>bit)&1,after=(stage(e,unsigned(r.first))->registers[4]>>bit)&1;
  if((r.first==4||r.first==5)&&(before!=0||after!=1)){r.reason="wrong_direction_for_pre_access_or_delivery_status";return r;}
  r.classification=r.firstBoundary;r.reason="unique_observed_context_boundary_not_hardware_causality";r.unique=true;return r;
}
// Only a negative guard layered over c45's NECESSARY-only audit. Never an
// acceptance result: even true cannot grant RETIREMENT_CONFIGURATION_SAFE.
inline l::Audit audit(const Evidence& e,const l::Facts& f) {
  if(!completeOrdered(e))return {false,"c46_gate_or_complete_timeline_unproven"};
  if(!classify(e,11).unique||!classify(e,16).unique)return {false,"c46_status_boundary_unresolved_or_multiple"};
  return l::audit(e.prior,f);
}
inline std::string transitionJson(const Evidence& e,unsigned bit) {
  const auto r=classify(e,bit);std::string out="{\"classification\":\""+std::string(r.classification)+"\",\"reason\":\""+r.reason+"\",\"unique\":"+d::boolean(r.unique)+",\"changes_observed\":"+std::to_string(r.changes)+",\"first_transition_boundary\":\""+r.firstBoundary+"\",\"first_observed_stage\":";
  out+=r.first<0?"null":"\""+std::string(name(unsigned(r.first)))+"\"";out+=",\"transitions\":[";bool first=true;
  for(unsigned i=1;i<count;++i)if(stage(e,i-1)&&stage(e,i)){
    const auto a=(stage(e,i-1)->registers[4]>>bit)&1,b=(stage(e,i)->registers[4]>>bit)&1;if(a==b)continue;
    if(!first)out+=',';
    first=false;
    out+="{\"from\":\""+std::string(name(i-1))+"\",\"to\":\""+name(i)+"\",\"before\":"+std::to_string(a)+",\"after\":"+std::to_string(b)+"}";
  }
  return out+"]}";
}
inline std::string number(std::optional<unsigned> n){return n?std::to_string(*n):"null";}
inline std::string json(const Evidence& e,const l::Facts& f) {
  const auto a=audit(e,f);const auto& g=e.gate;
  std::string out="{\"revision\":\"c46-v1\",\"complete_ordered\":"+d::boolean(completeOrdered(e))+",\"gate_proven\":"+d::boolean(gateProven(e))+",\"gate_reason\":\""+gateReason(e)+"\",\"RETIREMENT_CONFIGURATION_SAFE_added\":false,\"necessary_conditions_only\":"+d::boolean(a.necessaryConditions)+",\"audit_reason\":\""+a.reason+"\",\"gate\":{";
  out+="\"reached\":"+d::boolean(g.reached)+",\"captured_suspended\":"+d::boolean(g.capturedSuspended)+",\"captured_before_release\":"+d::boolean(g.capturedBeforeRelease)+",\"release_unsignaled_at_capture\":"+d::boolean(g.releaseUnsignaled)+",\"resumed_after_capture\":"+d::boolean(g.resumedAfterCapture)+",\"active_lease\":"+d::boolean(g.activeLease)+",\"owned_slots\":"+d::boolean(g.ownedSlots)+",\"arrival_thread\":"+std::to_string(g.arrivalThread)+",\"lease\":"+std::to_string(g.lease)+",\"generation\":"+std::to_string(g.generation);
  out+=",\"DB_events_at_arrival\":"+number(g.dbAtArrival)+",\"DB_events_before_suspend\":"+number(g.dbBeforeSuspend)+",\"DB_events_at_capture\":"+number(g.dbAtCapture)+",\"DB_events_after_read\":"+number(g.dbAfterRead)+",\"machine_entries_at_capture\":"+number(g.machineEntriesAtCapture)+",\"tickets_at_capture\":"+number(g.ticketsAtCapture)+",\"pending_at_capture\":"+number(g.pendingAtCapture)+",\"context_writes_at_watch\":"+number(g.writesAtWatch)+",\"context_writes_at_capture\":"+number(g.writesAtCapture)+"},\"stages\":[";
  for(unsigned i=0;i<count;++i){if(i)out+=',';const auto& s=stage(e,i);out+="{\"phase\":\""+std::string(name(i))+"\",\"captured\":"+d::boolean(bool(s));
    if(s){out+=",\"sequence\":"+std::to_string(s->sequence)+",\"thread\":"+std::to_string(s->thread)+",\"origin\":\""+(s->suspendedApi?"suspended_GetThreadContext":"saved_exception_context")+"\",\"Dr6_bit11\":"+std::to_string((s->registers[4]>>11)&1)+",\"Dr6_bit16\":"+std::to_string((s->registers[4]>>16)&1)+",\"registers\":{";for(unsigned n=0;n<6;++n){if(n)out+=',';out+="\""+std::string(d::names[n])+"\":\""+d::hex(s->registers[n])+"\"";}out+='}';}out+='}';
  }
  return out+"],\"bit11\":"+transitionJson(e,11)+",\"bit16\":"+transitionJson(e,16)+"}";
}
} // namespace ws::pre_access_lifecycle
