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
struct Writer {
  bool decoded{}, prefix{}, xyz{}, w{}, wConstant{};
  int wSourceReg{};
  std::int64_t wSourceOffset{};
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
// process memory, follows calls, narrows branches from equality or runs a
// fixpoint.
Writer branchWriter(const unsigned char *, std::size_t,
                    const std::array<unsigned, 4> &exactStores) noexcept;
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
