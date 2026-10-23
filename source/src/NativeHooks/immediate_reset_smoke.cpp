// TEST ONLY: real debug-register writes and a real MinHook on an independently
// written native function. Owned memory only; no game code or game gate bypass.
#include "../msm2/orientation_trace.cpp"
#include "immediate_reset_trace.cpp"
#include <MinHook.h>
#include <immintrin.h>
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <string>
#include <mutex>
static std::string output;static std::mutex outputLock;
static bool prearmTest{};static msm2::reset_trace::Request testRequest;static ws::native_source::Observer lifecycleObserver{};
static ws::native_source::Token fakeToken;static std::atomic<bool> fakeGuard{true};static HANDLE armedSignal{};
namespace msm2::native_source {void prearm(const ws::native_source::Token&) noexcept {}
bool observationGuardsAuthenticated() noexcept{return fakeGuard.load();}}
namespace ws::native_source {void setObserver(Observer fn) noexcept {lifecycleObserver=fn;}
bool lifecycle(Token& t) noexcept {t=fakeToken;if(prearmTest)t.updateMs=GetTickCount64();return true;}}
static msm2::Snapshot hero;static HANDLE go,done;static std::atomic<bool> finish{false};
static std::atomic<unsigned> mode{0};static float world[16],tilted[16];
extern "C" {void ws_source_thunk();void* ws_source_original{};std::atomic<unsigned> ws_source_recording{1},ws_native_source_enabled{0};}
struct Frame {std::uintptr_t r11,r10,r9,r8,rdx,rcx,rax,flags,ret;};
extern "C" void ws_source_capture(const Frame* f) noexcept {msm2::trace::resetEntry(f->rcx,f->rdx,f->r8,f->r9,f->ret,prearmTest&&mode.load()==2,77,fakeToken.generation,fakeToken.owner);}
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T done=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&done)&&done==n;}
bool sample(Snapshot& s){s=hero;return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* f,...){char b[4096];va_list a;va_start(a,f);vsnprintf(b,sizeof(b),f,a);va_end(a);std::lock_guard<std::mutex> lock(outputLock);output+=b;output+='\n';if(armedSignal&&std::strstr(b,"RESET_TRACE_WINDOW")&&std::strstr(b," armed=1"))SetEvent(armedSignal);}
}
__attribute__((noinline,noclone)) void commonWriter(float* target,const float* source,std::uintptr_t,std::uintptr_t){
 auto u=_mm_loadu_ps(source+4);_mm_storeu_ps(target+4,u);__asm__ volatile("" ::: "memory");
}
__attribute__((noinline,noclone)) void otherWriter(float* target){
 _mm_storeu_ps(target+4,_mm_set_ps(0,0,1,0));__asm__ volatile("" ::: "memory");
}
__attribute__((noinline,noclone)) void sequentialWriter(float* target){
 auto* up=static_cast<volatile float*>(target+4);up[0]=0;__asm__ volatile("" ::: "memory");up[1]=1;__asm__ volatile("" ::: "memory");up[2]=0;__asm__ volatile("" ::: "memory");
}
static std::atomic<unsigned> entryAssociationTest{0};
DWORD WINAPI writerThread(void*){
 while(WaitForSingleObject(go,INFINITE)==WAIT_OBJECT_0){if(finish.load())break;
  if(auto stage=entryAssociationTest.load()){
   msm2::trace::beginResetCopyEntry();
   if(stage==1){msm2::path_b::Entry e{};e.id=1234;e.session=6;e.rcx=hero.transform;e.thread=GetCurrentThreadId();msm2::trace::resetCopyEntry(e);}
  }
  if(prearmTest&&mode.load()==2){LARGE_INTEGER q{};QueryPerformanceCounter(&q);auto r=testRequest;r.commitQpc=q.QuadPart;r.overrideBeginQpc=q.QuadPart;msm2::reset_trace::committed(r);commonWriter(reinterpret_cast<float*>(hero.transform),tilted,0x1234,0x5678);}
  else if(prearmTest&&mode.load()==0){LARGE_INTEGER q{};QueryPerformanceCounter(&q);auto r=testRequest;r.resetCopy=true;r.commitQpc=r.overrideBeginQpc=q.QuadPart;msm2::reset_trace::committed(r);commonWriter(reinterpret_cast<float*>(hero.transform),world,0x1234,0x5678);}
  else if(prearmTest&&mode.load()==1){LARGE_INTEGER q{};QueryPerformanceCounter(&q);auto r=testRequest;r.resetCopy=true;r.commitQpc=r.overrideBeginQpc=q.QuadPart;msm2::reset_trace::committed(r);sequentialWriter(reinterpret_cast<float*>(hero.transform));}
  else if(mode.load()==1)otherWriter(reinterpret_cast<float*>(hero.transform));
  else commonWriter(reinterpret_cast<float*>(hero.transform),world,0x1234,0x5678);
  SetEvent(done);
 }return 0;
}
static CONTEXT debug(HANDLE thread){assert(SuspendThread(thread)!=DWORD(-1));CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;assert(GetThreadContext(thread,&c));assert(ResumeThread(thread)!=DWORD(-1));return c;}
int main(int argc,char** argv){
 bool sequence=argc>1&&std::strcmp(argv[1],"--sequence")==0;
 auto* memory=static_cast<float*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);
 world[0]=world[5]=world[10]=world[15]=1;tilted[0]=tilted[6]=tilted[15]=1;tilted[9]=-1;
 hero.hero=reinterpret_cast<std::uintptr_t>(memory+64);hero.transform=reinterpret_cast<std::uintptr_t>(memory);
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
 assert(MH_Initialize()==MH_OK);assert(MH_CreateHook(reinterpret_cast<void*>(&commonWriter),reinterpret_cast<void*>(&ws_source_thunk),&ws_source_original)==MH_OK);assert(MH_EnableHook(reinterpret_cast<void*>(&commonWriter))==MH_OK);
 go=CreateEventW(nullptr,FALSE,FALSE,nullptr);done=CreateEventW(nullptr,FALSE,FALSE,nullptr);DWORD tid=0;auto thread=CreateThread(nullptr,0,writerThread,nullptr,0,&tid);assert(thread);
 auto original=debug(thread);
 if(argc>1&&(std::strcmp(argv[1],"--prearm")==0||sequence)){
  if(sequence){float e[16]={.70710678f,0,.70710678f,0,-.70710678f,0,.70710678f,0,0,-1,0,0,0,0,0,1};std::memcpy(tilted,e,64);}
  prearmTest=true;std::memcpy(memory,world,64);fakeToken.phase=3;fakeToken.generation=1;fakeToken.owner=hero.hero;fakeToken.updates=77;fakeToken.thread=tid;
  armedSignal=CreateEventW(nullptr,FALSE,FALSE,nullptr);
  msm2::reset_trace::configure(true,base,nt->OptionalHeader.SizeOfImage);
  assert(output.find("RESET_TRACE_SELF_CHECK exact_build=1 common_writer_hook=1 reset_trace_enabled=1 reset_trace_admitted=1 primary_thread_prearm_supported=1")!=std::string::npos);
  testRequest.prearmed=true;testRequest.verifyDual=true;testRequest.initMs=GetTickCount64()-600;testRequest.session=1;testRequest.generation=1;testRequest.nativeUpdate=77;testRequest.hero=hero.hero;testRequest.transform=hero.transform;testRequest.owner=hero.hero;testRequest.thread=tid;testRequest.normal=sequence?ws::Vec3{-.70710678f,0,.70710678f}:ws::Vec3{0,0,1};
  msm2::reset_trace::request(testRequest);assert(WaitForSingleObject(armedSignal,1000)==WAIT_OBJECT_0);
  assert(windows[0].state==4&&windows[0].primaryInstalled&&windows[0].end-windows[0].start>350);
  assert(usedLeases==1&&leases[0].tid==tid);
  // Every target-matching caller, including an unreadable RDX, is retained.
  msm2::trace::resetEntry(hero.transform+64,reinterpret_cast<std::uintptr_t>(world),0,0,base+10,false,77,1,hero.hero);
  msm2::trace::resetEntry(hero.transform,1,0,0,base+11,false,77,1,hero.hero);
  Sleep(110);mode=2;SetEvent(go);assert(WaitForSingleObject(done,1000)==WAIT_OBJECT_0);
  SetEvent(go);assert(WaitForSingleObject(done,1000)==WAIT_OBJECT_0);assert(windows[0].hits==1);
  mode=sequence?1:0;SetEvent(go);assert(WaitForSingleObject(done,1000)==WAIT_OBJECT_0);Sleep(10);
  msm2::trace::drainLogs();{auto f=std::fopen(sequence?"logs/windows_dual_reset_sequence_synthetic.log":"logs/windows_prearmed_reset_synthetic.log","wb");assert(f);std::fwrite(output.data(),1,output.size(),f);std::fclose(f);}
  assert(output.find("trace_already_armed_before_override=1")!=std::string::npos);
  assert(output.find("COMMON_WRITER_TRACE session=1")!=std::string::npos&&output.find("source_valid=0")!=std::string::npos);
  assert(output.find("source_overridden_this_entry=1")!=std::string::npos);
  assert(output.find("RESET_WRITER_CANDIDATE=1 session=1 common_writer_reset=0 RESET_PATH_CORRELATED=0")!=std::string::npos);
  assert(output.find("NEW_RESET_AFTER_DUAL_OVERRIDE")!=std::string::npos);
  if(sequence)assert(output.find("RESET_SEQUENCE_CANDIDATE")!=std::string::npos);
  assert(!activeLeases());auto restored=debug(thread);assert(restored.Dr0==original.Dr0&&restored.Dr1==original.Dr1&&(restored.Dr7&ownMask)==(original.Dr7&ownMask));
  // Absolute deadline and occupied primary slot are failures, not fake prearm.
  testRequest.verifyDual=false;testRequest.session=2;testRequest.initMs=GetTickCount64()-1000;std::memcpy(memory,world,64);
  msm2::reset_trace::request(testRequest);assert(WaitForSingleObject(armedSignal,1000)==WAIT_OBJECT_0);
  Sleep(120);assert(!windows[1].capture);assert(!activeLeases());
  auto occupied=original;occupied.Dr0=base;occupied.Dr7=(occupied.Dr7&~ownMask)|0x90001;
  assert(SuspendThread(thread)!=DWORD(-1));assert(SetThreadContext(thread,&occupied));assert(ResumeThread(thread)!=DWORD(-1));
  testRequest.session=3;testRequest.initMs=GetTickCount64()-600;
  msm2::reset_trace::request(testRequest);assert(WaitForSingleObject(armedSignal,50)==WAIT_TIMEOUT);assert(!windows[2].capture&&!activeLeases());
  auto preserved=debug(thread);assert(preserved.Dr0==base&&(preserved.Dr7&ownMask)==(occupied.Dr7&ownMask));
  assert(SuspendThread(thread)!=DWORD(-1));assert(SetThreadContext(thread,&original));assert(ResumeThread(thread)!=DWORD(-1));
  testRequest.session=4;testRequest.initMs=GetTickCount64()-1101;
  msm2::reset_trace::request(testRequest);assert(WaitForSingleObject(armedSignal,50)==WAIT_TIMEOUT);assert(usedWindows==3&&!activeLeases());
  // Next session: synchronous Deinit invalidates capture before any later store.
  testRequest.session=5;testRequest.initMs=GetTickCount64()-600;std::memcpy(memory,world,64);
  msm2::reset_trace::request(testRequest);assert(WaitForSingleObject(armedSignal,1000)==WAIT_OBJECT_0);
  auto deinit=fakeToken;deinit.phase=0;assert(lifecycleObserver);lifecycleObserver(deinit);assert(!windows[3].capture);
  fakeGuard=false;Sleep(10);msm2::reset_trace::stop();msm2::trace::stop();assert(!activeLeases());
  finish=true;SetEvent(go);assert(WaitForSingleObject(thread,1000)==WAIT_OBJECT_0);CloseHandle(thread);CloseHandle(go);CloseHandle(done);CloseHandle(armedSignal);MH_Uninitialize();VirtualFree(memory,0,MEM_RELEASE);
  auto f=std::fopen(sequence?"logs/windows_dual_reset_sequence_synthetic.log":"logs/windows_prearmed_reset_synthetic.log","wb");assert(f);std::fwrite(output.data(),1,output.size(),f);std::fclose(f);
  if(sequence)puts("PASS: real hardware diagonal E sequential dword reset after dual commits; previous surface anchor captures final WorldUp despite non-surface intermediate row; synthetic only");
  puts("PASS: b7 .60 prearm primary-only before .70 source commit, all-caller/invalid-source trace, surface/reset hardware events, positive PREARM_STATUS, unknown-writer non-correlation, Deinit cancellation and DR cleanup; synthetic NOT game evidence");return 0;
 }
 if(argc>1){
  std::memcpy(memory,tilted,64);fakeToken.phase=3;fakeToken.generation=1;fakeToken.owner=hero.hero;fakeToken.updates=77;fakeToken.updateMs=GetTickCount64();fakeToken.thread=tid;
  armedSignal=CreateEventW(nullptr,FALSE,FALSE,nullptr);msm2::reset_trace::configure(true,base,nt->OptionalHeader.SizeOfImage,false);config.maxThreads=1;
  LARGE_INTEGER q{};QueryPerformanceCounter(&q);msm2::reset_trace::Request r{};r.session=1;r.generation=1;r.nativeUpdate=76;r.commitQpc=q.QuadPart;r.hero=hero.hero;r.transform=hero.transform;r.owner=hero.hero;r.thread=tid;r.normal={0,0,1};r.appliedUp={0,0,1};
  msm2::reset_trace::request(r);assert(WaitForSingleObject(armedSignal,1000)==WAIT_OBJECT_0);SetEvent(go);assert(WaitForSingleObject(done,1000)==WAIT_OBJECT_0);
  Sleep(20);msm2::reset_trace::stop();msm2::trace::stop();assert(!activeLeases());
  {auto f=std::fopen("logs/windows_reset_helper_synthetic.log","wb");assert(f);std::fwrite(output.data(),1,output.size(),f);std::fclose(f);}
  assert(output.find("RESET_WRITER_CANDIDATE")!=std::string::npos);assert(output.find("original_tailjump_preserved=1")!=std::string::npos);
  auto restored=debug(thread);assert(restored.Dr0==original.Dr0&&restored.Dr1==original.Dr1&&(restored.Dr7&ownMask)==(original.Dr7&ownMask));
  finish=true;SetEvent(go);assert(WaitForSingleObject(thread,1000)==WAIT_OBJECT_0);CloseHandle(thread);CloseHandle(go);CloseHandle(done);CloseHandle(armedSignal);MH_Uninitialize();VirtualFree(memory,0,MEM_RELEASE);
  puts("PASS: production async reset coordinator wake/identity/native gate + actual hardware event + stop/join/DR cleanup on owned synthetic memory; no game verification");return 0;
 }
 assert(msm2::trace::configureReset(true,base,nt->OptionalHeader.SizeOfImage));config.maxThreads=1;
 auto arm=[&](unsigned session,bool characterize=false){std::memcpy(memory,tilted,64);Snapshot snapshot{};assert(msm2::sample(snapshot));LARGE_INTEGER q{};QueryPerformanceCounter(&q);msm2::reset_trace::Request r{};r.session=session;r.characterize=characterize;r.nativeUpdate=76;r.commitQpc=q.QuadPart;r.hero=hero.hero;r.transform=hero.transform;r.thread=tid;r.normal={0,0,1};r.appliedUp={0,0,1};assert(msm2::trace::beginReset(r,snapshot));};
 arm(1);SetEvent(go);assert(WaitForSingleObject(done,1000)==WAIT_OBJECT_0);msm2::trace::pollReset(true);msm2::trace::drainLogs();assert(!activeLeases());assert(windows[0].hits==1);
 assert(output.find("RESET_WRITER_CANDIDATE session=1")!=std::string::npos);assert(output.find("R8=0x1234 R9=0x5678")!=std::string::npos);assert(output.find("source_up=(0,1,0)")!=std::string::npos);assert(output.find("native_update=0")!=std::string::npos); // unknown synthetic writer cannot inherit the trigger update
 assert(output.find("entry_correlated=0")!=std::string::npos);assert(output.find("RESET_WRITER_VERIFIED=1")==std::string::npos);
 auto restored=debug(thread);assert(restored.Dr0==original.Dr0&&restored.Dr1==original.Dr1&&(restored.Dr7&ownMask)==(original.Dr7&ownMask));
 // b9: a reset is an observation, not an end to the bounded window.
 arm(6,true);entryAssociationTest=1;auto eventIndex=head.load();SetEvent(go);assert(WaitForSingleObject(done,1000)==WAIT_OBJECT_0);
 assert(windows[1].capture&&windows[1].hits==1);assert(events[eventIndex%ringCap].copyEntry.id==1234);
 entryAssociationTest=2;eventIndex=head.load();
 std::memcpy(memory,tilted,64);SetEvent(go);assert(WaitForSingleObject(done,1000)==WAIT_OBJECT_0);
 assert(windows[1].capture&&windows[1].hits==2);assert(!events[eventIndex%ringCap].copyEntry.id);entryAssociationTest=0;
 msm2::trace::pollReset(false);assert(!activeLeases());
 puts("PASS: b9 characterization keeps hardware watch after first reset and captures second reset; cleanup restores debug registers");
 mode=1;arm(2);SetEvent(go);assert(WaitForSingleObject(done,1000)==WAIT_OBJECT_0);msm2::trace::pollReset(true);msm2::trace::drainLogs();assert(!activeLeases());assert(output.find("RESET_WRITER_CANDIDATE session=2")!=std::string::npos);
 // No reset: expires from source commit, not from setup completion.
 arm(3);Sleep(35);assert(!msm2::trace::pollReset(true));assert(!activeLeases());
 // Lifecycle/identity loss cancellation, then F8-equivalent; original still runs.
 arm(4);assert(!msm2::trace::pollReset(false));assert(!activeLeases());arm(5);msm2::trace::kill();assert(!msm2::trace::pollReset(true));assert(!activeLeases());
 std::memcpy(memory,tilted,64);SetEvent(go);assert(WaitForSingleObject(done,1000)==WAIT_OBJECT_0);assert(memory[5]==1);
 Snapshot snapshot{};msm2::sample(snapshot);msm2::reset_trace::Request stale{};stale.hero=hero.hero;stale.transform=hero.transform;assert(!msm2::trace::beginReset(stale,snapshot));
 msm2::trace::stop();finish=true;SetEvent(go);assert(WaitForSingleObject(thread,1000)==WAIT_OBJECT_0);CloseHandle(thread);CloseHandle(go);CloseHandle(done);
 assert(MH_DisableHook(reinterpret_cast<void*>(&commonWriter))==MH_OK);assert(MH_Uninitialize()==MH_OK);
 auto f=std::fopen("logs/windows_reset_trace_synthetic.log","wb");assert(f);std::fwrite(output.data(),1,output.size(),f);std::fclose(f);VirtualFree(memory,0,MEM_RELEASE);
 puts("PASS: b6 real hardware short watch + real MinHook raw entry; surface-to-world reset, independent second writer, raw RCX/RDX/R8/R9 and exactly64 source bytes, QPC, stop-on-reset, 30ms timeout, cancellation/F8/debug-register restoration; synthetic NEVER game verified");
}
