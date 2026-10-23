#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
namespace ws::compact_proof {
struct Pair {unsigned instruction{},source{},destination{};};
struct Result {bool admitted{};unsigned decodedBytes{},decodedInstructions{},pairs{},failureOffset{},decoderStatus{};std::array<Pair,9> map{};const char* reason="not_checked";};
inline constexpr const char* backend="Capstone_5.0.6_x86_64_static";
// Startup only: complete decode (skipdata OFF), followed by independent
// bounded symbolic verification. No decoder runs inside a writer hot hook.
Result writer(const unsigned char*,std::size_t) noexcept;
Result callerProof(const unsigned char*,std::size_t,std::uintptr_t begin,std::uintptr_t target,std::uintptr_t returnRva) noexcept;
bool caller(const unsigned char*,std::size_t,std::uintptr_t begin,std::uintptr_t target,std::uintptr_t returnRva) noexcept;
}
