#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <array>
#include <atomic>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <bcrypt.h>
#include "msm2/orientation_trace.hpp"
#include "diagnostics.hpp"
#include "Math/source_orientation.hpp"
#include "Math/movement_delta.hpp"
#include "NativeHooks/path_a_position_proof.hpp"
#include "NativeHooks/movement_lineage.hpp"
#include "NativeHooks/writer_entry_snapshot.hpp"
#include "NativeHooks/native_source_override.hpp"
extern "C" bool ws_guarded_call(void (*)(void*),void*);
namespace {
using namespace msm2;
constexpr unsigned windowsCap=16,threadsCap=128,leasesCap=windowsCap*(threadsCap+1),ringCap=1024;
constexpr DWORD64 ownMask=0xff000full,watchBits=0xd90005ull;
struct Entry {
 std::uint64_t id{},nativeUpdate{},generation{};std::uintptr_t owner{};DWORD tid{};double qpc{};std::uintptr_t args[4]{},caller{};unsigned char source[64]{};bool readable{},overridden{};ws::Vec3 actorUp{};
};
struct PositionWriter {DWORD64 pc{},caller{},function{};unsigned hits{},printed{},functionSize{};bool analyzed{};ws::position_proof::Store store{};char hash[65]="unavailable";};
struct Window {
 std::atomic<unsigned> lineageAttempts{0};double positionElapsed{};std::atomic<unsigned> positionInflight{0};float oldPosition[4]{};std::array<PositionWriter,16> positionWriters{};float matchedPosition[4]{};double matchedPositionMs{};unsigned positionWriterCount{},positionUnknown{},positionOverflow{};bool positionSummary{};std::atomic<bool> positionClosed{false};std::atomic<unsigned> positionCleanup{1};

 std::atomic<bool> capture{false};unsigned id{},state{};std::uint64_t session{};
 std::uintptr_t hero{},transform{},owner{};ws::Vec3 normal{},startUp{},oldObserved{};
 msm2::reset_trace::Request reset{};double firstSurface{},surfaceAnchorTime{};ws::Vec3 surfaceAnchor{};
 std::atomic<std::uint64_t> lastOverride{0},commonCommit{0},copyCommit{0};std::atomic<bool> primaryInstalled{false};double primaryArmedAt{};
 double start{},end{};std::atomic<unsigned> hits{0},drops{0};std::atomic_flag cache=ATOMIC_FLAG_INIT;
 bool expandRequested{},expanded{},expandFailed{},thirdReported{};std::atomic<bool> expandAttempted{false};
 unsigned entriesTotal{},ownerMatches{},seen{},eligible{},installedThreads{},foreignThreads{},failedThreads{},selfSkipped{},capExceeded{};
 bool snapshotCreated{},firstOk{};const char* enumStage{"not_attempted"};DWORD enumError{};
 std::atomic<const char*> stopReason{nullptr};std::atomic<unsigned> commonDrops{0},budgetDrops{0};
 struct Skip {DWORD tid{};const char* reason{};DWORD error{};unsigned count{};};std::array<Skip,16> skipSummary{};unsigned skips{},skipKinds{},suppressedSkips{};double expandedElapsed{};
 const char* firstInstallFailure{"none"};DWORD firstInstallError{};
};
struct Lease {
 std::atomic<DWORD> tid{0};unsigned window{};HANDLE handle{};bool installed{},suspended{};DWORD resumeError{};
 Entry entry{};path_b::Entry copyEntry{};DWORD64 old0{},old1{},old6{},old7{},up{};
};
struct LineageCapture {ws::lineage::Sample sample{};bool codeCurrent{};};
struct Event {
 std::uint64_t writerInvocation{};
 LineageCapture lineage{};
 unsigned window{},slotMask{};DWORD thread{};double timestamp{};DWORD64 ripAfter{},rsp{},callers[2]{},gpr[16]{};
 Entry entry{};path_b::Entry copyEntry{};double firstSurface{};bool resetCandidate{},dualCommitted{},sequenceCandidate{},thirdCandidate{};double surfaceAnchorTime{};ws::Vec3 surfaceAnchor{};std::uint64_t commitQpc{},commonQpc{},copyQpc{};
 float oldUp[4]{},newUp[4]{};unsigned char xmm[256]{};bool read{},callerValid{},xmmValid{};
};
struct CommonRecord {unsigned window{};Entry entry{};bool status{},armed{};double elapsed{};};
std::array<CommonRecord,1024> commonRecords;std::atomic<unsigned> commonHead{0},commonTail{0};
std::atomic<int> publishedReset{-1};
thread_local std::uint64_t currentCopyEntry{};
bool externalReset{};msm2::reset_trace::Request nextReset{};std::uint64_t entrySerial{};
trace::Config config;bool admitted{},haveDeinit{},wasActive{};std::atomic<bool> killed{true};
std::uintptr_t gameBase{};std::size_t gameSize{};LARGE_INTEGER frequency{};
std::array<Window,windowsCap> windows;std::array<Lease,leasesCap> leases;std::atomic<unsigned> publishedWindowCount{0};unsigned usedWindows{},positionWindows{},usedLeases{};int current=-1;
std::array<Event,ringCap> events;std::atomic<unsigned> head{0},tail{0};std::atomic_flag eventLock=ATOMIC_FLAG_INIT;
PVOID veh{};double lastSample{},deinitMs{};std::uint64_t lastSession{},runTraced{},crawlTraced{};
Snapshot lastHero{};ws::Vec3 lastNormal{};bool normalValid{};unsigned crawlStable{};
double now(){LARGE_INTEGER q{};if(!QueryPerformanceCounter(&q)||!frequency.QuadPart)return 0;return 1000.*double(q.QuadPart)/double(frequency.QuadPart);}
void closeCapture(Window& w,const char* reason){const char* empty=nullptr;w.stopReason.compare_exchange_strong(empty,reason);w.capture=false;if(w.state==5)msm2::writer_entry::cancel();}
unsigned own(const CONTEXT& c,const Lease& l){
 unsigned mask=0;
 if(c.Dr0==l.up&&(c.Dr7&0xf0003ull)==0x90001ull)mask|=1;
 if(c.Dr1==l.up+8&&(c.Dr7&0xf0000cull)==(windows[l.window].state==5?0x900004ull:0xd00004ull))mask|=2;
 return mask;
}
void restore(CONTEXT& c,const Lease& l,bool snapshot=false){
 auto mask=own(c,l);
 if(mask&1){c.Dr0=l.old0;c.Dr7=(c.Dr7&~0xf0003ull)|(l.old7&0xf0003ull);}
 if(mask&2){c.Dr1=l.old1;c.Dr7=(c.Dr7&~0xf0000cull)|(l.old7&0xf0000cull);}
 if(snapshot&&mask==3&&(c.Dr6&0xe00c)==(l.old6&0xe00c))c.Dr6=l.old6;
 else c.Dr6=(c.Dr6&~DWORD64(mask))|(snapshot?(l.old6&mask):0);
}
struct Unwind {CONTEXT c;DWORD64 returnSlots[2]{},callers[2]{};unsigned count{};};
void unwind(void* raw){
 auto& u=*static_cast<Unwind*>(raw);
 std::uintptr_t limit{},base{};__asm__ volatile("movq %%gs:0x10,%0":"=r"(limit));__asm__ volatile("movq %%gs:0x08,%0":"=r"(base));
 for(unsigned n=0;n<2;++n){
  if(u.c.Rsp<limit||u.c.Rsp>base||base-u.c.Rsp<8)break;
  MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(u.c.Rsp),&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS)))break;
  DWORD64 image=0;auto* f=RtlLookupFunctionEntry(u.c.Rip,&image,nullptr);auto oldRsp=u.c.Rsp;
  if(f){void* handler=nullptr;DWORD64 frame=0;RtlVirtualUnwind(UNW_FLAG_NHANDLER,image,u.c.Rip,f,&u.c,&handler,&frame,nullptr);}
  else {SIZE_T got{};if(!ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(u.c.Rsp),&u.c.Rip,8,&got)||got!=8)break;u.c.Rsp+=8;}
  if(u.c.Rsp<=oldRsp||u.c.Rsp>base||u.c.Rsp-oldRsp>0x10000||!VirtualQuery(reinterpret_cast<void*>(u.c.Rip),&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))||!(m.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)))break;
  u.returnSlots[u.count]=u.c.Rsp-8;u.callers[u.count++]=u.c.Rip;
 }
}

