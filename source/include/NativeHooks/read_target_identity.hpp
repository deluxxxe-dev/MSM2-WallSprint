#pragma once
#include <array>
#include <cstdint>
namespace ws::rdx {
inline constexpr std::uint32_t readIdentityTarget = 0x2cf7fe0;
// C25 actual complete table:527161 entries/6325932 bytes. Finite budget:
// <=600000 entries, <=8MiB, at most9375 guarded64-entry reads.
inline constexpr std::uint32_t readRuntimeTableEntryCap = 600000,
    readRuntimeTableByteCap = 8u*1024*1024;
struct RuntimeRegion {
  bool found{}, sameImage = true, readable = true;
  std::uint32_t begin{}, end{}, unwind{};
};
// Readers operate on owned fixtures or guarded RVAs of the admitted image.
using IdentityRead = bool (*)(void *, std::uint32_t, void *, unsigned);
using IdentityLookup = RuntimeRegion (*)(void *, std::uint32_t);
// C26 metadata/tag diagnostic only. Aliases of partial registers are described,
// never granted partial read/write semantics. Constants are instruction immediates.
struct ReadCmpContract {
  struct Operand {
    char kind[16] = "none", name[16] = "none", parentName[16] = "none",
        base[16] = "none", index[16] = "none", segment[16] = "none";
    unsigned width{}, access{}, parentTags{}, scale{};
    int parent = -1;
    std::int64_t immediate{}, displacement{};
    bool conservativeParent{}, valueTagsKnown{};
  };
  std::uint32_t site{}, flagsEntryGprMask{};
  unsigned operandCount{}, samples{}, flagsTags{};
  std::uint64_t decodedEflags{};
  bool decoded{}, formSupported{}, flagsOnly{}, hasMemory{}, modelApplied{};
  std::array<Operand, 2> operands{};
  const char *reason = "pinned_cmp_not_decoded";
};
struct ReadLeafBranch {
  std::uint32_t site{}, flagsProducer{}, entryCarrierMask{};
  unsigned samples{}, flagsTags{};
  bool decoded{}, conditional{}, flagsDependencyKnown{};
  char mnemonic[32] = "none";
  const char *condition = "unknown", *flagsRead = "unknown";
};
struct ReadTargetIdentity {
  bool enabled{}, admitted{}, missingRuntime{}, stable{}, tableValid{};
  bool directoryPairValid{}, directoryRangeInImage{}, directorySizeDivisible{},
      tableBudgetAdmitted{}, entryCapExceeded{}, byteCapExceeded{},
      validationSkippedByCap{}, tableReadFailed{}, tableReadComplete{}, lookupQueried{};
  std::uint32_t target{}, codeBegin{}, codeEnd{}, runtimeBegin{}, runtimeEnd{},
      nextRuntime{}, tableEntries{}, scanBytes{}, size{}, instructions{}, barrier{};
  std::uint32_t directoryRva{}, directoryBytes{}, metadataBytesRead{}, validatedEntries{};
  RuntimeRegion lookup{};
  ReadCmpContract cmp{};
  ReadLeafBranch branch{};
  const char *method = "none", *reason = "target_identity_not_enabled";
  char mnemonic[32] = "none";
  std::array<unsigned char, 128> snapshot{};
};
ReadTargetIdentity authenticateReadTarget(bool buildAndCallVerified,
    std::uint32_t target, std::uint32_t imageSize, IdentityRead,
    IdentityLookup, void *context) noexcept;
}
