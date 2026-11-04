// TEST ONLY: real owned-thread hardware exception, held AFTER the unmodified
// production VEH returns but BEFORE Windows/Wine resumes its saved CONTEXT.
// This is a deterministic scheduling fixture, not an engine hook or game test.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include <cassert>
static PVECTORED_EXCEPTION_HANDLER engineHandler{};
static HANDLE handlerReturned{},allowExceptionReturn{};
static std::atomic<bool> holdReturn{false};
static std::atomic<bool> holdContinueReturn{false};
static std::atomic<unsigned> continueCalls{0};
static void holdExceptionReturn() {
 const auto saved=GetLastError();
 SetEvent(handlerReturned);
 assert(WaitForSingleObject(allowExceptionReturn,2000)==WAIT_OBJECT_0);
 SetLastError(saved);
}
static LONG CALLBACK delayedReturn(EXCEPTION_POINTERS* e) {
 const auto result=engineHandler(e);
 if(result==EXCEPTION_CONTINUE_EXECUTION&&holdReturn.exchange(false)) {
  holdExceptionReturn();
 }
 return result;
}
static LONG CALLBACK continueBody(EXCEPTION_POINTERS*) {
 ++continueCalls;return EXCEPTION_CONTINUE_SEARCH;
}
static LONG CALLBACK delayedContinueReturn(EXCEPTION_POINTERS* e) {
 const auto result=continueBody(e);
 if(e&&e->ExceptionRecord&&e->ExceptionRecord->ExceptionCode==EXCEPTION_SINGLE_STEP&&holdContinueReturn.exchange(false))holdExceptionReturn();
 return result;
}
static PVOID WINAPI installDelayedReturn(ULONG first,PVECTORED_EXCEPTION_HANDLER handler) {
 assert(!engineHandler);engineHandler=handler;
 return AddVectoredExceptionHandler(first,delayedReturn);
}
#define AddVectoredExceptionHandler installDelayedReturn
#include "../msm2/orientation_trace.cpp"
#undef AddVectoredExceptionHandler
#include <cstdio>
#include <cstdarg>
#include <string>

