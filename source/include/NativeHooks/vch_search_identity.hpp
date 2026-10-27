#pragma once
// Test-owned c50 diagnostics only. No production ownership/cleanup admission.
#include <array>
#include <cstdint>
#include <string>
#include "NativeHooks/continuation_diagnostics.hpp"
namespace ws::vch_search_identity {
constexpr unsigned capacity=2064; // Existing production lease-table bound; not a debt bound.
struct Lease {
  unsigned index{},tid{},windowIndex{},windowId{},ownMask{};
  std::uint64_t generation{},up{};
  bool published{},metadataKnown{},ownKnown{},statusKnown{},installed{},suspended{};
};
struct Evidence {
  bool observed{},stableSnapshot{},search{},ripPreserved{};
  std::array<bool,5> registersPreserved{}; // Dr0/1/2/3/7, each independently.
  std::uint64_t entryCause{},postCause{},delta{},expectedGeneration{149},expectedUp{};
  unsigned worker{},usedTotal{},recordCount{},expectedWindow{1};
  bool legacyWindowKnown{};unsigned legacyWindowId{};std::uint64_t legacyWindowGeneration{};
  std::array<Lease,capacity> leases{};
};
struct Decision {
  unsigned workerCount{},ownedWorkerCount{},allThreadOwningCount{};
  int selected{-1};
  std::array<bool,14> legacy{};
  bool complete{},legacyAggregate{},onlyGlobalCountFailure{},uniqueMatching{},diagnosticAdmissible{};
};
inline constexpr const char* conjunctNames[]={
 "ws_search_ok","entry_cause_expected","post_engine_cause_expected","cause_delta_expected","rip_preserved",
 "Dr0_preserved","Dr1_preserved","Dr2_preserved","Dr3_preserved","Dr7_preserved",
 "global_used_leases_is_one","legacy_first_own_mask_is_three","legacy_window_zero_id_ok","legacy_window_zero_generation_ok"};
inline Decision evaluate(const Evidence& e) {
  Decision r;
  if(!e.observed)return r;
  r.complete=e.stableSnapshot&&e.usedTotal>0&&e.usedTotal<=capacity&&e.recordCount==e.usedTotal&&e.worker&&e.expectedUp;
  const Lease* selected=nullptr;
  for(unsigned i=0;i<e.recordCount&&i<capacity;++i){
    const auto& l=e.leases[i];
    if(l.index!=i||!l.metadataKnown||!l.ownKnown||!l.statusKnown||l.ownMask>3)r.complete=false;
    if(!l.published){if(l.tid||l.ownMask)r.complete=false;continue;}
    if(!l.tid){r.complete=false;continue;}
    if(l.ownMask)++r.allThreadOwningCount;
    if(l.tid!=e.worker)continue;
    ++r.workerCount;
    // Include partial ownership as a competing obligation; never select just
    // the full match and silently discard a second partial/tombstone match.
    if(l.ownMask){++r.ownedWorkerCount;selected=&l;r.selected=int(i);}
  }
  if(r.ownedWorkerCount!=1){selected=nullptr;r.selected=-1;}
  const auto& first=e.leases[0];
  r.legacy={e.search,e.entryCause==5,e.postCause==4,e.delta==1,e.ripPreserved,
    e.registersPreserved[0],e.registersPreserved[1],e.registersPreserved[2],e.registersPreserved[3],e.registersPreserved[4],
    e.usedTotal==1,e.recordCount>0&&first.ownKnown&&first.ownMask==3,
    e.legacyWindowKnown&&e.legacyWindowId==e.expectedWindow,
    e.legacyWindowKnown&&e.legacyWindowGeneration==e.expectedGeneration};
  r.legacyAggregate=true;bool others=true;
  for(unsigned i=0;i<r.legacy.size();++i){r.legacyAggregate&=r.legacy[i];if(i!=10)others&=r.legacy[i];}
  r.onlyGlobalCountFailure=others&&!r.legacy[10];
  // A separately named test property, not a replacement for production own().
  r.uniqueMatching=r.complete&&selected&&selected->tid==e.worker&&selected->ownMask==3&&
    selected->windowId==e.expectedWindow&&selected->windowIndex+1==selected->windowId&&
    selected->generation==e.expectedGeneration&&selected->up==e.expectedUp&&
    selected->installed&&!selected->suspended;
  r.diagnosticAdmissible=r.uniqueMatching&&others&&(r.legacyAggregate||r.onlyGlobalCountFailure);
  return r;
}
inline std::string json(const Evidence& e) {
  namespace d=ws::continuation_diagnostics;
  const auto r=evaluate(e);const auto* selected=r.selected>=0?&e.leases[unsigned(r.selected)]:nullptr;
  auto n=[&](std::uint64_t v){return e.observed?std::to_string(v):"null";};
  auto b=[&](bool v){return std::string(e.observed?(v?"true":"false"):"null");};
  std::string out="{\"revision\":\"c50-v1\",\"observed\":"+std::string(e.observed?"true":"false");
  out+=",\"entry_cause_mask\":"+n(e.entryCause)+",\"post_engine_cause_mask\":"+n(e.postCause)+",\"cause_delta\":"+n(e.delta);
  out+=",\"expected_entry_cause_mask\":5,\"expected_post_engine_cause_mask\":4,\"expected_cause_delta\":1";
  for(unsigned i=0;i<r.legacy.size();++i)out+=",\""+std::string(conjunctNames[i])+"\":"+b(r.legacy[i]);
  out+=",\"used_leases_total\":"+n(e.usedTotal)+",\"records_observed\":"+n(e.recordCount)+",\"matching_worker_lease_count\":"+n(r.workerCount)+",\"matching_owned_context_lease_count\":"+n(r.ownedWorkerCount)+",\"context_owning_lease_count_all_threads\":"+n(r.allThreadOwningCount);
  out+=",\"snapshot_complete_and_stable\":"+b(r.complete)+",\"expected_window_id\":"+std::to_string(e.expectedWindow)+",\"expected_generation\":"+std::to_string(e.expectedGeneration)+",\"expected_up\":\""+d::hex(e.expectedUp)+"\"";
  out+=",\"selected_lease_index\":"+(selected?std::to_string(selected->index):"null")+",\"selected_window_id\":"+(selected?std::to_string(selected->windowId):"null")+",\"selected_window_generation\":"+(selected?std::to_string(selected->generation):"null")+",\"selected_own_mask\":"+(selected?std::to_string(selected->ownMask):"null");
  out+=",\"legacy_window_zero_id\":"+(e.legacyWindowKnown?std::to_string(e.legacyWindowId):"null")+",\"legacy_window_zero_generation\":"+(e.legacyWindowKnown?std::to_string(e.legacyWindowGeneration):"null");
  out+=",\"legacy_c49_aggregate\":"+b(r.legacyAggregate)+",\"only_global_usedLeases_conjunct_failed\":"+b(r.onlyGlobalCountFailure)+",\"UNIQUE_MATCHING_DEBT_IDENTITY\":"+b(r.uniqueMatching)+",\"c50_diagnostic_admissible\":"+b(r.diagnosticAdmissible)+",\"legacy_failed_conjuncts\":[";
  bool comma=false;for(unsigned i=0;e.observed&&i<r.legacy.size();++i)if(!r.legacy[i]){if(comma)out+=',';comma=true;out+='"';out+=conjunctNames[i];out+='"';}
  out+="],\"leases\":[";
  for(unsigned i=0;i<e.recordCount&&i<capacity;++i){const auto& l=e.leases[i];if(i)out+=',';
    out+="{\"index\":"+std::to_string(l.index)+",\"tid\":"+std::to_string(l.tid)+",\"published\":"+d::boolean(l.published)+",\"window_index\":"+std::to_string(l.windowIndex)+",\"window_id\":"+(l.metadataKnown?std::to_string(l.windowId):"null")+",\"generation\":"+(l.metadataKnown?std::to_string(l.generation):"null")+",\"up\":\""+d::hex(l.up)+"\",\"own_mask\":"+(l.ownKnown?std::to_string(l.ownMask):"null")+",\"metadata_known\":"+d::boolean(l.metadataKnown)+",\"status_known\":"+d::boolean(l.statusKnown)+",\"installed\":"+(l.statusKnown?d::boolean(l.installed):"null")+",\"suspended\":"+(l.statusKnown?d::boolean(l.suspended):"null")+"}";
  }
  return out+"]}";
}
} // namespace ws::vch_search_identity
