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
// Windows' actual stack-walker handles table-less MinHook regions as leaf frames.
// Fabricated OWNED stack/context: validates Windows API semantics, not asynchronous
// game sampling, exception dispatch, or saved-context lifetime.
PVOID CALLBACK lookup(HANDLE,DWORD64 pc){DWORD64 base;return RtlLookupFunctionEntry(pc,&base,nullptr);}
DWORD64 CALLBACK module(HANDLE,DWORD64 pc){DWORD64 base=0;RtlLookupFunctionEntry(pc,&base,nullptr);if(base)return base;MEMORY_BASIC_INFORMATION info{};return VirtualQuery(reinterpret_cast<void*>(pc),&info,sizeof info)?reinterpret_cast<DWORD64>(info.AllocationBase):0;}
void unwind(void* pc,unsigned frame,bool saved,bool pushed,const char* name,bool gravity=false,bool table=true){
 alignas(16) uint64_t stack[512]{}; auto entry=reinterpret_cast<uint64_t>(&stack[384]);
 // x64 entry stack must be 8 mod16.
 entry+=8;
 const uint64_t ret=reinterpret_cast<uint64_t>(&ws_test_invoke_probe);
 *reinterpret_cast<uint64_t*>(entry)=ret;
 *reinterpret_cast<uint64_t*>(entry+8)=0x1111;
 *reinterpret_cast<uint64_t*>(entry+16)=gravity?0x3333:0x2222;
 *reinterpret_cast<uint64_t*>(entry+24)=0x3333;
 *reinterpret_cast<uint64_t*>(entry-8)=0x4444;
 CONTEXT c{};c.ContextFlags=CONTEXT_FULL;c.Rip=reinterpret_cast<uint64_t>(pc);c.Rsp=entry-frame;
 c.Rbx=saved?0xdead:0x1111;c.Rbp=saved&&!gravity?0xbeef:0x2222;c.Rsi=saved?0xfeed:0x3333;c.Rdi=pushed?0xaaaa:0x4444;
 STACKFRAME64 f{};f.AddrPC.Offset=c.Rip;f.AddrStack.Offset=c.Rsp;f.AddrFrame.Offset=c.Rsp;
 f.AddrPC.Mode=f.AddrStack.Mode=f.AddrFrame.Mode=AddrModeFlat;
 // First call initializes to current frame; subsequent call must reach owned caller.
 bool reached=false;for(int i=0;i<3;i++){
  if(!StackWalk64(IMAGE_FILE_MACHINE_AMD64,GetCurrentProcess(),GetCurrentThread(),&f,&c,nullptr,lookup,module,nullptr))break;
  if(f.AddrPC.Offset==ret){reached=f.AddrStack.Offset==entry+8;break;}
 }
 check(reached,std::string(name)+" StackWalk64 caller");
 // Table-backed frames additionally verify register restoration with VirtualUnwind.
 DWORD64 base=0;auto fn=RtlLookupFunctionEntry(reinterpret_cast<DWORD64>(pc),&base,nullptr);
 check((fn!=nullptr)==table,std::string(name)+" actual function-table presence");
 if(fn){CONTEXT v{};v.ContextFlags=CONTEXT_FULL;v.Rip=reinterpret_cast<uint64_t>(pc);v.Rsp=entry-frame;v.Rbx=saved?0xdead:0x1111;v.Rbp=saved&&!gravity?0xbeef:0x2222;v.Rsi=saved?0xfeed:0x3333;v.Rdi=pushed?0xaaaa:0x4444;
 PVOID data=nullptr;DWORD64 estab=0;RtlVirtualUnwind(UNW_FLAG_NHANDLER,base,v.Rip,fn,&v,&data,&estab,nullptr);
 check(v.Rip==ret&&v.Rsp==entry+8&&v.Rbx==0x1111&&v.Rbp==0x2222&&v.Rsi==0x3333&&v.Rdi==0x4444,std::string(name)+" VirtualUnwind stack/nonvolatiles");}
}
int wmain(int argc,wchar_t** argv){
 bool wine=GetProcAddress(GetModuleHandleA("ntdll.dll"),"wine_get_version")!=nullptr;
 if(!__builtin_cpu_supports("avx")){std::fprintf(stderr,"UNAVAILABLE AVX/OSXSAVE required\n");return 2;}
 try {
 must(SymInitialize(GetCurrentProcess(),nullptr,TRUE)!=FALSE,"DbgHelp initialize");
 for(unsigned i=0;i<128;i++)ws_test_vector_input[i]=static_cast<float>(i*3+1);
 void* targets[]={reinterpret_cast<void*>(&presence_request),reinterpret_cast<void*>(&presence_gravity)};
 void* observers[]={reinterpret_cast<void*>(&presence_observer_request),reinterpret_cast<void*>(&presence_observer_gravity)};
 void* restores[2][9]={{reinterpret_cast<void*>(&presence_observer_request_restore0),reinterpret_cast<void*>(&presence_observer_request_restore1),reinterpret_cast<void*>(&presence_observer_request_restore2),reinterpret_cast<void*>(&presence_observer_request_restore3),reinterpret_cast<void*>(&presence_observer_request_restore4),reinterpret_cast<void*>(&presence_observer_request_restore5),reinterpret_cast<void*>(&presence_observer_request_restore6),reinterpret_cast<void*>(&presence_observer_request_restore7),reinterpret_cast<void*>(&presence_observer_request_restore8)},{reinterpret_cast<void*>(&presence_observer_gravity_restore0),reinterpret_cast<void*>(&presence_observer_gravity_restore1),reinterpret_cast<void*>(&presence_observer_gravity_restore2),reinterpret_cast<void*>(&presence_observer_gravity_restore3),reinterpret_cast<void*>(&presence_observer_gravity_restore4),reinterpret_cast<void*>(&presence_observer_gravity_restore5),reinterpret_cast<void*>(&presence_observer_gravity_restore6),reinterpret_cast<void*>(&presence_observer_gravity_restore7),reinterpret_cast<void*>(&presence_observer_gravity_restore8)}};
 unsigned spans[]={7,5};std::array<std::array<unsigned char,7>,2> prefix{};
 Snapshot baseline[2];
 for(unsigned i=0;i<2;i++){memcpy(prefix[i].data(),targets[i],spans[i]);baseline[i]=invoke(targets[i]);}
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
 for(unsigned off:{0u,3u,7u,11u,15u,16u,23u})unwind(static_cast<unsigned char*>(targets[0])+off,off>=23?1128:off>=16?8:0,off>=23,off>=16,("request original+"+std::to_string(off)).c_str());
 for(unsigned off:{0u,5u,10u,11u,15u})unwind(static_cast<unsigned char*>(targets[1])+off,off>=15?56:off>=11?8:0,off>=15,off>=11,("gravity original+"+std::to_string(off)).c_str(),true);
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
 if(out){std::fprintf(out,"{\n\"schema\":\"WS-owned-presence-fixture-v1\",\"environment\":\"%s\",\"production_admitted\":false,\"pass\":%u,\"fail\":%u,\"native_windows_unavailable\":%u,\"assertions\":[",wine?"Wine":"native Windows",pass,fail,wine?1:0);for(size_t i=0;i<results.size();i++)std::fprintf(out,"%s{\"name\":\"%s\",\"pass\":%s}",i?",":"",results[i].name.c_str(),results[i].pass?"true":"false");std::fputs("]}\n",out);std::fclose(out);}else{std::fprintf(stderr,"output JSON path required/writable\n");return 2;}
 std::printf("SUMMARY environment=%s PASS=%u FAIL=%u native-Windows-UNAVAILABLE=%u production_admitted=false\n",wine?"Wine":"native Windows",pass,fail,wine?1:0);return fail?1:0;
}
