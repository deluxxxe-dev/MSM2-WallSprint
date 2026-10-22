#pragma once
#include <cstdint>
#include <cstddef>
namespace ws::bridge {
// Assembly layout. Nonvolatile values are captured before any local callback.
// The original caller frame bounds all Win64 outgoing stack arguments: no guessed
// signature or fixed number of argument slots is used.
constexpr std::size_t maxCallerBytes=0x1000;
struct Frame {
 std::uintptr_t rsp,rbx,rbp,rsi,rdi,r12,r13,r14,r15;
 std::uint64_t sequence;
 std::uintptr_t owner;
};
static_assert(offsetof(Frame,sequence)==0x48&&offsetof(Frame,owner)==0x50&&sizeof(Frame)==0x58);
}
extern "C" {
std::uint64_t ws_update_prepare(std::uintptr_t,ws::bridge::Frame*,void*) noexcept;
std::uint64_t ws_write_update_pre(std::uintptr_t) noexcept;
void ws_write_update_post(std::uintptr_t,std::uint64_t) noexcept;
void ws_write_bridge_reject(const char*) noexcept;
}
