#pragma once
#include <atomic>
#include <cstdint>
namespace ws::native_source {
enum class LifecycleRead : unsigned {Unknown,Active,Disabled,PublicationBusy,WriterCollision,InvalidSequence,InactivePhase,NoUpdates};
inline const char* lifecycleReadName(LifecycleRead s) noexcept {
 switch(s){case LifecycleRead::Active:return "coherent_active";case LifecycleRead::Disabled:return "disabled";case LifecycleRead::PublicationBusy:return "publication_busy";case LifecycleRead::WriterCollision:return "native_event_collision";case LifecycleRead::InvalidSequence:return "invalid_native_sequence";case LifecycleRead::InactivePhase:return "inactive_phase";case LifecycleRead::NoUpdates:return "no_native_updates";default:return "unknown";}
}
struct Token {std::uintptr_t owner{};std::uint64_t generation{},initMs{},updateMs{};unsigned phase{},updates{},thread{},lastKind{};bool observed{};LifecycleRead readStatus{};std::uint64_t acceptedEventEpoch{},collisionEpoch{},eventMs{};};
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
