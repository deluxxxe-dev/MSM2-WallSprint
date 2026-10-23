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
extern "C" bool ws_guarded_call(void (*)(void*),void*);
namespace {
using namespace msm2;
constexpr unsigned windowsCap=8,threadsCap=128,leasesCap=windowsCap*(threadsCap+1),ringCap=1024;
constexpr DWORD64 ownMask=0xff000full,watchBits=0xd90005ull;
struct Entry {
 std::uint64_t id{},nativeUpdate{},generation{};std::uintptr_t owner{};DWORD tid{};double qpc{};std::uintptr_t args[4]{},caller{};unsigned char source[64]{};bool readable{},overridden{};ws::Vec3 actorUp{};
};
struct Window {
 std::atomic<bool> capture{false};unsigned id{},state{};std::uint64_t session{};
 std::uintptr_t hero{},transform{},owner{};ws::Vec3 normal{},startUp{},oldObserved{};
 msm2::reset_trace::Request reset{};double firstSurface{},surfaceAnchorTime{};ws::Vec3 surfaceAnchor{};
 std::atomic<std::uint64_t> lastOverride{0},commonCommit{0},copyCommit{0};std::atomic<bool> primaryInstalled{false};double primaryArmedAt{};
 double start{},end{};std::atomic<unsigned> hits{0},drops{0};std::atomic_flag cache=ATOMIC_FLAG_INIT;
 bool expanded{},thirdReported{};unsigned seen{},eligible{},installedThreads{},foreignThreads{},failedThreads{},selfSkipped{},capExceeded{};
 struct Skip {DWORD tid{};const char* reason{};};std::array<Skip,16> skipSummary{};unsigned skips{};double expandedElapsed{};
};
struct Lease {
 std::atomic<DWORD> tid{0};unsigned window{};HANDLE handle{};bool installed{},suspended{};
 Entry entry{};path_b::Entry copyEntry{};DWORD64 old0{},old1{},old6{},old7{},up{};
};
struct Event {
 unsigned window{},slotMask{};DWORD thread{};double timestamp{};DWORD64 ripAfter{},rsp{},callers[2]{},gpr[16]{};
 Entry entry{};path_b::Entry copyEntry{};double firstSurface{};bool resetCandidate{},dualCommitted{},sequenceCandidate{},thirdCandidate{};double surfaceAnchorTime{};ws::Vec3 surfaceAnchor{};std::uint64_t commitQpc{},commonQpc{},copyQpc{};
 float oldUp[3]{},newUp[3]{};unsigned char xmm[256]{};bool read{},callerValid{},xmmValid{};
};
struct CommonRecord {unsigned window{};Entry entry{};bool status{},armed{};double elapsed{};};
std::array<CommonRecord,1024> commonRecords;std::atomic<unsigned> commonHead{0},commonTail{0};
std::atomic<int> publishedReset{-1};
thread_local std::uint64_t currentCopyEntry{};
bool externalReset{};msm2::reset_trace::Request nextReset{};std::uint64_t entrySerial{};
trace::Config config;bool admitted{},haveDeinit{},wasActive{};std::atomic<bool> killed{true};
std::uintptr_t gameBase{};std::size_t gameSize{};LARGE_INTEGER frequency{};
std::array<Window,windowsCap> windows;std::array<Lease,leasesCap> leases;unsigned usedWindows{},usedLeases{};int current=-1;
std::array<Event,ringCap> events;std::atomic<unsigned> head{0},tail{0};std::atomic_flag eventLock=ATOMIC_FLAG_INIT;
PVOID veh{};double lastSample{},deinitMs{};std::uint64_t lastSession{},runTraced{},crawlTraced{};
Snapshot lastHero{};ws::Vec3 lastNormal{};bool normalValid{};unsigned crawlStable{};
double now(){LARGE_INTEGER q{};if(!QueryPerformanceCounter(&q)||!frequency.QuadPart)return 0;return 1000.*double(q.QuadPart)/double(frequency.QuadPart);}
unsigned own(const CONTEXT& c,const Lease& l){
 unsigned mask=0;
 if(c.Dr0==l.up&&(c.Dr7&0xf0003ull)==0x90001ull)mask|=1;
 if(c.Dr1==l.up+8&&(c.Dr7&0xf0000cull)==0xd00004ull)mask|=2;
 return mask;
}
void restore(CONTEXT& c,const Lease& l,bool snapshot=false){
 auto mask=own(c,l);
 if(mask&1){c.Dr0=l.old0;c.Dr7=(c.Dr7&~0xf0003ull)|(l.old7&0xf0003ull);}
 if(mask&2){c.Dr1=l.old1;c.Dr7=(c.Dr7&~0xf0000cull)|(l.old7&0xf0000cull);}
 if(snapshot&&mask==3&&(c.Dr6&0xe00c)==(l.old6&0xe00c))c.Dr6=l.old6;
 else c.Dr6=(c.Dr6&~DWORD64(mask))|(snapshot?(l.old6&mask):0);
}
struct Unwind {CONTEXT c;DWORD64 callers[2]{};unsigned count{};};
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
  u.callers[u.count++]=u.c.Rip;
 }
}
LONG CALLBACK handler(EXCEPTION_POINTERS* e){
 if(!e||!e->ExceptionRecord||!e->ContextRecord||e->ExceptionRecord->ExceptionCode!=EXCEPTION_SINGLE_STEP)return EXCEPTION_CONTINUE_SEARCH;
 auto& c=*e->ContextRecord;if((c.ContextFlags&CONTEXT_DEBUG_REGISTERS)!=CONTEXT_DEBUG_REGISTERS)return EXCEPTION_CONTINUE_SEARCH;auto mask=unsigned(c.Dr6&3);if(!mask)return EXCEPTION_CONTINUE_SEARCH;
 DWORD tid=GetCurrentThreadId();Lease* lease=nullptr;unsigned claimed=0;bool mixed=false;
 // Immutable records also retire delayed old contexts; match each owned slot
 // independently so a debugger taking one slot cannot orphan the other.
 for(unsigned i=leasesCap;i>0;--i){auto& l=leases[i-1];if(l.tid.load(std::memory_order_acquire)!=tid)continue;auto bits=own(c,l)&mask&~claimed;if(!bits)continue;if(lease&&lease->up!=l.up)mixed=true;if(!lease)lease=&l;claimed|=bits;}
 if(!lease)return EXCEPTION_CONTINUE_SEARCH;
 auto& w=windows[lease->window];double time=now();
 if(mixed){killed=true;for(unsigned i=leasesCap;i>0;--i){auto& l=leases[i-1];if(l.tid.load()==tid&&own(c,l))restore(c,l);}}
 else if(own(c,*lease)!=3||killed.load()||!w.capture.load(std::memory_order_acquire)||(time<=0||time>w.end)){w.capture=false;restore(c,*lease);}
 else if(w.hits.load()>=config.maxEvents){w.capture=false;restore(c,*lease);}
 else if(eventLock.test_and_set(std::memory_order_acquire)){++w.drops;if(!w.reset.multithread){w.capture=false;restore(c,*lease);}}
 else {
  auto h=head.load(std::memory_order_relaxed);auto t=tail.load(std::memory_order_acquire);
  if(h-t>=ringCap){++w.drops;w.capture=false;restore(c,*lease);}
  else {
   Event r{};r.window=lease->window;r.slotMask=claimed;r.thread=tid;r.timestamp=time;r.ripAfter=c.Rip;r.rsp=c.Rsp;
   DWORD64 regs[]={c.Rax,c.Rcx,c.Rdx,c.Rbx,c.Rsp,c.Rbp,c.Rsi,c.Rdi,c.R8,c.R9,c.R10,c.R11,c.R12,c.R13,c.R14,c.R15};std::memcpy(r.gpr,regs,sizeof(regs));r.xmmValid=(c.ContextFlags&CONTEXT_FLOATING_POINT)==CONTEXT_FLOATING_POINT;if(r.xmmValid)std::memcpy(r.xmm,&c.Xmm0,256);
   SIZE_T done=0;std::memcpy(r.oldUp,&w.oldObserved,12);r.read=ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(lease->up),r.newUp,12,&done)&&done==12;
   if(r.read){
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
   bool unchanged=w.reset.verifyDual&&r.read&&!std::memcmp(r.oldUp,r.newUp,12);
   if(!unchanged){
   ++w.hits;
   Unwind u{};u.c=c;if(ws_guarded_call(unwind,&u)&&u.count){r.callerValid=true;std::memcpy(r.callers,u.callers,sizeof(r.callers));}
   events[h%ringCap]=r;head.store(h+1,std::memory_order_release);
   if(r.resetCandidate&&!w.reset.characterize&&!w.reset.multithread){w.capture=false;restore(c,*lease);}
   }
  }
  eventLock.clear(std::memory_order_release);
 }
 c.Dr6&=~DWORD64(claimed);
 // Never consume another debugger's single-step/hardware slots.
 return (c.Dr6&(DWORD64(0xf)|DWORD64(0xe000)))?EXCEPTION_CONTINUE_SEARCH:EXCEPTION_CONTINUE_EXECUTION;
}
bool resume(Lease& l){
 if(!l.suspended)return true;
 for(unsigned i=0;i<3;++i)if(ResumeThread(l.handle)!=DWORD(-1)){l.suspended=false;return true;}
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
  if(SuspendThread(l.handle)==DWORD(-1)){++pending;continue;}
  l.suspended=true;CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;bool got=GetThreadContext(l.handle,&c),ok=false;
  if(got){if(own(c,l)){restore(c,l,true);ok=SetThreadContext(l.handle,&c);}else {ok=true;++foreign;}}
  // No allocation, logging, SDK calls or user locks between Suspend and Resume.
  bool resumed=resume(l);
  if(ok&&resumed){l.installed=false;CloseHandle(l.handle);l.handle=nullptr;++restored;}else ++pending;
 }
 writeLog("orientation_trace_stop window=%u captured=%u dropped=%u restored=%u already_cleared_or_foreign=%u cleanup_pending=%u no_page_protection_change=1",w.id,std::min(w.hits.load(),config.maxEvents),w.drops.load(),restored,foreign,pending);
 if(w.reset.multithread){watchSummary(w,"stopped");writeLog("MULTITHREAD_UP_WATCH_RESTORE session=%llu restored=%u already_cleared_or_foreign=%u cleanup_pending=%u modified_slots=DR0_DR1 exact_saved_owned_slot_state=1",(unsigned long long)w.session,restored,foreign,pending);}
}
bool activeLeases(){for(unsigned i=0;i<usedLeases;++i)if(leases[i].installed||leases[i].suspended)return true;return false;}
void watchSummary(const Window& w,const char* phase){
 bool complete=w.expanded&&w.eligible>0&&!w.capExceeded&&!w.failedThreads&&!w.foreignThreads&&w.installedThreads==w.eligible&&w.drops.load()==0;
 writeLog("MULTITHREAD_UP_WATCH session=%llu phase=%s process_threads_seen=%u eligible=%u installed=%u skipped_foreign_dr=%u failed=%u complete_thread_coverage=%u window_start_elapsed=%.6f window_end_elapsed=1.500 cap=128 cap_exceeded=%u coordinator_self_excluded=%u coverage_scope=snapshot_eligible_threads_only rescan_count=0 dropped=%u",(unsigned long long)w.session,phase,w.seen,w.eligible,w.installedThreads,w.foreignThreads,w.failedThreads,unsigned(complete),w.expandedElapsed,w.capExceeded,w.selfSkipped,w.drops.load());
 for(unsigned i=0;i<std::min(w.skips,unsigned(w.skipSummary.size()));++i)writeLog("MULTITHREAD_UP_WATCH_SKIP session=%llu thread_id=%lu reason=%s summary_index=%u suppressed=%u",(unsigned long long)w.session,w.skipSummary[i].tid,w.skipSummary[i].reason,i,unsigned(w.skips>w.skipSummary.size()?w.skips-w.skipSummary.size():0));
}
struct Install {Window* window;Lease* lease;CONTEXT previous{};bool got{},ok{},foreign{};};
void installSuspended(void* raw){
 auto& p=*static_cast<Install*>(raw);auto& l=*p.lease;auto& w=*p.window;p.previous.ContextFlags=CONTEXT_DEBUG_REGISTERS;
 p.got=GetThreadContext(l.handle,&p.previous);if(!p.got)return;
 p.foreign=(p.previous.Dr7&0xf)||(p.previous.Dr7&(1ull<<13));if(p.foreign)return;
 auto c=p.previous;l.old0=c.Dr0;l.old1=c.Dr1;l.old6=c.Dr6;l.old7=c.Dr7;l.up=w.transform+0x10;
 // Publish immutable ownership before a successful SetThreadContext can be
 // resumed. No allocation/log/SDK/user lock occurs in this suspended region.
 l.tid.store(GetThreadId(l.handle),std::memory_order_release);
 c.Dr0=l.up;c.Dr1=l.up+8;c.Dr6&=~DWORD64(3);c.Dr7=(c.Dr7&~ownMask)|watchBits;
 l.installed=true;bool changed=SetThreadContext(l.handle,&c);CONTEXT readback{};readback.ContextFlags=CONTEXT_DEBUG_REGISTERS;
 p.ok=changed&&GetThreadContext(l.handle,&readback)&&own(readback,l)==3;
 if(p.ok&&l.tid.load()==w.reset.thread){w.primaryArmedAt=now();w.primaryInstalled.store(true,std::memory_order_release);}
 if(!p.ok){CONTEXT rollback{};rollback.ContextFlags=CONTEXT_DEBUG_REGISTERS;if(GetThreadContext(l.handle,&rollback)){restore(rollback,l,true);if(SetThreadContext(l.handle,&rollback))l.installed=false;}}
}
const char* installProcessThread(Window& w,DWORD tid){
 for(unsigned i=0;i<usedLeases;++i){auto& l=leases[i];if(l.window+1==w.id&&l.tid.load()==tid&&l.installed&&l.handle&&WaitForSingleObject(l.handle,0)!=WAIT_OBJECT_0)return nullptr;}
 if(usedLeases>=leasesCap)return "lease_cap";
 HANDLE thread=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_CONTEXT|THREAD_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,tid);if(!thread)return "open_thread_failed";
 if(GetProcessIdOfThread(thread)!=GetCurrentProcessId()){CloseHandle(thread);return "process_identity_mismatch";}
 auto depth=SuspendThread(thread);if(depth==DWORD(-1)){CloseHandle(thread);return "suspend_failed";}
 auto& l=leases[usedLeases++];l.window=w.id-1;l.handle=thread;l.suspended=true;Install install{&w,&l};
 bool protectedOk=true;
 if(depth==0&&w.capture.load())protectedOk=ws_guarded_call(installSuspended,&install);
 bool resumed=resume(l);
 if(!l.installed&&resumed){CloseHandle(l.handle);l.handle=nullptr;}
 if(!resumed){w.capture=false;return "resume_failed_cleanup_retained";}
 if(!protectedOk){w.capture=false;return "setup_exception_cleanup_retained";}
 if(install.ok)return nullptr;
 return depth?"already_suspended":install.foreign?"foreign_dr":!w.capture.load()?"window_closed":!install.got?"get_context_failed":"set_context_or_readback_failed";
}
void expandProcessWatch(Window& w){
 w.expanded=true;w.expandedElapsed=double(GetTickCount64()-w.reset.initMs)/1000.;
 auto skip=[&](DWORD tid,const char* reason){if(w.skips<w.skipSummary.size())w.skipSummary[w.skips]={tid,reason};++w.skips;};
 HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
 if(snapshot==INVALID_HANDLE_VALUE){++w.failedThreads;skip(0,"thread_snapshot_failed");watchSummary(w,"armed");return;}
 // Snapshot into a fixed local list BEFORE suspending anyone; the helper is
 // excluded because its code only observes Hero memory and cannot suspend itself.
 std::array<DWORD,threadsCap> ids{};unsigned count=0;THREADENTRY32 te{};te.dwSize=sizeof(te);
 if(!Thread32First(snapshot,&te)){++w.failedThreads;skip(0,"thread_enumeration_failed");}
 else {do {if(te.th32OwnerProcessID!=GetCurrentProcessId())continue;
  if(++w.seen>threadsCap){w.capExceeded=1;break;}
  if(te.th32ThreadID==GetCurrentThreadId()){++w.selfSkipped;continue;}
  ids[count++]=te.th32ThreadID;
 }while(Thread32Next(snapshot,&te));if(!w.capExceeded&&GetLastError()!=ERROR_NO_MORE_FILES){++w.failedThreads;skip(0,"thread_enumeration_incomplete");}}
 CloseHandle(snapshot);w.eligible=count;
 for(unsigned i=0;i<count;++i){if(!w.capture.load()||now()>=w.end){w.failedThreads+=count-i;skip(ids[i],"window_or_setup_deadline");break;}
  const char* reason=installProcessThread(w,ids[i]);if(!reason)++w.installedThreads;else {if(!std::strcmp(reason,"foreign_dr"))++w.foreignThreads;else ++w.failedThreads;skip(ids[i],reason);}
 }
 watchSummary(w,"armed");
}
bool begin(const Snapshot& s,unsigned state,std::uint64_t session,std::uintptr_t owner,ws::Vec3 normal,unsigned primary){
 if(s.transform%16||s.transform>UINTPTR_MAX-0x30)return false;
 if(usedWindows>=config.maxWindows||usedWindows>=windowsCap||activeLeases()||IsDebuggerPresent())return false;
 if(!veh){veh=AddVectoredExceptionHandler(1,handler);if(!veh){writeLog("orientation_trace_unavailable reason=VEH_registration_failed");return false;}}
 DWORD candidates[threadsCap]{};unsigned count=0,enumerated=0;auto self=GetCurrentThreadId();
 if(primary&&primary!=self)candidates[count++]=primary;
 // Prearmed capture deliberately covers only the known primary thread. No
 // Toolhelp enumeration or other suspension may delay securing this thread.
 if(state!=4){
 HANDLE snap=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);if(snap==INVALID_HANDLE_VALUE)return false;
 THREADENTRY32 te{};te.dwSize=sizeof(te);if(Thread32First(snap,&te))do{if(te.th32OwnerProcessID!=GetCurrentProcessId()||te.th32ThreadID==self)continue;++enumerated;if(count<config.maxThreads&&std::find(candidates,candidates+count,te.th32ThreadID)==candidates+count)candidates[count++]=te.th32ThreadID;}while(Thread32Next(snap,&te));CloseHandle(snap);
 }
 auto index=usedWindows++;auto& w=windows[index];w.id=index+1;w.state=state;w.session=session;w.hero=s.hero;w.transform=s.transform;w.owner=owner;w.normal=normal;auto axes=ws::diag::actorAxes(s.prefix);w.startUp=w.oldObserved=axes.up;w.start=now();w.end=w.start+config.windowMs;
 if(state>=3){w.reset=nextReset;w.lastOverride=w.reset.commitQpc;double commit=1000.*double(w.reset.commitQpc)/double(frequency.QuadPart);w.end=state==4?w.start+std::max(0.,(w.reset.verifyDual?1500.:1100.)-double(GetTickCount64()-w.reset.initMs)):commit+30.;ws::Vec3 u{};if(ws::unit(axes.up,u)&&ws::dot(u,normal)>=.5f)w.firstSurface=w.start;}
 w.capture=w.start<w.end;
 if(state==4)publishedReset.store(int(index),std::memory_order_release);
 unsigned installed=0,busy=0,failed=0;
 for(unsigned n=0;n<count&&usedLeases<leasesCap&&w.capture.load()&&now()<w.end;++n){
  if(w.reset.multithread){const char* reason=installProcessThread(w,candidates[n]);if(!reason)++installed;else if(!std::strcmp(reason,"foreign_dr"))++busy;else ++failed;continue;}
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
   c.Dr0=l->up;c.Dr1=l->up+8;c.Dr6&=~DWORD64(3);c.Dr7=(c.Dr7&~ownMask)|watchBits;bool changed=SetThreadContext(thread,&c);l->installed=changed;
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
}
namespace msm2::trace {
void configure(Config c,bool verified,std::uintptr_t base,std::size_t size){
 if(usedWindows){killed=true;admitted=false;writeLog("orientation_trace_rearm_refused reason=process_lifetime_budget_requires_full_game_restart");return;}
 config=c;admitted=verified&&base&&size&&c.enabled&&c.windowMs>=100&&c.windowMs<=500&&c.maxWindows<=windowsCap&&c.maxEvents>=1&&c.maxEvents<=256&&c.maxThreads>=1&&c.maxThreads<=threadsCap;gameBase=base;gameSize=size;QueryPerformanceFrequency(&frequency);killed=!admitted;
 writeLog("orientation_trace_config enabled=%u admitted=%u diagnostic_only=1 window_ms=%u max_windows=%u max_events=%u max_threads=%u actor_writes=0 unknown_game_calls=0 PAGE_GUARD=0 coverage=current_threads_bounded_NOT_complete",unsigned(c.enabled),unsigned(admitted),c.windowMs,c.maxWindows,c.maxEvents,c.maxThreads);
}
void kill() noexcept{killed=true;for(auto& w:windows)w.capture=false;}
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
void stop(){kill();for(unsigned attempt=0;attempt<3&&activeLeases();++attempt)for(unsigned i=0;i<usedWindows;++i)release(windows[i]);current=-1;drainLogs();writeLog("orientation_trace_shutdown cleanup_pending=%u VEH_tombstones_resident_until_process_exit=1",unsigned(activeLeases()));}
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
}
bool configureReset(bool verified,std::uintptr_t base,std::size_t size,bool prearmed,bool multithread){
 if(usedWindows||externalReset)return false;
 externalReset=true;config={true,multithread?900u:prearmed?500u:30u,8,multithread?512u:256u,multithread?128u:prearmed?1u:32u};gameBase=base;gameSize=size;
 admitted=verified&&(!multithread||prearmed)&&base&&size&&QueryPerformanceFrequency(&frequency)&&frequency.QuadPart>0&&!IsDebuggerPresent();
 if(admitted&&!veh)veh=AddVectoredExceptionHandler(1,handler);
 admitted=admitted&&veh;killed=!admitted;
 writeLog("RESET_TRACE_MODE prearmed=%u prearm_elapsed=0.60 override_elapsed=0.70 end_elapsed=%.2f primary_first=1 multithread=%u expand_elapsed=0.85 max_threads=%u",unsigned(prearmed),multithread?1.50:1.10,unsigned(multithread),config.maxThreads);
 writeLog("RESET_TRACE_CONFIG admitted=%u window_ms=%u deadline_native_elapsed_ms=%u max_sessions=8 max_events=%u max_threads=%u once_per_session=1 source_callback_wait=0 additional_game_hooks=0 original_tailjump_preserved=1",unsigned(admitted),config.windowMs,prearmed?(multithread?1500u:1100u):0u,config.maxEvents,config.maxThreads);return admitted;
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
bool pollReset(bool valid){
 if(current<0){if(activeLeases())for(unsigned i=0;i<usedWindows;++i)release(windows[i]);return false;}
 auto& w=windows[unsigned(current)];if(!valid||killed.load()||!w.capture.load()||now()<=0||now()>=w.end){publishedReset=-1;release(w);current=-1;return false;}
 if(w.reset.multithread&&!w.expanded&&GetTickCount64()>=w.reset.initMs&&GetTickCount64()-w.reset.initMs>=850)expandProcessWatch(w);
 return true;
}
void cancelReset() noexcept {
 auto index=publishedReset.load(std::memory_order_acquire);if(index>=0)windows[unsigned(index)].capture=false;
}
bool resetCommitted(const reset_trace::Request& r) noexcept {
 auto index=publishedReset.load(std::memory_order_acquire);if(index<0)return false;
 auto& w=windows[unsigned(index)];
 if(w.state!=4||w.session!=r.session||w.reset.generation!=r.generation||w.transform!=r.transform)return false;
 // Commit timestamps remain visible even while another thread's VEH owns the
 // diagnostic record lock. Contention can drop a log, never an A/B override.
 w.lastOverride=r.commitQpc;
 if(r.resetCopy)w.copyCommit=r.commitQpc;else w.commonCommit=r.commitQpc;
 if(eventLock.test_and_set(std::memory_order_acquire)){++w.drops;if(!w.reset.multithread)w.capture=false;return false;}
 bool armed=w.capture.load()&&!killed.load()&&w.primaryInstalled.load()&&w.reset.thread==GetCurrentThreadId()&&now()<w.end&&r.overrideBeginQpc&&w.primaryArmedAt<1000.*double(r.overrideBeginQpc)/double(frequency.QuadPart);
 w.lastOverride=r.commitQpc;
 if(r.resetCopy)w.copyCommit=r.commitQpc;else w.commonCommit=r.commitQpc;
 auto h=commonHead.load();if(h-commonTail.load()<commonRecords.size()){CommonRecord record{};record.window=unsigned(index);record.status=true;record.armed=armed;record.elapsed=double(GetTickCount64()-w.reset.initMs)/1000.;commonRecords[h%commonRecords.size()]=record;commonHead.store(h+1,std::memory_order_release);}else{++w.drops;w.capture=false;}
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
 if(w.state==4){auto h=commonHead.load();if(h-commonTail.load()<commonRecords.size()){CommonRecord record{};record.window=unsigned(index);record.entry=e;record.elapsed=double(GetTickCount64()-w.reset.initMs)/1000.;commonRecords[h%commonRecords.size()]=record;commonHead.store(h+1,std::memory_order_release);}else{++w.drops;w.capture=false;}}
 eventLock.clear(std::memory_order_release);
}

}