bool lineageRead(std::uintptr_t address, void *output, std::size_t size) {
  MEMORY_BASIC_INFORMATION m{};
  if (!address || !size || address > UINTPTR_MAX - size ||
      !VirtualQuery(reinterpret_cast<void *>(address), &m, sizeof(m)) ||
      m.State != MEM_COMMIT || (m.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
    return false;
  auto base = reinterpret_cast<std::uintptr_t>(m.BaseAddress);
  if (address < base || address - base > m.RegionSize ||
      size > m.RegionSize - (address - base))
    return false;
  return readMemory(address, output, size);
}
struct LineageRuntime {
  bool attempted{}, ready{}, caller2Verified{};
  unsigned writerBegin{}, writerEnd{}, callerBegin{}, callerEnd{},
      caller2Begin{}, caller2End{};
  ws::lineage::Proof writer{};
  ws::lineage::Call call{}, call2{};
  ws::lineage::Caller caller{};
  std::array<unsigned char, 5> callBytes{}, call2Bytes{};
  std::array<unsigned char, 1024> writerBytes{};
  unsigned writerSize{};
  char writerHash[65] = "unavailable", callerHash[65] = "unavailable";
};
LineageRuntime movementLineage;
void captureLineage(Event &r, Window &w) {
  const auto &m = movementLineage;
  if (!m.ready || r.ripAfter < gameBase + m.writerBegin ||
      r.ripAfter > gameBase + m.writerEnd || !r.callerValid ||
      r.callers[0] != gameBase + 0x351eeb0 ||
      r.callers[1] != gameBase + 0x351a020 || !r.read)
    return;
  if (w.lineageAttempts.fetch_add(1, std::memory_order_relaxed) >= 16)
    return;
  // Post-store hardware exception on this exact watched target/current thread.
  // No new detour, executable breakpoint, ABI, stack scan or game call.
  unsigned char current[1024]{};
  r.lineage.codeCurrent =
      lineageRead(gameBase + m.writerBegin, current, m.writerSize) &&
      (!std::memcmp(current, m.writerBytes.data(), m.writerSize)||
       writer_entry::originalIfOwned(gameBase+m.writerBegin,current,m.writerSize));
  unsigned char call[5]{}, call2[5]{};
  r.lineage.codeCurrent =
      r.lineage.codeCurrent && lineageRead(gameBase + 0x351eeab, call, 5) &&
      !std::memcmp(call, m.callBytes.data(), 5) &&
      (!m.caller2Verified || (lineageRead(gameBase + 0x351a01b, call2, 5) &&
                              !std::memcmp(call2, m.call2Bytes.data(), 5)));
  if (!r.lineage.codeCurrent)
    return;
  ws::lineage::Context c{};
  std::memcpy(c.gpr.data(), r.gpr, sizeof(r.gpr));
  c.floating = r.xmmValid;
  std::memcpy(c.xmm.data(), r.xmm, sizeof(r.xmm));
  r.lineage.sample = ws::lineage::sample(
      m.writer, unsigned(r.ripAfter - gameBase - m.writerBegin), c, w.transform,
      r.newUp, lineageRead);
}
LONG CALLBACK handler(EXCEPTION_POINTERS* e){
 if(!e||!e->ExceptionRecord||!e->ContextRecord||e->ExceptionRecord->ExceptionCode!=EXCEPTION_SINGLE_STEP)return EXCEPTION_CONTINUE_SEARCH;
 auto& c=*e->ContextRecord;if((c.ContextFlags&CONTEXT_DEBUG_REGISTERS)!=CONTEXT_DEBUG_REGISTERS)return EXCEPTION_CONTINUE_SEARCH;auto mask=unsigned(c.Dr6&3);if(!mask)return EXCEPTION_CONTINUE_SEARCH;
 DWORD tid=GetCurrentThreadId();Lease* lease=nullptr;unsigned claimed=0;bool mixed=false;
 // Immutable records also retire delayed old contexts; match each owned slot
 // independently so a debugger taking one slot cannot orphan the other.
 for(unsigned i=leasesCap;i>0;--i){auto& l=leases[i-1];if(l.tid.load(std::memory_order_acquire)!=tid)continue;auto bits=own(c,l)&mask&~claimed;if(!bits)continue;if(lease&&lease->up!=l.up)mixed=true;if(!lease)lease=&l;claimed|=bits;}
 if(!lease)return EXCEPTION_CONTINUE_SEARCH;
 auto savedLastError=GetLastError();auto& w=windows[lease->window];++w.positionInflight;double time=now();
 if(mixed){closeCapture(w,"mixed_watch_identity");killed=true;for(unsigned i=leasesCap;i>0;--i){auto& l=leases[i-1];if(l.tid.load()==tid&&own(c,l))restore(c,l,windows[l.window].state==5);}}
 else if(own(c,*lease)!=3||killed.load()||!w.capture.load(std::memory_order_acquire)||(time<=0||time>w.end)){closeCapture(w,own(c,*lease)!=3?"debug_slot_ownership_lost":killed.load()?"killed":time<=0?"qpc_failed":time>w.end?"deadline":"capture_closed");restore(c,*lease,w.state==5);}
 // Reserve three quarters of the event budget for the expanded watch. Record
 // pressure must never cancel the coordinator before its threshold transition.
 else if(w.state==5&&w.hits.load()>=256){closeCapture(w,"position_event_cap");restore(c,*lease,w.state==5);}
 else if(w.hits.load()>=(w.reset.multithread&&!w.expandAttempted.load()?config.maxEvents/4:config.maxEvents)){
  if(w.reset.multithread){++w.drops;++w.budgetDrops;}else {w.capture=false;restore(c,*lease,w.state==5);}
 }
 else if(eventLock.test_and_set(std::memory_order_acquire)){++w.drops;if(w.state==5||!w.reset.multithread){w.capture=false;restore(c,*lease,w.state==5);}}
 else {
  auto h=head.load(std::memory_order_relaxed);auto t=tail.load(std::memory_order_acquire);
  if(h-t>=ringCap){++w.drops;if(w.state==5||!w.reset.multithread){w.capture=false;restore(c,*lease,w.state==5);}}
  else {
   Event r{};r.window=lease->window;r.slotMask=claimed;r.thread=tid;r.timestamp=time;r.ripAfter=c.Rip;r.rsp=c.Rsp;
   DWORD64 regs[]={c.Rax,c.Rcx,c.Rdx,c.Rbx,c.Rsp,c.Rbp,c.Rsi,c.Rdi,c.R8,c.R9,c.R10,c.R11,c.R12,c.R13,c.R14,c.R15};std::memcpy(r.gpr,regs,sizeof(regs));r.xmmValid=(c.ContextFlags&CONTEXT_FLOATING_POINT)==CONTEXT_FLOATING_POINT;if(r.xmmValid)std::memcpy(r.xmm,&c.Xmm0,256);
   SIZE_T done=0;unsigned bytes=w.state==5?16:12;std::memcpy(r.oldUp,w.state==5?static_cast<void*>(w.oldPosition):static_cast<void*>(&w.oldObserved),bytes);r.read=ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(lease->up),r.newUp,bytes,&done)&&done==bytes;
   if(w.state==5){r.entry=lease->entry;r.commonQpc=w.commonCommit.load();r.copyQpc=w.copyCommit.load();if(r.read)std::memcpy(w.oldPosition,r.newUp,16);}
   if(r.read&&w.state!=5){
    std::memcpy(&w.oldObserved,r.newUp,12);
    if(w.state>=3){
     ws::Vec3 before{r.oldUp[0],r.oldUp[1],r.oldUp[2]},after{r.newUp[0],r.newUp[1],r.newUp[2]},b{},a{};
     bool valid=ws::unit(before,b)&&ws::unit(after,a);
     if(ws::unit(after,a)&&ws::dot(a,w.normal)>=(w.reset.multithread?.90f:.5f)){if(!w.firstSurface)w.firstSurface=time;w.surfaceAnchor=after;w.surfaceAnchorTime=time;}
     r.resetCandidate=valid&&(w.state!=4||w.lastOverride.load()!=0)&&ws::dot(b,w.normal)>=.5f&&a.y>=.98f;
     r.sequenceCandidate=w.reset.verifyDual&&valid&&a.y>=.98f&&w.surfaceAnchorTime>0&&time-w.surfaceAnchorTime<=5.&&w.lastOverride.load()!=0;
     r.resetCandidate|=r.sequenceCandidate;r.surfaceAnchor=w.surfaceAnchor;r.surfaceAnchorTime=w.surfaceAnchorTime;
     r.firstSurface=w.firstSurface;r.entry=lease->entry;if(currentCopyEntry&&currentCopyEntry==lease->copyEntry.id)r.copyEntry=lease->copyEntry;r.commitQpc=w.lastOverride.load();r.dualCommitted=w.commonCommit.load()&&w.copyCommit.load();
     r.commonQpc=w.commonCommit.load();r.copyQpc=w.copyCommit.load();
     if(w.reset.multithread&&r.dualCommitted&&ws::unit(after,a)){
      bool immediate=ws::unit(before,b)&&ws::dot(b,w.normal)>=.90f&&ws::dot(a,w.normal)<=.50f&&a.y>b.y;
      ws::Vec3 anchor{};bool sequential=w.surfaceAnchorTime>0&&time-w.surfaceAnchorTime<=5.&&ws::unit(w.surfaceAnchor,anchor)&&ws::dot(anchor,w.normal)>=.90f&&ws::dot(a,w.normal)<=.50f&&a.y>=.98f&&a.y>anchor.y;
      r.thirdCandidate=immediate||sequential;
     }
    }
   }
   bool unchanged=w.state!=5&&w.reset.verifyDual&&r.read&&!std::memcmp(r.oldUp,r.newUp,12);
   if(!unchanged){
   ++w.hits;
   Unwind u{};u.c=c;if(ws_guarded_call(unwind,&u)&&u.count){r.callerValid=true;std::memcpy(r.callers,u.callers,sizeof(r.callers));
    if(w.state==5&&r.ripAfter>gameBase+0x2cfc900&&r.ripAfter<gameBase+0x2cfc9b7)r.writerInvocation=writer_entry::pair(w.session,w.reset.generation,w.transform,u.returnSlots[0],r.callers[0]);}
   if(w.state==5)captureLineage(r,w);
   events[h%ringCap]=r;head.store(h+1,std::memory_order_release);
   if(r.resetCandidate&&!w.reset.characterize&&!w.reset.multithread){w.capture=false;restore(c,*lease,w.state==5);}
   }
  }
  eventLock.clear(std::memory_order_release);
 }
 --w.positionInflight;c.Dr6&=~DWORD64(claimed);
 if(w.state==5)SetLastError(savedLastError);
 // Never consume another debugger's single-step/hardware slots.
 return (c.Dr6&(DWORD64(0xf)|DWORD64(0xe000)))?EXCEPTION_CONTINUE_SEARCH:EXCEPTION_CONTINUE_EXECUTION;
}
bool resume(Lease& l){
 if(!l.suspended)return true;
 for(unsigned i=0;i<3;++i){if(ResumeThread(l.handle)!=DWORD(-1)){l.suspended=false;return true;}l.resumeError=GetLastError();}
 if(WaitForSingleObject(l.handle,0)==WAIT_OBJECT_0){l.suspended=false;return true;}
 killed=true;return false; // Retain handle and retry without adding suspension counts.
}
void watchSummary(const Window&,const char*);
void release(Window& w){
 w.capture=false;unsigned restored=0,foreign=0,pending=0;
 for(unsigned i=0;i<usedLeases;++i){auto& l=leases[i];if(l.window+1!=w.id||(!l.installed&&!l.suspended))continue;
  if(l.suspended&&!resume(l)){++pending;continue;}
  if(!l.installed){if(l.handle)CloseHandle(l.handle);l.handle=nullptr;continue;}
  if(WaitForSingleObject(l.handle,0)==WAIT_OBJECT_0){l.installed=false;CloseHandle(l.handle);l.handle=nullptr;continue;}
  if(l.tid.load()==GetCurrentThreadId()){++pending;continue;} // Cleanup coordinator must never suspend itself.
  if(SuspendThread(l.handle)==DWORD(-1)){++pending;continue;}
  l.suspended=true;CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;bool got=GetThreadContext(l.handle,&c),ok=false;
  if(got){if(own(c,l)){restore(c,l,true);ok=SetThreadContext(l.handle,&c);}else {ok=true;++foreign;}}
  // No allocation, logging, SDK calls or user locks between Suspend and Resume.
  bool resumed=resume(l);
  if(ok&&resumed){l.installed=false;CloseHandle(l.handle);l.handle=nullptr;++restored;}else ++pending;
 }
 writeLog("orientation_trace_stop window=%u captured=%u dropped=%u restored=%u already_cleared_or_foreign=%u cleanup_pending=%u no_page_protection_change=1",w.id,std::min(w.hits.load(),config.maxEvents),w.drops.load(),restored,foreign,pending);
 if(w.state==5&&pending==0)writer_entry::stop();
 if(w.state==5){w.positionCleanup=pending;w.positionClosed.store(true,std::memory_order_release);}
 if(w.reset.multithread&&w.state!=5){watchSummary(w,"stopped");writeLog("MULTITHREAD_UP_WATCH_RESTORE session=%llu restored=%u already_cleared_or_foreign=%u cleanup_pending=%u modified_slots=DR0_DR1 exact_saved_owned_slot_state=1",(unsigned long long)w.session,restored,foreign,pending);}
}
bool activeLeases(){for(unsigned i=0;i<usedLeases;++i)if(leases[i].installed||leases[i].suspended)return true;return false;}
void watchSummary(const Window& w,const char* phase){
 bool complete=w.expanded&&!w.expandFailed&&w.eligible>0&&!w.capExceeded&&!w.failedThreads&&!w.foreignThreads&&w.installedThreads==w.eligible&&w.drops.load()==0;
 writeLog("MULTITHREAD_UP_WATCH session=%llu phase=%s process_threads_seen=%u eligible=%u installed=%u skipped_foreign_dr=%u failed=%u complete_thread_coverage=%u window_start_elapsed=%.6f window_end_elapsed=1.500 cap=128 cap_exceeded=%u coordinator_self_excluded=%u coverage_scope=snapshot_eligible_threads_only rescan_count=0 dropped=%u expand_requested=%u expand_attempted=%u expand_completed=%u expand_failed=%u",(unsigned long long)w.session,phase,w.seen,w.eligible,w.installedThreads,w.foreignThreads,w.failedThreads,unsigned(complete),w.expandedElapsed,w.capExceeded,w.selfSkipped,w.drops.load(),unsigned(w.expandRequested),unsigned(w.expandAttempted.load()),unsigned(w.expanded),unsigned(w.expandFailed));
 writeLog("MULTITHREAD_EXPAND_STATE session=%llu phase=%s expand_requested=%u expand_attempted=%u expand_completed=%u expand_failed=%u entries_total=%u owner_pid_matches=%u win32_error_stage=%s win32_error_code=%lu stop_reason=%s common_record_drops=%u event_budget_drops=%u rescan_count=0",(unsigned long long)w.session,phase,unsigned(w.expandRequested),unsigned(w.expandAttempted.load()),unsigned(w.expanded),unsigned(w.expandFailed),w.entriesTotal,w.ownerMatches,w.enumStage,w.enumError,w.stopReason.load()?w.stopReason.load():"none",w.commonDrops.load(),w.budgetDrops.load());
 for(unsigned i=0;i<w.skipKinds;++i)writeLog("MULTITHREAD_UP_WATCH_SKIP session=%llu thread_id=%lu reason=%s win32_error_code=%lu count=%u thread_id_semantics=first_example summary_index=%u suppressed=%u",(unsigned long long)w.session,w.skipSummary[i].tid,w.skipSummary[i].reason,w.skipSummary[i].error,w.skipSummary[i].count,i,w.suppressedSkips);
}
struct Install {Window* window;Lease* lease;CONTEXT previous{};bool got{},ok{},foreign{};DWORD error{};const char* failure{"get_context_failed"};};
void installSuspended(void* raw){
 auto& p=*static_cast<Install*>(raw);auto& l=*p.lease;auto& w=*p.window;p.previous.ContextFlags=CONTEXT_DEBUG_REGISTERS;
 p.got=GetThreadContext(l.handle,&p.previous);if(!p.got){p.error=GetLastError();return;}
 p.foreign=(p.previous.Dr7&0xf)||(p.previous.Dr7&(1ull<<13));if(p.foreign)return;
 auto c=p.previous;l.old0=c.Dr0;l.old1=c.Dr1;l.old6=c.Dr6;l.old7=c.Dr7;l.up=w.transform+(w.state==5?0x30:0x10);
 // Publish immutable ownership before a successful SetThreadContext can be
 // resumed. No allocation/log/SDK/user lock occurs in this suspended region.
 l.tid.store(GetThreadId(l.handle),std::memory_order_release);
 c.Dr0=l.up;c.Dr1=l.up+8;c.Dr6&=~DWORD64(3);c.Dr7=(c.Dr7&~ownMask)|(w.state==5?0x990005ull:watchBits);
 l.installed=true;bool changed=SetThreadContext(l.handle,&c);if(!changed){p.error=GetLastError();p.failure="set_context_failed";}CONTEXT readback{};readback.ContextFlags=CONTEXT_DEBUG_REGISTERS;
 bool read=changed&&GetThreadContext(l.handle,&readback);if(changed&&!read){p.error=GetLastError();p.failure="readback_context_failed";}
 p.ok=read&&own(readback,l)==3;if(read&&!p.ok){p.error=ERROR_INVALID_DATA;p.failure="readback_ownership_mismatch";}
 if(p.ok&&l.tid.load()==w.reset.thread){w.primaryArmedAt=now();w.primaryInstalled.store(true,std::memory_order_release);}
 if(!p.ok){CONTEXT rollback{};rollback.ContextFlags=CONTEXT_DEBUG_REGISTERS;if(GetThreadContext(l.handle,&rollback)){restore(rollback,l,true);if(SetThreadContext(l.handle,&rollback))l.installed=false;}}
}
const char* installProcessThread(Window& w,DWORD tid,DWORD* osError=nullptr){
 auto error=[&](DWORD code){if(osError)*osError=code;};error(ERROR_SUCCESS);
 for(unsigned i=0;i<usedLeases;++i){auto& l=leases[i];if(l.window+1==w.id&&l.tid.load()==tid&&l.installed&&l.handle&&WaitForSingleObject(l.handle,0)!=WAIT_OBJECT_0)return nullptr;}
 if(usedLeases>=leasesCap)return "lease_cap";
 HANDLE thread=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_CONTEXT|THREAD_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,tid);if(!thread){error(GetLastError());return "open_thread_failed";}
 auto pid=GetProcessIdOfThread(thread);if(pid!=GetCurrentProcessId()){error(pid?ERROR_INVALID_DATA:GetLastError());CloseHandle(thread);return "process_identity_mismatch";}
 auto depth=SuspendThread(thread);if(depth==DWORD(-1)){error(GetLastError());CloseHandle(thread);return "suspend_failed";}
 auto& l=leases[usedLeases++];l.window=w.id-1;l.handle=thread;l.suspended=true;Install install{&w,&l};
 bool protectedOk=true;
 if(depth==0&&w.capture.load())protectedOk=ws_guarded_call(installSuspended,&install);
 bool resumed=resume(l);
 if(!l.installed&&resumed){CloseHandle(l.handle);l.handle=nullptr;}
 if(!resumed){error(l.resumeError);closeCapture(w,"resume_failed_cleanup_retained");return "resume_failed_cleanup_retained";}
 if(!protectedOk){error(ERROR_UNHANDLED_EXCEPTION);closeCapture(w,"setup_exception_cleanup_retained");return "setup_exception_cleanup_retained";}
 if(install.ok)return nullptr;
 error(install.error);
 return depth?"already_suspended":install.foreign?"foreign_dr":!w.capture.load()?"window_closed":install.failure;
}
void expandProcessWatch(Window& w){
 w.expandAttempted.store(true);w.expandedElapsed=double(GetTickCount64()-w.reset.initMs)/1000.;
 writeLog("MULTITHREAD_EXPAND_ATTEMPT session=%llu elapsed=%.6f process_id=%lu coordinator_tid=%lu primary_tid=%u trigger_seen=1",(unsigned long long)w.session,w.expandedElapsed,GetCurrentProcessId(),GetCurrentThreadId(),w.reset.thread);
 auto skip=[&](DWORD tid,const char* reason,DWORD error=ERROR_SUCCESS){
  ++w.skips;for(unsigned i=0;i<w.skipKinds;++i)if(w.skipSummary[i].error==error&&!std::strcmp(w.skipSummary[i].reason,reason)){++w.skipSummary[i].count;return;}
  if(w.skipKinds<w.skipSummary.size())w.skipSummary[w.skipKinds++]={tid,reason,error,1};else ++w.suppressedSkips;
 };
 // The production and Windows regression use this exact snapshot/filter path.
 std::array<DWORD,threadsCap> ids{};unsigned count=0;
 HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
 w.snapshotCreated=snapshot!=INVALID_HANDLE_VALUE;
 if(!w.snapshotCreated){w.enumStage="CreateToolhelp32Snapshot";w.enumError=GetLastError();++w.failedThreads;skip(0,"thread_snapshot_failed",w.enumError);}
 // Snapshot into a fixed local list BEFORE suspending anyone; the helper is
 // excluded because its code only observes Hero memory and cannot suspend itself.
 if(w.snapshotCreated){
  THREADENTRY32 te{};te.dwSize=sizeof(te);w.firstOk=Thread32First(snapshot,&te);
  if(!w.firstOk){w.enumStage="Thread32First";w.enumError=GetLastError();++w.failedThreads;skip(0,"thread_enumeration_failed",w.enumError);}
  else {w.enumStage="none";do {++w.entriesTotal;if(te.th32OwnerProcessID!=GetCurrentProcessId())continue;
   ++w.ownerMatches;if(w.seen==threadsCap){w.capExceeded=1;break;}++w.seen;
   if(te.th32ThreadID==GetCurrentThreadId()){++w.selfSkipped;continue;}
   ids[count++]=te.th32ThreadID;
  }while(Thread32Next(snapshot,&te));
  if(!w.capExceeded){auto code=GetLastError();if(code!=ERROR_NO_MORE_FILES){w.enumStage="Thread32Next";w.enumError=code;++w.failedThreads;skip(0,"thread_enumeration_incomplete",code);}}
  }
  CloseHandle(snapshot);
 }
 w.eligible=count;
 writeLog("MULTITHREAD_ENUM_RESULT session=%llu snapshot_created=%u thread32first_ok=%u entries_total=%u owner_pid_matches=%u cap_reached=%u win32_error_stage=%s win32_error_code=%lu coordinator_tid=%lu coordinator_self_excluded=%u",(unsigned long long)w.session,unsigned(w.snapshotCreated),unsigned(w.firstOk),w.entriesTotal,w.ownerMatches,w.capExceeded,w.enumStage,w.enumError,GetCurrentThreadId(),w.selfSkipped);
 for(unsigned i=0;i<count;++i){if(!w.capture.load()||now()>=w.end){w.failedThreads+=count-i;if(!std::strcmp(w.firstInstallFailure,"none"))w.firstInstallFailure="window_or_setup_deadline";for(unsigned n=i;n<count;++n)skip(ids[n],"window_or_setup_deadline");break;}
  DWORD code{};const char* reason=installProcessThread(w,ids[i],&code);if(!reason)++w.installedThreads;else {if(!std::strcmp(reason,"foreign_dr"))++w.foreignThreads;else {++w.failedThreads;if(!std::strcmp(w.firstInstallFailure,"none")){w.firstInstallFailure=reason;w.firstInstallError=code;}}skip(ids[i],reason,code);}
 }
 w.expanded=true;w.expandFailed=!w.snapshotCreated||!w.firstOk||w.enumError||!count||w.failedThreads||w.capExceeded;
 writeLog("MULTITHREAD_EXPAND_RESULT session=%llu expand_completed=1 expand_failed=%u failure_stage=%s first_install_failure_stage=%s first_install_win32_error_code=%lu skip_total=%u skip_reason_groups=%u skip_summary_suppressed=%u",(unsigned long long)w.session,unsigned(w.expandFailed),!w.snapshotCreated||!w.firstOk||w.enumError?w.enumStage:!count?"no_eligible_process_threads":w.capExceeded?"process_thread_cap":w.failedThreads?"thread_installation":"none",w.firstInstallFailure,w.firstInstallError,w.skips,w.skipKinds,w.suppressedSkips);
 watchSummary(w,"armed");
}
bool begin(const Snapshot& s,unsigned state,std::uint64_t session,std::uintptr_t owner,ws::Vec3 normal,unsigned primary){
 if(s.transform%16||s.transform>UINTPTR_MAX-0x40)return false;
 if((state==5?positionWindows>=8:usedWindows-positionWindows>=config.maxWindows)||usedWindows>=windowsCap||activeLeases()||IsDebuggerPresent())return false;
 if(!veh){veh=AddVectoredExceptionHandler(1,handler);if(!veh){writeLog("orientation_trace_unavailable reason=VEH_registration_failed");return false;}}
 DWORD candidates[threadsCap]{};unsigned count=0,enumerated=0;auto self=GetCurrentThreadId();
 if(primary&&primary!=self)candidates[count++]=primary;
 // Prearmed capture deliberately covers only the known primary thread. No
 // Toolhelp enumeration or other suspension may delay securing this thread.
 if(state!=4&&state!=5){
 HANDLE snap=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);if(snap==INVALID_HANDLE_VALUE)return false;
 THREADENTRY32 te{};te.dwSize=sizeof(te);if(Thread32First(snap,&te))do{if(te.th32OwnerProcessID!=GetCurrentProcessId()||te.th32ThreadID==self)continue;++enumerated;if(count<config.maxThreads&&std::find(candidates,candidates+count,te.th32ThreadID)==candidates+count)candidates[count++]=te.th32ThreadID;}while(Thread32Next(snap,&te));CloseHandle(snap);
 }
 auto index=usedWindows++;if(state==5)++positionWindows;auto& w=windows[index];w.id=index+1;w.state=state;w.session=session;w.hero=s.hero;w.transform=s.transform;w.owner=owner;w.normal=normal;auto axes=ws::diag::actorAxes(s.prefix);w.startUp=w.oldObserved=axes.up;w.start=now();w.end=w.start+config.windowMs;
 if(state>=3){w.reset=nextReset;w.lastOverride=w.reset.commitQpc;double commit=1000.*double(w.reset.commitQpc)/double(frequency.QuadPart);w.end=state==4?w.start+std::max(0.,(w.reset.verifyDual?1500.:1100.)-double(GetTickCount64()-w.reset.initMs)):commit+30.;ws::Vec3 u{};if(ws::unit(axes.up,u)&&ws::dot(u,normal)>=.5f)w.firstSurface=w.start;}
 if(state==5){w.positionElapsed=double(GetTickCount64()-w.reset.initMs)/1000.;w.end=w.start+600;if(!readMemory(s.transform+0x30,w.oldPosition,16))return false;}
 publishedWindowCount.store(usedWindows,std::memory_order_release);
 w.capture=w.start<w.end;
 if(w.reset.multithread&&state!=5)writeLog("MULTITHREAD_EXPAND_SCHEDULED session=%llu elapsed=%.6f threshold_elapsed=0.850 deadline_elapsed=1.500 expand_requested=0 expand_attempted=0 process_id=%lu coordinator_tid=%lu primary_tid=%u",(unsigned long long)w.session,double(GetTickCount64()-w.reset.initMs)/1000.,GetCurrentProcessId(),self,primary);
 if(state==4||state==5)publishedReset.store(int(index),std::memory_order_release);
 unsigned installed=0,busy=0,failed=0;
 for(unsigned n=0;n<count&&usedLeases<leasesCap&&w.capture.load()&&now()<w.end;++n){
  if(w.reset.multithread){DWORD code{};const char* reason=installProcessThread(w,candidates[n],&code);if(!reason)++installed;else if(!std::strcmp(reason,"foreign_dr"))++busy;else ++failed;writeLog("MULTITHREAD_PRIMARY_RESULT session=%llu thread_id=%lu installed=%u failure_stage=%s win32_error_code=%lu",(unsigned long long)w.session,candidates[n],unsigned(!reason),reason?reason:"none",code);continue;}
  HANDLE thread=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_CONTEXT|THREAD_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,candidates[n]);if(!thread){++failed;continue;}
  if(GetProcessIdOfThread(thread)!=GetCurrentProcessId()){CloseHandle(thread);++failed;continue;}
  if(SuspendThread(thread)==DWORD(-1)){CloseHandle(thread);++failed;continue;}
  CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;bool got=GetThreadContext(thread,&c),ok=false,occupied=false;
  Lease* l=nullptr;
  if(got){occupied=(c.Dr7&0xf)||(c.Dr7&(1ull<<13));if(!occupied){
   l=&leases[usedLeases++];l->window=index;l->handle=thread;l->old0=c.Dr0;l->old1=c.Dr1;l->old6=c.Dr6;l->old7=c.Dr7;l->up=s.transform+0x10;
   l->tid.store(candidates[n],std::memory_order_release); // Handler ready BEFORE Resume.
   if(state>=3&&!installed){
    // Seed the previous observation immediately before the first watched thread
    // resumes. No allocation, SDK call or user lock while it is suspended.
    ws::Vec3 up{};SIZE_T done=0;
    if(ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(l->up),&up,12,&done)&&done==12){w.oldObserved=up;ws::Vec3 u{};w.firstSurface=ws::unit(up,u)&&ws::dot(u,normal)>=.5f?now():0;}
   }
   c.Dr0=l->up;c.Dr1=l->up+8;c.Dr6&=~DWORD64(3);c.Dr7=(c.Dr7&~ownMask)|(w.state==5?0x990005ull:watchBits);bool changed=SetThreadContext(thread,&c);l->installed=changed;
   CONTEXT readback{};readback.ContextFlags=CONTEXT_DEBUG_REGISTERS;
   ok=changed&&GetThreadContext(thread,&readback)&&own(readback,*l)==3;
   if(ok&&candidates[n]==primary){w.primaryArmedAt=now();w.primaryInstalled=true;}
  }}
  // A lease also retains an unbalanced suspension on an OS resume failure.
  if(!l){l=&leases[usedLeases++];l->window=index;l->handle=thread;}
  l->suspended=true;bool resumed=resume(*l);
  if(ok&&resumed)++installed;else {if(resumed&&!l->installed){l->handle=nullptr;CloseHandle(thread);}if(occupied)++busy;else ++failed;}
  if(!resumed){w.capture=false;break;}
 }
 if((!installed||(state==4&&!w.primaryInstalled.load()))&&!w.reset.multithread)w.capture=false;
 current=int(index);
 if(state==5){current=int(index);expandProcessWatch(w);writeLog("POSITION_WATCH_ARM session=%llu elapsed=%.6f window_ms=600 watched=Transform+[0x30,0x40) slots=DR0_8_DR1_8 old_Up_leases_restored=1 threads_seen=%u threads_installed=%u coverage=snapshot_only rescan_count=0 position_writes=0",(unsigned long long)session,double(GetTickCount64()-w.reset.initMs)/1000.,w.seen,w.installedThreads);return w.capture.load()&&w.installedThreads>0;}
 writeLog("orientation_trace_arm window=%u state=%s session=%llu hero=0x%llx transform=0x%llx watched=Up_[0x10,0x1c) mechanism=hardware_DR0_8bytes_DR1_4bytes window_ms=%u threads_enumerated=%u installed=%u skipped_busy=%u failed=%u complete_thread_coverage=0 primary_native_thread=%u N398=(%.6f,%.6f,%.6f) normal_source=%s actor_up=(%.6f,%.6f,%.6f) actor_writes=0",w.id,state==1?"WallRun":"CrawlCandidate",static_cast<unsigned long long>(session),static_cast<unsigned long long>(s.hero),static_cast<unsigned long long>(s.transform),config.windowMs,enumerated,installed,busy,failed,primary,normal.x,normal.y,normal.z,state==1?"active_WallRun_field":"cached_pre_Deinit_NOT_fresh_contact",axes.up.x,axes.up.y,axes.up.z);
 return (installed>0||w.reset.multithread)&&w.capture.load();
}
const char* module(DWORD64 address,DWORD64& base,char (&name)[128]){
 HMODULE m=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(address),&m)){base=0;std::strcpy(name,"unknown");return name;}base=reinterpret_cast<DWORD64>(m);char path[MAX_PATH]{};GetModuleFileNameA(m,path,MAX_PATH);path[MAX_PATH-1]=0;auto* tail=std::strrchr(path,'\\');auto* label=tail?tail+1:path;auto length=std::min(std::strlen(label),std::size_t(127));std::memcpy(name,label,length);name[length]=0;return name;
}
void hex(const unsigned char* bytes,unsigned n,char* text){static const char* digits="0123456789abcdef";for(unsigned i=0;i<n;++i){text[2*i]=digits[bytes[i]>>4];text[2*i+1]=digits[bytes[i]&15];}text[2*n]=0;}
struct Writer {DWORD64 pc{};unsigned hits{},run{},crawl{};};std::array<Writer,32> writers;unsigned writerCount{};
bool sha256(const unsigned char* bytes,unsigned size,char (&text)[65]){
 BCRYPT_ALG_HANDLE alg{};BCRYPT_HASH_HANDLE hash{};unsigned char object[512]{},digest[32]{};DWORD length{},done{};
 bool ok=BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)>=0;
 if(ok)ok=BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&length),sizeof(length),&done,0)>=0&&length<=sizeof(object);
 if(ok)ok=BCryptCreateHash(alg,&hash,object,length,nullptr,0,0)>=0;
 if(ok)ok=BCryptHashData(hash,const_cast<PUCHAR>(bytes),size,0)>=0;
 if(ok)ok=BCryptFinishHash(hash,digest,32,0)>=0;
 if(hash)BCryptDestroyHash(hash);
 if(alg)BCryptCloseAlgorithmProvider(alg,0);
 if(ok)hex(digest,32,text);
 return ok;
}
double since(std::uint64_t qpc,double timestamp){double commit=1000.*double(qpc)/double(frequency.QuadPart);return qpc&&timestamp>=commit?(timestamp-commit)*1000.:-1.;}
std::uint64_t gameRva(DWORD64 address){return address>=gameBase&&address-gameBase<gameSize?address-gameBase:0;}
const char* writerClass(DWORD64 image,unsigned begin){return image==gameBase&&begin==0x2cfc580?"KNOWN_PATH_A":image==gameBase&&begin==0x2cfc3c0?"KNOWN_PATH_B":"UNKNOWN_WRITER";}
void classifyWrite(Window& w,const Event& r,DWORD64 moduleBase,const char* moduleName){
 DWORD64 image{};auto* function=RtlLookupFunctionEntry(r.ripAfter-1,&image,nullptr);RUNTIME_FUNCTION bounds{};
 bool metadata=function&&readMemory(reinterpret_cast<std::uintptr_t>(function),&bounds,sizeof(bounds));unsigned begin=metadata?bounds.BeginAddress:0,end=metadata?bounds.EndAddress:0;
 const char* kind=writerClass(image,begin);
 ws::Vec3 before{r.oldUp[0],r.oldUp[1],r.oldUp[2]},after{r.newUp[0],r.newUp[1],r.newUp[2]},b{},a{};bool bv=ws::unit(before,b),av=r.read&&ws::unit(after,a);
 writeLog("UP_WRITE_CLASSIFICATION session=%llu classification=%s thread_id=%lu rip=0x%llx module=%s module_offset_after=0x%llx writer_rva=0x%llx return_address=0x%llx caller_rva=0x%llx caller2_rva=0x%llx caller_valid=%u changed=%u read_valid=%u up_before=(%.9g,%.9g,%.9g) up_after=(%.9g,%.9g,%.9g) dot_before_surface=%.9g dot_after_surface=%.9g relative_us_from_last_path_a_override=%.3f relative_us_from_last_path_b_override=%.3f before_semantics=previous_serialized_watch_observation_NOT_atomic_pre_store",
  (unsigned long long)w.session,kind,r.thread,(unsigned long long)r.ripAfter,moduleName,(unsigned long long)(moduleBase?r.ripAfter-moduleBase:0),(unsigned long long)(image==gameBase?begin:gameRva(r.ripAfter)),(unsigned long long)r.callers[0],(unsigned long long)gameRva(r.callers[0]),(unsigned long long)gameRva(r.callers[1]),unsigned(r.callerValid),unsigned(r.read&&std::memcmp(r.oldUp,r.newUp,12)!=0),unsigned(r.read),before.x,before.y,before.z,after.x,after.y,after.z,bv?ws::dot(b,w.normal):0,av?ws::dot(a,w.normal):0,since(r.commonQpc,r.timestamp),since(r.copyQpc,r.timestamp));
 if(!r.thirdCandidate||w.thirdReported)return;
 w.thirdReported=true;bool immediate=bv&&av&&ws::dot(b,w.normal)>=.90f&&ws::dot(a,w.normal)<=.50f&&a.y>b.y;
 if(!immediate){before=r.surfaceAnchor;bv=ws::unit(before,b);}
 writeLog("THIRD_WRITER_EVENT session=%llu classification=%s thread_id=%lu rip=0x%llx writer_rva=0x%llx caller=0x%llx caller_rva=0x%llx caller2=0x%llx caller2_rva=0x%llx caller_valid=%u up_before=(%.9g,%.9g,%.9g) up_after=(%.9g,%.9g,%.9g) dot_before_surface=%.9g dot_after_surface=%.9g dot_before_world=%.9g dot_after_world=%.9g relative_us_from_last_path_a_override=%.3f relative_us_from_last_path_b_override=%.3f before_semantics=%s surface_anchor_age_us=%.3f evidence_only=1 third_path_modified=0",
  (unsigned long long)w.session,kind,r.thread,(unsigned long long)r.ripAfter,(unsigned long long)(image==gameBase?begin:gameRva(r.ripAfter)),(unsigned long long)r.callers[0],(unsigned long long)gameRva(r.callers[0]),(unsigned long long)r.callers[1],(unsigned long long)gameRva(r.callers[1]),unsigned(r.callerValid),before.x,before.y,before.z,after.x,after.y,after.z,bv?ws::dot(b,w.normal):0,av?ws::dot(a,w.normal):0,bv?b.y:0,av?a.y:0,since(r.commonQpc,r.timestamp),since(r.copyQpc,r.timestamp),immediate?"previous_watch_observation":"surface_anchor_across_partial_stores_NOT_atomic_pre_store",r.surfaceAnchorTime>0?(r.timestamp-r.surfaceAnchorTime)*1000.:-1.);
 std::array<unsigned char,0x4000> bytes{};char hash[65]{};unsigned size=end>begin?end-begin:0;
 bool captured=function&&image&&size&&size<=bytes.size()&&readMemory(image+begin,bytes.data(),size)&&sha256(bytes.data(),size,hash);
 writeLog("THIRD_WRITER_IDENTITY session=%llu rip_after=0x%llx module=%s image_base=0x%llx function_begin_rva=0x%x function_end_rva=0x%x function_size=%u function_sha256=%s function_boundary_verified=%u caller_rva=0x%llx caller2_rva=0x%llx thread_id=%lu capture_stage=current_runtime_may_contain_owned_entry_hooks",
  (unsigned long long)w.session,(unsigned long long)r.ripAfter,moduleName,(unsigned long long)image,begin,end,size,captured?hash:"unavailable",unsigned(captured),(unsigned long long)gameRva(r.callers[0]),(unsigned long long)gameRva(r.callers[1]),r.thread);
 if(captured)for(unsigned offset=0;offset<size;offset+=64){unsigned count=std::min(64u,size-offset);char text[129]{};hex(bytes.data()+offset,count,text);writeLog("THIRD_WRITER_CODE session=%llu offset=0x%x bytes=%s private_do_not_distribute=1",(unsigned long long)w.session,offset,text);}
}

