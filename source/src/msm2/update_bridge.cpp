#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "msm2/update_bridge.hpp"
extern "C" bool ws_guarded_call(void (*)(void*),void*);
namespace {
struct Unwind {CONTEXT context{};bool ok{};};
void unwind(void* raw){
 auto& u=*static_cast<Unwind*>(raw);DWORD64 image=0;
 auto* function=RtlLookupFunctionEntry(u.context.Rip,&image,nullptr);
 if(!function)return; // Custom/leaf callers have no proven argument extent.
 void* handler=nullptr;DWORD64 establisher=0;
 RtlVirtualUnwind(UNW_FLAG_NHANDLER,image,u.context.Rip,function,&u.context,&handler,&establisher,nullptr);
 u.ok=true;
}
}
extern "C" std::uint64_t ws_update_prepare(std::uintptr_t owner,ws::bridge::Frame* frame,void* outgoing) noexcept {
 // Standard CALL/RET wrapper, including CET shadow stacks and normal SEH unwind.
 // Clone only the current caller's bounded frame (homes + all outgoing args).
 // Unsupported/oversized/unreadable callers use the original tail-jump path.
 if(!frame||!outgoing)return 0;
 auto fail=[](const char* why){ws_write_bridge_reject(why);return std::uint64_t(0);};
 // AMD64 Windows NT_TIB.Self, same operation as NtCurrentTeb, avoiding
 // GCC14 false array-bounds warning in the MinGW intrinsic implementation.
 NT_TIB* tib=nullptr;__asm__ volatile("movq %%gs:0x30,%0":"=r"(tib));
 auto low=reinterpret_cast<std::uintptr_t>(tib->StackLimit),high=reinterpret_cast<std::uintptr_t>(tib->StackBase);
 auto sp=frame->rsp;
 if(sp<low||sp>high||high-sp<40||sp%16!=8)return fail("post_bridge_invalid_incoming_stack");
 std::uintptr_t caller=0;SIZE_T done=0;
 if(!ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(sp),&caller,8,&done)||done!=8||!caller)return fail("post_bridge_caller_unavailable");
 Unwind u;u.context.ContextFlags=CONTEXT_CONTROL|CONTEXT_INTEGER;
 u.context.Rip=caller-1;u.context.Rsp=sp+8;
 u.context.Rbx=frame->rbx;u.context.Rbp=frame->rbp;u.context.Rsi=frame->rsi;u.context.Rdi=frame->rdi;
 u.context.R12=frame->r12;u.context.R13=frame->r13;u.context.R14=frame->r14;u.context.R15=frame->r15;
 if(!ws_guarded_call(unwind,&u)||!u.ok)return fail("post_bridge_caller_unwind_unavailable");
 // Unwound RSP points just beyond the caller's return slot; exclude that slot.
 if(u.context.Rsp<sp+48||u.context.Rsp>high)return fail("post_bridge_caller_extent_invalid");
 auto bytes=u.context.Rsp-sp-16;
 if(bytes>ws::bridge::maxCallerBytes)return fail("post_bridge_caller_frame_exceeds_0x1000");
 // Do not overwrite the currently executing prepare callback's own 32-byte
 // home area. Assembly restores these four homes after this function returns.
 if(!ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(sp+40),static_cast<unsigned char*>(outgoing)+32,bytes-32,&done)||done!=bytes-32)return fail("post_bridge_argument_copy_failed");
 frame->owner=owner;
 return ws_write_update_pre(owner);
}
