#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "NativeHooks/source_lifecycle.hpp"
namespace {
std::atomic_flag gate=ATOMIC_FLAG_INIT;
ws::native_source::Token token;
std::atomic<ws::native_source::Observer> observer{nullptr};
std::atomic<bool> fault{false};std::atomic<std::uint64_t> contention{0};std::uint64_t cleanEpoch{};
}
namespace ws::native_source {
void setObserver(Observer fn) noexcept {observer.store(fn,std::memory_order_release);}
void armLifecycle(bool enabled){
 if(!enabled){ws_source_lifecycle_enabled=0;fault=true;return;}
 if(gate.test_and_set(std::memory_order_acquire))return;
 token={};fault=false;cleanEpoch=contention.load();gate.clear(std::memory_order_release);
 ws_source_lifecycle_callback=event;ws_source_lifecycle_enabled.store(1,std::memory_order_release);
}
bool lifecycle(Token& out) noexcept {
 out={};if(!ws_source_lifecycle_enabled.load()||fault.load()){out.observed=true;return false;}
 if(gate.test_and_set(std::memory_order_acquire))return false;
 out=token;out.observed=true;gate.clear(std::memory_order_release);return !fault.load()&&contention.load()==cleanEpoch&&out.phase==3&&out.updates>0;
}
void event(unsigned kind,std::uintptr_t owner) noexcept {
 if(!ws_source_lifecycle_enabled.load())return;
 auto error=GetLastError();auto entryEpoch=contention.load();
 if(gate.test_and_set(std::memory_order_acquire)){++contention;SetLastError(error);return;}
 auto time=GetTickCount64();token.lastKind=kind;
 if(kind==0){cleanEpoch=entryEpoch;++token.generation;token.owner=owner;token.initMs=time;token.updateMs=0;token.updates=0;token.phase=1;token.thread=GetCurrentThreadId();fault=false;}
 else if(token.owner!=owner){token.phase=0;fault=true;}
 else if(kind==1&&token.phase==1)token.phase=2;
 else if(kind==2&&(token.phase==2||token.phase==3)){token.phase=3;token.updateMs=time;++token.updates;token.thread=GetCurrentThreadId();}
 else if(kind==3){token.phase=0;++token.generation;}
 else {token.phase=0;fault=true;}
 auto observed=token;
 gate.clear(std::memory_order_release);
 if(auto fn=observer.load(std::memory_order_acquire))fn(observed);
 SetLastError(error);
}
}
