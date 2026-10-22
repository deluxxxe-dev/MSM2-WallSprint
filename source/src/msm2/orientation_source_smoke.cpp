// TEST ONLY: real MinHook entry capture on owned synthetic functions/allocations.
#include "orientation_source.cpp"
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <immintrin.h>
#include <fstream>
static std::string output;static msm2::Snapshot fake;static unsigned nativeCalls{};
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&got)&&got==n;}
bool sample(Snapshot& s){s=fake;return s.hero&&readMemory(s.transform,s.prefix,48);}
void writeLog(const char* f,...){char b[4096];va_list a;va_start(a,f);vsnprintf(b,sizeof(b),f,a);va_end(a);output+=b;output+='\n';}
}
extern "C" {
alignas(32) float ws_test_vector_input[128],ws_test_vectors[128];
std::uint64_t ws_test_gprs[11],ws_test_result;float ws_test_returns[128];std::uint64_t ws_test_return_gprs[11],ws_test_nonvolatile[8];
void ws_test_probe_target();void ws_test_invoke_probe(void*);
}
__attribute__((noinline,noclone)) std::uint64_t nativeCopy(void* dst,const void* source,std::uint64_t c,std::uint64_t d,std::uint64_t e,std::uint64_t f){
 ++nativeCalls;std::memcpy(dst,source,64);return (reinterpret_cast<std::uintptr_t>(dst)^reinterpret_cast<std::uintptr_t>(source))+3*c+5*d+7*e+11*f;
}
static void hook(void* t){target=reinterpret_cast<std::uintptr_t>(t);assert(MH_CreateHook(t,reinterpret_cast<void*>(&ws_source_thunk),&ws_source_original)==MH_OK);assert(MH_EnableHook(t)==MH_OK);installed=created=true;assert(msm2::readMemory(target,owned,32));killed=false;admitted=true;}
static void unhook(){msm2::input::stop();assert(!installed);assert(MH_RemoveHook(reinterpret_cast<void*>(target))==MH_OK);created=false;killed=false;}
int main(){
 auto image=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(image+dos->e_lfanew);
 msm2::input::configure({},false,image,nt->OptionalHeader.SizeOfImage);assert(!admitted&&!created);
 // Genuine body bytes cannot be authenticated at a synthetic executable/RVA.
 msm2::input::configure({},true,image,0x1000);assert(!admitted&&!created);
 // A registered synthetic image containing the exact captured writer bytes.
 // NEVER execute these game instructions; exercise the real installer/guards only.
 if(std::ifstream("tools/fixtures/orientation_v01b3/writer.hex").good()){
 const unsigned imageBytes=unsigned(writerRva+0x1000);auto* testImage=static_cast<unsigned char*>(VirtualAlloc(nullptr,imageBytes,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE));assert(testImage);
 auto syntheticBase=reinterpret_cast<std::uintptr_t>(testImage);auto* td=reinterpret_cast<IMAGE_DOS_HEADER*>(testImage);td->e_magic=IMAGE_DOS_SIGNATURE;td->e_lfanew=0x100;
 auto* tn=reinterpret_cast<IMAGE_NT_HEADERS64*>(testImage+0x100);tn->Signature=IMAGE_NT_SIGNATURE;tn->FileHeader.NumberOfSections=1;tn->FileHeader.SizeOfOptionalHeader=sizeof(IMAGE_OPTIONAL_HEADER64);tn->OptionalHeader.SizeOfImage=imageBytes;
 auto* section=reinterpret_cast<IMAGE_SECTION_HEADER*>(testImage+0x100+24+sizeof(IMAGE_OPTIONAL_HEADER64));section->VirtualAddress=writerRva;section->Misc.VirtualSize=0x800;section->Characteristics=IMAGE_SCN_MEM_EXECUTE;
 std::ifstream input("tools/fixtures/orientation_v01b3/writer.hex");std::string fixture;input>>fixture;assert(fixture.size()==writerSize*2);for(unsigned i=0;i<writerSize;++i)testImage[writerRva+i]=static_cast<unsigned char>(std::stoul(fixture.substr(i*2,2),nullptr,16));
 RUNTIME_FUNCTION rf{};rf.BeginAddress=writerRva;rf.EndAddress=writerRva+writerSize;rf.UnwindData=0x800;testImage[0x800]=1;assert(RtlAddFunctionTable(&rf,1,syntheticBase));
 // Hash mismatch with valid prefix must not install.
 testImage[writerRva+100]^=1;msm2::input::configure({},true,syntheticBase,imageBytes);assert(!admitted&&!created);testImage[writerRva+100]^=1;
 // Duplicate exact AOB must not install, despite correct target/hash.
 std::memcpy(testImage+writerRva+0x400,prefix,32);msm2::input::configure({},true,syntheticBase,imageBytes);assert(!admitted&&!created);std::memset(testImage+writerRva+0x400,0,32);
 msm2::input::configure({},true,syntheticBase,imageBytes);assert(admitted&&created&&installed&&ownership()&&msm2::input::authenticated());testImage[writerRva+100]^=1;assert(!msm2::input::authenticated());testImage[writerRva+100]^=1;assert(msm2::input::authenticated());
 msm2::input::stop();assert(!installed&&!std::memcmp(testImage+writerRva,prefix,32));assert(MH_RemoveHook(reinterpret_cast<void*>(target))==MH_OK);created=false;assert(RtlDeleteFunctionTable(&rf));assert(VirtualFree(testImage,0,MEM_RELEASE));
 }else puts("SKIP: optional private game-body installer replay not redistributed; independent native tests continue");
 auto initialized=MH_Initialize();assert(initialized==MH_OK||initialized==MH_ERROR_ALREADY_INITIALIZED);
 base=image;imageSize=nt->OptionalHeader.SizeOfImage;
 auto* memory=static_cast<unsigned char*>(VirtualAlloc(nullptr,8192,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);auto* p=reinterpret_cast<float*>(memory);auto* src=reinterpret_cast<float*>(memory+256);p[0]=p[5]=p[10]=1;src[0]=src[5]=src[10]=src[15]=1;
 fake.hero=reinterpret_cast<std::uintptr_t>(memory+512);fake.transform=reinterpret_cast<std::uintptr_t>(p);std::memcpy(fake.prefix,p,48);cfg={};cfg.otherArguments=true;cfg.windowMs=100;cfg.maxEvents=256;hook(reinterpret_cast<void*>(&nativeCopy));
 volatile auto invoke=&nativeCopy;begin(fake,1,1,{0,0,1});auto expected=(reinterpret_cast<std::uintptr_t>(p)^reinterpret_cast<std::uintptr_t>(src))+3*11+5*22+7*33+11*44;
 auto mx=_mm_getcsr();unsigned short cw=0,after=0;__asm__ volatile("fnstcw %0; fld1":"=m"(cw));assert(invoke(p,src,11,22,33,44)==expected);double x87=0;__asm__ volatile("fstpl %0; fnstcw %1":"=m"(x87),"=m"(after));assert(x87==1&&after==cw&&_mm_getcsr()==mx);
 SetLastError(12345);assert(invoke(p,src,11,22,33,44)==expected);assert(GetLastError()==12345);
 assert(head==2&&windows[0].count==2);auto e=ring[0];assert(e.args[0]==fake.transform&&e.args[1]==reinterpret_cast<std::uintptr_t>(src)&&e.args[2]==11&&e.args[3]==22&&e.valid[1]&&e.valid[0]&&!e.valid[2]&&!e.valid[3]);assert(!std::memcmp(e.bytes[1],src,128));assert(e.ret&&e.thread==GetCurrentThreadId());msm2::input::drainLogs();
 unsigned char buf[128];assert(!readable(0x800000000000ULL,buf,128));assert(!readable(1,buf,128));assert(!readable(reinterpret_cast<std::uintptr_t>(src),buf,129));
 DWORD old=0;assert(VirtualProtect(memory+4096,4096,PAGE_NOACCESS,&old));assert(!readable(reinterpret_cast<std::uintptr_t>(memory+4096-64),buf,128));assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&old));
 // Hard cap: native original continues after record budget is exhausted.
 finish("test");cfg.maxEvents=5;begin(fake,1,2,{0,0,1});auto beforeCalls=nativeCalls;auto beforeHead=head.load();for(unsigned i=0;i<20;++i)assert(invoke(p,src,11,22,33,44)==expected);assert(nativeCalls-beforeCalls==20&&head-beforeHead==5);msm2::input::drainLogs();
 // Same callback sees source BEFORE native copies it. Crawl basis changes only in fixture.
 finish("test");cfg.maxEvents=256;src[0]=1;src[5]=0;src[6]=1;src[9]=-1;src[10]=0;begin(fake,2,2,{0,0,1});assert(invoke(p,src,11,22,33,44)==expected);assert(ring[(head-1)%ringCap].bytes[1][0]==reinterpret_cast<unsigned char*>(src)[0]);msm2::input::drainLogs();
 // Automatic scheduling uses actual proper snapshot + verified lifecycle proxy.
 finish("schedule_test");auto owner=reinterpret_cast<std::uintptr_t>(memory+1024);ws::Vec3 n{0,0,1};std::memcpy(reinterpret_cast<void*>(owner+0x398),&n,12);
 ws::observed::VerifiedWallContext ctx{};ctx.lifecycleVerified=ctx.wallRunActive=true;ctx.elapsed=1.;ctx.wallRunUpdateCalls=20;ctx.wallRunSessionId=10;ctx.wallRunOwner=owner;lastSample=0;
 msm2::input::tick(ctx,"none",true);assert(current.load()<windowsCap&&windows[current].state==1);
 ctx.wallRunActive=false;ctx.wallRunOwner=0;msm2::input::tick(ctx,"verified_deinit",true);
 for(unsigned i=0;i<4;++i){Sleep(22);lastSample=0;msm2::input::tick(ctx,"verified_deinit",true);}assert(current.load()<windowsCap&&windows[current].state==2&&lastSession==10);
 fake.hero+=16;Sleep(22);lastSample=0;msm2::input::tick(ctx,"verified_deinit",true);assert(current==windowsCap&&!deinit);fake.hero-=16;
 // Safety gate revocation is latched and cannot be rearmed by later ticks.
 begin(fake,2,10,{0,0,1});msm2::input::tick(ctx,"verified_deinit",false);assert(killed&&current==windowsCap&&!ws_source_recording);
 msm2::input::kill();beforeHead=head;assert(invoke(p,src,11,22,33,44)==expected);assert(head==beforeHead);unhook();
 // Full raw ABI/state probe: all GPRs/flags, original return, stack arguments,
 // all YMM0..15 and nonvolatile registers survive the real callback.
 assert(__builtin_cpu_supports("avx"));for(unsigned i=0;i<128;++i)ws_test_vector_input[i]=i+.5f;
 ws_test_invoke_probe(reinterpret_cast<void*>(&ws_test_probe_target));std::array<std::uint64_t,11> gp{},ret{};std::array<float,128> vec{},returned{};std::array<std::uint64_t,8> ng{};
 std::memcpy(gp.data(),ws_test_gprs,sizeof(ws_test_gprs));std::memcpy(ret.data(),ws_test_return_gprs,sizeof(ws_test_return_gprs));std::memcpy(vec.data(),ws_test_vectors,sizeof(ws_test_vectors));std::memcpy(returned.data(),ws_test_returns,sizeof(ws_test_returns));std::memcpy(ng.data(),ws_test_nonvolatile,sizeof(ws_test_nonvolatile));
 hook(reinterpret_cast<void*>(&ws_test_probe_target));begin(fake,1,3,{0,0,1});beforeHead=head;ws_test_invoke_probe(reinterpret_cast<void*>(&ws_test_probe_target));assert(head==beforeHead+1);
 for(unsigned i=0;i<11;++i){auto mask=i==7?0x8c5ull:~0ull;assert((ws_test_gprs[i]&mask)==(gp[i]&mask));}
 assert(ws_test_result==0x77aabbcc11223344ull&&!std::memcmp(vec.data(),ws_test_vectors,sizeof(ws_test_vectors))&&!std::memcmp(ret.data(),ws_test_return_gprs,sizeof(ws_test_return_gprs))&&!std::memcmp(returned.data(),ws_test_returns,sizeof(ws_test_returns))&&!std::memcmp(ng.data(),ws_test_nonvolatile,sizeof(ws_test_nonvolatile)));
 assert(output.find("ORIENTATION_WRITER_ENTRY")!=std::string::npos&&output.find("ORIENTATION_SOURCE_SNAPSHOT")!=std::string::npos);unhook();
 assert(VirtualFree(memory,0,MEM_RELEASE));assert(MH_Uninitialize()==MH_OK);
 auto file=std::fopen("logs/windows_orientation_source_synthetic.log","wb");assert(file);std::fwrite(output.data(),1,output.size(),file);std::fclose(file);
 puts("PASS: actual MinHook entry/raw RCX/RDX/R8/R9/return snapshots on owned native functions, pre-original source, x87/MXCSR/YMM0..15/GPR/flags/stackargs/nonvolatiles/return unchanged; unreadable/cross-page/oversize rejection, event cap/F8 continue originals, wrong build refuses; synthetic only");
}
