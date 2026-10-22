#pragma once
#include <atomic>
#include <cstdint>
namespace ws::native_source {
struct Token {std::uintptr_t owner{};std::uint64_t generation{},initMs{},updateMs{};unsigned phase{},updates{},thread{},lastKind{};bool observed{};};
void armLifecycle(bool enabled);
bool lifecycle(Token&) noexcept;
using Observer=void (*)(const Token&) noexcept;
void setObserver(Observer) noexcept;
void event(unsigned kind,std::uintptr_t owner) noexcept;
}
extern "C" {
extern std::atomic<unsigned> ws_source_lifecycle_enabled;
extern void (*ws_source_lifecycle_callback)(unsigned,std::uintptr_t) noexcept;
}
