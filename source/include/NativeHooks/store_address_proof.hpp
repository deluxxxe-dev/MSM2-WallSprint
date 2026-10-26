#pragma once
#include <cstdint>
#include <limits>
namespace ws::rdx {
// Instruction-derived numeric path fact, wholly separate from may-depend tags.
struct ReadOffsetRange {
  bool enabled{},predicateKnown{},fallthrough{},sourceUnchanged{},signProven{},shiftProven{},
      noWrap{},extentKnown{},currentProof{};
  unsigned shiftCount{},width{};
  std::uint64_t compareBound{},preMin{},preMax{},signMin{},signMax{},offsetMin{},offsetMax{},stride{},endExclusive{};
  const char *reason="range_not_enabled";
  bool proven() const {return predicateKnown && fallthrough && sourceUnchanged && signProven && shiftProven && noWrap && extentKnown;}
};
inline ReadOffsetRange beginReadOffsetRange(std::uint32_t immediate,bool taken) {
  ReadOffsetRange r{};r.enabled=r.predicateKnown=r.sourceUnchanged=true;r.fallthrough=!taken;r.compareBound=immediate;
  r.preMin=taken?std::uint64_t(immediate)+1:0;r.preMax=taken?UINT32_MAX:immediate;
  if (r.preMin>r.preMax) r.predicateKnown=false;
  r.reason=taken?"JA_unsigned_above_taken":"JA_unsigned_not_above_fallthrough";return r;
}
inline void invalidateReadOffsetRange(ReadOffsetRange &r) {
  if (!r.enabled) return;
  r.sourceUnchanged=r.signProven=r.shiftProven=r.extentKnown=r.noWrap=r.currentProof=false;
  r.reason="R8_replaced_outside_exact_range_sequence";
}
inline void signReadOffsetRange(ReadOffsetRange &r) {
  r.signProven=r.shiftProven=r.noWrap=r.extentKnown=r.currentProof=false;
  if (!r.predicateKnown || !r.fallthrough || !r.sourceUnchanged || r.preMax>INT32_MAX) {
    r.reason="MOVSXD_nonnegative_interval_unproven";return;
  }
  r.signMin=r.preMin;r.signMax=r.preMax;r.signProven=true;r.reason="MOVSXD_nonnegative_32_to_64_exact";
}
inline void shiftReadOffsetRange(ReadOffsetRange &r,unsigned count) {
  r.shiftProven=r.noWrap=r.extentKnown=r.currentProof=false;r.shiftCount=count;
  if (!r.signProven || !r.sourceUnchanged || count>=64 || r.signMax>(UINT64_MAX>>count)) {
    r.reason="SHL_interval_or_nonwrap_unproven";return;
  }
  r.stride=std::uint64_t(1)<<count;r.offsetMin=r.signMin<<count;r.offsetMax=r.signMax<<count;
  r.shiftProven=r.noWrap=true;r.reason="SHL_64_interval_nonwrapping_exact";
}
inline void extentReadOffsetRange(ReadOffsetRange &r,unsigned width) {
  r.extentKnown=r.currentProof=false;r.width=width;
  if (!r.shiftProven || !r.noWrap || !width || r.offsetMax>UINT64_MAX-width) {
    r.reason="store_end_extent_or_nonwrap_unproven";return;
  }
  r.endExclusive=r.offsetMax+width;r.extentKnown=true;r.reason="offset_range_proven_base_publication_still_unproven";
}
struct ReadSlotPublication {
  std::uint32_t slot{};
  bool slotMetadataKnown{},slotWritable{},xrefScanPerformed{},writerEnumerationComplete{},
      publicationSourceKnown{},allocationProven{},rangeProven{},lifetimeProven{},criticalDisjointnessProven{},baseProven{};
  std::uint64_t extentBytes{};bool addressAdditionNoWrap{};
  const char *sourceKind="opaque_loaded_qword_not_pointer";
  const char *reason="exact_slot_writer_provenance_not_enumerated_by_bounded_target_walk";
};
// Neither component alone certifies a full address. This task has no base-proof producer.
inline bool readAddressContained(const ReadOffsetRange &r,const ReadSlotPublication &b) {
  return r.proven() && r.currentProof && b.baseProven && b.writerEnumerationComplete && b.publicationSourceKnown &&
      b.allocationProven && b.rangeProven && b.lifetimeProven && b.criticalDisjointnessProven &&
      b.addressAdditionNoWrap && b.extentBytes>=r.endExclusive;
}
}
