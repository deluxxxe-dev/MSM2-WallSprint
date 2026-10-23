// Independent owned memory/functions and actual thread DR/VEH operations.
// No game code, unknown ABI, or game ownership claim.
#include "../msm2/orientation_trace.cpp"
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <string>
#include <immintrin.h>
static std::string output;
static Snapshot hero;
static reset_trace::Request request;
struct Worker {HANDLE go{},done{},ready{},thread{};DWORD tid{};unsigned role{};CONTEXT previous{};};
static std::atomic<bool> finish{false};
static std::atomic<unsigned> mode{0};
namespace msm2::native_source {bool diagnosticIdentity(const msm2::reset_trace::Request&) noexcept {return false;}}
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&got)&&got==n;}
bool sample(Snapshot& s){s=hero;return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char b[4096]{};va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);output+=b;output+='\n';}
}
__attribute__((noinline,noclone)) void surfaceWriter(float* t){_mm_storeu_ps(t+4,_mm_set_ps(0,1,0,0));__asm__ volatile("" ::: "memory");}
__attribute__((noinline,noclone)) void unknownWriter(float* t){_mm_storeu_ps(t+4,_mm_set_ps(0,0,1,0));__asm__ volatile("" ::: "memory");}
__attribute__((noinline,noclone)) void sequentialUnknown(float* t){auto* p=static_cast<volatile float*>(t+4);p[0]=0;p[1]=1;p[2]=0;__asm__ volatile("" ::: "memory");}
static DWORD WINAPI loop(void* raw){auto& worker=*static_cast<Worker*>(raw);SetEvent(worker.ready);
 while(WaitForSingleObject(worker.go,INFINITE)==WAIT_OBJECT_0){if(finish.load())break;
  if(worker.role==0){for(unsigned i=0;i<2;++i){auto r=request;LARGE_INTEGER q{};QueryPerformanceCounter(&q);r.resetCopy=i;r.commitQpc=r.overrideBeginQpc=q.QuadPart;msm2::trace::resetCommitted(r);}surfaceWriter(reinterpret_cast<float*>(hero.transform));if(mode.load()==2)for(unsigned i=0;i<140;++i){unknownWriter(reinterpret_cast<float*>(hero.transform));surfaceWriter(reinterpret_cast<float*>(hero.transform));}}
  else if(worker.role==1){if(mode.load())sequentialUnknown(reinterpret_cast<float*>(hero.transform));else unknownWriter(reinterpret_cast<float*>(hero.transform));}
  SetEvent(worker.done);
 }return 0;
}
static CONTEXT debug(HANDLE thread){assert(SuspendThread(thread)!=DWORD(-1));CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;assert(GetThreadContext(thread,&c));assert(ResumeThread(thread)!=DWORD(-1));return c;}
static void setDebug(HANDLE thread,CONTEXT c){assert(SuspendThread(thread)!=DWORD(-1));assert(SetThreadContext(thread,&c));assert(ResumeThread(thread)!=DWORD(-1));}
static bool equalDebug(const CONTEXT& a,const CONTEXT& b){bool same=a.Dr0==b.Dr0&&a.Dr1==b.Dr1&&a.Dr2==b.Dr2&&a.Dr3==b.Dr3&&a.Dr6==b.Dr6&&a.Dr7==b.Dr7;if(!same)std::printf("DEBUG_STATE after=(%llx,%llx,%llx,%llx,%llx,%llx) before=(%llx,%llx,%llx,%llx,%llx,%llx)\n",a.Dr0,a.Dr1,a.Dr2,a.Dr3,a.Dr6,a.Dr7,b.Dr0,b.Dr1,b.Dr2,b.Dr3,b.Dr6,b.Dr7);return same;}
static void run(Worker& w){SetEvent(w.go);assert(WaitForSingleObject(w.done,1000)==WAIT_OBJECT_0);}
static unsigned occurrences(const std::string& s,const char* needle){unsigned n=0;std::size_t p=0;while((p=s.find(needle,p))!=std::string::npos){++n;p+=std::strlen(needle);}return n;}
int main(){
 auto* memory=static_cast<float*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);memory[0]=memory[5]=memory[10]=memory[15]=1;memory[12]=17;memory[13]=23;memory[14]=42;
 hero.hero=reinterpret_cast<std::uintptr_t>(memory+32);hero.transform=reinterpret_cast<std::uintptr_t>(memory);
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
 std::array<Worker,3> workers{};
 for(unsigned i=0;i<workers.size();++i){auto& w=workers[i];w.role=i;w.go=CreateEventW(nullptr,FALSE,FALSE,nullptr);w.done=CreateEventW(nullptr,FALSE,FALSE,nullptr);w.ready=CreateEventW(nullptr,TRUE,FALSE,nullptr);w.thread=CreateThread(nullptr,0,loop,&w,0,&w.tid);assert(w.thread&&WaitForSingleObject(w.ready,1000)==WAIT_OBJECT_0);w.previous=debug(w.thread);}
 // A foreign DR2 remains in use while our free DR0/DR1 are installed.
 auto dr2=workers[0].previous;dr2.Dr2=base;dr2.Dr7|=0x10;setDebug(workers[0].thread,dr2);workers[0].previous=debug(workers[0].thread);
 auto foreign=workers[2].previous;foreign.Dr0=base;foreign.Dr7=(foreign.Dr7&~ownMask)|0x90001;setDebug(workers[2].thread,foreign);foreign=debug(workers[2].thread);
 assert(msm2::trace::configureReset(true,base,nt->OptionalHeader.SizeOfImage,true,true));
 assert(!std::strcmp(writerClass(base,0x2cfc580),"KNOWN_PATH_A")&&!std::strcmp(writerClass(base,0x2cfc3c0),"KNOWN_PATH_B")&&!std::strcmp(writerClass(base+1,0x2cfc580),"UNKNOWN_WRITER"));
 auto arm=[&](unsigned session,unsigned elapsed,bool expand=true){
  request={};request.prearmed=request.verifyDual=request.characterize=request.multithread=true;request.session=session;request.generation=1;request.hero=hero.hero;request.transform=hero.transform;request.owner=hero.hero;request.thread=workers[0].tid;request.normal={0,0,1};request.initMs=GetTickCount64()-elapsed;
  Snapshot s{};assert(msm2::sample(s));assert(msm2::trace::beginReset(request,s));assert(msm2::trace::pollReset(true));auto& w=windows[unsigned(current)];assert(w.expanded==expand);if(expand)assert(w.expandAttempted&&w.entriesTotal>1&&w.ownerMatches>1&&w.installedThreads>=2);
 };
 arm(1,900);assert(windows[0].foreignThreads>=1);DWORD error{};assert(!std::strcmp(installProcessThread(windows[0],0xfffffffe,&error),"open_thread_failed")&&error!=ERROR_SUCCESS);
 run(workers[0]);run(workers[1]);msm2::trace::drainLogs();assert(windows[0].thirdReported);
 assert(output.find("THIRD_WRITER_EVENT session=1 classification=UNKNOWN_WRITER thread_id="+std::to_string(workers[1].tid))!=std::string::npos);
 assert(output.find("THIRD_WRITER_IDENTITY session=1")!=std::string::npos&&output.find("function_boundary_verified=1")!=std::string::npos&&output.find("caller_valid=1")!=std::string::npos);
 run(workers[0]);run(workers[1]);msm2::trace::drainLogs();assert(occurrences(output,"THIRD_WRITER_EVENT session=1 ")==1);
 // Concurrent diagnostic work can drop a record, but cannot disable the watch
 // or suppress the shared A/B commit timestamps.
 assert(!eventLock.test_and_set());run(workers[0]);run(workers[1]);assert(windows[0].capture&&windows[0].commonCommit&&windows[0].copyCommit);eventLock.clear();assert(windows[0].drops>0);
 Unwind invalid{};invalid.c.Rip=reinterpret_cast<DWORD64>(&unknownWriter);invalid.c.Rsp=1;assert(ws_guarded_call(unwind,&invalid)&&invalid.count==0);
 msm2::trace::pollReset(false);assert(!activeLeases());assert(equalDebug(debug(workers[2].thread),foreign));
 for(unsigned i=0;i<2;++i)assert(equalDebug(debug(workers[i].thread),workers[i].previous));
 // A captured reset after the old 1.10 deadline, including sequential dwords.
 setDebug(workers[2].thread,workers[2].previous);mode=1;arm(2,1200);run(workers[0]);run(workers[1]);msm2::trace::drainLogs();assert(windows[1].thirdReported);
 assert(output.find("surface_anchor_across_partial_stores_NOT_atomic_pre_store")!=std::string::npos);
 msm2::trace::pollReset(false);assert(!activeLeases());
 // Timeout, synchronous lifecycle/identity cancellation and F8/shutdown cleanup.
 mode=0;arm(3,900);Sleep(620);assert(!msm2::trace::pollReset(true)&&!activeLeases());
 arm(4,900);msm2::trace::cancelReset();assert(!msm2::trace::pollReset(true)&&!activeLeases());
 arm(5,900);assert(!msm2::trace::pollReset(false)&&!activeLeases());
 auto primaryForeign=workers[0].previous;primaryForeign.Dr0=base;primaryForeign.Dr7|=1;setDebug(workers[0].thread,primaryForeign);primaryForeign=debug(workers[0].thread);
 arm(6,900);assert(!windows[5].primaryInstalled&&windows[5].foreignThreads>=1);msm2::trace::pollReset(false);assert(!activeLeases()&&equalDebug(debug(workers[0].thread),primaryForeign));setDebug(workers[0].thread,workers[0].previous);
 // Production threshold leap and realistic record pressure BEFORE enumeration.
 // c3 closes capture on the first common-ring overflow and skips expansion.
 msm2::trace::drainLogs();arm(7,600,false);auto& pressure=windows[6];
 for(unsigned i=0;i<1100;++i)msm2::trace::resetEntry(hero.transform,hero.transform,0,0,base,false,1,1,hero.hero);
 assert(pressure.commonDrops>0&&pressure.capture&&!pressure.expandAttempted);
 mode=2;run(workers[0]);assert(pressure.budgetDrops>0&&pressure.capture&&pressure.hits==config.maxEvents/4);mode=0;
 auto elapsed=GetTickCount64()-request.initMs;if(elapsed<930)Sleep(DWORD(930-elapsed));
 assert(msm2::trace::pollReset(true)&&pressure.expandRequested&&pressure.expandAttempted&&pressure.expanded&&pressure.entriesTotal>1&&pressure.ownerMatches>1&&pressure.installedThreads>1);
 assert(msm2::trace::pollReset(true));assert(occurrences(output,"MULTITHREAD_EXPAND_ATTEMPT session=7 ")==1);
 run(workers[0]);run(workers[1]);msm2::trace::drainLogs();assert(pressure.thirdReported);
 msm2::trace::pollReset(false);assert(!activeLeases());
 arm(8,900);msm2::trace::kill();assert(!msm2::trace::pollReset(true)&&!activeLeases());msm2::trace::stop();
 for(auto& w:workers)assert(equalDebug(debug(w.thread),w.previous));
 assert(memory[12]==17&&memory[13]==23&&memory[14]==42);MEMORY_BASIC_INFORMATION page{};assert(VirtualQuery(memory,&page,sizeof(page))&&page.Protect==PAGE_READWRITE);
 assert(output.find("reason=foreign_dr")!=std::string::npos&&output.find("cleanup_pending=0")!=std::string::npos&&output.find("complete_thread_coverage=0")!=std::string::npos);
 auto file=std::fopen("logs/windows_third_writer_synthetic.log","wb");assert(file);std::fwrite(output.data(),1,output.size(),file);std::fclose(file);
 finish=true;for(auto& w:workers){SetEvent(w.go);assert(WaitForSingleObject(w.thread,1000)==WAIT_OBJECT_0);CloseHandle(w.thread);CloseHandle(w.go);CloseHandle(w.done);CloseHandle(w.ready);}assert(VirtualFree(memory,0,MEM_RELEASE));
 std::puts("PASS: process-thread Up watch captures a second-thread unknown writer after dual commits, bounded identity/hash/caller evidence and once/session trigger; sequential reset after1.10s; foreign DR and all prior debug state preserved; timeout/Deinit/identity/F8/shutdown cleanup; Position/page protection unchanged; owned fixtures only");
 std::puts("PASS: production Toolhelp enumeration and >=0.85 transition after sparse samples; entries_total>1 owner_pid_matches>1 installed>1; pre-expansion common-ring overflow and event budget pressure preserve coordinator and reserve post-expansion evidence; one attempt only; explicit OpenThread OS error");
}
