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
HANDLE wake{},stopped{},helper{};std::atomic<bool> disabled{true};bool processWatch{};
std::atomic<unsigned> pending{0};msm2::reset_trace::Request slot;
void nativeEvent(const ws::native_source::Token& token) noexcept {
 if(token.phase!=3){msm2::native_source::lifecycleClosed(token);msm2::trace::cancelReset();}
 else msm2::native_source::prearm(token);
}
bool live(const msm2::reset_trace::Request& r,msm2::Snapshot& s,const char** reason=nullptr){
 ws::native_source::Token token{};
 const char* failure=disabled.load()?"helper_disabled":!msm2::native_source::observationGuardsAuthenticated()?"observation_guards_lost":
  !ws::native_source::lifecycle(token)?"lifecycle_unavailable":token.phase!=3?"lifecycle_not_active":
  GetTickCount64()<token.updateMs||GetTickCount64()-token.updateMs>100?"native_update_not_fresh":
  token.generation!=r.generation?"generation_changed":token.owner!=r.owner?"owner_changed":
  !msm2::sample(s)?"hero_sample_failed":s.hero!=r.hero?"hero_changed":s.transform!=r.transform?"transform_changed":nullptr;
 if(reason)*reason=failure;
 return !failure;
}
DWORD WINAPI run(void*){
 HANDLE handles[]={stopped,wake};
 for(;;){
  if(WaitForMultipleObjects(2,handles,FALSE,INFINITE)!=WAIT_OBJECT_0+1)break;
  if(pending.load(std::memory_order_acquire)!=2)continue;
  auto r=slot;pending.store(0,std::memory_order_release);msm2::Snapshot s{};const char* reason=nullptr;
  if(!live(r,s,&reason)){msm2::writeLog("RESET_TRACE_SKIPPED session=%llu reason=%s",static_cast<unsigned long long>(r.session),reason);continue;}
  if(!msm2::trace::beginReset(r,s))continue;
  do {if(WaitForSingleObject(stopped,1)==WAIT_OBJECT_0)break;bool valid=live(r,s,&reason);if(!msm2::trace::pollReset(valid,valid?"none":reason))break;}while(true);
  msm2::trace::pollReset(false,"helper_stop");
 }
 msm2::trace::pollReset(false);return 0;
}
}
namespace msm2::reset_trace {
void configure(bool verified,std::uintptr_t base,std::size_t size,bool prearmed,bool multithread){
 if(helper)return; // No rearming or configuration mutation within a process.
 processWatch=multithread;
 if(!trace::configureReset(verified,base,size,prearmed,multithread)){
  writeLog("RESET_TRACE_SELF_CHECK prerequisite_gates_verified=%u common_writer_hook=%u reset_trace_enabled=1 reset_trace_admitted=0 primary_thread_prearm_supported=0",unsigned(verified),unsigned(msm2::native_source::observationGuardsAuthenticated()));
  writeLog("V01B7_DIAGNOSTIC_NOT_ADMITTED reason=build_hook_or_hardware_engine_gate");return;
 }
 wake=CreateEventW(nullptr,FALSE,FALSE,nullptr);stopped=CreateEventW(nullptr,TRUE,FALSE,nullptr);
 if(wake&&stopped){disabled=false;helper=CreateThread(nullptr,0,run,nullptr,0,nullptr);}
 if(!helper){disabled=true;trace::kill();writeLog("RESET_TRACE_DISABLED reason=helper_creation_failed");}
 ws::native_source::setObserver(nativeEvent);
 writeLog("RESET_TRACE_SELF_CHECK prerequisite_gates_verified=%u common_writer_hook=%u reset_trace_enabled=1 reset_trace_admitted=%u primary_thread_prearm_supported=%u",unsigned(verified),unsigned(msm2::native_source::observationGuardsAuthenticated()),unsigned(!disabled.load()),unsigned(helper!=nullptr));
 if(disabled.load())writeLog("V01B7_DIAGNOSTIC_NOT_ADMITTED reason=helper_creation_failed");
}
void request(const Request& r) noexcept {
 if(disabled.load()||(!r.prearmed&&!r.commitQpc))return;
 unsigned expected=0;if(!pending.compare_exchange_strong(expected,1,std::memory_order_acq_rel))return;
 slot=r;slot.multithread=processWatch;pending.store(2,std::memory_order_release);SetEvent(wake);
}
bool committed(const Request& r) noexcept {return !disabled.load()&&trace::resetCommitted(r);}
void writerEntry(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t caller,bool overridden) noexcept {
 if(disabled.load())return;
 auto error=GetLastError();ws::native_source::Token token{};bool known=ws::native_source::lifecycle(token);trace::resetEntry(rcx,rdx,r8,r9,caller,overridden,known?token.updates:0,known?token.generation:0,known?token.owner:0);SetLastError(error);
}
void kill() noexcept{disabled=true;if(stopped)SetEvent(stopped);trace::kill();}
void stop(){kill();if(helper){WaitForSingleObject(helper,INFINITE);CloseHandle(helper);helper=nullptr;}/* event handles retained: an admitted callback may still SetEvent */}
}
