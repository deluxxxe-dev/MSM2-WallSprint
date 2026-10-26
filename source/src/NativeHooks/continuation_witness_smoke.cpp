// TEST ONLY. Actual owned DR/VEH contexts, never linked into WallSprint.dll.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include <cassert>
static PVECTORED_EXCEPTION_HANDLER c41_engine{};
static LONG CALLBACK c41_handler(EXCEPTION_POINTERS*);
static PVOID WINAPI c41_install(ULONG first,PVECTORED_EXCEPTION_HANDLER fn){assert(!c41_engine);c41_engine=fn;return AddVectoredExceptionHandler(first,c41_handler);}
#define AddVectoredExceptionHandler c41_install
#include "../msm2/orientation_trace.cpp"
#undef AddVectoredExceptionHandler
#include <capstone/capstone.h>
#include <cpuid.h>
#include <cstdio>
#include <cstdarg>
#include <string>
#include <climits>
#include "NativeHooks/continuation_mitigations.hpp"
#include "NativeHooks/continuation_diagnostics.hpp"

namespace c41 {const char* failureText();std::string reportFields();[[noreturn]] void failed(const char* expression,unsigned line){std::fprintf(stderr,"c41 owned assertion at line %u: %s\n",line,expression);std::printf("{\"schema\":3,\"task\":\"c43\",\"witness_task\":\"c41\",\"result\":\"FAIL\",\"reason\":\"owned_assertion_failed\",\"rejection\":\"%s\",\"line\":%u,%s,\"game_used\":false}\n",failureText(),line,reportFields().c_str());std::fflush(stdout);ExitProcess(3);}}
#undef assert
#define assert(condition) do {if(!(condition))c41::failed(#condition,__LINE__);} while(false)

