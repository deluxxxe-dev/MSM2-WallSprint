// TEST ONLY: c51 one-debt Position/close VCH post-return feasibility.
// Production is unchanged; reviewed c41 stub storage lives until worker exit. Never linked into WallSprint.dll.
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
#include "NativeHooks/vch_search_identity.hpp"
#include "NativeHooks/c41_owned_stub.hpp"
#include "NativeHooks/vch_post_return_witness.hpp"

extern "C" {
alignas(64) unsigned char ws_c41_before[2048]{},ws_c41_after[2048]{};
alignas(32) unsigned char ws_c41_pattern[512]{};
volatile LONG ws_c41_captured{},ws_c41_finish{};
void ws_c41_machine(void*);
extern unsigned char ws_c41_after_store[];
}
namespace c51 {
namespace witness=ws::vch_post_return_witness;
namespace stub=ws::c41_owned_stub;
namespace comparison=ws::debug_register_comparison;
witness::Ledger ledger;stub::Ticket ticket;witness::Identity boundIdentity;
ws::continuation_diagnostics::Report restoration;witness::Configuration configuration;
std::atomic<bool> stubPublished{false},redirectCommitted{false},ripOnly{false},unsupported{false};
std::array<unsigned char,sizeof(CONTEXT)> changedBytes{};
std::uintptr_t preparedCode{};unsigned prepareOrder{},publicationOrder{},ackOrder{};
std::uint32_t observedToken{};bool tokenObserved{},debtAcknowledged{},cleanupAllowed{},configurationSafe{},resurrection{},machinePreserved{},storageRetained{},storageFreed{},helperOnly{};
unsigned cpuSignature{},leaf7B{},leaf7C{},leaf7D{};bool avxReady{},leaf7Known{};
namespace mitigation=ws::continuation_mitigations;
namespace diag=ws::continuation_diagnostics;
namespace ids=ws::vch_search_identity;
ids::Evidence identityEvidence;
int selectedLease=-1;unsigned frozenLeases{},frozenWindows{};
std::atomic<bool> coordinatorFrozen{false};
HANDLE unrelatedThread{},unrelatedReady{},unrelatedFinish{};bool unrelatedWorker{};
constexpr const char* unavailable="VCH_POST_RETURN_WITNESS_BRIDGE_UNAVAILABLE";
constexpr const char* failed="VCH_POST_RETURN_WITNESS_BRIDGE_FEASIBILITY_FAIL";
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
Stage wsEntry,wsAfter,foreignBefore,foreignAfter,vch,redirectedContext;
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
bool debtIdentity(const Stage& s) {
  return debt.thread==workerId&&s.thread==debt.thread&&debt.epoch==epoch&&epoch&&
         selectedLease>=0&&debt.gen==identityEvidence.leases[unsigned(selectedLease)].generation&&
         debt.lease==identityEvidence.leases[unsigned(selectedLease)].windowId&&debt.sequence==1&&
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
witness::Identity debtKey(){return {debt.thread,debt.epoch,debt.context,debt.exception,selectedLease>=0?std::uint64_t(selectedLease)+1:0,debt.lease,debt.gen,debt.sequence,debt.originalRip};}
std::string identityJson(const witness::Identity& id){
 if(!id.context)return "null";
 return "{\"thread\":"+std::to_string(id.thread)+",\"epoch\":"+std::to_string(id.epoch)+",\"context\":\""+diag::hex(id.context)+"\",\"exception\":\""+diag::hex(id.exception)+"\",\"lease\":"+std::to_string(id.lease)+",\"window\":"+std::to_string(id.window)+",\"generation\":"+std::to_string(id.generation)+",\"sequence\":"+std::to_string(id.sequence)+",\"original_Rip\":\""+diag::hex(id.originalRip)+"\"}";
}
std::string registersJson(const diag::Registers& r){std::string o="{";for(unsigned i=0;i<6;++i){if(i)o+=',';o+="\""+std::string(diag::names[i])+"\":\""+diag::hex(r[i])+"\"";}return o+"}";}
void report(const char* result,const char* reason) {
 const auto stability=comparison::stability((cpuVendor=="GenuineIntel"?comparison::Vendor::Intel:comparison::Vendor::Unknown),restoration);
 std::string o="{\"schema\":3,\"task\":\"c51\",\"result\":\""+std::string(result)+"\",\"reason\":\""+reason+"\",\"environment\":\""+(wine?"Wine":"native_Windows")+"\",\"cpu_vendor\":\""+cpuVendor+"\",\"mode\":\"Position\",\"scenario\":\"close\"";
 o+=",\"mitigation_classifier\":"+mitigation::json(policies)+",\"prerequisites_admitted\":"+diag::boolean(admitted)+",\"cpuid_signature\":"+std::to_string(cpuSignature)+",\"leaf7_known\":"+diag::boolean(leaf7Known)+",\"AVX_OSXSAVE_XCR0_admitted\":"+diag::boolean(avxReady);
 o+=",\"WS_disposition\":\""+std::string(wsDisposition==EXCEPTION_CONTINUE_SEARCH?"CONTINUE_SEARCH":"CONTINUE_EXECUTION")+"\",\"downstream_disposition\":\""+(foreignDisposition==EXCEPTION_CONTINUE_EXECUTION?"CONTINUE_EXECUTION":"CONTINUE_SEARCH")+"\",\"WS_calls\":"+std::to_string(wsCalls.load())+",\"downstream_calls\":"+std::to_string(foreignCalls.load())+",\"VCH_registered\":"+diag::boolean(registered)+",\"VCH_callback_count\":"+std::to_string(vchCalls.load());
 o+=",\"stages\":{\"WS_entry\":"+stageJson(wsEntry)+",\"WS_after_engine\":"+stageJson(wsAfter)+",\"downstream_before\":"+stageJson(foreignBefore)+",\"downstream_final\":"+stageJson(foreignAfter)+",\"VCH\":"+stageJson(vch)+",\"VCH_redirected\":"+stageJson(redirectedContext)+"}";
 o+=",\"ordered_final_context_observed\":"+diag::boolean(boundaryMatches)+",\"native_boundary_observed\":"+diag::boolean(!wine&&boundaryMatches)+",\"identity_diagnostics\":"+ids::json(identityEvidence)+",\"reserved_debt\":"+identityJson(debtKey());
 o+=",\"reservation_order\":"+std::to_string(debt.reservedOrder)+",\"SEARCH_escape_order\":"+std::to_string(debt.escapeOrder)+",\"debt_reserved\":"+diag::boolean(debt.reservedOrder!=0)+",\"debt_published\":"+diag::boolean(pending.load()!=0||debtAcknowledged);
 o+=",\"stub\":{\"prepared\":"+diag::boolean(preparedCode!=0)+",\"published\":"+diag::boolean(stubPublished)+",\"prepare_order\":"+std::to_string(prepareOrder)+",\"publication_order\":"+std::to_string(publicationOrder)+",\"identity\":"+identityJson(boundIdentity)+",\"original_Rip\":\""+diag::hex(ticket.original)+"\",\"redirected_Rip\":\""+diag::hex(preparedCode)+"\",\"c41_helpers_unchanged\":true}";
 o+=",\"VCH_context_fields_changed\":[";bool any=false;
 if(redirectCommitted){for(unsigned i=0;i<8;++i)any|=changedBytes[offsetof(CONTEXT,Rip)+i]!=0;if(any)o+="\"Rip\"";bool extra=false;for(unsigned i=0;i<sizeof(CONTEXT);++i)if((i<offsetof(CONTEXT,Rip)||i>=offsetof(CONTEXT,Rip)+8)&&changedBytes[i])extra=true;if(extra){if(any)o+=',';o+="\"UNKNOWN_EXTRA_CONTEXT_BYTES\"";}}
 o+="],\"VCH_extra_changed_byte_offsets\":[";bool comma=false;for(unsigned i=0;i<sizeof(CONTEXT);++i)if(redirectCommitted&&(i<offsetof(CONTEXT,Rip)||i>=offsetof(CONTEXT,Rip)+8)&&changedBytes[i]){if(comma)o+=',';comma=true;o+=std::to_string(i);}o+="],\"VCH_RIP_only\":"+diag::boolean(redirectCommitted?std::optional<bool>(ripOnly.load()):std::nullopt)+",\"VCH_to_witness_binding\":"+(stubPublished?identityJson(boundIdentity):"null");
 o+=",\"witness_token\":{\"expected\":"+(ticket.token?std::to_string(std::uint32_t(ticket.token)):"null")+",\"observed\":"+(tokenObserved?std::to_string(observedToken):"null")+",\"acknowledgment_identity\":"+(debtAcknowledged?identityJson(boundIdentity):"null")+",\"ack_order\":"+std::to_string(ackOrder)+"}";
 o+=",\"DEBT_ACKNOWLEDGED\":"+diag::boolean(debtAcknowledged)+",\"pending_before_witness\":"+std::to_string(debt.reservedOrder?1:0)+",\"pending_after_witness\":"+(debtAcknowledged?"0":"null")+",\"pending_at_worker_exit\":"+std::to_string(pending.load());
 o+=",\"cleanup_requested_before_witness\":"+diag::boolean(cleanupRequested)+",\"can_retire_before_worker_exit\":"+diag::boolean(cleanupAllowed)+",\"live_final_cleanup_performed\":"+diag::boolean(restoration.attempted)+",\"cleanup_after_worker_exit\":"+diag::boolean(cleanupAfterExit)+",\"VCH_hold_exercised\":"+diag::boolean(heldAtVch);
 o+=",\"final_restoration\":"+diag::json(restoration)+",\"debug_register_comparison\":"+comparison::reportJson((cpuVendor=="GenuineIntel"?comparison::Vendor::Intel:comparison::Vendor::Unknown),restoration);
 o+=",\"RAW_EXACT\":"+diag::boolean(stability.complete?std::optional<bool>(stability.rawAll):std::nullopt)+",\"ARCHITECTURAL_CANONICAL\":"+diag::boolean(stability.complete?std::optional<bool>(stability.canonicalAll):std::nullopt)+",\"c47_profile_applied\":false,\"configuration_contract\":\"c51_same_event_Intel_mixed_Position_close_v1\",\"same_event_configuration_restored\":"+diag::boolean(restoration.attempted?std::optional<bool>(configurationSafe):std::nullopt);
 o+=",\"post_return_debug\":"+(configuration.complete?registersJson(configuration.postReturn):"null")+",\"resurrection_after_final_cleanup\":"+diag::boolean(stability.complete?std::optional<bool>(resurrection):std::nullopt)+",\"second_watched_accesses\":0,\"machine_state_preserved\":"+diag::boolean(machinePreserved);
 o+=",\"storage_retained_until_worker_exit\":"+(preparedCode?diag::boolean(storageRetained):"null")+",\"STUB_EXECUTION_QUIESCENT\":"+(preparedCode?diag::boolean(workerExited):"null")+",\"STORAGE_RECLAIMABLE\":"+(preparedCode?diag::boolean(workerExited):"null")+",\"storage_freed_after_exit\":"+diag::boolean(storageFreed)+",\"poisoned\":"+diag::boolean(poisoned)+",\"bridge_implemented\":true,\"redirection_exercised\":"+diag::boolean(redirectCommitted)+",\"observer_DR_repairs\":0,\"foreign_DR6_cause_clears\":"+std::to_string(foreignCalls.load())+",\"unrelated_worker_requested\":"+diag::boolean(unrelatedWorker)+",\"production_integrated\":false,\"exact_slot_watch_enabled\":false,\"game_used\":false,\"BLOCKED\":0}";
 std::printf("%s\n",o.c_str());
}
[[noreturn]] void assertion(const char* expression,unsigned line) {
  std::fprintf(stderr,"c51 assertion line %u: %s\n",line,expression);report(failed,"owned_probe_assertion_failed");std::fflush(stdout);ExitProcess(3);
}
}
#undef assert
#define assert(condition) do {if(!(condition))c51::assertion(#condition,__LINE__);} while(false)
namespace msm2::native_source {bool diagnosticIdentity(const msm2::reset_trace::Request&) noexcept{return false;}}
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&got)&&got==n;}
bool sample(Snapshot& s){s=c51::hero;return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char b[4096];va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);c51::output+=b;c51::output+='\n';}
}
static LONG CALLBACK observeEngine(EXCEPTION_POINTERS* e) {
  using namespace c51;
  if(!relevant(e))return originalEngine(e);
  if(++wsCalls!=1){reject("additional_DB_or_nested_context_outside_single_debt_probe");return originalEngine(e);}
  wsEntry=capture(e);
  wsDisposition=originalEngine(e);wsAfter=capture(e);
  const auto& a=wsEntry.context;const auto& b=wsAfter.context;
  auto& d=identityEvidence;d.observed=true;d.search=wsDisposition==EXCEPTION_CONTINUE_SEARCH;
  d.entryCause=a.Dr6&0xe00f;d.postCause=b.Dr6&0xe00f;d.delta=a.Dr6^b.Dr6;d.ripPreserved=a.Rip==b.Rip;
  d.registersPreserved={a.Dr0==b.Dr0,a.Dr1==b.Dr1,a.Dr2==b.Dr2,a.Dr3==b.Dr3,a.Dr7==b.Dr7};
  d.worker=workerId;d.expectedUp=hero.transform+(position?0x30:0x10);
  d.legacyWindowKnown=usedWindows>0;
  if(d.legacyWindowKnown){d.legacyWindowId=windows[0].id;d.legacyWindowGeneration=windows[0].reset.generation;}
  // The owned coordinator finished begin/expansion before go and performs no
  // install/release while waiting for this capture. No production publication
  // or mutable lease flag is changed to manufacture a unique identity.
  d.usedTotal=usedLeases;d.recordCount=std::min(usedLeases,ids::capacity);
  d.stableSnapshot=coordinatorFrozen.load(std::memory_order_acquire)&&usedLeases==frozenLeases&&usedWindows==frozenWindows;
  for(unsigned i=0;i<d.recordCount;++i){
    const auto& l=leases[i];auto& t=d.leases[i];t.index=i;t.tid=l.tid.load(std::memory_order_acquire);t.published=t.tid!=0;
    t.windowIndex=l.window;t.up=l.up;t.metadataKnown=l.window<usedWindows&&l.window<windowsCap;
    if(t.metadataKnown){t.windowId=windows[l.window].id;t.generation=windows[l.window].reset.generation;t.ownMask=own(b,l);t.ownKnown=true;}
    t.statusKnown=d.stableSnapshot;if(t.statusKnown){t.installed=l.installed;t.suspended=l.suspended;}
  }
  const auto decision=ids::evaluate(d);selectedLease=decision.selected;
  if(b.Rip!=reinterpret_cast<std::uintptr_t>(ws_c41_after_store)||(b.EFlags&0x10100)||
     (b.ContextFlags&CONTEXT_CONTROL)!=CONTEXT_CONTROL||(b.ContextFlags&CONTEXT_INTEGER)!=CONTEXT_INTEGER){unsupported=true;reject("c41_exact_RIP_flags_or_CONTROL_INTEGER_guard_refused");}
  // All14 old conjuncts, the immutable lease snapshot and selected tuple have
  // now been captured independently, BEFORE any sticky diagnostic decision.
  if(!decision.diagnosticAdmissible)reject("decomposed_cause_preservation_or_unique_lease_conflict");
  // Immutable reservation before this wrapper lets SEARCH escape. No CONTEXT
  // edit, RIP redirection, allocation or diagnostic-ring lock is introduced.
  const auto* selected=selectedLease>=0?&identityEvidence.leases[unsigned(selectedLease)]:nullptr;
  debt={workerId,epoch,selected?selected->windowId:0,selected?selected->generation:0,1,wsAfter.contextAddress,wsAfter.exceptionAddress,b.Rip,clock.fetch_add(1)+1,0};
  pending.store(1,std::memory_order_release);debt.escapeOrder=clock.fetch_add(1)+1;
  return wsDisposition;
}
static LONG CALLBACK downstreamOwner(EXCEPTION_POINTERS* e) {
  using namespace c51;if(!relevant(e))return EXCEPTION_CONTINUE_SEARCH;
  if(++foreignCalls!=1){reject("duplicate_downstream");return EXCEPTION_CONTINUE_SEARCH;}
  foreignBefore=capture(e);
  if(!debtIdentity(foreignBefore)||pending.load(std::memory_order_acquire)!=1||
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
  using namespace c51;if(!relevant(e))return EXCEPTION_CONTINUE_SEARCH;
  if(++vchCalls!=1){reject("duplicate_VCH");return EXCEPTION_CONTINUE_SEARCH;}
  vch=capture(e);
  const bool match=downstreamFinal.load(std::memory_order_acquire)&&debtIdentity(vch)&&pending.load()==1&&
    foreignDisposition==EXCEPTION_CONTINUE_EXECUTION&&foreignAfter.order<vch.order&&
    !std::memcmp(&vch.context,&foreignAfter.context,sizeof(CONTEXT))&&selectedLease>=0&&own(vch.context,leases[unsigned(selectedLease)])==3&&
    !(vch.context.Dr6&0xe00f);
  boundaryMatches=match;if(!match)reject("VCH_not_after_exact_downstream_final_context");
  // Deliberate test-owned hold, NOT a production handler pattern. Observation
  // stays allocation/format/ring-lock free; preparation is coordinator-only.
  assert(SetEvent(vchHeld));assert(WaitForSingleObject(vchRelease,2000)==WAIT_OBJECT_0);
  if(stubPublished.load(std::memory_order_acquire)&&!poisoned.load()){
    MEMORY_BASIC_INFORMATION target{};
    const bool executable=VirtualQuery(reinterpret_cast<void*>(ticket.original),&target,sizeof(target))&&target.State==MEM_COMMIT&&(target.Protect==PAGE_EXECUTE_READ||target.Protect==PAGE_EXECUTE);
    if(!witness::same(debtKey(),boundIdentity)||ticket.thread!=boundIdentity.thread||ticket.watchLease!=boundIdentity.lease||
       ticket.gen!=boundIdentity.generation||ticket.sequence!=boundIdentity.sequence||ticket.original!=boundIdentity.originalRip||
       preparedCode!=reinterpret_cast<std::uintptr_t>(ticket.storage)||!stub::current(ticket)||!executable||stub::load(ticket.marker)!=0||
       std::memcmp(e->ContextRecord,&vch.context,sizeof(CONTEXT))){reject("published_stub_or_final_context_changed");return EXCEPTION_CONTINUE_SEARCH;}
    const auto before=*e->ContextRecord;
    e->ContextRecord->Rip=preparedCode; // The ONLY c51 saved-CONTEXT change.
    redirectedContext=capture(e);
    const auto* a=reinterpret_cast<const unsigned char*>(&before);const auto* b=reinterpret_cast<const unsigned char*>(&redirectedContext.context);
    bool inRip=false,extra=false;
    for(unsigned i=0;i<sizeof(CONTEXT);++i){changedBytes[i]=a[i]!=b[i];if(changedBytes[i]){if(i>=offsetof(CONTEXT,Rip)&&i<offsetof(CONTEXT,Rip)+8)inRip=true;else extra=true;}}
    ripOnly=inRip&&!extra;if(!ripOnly)reject("VCH_changed_context_beyond_RIP");
    redirectCommitted.store(true,std::memory_order_release);
  }
  return EXCEPTION_CONTINUE_SEARCH;
}
namespace c51 {
DWORD WINAPI runUnrelated(void*) {
  assert(SetEvent(unrelatedReady));assert(WaitForSingleObject(unrelatedFinish,2000)==WAIT_OBJECT_0);return 0;
}
DWORD WINAPI runWorker(void*) {
  assert(SetEvent(ready));assert(WaitForSingleObject(go,2000)==WAIT_OBJECT_0);
  ws_c41_machine(reinterpret_cast<void*>(hero.transform+0x30));
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
  unsigned a1{},b1{},c1{},d1{},a7{};
  const bool leaf1=__get_cpuid(1,&a1,&b1,&c1,&d1);cpuSignature=a1;
  leaf7Known=__get_cpuid_count(7,0,&a7,&leaf7B,&leaf7C,&leaf7D);
  if(leaf1&&(c1&bit_AVX)&&(c1&bit_OSXSAVE)){unsigned lo{},hi{};__asm__ volatile("xgetbv":"=a"(lo),"=d"(hi):"c"(0));avxReady=(lo&6)==6;}
  admitted=avxReady&&leaf7Known&&cpuVendor=="GenuineIntel"&&!IsDebuggerPresent()&&(cpuVendor=="GenuineIntel"||cpuVendor=="AuthenticAMD")&&mitigation::admitted(policies);
  return admitted;
}
}
static ws::continuation_diagnostics::Registers regs(const CONTEXT& c){return {c.Dr0,c.Dr1,c.Dr2,c.Dr3,c.Dr6,c.Dr7};}
static void stubContractTest(){
 using namespace c51;
 std::int32_t delta{};std::array<unsigned char,15> bytes{};
 assert(stub::relative(0x100000000,0x17fffffff,delta)&&delta==INT32_MAX);
 assert(stub::relative(0x100000000,0x80000000,delta)&&delta==INT32_MIN);
 assert(!stub::relative(0x100000000,0x180000000,delta));
 assert(!stub::encode(UINTPTR_MAX-10,4096,8192,1,bytes)&&!stub::encode(4096,4101,8192,1,bytes)&&!stub::encode(4096,8192,UINTPTR_MAX,1,bytes));
 stub::Ticket t;t.sequence=1;t.original=reinterpret_cast<std::uintptr_t>(ws_c41_after_store);assert(stub::prepare(t));
 auto bad=t;bad.bytes[0]=0x90;assert(!stub::decode(bad)&&!stub::current(bad));bad=t;++bad.original;assert(!stub::decode(bad));bad=t;bad.target[0]^=1;assert(!stub::current(bad));
 auto* rw=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);assert(rw);stub::Ticket wrong;wrong.sequence=1;wrong.original=reinterpret_cast<std::uintptr_t>(rw);assert(!stub::prepare(wrong)&&!wrong.storage);
 assert(VirtualFree(rw,0,MEM_RELEASE));assert(VirtualFree(t.storage,0,MEM_RELEASE)); // Never published/reachable by a worker.
 std::puts("PASS: c51 actual reviewed c41 helper range/overflow/near-RX-RW/current-code/decoder refusal contract; no VCH/native bridge evidence");
}
int main(int argc,char** argv) {
 using namespace c51;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
 position=true;
 for(int i=1;i<argc;++i){if(!std::strcmp(argv[i],"--position")||!std::strcmp(argv[i],"--mixed-search-vch")){}else if(!std::strcmp(argv[i],"--json"))jsonOnly=true;else if(!std::strcmp(argv[i],"--unrelated-worker"))unrelatedWorker=true;else if(!std::strcmp(argv[i],"--stub-contract-test"))helperOnly=true;else assert(false);}
 if(!environment()){report(unavailable,"c41_CPU_or_actual_mitigation_prerequisite_unavailable");return 77;}
 if(helperOnly){stubContractTest();return 0;}
 for(unsigned i=0;i<512;++i)ws_c41_pattern[i]=static_cast<unsigned char>(i*37+11);
 auto* memory=static_cast<float*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);
 memory[0]=memory[5]=memory[10]=memory[15]=1;hero.hero=reinterpret_cast<std::uintptr_t>(memory+32);hero.transform=reinterpret_cast<std::uintptr_t>(memory);
 for(auto* h:{&ready,&go,&done,&finish,&foreignHeld,&foreignRelease,&vchHeld,&vchRelease}){*h=CreateEventW(nullptr,h==&vchHeld?TRUE:FALSE,FALSE,nullptr);assert(*h);}
 worker=CreateThread(nullptr,0,runWorker,nullptr,0,&workerId);assert(worker&&WaitForSingleObject(ready,2000)==WAIT_OBJECT_0);
 FILETIME creation{},exit{},kernel{},user{};assert(GetThreadTimes(worker,&creation,&exit,&kernel,&user));epoch=(std::uint64_t(creation.dwHighDateTime)<<32)|creation.dwLowDateTime;assert(epoch);
 auto foreign=debug();assert(!(foreign.Dr7&0xff)&&!(foreign.Dr6&0xe00f));foreign.Dr2=hero.transform+0x30;foreign.Dr7|=0xd000010ull;
 assert(SuspendThread(worker)==0);assert(SetThreadContext(worker,&foreign));assert(ResumeThread(worker)==1);
 const auto saved=debug();restoration.saved=regs(saved);
 if(unrelatedWorker){unrelatedReady=CreateEventW(nullptr,FALSE,FALSE,nullptr);unrelatedFinish=CreateEventW(nullptr,FALSE,FALSE,nullptr);assert(unrelatedReady&&unrelatedFinish);unrelatedThread=CreateThread(nullptr,0,runUnrelated,nullptr,0,nullptr);assert(unrelatedThread&&WaitForSingleObject(unrelatedReady,2000)==WAIT_OBJECT_0);}
 const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
 assert(msm2::trace::configureReset(true,base,nt->OptionalHeader.SizeOfImage,true,true));
 auto downstream=AddVectoredExceptionHandler(0,downstreamOwner);assert(downstream);
 auto vchHandle=AddVectoredContinueHandler(0,continueObserver);registered=vchHandle!=nullptr;
 if(!registered){report(unavailable,"VCH_registration_unavailable");return 77;} // Process exit; no worker/stub was released or published.
 msm2::reset_trace::Request r{};r.prearmed=r.verifyDual=r.multithread=true;r.initMs=GetTickCount64()-1600;r.session=1;r.generation=generation;r.hero=hero.hero;r.transform=hero.transform;r.owner=hero.hero;r.thread=workerId;r.position=true;r.normal={0,0,1};r.basis={{-1,0,0},{0,0,1},{0,1,0},true};
 msm2::Snapshot snapshot{};assert(msm2::sample(snapshot));assert(msm2::trace::beginPosition(r,snapshot));
 frozenLeases=usedLeases;frozenWindows=usedWindows;coordinatorFrozen.store(true,std::memory_order_release);
 assert(SetEvent(go));assert(WaitForSingleObject(foreignHeld,2000)==WAIT_OBJECT_0);
 assert(pending==1&&wsCalls==1&&foreignCalls==1&&activeLeases()&&windows[0].hits==1&&windows[0].positionInflight==0);
 boundIdentity=debtKey();if(!ledger.reserve(boundIdentity,unsigned(debt.reservedOrder),unsigned(debt.escapeOrder)))reject("debt_reservation_or_identity_refused");
 assert(SetEvent(foreignRelease));
 const auto waitEnd=GetTickCount64()+2000;
 while(WaitForSingleObject(vchHeld,0)!=WAIT_OBJECT_0&&stub::load(&ws_c41_captured)==0&&GetTickCount64()<waitEnd)SwitchToThread();
 heldAtVch=WaitForSingleObject(vchHeld,0)==WAIT_OBJECT_0;
 if(heldAtVch){assert(vchCalls.load()==1);}
 else assert(stub::load(&ws_c41_captured)==1&&vchCalls==0);
 // The only positive close request is made with real VCH held. Wine's absence
 // remains observation-only, pending1, no final live-thread cleanup.
 msm2::trace::cancelReset();cleanupRequested=true;assert(pending==1&&activeLeases()&&!ledger.canCleanup());
 if(heldAtVch){
   const auto selected=ids::evaluate(identityEvidence);
   if(!ledger.observeFinal(boundIdentity,foreignAfter.order,vch.order,selected.diagnosticAdmissible,boundaryMatches.load(),true))reject("VCH_final_identity_or_order_refused");
   if(!poisoned.load()){
     ticket.thread=workerId;ticket.sequence=1;ticket.watchLease=unsigned(boundIdentity.lease);ticket.gen=generation;ticket.original=vch.context.Rip;
     if(!stub::prepare(ticket)){unsupported=true;ledger.refuse();reject("reviewed_c41_prepare_range_or_page_guard_unavailable");}
     else {
       preparedCode=reinterpret_cast<std::uintptr_t>(ticket.storage);prepareOrder=clock.fetch_add(1)+1;storageRetained=true;
       if(!ledger.publish(boundIdentity,std::uint32_t(ticket.token),true,stub::current(ticket),true,c51::admitted))reject("stub_publication_refused");
       else {publicationOrder=clock.fetch_add(1)+1;stubPublished.store(true,std::memory_order_release);}
     }
   }
   assert(pending==1&&!ledger.canCleanup());assert(SetEvent(vchRelease));
   if(stubPublished.load()){
     const auto end=GetTickCount64()+2000;
     while(!redirectCommitted.load(std::memory_order_acquire)&&!poisoned.load()&&GetTickCount64()<end)SwitchToThread();
     if(!redirectCommitted.load()||!ledger.redirect(boundIdentity,ripOnly.load(),boundaryMatches.load()))reject("RIP_only_redirect_not_observed");
     while(stub::load(ticket.marker)==0&&!poisoned.load()&&GetTickCount64()<end)SwitchToThread();
     observedToken=std::uint32_t(stub::load(ticket.marker));tokenObserved=true;
     if(!poisoned.load()&&ledger.acknowledge(debtKey(),observedToken,stub::current(ticket))){debtAcknowledged=true;ackOrder=clock.fetch_add(1)+1;pending.store(0,std::memory_order_release);}else reject("exact_post_return_token_not_acknowledged");
   }
 }
 if(debtAcknowledged&&!poisoned.load()){
   cleanupAllowed=ledger.canCleanup();assert(cleanupAllowed&&pending==0&&ticket.storage&&!workerExited);
   const auto before=debug();restoration.attempted=true;restoration.before=regs(before);restoration.activeBefore=activeLeases();restoration.pendingBefore=pending.load();restoration.canRetireBefore=cleanupAllowed;
   restoration.pollReturned=msm2::trace::pollReset(false,"c51_exact_VCH_post_return_token");restoration.activeAfter=activeLeases();restoration.pendingAfter=pending.load();
   const auto after=debug();restoration.after=regs(after);resurrection=selectedLease>=0&&own(after,leases[unsigned(selectedLease)])!=0;
   for(unsigned i=0;i<3;++i){const auto check=debug();restoration.rechecks[i]=regs(check);resurrection|=selectedLease>=0&&own(check,leases[unsigned(selectedLease)])!=0;}
   const auto stability=comparison::stability(comparison::Vendor::Intel,restoration);
   configuration.nativeWindows=!wine;configuration.intel=cpuVendor=="GenuineIntel";configuration.admitted=c51::admitted;configuration.boundAndAcknowledged=debtAcknowledged&&pending==0&&ledger.canCleanup();
   configuration.pollFalse=!*restoration.pollReturned;configuration.leasesInactive=!*restoration.activeAfter;configuration.stable=stability.complete&&stability.rawStable;configuration.complete=stability.complete;
   configuration.saved=*restoration.saved;configuration.entry=regs(wsEntry.context);configuration.afterWS=regs(wsAfter.context);configuration.finalVch=regs(vch.context);configuration.postReturn=regs(before);configuration.after=regs(after);
   for(unsigned i=0;i<3;++i)configuration.readbacks[i]=*restoration.rechecks[i];
   configurationSafe=witness::configurationRestored(configuration);
   if(!configurationSafe||resurrection)reject("same_event_configuration_restoration_or_resurrection_failure");
 }
 const auto capturedEnd=GetTickCount64()+2000;while(stub::load(&ws_c41_captured)==0&&GetTickCount64()<capturedEnd)SwitchToThread();assert(stub::load(&ws_c41_captured)==1);
 machinePreserved=!std::memcmp(ws_c41_before,ws_c41_after,128)&&!std::memcmp(ws_c41_before+128,ws_c41_after+128,8)&&!std::memcmp(ws_c41_before+160,ws_c41_after+160,512)&&!std::memcmp(ws_c41_before+672,ws_c41_after+672,512)&&!std::memcmp(ws_c41_before+1184,ws_c41_after+1184,64);
 if(!machinePreserved)reject("c41_machine_state_not_preserved");
 assert(windows[0].hits==1&&wsCalls==1&&foreignCalls==1);
 if(ticket.storage)assert(!workerExited&&WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
 InterlockedExchange(&ws_c41_finish,1);assert(WaitForSingleObject(done,2000)==WAIT_OBJECT_0);assert(SetEvent(finish));assert(WaitForSingleObject(worker,2000)==WAIT_OBJECT_0);workerExited=true;
 if(unrelatedThread){assert(SetEvent(unrelatedFinish));assert(WaitForSingleObject(unrelatedThread,2000)==WAIT_OBJECT_0);for(auto h:{unrelatedThread,unrelatedReady,unrelatedFinish})CloseHandle(h);}
 if(ticket.storage){assert(ledger.storageReclaimable(workerExited));assert(VirtualFree(ticket.storage,0,MEM_RELEASE));storageFreed=true;ticket.storage=nullptr;ticket.marker=nullptr;}
 msm2::trace::stop();cleanupAfterExit=!activeLeases();assert(cleanupAfterExit);
 assert(RemoveVectoredContinueHandler(vchHandle));assert(RemoveVectoredExceptionHandler(downstream));
 const bool nativePass=!wine&&boundaryMatches&&redirectCommitted&&ripOnly&&debtAcknowledged&&pending==0&&configurationSafe&&!resurrection&&machinePreserved&&storageRetained&&storageFreed&&!poisoned;
 const char* result=unsupported?unavailable:poisoned?failed:nativePass?"VCH_POST_RETURN_WITNESS_BRIDGE_FEASIBILITY_PASS":unavailable;
 const char* reason=unsupported?failure.load():poisoned?failure.load():nativePass?"native_exact_one_debt_post_return_token_then_configuration_cleanup":"VCH_callback_not_dispatched_no_bridge_or_cleanup_certificate";
 report(result,reason);
 if(!jsonOnly)std::printf("%s: c51 %s; test-only; no production integration\n",nativePass?"PASS":unsupported||!poisoned?"UNAVAILABLE":"FAIL",reason);
 for(auto h:{worker,ready,go,done,finish,foreignHeld,foreignRelease,vchHeld,vchRelease})CloseHandle(h);
 assert(VirtualFree(memory,0,MEM_RELEASE));return poisoned&&!unsupported?3:0;
}
