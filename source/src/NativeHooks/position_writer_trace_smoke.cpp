// TEST ONLY. Owned functions/threads/Transform, no captured game bytes.
#include "../msm2/orientation_trace.cpp"
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <string>
#include <immintrin.h>
static std::string output;static msm2::Snapshot hero;
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&got)&&got==n;}
bool sample(Snapshot& s){s=hero;return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char b[4096];va_list args;va_start(args,fmt);vsnprintf(b,sizeof(b),fmt,args);va_end(args);output+=b;output+='\n';}
}
__attribute__((noinline,noclone)) void ownedPosition(float* dst,const float* src){auto v=_mm_loadu_ps(src+12);_mm_storeu_ps(dst+12,v);__asm__ volatile("" ::: "memory");}
__attribute__((noinline,noclone)) void otherPosition(volatile float* dst){dst[12]+=2;dst[15]=7;__asm__ volatile("" ::: "memory");}

// Leaf owned scalar writer: no game code or ABI. The real hardware exception
// captures each MOVSS source in the production engine's normal ring.
extern "C" void ws_owned_lineage(float*,const float*);
extern "C" unsigned char ws_owned_lineage_end[];
__asm__(".text\n.global ws_owned_lineage\nws_owned_lineage:\n"
 "movss 48(%rdx),%xmm0\nmovss %xmm0,48(%rcx)\n"
 "movss 52(%rdx),%xmm0\nmovss %xmm0,52(%rcx)\n"
 "movss 56(%rdx),%xmm0\nmovss %xmm0,56(%rcx)\n"
 "movss 60(%rdx),%xmm0\nmovss %xmm0,60(%rcx)\nret\n"
 ".global ws_owned_lineage_end\nws_owned_lineage_end:\n");