static std::string output;static msm2::Snapshot hero;
namespace msm2::native_source {
bool diagnosticIdentity(const msm2::reset_trace::Request&) noexcept{return false;}
}
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&got)&&got==n;}
bool sample(Snapshot& s){s=hero;return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char b[4096];va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);output+=b;output+='\n';}
}
static HANDLE ready{},go{},done{};static std::atomic<bool> finish{false};static unsigned watchedIndex=4;
static DWORD WINAPI ownedWriter(void*) {
 SetEvent(ready);
 while(WaitForSingleObject(go,INFINITE)==WAIT_OBJECT_0) {
  if(finish.load())break;
  // Exactly one scalar store, owned memory only. The second iteration triggers
  // even though the value is unchanged: this is a hardware write breakpoint.
  reinterpret_cast<volatile float*>(hero.transform)[watchedIndex]=1;
  SetEvent(done);
 }
 return 0;
}
static CONTEXT debug(HANDLE thread) {
 assert(SuspendThread(thread)!=DWORD(-1));CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;
 assert(GetThreadContext(thread,&c));assert(ResumeThread(thread)!=DWORD(-1));return c;
}
static bool same(const CONTEXT& a,const CONTEXT& b) {
 return a.Dr0==b.Dr0&&a.Dr1==b.Dr1&&a.Dr2==b.Dr2&&a.Dr3==b.Dr3&&a.Dr6==b.Dr6&&a.Dr7==b.Dr7;
}
static bool recoveredState(const CONTEXT& a,const CONTEXT& b,bool position) {
 // Position promises a saved DR6 snapshot; legacy Up only clears the owned
 // status bits and preserves unrelated status. Reserved DR6 bits are OS state.
 auto normalized=a;normalized.Dr6=b.Dr6;
 return same(normalized,b)&&(position?a.Dr6==b.Dr6:(a.Dr6&0xe00full)==(b.Dr6&0xe00cull));
}
int main(int argc,char** argv) {
 bool position=false,normal=false,timeout=false,killClose=false,continueReturn=false;
 for(int i=1;i<argc;++i) {
  if(!std::strcmp(argv[i],"--position"))position=true;
  else if(!std::strcmp(argv[i],"--normal"))normal=true;
  else if(!std::strcmp(argv[i],"--timeout"))timeout=true;
  else if(!std::strcmp(argv[i],"--kill"))killClose=true;
  else if(!std::strcmp(argv[i],"--continue-return"))continueReturn=true;
  else assert(false&&"unknown retirement test argument");
 }
 assert(unsigned(normal)+unsigned(timeout)+unsigned(killClose)+unsigned(continueReturn)<=1);
 watchedIndex=position?12:4;
 auto* memory=static_cast<float*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);
 memory[0]=memory[5]=memory[10]=memory[15]=1;
 hero.hero=reinterpret_cast<std::uintptr_t>(memory+32);hero.transform=reinterpret_cast<std::uintptr_t>(memory);
 handlerReturned=CreateEventW(nullptr,FALSE,FALSE,nullptr);allowExceptionReturn=CreateEventW(nullptr,FALSE,FALSE,nullptr);
 ready=CreateEventW(nullptr,TRUE,FALSE,nullptr);go=CreateEventW(nullptr,FALSE,FALSE,nullptr);done=CreateEventW(nullptr,FALSE,FALSE,nullptr);
 DWORD tid{};auto thread=CreateThread(nullptr,0,ownedWriter,nullptr,0,&tid);assert(thread&&WaitForSingleObject(ready,2000)==WAIT_OBJECT_0);
 const auto original=debug(thread);
 PVOID continueHandler=nullptr;
 if(continueReturn){continueHandler=AddVectoredContinueHandler(1,delayedContinueReturn);assert(continueHandler);}
 const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
 const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
 assert(msm2::trace::configureReset(true,base,nt->OptionalHeader.SizeOfImage,true,true));
 msm2::reset_trace::Request r{};r.prearmed=r.verifyDual=r.multithread=true;
 r.initMs=GetTickCount64()-(position?1600:600);r.session=r.generation=1;r.hero=hero.hero;r.transform=hero.transform;r.owner=hero.hero;r.thread=tid;r.normal={0,0,1};
 r.position=position;r.basis={{-1,0,0},{0,0,1},{0,1,0},true};
 msm2::Snapshot sample{};assert(msm2::sample(sample));assert(position?msm2::trace::beginPosition(r,sample):msm2::trace::beginReset(r,sample));
 assert(current==0&&windows[0].capture&&windows[0].primaryInstalled&&usedLeases==1);
 holdReturn=!normal&&!continueReturn;holdContinueReturn=continueReturn;SetEvent(go);
 if(normal) {
  // Rejoining the OWNED writer after its store is a real post-exception-return
  // witness in this fixture. The production watch has no equivalent verified
  // acknowledgment covering every watched thread and pending saved context.
  assert(WaitForSingleObject(done,2000)==WAIT_OBJECT_0);
  assert(windows[0].hits==1&&windows[0].capture&&windows[0].positionInflight==0);
  assert(!msm2::trace::pollReset(false,"owned_completed_return_fixture"));
  assert(!activeLeases()&&current==-1);
  assert(output.find("cleanup_pending=0")!=std::string::npos);
  for(unsigned n=0;n<3;++n)assert(same(debug(thread),original));
  msm2::trace::stop();assert(!activeLeases()&&same(debug(thread),original));
  std::printf("PASS: %s normal close after owned writer post-return acknowledgment restores exact saved debug state; no subsequent access required; acknowledgment is fixture-only\n",position?"Position":"reset");
  finish=true;SetEvent(go);assert(WaitForSingleObject(thread,2000)==WAIT_OBJECT_0);
  for(auto h:{thread,ready,go,done,handlerReturned,allowExceptionReturn})CloseHandle(h);
  assert(VirtualFree(memory,0,MEM_RELEASE));return 0;
 }
 if(continueReturn) {
  HANDLE resultEvents[]={handlerReturned,done};
  const auto result=WaitForMultipleObjects(2,resultEvents,FALSE,2000);
  if(result==WAIT_OBJECT_0+1) {
   // Capability probe, not a passing retirement acceptance test. This Wine
   // path may accept registration without dispatching the VCH for a VEH-
   // handled data breakpoint. Do not manufacture a callback/acknowledgment.
   assert(continueCalls.load()==0&&windows[0].hits==1&&windows[0].positionInflight==0);
   assert(holdContinueReturn.exchange(false));
   assert(!msm2::trace::pollReset(false,"owned_VCH_not_dispatched"));
   assert(!activeLeases()&&same(debug(thread),original));
   msm2::trace::stop();assert(same(debug(thread),original));
   std::printf("UNAVAILABLE: %s registered VCH was not invoked for the actual VEH-handled hardware event before owned writer acknowledgment; no VCH retirement test or certificate claimed\n",position?"Position":"reset");
   finish=true;SetEvent(go);assert(WaitForSingleObject(thread,2000)==WAIT_OBJECT_0);
   for(auto h:{thread,ready,go,done,handlerReturned,allowExceptionReturn})CloseHandle(h);
   assert(RemoveVectoredContinueHandler(continueHandler));assert(VirtualFree(memory,0,MEM_RELEASE));return 0;
  }
  assert(result==WAIT_OBJECT_0);
 }else assert(WaitForSingleObject(handlerReturned,2000)==WAIT_OBJECT_0);
 if(continueReturn)assert(continueCalls.load()>0);
 assert(windows[0].hits==1&&windows[0].capture&&windows[0].positionInflight==0);
 // No production deadline is extended. Close through the real coordinator path
 // while the thread is stopped in the OS exception-return interval.
 if(timeout) {
  // Sleep granularity may return slightly before the QPC-based deadline (seen under Wine); wait for the real deadline, bounded to 200ms.
  for(unsigned waits=0;waits<200&&now()<windows[0].end;++waits)Sleep(1);
  assert(now()>=windows[0].end);assert(!msm2::trace::pollReset(true));
  assert(!std::strcmp(windows[0].stopReason.load(),"deadline"));
 }else if(killClose) {
  msm2::trace::kill();assert(!msm2::trace::pollReset(true));
  assert(!std::strcmp(windows[0].stopReason.load(),"kill_or_shutdown"));
 }else assert(!msm2::trace::pollReset(false,"owned_retirement_fixture"));
 assert(!activeLeases()&&current==-1);
 assert(output.find("cleanup_pending=0")!=std::string::npos);
 const auto beforeReturn=debug(thread);assert(same(beforeReturn,original));
 // Repeating instantaneous readback is not an OS exception-return witness.
 for(unsigned n=0;n<3;++n)assert(same(debug(thread),original));
 SetEvent(allowExceptionReturn);assert(WaitForSingleObject(done,2000)==WAIT_OBJECT_0);
 const auto afterReturn=debug(thread);
 // This assertion intentionally records a limitation of the current engine.
 // Do not turn it into a claim that cleanup succeeded or release a c39 watch.
 assert(own(afterReturn,leases[0])==3&&!same(afterReturn,original));
 assert(!activeLeases()); // Bookkeeping cannot see this resurrected context.
 const auto scenario=timeout?"deadline":killClose?"kill":continueReturn?"VCH_return":"VEH_return";
 std::printf("PASS: %s %s reproduces cleanup_pending=0 / inflight=0 / no active leases and repeated restored readbacks while saved exception CONTEXT re-enables retired DR0/DR1; c40 retirement acceptance NOT MET; exact-slot admission remains unavailable\n",position?"Position":"reset",scenario);
 // Existing immutable lease tombstones safely retire a LATER matching hit.
 // That is recovery, not proof of bounded restoration in the silent interval.
 const auto captured=windows[0].hits.load();SetEvent(go);assert(WaitForSingleObject(done,2000)==WAIT_OBJECT_0);
 const auto recovered=debug(thread);
 if(windows[0].hits!=captured||!recoveredState(recovered,original,position))std::fprintf(stderr,"RETIREMENT_RECOVERY hits=%u expected=%u DR0=%llx/%llx DR1=%llx/%llx DR2=%llx/%llx DR3=%llx/%llx DR6=%llx/%llx DR7=%llx/%llx\n",windows[0].hits.load(),captured,recovered.Dr0,original.Dr0,recovered.Dr1,original.Dr1,recovered.Dr2,original.Dr2,recovered.Dr3,original.Dr3,recovered.Dr6,original.Dr6,recovered.Dr7,original.Dr7);
 assert(windows[0].hits==captured&&recoveredState(recovered,original,position));
 msm2::trace::stop();assert(!activeLeases()&&recoveredState(debug(thread),original,position));
 std::printf("PASS: %s %s later owned hit uses existing closed-window tombstone without new capture and restores saved slots with existing DR6 policy; kill/stop unchanged; no game slot/value/code accessed\n",position?"Position":"reset",scenario);
 finish=true;SetEvent(go);assert(WaitForSingleObject(thread,2000)==WAIT_OBJECT_0);
 for(auto h:{thread,ready,go,done,handlerReturned,allowExceptionReturn})CloseHandle(h);
 if(continueHandler)assert(RemoveVectoredContinueHandler(continueHandler));
 assert(VirtualFree(memory,0,MEM_RELEASE));
}