bool movementFunction(unsigned rva, unsigned &begin, unsigned &end) {
  if (!gameBase || rva >= gameSize)
    return false;
  DWORD64 image{};
  auto f = RtlLookupFunctionEntry(gameBase + rva, &image, nullptr);
  RUNTIME_FUNCTION b{};
  if (image != gameBase || !f ||
      !readMemory(reinterpret_cast<std::uintptr_t>(f), &b, sizeof(b)) ||
      b.BeginAddress > rva || b.EndAddress <= rva || b.EndAddress > gameSize)
    return false;
  begin = b.BeginAddress;
  end = b.EndAddress;
  return true;
}
bool movementEntryGate(const reset_trace::Request& r) noexcept {
 if(killed.load()||!native_source::diagnosticIdentity(r))return false;
 Snapshot live{};return sample(live)&&live.hero==r.hero&&live.transform==r.transform;
}
void configureMovementLineage() {
  auto &m = movementLineage;
  if (m.attempted)
    return;
  m.attempted = true;
  bool wb = movementFunction(0x2cfc900, m.writerBegin, m.writerEnd) &&
            m.writerBegin == 0x2cfc900;
  m.writerSize = wb ? m.writerEnd - m.writerBegin : 0;
  if (wb && m.writerSize <= m.writerBytes.size() &&
      lineageRead(gameBase + m.writerBegin, m.writerBytes.data(),
                  m.writerSize)) {
    sha256(m.writerBytes.data(), m.writerSize, m.writerHash);
    m.writer = ws::lineage::writer(m.writerBytes.data(), m.writerSize);
  }
  writeLog("MOVEMENT_WRITER_PROOF writer_rva=0x2cfc900 function_begin=0x%x "
           "function_end=0x%x function_size=%u complete_decode=%u "
           "current_code_hash=%s position_store_count=%u destination_mask=0x%x "
           "entry_target_relation=%s lineage_complete=%u "
           "source_map_supported_prefix=%u first_unsupported_offset=0x%x "
           "unsupported=%u reason=%s "
           "hash_scope=accepted_exe_current_runtime_NOT_exclusive_semantics "
           "full_bytes_logged=0 writable_source_proven=0",
           m.writerBegin, m.writerEnd, m.writerSize, unsigned(m.writer.decoded),
           m.writerHash, m.writer.storeCount, m.writer.mask,
           m.writer.sourceMap ? "entry_RCX_plus_0x30_through_0x3c"
                              : "unproven_see_components",
           unsigned(m.writer.complete), unsigned(m.writer.sourceMap),
           m.writer.firstUnsupported, m.writer.unsupported,
           wb ? m.writerSize > m.writerBytes.size() ? "writer_span_over_1024"
                                                    : m.writer.reason
              : "exact_runtime_writer_boundary_missing");
  for (unsigned i = 0;
       i < std::min<unsigned>(m.writer.unsupported,
                              m.writer.unsupportedOffsets.size());
       ++i)
    writeLog(
        "MOVEMENT_LINEAGE_UNSUPPORTED function=writer instruction_rva=0x%x "
        "reason=unsupported_effect_or_control_flow full_bytes_logged=0",
        m.writerBegin + m.writer.unsupportedOffsets[i]);
  for (unsigned i = 0; i < m.writer.storeCount; ++i) {
    auto &t = m.writer.stores[i];
    char expression[384]{};
    ws::lineage::describe(m.writer, t.value, expression, sizeof(expression));
    writeLog("MOVEMENT_WRITER_SOURCE_COMPONENT component=%u store_rva=0x%x "
             "destination_offset=0x%x source=%s whole_function_lineage=%u "
             "entry_values_captured=0",
             t.component, m.writerBegin + t.offset, 48 + 4 * t.component,
             expression, unsigned(m.writer.complete));
  }
  std::array<unsigned char, 8192> caller{};
  bool cb = movementFunction(0x351eeaf, m.callerBegin, m.callerEnd);
  unsigned size = cb ? m.callerEnd - m.callerBegin : 0;
  if (cb && size <= caller.size() &&
      lineageRead(gameBase + m.callerBegin, caller.data(), size)) {
    sha256(caller.data(), size, m.callerHash);
    m.call = ws::lineage::callsite(caller.data(), size, m.callerBegin,
                                   0x351eeb0, 0x2cfc900);
    if (m.call.verified)
      std::memcpy(m.callBytes.data(), caller.data() + m.call.offset, 5);
    if (m.call.verified)
      m.caller =
          ws::lineage::caller(caller.data(), size, m.call.offset, m.writer);
  }
  if (cb && size > caller.size())
    m.call.reason = "caller_runtime_span_over_8192";
  std::array<unsigned char, 8192> caller2{};
  bool c2 = cb && movementFunction(0x351a01f, m.caller2Begin, m.caller2End);
  unsigned c2size = c2 ? m.caller2End - m.caller2Begin : 0;
  if (c2 && c2size <= caller2.size() &&
      lineageRead(gameBase + m.caller2Begin, caller2.data(), c2size))
    m.call2 = ws::lineage::callsite(caller2.data(), c2size, m.caller2Begin,
                                    0x351a020, m.callerBegin);
  if (c2 && c2size > caller2.size())
    m.call2.reason = "caller2_runtime_span_over_8192";
  m.caller2Verified = m.call2.verified;
  if (m.caller2Verified)
    std::memcpy(m.call2Bytes.data(), caller2.data() + m.call2.offset, 5);
  writeLog("MOVEMENT_CALLSITE_PROOF return_rva=0x351eeb0 call_rva=0x%x "
           "target_rva=0x2cfc900 direct_call_verified=%u "
           "caller_function_begin=0x%x caller_function_end=0x%x caller_hash=%s "
           "complete_decode=%u caller2_return_rva=0x351a020 "
           "caller2_function_begin=0x%x caller2_function_end=0x%x "
           "caller2_direct_relation_verified=%u caller2_reason=%s reason=%s",
           m.call.verified ? m.callerBegin + m.call.offset : 0,
           unsigned(m.call.verified), m.callerBegin, m.callerEnd, m.callerHash,
           unsigned(m.call.decoded), m.caller2Begin, m.caller2End,
           unsigned(m.caller2Verified), m.call2.reason, m.call.reason);
  writeLog(
      "MOVEMENT_CALLER_SOURCE_PROOF caller_rva=0x351eeb0 "
      "source_storage=bounded_symbolic_caller_slice construction_kind=%s "
      "arithmetic_shape=%s producer_rva=0 slice_last_instruction_rva=0x%x "
      "producer_caller_rva=0x351a020 uses_current_position=unknown "
      "uses_dt=unknown uses_direction_like_vector=unknown "
      "uses_speed_like_scalar=unknown slice_complete=%u unsupported_edges=%u "
      "last_barrier_offset=0x%x surface_tangent_correlation=pending_runtime "
      "exclusive_to_hero_translation=0 writable_source_proven=0 reason=%s",
      m.caller.construction, m.caller.shape,
      m.callerBegin + m.caller.producerOffset, unsigned(m.caller.sliceComplete),
      m.caller.unsupported, m.caller.barrier, m.caller.reason);
  for (unsigned k = 0; k < 4; ++k) {
    char expression[384]{};
    ws::lineage::describe(m.caller.slice, m.caller.payload[k], expression,
                          sizeof(expression));
    writeLog("MOVEMENT_CALLER_SOURCE_COMPONENT component=%u expression=%s "
             "typed_semantics=unknown writable_source_proven=0",
             k, expression);
  }
  writer_entry::Spec entrySpec{};entrySpec.verified=admitted;entrySpec.decoded=m.writer.decoded;entrySpec.directCall=m.call.verified;entrySpec.base=gameBase;entrySpec.imageSize=gameSize;entrySpec.begin=m.writerBegin;entrySpec.end=m.writerEnd;entrySpec.callerBegin=m.callerBegin;entrySpec.callerEnd=m.callerEnd;entrySpec.original=m.writerBytes.data();entrySpec.hash=m.writerHash;entrySpec.callerOriginal=caller.data();entrySpec.callerHash=m.callerHash;entrySpec.caller2Direct=m.caller2Verified;
  if(c2&&c2size<=caller2.size())entrySpec.caller2Relation=writer_entry::callerRelation(caller2.data(),c2size,gameBase+m.caller2Begin,gameBase+0x351a020,gameBase+m.callerBegin,lineageRead);
  writer_entry::configure(entrySpec,movementEntryGate);
  m.ready = m.writer.sourceMap && m.call.verified;
  writeLog("MOVEMENT_SOURCE_CANDIDATE candidate_id=c12_verified_chain "
           "producer_rva=0x%x consumer_rva=0x2cfc900 source_kind=%s "
           "storage_or_stack_location=symbolic_component_records "
           "hero_target_verified=0 wallrun_owned=0 "
           "correlates_with_translation=unknown "
           "surface_tangent_relation=pending_runtime writable_source_proven=0 "
           "reason=%s source_capture_ready=%u input_observation=0",
           m.callerBegin, m.caller.construction,
           m.ready ? "trap_time_reconstruction_only_entry_snapshot_and_W_"
                     "exclusive_effects_missing"
                   : "writer_source_prefix_or_direct_callsite_proof_missing",
           unsigned(m.ready));
}
void movementSourceEvent(Window &w, const Event &r) {
  auto &m = movementLineage;
  if (!r.callerValid || gameRva(r.callers[0]) != 0x351eeb0 ||
      gameRva(r.ripAfter) < 0x2cfc900 || gameRva(r.ripAfter) > m.writerEnd)
    return;
  auto &s = r.lineage.sample;
  writeLog(
      "MOVEMENT_WRITER_SOURCE_MAP session=%llu caller_rva=0x351eeb0 "
      "thread_id=%lu source_component_x=see_static_component_0 "
      "source_component_y=see_static_component_1 "
      "source_component_z=see_static_component_2 "
      "source_component_w=see_static_component_3 source_kind=%s source_span=%u "
      "source_readable=%u source_identity_stable=0 recovered_aliases_stable=%u "
      "post_store_match=%u position_w_semantics_observed=0 W_bits_sampled=%u "
      "reconstructed_mask=0x%x code_current=%u hero_target_verified=%u "
      "entry_snapshot_captured=0 "
      "observation=post_instruction_surviving_aliases_and_trap_time_pointed_"
      "values writable_source_proven=0 reason=%s",
      (unsigned long long)w.session, r.thread,
      m.writer.sourceMap ? "computed_or_entry_leaves" : "unknown",
      m.writer.sourceMap ? 16u : 0u, unsigned(s.readable), unsigned(s.stable),
      unsigned(s.match), unsigned(s.wObserved), s.mask,
      unsigned(r.lineage.codeCurrent), unsigned(s.target),
      s.found ? "true_entry_snapshot_W_and_exclusive_effects_unproven"
              : "writer_lineage_call_chain_or_surviving_values_unproven");
  if (!s.target || !s.fullRow)
    return;
  ws::Vec3 before{r.oldUp[0], r.oldUp[1], r.oldUp[2]},
      observed{r.newUp[0], r.newUp[1], r.newUp[2]},
      payload{s.payload[0], s.payload[1], s.payload[2]};
  auto d = ws::movement::decompose(before, observed, w.reset.basis, 1);
  auto predicted = payload - before;
  auto error = payload - observed;
  writeLog(
      "MOVEMENT_SOURCE_CORRELATION session=%llu elapsed=%.6f "
      "source_payload=(%.9g,%.9g,%.9g,%.9g) "
      "observed_position_before=(%.9g,%.9g,%.9g,%.9g) "
      "observed_position_after=(%.9g,%.9g,%.9g,%.9g) "
      "predicted_delta=(%.9g,%.9g,%.9g) observed_delta=(%.9g,%.9g,%.9g) "
      "delta_error=%.9g delta_normal=%.9g delta_forward=%.9g delta_right=%.9g "
      "post_store_match=%u component=%u full_row_final_store=%u "
      "before_semantics=previous_serialized_watch_snapshot "
      "after_semantics=scalar_store_NOT_whole_function_return "
      "delta_semantics=correlation_NOT_physics_velocity "
      "concurrent_or_downstream_effects_excluded=0 writable_source_proven=0",
      (unsigned long long)w.session,
      w.positionElapsed + (r.timestamp - w.start) / 1000., s.payload[0],
      s.payload[1], s.payload[2], s.payload[3], r.oldUp[0], r.oldUp[1],
      r.oldUp[2], r.oldUp[3], r.newUp[0], r.newUp[1], r.newUp[2], r.newUp[3],
      predicted.x, predicted.y, predicted.z, d.world.x, d.world.y, d.world.z,
      std::sqrt(ws::dot(error, error)), d.normal, d.forward, d.right,
      unsigned(s.match), s.component, unsigned(s.finalStore));
}
void positionEvent(Window& w,const Event& r,DWORD64 moduleBase,const char* moduleName){
 DWORD64 image{};auto* function=RtlLookupFunctionEntry(r.ripAfter?r.ripAfter-1:0,&image,nullptr);auto begin=function?image+function->BeginAddress:0;
 if(r.writerInvocation&&r.read&&image==gameBase&&function&&function->BeginAddress==0x2cfc900)writer_entry::observe(r.writerInvocation,r.ripAfter,w.transform,r.gpr,r.xmm,r.xmmValid,r.newUp,r.callers[0],r.callers[1]);
 PositionWriter* writer=nullptr;for(unsigned i=0;i<w.positionWriterCount;++i)if(w.positionWriters[i].pc==r.ripAfter&&w.positionWriters[i].caller==r.callers[0]){writer=&w.positionWriters[i];break;}
 if(!writer){if(w.positionWriterCount==w.positionWriters.size()){++w.positionOverflow;return;}writer=&w.positionWriters[w.positionWriterCount++];writer->pc=r.ripAfter;writer->caller=r.callers[0];writer->function=begin;}
 ++writer->hits;bool pathA=image==gameBase&&function&&function->BeginAddress==0x2cfc580;if(!pathA)++w.positionUnknown;

 if(writer->printed++>=4)return;
 movementSourceEvent(w,r);
 ws::Vec3 before{r.oldUp[0],r.oldUp[1],r.oldUp[2]},after{r.newUp[0],r.newUp[1],r.newUp[2]};auto delta=ws::movement::decompose(before,after,w.reset.basis,1);
 unsigned char bytes[1024]{};unsigned size=function?function->EndAddress-function->BeginAddress:0;
 // Owned Path A entry bytes are hooked; startup inspection owns its original
 // proof. Dynamic decode of a hooked body is never advertised as original.
 if(!writer->analyzed){writer->analyzed=true;writer->functionSize=size;if(!pathA&&size&&size<=sizeof(bytes)&&(writer_entry::originalIfOwned(begin,bytes,size)||readMemory(begin,bytes,size))){writer->store=ws::position_proof::positionStore(bytes,size,unsigned(r.ripAfter-begin));sha256(bytes,size,writer->hash);}}
 auto store=writer->store;const char* hash=writer->hash;
 bool effective=false;std::uintptr_t address{};
 if(store.matched&&store.baseRegister<16){auto reg=r.gpr[store.baseRegister];if(store.displacement>=0&&reg<=UINTPTR_MAX-std::uint64_t(store.displacement)){address=reg+store.displacement;effective=address>=w.transform+0x30&&address<w.transform+0x40&&store.width<=w.transform+0x40-address;}else if(store.displacement<0&&reg>=std::uint64_t(-store.displacement)){address=reg-std::uint64_t(-store.displacement);effective=address>=w.transform+0x30&&address<w.transform+0x40&&store.width<=w.transform+0x40-address;}}
 if(writer->hits==1)writeLog("MOVEMENT_WRITER_STATIC session=%llu writer_function_rva=0x%llx function_size=%u complete_decode=%u store_matched=%u exact_store_instruction_offset=0x%x calls=%u branches=%u source_register_id=%u original_body_verified=%u reason=%s full_bytes_logged=0 writable_source_proven=0",(unsigned long long)w.session,(unsigned long long)(image==gameBase&&function?function->BeginAddress:0),size,unsigned(store.decoded),unsigned(store.matched),store.instruction,store.calls,store.branches,store.sourceRegister,0u,pathA?"owned_patch_originals_inspected_at_startup":store.decoded?"bounded_complete_decode_runtime_operand_only":"missing_boundary_over_1024_bytes_or_decode_failed");
 auto& entry=r.entry;bool linked=pathA&&entry.readable&&entry.overridden&&entry.tid==r.thread&&entry.args[0]==w.transform&&entry.owner==w.owner&&entry.generation==w.reset.generation&&entry.caller==gameBase+0x2cfc8f3&&r.callerValid&&r.callers[0]==entry.caller&&entry.qpc<=r.timestamp&&r.timestamp-entry.qpc<=5.;
 bool match=linked&&r.read&&!std::memcmp(entry.source+48,r.newUp,16);bool overwrite=!pathA&&r.read&&w.matchedPositionMs&&r.timestamp>=w.matchedPositionMs&&std::memcmp(w.matchedPosition,r.newUp,16);if(match){std::memcpy(w.matchedPosition,r.newUp,16);w.matchedPositionMs=r.timestamp;}unsigned oldW{},newW{};std::memcpy(&oldW,r.oldUp+3,4);std::memcpy(&newW,r.newUp+3,4);
 writeLog("POSITION_WRITE_EVENT invocation_id=%llu session=%llu elapsed=%.6f thread_id=%lu rip=0x%llx module=%s module_rva=0x%llx writer_function_rva=0x%llx caller_rva=0x%llx caller2_rva=0x%llx target_transform=0x%llx slot_mask=%u position_before=(%.9g,%.9g,%.9g,%.9g) position_after=(%.9g,%.9g,%.9g,%.9g) position_w_bits_before=0x%x position_w_bits_after=0x%x delta=(%.9g,%.9g,%.9g) delta_normal=%.9g delta_surface_forward=%.9g delta_surface_right=%.9g delta_valid=%u relative_us_from_last_path_a=%.3f relative_us_from_last_path_b=%.3f classification=%s before_semantics=previous_serialized_watch_snapshot_NOT_atomic_pre_store after_semantics=post_instruction_NOT_function_return read_valid=%u store_instruction_rva=0x%llx store_width=%u destination_offset=%lld anchored_decode=%u effective_address_verified=%u function_sha256=%s printed_cap_per_writer=4 position_writes=0",
  (unsigned long long)r.writerInvocation,(unsigned long long)w.session, w.positionElapsed+(r.timestamp-w.start)/1000.,r.thread,(unsigned long long)r.ripAfter,moduleName,(unsigned long long)(r.ripAfter-moduleBase),(unsigned long long)(image==gameBase&&function?function->BeginAddress:0),(unsigned long long)gameRva(r.callers[0]),(unsigned long long)gameRva(r.callers[1]),(unsigned long long)w.transform,r.slotMask,r.oldUp[0],r.oldUp[1],r.oldUp[2],r.oldUp[3],r.newUp[0],r.newUp[1],r.newUp[2],r.newUp[3],oldW,newW,delta.world.x,delta.world.y,delta.world.z,delta.normal,delta.forward,delta.right,unsigned(delta.valid),since(r.commonQpc,r.timestamp),since(r.copyQpc,r.timestamp),pathA?"PATH_A_POSITION_CANDIDATE":"OTHER_POSITION_WRITER",unsigned(r.read),(unsigned long long)(store.matched&&image==gameBase&&begin?begin+store.instruction-gameBase:0),store.width,effective?static_cast<long long>(address-w.transform):-1,unsigned(store.decoded),unsigned(effective),hash);
 writeLog("PATH_A_POSITION_CAUSALITY_PROOF admitted=0 session=%llu writer_verified=%u caller_verified=%u stack_relation_verified=%u source_offsets=candidate_[0x30,0x40) destination_offsets=watch_[0x30,0x40) post_writer_match=%u position_w_observed=%u downstream_overwrite_observed=%u exclusive_effects_proven=0 reason=whole_function_CFG_caller_construction_W_and_exclusive_effects_not_proven write_permission=0",
  (unsigned long long)w.session,unsigned(linked),unsigned(linked),unsigned(linked),unsigned(match),unsigned(r.read),unsigned(overwrite));
 if(writer->hits==1)writeLog("MOVEMENT_SOURCE_CANDIDATE candidate_id=%u session=%llu producer_rva=0x%llx consumer_rva=0x%llx source_kind=%s storage_or_stack_location=0x%llx wallrun_owned=%u hero_target_verified=1 verification_scope=watch_identity_NOT_atomic_game_snapshot correlates_with_translation=%u surface_tangent_relation=observed_delta_only writable_source_proven=0 reason=runtime_store_identity_only_upstream_lineage_and_exclusive_effects_missing",w.positionWriterCount,(unsigned long long)w.session,(unsigned long long)gameRva(r.callers[0]),(unsigned long long)(image==gameBase&&function?function->BeginAddress:0),linked?"position":"unknown",(unsigned long long)(linked?entry.args[1]+48:0),unsigned(linked),unsigned(r.read&&delta.valid&&ws::dot(delta.world,delta.world)>0));
}
}
namespace msm2::trace {
void configure(Config c,bool verified,std::uintptr_t base,std::size_t size){
 if(usedWindows){killed=true;admitted=false;writeLog("orientation_trace_rearm_refused reason=process_lifetime_budget_requires_full_game_restart");return;}
 config=c;admitted=verified&&base&&size&&c.enabled&&c.windowMs>=100&&c.windowMs<=500&&c.maxWindows<=windowsCap&&c.maxEvents>=1&&c.maxEvents<=256&&c.maxThreads>=1&&c.maxThreads<=threadsCap;gameBase=base;gameSize=size;QueryPerformanceFrequency(&frequency);killed=!admitted;
 if(admitted)configureMovementLineage();
 writeLog("orientation_trace_config enabled=%u admitted=%u diagnostic_only=1 window_ms=%u max_windows=%u max_events=%u max_threads=%u actor_writes=0 unknown_game_calls=0 PAGE_GUARD=0 coverage=current_threads_bounded_NOT_complete",unsigned(c.enabled),unsigned(admitted),c.windowMs,c.maxWindows,c.maxEvents,c.maxThreads);
}
void kill() noexcept{writer_entry::cancel();killed=true;for(auto& w:windows)closeCapture(w,"kill_or_shutdown");}
void tick(const ws::observed::VerifiedWallContext& ctx,unsigned nativeThread,const char* exitReason){
 if(externalReset)return; // Dedicated helper is the sole owner of external windows.
 double time=now();
 if(current<0&&activeLeases()){for(unsigned i=0;i<usedWindows;++i)release(windows[i]);}
 if(current>=0){auto& w=windows[unsigned(current)];if((w.state==1&&!ctx.wallRunActive)||(w.state==2&&ctx.wallRunActive))w.capture=false;if(killed.load()||!w.capture.load()||time>=w.end){release(w);current=-1;}}
 if(!ctx.lifecycleVerified){kill();return;}
 if(killed.load()||!admitted)return;
 if(IsDebuggerPresent()){kill();return;}
 if(wasActive&&!ctx.wallRunActive){if(current>=0&&windows[unsigned(current)].state==1)windows[unsigned(current)].capture=false;haveDeinit=std::strcmp(exitReason,"verified_deinit")==0;deinitMs=time;crawlStable=0;}wasActive=ctx.wallRunActive;
 if(!ctx.wallRunActive&&(!haveDeinit||time-deinitMs>4000))return;
 if(time-lastSample<20)return;
 lastSample=time;Snapshot s{};
 if(!sample(s)){if(current>=0)windows[unsigned(current)].capture=false;return;}
 auto axes=ws::diag::actorAxes(s.prefix);if(current>=0&&windows[unsigned(current)].state==2&&std::abs(axes.up.y)>=.35f)windows[unsigned(current)].capture=false;if(!axes.valid){if(current>=0)windows[unsigned(current)].capture=false;return;}
 if(lastHero.hero&&(s.hero!=lastHero.hero||s.transform!=lastHero.transform)){if(current>=0)windows[unsigned(current)].capture=false;haveDeinit=false;normalValid=false;crawlStable=0;}
 if(ctx.wallRunActive){
  lastHero=s;lastSession=ctx.wallRunSessionId;haveDeinit=false;
  ws::Vec3 n{};normalValid=ctx.wallRunOwner<=UINTPTR_MAX-0x3a4&&readMemory(ctx.wallRunOwner+0x398,&n,12)&&ws::unit(n,lastNormal);
  if(current<0&&normalValid&&ctx.elapsed>=.7&&ctx.wallRunUpdateCalls>0&&runTraced!=lastSession){begin(s,1,lastSession,ctx.wallRunOwner,lastNormal,nativeThread);runTraced=lastSession;}
 }else if(haveDeinit&&s.hero==lastHero.hero&&s.transform==lastHero.transform){
  if(std::abs(axes.up.y)<.35f)++crawlStable;else crawlStable=0;
  if(current<0&&normalValid&&crawlStable>=3&&crawlTraced!=lastSession){begin(s,2,lastSession,0,lastNormal,nativeThread);crawlTraced=lastSession;}
 }
}
void stop(){kill();for(unsigned attempt=0;attempt<3&&activeLeases();++attempt)for(unsigned i=0;i<usedWindows;++i)release(windows[i]);current=-1;writer_entry::stop();drainLogs();writeLog("orientation_trace_shutdown cleanup_pending=%u VEH_tombstones_resident_until_process_exit=1",unsigned(activeLeases()));}
void drainLogs(){
 for(unsigned n=0;n<commonRecords.size();++n){
  auto t=commonTail.load(std::memory_order_relaxed);if(t==commonHead.load(std::memory_order_acquire))break;
  auto r=commonRecords[t%commonRecords.size()];commonTail.store(t+1,std::memory_order_release);auto& w=windows[r.window];auto& e=r.entry;
  if(r.status){writeLog("PREARM_STATUS session=%llu elapsed=%.6f primary_thread=%u DR0_installed=%u DR1_installed=%u trace_already_armed_before_override=%u",(unsigned long long)w.session,r.elapsed,w.reset.thread,unsigned(r.armed),unsigned(r.armed),unsigned(r.armed));continue;}
  float m[16]{};std::memcpy(m,e.source,64);ws::Vec3 u{};bool valid=e.readable&&ws::unit({m[4],m[5],m[6]},u);
  writeLog("COMMON_WRITER_TRACE session=%llu elapsed=%.6f entry_id=%llu qpc_ms=%.6f thread_id=%lu caller_rva=0x%llx RCX=0x%llx RDX=0x%llx R8=0x%llx R9=0x%llx source_valid=%u source_right=(%.9g,%.9g,%.9g) source_up=(%.9g,%.9g,%.9g) source_forward=(%.9g,%.9g,%.9g) source_up_dot_world=%.9g source_up_dot_surface=%.9g actor_up_before=(%.9g,%.9g,%.9g) N398=(%.9g,%.9g,%.9g) source_overridden_this_entry=%u dropped=%u",
   (unsigned long long)w.session,r.elapsed,(unsigned long long)e.id,e.qpc,e.tid,(unsigned long long)(e.caller>=gameBase?e.caller-gameBase:0),(unsigned long long)e.args[0],(unsigned long long)e.args[1],(unsigned long long)e.args[2],(unsigned long long)e.args[3],unsigned(valid),m[0],m[1],m[2],m[4],m[5],m[6],m[8],m[9],m[10],valid?u.y:0,valid?ws::dot(u,w.normal):0,e.actorUp.x,e.actorUp.y,e.actorUp.z,w.normal.x,w.normal.y,w.normal.z,unsigned(e.overridden),w.drops.load());
 }
 for(unsigned n=0;n<ringCap;++n){auto t=tail.load(std::memory_order_relaxed);if(t==head.load(std::memory_order_acquire))break;auto r=events[t%ringCap];tail.store(t+1,std::memory_order_release);auto& w=windows[r.window];DWORD64 base=0;char name[128]{};module(r.ripAfter,base,name);
  if(w.state==5){positionEvent(w,r,base,name);continue;}
  if(w.reset.multithread)classifyWrite(w,r,base,name);
  if(w.state>=3){
   double commit=1000.*double(r.commitQpc)/double(frequency.QuadPart);
   ws::Vec3 before{r.oldUp[0],r.oldUp[1],r.oldUp[2]},after{r.newUp[0],r.newUp[1],r.newUp[2]},b{},a{};
   bool vectors=r.read&&ws::unit(before,b)&&ws::unit(after,a);
   DWORD64 image=0;auto* f=RtlLookupFunctionEntry(r.ripAfter-1,&image,nullptr);
   auto functionRva=f&&image==gameBase?f->BeginAddress:0u;
   const auto& entry=r.entry;
   const auto& copyEntry=r.copyEntry;
   if(copyEntry.id&&functionRva==0x2cfc3c0){
    bool linked=copyEntry.session==w.session&&copyEntry.generation==w.reset.generation&&copyEntry.owner==w.owner&&copyEntry.thread==r.thread&&copyEntry.rcx==w.transform&&r.callerValid&&r.callers[0]==gameBase+copyEntry.caller&&copyEntry.qpc<=std::uint64_t(r.timestamp*double(frequency.QuadPart)/1000.)&&r.timestamp-1000.*double(copyEntry.qpc)/double(frequency.QuadPart)<=5.;
    writeLog("PATH_B_ENTRY_LINK session=%llu qpc_ms=%.6f entry_id=%llu entry_qpc=%llu thread_id=%lu writer_rva=0x2cfc3c0 caller_rva=0x%llx target=0x%llx entry_link_checks=%u static_store_proof=0 requires_anchored_decode=1",(unsigned long long)w.session,r.timestamp,(unsigned long long)copyEntry.id,(unsigned long long)copyEntry.qpc,r.thread,(unsigned long long)copyEntry.caller,(unsigned long long)copyEntry.rcx,unsigned(linked));
   }

   bool correlated=entry.id&&entry.nativeUpdate&&entry.generation==w.reset.generation&&entry.owner==w.owner&&entry.tid==r.thread&&entry.qpc<=r.timestamp&&entry.args[0]==w.transform&&r.callerValid&&entry.caller==r.callers[0]&&r.gpr[6]==entry.args[1]&&functionRva==0x2cfc580;
   writeLog("RESET_TRACE_EVENT session=%llu native_update=%llu relative_us_from_override=%.3f override_commit_qpc=%llu qpc_ms=%.6f rip=0x%llx writer_function_rva=0x%x caller=0x%llx caller_rva=0x%llx caller2=0x%llx caller2_rva=0x%llx thread_id=%lu up_before=(%.9g,%.9g,%.9g) up_after=(%.9g,%.9g,%.9g) dot_before_surface=%.9g dot_after_surface=%.9g dot_before_world=%.9g dot_after_world=%.9g N398=(%.9g,%.9g,%.9g) transform=0x%llx target_verified_by_watch=1 read_valid=%u old_value_semantics=previous_observation_NOT_atomic_instruction_before entry_id=%llu entry_correlated=%u first_surface_observed_qpc_ms=%.6f first_surface_semantics=first_watched_or_arm_snapshot_NOT_exact_first_write dropped=%u",
    (unsigned long long)w.session,(unsigned long long)(correlated?entry.nativeUpdate:0),(r.commitQpc?(r.timestamp-commit)*1000.:-1.),(unsigned long long)r.commitQpc,r.timestamp,(unsigned long long)r.ripAfter,functionRva,(unsigned long long)r.callers[0],(unsigned long long)(r.callers[0]>=gameBase?r.callers[0]-gameBase:0),(unsigned long long)r.callers[1],(unsigned long long)(r.callers[1]>=gameBase?r.callers[1]-gameBase:0),r.thread,before.x,before.y,before.z,after.x,after.y,after.z,vectors?ws::dot(b,w.normal):0,vectors?ws::dot(a,w.normal):0,vectors?b.y:0,vectors?a.y:0,w.normal.x,w.normal.y,w.normal.z,(unsigned long long)w.transform,unsigned(r.read),(unsigned long long)entry.id,unsigned(correlated),r.firstSurface,w.drops.load());
   if(entry.id){
    float up[3]{};std::memcpy(up,entry.source+0x10,12);char raw[129]{};hex(entry.source,64,raw);
    writeLog("RESET_ENTRY_SOURCE session=%llu entry_id=%llu thread_id=%lu qpc_ms=%.6f RCX=0x%llx RDX=0x%llx R8=0x%llx R9=0x%llx caller_rva=0x%llx source_up=(%.9g,%.9g,%.9g) source_0x40=%s readable=%u source_overridden_this_entry=%u snapshot_phase=AFTER_override_BEFORE_original entry_correlated=%u post_RSI_matches_entry_RDX=%u",(unsigned long long)w.session,(unsigned long long)entry.id,entry.tid,entry.qpc,(unsigned long long)entry.args[0],(unsigned long long)entry.args[1],(unsigned long long)entry.args[2],(unsigned long long)entry.args[3],(unsigned long long)(entry.caller>=gameBase?entry.caller-gameBase:0),up[0],up[1],up[2],raw,unsigned(entry.readable),unsigned(entry.overridden),unsigned(correlated),unsigned(r.gpr[6]==entry.args[1]));
   }
   writeLog("RESET_TRACE_EVENT_ADDRESS session=%llu qpc_ms=%.6f RIP=0x%llx writer_rva=0x%x return_address=0x%llx watched_address=0x%llx write_address=UNKNOWN_requires_anchored_offline_decode relative_us_from_last_override=%.3f last_override_known=%u",(unsigned long long)w.session,r.timestamp,(unsigned long long)r.ripAfter,functionRva,(unsigned long long)r.callers[0],(unsigned long long)(w.transform+0x10),(r.commitQpc?(r.timestamp-commit)*1000.:-1.),unsigned(r.commitQpc!=0));
   if(r.sequenceCandidate)writeLog("RESET_SEQUENCE_CANDIDATE session=%llu surface_anchor=(%.9g,%.9g,%.9g) surface_anchor_qpc_ms=%.6f final_world_qpc_ms=%.6f caveat=previous_observation_across_sequential_dword_stores_NOT_atomic_before",(unsigned long long)w.session,r.surfaceAnchor.x,r.surfaceAnchor.y,r.surfaceAnchor.z,r.surfaceAnchorTime,r.timestamp);
   if(r.resetCandidate&&r.dualCommitted)writeLog("NEW_RESET_AFTER_DUAL_OVERRIDE session=%llu writer_rva=0x%x caller_rva=0x%llx latency_ms=%.6f up_before=(%.9g,%.9g,%.9g) up_after=(%.9g,%.9g,%.9g)",(unsigned long long)w.session,functionRva,(unsigned long long)(r.callers[0]>=gameBase?r.callers[0]-gameBase:0),r.timestamp-commit,before.x,before.y,before.z,after.x,after.y,after.z);
   if(r.resetCandidate){
    float sourceUp[3]{};if(correlated&&entry.readable)std::memcpy(sourceUp,entry.source+16,12);
    writeLog("RESET_WRITER_CANDIDATE=1 session=%llu common_writer_reset=%u RESET_PATH_CORRELATED=%u reset_caller_rva=0x%llx reset_source_valid=%u reset_source_up=(%.9g,%.9g,%.9g) RESET_WRITER_VERIFIED=0",(unsigned long long)w.session,unsigned(functionRva==0x2cfc580),unsigned(correlated&&entry.readable),(unsigned long long)(r.callers[0]>=gameBase?r.callers[0]-gameBase:0),unsigned(correlated&&entry.readable),sourceUp[0],sourceUp[1],sourceUp[2]);
   }
   if(r.resetCandidate)writeLog("RESET_WRITER_CANDIDATE session=%llu writer_function_rva=0x%x caller_rva=0x%llx thread_id=%lu reset_latency_ms=%.6f reset_from_first_surface_ms=%.6f source_entry_correlated=%u RESET_WRITER_VERIFIED=0 requires_offline_instruction_validation_and_real_A_E_repeat=1",(unsigned long long)w.session,functionRva,(unsigned long long)(r.callers[0]>=gameBase?r.callers[0]-gameBase:0),r.thread,r.timestamp-commit,r.firstSurface?r.timestamp-r.firstSurface:-1.,unsigned(correlated));
   if(f&&image==gameBase)writeLog("RESET_WRITER_FUNCTION session=%llu begin_rva=0x%x end_rva=0x%x size=%u SHA256=offline_from_private_captured_function",(unsigned long long)w.session,f->BeginAddress,f->EndAddress,f->EndAddress-f->BeginAddress);
  }
  unsigned id=writerCount;for(unsigned i=0;i<writerCount;++i)if(writers[i].pc==r.ripAfter){id=i;break;}
  if(id==writerCount&&writerCount<writers.size()){writers[writerCount++].pc=r.ripAfter;}
  if(id>=writers.size()){writeLog("orientation_writer_unique_limit limit=32");continue;}auto& writer=writers[id];++writer.hits;if(w.state==1)++writer.run;else ++writer.crawl;
  writeLog("ORIENTATION_WRITER_EVENT writer_id=%u window=%u state=%s session=%llu thread_id=%lu rip=0x%llx rip_semantics=AFTER_DATA_BREAKPOINT_INSTRUCTION module=%s module_base=0x%llx module_offset_after=0x%llx caller=0x%llx caller2=0x%llx caller_valid=%u qpc_ms=%.6f watched_address=0x%llx transform=0x%llx transform_offset=0x10 slot_mask=%u old_observed_up=(%.9g,%.9g,%.9g) new_observed_up=(%.9g,%.9g,%.9g) old_value_semantics=previous_observation_NOT_atomic_instruction_before read_valid=%u N398=(%.6f,%.6f,%.6f)",id+1,w.id,w.state!=2?"WallRun":"CrawlCandidate",static_cast<unsigned long long>(w.session),r.thread,static_cast<unsigned long long>(r.ripAfter),name,static_cast<unsigned long long>(base),static_cast<unsigned long long>(r.ripAfter-base),static_cast<unsigned long long>(r.callers[0]),static_cast<unsigned long long>(r.callers[1]),unsigned(r.callerValid),r.timestamp,static_cast<unsigned long long>(w.transform+0x10),static_cast<unsigned long long>(w.transform),r.slotMask,r.oldUp[0],r.oldUp[1],r.oldUp[2],r.newUp[0],r.newUp[1],r.newUp[2],unsigned(r.read),w.normal.x,w.normal.y,w.normal.z);
  writeLog("ORIENTATION_WRITER_REGS writer_id=%u qpc_ms=%.6f post_gpr_RAX_RCX_RDX_RBX_RSP_RBP_RSI_RDI_R8_R9_R10_R11_R12_R13_R14_R15=%llx,%llx,%llx,%llx,%llx,%llx,%llx,%llx,%llx,%llx,%llx,%llx,%llx,%llx,%llx,%llx",id+1,r.timestamp,r.gpr[0],r.gpr[1],r.gpr[2],r.gpr[3],r.gpr[4],r.gpr[5],r.gpr[6],r.gpr[7],r.gpr[8],r.gpr[9],r.gpr[10],r.gpr[11],r.gpr[12],r.gpr[13],r.gpr[14],r.gpr[15]);
  char xmmText[513]{};hex(r.xmm,256,xmmText);writeLog("orientation_writer_xmm writer_id=%u qpc_ms=%.6f XMM0_15_hex=%s XMM_valid=%u YMM_upper_captured=0",id+1,r.timestamp,xmmText,unsigned(r.xmmValid));
  if(writer.hits==1&&!w.reset.multithread){
   unsigned char bytes[64]{};char text[129]{};if(r.ripAfter>=64&&readMemory(r.ripAfter-64,bytes,64)){hex(bytes,64,text);writeLog("orientation_writer_instruction_window writer_id=%u base=0x%llx ends_at=0x%llx bytes=%s",id+1,r.ripAfter-64,r.ripAfter,text);}
   DWORD64 image=0;auto* f=RtlLookupFunctionEntry(r.ripAfter-1,&image,nullptr);
   if(f&&f->EndAddress>f->BeginAddress&&f->EndAddress-f->BeginAddress<=0x4000){auto begin=image+f->BeginAddress;unsigned size=f->EndAddress-f->BeginAddress;std::array<unsigned char,0x4000> code{};if(readMemory(begin,code.data(),size)){writeLog("orientation_writer_function writer_id=%u image=0x%llx begin=0x%llx size=%u",id+1,image,begin,size);for(unsigned offset=0;offset<size;offset+=64){unsigned count=std::min(64u,size-offset);hex(code.data()+offset,count,text);writeLog("orientation_writer_code writer_id=%u offset=0x%x bytes=%s",id+1,offset,text);}}}
  }
  writeLog("orientation_writer_summary writer_id=%u hit_count=%u WallRun_hits=%u CrawlCandidate_hits=%u verified_WallCrawl_state=0",id+1,writer.hits,writer.run,writer.crawl);
 }
 bool entryDrainSafe=head.load()==tail.load();for(unsigned i=0;i<publishedWindowCount.load(std::memory_order_acquire);++i)if(windows[i].state==5&&windows[i].positionInflight.load())entryDrainSafe=false;
 if(entryDrainSafe)writer_entry::drain();
 for(unsigned i=0;i<publishedWindowCount.load(std::memory_order_acquire);++i){auto& w=windows[i];if(w.state!=5||!w.positionClosed.load(std::memory_order_acquire)||w.positionInflight.load()||head.load()!=tail.load()||w.positionSummary)continue;w.positionSummary=true;
  writeLog("MOVEMENT_LINEAGE_SUMMARY session=%llu source_capture_ready=%u eligible_hits=%u reconstruction_attempt_cap=16 attempts=%u complete_writer_CFG=%u source_prefix_proven=%u direct_call_verified=%u caller2_direct_relation_verified=%u true_entry_snapshot=0 writable_source_proven=0 Position_writes_by_mod=0 reason=entry_snapshot_typed_caller_W_and_exclusive_effects_unproven",(unsigned long long)w.session,unsigned(movementLineage.ready),w.lineageAttempts.load(),std::min(16u,w.lineageAttempts.load()),unsigned(movementLineage.writer.complete),unsigned(movementLineage.writer.sourceMap),unsigned(movementLineage.call.verified),unsigned(movementLineage.caller2Verified));
  writeLog("POSITION_WRITER_SUMMARY session=%llu watch_armed=%u threads_seen=%u threads_installed=%u unique_writers=%u total_write_events=%u unknown_writer_events=%u writer_table_overflow=%u dropped=%u cleanup_restored=%u cleanup_pending=%u complete_thread_coverage=0 rescan_count=0 event_cap=256 per_writer_print_cap=4 Position_writes_by_mod=0",(unsigned long long)w.session,unsigned(w.installedThreads>0),w.seen,w.installedThreads,w.positionWriterCount,w.hits.load(),w.positionUnknown,w.positionOverflow,w.drops.load(),unsigned(w.positionCleanup.load()==0),w.positionCleanup.load());
  for(unsigned j=0;j<w.positionWriterCount;++j){auto& writer=w.positionWriters[j];writeLog("POSITION_WRITER_HITS session=%llu writer_id=%u writer_rva=0x%llx rip_rva=0x%llx hits=%u",(unsigned long long)w.session,j+1,(unsigned long long)gameRva(writer.function),(unsigned long long)gameRva(writer.pc),writer.hits);}
 }

}
bool configureReset(bool verified,std::uintptr_t base,std::size_t size,bool prearmed,bool multithread){
 if(usedWindows||externalReset)return false;
 externalReset=true;config={true,multithread?900u:prearmed?500u:30u,8,multithread?512u:256u,multithread?128u:prearmed?1u:32u};gameBase=base;gameSize=size;
 admitted=verified&&(!multithread||prearmed)&&base&&size&&QueryPerformanceFrequency(&frequency)&&frequency.QuadPart>0&&!IsDebuggerPresent();
 if(admitted&&!veh)veh=AddVectoredExceptionHandler(1,handler);
 admitted=admitted&&veh;killed=!admitted;
 if(admitted)configureMovementLineage();
 writeLog("RESET_TRACE_MODE prearmed=%u prearm_elapsed=0.60 override_elapsed=0.70 end_elapsed=%.2f primary_first=1 multithread=%u expand_elapsed=0.85 max_threads=%u",unsigned(prearmed),multithread?1.50:1.10,unsigned(multithread),config.maxThreads);
 writeLog("RESET_TRACE_CONFIG admitted=%u window_ms=%u deadline_native_elapsed_ms=%u max_sessions=8 max_events=%u max_threads=%u once_per_session=1 source_callback_wait=0 additional_game_hooks=0 original_tailjump_preserved=1",unsigned(admitted),config.windowMs,prearmed?(multithread?1500u:1100u):0u,config.maxEvents,config.maxThreads);return admitted;
}
bool beginPosition(const reset_trace::Request& r,const Snapshot& s){
 auto tick=GetTickCount64();
 if(!externalReset||!admitted||killed.load()||!r.position||!r.prearmed||tick<r.initMs||tick-r.initMs<1550||tick-r.initMs>3000||activeLeases()||current>=0||r.hero!=s.hero||r.transform!=s.transform||!r.basis.valid||!ws::diag::measure({r.basis.right,r.basis.up,r.basis.forward}).properRotation){writeLog("POSITION_WATCH_SKIPPED session=%llu reason=identity_timing_capacity_or_previous_Up_cleanup_gate",(unsigned long long)r.session);return false;}
 if(!writer_entry::start(r))writeLog("MOVEMENT_ENTRY_DIAGNOSTIC_START session=%llu installed=0 reason=entry_gate_live_identity_integrity_capacity_or_install_refused orientation_continues=1",(unsigned long long)r.session);
 nextReset=r;bool ok=begin(s,5,r.session,r.owner,r.normal,r.thread);if(ok){publishedReset.store(current,std::memory_order_release);if(!writer_entry::activate())writeLog("MOVEMENT_ENTRY_DIAGNOSTIC_ACTIVE session=%llu capture_ready=0 reason=entry_hook_or_fresh_identity_unavailable",(unsigned long long)r.session);}else {pollReset(false,"position_arm_failed");writer_entry::stop();}return ok;
}
void positionCopyCommit(std::uintptr_t transform) noexcept {
 auto error=GetLastError();auto index=publishedReset.load();if(index>=0){auto& w=windows[unsigned(index)];if(w.state==5&&w.transform==transform&&w.capture.load()){LARGE_INTEGER q{};QueryPerformanceCounter(&q);w.copyCommit=q.QuadPart;}}SetLastError(error);
}
bool beginReset(const reset_trace::Request& r,const Snapshot& s){
 double commit=frequency.QuadPart?1000.*double(r.commitQpc)/double(frequency.QuadPart):0;
 bool deadline=r.prearmed?(GetTickCount64()<r.initMs||GetTickCount64()-r.initMs>=(r.verifyDual?1500u:1100u)):now()>=commit+30.;
 if(!externalReset||!admitted||killed.load()||now()<=0||deadline||(r.multithread&&(!r.prearmed||!r.verifyDual))||!ws::diag::actorAxes(s.prefix).valid||r.hero!=s.hero||r.transform!=s.transform){writeLog("RESET_TRACE_SKIPPED session=%llu reason=gate_or_identity_or_arm_deadline arm_delay_us=%.3f",(unsigned long long)r.session,(now()-commit)*1000.);return false;}
 nextReset=r;bool ok=begin(s,r.prearmed?4:3,r.session,r.owner,r.normal,r.thread);
 if(r.prearmed)writeLog("RESET_TRACE_WINDOW session=%llu prearmed=1 elapsed=%.6f deadline_elapsed=%.2f primary_thread=%u armed=%u primary_only=%u expand_to_process_threads_at=0.85 unchanged_writes_suppressed=%u coordinator_window_active=%u",(unsigned long long)r.session,double(GetTickCount64()-r.initMs)/1000.,r.verifyDual?1.50:1.10,r.thread,unsigned(ok&&windows[unsigned(current)].primaryInstalled.load()),unsigned(!r.multithread),unsigned(r.verifyDual),unsigned(ok));
 else writeLog("RESET_TRACE_WINDOW session=%llu override_commit_qpc=%llu qpc_frequency=%llu arm_delay_us=%.3f deadline_from_commit_ms=30 armed=%u initial_applied_up=(%.9g,%.9g,%.9g) coverage=bounded_current_threads_NOT_complete_initial_write_may_precede_arm",(unsigned long long)r.session,(unsigned long long)r.commitQpc,(unsigned long long)frequency.QuadPart,(now()-commit)*1000.,unsigned(ok),r.appliedUp.x,r.appliedUp.y,r.appliedUp.z);
 if(ok)publishedReset.store(current,std::memory_order_release);
 if(!ok)pollReset(false);
 return ok;
}
bool pollReset(bool valid,const char* invalidReason){
 if(current<0){if(activeLeases())for(unsigned i=0;i<usedWindows;++i)release(windows[i]);return false;}
 auto& w=windows[unsigned(current)];auto tick=GetTickCount64();double time=now();
 const char* reason=!valid?invalidReason:killed.load()?"killed":time<=0?"qpc_failed":time>=w.end?"deadline":tick<w.reset.initMs?"tick_before_init":nullptr;
 if(reason)closeCapture(w,reason);
 // >= handles sparse/coalesced samples. Only verified live windows expand;
 // cancellation/identity/guard/kill/deadline never reopens capture.
 if(w.capture.load()&&w.reset.multithread&&!w.expandAttempted.load()&&tick-w.reset.initMs>=850){w.expandRequested=true;expandProcessWatch(w);}
 if(!w.capture.load()){publishedReset=-1;release(w);current=-1;return false;}
 return true;
}
void cancelReset() noexcept {
 auto index=publishedReset.load(std::memory_order_acquire);if(index>=0)closeCapture(windows[unsigned(index)],"lifecycle_cancel");
}
bool resetCommitted(const reset_trace::Request& r) noexcept {
 auto index=publishedReset.load(std::memory_order_acquire);if(index<0)return false;
 auto& w=windows[unsigned(index)];
 if(w.state==5&&w.session==r.session&&w.reset.generation==r.generation&&w.transform==r.transform){if(r.resetCopy)w.copyCommit=r.commitQpc;else w.commonCommit=r.commitQpc;return false;}
 if(w.state!=4||w.session!=r.session||w.reset.generation!=r.generation||w.transform!=r.transform)return false;
 // Commit timestamps remain visible even while another thread's VEH owns the
 // diagnostic record lock. Contention can drop a log, never an A/B override.
 w.lastOverride=r.commitQpc;
 if(r.resetCopy)w.copyCommit=r.commitQpc;else w.commonCommit=r.commitQpc;
 if(eventLock.test_and_set(std::memory_order_acquire)){++w.drops;if(!w.reset.multithread)w.capture=false;return false;}
 bool armed=w.capture.load()&&!killed.load()&&w.primaryInstalled.load()&&w.reset.thread==GetCurrentThreadId()&&now()<w.end&&r.overrideBeginQpc&&w.primaryArmedAt<1000.*double(r.overrideBeginQpc)/double(frequency.QuadPart);
 w.lastOverride=r.commitQpc;
 if(r.resetCopy)w.copyCommit=r.commitQpc;else w.commonCommit=r.commitQpc;
 auto h=commonHead.load();if(h-commonTail.load()<commonRecords.size()){CommonRecord record{};record.window=unsigned(index);record.status=true;record.armed=armed;record.elapsed=double(GetTickCount64()-w.reset.initMs)/1000.;commonRecords[h%commonRecords.size()]=record;commonHead.store(h+1,std::memory_order_release);}else{++w.drops;++w.commonDrops;if(!w.reset.multithread)w.capture=false;}
 eventLock.clear(std::memory_order_release);return armed;
}
void beginResetCopyEntry() noexcept {currentCopyEntry=0;}
void resetCopyEntry(const path_b::Entry& entry) noexcept {
 if(killed.load())return;
 auto index=publishedReset.load(std::memory_order_acquire);if(index<0)return;auto& w=windows[unsigned(index)];
 if(!w.capture.load()||entry.session!=w.session||entry.generation!=w.reset.generation||entry.owner!=w.owner||entry.rcx!=w.transform||now()>=w.end)return;
 if(eventLock.test_and_set(std::memory_order_acquire)){++w.drops;return;}
 for(auto& l:leases)if(l.tid.load(std::memory_order_acquire)==entry.thread&&l.window==unsigned(index)){l.copyEntry=entry;currentCopyEntry=entry.id;break;}
 eventLock.clear(std::memory_order_release);
}
void resetEntry(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t caller,bool overridden,std::uint64_t nativeUpdate,std::uint64_t generation,std::uintptr_t owner) noexcept {
 if(!externalReset||killed.load())return;
 auto index=publishedReset.load(std::memory_order_acquire);if(index<0)return;auto& w=windows[unsigned(index)];
 if(!w.capture.load()||w.transform!=rcx||now()>=w.end)return;
 DWORD tid=GetCurrentThreadId();Entry e{};e.nativeUpdate=nativeUpdate;e.generation=generation;e.owner=owner;e.tid=tid;e.args[0]=rcx;e.args[1]=rdx;e.args[2]=r8;e.args[3]=r9;e.caller=caller;e.qpc=now();e.overridden=overridden;
 // One bounded read, no pointer chasing or guessed source ABI on invalid data.
 ws::source_math::Matrix matrix{};e.readable=rdx&&rdx<=UINTPTR_MAX-64&&readMemory(rdx,&matrix,64)&&ws::source_math::valid(matrix);
 if(e.readable)std::memcpy(e.source,&matrix,64);
 readMemory(rcx+0x10,&e.actorUp,12);
 if(eventLock.test_and_set(std::memory_order_acquire)){++w.drops;if(!w.reset.multithread)w.capture=false;return;}
 e.id=++entrySerial;
 for(unsigned i=leasesCap;i>0;--i){auto& l=leases[i-1];if(l.tid.load(std::memory_order_acquire)==tid&&l.window==unsigned(index)){l.entry=e;break;}}
 if(w.state==5&&overridden&&generation==w.reset.generation&&owner==w.owner){LARGE_INTEGER q{};QueryPerformanceCounter(&q);w.commonCommit=q.QuadPart;}
 if(w.state==4){auto h=commonHead.load();if(h-commonTail.load()<commonRecords.size()){CommonRecord record{};record.window=unsigned(index);record.entry=e;record.elapsed=double(GetTickCount64()-w.reset.initMs)/1000.;commonRecords[h%commonRecords.size()]=record;commonHead.store(h+1,std::memory_order_release);}else{++w.drops;++w.commonDrops;if(!w.reset.multithread)w.capture=false;}}
 eventLock.clear(std::memory_order_release);
}

}
