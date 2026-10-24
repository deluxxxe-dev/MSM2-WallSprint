#pragma once
#include <array>
#include <cstdint>
namespace ws::rdx {
inline constexpr std::uint32_t readIdentityTarget = 0x2cf7fe0;
// C24 reports floor-count527161; aligned bytes would be6325932. Finite budget:
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
struct ReadTargetIdentity {
  bool enabled{}, admitted{}, missingRuntime{}, stable{}, tableValid{};
  bool directoryPairValid{}, directoryRangeInImage{}, directorySizeDivisible{},
      tableBudgetAdmitted{}, entryCapExceeded{}, byteCapExceeded{},
      validationSkippedByCap{}, tableReadFailed{}, tableReadComplete{}, lookupQueried{};
  std::uint32_t target{}, codeBegin{}, codeEnd{}, runtimeBegin{}, runtimeEnd{},
      nextRuntime{}, tableEntries{}, scanBytes{}, size{}, instructions{}, barrier{};
  std::uint32_t directoryRva{}, directoryBytes{}, metadataBytesRead{}, validatedEntries{};
  RuntimeRegion lookup{};
  const char *method = "none", *reason = "target_identity_not_enabled";
  char mnemonic[32] = "none";
  std::array<unsigned char, 128> snapshot{};
};
ReadTargetIdentity authenticateReadTarget(bool buildAndCallVerified,
    std::uint32_t target, std::uint32_t imageSize, IdentityRead,
    IdentityLookup, void *context) noexcept;
}
