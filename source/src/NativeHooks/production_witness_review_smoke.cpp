// TEST ONLY: owned B0 + foreign Dr2 events pass the unchanged production VEH
// and are held in a following test-owned handler. Observer/downstream handler
// never edits CONTEXT; the original engine performs its normal B0 claim.
// Foreign-only controls separately retain their observed non-resurrection.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include <cassert>
static PVECTORED_EXCEPTION_HANDLER engine{};
static DWORD workerId{};
static std::atomic<unsigned> passedSearch{0},foreignEvents{0};
static std::atomic<bool> contextUnchanged{false},armedSavedContext{false},debugContextPresent{false};
static HANDLE foreignHeld{},allowReturn{};
static LONG CALLBACK observeProduction(EXCEPTION_POINTERS*);
static PVOID WINAPI installObserver(ULONG first,PVECTORED_EXCEPTION_HANDLER fn){assert(!engine);engine=fn;return AddVectoredExceptionHandler(first,observeProduction);}
#define AddVectoredExceptionHandler installObserver
#include "../msm2/orientation_trace.cpp"
#undef AddVectoredExceptionHandler
#include <cstdio>
#include <cstdarg>
#include <string>
#include "NativeHooks/continuation_diagnostics.hpp"
static msm2::Snapshot hero;static std::string logText;static bool foreignOnly{};static unsigned targetOffset{};
static HANDLE ready{},go{},done{},finish{};
static std::atomic<unsigned> stores{0};
namespace msm2::native_source {bool diagnosticIdentity(const msm2::reset_trace::Request&) noexcept{return false;}}
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t size){SIZE_T n{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,size,&n)&&n==size;}
bool sample(Snapshot& s){s=hero;return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char text[4096];va_list a;va_start(a,fmt);vsnprintf(text,sizeof(text),fmt,a);va_end(a);logText+=text;logText+='\n';}
}
using Registers=ws::continuation_diagnostics::Registers;
static Registers registers(const CONTEXT& c){return {c.Dr0,c.Dr1,c.Dr2,c.Dr3,c.Dr6,c.Dr7};}
static bool sameSlots(const CONTEXT& a,const CONTEXT& b){return a.Dr0==b.Dr0&&a.Dr1==b.Dr1&&a.Dr2==b.Dr2&&a.Dr3==b.Dr3&&a.Dr7==b.Dr7;}
static LONG CALLBACK observeProduction(EXCEPTION_POINTERS* e){
 const auto before=e&&e->ContextRecord?registers(*e->ContextRecord):Registers{};
 const auto result=engine(e);
 if(e&&e->ExceptionRecord&&e->ContextRecord&&GetCurrentThreadId()==workerId&&e->ExceptionRecord->ExceptionCode==EXCEPTION_SINGLE_STEP&&(e->ContextRecord->Dr6&0xf)==4){
  auto observed=registers(*e->ContextRecord);contextUnchanged=before==observed;
  if(!foreignOnly){auto claimed=before;claimed[4]&=~1ull;assert(claimed==observed);}
  debugContextPresent=(e->ContextRecord->ContextFlags&CONTEXT_DEBUG_REGISTERS)==CONTEXT_DEBUG_REGISTERS;
  armedSavedContext=usedLeases==1&&own(*e->ContextRecord,leases[0])==3;
  if(result==EXCEPTION_CONTINUE_SEARCH)++passedSearch;
 }
 return result;
}
static LONG CALLBACK foreignOwner(EXCEPTION_POINTERS* e){
 if(!e||!e->ExceptionRecord||!e->ContextRecord||GetCurrentThreadId()!=workerId||e->ExceptionRecord->ExceptionCode!=EXCEPTION_SINGLE_STEP||(e->ContextRecord->Dr6&0xf)!=4)return EXCEPTION_CONTINUE_SEARCH;
 assert(++foreignEvents==1);assert(SetEvent(foreignHeld));assert(WaitForSingleObject(allowReturn,2000)==WAIT_OBJECT_0);
 // This downstream owner consumes its real foreign event, without writing any
 // CONTEXT, DR, RIP, stack, ticket or production lease field.
 return EXCEPTION_CONTINUE_EXECUTION;
}
static DWORD WINAPI foreignWriter(void*){
 assert(SetEvent(ready));assert(WaitForSingleObject(go,2000)==WAIT_OBJECT_0);
 *reinterpret_cast<volatile unsigned*>(hero.transform+targetOffset)=1;++stores;
 assert(SetEvent(done));assert(WaitForSingleObject(finish,2000)==WAIT_OBJECT_0);return 0;
}
static CONTEXT debug(HANDLE h){assert(SuspendThread(h)!=DWORD(-1));CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;assert(GetThreadContext(h,&c));assert(ResumeThread(h)!=DWORD(-1));return c;}
static std::string json(const CONTEXT& c){auto r=registers(c);std::string s="{";for(unsigned i=0;i<6;++i){if(i)s+=',';s+="\""+std::string(ws::continuation_diagnostics::names[i])+"\":\""+ws::continuation_diagnostics::hex(r[i])+"\"";}return s+"}";}
int main(int argc,char** argv){
 bool position=false,deadline=false,kill=false;for(int i=1;i<argc;++i){if(!std::strcmp(argv[i],"--position"))position=true;else if(!std::strcmp(argv[i],"--timeout"))deadline=true;else if(!std::strcmp(argv[i],"--kill"))kill=true;else if(!std::strcmp(argv[i],"--foreign-only"))foreignOnly=true;else assert(false);}assert(!(deadline&&kill));
 targetOffset=foreignOnly?0x100:position?0x30:0x10;
 auto* memory=static_cast<float*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);memory[0]=memory[5]=memory[10]=memory[15]=1;hero.hero=reinterpret_cast<std::uintptr_t>(memory+32);hero.transform=reinterpret_cast<std::uintptr_t>(memory);
 ready=CreateEventW(nullptr,FALSE,FALSE,nullptr);go=CreateEventW(nullptr,FALSE,FALSE,nullptr);done=CreateEventW(nullptr,FALSE,FALSE,nullptr);finish=CreateEventW(nullptr,FALSE,FALSE,nullptr);foreignHeld=CreateEventW(nullptr,FALSE,FALSE,nullptr);allowReturn=CreateEventW(nullptr,FALSE,FALSE,nullptr);assert(ready&&go&&done&&finish&&foreignHeld&&allowReturn);
 auto thread=CreateThread(nullptr,0,foreignWriter,nullptr,0,&workerId);assert(thread&&WaitForSingleObject(ready,2000)==WAIT_OBJECT_0);
 auto foreign=debug(thread);assert(!(foreign.Dr7&0xff));foreign.Dr2=hero.transform+targetOffset;foreign.Dr7|=0xd000010ull;
 assert(SuspendThread(thread)!=DWORD(-1));assert(SetThreadContext(thread,&foreign));assert(ResumeThread(thread)!=DWORD(-1));const auto saved=debug(thread);
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
 assert(msm2::trace::configureReset(true,base,nt->OptionalHeader.SizeOfImage,true,true));
 auto downstream=AddVectoredExceptionHandler(0,foreignOwner);assert(downstream);
 msm2::reset_trace::Request r{};r.prearmed=r.verifyDual=r.multithread=true;r.initMs=GetTickCount64()-(position?1600:600);r.session=r.generation=1;r.hero=hero.hero;r.transform=hero.transform;r.owner=hero.hero;r.thread=workerId;r.position=position;r.normal={0,0,1};r.basis={{-1,0,0},{0,0,1},{0,1,0},true};msm2::Snapshot s{};assert(msm2::sample(s));assert(position?msm2::trace::beginPosition(r,s):msm2::trace::beginReset(r,s));
 assert(usedLeases==1);assert(SetEvent(go));assert(WaitForSingleObject(foreignHeld,2000)==WAIT_OBJECT_0);
 assert(passedSearch==1&&foreignEvents==1&&debugContextPresent&&contextUnchanged.load()==foreignOnly&&armedSavedContext&&stores==0);
 assert(windows[0].positionInflight==0&&windows[0].hits==unsigned(!foreignOnly)&&activeLeases());
 if(deadline){{const auto waitStart=GetTickCount64();while(now()<windows[0].end&&GetTickCount64()-waitStart<5000){const auto left=windows[0].end-now();Sleep(left>1?DWORD(std::ceil(left)):1);}}assert(now()>=windows[0].end);assert(!msm2::trace::pollReset(true));}
 else {if(kill)msm2::trace::kill();assert(!msm2::trace::pollReset(false,"c48_owned_foreign_return_hold"));}
 assert(!activeLeases()&&current==-1);const auto before=debug(thread);assert(sameSlots(saved,before));
 assert(SetEvent(allowReturn));assert(WaitForSingleObject(done,2000)==WAIT_OBJECT_0);const auto after=debug(thread);
 assert(own(after,leases[0])==(foreignOnly?0u:3u)&&!activeLeases()&&windows[0].positionInflight==0&&windows[0].hits==unsigned(!foreignOnly)&&stores==1);
 if(foreignOnly)assert(sameSlots(saved,after));
 assert(after.Dr2==saved.Dr2&&(after.Dr7&0xf000030ull)==(saved.Dr7&0xf000030ull));
 const auto scenario=deadline?"deadline":kill?"kill":"close";
 std::printf("PASS: c48 %s %s %s actual SEARCH/inflight0 holds armed saved context; %s; no production certificate\n",position?"Position":"reset",scenario,foreignOnly?"foreign_only":"owned_B0_plus_foreign_B2",foreignOnly?"unchanged context does not resurrect on this Wine path":"unchanged engine clears B0 then modified saved context resurrects slots after cleanup");
 std::printf("{\"task\":\"c48\",\"mode\":\"%s\",\"scenario\":\"%s\",\"foreign_only\":%s,\"production_result\":\"CONTINUE_SEARCH\",\"foreign_events\":1,\"owned_hits\":%u,\"inflight_while_held\":0,\"saved_context_armed\":true,\"engine_context_unchanged\":%s,\"observer_context_writes\":0,\"bookkeeping_inactive_after_poll\":true,\"current_slots_restored_while_held\":true,\"owned_slots_resurrect_after_return\":%s,\"saved\":%s,\"while_held\":%s,\"after_return\":%s,\"production_integrated\":false,\"game_used\":false}\n",position?"Position":"reset",scenario,foreignOnly?"true":"false",unsigned(!foreignOnly),foreignOnly?"true":"false",foreignOnly?"false":"true",json(saved).c_str(),json(before).c_str(),json(after).c_str());
 assert(SetEvent(finish));assert(WaitForSingleObject(thread,2000)==WAIT_OBJECT_0);msm2::trace::stop();assert(!activeLeases());assert(RemoveVectoredExceptionHandler(downstream));
 for(auto h:{thread,ready,go,done,finish,foreignHeld,allowReturn})CloseHandle(h);
 assert(VirtualFree(memory,0,MEM_RELEASE));
}
