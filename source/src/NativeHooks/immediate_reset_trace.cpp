#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include "NativeHooks/immediate_reset_trace.hpp"
#include "NativeHooks/native_source_override.hpp"
#include "NativeHooks/source_lifecycle.hpp"
#include "msm2/orientation_trace.hpp"
#include "diagnostics.hpp"
namespace {
// One slot, no allocation or wait in the game callback. No worker lock is held
// while any thread is suspended by the reused hardware-watch engine.
HANDLE wake{},stopped{},helper{};std::atomic<bool> disabled{true};
std::atomic<unsigned> pending{0};msm2::reset_trace::Request slot;
bool live(const msm2::reset_trace::Request& r,msm2::Snapshot& s){
 ws::native_source::Token token{};
 return !disabled.load()&&msm2::native_source::observationGuardsAuthenticated()&&
  ws::native_source::lifecycle(token)&&token.phase==3&&GetTickCount64()>=token.updateMs&&GetTickCount64()-token.updateMs<=100&&token.generation==r.generation&&token.owner==r.owner&&
  msm2::sample(s)&&s.hero==r.hero&&s.transform==r.transform&&ws::diag::actorAxes(s.prefix).valid;
}
DWORD WINAPI run(void*){
 HANDLE handles[]={stopped,wake};
 for(;;){
  if(WaitForMultipleObjects(2,handles,FALSE,INFINITE)!=WAIT_OBJECT_0+1)break;
  if(pending.load(std::memory_order_acquire)!=2)continue;
  auto r=slot;pending.store(0,std::memory_order_release);msm2::Snapshot s{};
  if(!live(r,s)){msm2::writeLog("RESET_TRACE_SKIPPED session=%llu reason=inactive_or_identity_or_guard_loss",static_cast<unsigned long long>(r.session));continue;}
  if(!msm2::trace::beginReset(r,s))continue;
  do {if(WaitForSingleObject(stopped,1)==WAIT_OBJECT_0)break;}while(msm2::trace::pollReset(live(r,s)));
  msm2::trace::pollReset(false);
 }
 msm2::trace::pollReset(false);return 0;
}
}
namespace msm2::reset_trace {
void configure(bool verified,std::uintptr_t base,std::size_t size){
 if(helper)return; // No rearming or configuration mutation within a process.
 if(!trace::configureReset(verified,base,size))return;
 wake=CreateEventW(nullptr,FALSE,FALSE,nullptr);stopped=CreateEventW(nullptr,TRUE,FALSE,nullptr);
 if(wake&&stopped){disabled=false;helper=CreateThread(nullptr,0,run,nullptr,0,nullptr);}
 if(!helper){disabled=true;trace::kill();writeLog("RESET_TRACE_DISABLED reason=helper_creation_failed");}
}
void request(const Request& r) noexcept {
 if(disabled.load()||!r.commitQpc)return;
 unsigned expected=0;if(!pending.compare_exchange_strong(expected,1,std::memory_order_acq_rel))return;
 slot=r;pending.store(2,std::memory_order_release);SetEvent(wake);
}
void writerEntry(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t caller,bool overridden) noexcept {
 if(disabled.load())return;
 auto error=GetLastError();ws::native_source::Token token{};bool known=ws::native_source::lifecycle(token);trace::resetEntry(rcx,rdx,r8,r9,caller,overridden,known?token.updates:0,known?token.generation:0,known?token.owner:0);SetLastError(error);
}
void kill() noexcept{disabled=true;if(stopped)SetEvent(stopped);trace::kill();}
void stop(){kill();if(helper){WaitForSingleObject(helper,INFINITE);CloseHandle(helper);helper=nullptr;}/* event handles retained: an admitted callback may still SetEvent */}
}
