#pragma once
#include <array>
#include <cstdint>
namespace ws::rdx {
inline constexpr std::uint32_t readIdentityTarget = 0x2cf7fe0;
struct RuntimeRegion {
  bool found{}, sameImage = true, readable = true;
  std::uint32_t begin{}, end{}, unwind{};
};
// Readers operate on owned fixtures or guarded RVAs of the admitted image.
using IdentityRead = bool (*)(void *, std::uint32_t, void *, unsigned);
using IdentityLookup = RuntimeRegion (*)(void *, std::uint32_t);
struct ReadTargetIdentity {
  bool enabled{}, admitted{}, missingRuntime{}, stable{}, tableValid{};
  std::uint32_t target{}, codeBegin{}, codeEnd{}, runtimeBegin{}, runtimeEnd{},
      nextRuntime{}, tableEntries{}, scanBytes{}, size{}, instructions{}, barrier{};
  RuntimeRegion lookup{};
  const char *method = "none", *reason = "target_identity_not_enabled";
  char mnemonic[32] = "none";
  std::array<unsigned char, 128> snapshot{};
};
ReadTargetIdentity authenticateReadTarget(bool buildAndCallVerified,
    std::uint32_t target, std::uint32_t imageSize, IdentityRead,
    IdentityLookup, void *context) noexcept;
}
