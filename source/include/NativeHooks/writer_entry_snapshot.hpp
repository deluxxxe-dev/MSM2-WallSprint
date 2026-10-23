#pragma once
#include "NativeHooks/immediate_reset_trace.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
namespace msm2::writer_entry {
struct Frame {
  std::uintptr_t r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, rbx, rdx,
      rcx, rax, flags, ret;
};
static_assert(offsetof(Frame, ret) == 128 && offsetof(Frame, rcx) == 104);
using Gate = bool (*)(const reset_trace::Request &) noexcept;
struct Spec {
  bool verified{}, decoded{}, directCall{};
  std::uintptr_t base{};
  std::size_t imageSize{};
  unsigned begin{}, end{}, callerBegin{}, callerEnd{};
  const unsigned char *original{};
  const unsigned char *callerOriginal{};
  const char *callerHash{};
  const char *hash{};
  const char *caller2Relation = "unresolved";
  bool caller2Direct{};
  const unsigned char *caller2Original{};
  unsigned caller2Begin{}, caller2End{};
};
void configure(const Spec &, Gate) noexcept;
bool start(const reset_trace::Request
               &) noexcept; // helper only; before hardware leases
bool activate() noexcept;   // helper: only after Position hardware watch is
                            // published
void cancel() noexcept;     // no unhook or wait in native callbacks
void stop() noexcept;       // helper only; after hardware leases restored
bool originalIfOwned(std::uintptr_t, void *, std::size_t) noexcept;
std::uint64_t pair(std::uint64_t session, std::uint64_t generation,
                   std::uintptr_t target, std::uintptr_t entryRsp,
                   std::uintptr_t ret) noexcept;
void observe(std::uint64_t id, std::uintptr_t ripAfter,
             std::uintptr_t transform, const std::uint64_t *gpr,
             const unsigned char *xmm, bool xmmValid, const float position[4],
             std::uintptr_t ret, std::uintptr_t caller2,
             bool positionReadable = true) noexcept; // worker only
struct CaptureSummary {
  bool activated{};
  unsigned snapshots{}, complete{}, xyzMatches{}, candidate{27};
};
// Worker only, after event drain. Does not alter c12 admission/reconstruction.
CaptureSummary summary(std::uint64_t session) noexcept;
void drain() noexcept; // worker only
bool guardedRead(std::uintptr_t, void *, std::size_t) noexcept;
// Full bounded decode; wrapper only when a direct call lands on an exact E9
// tail jump to the containing caller. No guessed wrapper/indirect relation.
const char *
callerRelation(const unsigned char *, std::size_t, std::uintptr_t begin,
               std::uintptr_t ret, std::uintptr_t caller,
               bool (*read)(std::uintptr_t, void *, std::size_t)) noexcept;
} // namespace msm2::writer_entry
