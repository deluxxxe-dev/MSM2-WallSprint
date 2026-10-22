#include "msm2/hook_capture.hpp"
extern "C" {
alignas(64) ws::capture::Slot ws_hook_slots[ws::capture::capacity];
std::atomic<std::uint64_t> ws_hook_head{0},ws_hook_dropped{0};
std::atomic<std::uint32_t> ws_hook_recording{0};
void* ws_hook_originals[4]{};
}
namespace ws::capture {
namespace {std::uint64_t tail{};}
void initializeQueue(){// Exactly once before the first native patch; never reset while a thunk may execute.
 for(std::size_t i=0;i<capacity;++i)ws_hook_slots[i].sequence.store(i,std::memory_order_relaxed);
 tail=0;ws_hook_head.store(0);ws_hook_dropped.store(0);
}
bool pop(Event& out){
 auto& slot=ws_hook_slots[tail&(capacity-1)];
 if(slot.sequence.load(std::memory_order_acquire)!=tail+1)return false;
 out={slot.tsc,slot.self,slot.caller,slot.kind,slot.thread};
 slot.sequence.store(tail+capacity,std::memory_order_release);++tail;return true;
}
std::uint64_t dropped(){return ws_hook_dropped.load(std::memory_order_relaxed);}
}

extern "C" {
std::atomic<unsigned> ws_source_lifecycle_enabled{0};
void (*ws_source_lifecycle_callback)(unsigned,std::uintptr_t) noexcept = nullptr;
}