struct Thread {HANDLE handle{},ready{},go{},done{};DWORD tid{};unsigned role{};CONTEXT previous{};};
static std::atomic<bool> finish{false};alignas(16) static float incoming[16];
static DWORD WINAPI loop(void* raw){auto& t=*static_cast<Thread*>(raw);SetEvent(t.ready);while(WaitForSingleObject(t.go,INFINITE)==WAIT_OBJECT_0){if(finish.load())break;SetLastError(0x31337);if(t.role==3)ws_owned_lineage(reinterpret_cast<float*>(hero.transform),incoming);else if(t.role==0)ownedPosition(reinterpret_cast<float*>(hero.transform),incoming);else otherPosition(reinterpret_cast<float*>(hero.transform));assert(GetLastError()==0x31337);SetEvent(t.done);}return 0;}
static CONTEXT debug(Thread& t){assert(SuspendThread(t.handle)!=DWORD(-1));CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;assert(GetThreadContext(t.handle,&c));assert(ResumeThread(t.handle)!=DWORD(-1));return c;}
static void setDebug(Thread& t,CONTEXT c){assert(SuspendThread(t.handle)!=DWORD(-1));assert(SetThreadContext(t.handle,&c));assert(ResumeThread(t.handle)!=DWORD(-1));}
static bool same(CONTEXT a,CONTEXT b){if(a.Dr0!=b.Dr0||a.Dr1!=b.Dr1||a.Dr2!=b.Dr2||a.Dr3!=b.Dr3||a.Dr6!=b.Dr6||a.Dr7!=b.Dr7)std::printf("DR mismatch got=%llx,%llx,%llx,%llx,%llx,%llx expected=%llx,%llx,%llx,%llx,%llx,%llx\n",a.Dr0,a.Dr1,a.Dr2,a.Dr3,a.Dr6,a.Dr7,b.Dr0,b.Dr1,b.Dr2,b.Dr3,b.Dr6,b.Dr7);return a.Dr0==b.Dr0&&a.Dr1==b.Dr1&&a.Dr2==b.Dr2&&a.Dr3==b.Dr3&&a.Dr6==b.Dr6&&a.Dr7==b.Dr7;}
static void run(Thread& t){SetEvent(t.go);assert(WaitForSingleObject(t.done,2000)==WAIT_OBJECT_0);}
int main(){
 auto* mem=static_cast<float*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(mem);mem[0]=mem[5]=mem[10]=mem[15]=1;hero.hero=reinterpret_cast<std::uintptr_t>(mem+32);hero.transform=reinterpret_cast<std::uintptr_t>(mem);incoming[12]=10;incoming[13]=20;incoming[14]=30;incoming[15]=1;
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
 std::array<Thread,4> threads{};for(unsigned i=0;i<threads.size();++i){auto& t=threads[i];t.role=i==3?3:i?1:0;t.ready=CreateEventW(nullptr,TRUE,FALSE,nullptr);t.go=CreateEventW(nullptr,FALSE,FALSE,nullptr);t.done=CreateEventW(nullptr,FALSE,FALSE,nullptr);t.handle=CreateThread(nullptr,0,loop,&t,0,&t.tid);assert(t.handle&&WaitForSingleObject(t.ready,2000)==WAIT_OBJECT_0);t.previous=debug(t);}
 auto foreign2=threads[0].previous;foreign2.Dr2=base;foreign2.Dr7|=0x10;setDebug(threads[0],foreign2);foreign2=debug(threads[0]);auto foreign0=threads[2].previous;foreign0.Dr0=base;foreign0.Dr7|=1;setDebug(threads[2],foreign0);foreign0=debug(threads[2]);
 assert(msm2::trace::configureReset(true,base,nt->OptionalHeader.SizeOfImage,true,true));
 msm2::reset_trace::Request r{};r.prearmed=r.multithread=r.verifyDual=true;r.session=1;r.generation=1;r.initMs=GetTickCount64()-900;r.hero=hero.hero;r.transform=hero.transform;r.owner=hero.hero;r.thread=threads[0].tid;r.normal={0,0,1};r.basis={{-1,0,0},{0,0,1},{0,1,0},true};
 msm2::Snapshot sample{};assert(msm2::sample(sample));assert(msm2::trace::beginReset(r,sample));auto p=r;p.position=true;p.initMs=GetTickCount64()-1600;assert(!msm2::trace::beginPosition(p,sample));msm2::trace::pollReset(false);assert(!activeLeases());
 auto arm=[&](unsigned session){p.session=session;p.initMs=GetTickCount64()-1600;assert(msm2::sample(sample));assert(msm2::trace::beginPosition(p,sample));if(windows[unsigned(current)].state!=5||windows[unsigned(current)].foreignThreads<1){std::printf("ARM session=%u state=%u seen=%u installed=%u failed=%u foreign=%u\n",session,windows[unsigned(current)].state,windows[unsigned(current)].seen,windows[unsigned(current)].installedThreads,windows[unsigned(current)].failedThreads,windows[unsigned(current)].foreignThreads);std::printf("%s",output.c_str());}assert(windows[unsigned(current)].state==5&&windows[unsigned(current)].foreignThreads>=1);};

 auto* guarded=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);assert(guarded);DWORD previous{};assert(VirtualProtect(guarded,4096,PAGE_READWRITE|PAGE_GUARD,&previous));float untouched{};assert(!lineageRead(reinterpret_cast<std::uintptr_t>(guarded),&untouched,4));MEMORY_BASIC_INFORMATION guardState{};assert(VirtualQuery(guarded,&guardState,sizeof(guardState))&&(guardState.Protect&PAGE_GUARD));VirtualFree(guarded,0,MEM_RELEASE);
 assert(output.find("exact_runtime_writer_boundary_missing")!=std::string::npos); // Owned executable is never accepted as the game writer.
 arm(2);
 auto ownedBegin=reinterpret_cast<std::uintptr_t>(ws_owned_lineage);auto ownedEnd=reinterpret_cast<std::uintptr_t>(ws_owned_lineage_end);assert(ownedEnd>ownedBegin&&ownedEnd-ownedBegin<1024);auto ownedProof=ws::lineage::writer(reinterpret_cast<const unsigned char*>(ownedBegin),ownedEnd-ownedBegin);assert(ownedProof.complete);
 auto ringStart=head.load();run(threads[3]);unsigned realScalarSamples=0,realFullMatches=0;
 for(unsigned i=ringStart;i<head.load();++i){auto& event=events[i%ringCap];if(event.thread!=threads[3].tid||event.ripAfter<=ownedBegin||event.ripAfter>ownedEnd)continue;ws::lineage::Context context{};std::memcpy(context.gpr.data(),event.gpr,sizeof(event.gpr));std::memcpy(context.xmm.data(),event.xmm,sizeof(event.xmm));context.floating=event.xmmValid;auto observed=ws::lineage::sample(ownedProof,unsigned(event.ripAfter-ownedBegin),context,hero.transform,event.newUp,lineageRead);assert(observed.target&&observed.fullRow&&observed.mask==15);++realScalarSamples;if(observed.match)++realFullMatches;}
 assert(realScalarSamples==4&&realFullMatches>=1); // Earlier scalar stores need not match the full row.
 run(threads[0]);assert(!std::memcmp(mem+12,incoming+12,16));run(threads[1]);assert(std::memcmp(mem+12,incoming+12,16)&&mem[15]==7);for(unsigned i=0;i<9;++i){incoming[12]+=1;run(threads[0]);run(threads[1]);}msm2::trace::drainLogs();auto& w=windows[unsigned(current)];assert(w.positionWriterCount>=2&&w.positionUnknown>0&&w.hits>=18);unsigned printed=0;for(auto& writer:w.positionWriters)printed+=std::min(writer.printed,4u);assert(printed<=4*w.positionWriterCount);
 msm2::trace::pollReset(false);msm2::trace::drainLogs();assert(!activeLeases()&&same(debug(threads[0]),foreign2)&&same(debug(threads[1]),threads[1].previous)&&same(debug(threads[2]),foreign0));
 assert(output.find("effective_address_verified=1")!=std::string::npos&&output.find("position_w_bits_after=0x40e00000")!=std::string::npos&&output.find("cleanup_restored=1 cleanup_pending=0")!=std::string::npos);
 arm(3);for(unsigned i=0;i<300&&windows[unsigned(current)].capture.load();++i)run(threads[0]);assert(!windows[unsigned(current)].capture.load()&&windows[unsigned(current)].hits<=256);msm2::trace::pollReset(false);msm2::trace::drainLogs();assert(!activeLeases());
 arm(4);Sleep(620);assert(!msm2::trace::pollReset(true)&&!activeLeases());
 arm(5);msm2::trace::cancelReset();assert(!msm2::trace::pollReset(true)&&!activeLeases());
 auto wrong=p;wrong.initMs=GetTickCount64()-1000;assert(!msm2::trace::beginPosition(wrong,sample));wrong=p;wrong.transform+=16;assert(!msm2::trace::beginPosition(wrong,sample));wrong=p;wrong.basis.valid=false;assert(!msm2::trace::beginPosition(wrong,sample));
 arm(6);msm2::trace::kill();assert(!msm2::trace::pollReset(true)&&!activeLeases());msm2::trace::stop();
 assert(same(debug(threads[0]),foreign2)&&same(debug(threads[1]),threads[1].previous)&&same(debug(threads[2]),foreign0));assert(same(debug(threads[3]),threads[3].previous));setDebug(threads[0],threads[0].previous);setDebug(threads[2],threads[2].previous);
 MEMORY_BASIC_INFORMATION page{};assert(VirtualQuery(mem,&page,sizeof(page))&&page.Protect==PAGE_READWRITE&&mem[5]==1);assert(output.find("PATH_A_POSITION_CAUSALITY_PROOF admitted=0")!=std::string::npos);
 auto file=std::fopen("logs/windows_position_writer_synthetic.log","wb");assert(file);std::fwrite(output.data(),1,output.size(),file);std::fclose(file);
 finish=true;for(auto& t:threads){SetEvent(t.go);assert(WaitForSingleObject(t.handle,2000)==WAIT_OBJECT_0);for(auto h:{t.handle,t.ready,t.go,t.done})CloseHandle(h);}VirtualFree(mem,0,MEM_RELEASE);
 puts("PASS: c12 real scalar MOVSS hardware contexts reconstruct full XYZ/W with earlier partial-row negatives; PAGE_GUARD refused unchanged, owned module not admitted as game, exact debug restoration and unchanged c11 watch bounds");
 puts("PASS: c11 real Position Vec4 hardware watch after exclusive Up cleanup; two owned Position writers/anchored stores/fourth-component observations, unique identity aggregation/4-event cap, raw256 cap, foreign DR0 refusal/DR2 exact preservation, timeout/Deinit/F8/identity/timing/invalid-frame gates and full restore; no protection or orientation modification");
}
