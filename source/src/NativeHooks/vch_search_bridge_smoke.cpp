// TEST ONLY: c49 stage-2 boundary observation. No executable bridge is attempted
// before native dispatch/order evidence exists. Never linked into WallSprint.dll.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include <array>
#include <cassert>
static PVECTORED_EXCEPTION_HANDLER originalEngine{};
static LONG CALLBACK observeEngine(EXCEPTION_POINTERS*);
static PVOID WINAPI installObserved(ULONG first,PVECTORED_EXCEPTION_HANDLER fn) {
  assert(!originalEngine);originalEngine=fn;return AddVectoredExceptionHandler(first,observeEngine);
}
#define AddVectoredExceptionHandler installObserved
#include "../msm2/orientation_trace.cpp"
#undef AddVectoredExceptionHandler
#include <cpuid.h>
#include <cstdio>
#include <cstdarg>
#include <string>
#include "NativeHooks/continuation_mitigations.hpp"
#include "NativeHooks/continuation_diagnostics.hpp"

namespace c49 {
namespace mitigation=ws::continuation_mitigations;
namespace diag=ws::continuation_diagnostics;
constexpr const char* unavailable="VCH_SEARCH_CONTINUATION_BRIDGE_UNAVAILABLE";
constexpr const char* failed="VCH_SEARCH_CONTINUATION_BRIDGE_FEASIBILITY_FAIL";
constexpr std::uint64_t generation=149;
struct Stage {
  CONTEXT context{};
  std::uintptr_t contextAddress{},exceptionAddress{};
  DWORD thread{};
  unsigned order{};
};
struct Debt {
  DWORD thread{};std::uint64_t epoch{},lease{},gen{},sequence{};
  std::uintptr_t context{},exception{},originalRip{};
  unsigned reservedOrder{},escapeOrder{};
};
Stage wsEntry,wsAfter,foreignBefore,foreignAfter,vch;
Debt debt;
std::atomic<unsigned> clock{0},wsCalls{0},foreignCalls{0},vchCalls{0},pending{0};
std::atomic<bool> poisoned{false},downstreamFinal{false},boundaryMatches{false};
std::atomic<const char*> failure{nullptr};
HANDLE worker{},ready{},go{},done{},finish{},foreignHeld{},foreignRelease{},vchHeld{},vchRelease{};
DWORD workerId{};std::uint64_t epoch{};
msm2::Snapshot hero;
bool position{},timeout{},killClose{},wine{},jsonOnly{},registered{},admitted{},heldAtVch{},workerExited{},cleanupAfterExit{},cleanupRequested{};
std::string output,cpuVendor="unknown";
mitigation::Inputs policies{};
LONG wsDisposition=EXCEPTION_CONTINUE_SEARCH,foreignDisposition=EXCEPTION_CONTINUE_SEARCH;

void reject(const char* reason) {
  const char* empty=nullptr;failure.compare_exchange_strong(empty,reason);poisoned=true;
}
Stage capture(EXCEPTION_POINTERS* e) {
  Stage s;s.context=*e->ContextRecord;s.contextAddress=reinterpret_cast<std::uintptr_t>(e->ContextRecord);
  s.exceptionAddress=reinterpret_cast<std::uintptr_t>(e->ExceptionRecord);
  s.thread=GetCurrentThreadId();s.order=clock.fetch_add(1)+1;return s;
}
bool relevant(EXCEPTION_POINTERS* e) {
  return e&&e->ExceptionRecord&&e->ContextRecord&&GetCurrentThreadId()==workerId&&e->ExceptionRecord->ExceptionCode==EXCEPTION_SINGLE_STEP;
}
bool identity(const Stage& s) {
  return debt.thread==workerId&&s.thread==debt.thread&&debt.epoch==epoch&&epoch&&
         debt.gen==generation&&debt.lease==1&&debt.sequence==1&&
         s.contextAddress==debt.context&&s.exceptionAddress==debt.exception&&s.context.Rip==debt.originalRip;
}
std::string stageJson(const Stage& s) {
  if(!s.order)return "null";
  std::string out="{\"order\":"+std::to_string(s.order)+",\"thread\":"+std::to_string(s.thread);
  out+=",\"context\":\""+diag::hex(s.contextAddress)+"\",\"exception\":\""+diag::hex(s.exceptionAddress)+"\",\"Rip\":\""+diag::hex(s.context.Rip)+"\",\"EFlags\":\""+diag::hex(s.context.EFlags)+"\",\"ContextFlags\":\""+diag::hex(s.context.ContextFlags)+"\"";
  const diag::Registers r{s.context.Dr0,s.context.Dr1,s.context.Dr2,s.context.Dr3,s.context.Dr6,s.context.Dr7};
  for(unsigned i=0;i<6;++i)out+=",\""+std::string(diag::names[i])+"\":\""+diag::hex(r[i])+"\"";
  return out+"}";
}
void report(const char* result,const char* reason) {
  std::string d="null";
  if(debt.reservedOrder)d="{\"thread\":"+std::to_string(debt.thread)+",\"thread_creation_epoch\":"+std::to_string(debt.epoch)+",\"lease\":"+std::to_string(debt.lease)+",\"generation\":"+std::to_string(debt.gen)+",\"sequence\":"+std::to_string(debt.sequence)+",\"context\":\""+diag::hex(debt.context)+"\",\"exception\":\""+diag::hex(debt.exception)+"\",\"original_Rip\":\""+diag::hex(debt.originalRip)+"\",\"reserved_order\":"+std::to_string(debt.reservedOrder)+",\"SEARCH_escape_order\":"+std::to_string(debt.escapeOrder)+"}";
  std::printf("{\"schema\":1,\"task\":\"c49\",\"probe_stage\":\"boundary_observation_only\",\"result\":\"%s\",\"reason\":\"%s\",\"environment\":\"%s\",\"cpu_vendor\":\"%s\",\"mode\":\"%s\",\"scenario\":\"%s\",\"mitigation_classifier\":%s,\"prerequisites_admitted\":%s,\"WS_disposition\":\"%s\",\"downstream_disposition\":\"%s\",\"WS_calls\":%u,\"downstream_calls\":%u,\"VCH_registered\":%s,\"VCH_callback_count\":%u,\"stages\":{\"WS_entry\":%s,\"WS_after_engine\":%s,\"downstream_before\":%s,\"downstream_final\":%s,\"VCH\":%s},\"reserved_debt\":%s,\"ordered_final_context_observed\":%s,\"native_boundary_observed\":%s,\"VCH_to_witness_binding\":null,\"witness_acknowledgment\":null,\"pending_before_witness\":%u,\"pending_after_witness\":null,\"pending_at_worker_exit\":%u,\"can_retire_before_worker_exit\":false,\"poisoned\":%s,\"VCH_hold_exercised\":%s,\"cleanup_requested_while_pending\":%s,\"live_final_cleanup_performed\":false,\"cleanup_after_worker_exit\":%s,\"resurrection_after_final_cleanup\":null,\"bridge_implemented\":false,\"observer_Rip_writes\":0,\"observer_DR_repairs\":0,\"foreign_DR6_cause_clears\":%u,\"production_integrated\":false,\"exact_slot_watch_enabled\":false,\"game_used\":false}\n",
    result,reason,wine?"Wine":"native_Windows",cpuVendor.c_str(),position?"Position":"reset",timeout?"deadline":killClose?"kill":"close",mitigation::json(policies).c_str(),admitted?"true":"false",
    wsDisposition==EXCEPTION_CONTINUE_EXECUTION?"CONTINUE_EXECUTION":"CONTINUE_SEARCH",foreignDisposition==EXCEPTION_CONTINUE_EXECUTION?"CONTINUE_EXECUTION":"CONTINUE_SEARCH",wsCalls.load(),foreignCalls.load(),registered?"true":"false",vchCalls.load(),
    stageJson(wsEntry).c_str(),stageJson(wsAfter).c_str(),stageJson(foreignBefore).c_str(),stageJson(foreignAfter).c_str(),stageJson(vch).c_str(),d.c_str(),boundaryMatches?"true":"false",!wine&&boundaryMatches?"true":"false",pending.load(),pending.load(),poisoned?"true":"false",heldAtVch?"true":"false",cleanupRequested?"true":"false",cleanupAfterExit?"true":"false",foreignCalls.load());
}
[[noreturn]] void assertion(const char* expression,unsigned line) {
  std::fprintf(stderr,"c49 assertion line %u: %s\n",line,expression);report(failed,"owned_probe_assertion_failed");std::fflush(stdout);ExitProcess(3);
}
}
#undef assert
#define assert(condition) do {if(!(condition))c49::assertion(#condition,__LINE__);} while(false)
namespace msm2::native_source {bool diagnosticIdentity(const msm2::reset_trace::Request&) noexcept{return false;}}
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&got)&&got==n;}
bool sample(Snapshot& s){s=c49::hero;return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char b[4096];va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);c49::output+=b;c49::output+='\n';}
}
static LONG CALLBACK observeEngine(EXCEPTION_POINTERS* e) {
  using namespace c49;
  if(!relevant(e))return originalEngine(e);
  if(++wsCalls!=1){reject("additional_DB_or_nested_context_outside_single_debt_probe");return originalEngine(e);}
  wsEntry=capture(e);
  wsDisposition=originalEngine(e);wsAfter=capture(e);
  const auto& a=wsEntry.context;const auto& b=wsAfter.context;
  if(wsDisposition!=EXCEPTION_CONTINUE_SEARCH||(a.Dr6&0xe00f)!=5||(b.Dr6&0xe00f)!=4||
     (a.Dr6^b.Dr6)!=1||a.Rip!=b.Rip||a.Dr0!=b.Dr0||a.Dr1!=b.Dr1||a.Dr2!=b.Dr2||a.Dr3!=b.Dr3||a.Dr7!=b.Dr7||
     usedLeases!=1||own(b,leases[0])!=3||windows[0].id!=1||windows[0].reset.generation!=generation)reject("WS_mixed_SEARCH_identity_or_cause_mismatch");
  // Immutable reservation before this wrapper lets SEARCH escape. No CONTEXT
  // edit, RIP redirection, allocation or diagnostic-ring lock is introduced.
  debt={workerId,epoch,windows[0].id,generation,1,wsAfter.contextAddress,wsAfter.exceptionAddress,b.Rip,clock.fetch_add(1)+1,0};
  pending.store(1,std::memory_order_release);debt.escapeOrder=clock.fetch_add(1)+1;
  return wsDisposition;
}
static LONG CALLBACK downstreamOwner(EXCEPTION_POINTERS* e) {
  using namespace c49;if(!relevant(e))return EXCEPTION_CONTINUE_SEARCH;
  if(++foreignCalls!=1){reject("duplicate_downstream");return EXCEPTION_CONTINUE_SEARCH;}
  foreignBefore=capture(e);
  if(!identity(foreignBefore)||pending.load(std::memory_order_acquire)!=1||
     !(wsAfter.order<debt.reservedOrder&&debt.reservedOrder<debt.escapeOrder&&debt.escapeOrder<foreignBefore.order)||
     std::memcmp(&foreignBefore.context,&wsAfter.context,sizeof(CONTEXT)))reject("downstream_did_not_receive_exact_reserved_SEARCH_context");
  assert(SetEvent(foreignHeld));assert(WaitForSingleObject(foreignRelease,2000)==WAIT_OBJECT_0);
  // The separate synthetic foreign owner clears ONLY its own B2 cause. No
  // unrelated observer changes DR/RIP/flags/stack to manufacture VCH dispatch.
  e->ContextRecord->Dr6&=~4ull;
  foreignAfter=capture(e);foreignDisposition=EXCEPTION_CONTINUE_EXECUTION;
  auto expected=foreignBefore.context;expected.Dr6&=~4ull;
  if(std::memcmp(&expected,&foreignAfter.context,sizeof(CONTEXT)))reject("foreign_owner_changed_more_than_own_B2_cause");
  downstreamFinal.store(true,std::memory_order_release);return foreignDisposition;
}
static LONG CALLBACK continueObserver(EXCEPTION_POINTERS* e) {
  using namespace c49;if(!relevant(e))return EXCEPTION_CONTINUE_SEARCH;
  if(++vchCalls!=1){reject("duplicate_VCH");return EXCEPTION_CONTINUE_SEARCH;}
  vch=capture(e);
  const bool match=downstreamFinal.load(std::memory_order_acquire)&&identity(vch)&&pending.load()==1&&
    foreignDisposition==EXCEPTION_CONTINUE_EXECUTION&&foreignAfter.order<vch.order&&
    !std::memcmp(&vch.context,&foreignAfter.context,sizeof(CONTEXT))&&own(vch.context,leases[0])==3&&
    !(vch.context.Dr6&0xe00f);
  boundaryMatches=match;if(!match)reject("VCH_not_after_exact_downstream_final_context");
  // Deliberate test-owned hold, NOT a production handler pattern. Observation
  // stays allocation/format/ring-lock free. Stage3/RIP bridge remains absent.
  assert(SetEvent(vchHeld));assert(WaitForSingleObject(vchRelease,2000)==WAIT_OBJECT_0);
  return EXCEPTION_CONTINUE_SEARCH;
}
namespace c49 {
DWORD WINAPI runWorker(void*) {
  assert(SetEvent(ready));assert(WaitForSingleObject(go,2000)==WAIT_OBJECT_0);
  *reinterpret_cast<volatile unsigned*>(hero.transform+(position?0x30:0x10))=1;
  assert(SetEvent(done));assert(WaitForSingleObject(finish,2000)==WAIT_OBJECT_0);return 0;
}
CONTEXT debug() {
  assert(SuspendThread(worker)==0);CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;
  assert(GetThreadContext(worker,&c));assert(ResumeThread(worker)==1);return c;
}
bool environment() {
  wine=GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"wine_get_version")!=nullptr;
  unsigned a{},b{},c{},d{};if(__get_cpuid(0,&a,&b,&c,&d)){char bytes[13]{};std::memcpy(bytes,&b,4);std::memcpy(bytes+4,&d,4);std::memcpy(bytes+8,&c,4);cpuVendor=bytes;}
  using Query=BOOL(WINAPI*)(HANDLE,PROCESS_MITIGATION_POLICY,PVOID,SIZE_T);Query query{};
  auto raw=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"GetProcessMitigationPolicy");static_assert(sizeof(query)==sizeof(raw));std::memcpy(&query,&raw,sizeof(query));
  const PROCESS_MITIGATION_POLICY ids[]={ProcessDynamicCodePolicy,ProcessControlFlowGuardPolicy,ProcessUserShadowStackPolicy};
  for(unsigned i=0;i<3;++i){DWORD flags{};const bool known=query&&query(GetCurrentProcess(),ids[i],&flags,sizeof(flags));policies[i]={known,flags};}
  admitted=!IsDebuggerPresent()&&(cpuVendor=="GenuineIntel"||cpuVendor=="AuthenticAMD")&&mitigation::admitted(policies);
  return admitted;
}
}
int main(int argc,char** argv) {
  using namespace c49;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
  for(int i=1;i<argc;++i){if(!std::strcmp(argv[i],"--position"))position=true;else if(!std::strcmp(argv[i],"--timeout"))timeout=true;else if(!std::strcmp(argv[i],"--kill"))killClose=true;else if(!std::strcmp(argv[i],"--json"))jsonOnly=true;else if(!std::strcmp(argv[i],"--mixed-search-vch")){}else assert(false);}
  assert(!(timeout&&killClose));
  if(!environment()){report(unavailable,"prerequisite_unknown_or_refused_no_mitigation_changes");return 77;}
  auto* memory=static_cast<float*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);
  memory[0]=memory[5]=memory[10]=memory[15]=1;hero.hero=reinterpret_cast<std::uintptr_t>(memory+32);hero.transform=reinterpret_cast<std::uintptr_t>(memory);
  for(auto* h:{&ready,&go,&done,&finish,&foreignHeld,&foreignRelease,&vchHeld,&vchRelease}){*h=CreateEventW(nullptr,FALSE,FALSE,nullptr);assert(*h);}
  worker=CreateThread(nullptr,0,runWorker,nullptr,0,&workerId);assert(worker&&WaitForSingleObject(ready,2000)==WAIT_OBJECT_0);
  FILETIME creation{},exit{},kernel{},user{};assert(GetThreadTimes(worker,&creation,&exit,&kernel,&user));epoch=(std::uint64_t(creation.dwHighDateTime)<<32)|creation.dwLowDateTime;assert(epoch);
  auto foreign=debug();assert(!(foreign.Dr7&0xff));foreign.Dr2=hero.transform+(position?0x30:0x10);foreign.Dr7|=0xd000010ull;
  assert(SuspendThread(worker)==0);assert(SetThreadContext(worker,&foreign));assert(ResumeThread(worker)==1);
  const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
  assert(msm2::trace::configureReset(true,base,nt->OptionalHeader.SizeOfImage,true,true));
  auto downstream=AddVectoredExceptionHandler(0,downstreamOwner);assert(downstream);
  auto vchHandle=AddVectoredContinueHandler(0,continueObserver);registered=vchHandle!=nullptr;
  if(!registered){
    assert(SetEvent(go));assert(WaitForSingleObject(done,2000)==WAIT_OBJECT_0);assert(SetEvent(finish));assert(WaitForSingleObject(worker,2000)==WAIT_OBJECT_0);
    workerExited=true;msm2::trace::stop();cleanupAfterExit=!activeLeases();assert(RemoveVectoredExceptionHandler(downstream));
    report(unavailable,"VCH_registration_failed_no_dispatch_question_exercised");
    for(auto h:{worker,ready,go,done,finish,foreignHeld,foreignRelease,vchHeld,vchRelease})CloseHandle(h);
    assert(VirtualFree(memory,0,MEM_RELEASE));return 77;
  }
  msm2::reset_trace::Request r{};r.prearmed=r.verifyDual=r.multithread=true;r.initMs=GetTickCount64()-(position?1600:600);r.session=1;r.generation=generation;r.hero=hero.hero;r.transform=hero.transform;r.owner=hero.hero;r.thread=workerId;r.position=position;r.normal={0,0,1};r.basis={{-1,0,0},{0,0,1},{0,1,0},true};
  msm2::Snapshot snapshot{};assert(msm2::sample(snapshot));assert(position?msm2::trace::beginPosition(r,snapshot):msm2::trace::beginReset(r,snapshot));
  assert(SetEvent(go));assert(WaitForSingleObject(foreignHeld,2000)==WAIT_OBJECT_0);
  assert(pending==1&&wsCalls==1&&foreignCalls==1&&activeLeases()&&windows[0].hits==1&&windows[0].positionInflight==0);
  if(timeout){const auto left=windows[0].end-now();if(left>0)Sleep(DWORD(std::ceil(left))+1);assert(now()>=windows[0].end);}
  if(killClose)msm2::trace::kill();else msm2::trace::cancelReset();cleanupRequested=true;
  // Test-owned coordinator refuses the legacy final poll while reserved debt
  // lacks a witness. Close/deadline/kill never fabricates acknowledgment.
  assert(pending==1&&activeLeases());assert(SetEvent(foreignRelease));
  HANDLE signals[]={vchHeld,done};const auto observed=WaitForMultipleObjects(2,signals,FALSE,2000);
  assert(observed==WAIT_OBJECT_0||observed==WAIT_OBJECT_0+1);
  if(observed==WAIT_OBJECT_0){heldAtVch=true;assert(pending==1&&activeLeases());assert(SetEvent(vchRelease));assert(WaitForSingleObject(done,2000)==WAIT_OBJECT_0);}
  else assert(vchCalls==0);
  assert(pending==1&&activeLeases());const auto postReturn=debug();assert(own(postReturn,leases[0])==3&&postReturn.Dr2==foreign.Dr2);
  assert(SetEvent(finish));assert(WaitForSingleObject(worker,2000)==WAIT_OBJECT_0);workerExited=true;
  // No positive retirement/witness claim. Real retained-handle thread exit ends
  // this owned worker; unresolved observation debt is retained in the report.
  msm2::trace::stop();cleanupAfterExit=!activeLeases();assert(cleanupAfterExit);
  assert(RemoveVectoredContinueHandler(vchHandle));assert(RemoveVectoredExceptionHandler(downstream));
  const char* result=poisoned?failed:unavailable;
  const char* reason=poisoned?failure.load():!vchCalls?"registered_VCH_not_dispatched_for_mixed_SEARCH_DB":"boundary_observed_bridge_not_attempted_native_stage2_review_required";
  report(result,reason);
  if(!jsonOnly){if(poisoned)std::puts("FAIL: c49 observed boundary violated exact identity/order contract");else std::puts("UNAVAILABLE: c49 no native post-return bridge certificate; immutable reserved debt retained until owned worker exit");}
  for(auto h:{worker,ready,go,done,finish,foreignHeld,foreignRelease,vchHeld,vchRelease})CloseHandle(h);
  assert(VirtualFree(memory,0,MEM_RELEASE));return poisoned?3:0;
}
