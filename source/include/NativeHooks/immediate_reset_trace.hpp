#pragma once
#include <cstdint>
#include <cstddef>
#include "wallsprint.hpp"
namespace msm2::reset_trace {
struct Request {
 std::uint64_t session{},generation{},nativeUpdate{},commitQpc{};
 std::uintptr_t hero{},transform{},owner{};unsigned thread{};
 ws::Vec3 normal{},appliedUp{};
};
void configure(bool verified,std::uintptr_t base,std::size_t size);
void request(const Request&) noexcept;
void writerEntry(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t caller,bool overridden) noexcept;
void kill() noexcept;
void stop();
}
