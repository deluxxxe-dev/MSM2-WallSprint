#pragma once
#include <atomic>
#include <cstdint>
#include <cstddef>
namespace ws::capture {
constexpr std::size_t capacity=1024;
struct alignas(64) Slot {std::atomic<std::uint64_t> sequence;std::uint64_t tsc;std::uintptr_t self,caller;std::uint32_t kind;std::uint32_t thread;std::uint64_t pad[3];};
struct Event {std::uint64_t tsc;std::uintptr_t self,caller;std::uint32_t kind;std::uint32_t thread{};std::uint64_t faceTag{};};
static_assert(sizeof(Slot)==64&&offsetof(Slot,tsc)==8&&offsetof(Slot,self)==16&&offsetof(Slot,caller)==24&&offsetof(Slot,kind)==32&&offsetof(Slot,thread)==36);
static_assert(std::atomic<std::uint64_t>::is_always_lock_free);
void initializeQueue();
bool pop(Event&);
std::uint64_t dropped();
using FaceEventListener=void(*)(const Event&,double) noexcept;
extern std::atomic<FaceEventListener> faceEventListener;
void observeFaceEvent(const Event&,double);

}
extern "C" {
alignas(64) extern ws::capture::Slot ws_hook_slots[ws::capture::capacity];
extern std::atomic<std::uint64_t> ws_hook_head,ws_hook_dropped;
extern std::atomic<std::uint32_t> ws_hook_recording;
extern std::atomic<std::uint64_t> ws_face_wr_diagnostic_tag;
extern void* ws_hook_originals[4];
void ws_hook_init();void ws_hook_entry();void ws_hook_update();void ws_hook_deinit();
}
