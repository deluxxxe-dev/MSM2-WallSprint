#pragma once
// TEST ONLY. Positive match of the owner c46 lifecycle; NOT a general DR6 mask.
#include "NativeHooks/pre_access_lifecycle.hpp"
#include "NativeHooks/continuation_mitigations.hpp"

namespace ws::retirement_configuration {
namespace p=ws::pre_access_lifecycle;namespace l=ws::dr6_lifecycle;
namespace d=ws::continuation_diagnostics;namespace c=ws::debug_register_comparison;namespace m=ws::continuation_mitigations;
inline constexpr const char* profileName="Intel_SDM253669_090US_DR6_owner_c46_Position_multiple_v1";
enum class Profile {Unknown,OwnerC46IntelPositionMultipleV1};
struct Ticket {unsigned thread{},sequence{},lease{};std::uint64_t generation{};bool acknowledged{},currentCode{};};
struct Inputs {
 Profile profile=Profile::Unknown;c::Vendor vendor=c::Vendor::Unknown;
 bool nativeWindows{},prerequisitesKnown{},cpuidKnown{},leaf7Known{},debuggerAbsent{},positionMultiple{},poisoned=true;
 unsigned signature{};m::Inputs mitigations{};
 p::Evidence lifecycle;d::Report restoration;
 bool ticketsCaptured{};unsigned ticketCount{};std::array<Ticket,3> tickets{};
 std::optional<unsigned> watchWrites,outerDbEvents,machineEntries,watchHits,totalWriteAttempts,finalPollWriteAttempts,unscopedWriteAttempts,failedWriteAttempts;
 bool ordinaryOwnedDataDb{},engineClaimed{};
};
struct Result {bool safe{};const char* reason="missing_evidence";};
inline bool dr7Final(std::uint64_t saved,std::uint64_t current) {
 const auto mask=c::masks(c::Vendor::Intel,7);
 return !((saved^current)&(mask.meaningful|mask.osExact))&&!((saved|current)&mask.unknown)&&
        (current&mask.one)==mask.one&&!(current&mask.zero);
}
inline Result compare(const Inputs& f) {
 if(!f.nativeWindows)return {false,"native_Windows_required_not_Wine"};
 if(f.profile!=Profile::OwnerC46IntelPositionMultipleV1||f.vendor!=c::Vendor::Intel||!f.prerequisitesKnown||!f.cpuidKnown||!f.leaf7Known||!f.signature||!f.debuggerAbsent||!f.positionMultiple)return {false,"unsupported_or_unknown_owner_profile"};
 if(!m::admitted(f.mitigations))return {false,"mitigation_refused_or_unknown"};
 for(unsigned i=0;i<3;++i)if(!f.mitigations[i].known||f.mitigations[i].flags!=(i==2?256u:0u))return {false,"mitigation_tuple_outside_measured_owner_profile"};
 const auto& e=f.lifecycle;const auto& r=f.restoration;
 if(!p::completeOrdered(e))return {false,"complete_c46_gate_and_order_required"};
 for(unsigned bit:{11u,16u}){const auto t=p::classify(e,bit);if(!t.unique||t.changes!=1||t.first!=4||std::strcmp(t.classification,"WATCH_ENABLED_RESUME_PRE_ACCESS"))return {false,"not_exact_single_pre_access_status_transition"};}
 if(!f.watchWrites||*f.watchWrites!=1||!f.outerDbEvents||*f.outerDbEvents!=1||!f.machineEntries||*f.machineEntries!=1||!f.watchHits||*f.watchHits!=1||!f.ordinaryOwnedDataDb||!f.engineClaimed)return {false,"owned_event_access_or_watch_identity_not_exact"};
 if(!f.totalWriteAttempts||*f.totalWriteAttempts!=2||!f.finalPollWriteAttempts||*f.finalPollWriteAttempts!=1||!f.unscopedWriteAttempts||*f.unscopedWriteAttempts||!f.failedWriteAttempts||*f.failedWriteAttempts)return {false,"extra_missing_failed_or_unscoped_context_write"};
 if(f.poisoned||!f.ticketsCaptured||f.ticketCount!=3)return {false,"continuation_ticket_capture_or_count_failed"};
 for(unsigned i=0;i<3;++i){const auto& t=f.tickets[i];if(t.thread!=e.prior.worker||t.sequence!=i+1||t.lease!=e.expectedLease||t.generation!=e.expectedGeneration||!t.acknowledged||!t.currentCode)return {false,"continuation_ticket_identity_ack_or_code_failed"};}
 if(!r.attempted||!r.activeBefore||!*r.activeBefore||!r.activeAfter||*r.activeAfter||!r.pollReturned||*r.pollReturned||!r.canRetireBefore||!*r.canRetireBefore||!r.pendingBefore||*r.pendingBefore||!r.pendingAfter||*r.pendingAfter)return {false,"lease_pending_canRetire_or_poll_failed"};
 const auto stable=c::stability(c::Vendor::Intel,r);
 if(!stable.complete||!stable.rawStable||!stable.canonicalResultsStable||!r.saved||!r.before)return {false,"missing_or_unstable_final_readbacks"};
 const auto& saved=*r.saved;
 if(!d::same(saved,e.prior.stages[2]->registers)||saved[4]!=0||saved[5]!=0x10||!saved[2])return {false,"saved_owner_representation_or_foreign_Dr2_mismatch"};
 if(!d::same(*r.before,e.prior.stages[6]->registers)||!d::same(*r.after,e.prior.stages[7]->registers))return {false,"restoration_and_lifecycle_snapshots_disagree"};
 for(unsigned i=0;i<3;++i)if(!d::same(*r.rechecks[i],e.prior.stages[8+i]->registers))return {false,"verification_and_lifecycle_snapshots_disagree"};
 // Match ALL observed DR6 values. No field is removed from the evidence or XOR.
 // Status is already 1/1 at zero-event pre-access, B0 only at the actual DB,
 // then no cause/residue/drift through handler, return, poll and three readbacks.
 for(unsigned i=0;i<p::count;++i){
  const auto& now=p::stage(e,i)->registers;
  const std::uint64_t expected6=i<4?0:i==5?0xffff0ff1ull:0xffff0ff0ull;
  if(now[4]!=expected6)return {false,"DR6_not_exact_observed_status_and_cause_lifecycle"};
  if(now[5]&0x800)return {false,"advanced_RTM_debug_control_enabled"};
  const auto mask=c::masks(c::Vendor::Intel,7);
  if(now[5]&(mask.unknown|mask.zero))return {false,"unknown_or_reserved_Dr7_profile"};
  const bool watched=i>=3&&i<=7;
  const auto& watch=e.prior.stages[3]->registers;
  const std::uint64_t expected7=watched?0x990015ull:i?0x10ull:0;
  if((now[5]^expected7)&(mask.meaningful|mask.osExact))return {false,"Dr7_owned_or_foreign_controls_not_exact"};
  for(unsigned reg=0;reg<4;++reg){
   const auto expected=watched&&reg<2?watch[reg]:reg==2&&i==0?e.prior.stages[0]->registers[2]:saved[reg];
   if(now[reg]!=expected)return {false,"Dr0_Dr3_or_foreign_Dr2_not_exact"};
  }
 }
 const auto& watch=e.prior.stages[3]->registers;
 if(!watch[0]||(watch[0]&3)||watch[0]>UINT64_MAX-8||watch[1]!=watch[0]+8)return {false,"owned_Position_slot_layout_unknown"};
 for(unsigned i=8;i<p::count;++i){
  const auto& now=p::stage(e,i)->registers;const auto mask=c::masks(c::Vendor::Intel,6);
  if(!dr7Final(saved[5],now[5])||(now[4]&mask.one)!=mask.one||(now[4]&mask.zero)||((saved[4]|now[4])&mask.unknown)||(now[4]&0xe00f))return {false,"c44_Dr7_or_DR6_fixed_unknown_cause_rule_failed"};
 }
 return {true,"exact_owner_c46_configuration_retired_status_representation_contract"};
}
inline std::string json(const Inputs& f) {
 const auto r=compare(f);std::string out="{\"revision\":\"c47-v1\",\"profile\":\""+std::string(profileName)+"\",\"scope\":\"owned_native_Position_multiple_only_not_production\",\"synthetic_inputs\":"+d::boolean(f.restoration.syntheticInputs)+",\"native_Windows\":"+d::boolean(f.nativeWindows)+",\"prerequisites_known\":"+d::boolean(f.prerequisitesKnown)+",\"cpuid_known\":"+d::boolean(f.cpuidKnown)+",\"leaf7_known\":"+d::boolean(f.leaf7Known)+",\"debugger_absent_at_admission\":"+d::boolean(f.debuggerAbsent)+",\"Position_multiple_profile\":"+d::boolean(f.positionMultiple)+",\"RETIREMENT_CONFIGURATION_SAFE\":"+d::boolean(r.safe)+",\"reason\":\""+r.reason+"\",\"ticket_capture_complete\":"+d::boolean(f.ticketsCaptured)+",\"ticket_count\":"+std::to_string(f.ticketCount)+",\"tickets\":[";
 for(unsigned i=0;i<f.ticketCount&&i<3;++i){if(i)out+=',';const auto& t=f.tickets[i];out+="{\"thread\":"+std::to_string(t.thread)+",\"sequence\":"+std::to_string(t.sequence)+",\"lease\":"+std::to_string(t.lease)+",\"generation\":"+std::to_string(t.generation)+",\"acknowledged\":"+d::boolean(t.acknowledged)+",\"current_code\":"+d::boolean(t.currentCode)+"}";}
 return out+"],\"total_context_write_attempts\":"+d::number(f.totalWriteAttempts)+",\"final_poll_context_write_attempts\":"+d::number(f.finalPollWriteAttempts)+",\"unscoped_context_write_attempts\":"+d::number(f.unscopedWriteAttempts)+",\"failed_context_write_attempts\":"+d::number(f.failedWriteAttempts)+"}";
}
} // namespace ws::retirement_configuration
