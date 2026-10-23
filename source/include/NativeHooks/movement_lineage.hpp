#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
namespace ws::lineage {
constexpr unsigned maxNodes = 256, maxStores = 8;
enum class Op : unsigned char {
  unknown,
  gpr,
  xmm,
  constant,
  load,
  add,
  sub,
  mul
};
struct Expr {
  Op op{};
  unsigned a{}, b{}, reg{}, lane{};
  std::int64_t offset{};
  std::uint32_t bits{};
};
struct Store {
  unsigned offset{}, after{}, component{}, value{}, base{}, sourceReg{},
      sourceLane{};
  bool vector{};
  std::array<unsigned, 16> liveGpr{};
  std::array<unsigned, 64> liveXmm{};
};
struct Proof {
  bool decoded{}, complete{}, sourceMap{}, returned{}, exhausted{};
  unsigned bytes{}, failure{}, unsupported{}, firstUnsupported{}, storeCount{},
      mask{}, nodes{1};
  std::array<unsigned, 16> unsupportedOffsets{};
  const char *reason = "missing_function";
  std::array<Expr, maxNodes> expr{};
  std::array<Store, maxStores> stores{};
};
struct Call {
  bool decoded{}, verified{};
  unsigned offset{}, failure{};
  const char *reason = "missing_caller";
};
Proof writer(const unsigned char *, std::size_t) noexcept;
Call callsite(const unsigned char *, std::size_t, std::uint64_t begin,
              std::uint64_t ret, std::uint64_t target) noexcept;
struct Caller {
  bool decoded{}, sliceComplete{};
  unsigned barrier{}, unsupported{}, producerOffset{};
  const char *construction = "unknown";
  const char *shape = "unknown";
  const char *reason = "missing_caller";
  Proof slice{};
  std::array<unsigned, 4> payload{};
};
Caller caller(const unsigned char *, std::size_t, unsigned callOffset,
              const Proof &) noexcept;
void describe(const Proof &, unsigned, char *, std::size_t) noexcept;
struct Context {
  std::array<std::uint64_t, 16> gpr{};
  std::array<float, 64> xmm{};
  bool floating{};
};
using Read = bool (*)(std::uintptr_t, void *, std::size_t);
struct Sample {
  bool found{}, target{}, readable{}, stable{}, match{}, fullRow{}, wObserved{},
      finalStore{};
  unsigned component{}, mask{};
  float payload[4]{};
};
// Evaluates only entry leaves recoverable from exact surviving post-instruction
// register aliases. Pointed values are a trap-time read, never an entry
// snapshot.
Sample sample(const Proof &, unsigned after, const Context &,
              std::uintptr_t transform, const float observed[4], Read) noexcept;
} // namespace ws::lineage
