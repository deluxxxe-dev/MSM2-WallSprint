#pragma once
// TEST ONLY: exact owned-thread snapshots, never a restoration policy.
#include <array>
#include <cstdint>
#include <optional>
#include <string>

namespace ws::continuation_diagnostics {
using Registers = std::array<std::uint64_t, 6>;
inline constexpr const char* names[] = {"Dr0", "Dr1", "Dr2", "Dr3", "Dr6", "Dr7"};
inline bool same(const Registers& a, const Registers& b) { return a == b; }
inline const char* classification(bool active, const Registers& saved, const Registers& current) {
  const bool equal = same(saved, current);
  return active ? (equal ? "LEASE_ONLY_FAILURE" : "LEASE_AND_DR_FAILURE")
                : (equal ? "ALL_RESTORED" : "DR_ONLY_FAILURE");
}
inline std::string hex(std::uint64_t value) {
  std::string out = "0x0000000000000000";
  for (unsigned i = 0; i < 16; ++i) { out[17-i] = "0123456789abcdef"[value & 15]; value >>= 4; }
  return out;
}
inline std::string boolean(std::optional<bool> value) {
  return value ? (*value ? "true" : "false") : "null";
}
inline std::string number(std::optional<unsigned> value) {
  return value ? std::to_string(*value) : "null";
}
inline std::string value(const std::optional<Registers>& registers, unsigned i) {
  return registers ? "\"" + hex((*registers)[i]) + "\"" : "null";
}
struct Report {
  bool syntheticInputs{};
  bool attempted{};
  std::optional<bool> activeBefore, activeAfter, pollReturned, canRetireBefore;
  std::optional<unsigned> pendingBefore, pendingAfter;
  std::optional<Registers> saved, before, after;
  std::array<std::optional<Registers>, 3> rechecks{};
};
inline std::string json(const Report& r) {
  const bool complete = r.saved && r.before && r.after && r.activeBefore &&
      r.activeAfter && r.pollReturned && r.canRetireBefore && r.pendingBefore && r.pendingAfter;
  const bool comparable = r.saved && r.after && r.activeAfter;
  std::string out = "{\"revision\":\"c43-v1\",\"comparison\":\"EXACT_UNMASKED\",\"synthetic_inputs\":";
  out += r.syntheticInputs ? "true" : "false";
  out += ",\"attempted\":"; out += r.attempted ? "true" : "false";
  out += ",\"capture_complete\":"; out += complete ? "true" : "false";
  out += ",\"active_leases_before_final_poll\":" + boolean(r.activeBefore);
  out += ",\"active_leases_after_final_poll\":" + boolean(r.activeAfter);
  out += ",\"final_poll_returned\":" + boolean(r.pollReturned);
  out += ",\"pending_contexts_before_final_poll\":" + number(r.pendingBefore);
  out += ",\"pending_contexts_after_final_poll\":" + number(r.pendingAfter);
  out += ",\"can_retire_before_final_poll\":" + boolean(r.canRetireBefore);
  out += ",\"final_predicate\":" + boolean(comparable ? std::optional<bool>(!*r.activeAfter && same(*r.saved,*r.after)) : std::nullopt);
  out += ",\"classification\":\"";
  out += comparable ? classification(*r.activeAfter,*r.saved,*r.after) : "NOT_OBSERVED";
  out += "\",\"registers\":{";
  for (unsigned i=0;i<6;++i) {
    if(i) out += ',';
    out += "\"" + std::string(names[i]) + "\":{\"saved\":" + value(r.saved,i);
    out += ",\"before\":" + value(r.before,i) + ",\"after\":" + value(r.after,i);
    out += ",\"before_equal\":" + boolean(r.saved && r.before ? std::optional<bool>((*r.saved)[i]==(*r.before)[i]) : std::nullopt);
    out += ",\"after_equal\":" + boolean(r.saved && r.after ? std::optional<bool>((*r.saved)[i]==(*r.after)[i]) : std::nullopt);
    out += ",\"after_xor\":";
    out += r.saved && r.after ? "\"" + hex((*r.saved)[i]^(*r.after)[i]) + "\"" : "null";
    out += '}';
  }
  out += "},\"verification_readbacks\":[";
  for(unsigned n=0;n<r.rechecks.size();++n) {
    if(n) out += ',';
    out += "{\"captured\":"; out += r.rechecks[n] ? "true" : "false";
    out += ",\"same_saved\":" + boolean(r.saved && r.rechecks[n] ? std::optional<bool>(same(*r.saved,*r.rechecks[n])) : std::nullopt);
    out += ",\"registers\":{";
    for(unsigned i=0;i<6;++i) { if(i) out += ','; out += "\""+std::string(names[i])+"\":"+value(r.rechecks[n],i); }
    out += "}}";
  }
  return out + "]}";
}
} // namespace ws::continuation_diagnostics
