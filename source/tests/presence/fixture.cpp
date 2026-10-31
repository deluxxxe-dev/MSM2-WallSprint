#define _WIN32_WINNT 0x0602
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#include <MinHook.h>
extern "C" {
#include "hde64.h"
}
#include <array>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <thread>
#include <stdexcept>
extern "C" {
alignas(32) float ws_test_vector_input[128],ws_test_vectors[128],ws_test_returns[128];
uint64_t ws_test_gprs[11],ws_test_result,ws_test_return_gprs[11],ws_test_nonvolatile[8];
void presence_unwind_return();
uint64_t presence_unwind_caller();
void ws_test_probe_target(); void presence_race_target();
void ws_test_invoke_probe(void*); void presence_request(); void presence_gravity();
void presence_observer_request(); void presence_observer_gravity();
void presence_observer_request_restore0();
void presence_observer_request_restore1();
void presence_observer_request_restore2();
void presence_observer_request_restore3();
void presence_observer_request_restore4();
void presence_observer_request_restore5();
void presence_observer_request_restore6();
void presence_observer_request_restore7();
void presence_observer_request_restore8();
void presence_observer_gravity_restore0();
void presence_observer_gravity_restore1();
void presence_observer_gravity_restore2();
void presence_observer_gravity_restore3();
void presence_observer_gravity_restore4();
void presence_observer_gravity_restore5();
void presence_observer_gravity_restore6();
void presence_observer_gravity_restore7();
void presence_observer_gravity_restore8();
void* presence_originals[2];
volatile LONG presence_enabled=0,presence_count=0,presence_suppressed=0;
uint64_t presence_records[16][8]{};
volatile LONG presence_forwarded=0;
}
struct Result {std::string name; bool pass;};
std::vector<Result> results;
void check(bool ok,const std::string& name){results.push_back({name,ok}); std::printf("%s %s\n",ok?"PASS":"FAIL",name.c_str());}
void must(bool ok,const std::string& name){check(ok,name);if(!ok)throw std::runtime_error("fixture prerequisite failed; dependent tests not executed");}
struct Snapshot { uint64_t g[11],ret[11],nv[8]; float vec[128],rv[128];};
Snapshot invoke(void* target){ws_test_invoke_probe(target);Snapshot s{}; memcpy(s.g,ws_test_gprs,sizeof s.g);memcpy(s.ret,ws_test_return_gprs,sizeof s.ret);memcpy(s.nv,ws_test_nonvolatile,sizeof s.nv);memcpy(s.vec,ws_test_vectors,sizeof s.vec);memcpy(s.rv,ws_test_returns,sizeof s.rv);return s;}
uint64_t qword(const void* p){uint64_t n;memcpy(&n,p,8);return n;}
void transfer(void* target,void* observer,void* original,unsigned stolen,const char* name){
 auto t=static_cast<unsigned char*>(target);auto b=static_cast<unsigned char*>(original);
 must(t[0]==0xe9,std::string(name)+" entry normal rel32 patch");
 int32_t displacement;memcpy(&displacement,t+1,4);auto relay=t+5+displacement;
 check(relay[0]==0xff&&relay[1]==0x25&&qword(relay+6)==reinterpret_cast<uint64_t>(observer),std::string(name)+" real relay absolute destination");
 check(qword(relay+2)%0x100000000ull==0,std::string(name)+" relay zero indirect displacement");
 check(b[stolen]==0xff&&b[stolen+1]==0x25&&qword(b+stolen+6)==reinterpret_cast<uint64_t>(t+stolen),std::string(name)+" real trampoline continuation destination");
 check(qword(b+stolen+2)%0x100000000ull==0,std::string(name)+" continuation zero indirect displacement");
 unsigned decoded=0;while(decoded<5){hde64s h{};unsigned n=hde64_disasm(b+decoded,&h);if(!n||(h.flags&F_ERROR))break;decoded+=n;}
 check(decoded==stolen,std::string(name)+" actual HDE whole-instruction span");
 // The only other observer transfer is a RIP-relative indirect tail to its original slot.
 auto o=static_cast<unsigned char*>(observer);bool found=false;
 for(unsigned i=0;i<240;i++)if(o[i]==0xff&&o[i+1]==0x25){int32_t d;memcpy(&d,o+i+2,4);auto slot=o+i+6+d;found=qword(slot)==reinterpret_cast<uint64_t>(original);break;}
 check(found,std::string(name)+" observer tail forwards actual trampoline");
}
// Test the actual stack-walker at table-less MinHook regions; do not assume native success.
// Fabricated OWNED stack/context: validates Windows API semantics, not asynchronous
// game sampling, exception dispatch, or saved-context lifetime.
struct CallbackEvent { unsigned kind; uint64_t pc,base,value; };
struct VirtualTrace {std::string name;bool available;uint64_t base,entry,pc,sp,expectedPc,expectedSp,rbx,rbp,rsi,rdi;};
std::vector<VirtualTrace> virtuals;
struct WalkStep { BOOL ok; DWORD error; uint64_t pc,sp,fp,ret,contextPc,contextSp; };
struct WalkTrace {
 std::string sample,variant;
 uint64_t pc,sp,fp,contextRbp,expectedPc,expectedSp,stackLow,stackHigh,tebLow,tebHigh;
 DWORD contextFlags; WORD cs,ss; DWORD flags;
 bool reached=false; unsigned dropped=0;
 std::vector<WalkStep> steps;std::vector<CallbackEvent> callbacks;
};
std::vector<WalkTrace> walks;
WalkTrace* activeWalk=nullptr;
void event(unsigned kind,uint64_t pc,uint64_t base,uint64_t value){
 if(!activeWalk)return;
 if(activeWalk->callbacks.size()<24)activeWalk->callbacks.push_back({kind,pc,base,value});
 else activeWalk->dropped++;
}
PVOID CALLBACK lookup(HANDLE,DWORD64 pc){DWORD64 base=0;auto fn=RtlLookupFunctionEntry(pc,&base,nullptr);event(1,pc,base,reinterpret_cast<uint64_t>(fn));return fn;}
DWORD64 CALLBACK module(HANDLE,DWORD64 pc){DWORD64 base=0;RtlLookupFunctionEntry(pc,&base,nullptr);if(!base){MEMORY_BASIC_INFORMATION info{};if(VirtualQuery(reinterpret_cast<void*>(pc),&info,sizeof info))base=reinterpret_cast<DWORD64>(info.AllocationBase);}event(2,pc,base,0);return base;}
WalkTrace walk(CONTEXT c,uint64_t initialFrame,uint64_t expectedPc,uint64_t expectedSp,
 uint64_t low,uint64_t high,const char* name,const char* variant){
 STACKFRAME64 f{};f.AddrPC.Offset=c.Rip;f.AddrStack.Offset=c.Rsp;f.AddrFrame.Offset=initialFrame;
 f.AddrPC.Mode=f.AddrStack.Mode=f.AddrFrame.Mode=AddrModeFlat;
 ULONG_PTR stackLow=0,stackHigh=0;GetCurrentThreadStackLimits(&stackLow,&stackHigh);
 WalkTrace trace{};trace.sample=name;trace.variant=variant;
 trace.pc=c.Rip;trace.sp=c.Rsp;trace.fp=initialFrame;trace.contextRbp=c.Rbp;trace.expectedPc=expectedPc;trace.expectedSp=expectedSp;
 trace.stackLow=low;trace.stackHigh=high;trace.tebLow=stackLow;trace.tebHigh=stackHigh;
 trace.contextFlags=c.ContextFlags;trace.cs=c.SegCs;trace.ss=c.SegSs;trace.flags=c.EFlags;
 activeWalk=&trace;
 for(int i=0;i<3;i++){
  // API generally does not set last-error: clear stale state and retain value
  // only as an observation, never infer a failure reason from zero or old errno.
  SetLastError(ERROR_SUCCESS);
  BOOL ok=StackWalk64(IMAGE_FILE_MACHINE_AMD64,GetCurrentProcess(),GetCurrentThread(),&f,&c,nullptr,lookup,module,nullptr);
  DWORD error=GetLastError();
  trace.steps.push_back({ok,error,f.AddrPC.Offset,f.AddrStack.Offset,f.AddrFrame.Offset,f.AddrReturn.Offset,c.Rip,c.Rsp});
  if(!ok)break;
  if(f.AddrPC.Offset==expectedPc){trace.reached=f.AddrStack.Offset==expectedSp;break;}
 }
 activeWalk=nullptr;return trace;
}
void writeDbgHelpVersion(FILE* out){
 wchar_t path[MAX_PATH]{};bool known=false;VS_FIXEDFILEINFO version{};DWORD ignored=0;
 DWORD length=GetModuleFileNameW(GetModuleHandleW(L"dbghelp.dll"),path,MAX_PATH);
 DWORD bytes=length&&length<MAX_PATH?GetFileVersionInfoSizeW(path,&ignored):0;
 if(bytes&&bytes<=1024*1024){std::vector<unsigned char> data(bytes);PVOID value=nullptr;UINT size=0;
  if(GetFileVersionInfoW(path,0,bytes,data.data())&&VerQueryValueW(data.data(),L"\\",&value,&size)&&size>=sizeof version){memcpy(&version,value,sizeof version);known=version.dwSignature==0xfeef04bd;}}
 std::fprintf(out,",\"dbghelp_file_version\":{\"known\":%s,\"major\":%u,\"minor\":%u,\"build\":%u,\"revision\":%u}",known?"true":"false",HIWORD(version.dwFileVersionMS),LOWORD(version.dwFileVersionMS),HIWORD(version.dwFileVersionLS),LOWORD(version.dwFileVersionLS));
}
void writeWalks(FILE* out){
 std::fputs(",\"stackwalk_diagnostics\":[",out);
 for(size_t i=0;i<walks.size();i++){
  auto& w=walks[i];
  std::fprintf(out,"%s{\"sample\":\"%s\",\"variant\":\"%s\",\"initial_pc\":%llu,\"initial_sp\":%llu,\"initial_fp\":%llu,\"context_rbp\":%llu,\"expected_pc\":%llu,\"expected_sp\":%llu,\"stack_low\":%llu,\"stack_high\":%llu,\"teb_low\":%llu,\"teb_high\":%llu,\"context_flags\":%lu,\"cs\":%u,\"ss\":%u,\"eflags\":%lu,\"predicate_pass\":%s,\"callback_dropped\":%u,\"steps\":[",i?",":"",w.sample.c_str(),w.variant.c_str(),w.pc,w.sp,w.fp,w.contextRbp,w.expectedPc,w.expectedSp,w.stackLow,w.stackHigh,w.tebLow,w.tebHigh,w.contextFlags,w.cs,w.ss,w.flags,w.reached?"true":"false",w.dropped);
  for(size_t k=0;k<w.steps.size();k++){auto& x=w.steps[k];std::fprintf(out,"%s{\"ok\":%s,\"last_error_after_clear\":%lu,\"pc\":%llu,\"sp\":%llu,\"fp\":%llu,\"return_pc\":%llu,\"context_pc\":%llu,\"context_sp\":%llu}",k?",":"",x.ok?"true":"false",x.error,x.pc,x.sp,x.fp,x.ret,x.contextPc,x.contextSp);}
  std::fputs("],\"callbacks\":[",out);
  for(size_t k=0;k<w.callbacks.size();k++){auto& x=w.callbacks[k];std::fprintf(out,"%s{\"kind\":%u,\"pc\":%llu,\"base\":%llu,\"function_entry\":%llu}",k?",":"",x.kind,x.pc,x.base,x.value);}
  std::fputs("]}",out);
 }
 std::fputs("],\"virtual_unwind_diagnostics\":[",out);
 for(size_t i=0;i<virtuals.size();i++){auto& v=virtuals[i];std::fprintf(out,"%s{\"sample\":\"%s\",\"available\":%s,\"base\":%llu,\"function_entry\":%llu,\"pc\":%llu,\"sp\":%llu,\"expected_pc\":%llu,\"expected_sp\":%llu,\"rbx\":%llu,\"rbp\":%llu,\"rsi\":%llu,\"rdi\":%llu}",i?",":"",v.name.c_str(),v.available?"true":"false",v.base,v.entry,v.pc,v.sp,v.expectedPc,v.expectedSp,v.rbx,v.rbp,v.rsi,v.rdi);}
 std::fputs("]",out);
}
void unwind(void* pc,unsigned frame,bool saved,bool pushed,const char* name,bool gravity=false,bool table=true){
 alignas(16) uint64_t stack[512]{}; auto entry=reinterpret_cast<uint64_t>(&stack[384]);
 // x64 entry stack must be 8 mod16.
 entry+=8;
 const uint64_t ret=reinterpret_cast<uint64_t>(&presence_unwind_return);
 *reinterpret_cast<uint64_t*>(entry)=ret;
 // Caller allocated40 bytes before CALL: caller RSP=entry+8; its outer
 // return slot is entry+48. Zero terminates further synthetic walking.
 *reinterpret_cast<uint64_t*>(entry+48)=0;
 *reinterpret_cast<uint64_t*>(entry+8)=0x1111;
 *reinterpret_cast<uint64_t*>(entry+16)=gravity?0x3333:0x2222;
 *reinterpret_cast<uint64_t*>(entry+24)=0x3333;
 *reinterpret_cast<uint64_t*>(entry-8)=0x4444;
 CONTEXT c{};c.ContextFlags=CONTEXT_FULL;c.Rip=reinterpret_cast<uint64_t>(pc);c.Rsp=entry-frame;
 c.Rbx=saved?0xdead:0x1111;c.Rbp=saved&&!gravity?0xbeef:0x2222;c.Rsi=saved?0xfeed:0x3333;c.Rdi=pushed?0xaaaa:0x4444;
 // Corrected primary is exactly the v2 native55/55 genuine-return path.
 auto primary=walk(c,c.Rsp,ret,entry+8,reinterpret_cast<uint64_t>(stack),reinterpret_cast<uint64_t>(stack+512),name,"primary_valid_call_return");
 bool reached=primary.reached;walks.push_back(primary);
 // Preserve the old invalid sentinel only as a separately labeled diagnostic.
 // Its failure/success cannot change the strict valid-caller assertion.
 auto invalidEntry=reinterpret_cast<uint64_t>(&ws_test_invoke_probe);
 *reinterpret_cast<uint64_t*>(entry)=invalidEntry;
 walks.push_back(walk(c,c.Rsp,invalidEntry,entry+8,reinterpret_cast<uint64_t>(stack),reinterpret_cast<uint64_t>(stack+512),name,"invalid_function_entry_sentinel_diagnostic"));
 *reinterpret_cast<uint64_t*>(entry)=ret;
 check(reached,std::string(name)+" StackWalk64 caller");
 // Table-backed frames additionally verify register restoration with VirtualUnwind.
 DWORD64 base=0;auto fn=RtlLookupFunctionEntry(reinterpret_cast<DWORD64>(pc),&base,nullptr);
 check((fn!=nullptr)==table,std::string(name)+" actual function-table presence");
 if(fn){CONTEXT v{};v.ContextFlags=CONTEXT_FULL;v.Rip=reinterpret_cast<uint64_t>(pc);v.Rsp=entry-frame;v.Rbx=saved?0xdead:0x1111;v.Rbp=saved&&!gravity?0xbeef:0x2222;v.Rsi=saved?0xfeed:0x3333;v.Rdi=pushed?0xaaaa:0x4444;
 PVOID data=nullptr;DWORD64 estab=0;RtlVirtualUnwind(UNW_FLAG_NHANDLER,base,v.Rip,fn,&v,&data,&estab,nullptr);
 virtuals.push_back({name,true,base,reinterpret_cast<uint64_t>(fn),v.Rip,v.Rsp,ret,entry+8,v.Rbx,v.Rbp,v.Rsi,v.Rdi});
 check(v.Rip==ret&&v.Rsp==entry+8&&v.Rbx==0x1111&&v.Rbp==0x2222&&v.Rsi==0x3333&&v.Rdi==0x4444,std::string(name)+" VirtualUnwind stack/nonvolatiles");}
 else virtuals.push_back({name,false,base,0,0,0,ret,entry+8,0,0,0,0});
}
int wmain(int argc,wchar_t** argv){
 bool wine=GetProcAddress(GetModuleHandleA("ntdll.dll"),"wine_get_version")!=nullptr;
 if(!__builtin_cpu_supports("avx")){std::fprintf(stderr,"UNAVAILABLE AVX/OSXSAVE required\n");return 2;}
 try {
 must(SymInitialize(GetCurrentProcess(),nullptr,TRUE)!=FALSE,"DbgHelp initialize");
 // Validate that the sentinel is a real return PC inside the owned caller,
 // not merely a nominal symbol. Exercise that actual CALL/return as a control.
 DWORD64 callerBase=0;auto callerReturn=reinterpret_cast<unsigned char*>(&presence_unwind_return);
 auto callerFn=RtlLookupFunctionEntry(reinterpret_cast<DWORD64>(callerReturn),&callerBase,nullptr);
 must(callerFn&&callerBase+callerFn->BeginAddress==reinterpret_cast<DWORD64>(&presence_unwind_caller)&&callerBase+callerFn->EndAddress>reinterpret_cast<DWORD64>(callerReturn),"valid caller return inside exact owned runtime-function range");
 // Use the runtime-table address, avoiding C++ object arithmetic across
 // separately declared assembly labels (which the optimizer may treat as distinct functions).
 auto callSite=reinterpret_cast<unsigned char*>(callerBase+callerFn->BeginAddress+4);
 int32_t callDisplacement=0;memcpy(&callDisplacement,callSite+1,4);
 must(callSite[0]==0xe8&&reinterpret_cast<uint64_t>(callSite)+5==reinterpret_cast<uint64_t>(callerReturn)&&reinterpret_cast<uint64_t>(callerReturn)+callDisplacement==reinterpret_cast<uint64_t>(&presence_race_target),"valid caller actual direct CALL ends exactly at return label");
 auto callerUnwind=reinterpret_cast<const unsigned char*>(callerBase+callerFn->UnwindData);
 must(callerUnwind[0]==1&&callerUnwind[1]==4&&callerUnwind[2]==1&&callerUnwind[3]==0&&callerUnwind[4]==4&&callerUnwind[5]==0x42,"valid caller unwind describes real40byte allocation");
 LONG beforeCaller=presence_forwarded;
 must(presence_unwind_caller()==0x77aabbcc11223344ull&&presence_forwarded==beforeCaller+1,"valid caller executes actual CALL return exactly once");
 for(unsigned i=0;i<128;i++)ws_test_vector_input[i]=static_cast<float>(i*3+1);
 void* targets[]={reinterpret_cast<void*>(&presence_request),reinterpret_cast<void*>(&presence_gravity)};
 void* observers[]={reinterpret_cast<void*>(&presence_observer_request),reinterpret_cast<void*>(&presence_observer_gravity)};
 void* restores[2][9]={{reinterpret_cast<void*>(&presence_observer_request_restore0),reinterpret_cast<void*>(&presence_observer_request_restore1),reinterpret_cast<void*>(&presence_observer_request_restore2),reinterpret_cast<void*>(&presence_observer_request_restore3),reinterpret_cast<void*>(&presence_observer_request_restore4),reinterpret_cast<void*>(&presence_observer_request_restore5),reinterpret_cast<void*>(&presence_observer_request_restore6),reinterpret_cast<void*>(&presence_observer_request_restore7),reinterpret_cast<void*>(&presence_observer_request_restore8)},{reinterpret_cast<void*>(&presence_observer_gravity_restore0),reinterpret_cast<void*>(&presence_observer_gravity_restore1),reinterpret_cast<void*>(&presence_observer_gravity_restore2),reinterpret_cast<void*>(&presence_observer_gravity_restore3),reinterpret_cast<void*>(&presence_observer_gravity_restore4),reinterpret_cast<void*>(&presence_observer_gravity_restore5),reinterpret_cast<void*>(&presence_observer_gravity_restore6),reinterpret_cast<void*>(&presence_observer_gravity_restore7),reinterpret_cast<void*>(&presence_observer_gravity_restore8)}};
 unsigned spans[]={7,5};std::array<std::array<unsigned char,7>,2> prefix{};
 Snapshot baseline[2];
 for(unsigned i=0;i<2;i++){memcpy(prefix[i].data(),targets[i],spans[i]);baseline[i]=invoke(targets[i]);}
 // True unhooked controls execute before even initializing MinHook.
 for(unsigned off:{0u,3u,7u,11u,15u,16u,23u})unwind(static_cast<unsigned char*>(targets[0])+off,off>=23?1128:off>=16?8:0,off>=23,off>=16,("request original+"+std::to_string(off)).c_str());
 for(unsigned off:{0u,5u,10u,11u,15u})unwind(static_cast<unsigned char*>(targets[1])+off,off>=15?56:off>=11?8:0,off>=15,off>=11,("gravity original+"+std::to_string(off)).c_str(),true);
 must(MH_Initialize()==MH_OK,"real MinHook initialize");
 for(unsigned i=0;i<2;i++){must(MH_CreateHook(targets[i],observers[i],&presence_originals[i])==MH_OK,"real MinHook create "+std::to_string(i));check(memcmp(prefix[i].data(),presence_originals[i],spans[i])==0,"whole actual stolen span "+std::to_string(i));must(MH_EnableHook(targets[i])==MH_OK,"real MinHook enable "+std::to_string(i));transfer(targets[i],observers[i],presence_originals[i],spans[i],i?"gravity":"request");}
 for(unsigned i=0;i<2;i++){
 DWORD64 base=0;auto fn=RtlLookupFunctionEntry(reinterpret_cast<DWORD64>(targets[i]),&base,nullptr);
 auto u=fn?reinterpret_cast<const unsigned char*>(base+fn->UnwindData):nullptr;
 check(u&&u[0]==1&&u[1]==(i?15:23)&&u[2]==(i?6:9)&&u[3]==0,"equivalent original unwind version/prologue/code slots/frame "+std::to_string(i));
 }
 presence_enabled=1;
 for(unsigned i=0;i<2;i++){auto hooked=invoke(targets[i]);check(memcmp(&baseline[i],&hooked,sizeof hooked)==0,"original vs hooked GPR/flags/YMM0-15/return/stack/nonvolatile "+std::to_string(i));}
 check(presence_count==2,"opaque records reserved for both sites");
 check(presence_records[0][0]==0x1122334455667788ull&&presence_records[0][1]==0x2233445566778899ull&&presence_records[0][5]==GetCurrentThreadId(),"opaque RCX RDX and thread identity, no dereference");
 for(unsigned i=0;i<40;i++)invoke(targets[0]);
 check(presence_count==16&&presence_suppressed==26,"bounded16 records and explicit suppressed count");
 presence_enabled=0;invoke(targets[0]);check(presence_count==16,"disabled observer forwards without reservation");
 // Unwind original sites only after disable restores their real instruction bytes.
 for(unsigned i=0;i<2;i++)check(MH_DisableHook(targets[i])==MH_OK,"disable owned hook "+std::to_string(i));
 for(unsigned i=0;i<2;i++){
  // Entry is currently restored: obtain relay location from trampoline layout
  // (vendored MinHook appends relay immediately after continuation14).
  auto relay=static_cast<unsigned char*>(presence_originals[i])+spans[i]+14;
  unwind(relay,0,false,false,("actual relay "+std::to_string(i)).c_str(),false,false);
  if(i==0)unwind(static_cast<unsigned char*>(presence_originals[i])+3,0,false,false,"request trampoline after R11 assignment",false,false);
  unwind(presence_originals[i],0,false,false,("trampoline start "+std::to_string(i)).c_str(),false,false);
  unwind(static_cast<unsigned char*>(presence_originals[i])+spans[i],0,false,false,("trampoline absolute continuation "+std::to_string(i)).c_str(),false,false);
  // Observer push lengths: 1,1,1,1,2,2,2,2 => cumulative 1/2/3/4/6/8/10/12.
  unsigned offsets[]={0,1,2,3,4,6,8,10,12};
  for(unsigned k=0;k<9;k++)unwind(static_cast<unsigned char*>(observers[i])+offsets[k],k*8,false,false,("observer "+std::to_string(i)+"+"+std::to_string(offsets[k])).c_str());
 }
 for(unsigned i=0;i<2;i++)for(unsigned k=0;k<9;k++)unwind(restores[i][k],64-k*8,false,false,("observer restore "+std::to_string(i)+" step"+std::to_string(k)).c_str());
 // Stop policy: disabled, retained mappings; do NOT pretend removal proves saved-CONTEXT retirement.
 for(unsigned i=0;i<2;i++){auto after=invoke(targets[i]);check(memcmp(&baseline[i],&after,sizeof after)==0,"post-disable exact forwarding "+std::to_string(i));}
 // A separate leaf oracle exposes entry R11 and flags that native SUB/MOV
 // prologues intentionally overwrite; compare all 256-bit lanes as well.
 auto leaf=reinterpret_cast<void*>(&ws_test_probe_target);
 auto leafBaseline=invoke(leaf);void* leafTrampoline=nullptr;
 must(MH_CreateHook(leaf,observers[0],&leafTrampoline)==MH_OK,"leaf ABI oracle create actual hook");
 auto retained=presence_originals[0];presence_originals[0]=leafTrampoline;
 must(MH_EnableHook(leaf)==MH_OK,"leaf ABI oracle enable");
 presence_enabled=1;presence_count=0;presence_suppressed=0;
 auto leafHooked=invoke(leaf);
 check(memcmp(&leafBaseline,&leafHooked,sizeof leafHooked)==0,"leaf entry flags R11 and full ABI transparency");
 check(MH_DisableHook(leaf)==MH_OK,"leaf oracle disable; retain mapping");
 presence_enabled=0;presence_originals[0]=reinterpret_cast<void*>(&presence_race_target);
 presence_count=0;presence_suppressed=0;presence_forwarded=0;
 check(presence_count==0,"armed independent zero-hit fixture starts empty, no WR lease");
 presence_enabled=1;
 std::vector<std::thread> threads;
 for(unsigned i=0;i<8;i++)threads.emplace_back([]{for(unsigned n=0;n<100;n++)presence_observer_request();});
 for(auto& t:threads)t.join();
 presence_enabled=0;
 check(presence_count==16&&presence_suppressed==784,"8-thread bounded reservations plus losses account for all800 calls");
 check(presence_forwarded==800,"all race calls forward exactly once");
 bool recordsValid=true;for(unsigned i=0;i<16;i++)recordsValid&=presence_records[i][5]!=0&&presence_records[i][6]!=0;
 check(recordsValid,"joined writers published all reserved records with thread/time");
 presence_count=0;presence_suppressed=0;presence_forwarded=0;presence_enabled=1;
 threads.clear();
 for(unsigned i=0;i<8;i++)threads.emplace_back([]{for(unsigned n=0;n<1000;n++)presence_observer_request();});
 InterlockedExchange(&presence_enabled,0);
 for(auto& t:threads)t.join();
 check(presence_forwarded==8000&&presence_count<=16&&presence_count+presence_suppressed<=8000,"concurrent close: all8000 forwarded, bounded reservations, mappings retained");
 presence_originals[0]=retained;
 // Retaining code and hook mappings avoids asserting saved-context reclamation.
 }catch(const std::exception& e){check(false,e.what());}
 unsigned pass=0,fail=0;for(auto& r:results)(r.pass?pass:fail)++;
 FILE* out=argc>1?_wfopen(argv[1],L"wb"):nullptr;
 if(out){std::fprintf(out,"{\n\"schema\":\"WS-owned-presence-fixture-v1\",\"environment\":\"%s\",\"production_admitted\":false,\"pass\":%u,\"fail\":%u,\"native_windows_unavailable\":%u,\"assertions\":[",wine?"Wine":"native Windows",pass,fail,wine?1:0);for(size_t i=0;i<results.size();i++)std::fprintf(out,"%s{\"name\":\"%s\",\"pass\":%s}",i?",":"",results[i].name.c_str(),results[i].pass?"true":"false");std::fputs("]",out);writeWalks(out);writeDbgHelpVersion(out);
 API_VERSION requested{API_VERSION_NUMBER,0,0,0},version{};auto actual=ImagehlpApiVersionEx(&requested);if(actual)version=*actual;
 std::fprintf(out,",\"dbghelp_api\":{\"major\":%u,\"minor\":%u,\"revision\":%u},\"diagnostic_revision\":3}\n",version.MajorVersion,version.MinorVersion,version.Revision);std::fclose(out);}else{std::fprintf(stderr,"output JSON path required/writable\n");return 2;}
 std::printf("SUMMARY environment=%s PASS=%u FAIL=%u native-Windows-UNAVAILABLE=%u production_admitted=false\n",wine?"Wine":"native Windows",pass,fail,wine?1:0);return fail?1:0;
}
