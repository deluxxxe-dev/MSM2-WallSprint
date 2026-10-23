#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
namespace ws::compact_proof {
struct Pair {unsigned instruction{},source{},destination{};};
struct Result {bool admitted{};unsigned decodedBytes{},decodedInstructions{},pairs{};std::array<Pair,9> map{};const char* reason="not_checked";};
// WallSprint-owned symbolic verification over the existing bounded HDE decoder.
Result writer(const unsigned char*,std::size_t) noexcept;
bool caller(const unsigned char*,std::size_t,std::uintptr_t begin,std::uintptr_t target,std::uintptr_t returnRva) noexcept;
}