extern "C" {
alignas(64) unsigned char ws_c41_before[2048]{},ws_c41_after[2048]{};
alignas(32) unsigned char ws_c41_pattern[512]{};
volatile LONG ws_c41_captured{},ws_c41_finish{};
void ws_c41_machine(void*);
extern unsigned char ws_c41_after_store[];
}
namespace c41 {
namespace mitigation=ws::continuation_mitigations;
namespace diagnostics=ws::continuation_diagnostics;
diagnostics::Report restoration;
std::string classifierJson=mitigation::json(mitigation::Inputs{});
const char* classifierReport(){return classifierJson.c_str();}
constexpr unsigned capacity=4;constexpr DWORD nestedCode=0xe0410041;
constexpr std::uint64_t generation=101;constexpr unsigned lease=1;
struct Ticket {
 DWORD thread{};unsigned sequence{},watchLease{};std::uint64_t gen{};std::uintptr_t original{};
 HANDLE ready{},prepared{},held{},release{};unsigned char* storage{};volatile LONG* marker{};
 LONG token{};std::array<unsigned char,15> bytes{};std::array<unsigned char,16> target{};
};
std::array<Ticket,capacity> tickets;std::atomic<unsigned> published{0};unsigned next{},bound=capacity,nestedCount{};
std::atomic<bool> poisoned{false},armed{false};std::atomic<const char*> failure{nullptr};
DWORD workerId{};HANDLE worker{},ready{},go{};msm2::Snapshot hero{};
bool position{},timeout{},killClose{},capCase{},wine{},jsonOnly{};bool policyKnown[3]{};DWORD policyFlags[3]{};const char* environmentFailure="none";
std::string output;
const char* scenarioName(){return capCase?"cap_refusal":nestedCount==2?"multiple":nestedCount?"nested":timeout?"timeout":killClose?"kill":"close";}
std::string reportFields(){
 std::string out="\"environment\":\"";out+=wine?"Wine":"native_Windows";
 out+="\",\"mode\":\"";out+=position?"Position":"reset";
 out+="\",\"scenario\":\"";out+=scenarioName();
 out+="\",\"thread\":"+std::to_string(workerId)+",\"lease\":"+std::to_string(lease)+",\"generation\":"+std::to_string(generation)+",\"contexts\":"+std::to_string(published.load());
 out+=",\"mitigation_query_known\":[";for(unsigned i=0;i<3;++i){if(i)out+=',';out+=policyKnown[i]?"true":"false";}
 out+="],\"mitigation_flags\":[";for(unsigned i=0;i<3;++i){if(i)out+=',';out+=std::to_string(policyFlags[i]);}
 return out+"],\"mitigation_classifier\":"+classifierReport()+",\"final_restoration\":"+diagnostics::json(restoration);
}
LONG load(volatile LONG* p){return InterlockedCompareExchange(p,0,0);}
const char* failureText(){const auto value=failure.load();return value?value:"none";}
void reject(const char* why){const char* empty=nullptr;failure.compare_exchange_strong(empty,why);poisoned=true;}
bool identity(const Ticket& t,DWORD tid,unsigned seq,std::uint64_t gen,unsigned requestedLease=lease){return tid==workerId&&t.thread==tid&&seq==t.sequence&&gen==generation&&t.gen==gen&&seq>0&&seq<=bound&&t.watchLease==requestedLease&&requestedLease==lease;}
bool acknowledged(const Ticket& t){return t.marker&&load(t.marker)==t.token&&t.token!=0;}
unsigned pending(){unsigned n=0;for(unsigned i=0;i<published.load(std::memory_order_acquire);++i)if(!acknowledged(tickets[i]))++n;return n;}
bool relative(std::uintptr_t from,std::uintptr_t to,std::int32_t& d){
 if(to>=from){if(to-from>INT32_MAX)return false;d=std::int32_t(to-from);}
 else {if(from-to>std::uint64_t(INT32_MAX)+1)return false;d=from-to==std::uint64_t(INT32_MAX)+1?INT32_MIN:-std::int32_t(from-to);}return true;
}
bool encode(std::uintptr_t code,std::uintptr_t marker,std::uintptr_t target,LONG token,std::array<unsigned char,15>& b){
 if(!code||!marker||!target||!token||code>UINTPTR_MAX-15||marker%4)return false;
 std::int32_t m{},j{};if(!relative(code+10,marker,m)||!relative(code+15,target,j))return false;
 b={0xc7,0x05,0,0,0,0,0,0,0,0,0xe9,0,0,0,0};std::memcpy(b.data()+2,&m,4);std::memcpy(b.data()+6,&token,4);std::memcpy(b.data()+11,&j,4);return true;
}
bool decode(const Ticket& t){
 csh h{};if(cs_open(CS_ARCH_X86,CS_MODE_64,&h)!=CS_ERR_OK)return false;
 cs_option(h,CS_OPT_DETAIL,CS_OPT_ON);cs_insn* ins{};auto n=cs_disasm(h,t.bytes.data(),t.bytes.size(),reinterpret_cast<std::uintptr_t>(t.storage),0,&ins);
 bool ok=n==2&&ins[0].id==X86_INS_MOV&&ins[0].size==10&&ins[1].id==X86_INS_JMP&&ins[1].size==5&&ins[0].detail&&ins[1].detail;
 if(ok){auto& x=ins[0].detail->x86;auto& y=ins[1].detail->x86;ok=x.op_count==2&&x.operands[0].type==X86_OP_MEM&&x.operands[0].size==4&&x.operands[0].mem.base==X86_REG_RIP&&x.operands[0].mem.index==X86_REG_INVALID&&x.operands[0].mem.segment==X86_REG_INVALID&&x.operands[1].type==X86_OP_IMM&&std::uint32_t(x.operands[1].imm)==std::uint32_t(t.token)&&ins[0].address+10+x.operands[0].mem.disp==reinterpret_cast<std::uintptr_t>(t.marker)&&y.op_count==1&&y.operands[0].type==X86_OP_IMM&&std::uint64_t(y.operands[0].imm)==t.original&&!x.eflags&&!y.eflags;}
 for(std::size_t i=0;ok&&i<n;++i){cs_regs reads{},writes{};std::uint8_t nr{},nw{};ok=cs_regs_access(h,&ins[i],reads,&nr,writes,&nw)==CS_ERR_OK;for(unsigned j=0;j<nw;++j)if(writes[j]!=X86_REG_RIP)ok=false;}
 if(ins)cs_free(ins,n);
 cs_close(&h);return ok;
}
bool current(const Ticket& t){
 if(!t.storage||!t.marker)return false;
 MEMORY_BASIC_INFORMATION code{},data{};
 if(!VirtualQuery(t.storage,&code,sizeof(code))||!VirtualQuery(const_cast<LONG*>(t.marker),&data,sizeof(data))||code.State!=MEM_COMMIT||code.Protect!=PAGE_EXECUTE_READ||data.State!=MEM_COMMIT||data.Protect!=PAGE_READWRITE||code.AllocationBase!=t.storage||data.AllocationBase!=t.storage)return false;
 std::array<unsigned char,15> b{};std::array<unsigned char,16> target{};SIZE_T got{};
 return ReadProcessMemory(GetCurrentProcess(),t.storage,b.data(),b.size(),&got)&&got==b.size()&&b==t.bytes&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(t.original),target.data(),target.size(),&got)&&got==target.size()&&target==t.target;
}
bool prepare(Ticket& t){
 MEMORY_BASIC_INFORMATION code{};if(!VirtualQuery(reinterpret_cast<void*>(t.original),&code,sizeof(code))||code.State!=MEM_COMMIT||(code.Protect!=PAGE_EXECUTE_READ&&code.Protect!=PAGE_EXECUTE))return false;
 SIZE_T got{};if(!ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(t.original),t.target.data(),t.target.size(),&got)||got!=t.target.size())return false;
 const auto origin=t.original&~std::uintptr_t(65535);
 for(unsigned i=1;i<=256&&!t.storage;++i){const auto distance=std::uintptr_t((i+1)/2)*65536;std::uintptr_t at{};
  if(i%2){if(origin>UINTPTR_MAX-distance-8192)continue;at=origin+distance;}else{if(origin<distance)continue;at=origin-distance;}
  t.storage=static_cast<unsigned char*>(VirtualAlloc(reinterpret_cast<void*>(at),8192,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));
 }
 if(!t.storage)return false;
 t.marker=reinterpret_cast<volatile LONG*>(t.storage+4096);t.token=LONG(0x41000000u+t.sequence);
 if(!encode(reinterpret_cast<std::uintptr_t>(t.storage),reinterpret_cast<std::uintptr_t>(t.marker),t.original,t.token,t.bytes))return false;
 std::memcpy(t.storage,t.bytes.data(),t.bytes.size());DWORD previous{};
 return VirtualProtect(t.storage,4096,PAGE_EXECUTE_READ,&previous)&&previous==PAGE_READWRITE&&FlushInstructionCache(GetCurrentProcess(),t.storage,15)&&decode(t)&&current(t)&&load(t.marker)==0;
}
bool canRetire(){if(poisoned.load()||pending())return false;for(unsigned i=0;i<published.load();++i)if(!identity(tickets[i],workerId,i+1,generation)||!current(tickets[i]))return false;return true;}
bool freeStorage(){if(WaitForSingleObject(worker,0)!=WAIT_OBJECT_0)return false;for(auto& t:tickets)if(t.storage){assert(VirtualFree(t.storage,0,MEM_RELEASE));t.storage=nullptr;t.marker=nullptr;}return true;}
CONTEXT debug(){assert(SuspendThread(worker)!=DWORD(-1));CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;assert(GetThreadContext(worker,&c));assert(ResumeThread(worker)!=DWORD(-1));return c;}
bool same(const CONTEXT& a,const CONTEXT& b){return a.Dr0==b.Dr0&&a.Dr1==b.Dr1&&a.Dr2==b.Dr2&&a.Dr3==b.Dr3&&a.Dr6==b.Dr6&&a.Dr7==b.Dr7;}
diagnostics::Registers registers(const CONTEXT& c){return {c.Dr0,c.Dr1,c.Dr2,c.Dr3,c.Dr6,c.Dr7};}
DWORD WINAPI run(void*){SetEvent(ready);assert(WaitForSingleObject(go,2000)==WAIT_OBJECT_0);ws_c41_machine(reinterpret_cast<void*>(hero.transform+(position?0x30:0x10)));return 0;}
bool environment(){
 wine=GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"wine_get_version")!=nullptr;
 if(IsDebuggerPresent()){environmentFailure="debugger_present";return false;}
 unsigned a{},b{},c{},d{};if(!__get_cpuid(1,&a,&b,&c,&d)||(c&bit_AVX)==0||(c&bit_OSXSAVE)==0){environmentFailure="AVX_or_OSXSAVE_missing";return false;}
 unsigned lo{},hi{};__asm__ volatile("xgetbv":"=a"(lo),"=d"(hi):"c"(0));if((lo&6)!=6){environmentFailure="XCR0_YMM_state_missing";return false;}
 using Query=BOOL(WINAPI*)(HANDLE,PROCESS_MITIGATION_POLICY,PVOID,SIZE_T);Query query{};
 auto raw=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"GetProcessMitigationPolicy");static_assert(sizeof(query)==sizeof(raw));std::memcpy(&query,&raw,sizeof(query));
 PROCESS_MITIGATION_POLICY policies[]={ProcessDynamicCodePolicy,ProcessControlFlowGuardPolicy,ProcessUserShadowStackPolicy};
 static_assert(sizeof(PROCESS_MITIGATION_DYNAMIC_CODE_POLICY)==sizeof(DWORD)&&sizeof(PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY)==sizeof(DWORD)&&sizeof(PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY)==sizeof(DWORD));
 for(unsigned i=0;i<3;++i)policyKnown[i]=query&&query(GetCurrentProcess(),policies[i],&policyFlags[i],sizeof(DWORD));
 mitigation::Inputs values{};for(unsigned i=0;i<3;++i)values[i]={policyKnown[i],std::uint32_t(policyFlags[i])};
 classifierJson=mitigation::json(values);
 if(!mitigation::admitted(values)){environmentFailure=(!policyKnown[0]||!policyKnown[1]||!policyKnown[2])?"mitigation_query_unknown":"mitigation_relevant_or_unknown_bit";return false;}
 return true;
}
}
namespace msm2::native_source {bool diagnosticIdentity(const msm2::reset_trace::Request&) noexcept{return false;}}
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&got)&&got==n;}
bool sample(Snapshot& s){s=c41::hero;return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char b[4096];va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);c41::output+=b;c41::output+='\n';}
}
static LONG CALLBACK c41_handler(EXCEPTION_POINTERS* e){
 using namespace c41;if(!e||!e->ExceptionRecord||!e->ContextRecord)return EXCEPTION_CONTINUE_SEARCH;
 const bool nested=e->ExceptionRecord->ExceptionCode==nestedCode;
 const auto result=nested?EXCEPTION_CONTINUE_EXECUTION:c41_engine(e);
 if(result!=EXCEPTION_CONTINUE_EXECUTION){if(armed.load()&&GetCurrentThreadId()==workerId)reject("unmodeled_exception_context");return result;}
 if(GetCurrentThreadId()!=workerId){reject("thread_mismatch");return result;}
 if(next>=bound){reject("context_cap");return result;}
 auto& t=tickets[next++];t.thread=GetCurrentThreadId();t.sequence=next;t.gen=generation;t.watchLease=windows[0].id;t.original=e->ContextRecord->Rip;
 if((!nested&&t.original!=reinterpret_cast<std::uintptr_t>(ws_c41_after_store))||(e->ContextRecord->EFlags&0x10100)||(e->ContextRecord->ContextFlags&CONTEXT_CONTROL)!=CONTEXT_CONTROL||(e->ContextRecord->ContextFlags&CONTEXT_INTEGER)!=CONTEXT_INTEGER){reject("RIP_flags_or_CONTROL_INTEGER_boundary");return result;}
 published.store(next,std::memory_order_release);SetEvent(t.ready);assert(WaitForSingleObject(t.prepared,2000)==WAIT_OBJECT_0);
 if(!t.storage||!identity(t,GetCurrentThreadId(),t.sequence,generation)||load(t.marker)!=0){reject("unprepared_or_identity");return result;}
 // The sole CONTEXT change: route its original continuation through the owned
 // MOV [RIP+marker],token; JMP rel32 stub. No flags/register/stack edits.
 e->ContextRecord->Rip=reinterpret_cast<std::uintptr_t>(t.storage);
 if(!nested)for(unsigned i=0;i<nestedCount;++i)RaiseException(nestedCode,0,0,nullptr);
 SetEvent(t.held);assert(WaitForSingleObject(t.release,2000)==WAIT_OBJECT_0);return result;
}
int main(int argc,char** argv){
 using namespace c41;
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
 for(int i=1;i<argc;++i){if(!std::strcmp(argv[i],"--position"))position=true;else if(!std::strcmp(argv[i],"--timeout"))timeout=true;else if(!std::strcmp(argv[i],"--kill"))killClose=true;else if(!std::strcmp(argv[i],"--nested"))nestedCount=1;else if(!std::strcmp(argv[i],"--multiple"))nestedCount=2;else if(!std::strcmp(argv[i],"--json"))jsonOnly=true;else if(!std::strcmp(argv[i],"--cap")){capCase=true;bound=1;nestedCount=1;}else assert(false&&"unknown probe argument");}
 assert(!(timeout&&killClose));
 if(!environment()){std::printf("{\"schema\":3,\"task\":\"c43\",\"witness_task\":\"c41\",\"result\":\"REFUSED\",\"reason\":\"%s\",%s,\"game_used\":false}\n",environmentFailure,reportFields().c_str());return 77;}
 std::int32_t delta{};std::array<unsigned char,15> synthetic{};
 assert(relative(0x100000000,0x17fffffff,delta)&&delta==INT32_MAX);assert(relative(0x100000000,0x80000000,delta)&&delta==INT32_MIN);assert(!relative(0x100000000,0x180000000,delta));
 assert(!encode(UINTPTR_MAX-10,4096,8192,1,synthetic)&&!encode(4096,4101,8192,1,synthetic)&&!encode(4096,8192,UINTPTR_MAX,1,synthetic));
 for(unsigned i=0;i<512;++i)ws_c41_pattern[i]=static_cast<unsigned char>(i*37+11);
 auto* memory=static_cast<float*>(VirtualAlloc(nullptr,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(memory);memory[0]=memory[5]=memory[10]=memory[15]=1;hero.hero=reinterpret_cast<std::uintptr_t>(memory+32);hero.transform=reinterpret_cast<std::uintptr_t>(memory);
 for(auto& t:tickets){t.ready=CreateEventW(nullptr,FALSE,FALSE,nullptr);t.prepared=CreateEventW(nullptr,FALSE,FALSE,nullptr);t.held=CreateEventW(nullptr,FALSE,FALSE,nullptr);t.release=CreateEventW(nullptr,FALSE,FALSE,nullptr);assert(t.ready&&t.prepared&&t.held&&t.release);}
 ready=CreateEventW(nullptr,FALSE,FALSE,nullptr);go=CreateEventW(nullptr,FALSE,FALSE,nullptr);worker=CreateThread(nullptr,0,run,nullptr,0,&workerId);assert(worker&&WaitForSingleObject(ready,2000)==WAIT_OBJECT_0);
 auto saved=debug();assert(!(saved.Dr7&0xff)&&!(saved.Dr7&(1ull<<13)));auto foreign=saved;foreign.Dr2=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));foreign.Dr7|=0x10;
 assert(SuspendThread(worker)!=DWORD(-1));assert(SetThreadContext(worker,&foreign));assert(ResumeThread(worker)!=DWORD(-1));saved=debug();restoration.saved=registers(saved);
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
 assert(msm2::trace::configureReset(true,base,nt->OptionalHeader.SizeOfImage,true,true));msm2::reset_trace::Request r{};r.prearmed=r.verifyDual=r.multithread=true;r.session=lease;r.generation=generation;r.initMs=GetTickCount64()-(position?1600:600);r.hero=hero.hero;r.transform=hero.transform;r.owner=hero.hero;r.thread=workerId;r.position=position;r.normal={0,0,1};r.basis={{-1,0,0},{0,0,1},{0,1,0},true};msm2::Snapshot s{};assert(msm2::sample(s));assert(position?msm2::trace::beginPosition(r,s):msm2::trace::beginReset(r,s));
 armed=true;SetEvent(go);auto& outer=tickets[0];assert(WaitForSingleObject(outer.ready,2000)==WAIT_OBJECT_0);assert(prepare(outer));assert(identity(outer,workerId,1,generation)&&!identity(outer,workerId+1,1,generation)&&!identity(outer,workerId,2,generation)&&!identity(outer,workerId,1,generation+1)&&!identity(outer,workerId,1,generation,lease+1));assert(!canRetire()&&pending()==1);
 auto wrong=outer;wrong.bytes[0]=0x90;assert(!decode(wrong)&&!c41::current(wrong));wrong=outer;++wrong.original;assert(!decode(wrong));assert(c41::current(outer));SetEvent(outer.prepared);
 for(unsigned i=1;i<=nestedCount&&!capCase;++i){auto& t=tickets[i];assert(WaitForSingleObject(t.ready,2000)==WAIT_OBJECT_0);assert(prepare(t));SetEvent(t.prepared);assert(WaitForSingleObject(t.held,2000)==WAIT_OBJECT_0);assert(load(t.marker)==0&&pending()>=2&&!canRetire());msm2::trace::cancelReset();assert(activeLeases()&&!canRetire());SetEvent(t.release);
  const auto deadline=GetTickCount64()+2000;while(!acknowledged(t)&&GetTickCount64()<deadline)SwitchToThread();assert(acknowledged(t)&&!acknowledged(outer)&&!canRetire());
 }
 assert(WaitForSingleObject(outer.held,2000)==WAIT_OBJECT_0);assert(windows[0].hits==1&&windows[0].positionInflight==0&&load(outer.marker)==0);
 if(timeout){const auto left=windows[0].end-now();if(left>0)Sleep(DWORD(std::ceil(left))+1);assert(now()>=windows[0].end);}
 if(killClose)msm2::trace::kill();else msm2::trace::cancelReset();
 assert(!canRetire()&&activeLeases()&&pending()>=1);assert(!freeStorage());
 // Test-owned coordinator deliberately REFUSES the legacy final release while
 // any ticket is outstanding. Neither deadline nor kill discharges a ticket.
 SetEvent(outer.release);auto deadline=GetTickCount64()+2000;while(!acknowledged(outer)&&GetTickCount64()<deadline)SwitchToThread();assert(acknowledged(outer)&&pending()==0);
 if(capCase){assert(poisoned.load()&&!std::strcmp(failureText(),"context_cap")&&!canRetire());}
 else {
  // Read-only snapshots of the existing owned, live worker. No formatting,
  // allocation, second watched hit or register write is added around final poll.
  restoration.attempted=true;restoration.before=registers(debug());
  restoration.activeBefore=activeLeases();restoration.pendingBefore=pending();
  restoration.canRetireBefore=canRetire();assert(*restoration.canRetireBefore);
  restoration.pollReturned=msm2::trace::pollReset(false,"owned_c41_post_return_witness");
  restoration.activeAfter=activeLeases();restoration.pendingAfter=pending();
  const auto after=debug();restoration.after=registers(after);
  // Record both sides even if poll unexpectedly returns true. Preserve every
  // acceptance predicate and the three exact stability readbacks, unmasked.
  assert(!*restoration.pollReturned);
  assert(!*restoration.activeAfter&&same(after,saved));
  for(unsigned i=0;i<3;++i){const auto check=debug();restoration.rechecks[i]=registers(check);assert(same(check,saved));}
 }
 deadline=GetTickCount64()+2000;while(!load(&ws_c41_captured)&&GetTickCount64()<deadline)SwitchToThread();assert(load(&ws_c41_captured));
 assert(!std::memcmp(ws_c41_before,ws_c41_after,128));assert(!std::memcmp(ws_c41_before+128,ws_c41_after+128,8));assert(!std::memcmp(ws_c41_before+160,ws_c41_after+160,512));assert(!std::memcmp(ws_c41_before+672,ws_c41_after+672,512));assert(!std::memcmp(ws_c41_before+1184,ws_c41_after+1184,64));
 assert(windows[0].hits==1&&!freeStorage());InterlockedExchange(&ws_c41_finish,1);assert(WaitForSingleObject(worker,2000)==WAIT_OBJECT_0);msm2::trace::stop();assert(!activeLeases());assert(freeStorage());
 const auto scenario=scenarioName();
 if(!jsonOnly)std::printf("PASS: c41 %s %s: exact GPR/RFLAGS/RSP/active-stack/YMM0-15/FXSAVE state; %s; no second watched hit; RX storage retained until thread exit\n",position?"Position":"reset",scenario,capCase?"unknown debt refuses final retirement, terminal-thread teardown only":"per-context post-return debt acknowledged then exact foreign DR2/owned DR restoration");
 std::printf("{\"schema\":3,\"task\":\"c43\",\"witness_task\":\"c41\",\"result\":\"%s\",%s,\"retirement_accepted\":%s,\"game_used\":false,\"new_gameplay_writes\":0,\"production_integrated\":false,\"native_candidate_probe_passed\":%s}\n",capCase?"FAIL_CLOSED_EXPECTED":"OWNED_WITNESS_PASS",reportFields().c_str(),capCase?"false":"true",!wine&&!capCase?"true":"false");
 for(auto& t:tickets)for(auto h:{t.ready,t.prepared,t.held,t.release})CloseHandle(h);
 for(auto h:{worker,ready,go})CloseHandle(h);
 assert(VirtualFree(memory,0,MEM_RELEASE));
}
