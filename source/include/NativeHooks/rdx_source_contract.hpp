#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
namespace ws::rdx {
// Bounded local slices, independent of the unchanged c12 analyzer.
// A certificate covers entry through the named stores, never the whole CFG.
inline constexpr unsigned writerByteCap = 183, blockCap = 16, edgeCap = 24,
                          branchCap = 4, pathCap = 16, stateCap = 31,
                          visitCap = 4096;
inline constexpr unsigned calleeByteCap = 1024;
// One exact call only. Target bytes are supplied by the authenticated runtime
// owner; this offline API never reads process memory or follows another edge.
struct CallEffect {
  bool instructionVerified{}, boundaryVerified{}, decoded{}, noWrite{},
      relevant{};
  std::uintptr_t writer{}, call{}, ret{}, target{}, begin{}, end{}, barrier{};
  unsigned gprClobber{}, xmmClobber{};
  std::int64_t privateStackLow{};
  const char *kind = "malformed/unresolved";
  const char *effect = "source_span_effect_unknown";
  const char *reason = "exact_call_not_authenticated";
  char mnemonic[32] = "none";
};
CallEffect exactCall(const unsigned char *, std::size_t,
                     std::uintptr_t writerRva, std::size_t imageSize,
                     unsigned callOffset, unsigned returnOffset) noexcept;
void classifyCallee(CallEffect &, const unsigned char *, std::size_t,
                    std::uintptr_t begin, std::uintptr_t end) noexcept;
struct Writer {
  bool decoded{}, prefix{}, xyz{}, w{}, wConstant{};
  int wSourceReg{};
  std::int64_t wSourceOffset{}, callStackLow{};
  unsigned mask{}, barrier{}, prefixEnd{};
  std::array<unsigned, 4> stores{};
  const char *wKind = "unknown";
  std::uint32_t wBits{};
  char barrierMnemonic[32] = "none";
  bool branchAware{}, bounded{}, exactSites{}, allRelevant{}, barrierValid{},
      barrierHasEdge{};
  unsigned blocks{}, edges{}, branches{}, blocksVisited{}, edgesVisited{},
      firstBranch{}, pathsExamined{}, statesScheduled{}, visits{},
      barrierEdge{}, componentProofMask{};
  std::array<unsigned, 4> reachingStates{};
  std::array<const char *, 3> paths{{"unproven", "unproven", "unproven"}};
  const char *reason = "missing_writer";
};
Writer writer(const unsigned char *,
              std::size_t) noexcept; // retained c14 straight-line API
// Enumerates every relevant forward local path under hard caps. Never reads
// process memory, follows new call edges, narrows branches from equality or
// runs a fixpoint. Optional call summary is conditional on the per-invocation
// entry RDX==entry RSP+0x28, nonstack Transform and callStackLow within
// the captured current-thread TEB stack bounds.
Writer branchWriter(const unsigned char *, std::size_t,
                    const std::array<unsigned, 4> &exactStores,
                    CallEffect *call = nullptr) noexcept;
struct Caller {
  bool decoded{}, prefix{}, rdxLocal{}, owned{}, constructed{};
  std::int64_t rspOffset{}, rdxOffset{};
  unsigned barrier{};
  std::array<unsigned, 3> producers{};
  char barrierMnemonic[32] = "none";
  const char *reason = "missing_caller";
};
Caller caller(const unsigned char *, std::size_t, unsigned callOffset) noexcept;
using Read = bool (*)(std::uintptr_t, void *, std::size_t);
struct Relation {
  bool decoded{}, reaches{};
  std::uintptr_t call{}, target{}, next{};
  unsigned hops{};
  const char *kind = "unresolved";
  const char *reason = "missing_anchored_call";
};
// Only an anchored CALL and up to two exact direct JMP thunks (E9/EB).
// Indirect dispatch records the concrete operand site, never an assumed target.
Relation relation(const unsigned char *, std::size_t, std::uintptr_t begin,
                  std::uintptr_t ret, std::uintptr_t consumer, Read) noexcept;
struct Storage {
  bool bounded{}, sameBytes{}, callerOwned{};
};
Storage storage(std::uintptr_t rsp, std::uintptr_t rdx, std::uintptr_t low,
                std::uintptr_t high, bool reads, bool sameBytes,
                const Caller &) noexcept;
} // namespace ws::rdx
