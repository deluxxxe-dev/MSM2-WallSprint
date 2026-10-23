// TEST ONLY: two synthetic native writer threads and owned Transform allocation.
#include "orientation_trace.cpp"
#include <cassert>
#include <string>
#include <cstdarg>
#include <cstdio>
static std::string output;static std::uintptr_t fakeHero{},fakeTransform{};
static std::atomic<bool> running{true};static std::atomic<unsigned> selected{1},crawl{0};static std::atomic<bool> burst{false};static std::atomic<unsigned> nativeCalls{0};
static HANDLE threadReady[2]{};
namespace msm2::native_source {bool diagnosticIdentity(const msm2::reset_trace::Request&) noexcept {return false;}}
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T done=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&done)&&done==n;}
bool sample(Snapshot& s){s={};s.hero=fakeHero;s.transform=fakeTransform;float data[15]{};if(!readMemory(fakeTransform,data,sizeof(data)))return false;std::memcpy(s.prefix,data,48);std::memcpy(s.position,data+12,12);return true;}
void writeLog(const char* fmt,...){char b[4096];va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);output+=b;output+='\n';}
}
__attribute__((noinline,noclone)) void nativeWriter(volatile float* t,float x,float y){
 volatile float scratch[4]{x,y,x,y};
 t[0]=scratch[1];t[1]=-scratch[0];t[2]=0;
 t[4]=scratch[0];t[5]=scratch[1];t[6]=0;
 t[8]=t[9]=0;t[10]=1;
 ++nativeCalls;
}
DWORD WINAPI nativeLoop(void* raw){auto id=unsigned(reinterpret_cast<std::uintptr_t>(raw));SetEvent(threadReady[id-1]);while(running.load()){if(selected.load()==id){bool c=crawl.load();nativeWriter(reinterpret_cast<float*>(fakeTransform),c?1.f:0.f,c?0.f:1.f);}Sleep(burst.load()?0:5);}return 0;}
CONTEXT debug(HANDLE t){assert(SuspendThread(t)!=DWORD(-1));CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;assert(GetThreadContext(t,&c));assert(ResumeThread(t)!=DWORD(-1));return c;}
int main(){
 auto* t=static_cast<float*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));auto* h=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);auto* owner=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(t&&h&&owner);fakeTransform=reinterpret_cast<std::uintptr_t>(t);fakeHero=reinterpret_cast<std::uintptr_t>(h);std::memcpy(h,&fakeTransform,8);t[0]=t[5]=t[10]=1;t[3]=7;t[7]=8;t[11]=9;t[12]=10;t[13]=20;t[14]=30;ws::Vec3 n{1,0,0};std::memcpy(owner+0x398,&n,12);
 for(auto& ready:threadReady)ready=CreateEventW(nullptr,TRUE,FALSE,nullptr);
 DWORD ids[2]{};HANDLE threads[2]={CreateThread(nullptr,0,nativeLoop,reinterpret_cast<void*>(1),0,&ids[0]),CreateThread(nullptr,0,nativeLoop,reinterpret_cast<void*>(2),0,&ids[1])};assert(threads[0]&&threads[1]);assert(WaitForMultipleObjects(2,threadReady,TRUE,1000)==WAIT_OBJECT_0);auto original0=debug(threads[0]),original1=debug(threads[1]);
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
 msm2::trace::Config cfg;cfg.windowMs=120;ws::observed::VerifiedWallContext ctx{};ctx.lifecycleVerified=ctx.wallRunActive=true;ctx.wallRunSessionId=1;ctx.wallRunOwner=reinterpret_cast<std::uintptr_t>(owner);ctx.elapsed=1;ctx.wallRunUpdateCalls=20;
 msm2::trace::configure(cfg,false,base,nt->OptionalHeader.SizeOfImage);msm2::trace::tick(ctx,ids[0],"none");assert(!usedWindows);
 msm2::trace::configure(cfg,true,base,nt->OptionalHeader.SizeOfImage);msm2::trace::tick(ctx,ids[0],"none");assert(usedWindows==1&&current==0);assert(windows[0].capture);
 for(unsigned i=0;i<20;++i){Sleep(10);msm2::trace::tick(ctx,ids[0],"none");msm2::trace::drainLogs();}
 if(!windows[0].hits)std::printf("TRACE_FIXTURE_FAILURE native_calls=%u output=%s\n",nativeCalls.load(),output.c_str());
 assert(windows[0].hits>0);assert(current<0&&!activeLeases());assert(writerCount>0);assert(output.find("state=WallRun")!=std::string::npos);
 auto after0=debug(threads[0]);assert(after0.Dr0==original0.Dr0&&after0.Dr1==original0.Dr1&&(after0.Dr7&ownMask)==(original0.Dr7&ownMask));
 MEMORY_BASIC_INFORMATION page{};assert(VirtualQuery(t,&page,sizeof(page))&&page.Protect==PAGE_READWRITE);assert(t[3]==7&&t[7]==8&&t[11]==9&&t[12]==10&&t[13]==20&&t[14]==30);
 // Positive control uses the same native writer on a different thread, no MOD rotation.
 ctx.wallRunActive=false;ctx.wallRunOwner=0;ctx.wallRunSessionId=0;msm2::trace::tick(ctx,ids[0],"verified_deinit");crawl=1;selected=2;
 for(unsigned i=0;i<35;++i){Sleep(10);msm2::trace::tick(ctx,ids[0],"verified_deinit");msm2::trace::drainLogs();}
 assert(usedWindows==2&&windows[1].state==2&&windows[1].hits>0);assert(current<0&&!activeLeases());assert(output.find("state=CrawlCandidate")!=std::string::npos);
 bool common=false;for(unsigned i=0;i<writerCount;++i)if(writers[i].run&&writers[i].crawl)common=true;assert(common);auto after1=debug(threads[1]);assert(after1.Dr0==original1.Dr0&&after1.Dr1==original1.Dr1&&(after1.Dr7&ownMask)==(original1.Dr7&ownMask));
 // Unrelated exceptions and debugger slots are not consumed or replaced.
 EXCEPTION_RECORD ex{};CONTEXT c{};EXCEPTION_POINTERS ep{&ex,&c};ex.ExceptionCode=EXCEPTION_ACCESS_VIOLATION;assert(handler(&ep)==EXCEPTION_CONTINUE_SEARCH);ex.ExceptionCode=EXCEPTION_SINGLE_STEP;c.ContextFlags=CONTEXT_DEBUG_REGISTERS;c.Dr6=4;assert(handler(&ep)==EXCEPTION_CONTINUE_SEARCH);
 // Own retired watch records remain able to clear delayed debug contexts.
 Lease* old=nullptr;for(unsigned i=0;i<usedLeases;++i)if(leases[i].tid==ids[0]){old=&leases[i];break;}assert(old);
 // A late retired context with one foreign slot clears only our other slot.
 auto& tombstone=leases[usedLeases++];tombstone.window=0;tombstone.up=old->up;tombstone.old0=old->old0;tombstone.old1=old->old1;tombstone.old7=old->old7;tombstone.tid=GetCurrentThreadId();
 c={};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;c.Dr0=base;c.Dr1=tombstone.up+8;c.Dr7=watchBits;c.Dr6=2;assert(handler(&ep)==EXCEPTION_CONTINUE_EXECUTION);assert(c.Dr0==base&&(c.Dr7&0xf0003ull)==0x90001ull&&c.Dr1==tombstone.old1);
 c={};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;c.Dr0=base;c.Dr1=tombstone.up+8;c.Dr7=watchBits;c.Dr6=3;assert(handler(&ep)==EXCEPTION_CONTINUE_SEARCH);assert((c.Dr6&1)&&c.Dr0==base&&c.Dr1==tombstone.old1);
 // Existing occupied DR0 is preserved. Limit candidate list to this primary thread.
 config.maxThreads=1;CONTEXT occupied=original0;occupied.Dr0=base;occupied.Dr7=(occupied.Dr7&~ownMask)|0x90001;
 assert(SuspendThread(threads[0])!=DWORD(-1));assert(SetThreadContext(threads[0],&occupied));assert(ResumeThread(threads[0])!=DWORD(-1));
 Snapshot snapshot{};assert(msm2::sample(snapshot));assert(!begin(snapshot,1,2,reinterpret_cast<std::uintptr_t>(owner),n,ids[0]));auto kept=debug(threads[0]);assert(kept.Dr0==base&&(kept.Dr7&ownMask)==(occupied.Dr7&ownMask));
 assert(SuspendThread(threads[0])!=DWORD(-1));assert(SetThreadContext(threads[0],&original0));assert(ResumeThread(threads[0])!=DWORD(-1));config.maxThreads=32;
 // Hard event budget and identity cancellation are independent of timeout.
 config.windowMs=500;burst=true;assert(begin(snapshot,2,3,0,n,ids[0]));unsigned capped=unsigned(current);
 for(unsigned k=0;k<60&&current>=0;++k){Sleep(10);msm2::trace::tick(ctx,ids[0],"verified_deinit");msm2::trace::drainLogs();}
 burst=false;assert(windows[capped].hits>=config.maxEvents);assert(!activeLeases());config.windowMs=120;
 assert(begin(snapshot,2,4,0,n,ids[0]));auto savedHero=fakeHero;fakeHero+=16;Sleep(25);msm2::trace::tick(ctx,ids[0],"verified_deinit");fakeHero=savedHero;msm2::trace::tick(ctx,ids[0],"verified_deinit");assert(!activeLeases());
 // F8-equivalent stops an active window and restores debug contexts.
 assert(begin(snapshot,2,3,0,n,ids[0]));msm2::trace::kill();msm2::trace::tick(ctx,ids[0],"verified_deinit");assert(!activeLeases());auto calls=nativeCalls.load();Sleep(30);assert(nativeCalls>calls); // original still executes
 msm2::trace::stop();assert(!activeLeases());running=false;for(auto thread:threads){assert(WaitForSingleObject(thread,1000)==WAIT_OBJECT_0);CloseHandle(thread);}
 for(auto ready:threadReady)CloseHandle(ready);
 assert(output.find("orientation_writer_function")!=std::string::npos&&output.find("ORIENTATION_WRITER_REGS")!=std::string::npos);
 auto f=std::fopen("logs/windows_orientation_trace_synthetic.log","wb");assert(f);std::fwrite(output.data(),1,output.size(),f);std::fclose(f);
 assert(VirtualFree(t,0,MEM_RELEASE)&&VirtualFree(h,0,MEM_RELEASE)&&VirtualFree(owner,0,MEM_RELEASE));
 puts("PASS: real hardware Up write-watch and VEH on two owned-memory native threads; WallRun/CrawlCandidate same writer, RIP/regs/callers/code, timeout/event-budget/identity/F8 DR restoration, occupied debug-slot refusal, unrelated exception pass-through, page protection/position/padding unchanged; no game execution");
}
