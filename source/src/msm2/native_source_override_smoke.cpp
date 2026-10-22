// TEST ONLY: independent native caller/writer on owned buffers. No game bytes executed.
#include "../NativeHooks/source_lifecycle.cpp"
#include "../NativeHooks/native_source_override.cpp"
#include <MinHook.h>
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <string>
#include <cassert>
static unsigned resetRequests{},prearmRequests{};
namespace msm2::reset_trace {void request(const Request& r) noexcept {assert(r.prearmed&&!r.commitQpc);++prearmRequests;}
bool committed(const Request& r) noexcept {assert(r.commitQpc);++resetRequests;return false;}}
static msm2::Snapshot fakeHero;static std::string output;static bool sdkAvailable=true,sourceAuth=true;static unsigned char installedBytes[32]{},copyInstalledBytes[32]{};static bool copyAuth=true;
extern "C" {
alignas(16) float ws_test_copy_received[16];std::uint64_t ws_test_copy_calls{};
void ws_test_copy_writer();std::uint64_t ws_test_copy_caller(void*,const void*);void ws_reset_copy_thunk();void* ws_reset_copy_original{};std::atomic<unsigned> ws_reset_copy_recording{0};
alignas(16) float ws_test_source_received[16];std::uint64_t ws_test_source_calls{};
std::uint64_t ws_test_stack_caller(void*,const void*);void ws_test_native_source_writer();
void ws_source_thunk();void* ws_source_original{};std::atomic<unsigned> ws_source_recording{0};
}
struct CaptureFrame {std::uintptr_t r11,r10,r9,r8,rdx,rcx,rax,flags,ret;};
extern "C" void ws_source_capture(const CaptureFrame* f) noexcept {msm2::native_source::capture(f->rcx,f->rdx,f->r8,f->r9,f->ret,reinterpret_cast<std::uintptr_t>(&f->ret));}
extern "C" void ws_reset_copy_capture(const CaptureFrame* f) noexcept {msm2::native_source::captureCopy(f->rcx,f->rdx,f->r8,f->r9,f->ret,reinterpret_cast<std::uintptr_t>(&f->ret));}
namespace msm2::reset_copy {bool authenticated() noexcept {unsigned char b[32]{};return copyAuth&&msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_copy_writer),b,32)&&!std::memcmp(b,copyInstalledBytes,32);}}
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T done=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&done)&&done==n;}
bool sample(Snapshot& s){s=fakeHero;if(!sdkAvailable)return false;return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char b[4096];va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);output+=b;output+='\n';}
}
namespace msm2::input {bool authenticated(){unsigned char b[32]{};return sourceAuth&&msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_native_source_writer),b,32)&&!std::memcmp(b,installedBytes,32);}}
static ws::source_math::Matrix identity(){ws::source_math::Matrix m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;m.v[12]=123;m.v[13]=45;m.v[14]=67;return m;}
static std::uintptr_t owner{};static unsigned session{};static ws::source_math::Matrix vanilla;
static void prepare(ws::Vec3 n){
 killed=false;admitted=true;ws_native_source_enabled=1;config={};config.dualSource=false;state={};
 ws::native_source::armLifecycle(true);ws::native_source::event(0,owner);ws::native_source::event(1,owner);token.initMs=GetTickCount64()-800;ws::native_source::event(2,owner);
 ws::observed::VerifiedWallContext ctx{};ctx.lifecycleVerified=ctx.wallRunActive=true;ctx.wallRunOwner=owner;ctx.wallRunSessionId=++session;ctx.elapsed=.8;ctx.wallRunUpdateCalls=2;
 vanilla=identity();vanilla.v[0]=0;vanilla.v[2]=-1;vanilla.v[8]=1;vanilla.v[10]=0; // Forward +X, valid on A/E.
 std::memcpy(reinterpret_cast<void*>(fakeHero.transform),&vanilla,64);std::memcpy(reinterpret_cast<void*>(owner+0x398),&n,12);
 msm2::native_source::tick(ctx,"none",true);assert(state.published&&state.generation==token.generation);
}
static void call(bool advance=true){if(advance)ws::native_source::event(2,owner);state.published=GetTickCount64();auto result=ws_test_stack_caller(reinterpret_cast<void*>(fakeHero.transform),&vanilla);assert(result==0xabcdef0011223344ull);assert(!std::memcmp(ws_test_source_received+12,vanilla.v+12,16));msm2::native_source::drainLogs();}
int main(){
 auto* mem=static_cast<unsigned char*>(VirtualAlloc(nullptr,8192,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(mem);owner=reinterpret_cast<std::uintptr_t>(mem+1024);fakeHero.hero=reinterpret_cast<std::uintptr_t>(mem+256);fakeHero.transform=reinterpret_cast<std::uintptr_t>(mem);
 // Wrong build refuses master ON before any pointer/code operation.
 msm2::native_source::configure({},false,1,1,{});assert(!admitted&&!ws_native_source_enabled);
 // Real MinHook on our own independent native writer; synthetic caller layout
 // deliberately shares only documented ABI/stack offsets, NOT game body/hash.
 assert(MH_Initialize()==MH_OK);assert(MH_CreateHook(reinterpret_cast<void*>(&ws_test_native_source_writer),reinterpret_cast<void*>(&ws_source_thunk),&ws_source_original)==MH_OK);assert(MH_EnableHook(reinterpret_cast<void*>(&ws_test_native_source_writer))==MH_OK);assert(msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_native_source_writer),installedBytes,32));
 imageBase=reinterpret_cast<std::uintptr_t>(&ws_test_stack_caller)-callBegin;assert(msm2::readMemory(imageBase+callBegin,callerOriginal,168));
 for(unsigned i=0;i<4;++i){lifecyclePatches[i].address=reinterpret_cast<std::uintptr_t>(mem+4096+i*64);for(unsigned j=0;j<32;++j)lifecyclePatches[i].bytes[j]=static_cast<unsigned char>(i*32+j);std::memcpy(reinterpret_cast<void*>(lifecyclePatches[i].address),lifecyclePatches[i].bytes.data(),32);}
 // Native Update path requests at .60 before any source entry/override.
 prepare({0,0,1});token.initMs=GetTickCount64()-610;state.elapsed=.61f;
 auto prearmBefore=prearmRequests;
 ws::native_source::setObserver(msm2::native_source::prearm);
 ws::native_source::event(2,owner);assert(prearmRequests==prearmBefore+1&&state.writes==0);
 call();assert(prearmRequests==prearmBefore+1&&state.writes==0);
 ws::native_source::setObserver(nullptr);
 for(ws::Vec3 n: {ws::Vec3{0,0,1},ws::Vec3{-.70710678f,0,.70710678f}}){
  prepare(n);auto resetBefore=resetRequests;auto callerInput=vanilla;call();assert(state.writes==1&&state.transition.active&&state.lastT==0&&!std::memcmp(&callerInput,&vanilla,64));
  state.transition.entryMs=GetTickCount64()-260;call();assert(state.lastT==1);auto up=ws::diag::actorAxes(ws_test_source_received).up;assert(ws::dot(up,n)>.999f);
  for(unsigned i=0;i<22;++i){call();assert(state.transition.active);}assert(state.pass&&state.heldUpdates>=20);assert(resetRequests==resetBefore+state.writes);
  assert(output.find("position_unchanged=1 source_is_stack_local=1")!=std::string::npos);
  // Deinit closes the NATIVE gate before worker processes the event.
  auto writes=state.writes;ws::native_source::event(3,owner);call(false);assert(state.writes==writes&&!std::memcmp(ws_test_source_received,&vanilla,64));
 }
 // Delay gate, wrong RCX, wrong caller/R8, stale binding/native frame: no source write.
 prepare({0,0,1});token.initMs=GetTickCount64();call();assert(state.writes==0);token.initMs-=800;state.elapsed=.5f;call();assert(state.writes==0);state.elapsed=.8f;
 auto before=state.writes;msm2::native_source::capture(fakeHero.transform, reinterpret_cast<std::uintptr_t>(&vanilla),1,0,imageBase+callReturn,reinterpret_cast<std::uintptr_t>(&vanilla)-0x28);assert(state.writes==before); // R8 must match observed null ABI
 msm2::native_source::capture(fakeHero.transform+16,reinterpret_cast<std::uintptr_t>(&vanilla),0,0,imageBase+callReturn,reinterpret_cast<std::uintptr_t>(&vanilla)-0x28);assert(state.writes==before);
 prepare({0,0,1});
 // A heap source pretending to be a caller-local matrix must fail TEB range.
 auto p=reinterpret_cast<std::uintptr_t>(mem+2048);std::memcpy(reinterpret_cast<void*>(p),&vanilla,64);msm2::native_source::capture(fakeHero.transform,p,0,0,imageBase+callReturn,p-0x28);assert(state.writes==0&&!std::memcmp(reinterpret_cast<void*>(p),&vanilla,64));
 // A reset must not disable exact source ownership and allow late WorldUp calls.
 prepare({0,0,1});call();state.transition.entryMs=GetTickCount64()-260;call();call();assert(state.bestSurface>.98f);std::memcpy(reinterpret_cast<void*>(fakeHero.transform),&vanilla,64);before=state.writes;call();assert(state.writes==before+1&&state.transition.active&&state.heldUpdates==0);call();assert(state.writes==before+2);
 // The same source setter from a Crawl path never receives an override.
 prepare({0,0,1});before=state.writes;msm2::native_source::capture(fakeHero.transform,reinterpret_cast<std::uintptr_t>(&vanilla),0,0,imageBase+0x1021c9e,reinterpret_cast<std::uintptr_t>(&vanilla)-0x28);assert(state.writes==before);
 // Same-value duplicate Init/owner reuse cannot authorize a stale worker context.
 prepare({0,0,1});before=state.writes;ws::native_source::event(3,owner);ws::native_source::event(0,owner);ws::native_source::event(1,owner);token.initMs-=800;ws::native_source::event(2,owner);call();assert(state.writes==before);
 prepare({0,0,1});ws::Vec3 floor{0,1,0};std::memcpy(reinterpret_cast<void*>(owner+0x398),&floor,12);call();assert(!state.writes);
 prepare({0,0,1});token.updateMs=GetTickCount64()-101;call(false);assert(!state.writes);
 prepare({0,0,1});fakeHero.hero+=16;call();assert(!state.writes);fakeHero.hero-=16;
 prepare({0,0,1});*reinterpret_cast<unsigned char*>(lifecyclePatches[0].address)^=1;call();assert(!state.writes);*reinterpret_cast<unsigned char*>(lifecyclePatches[0].address)^=1;
 prepare({0,0,1});msm2::native_source::kill();call();assert(!state.writes&&!ws_native_source_enabled);
 prepare({0,0,1});sdkAvailable=false;call();assert(!state.writes);sdkAvailable=true;
 // Native lifecycle lock contention is a missing sample, never an invented Deinit.
 prepare({0,0,1});call();assert(state.transition.active);assert(!gate.test_and_set());ws::native_source::Token missing{};assert(!ws::native_source::lifecycle(missing)&&!missing.observed);gate.clear();
 // Wrong native thread cannot reuse this thread's lifecycle token.
 prepare({0,0,1});auto thread=CreateThread(nullptr,0,[](void*)->DWORD{ws_test_stack_caller(reinterpret_cast<void*>(fakeHero.transform),&vanilla);return 0;},nullptr,0,nullptr);assert(thread&&WaitForSingleObject(thread,2000)==WAIT_OBJECT_0);CloseHandle(thread);assert(!state.writes);
 // Real second raw-entry thunk + independent native writer, not game bytes.
 assert(MH_CreateHook(reinterpret_cast<void*>(&ws_test_copy_writer),reinterpret_cast<void*>(&ws_reset_copy_thunk),&ws_reset_copy_original)==MH_OK);assert(MH_EnableHook(reinterpret_cast<void*>(&ws_test_copy_writer))==MH_OK);assert(msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_copy_writer),copyInstalledBytes,32));
 for(ws::Vec3 n: {ws::Vec3{0,0,1},ws::Vec3{-.70710678f,0,.70710678f}}){
  prepare(n);config.dualSource=true;state.elapsed=.69f;token.initMs=GetTickCount64()-710;
  call();assert(state.commonWrites==1);auto entryTime=state.transition.entryMs;auto entryQ=state.transition.entryQ;
  state.transition.entryMs=GetTickCount64()-260;entryTime=state.transition.entryMs;
  auto callB=[&]{auto original=vanilla;auto result=ws_test_copy_caller(reinterpret_cast<void*>(fakeHero.transform),&vanilla);assert(result==0xabcdef0011223344ull);assert(!std::memcmp(&original,&vanilla,64));assert(!std::memcmp(ws_test_copy_received+12,vanilla.v+12,16));msm2::native_source::drainLogs();};
  callB();assert(state.copyWrites==1&&state.lastT==1);call(false);assert(state.commonWrites==2&&state.transition.entryMs==entryTime&&!std::memcmp(&entryQ,&state.transition.entryQ,sizeof(entryQ)));
  assert(!std::memcmp(ws_test_source_received,ws_test_copy_received,48));
  auto nativeUpdates=token.updates;for(unsigned i=0;i<22;++i)callB();assert(token.updates==nativeUpdates&&state.heldUpdates<20&&!state.pass);
  for(unsigned i=0;i<22;++i){call();callB();}assert(state.pass&&state.heldUpdates>=20);
  assert(output.find("RESET_COPY_SOURCE_ENTRY")!=std::string::npos&&output.find("R8=0x1234 R9=0x5678")!=std::string::npos&&output.find("LATE_COMMON_WORLDUP_OVERRIDE=1")!=std::string::npos&&output.find("ORIENTATION_RETENTION_PASS=1")!=std::string::npos);
  auto count=state.writes;ws::native_source::event(3,owner);callB();assert(state.writes==count);
 }
 // A same-update reset between A and B must break continuity, even if the
 // next exact source promptly restores Surface Up. Never count20 such updates.
 prepare({0,0,1});config.dualSource=true;call();state.transition.entryMs=GetTickCount64()-260;ws_test_copy_caller(reinterpret_cast<void*>(fakeHero.transform),&vanilla);
 for(unsigned i=0;i<24;++i){call();std::memcpy(reinterpret_cast<void*>(fakeHero.transform),&vanilla,64);ws_test_copy_caller(reinterpret_cast<void*>(fakeHero.transform),&vanilla);}
 assert(!state.pass&&state.heldUpdates==0&&state.transition.active);
 // Real time retention alternative: >=300 ms with fewer than20 updates.
 prepare({0,0,1});config.dualSource=true;call();state.transition.entryMs=GetTickCount64()-260;
 ws_test_copy_caller(reinterpret_cast<void*>(fakeHero.transform),&vanilla);
 for(unsigned i=0;i<8;++i){call();ws_test_copy_caller(reinterpret_cast<void*>(fakeHero.transform),&vanilla);Sleep(50);}
 assert(state.pass&&state.heldUpdates<20&&GetTickCount64()-state.firstHeld>=300);
 msm2::native_source::drainLogs();
 prepare({0,0,1});config.dualSource=true;auto copyCount=state.copyWrites;
 // Heap/wrong caller/target/delay/copy-hook ownership fail closed.
 msm2::native_source::captureCopy(fakeHero.transform,p,0x1234,0x5678,imageBase+0x2cfc84b,p-0x28);assert(state.copyWrites==copyCount);
 prepare({0,0,1});config.dualSource=true;msm2::native_source::captureCopy(fakeHero.transform,reinterpret_cast<std::uintptr_t>(&vanilla),0,0,imageBase+0x2cfc84c,reinterpret_cast<std::uintptr_t>(&vanilla)-0x28);assert(!state.copyWrites);
 prepare({0,0,1});config.dualSource=true;auto local=vanilla;auto localPtr=reinterpret_cast<std::uintptr_t>(&local);
 msm2::native_source::captureCopy(fakeHero.transform,localPtr,0x1234,0x5678,imageBase+0x2cfc84b,localPtr-0x500);assert(!state.copyWrites&&!std::memcmp(&local,&vanilla,64));
 prepare({0,0,1});config.dualSource=true;local=vanilla;local.v[4]=1000;
 msm2::native_source::captureCopy(fakeHero.transform,localPtr,0x1234,0x5678,imageBase+0x2cfc84b,localPtr-0x28);assert(!state.copyWrites&&local.v[4]==1000);msm2::native_source::drainLogs();assert(output.find("caller_local_provenance_or_source_sanity_failed")!=std::string::npos);
 prepare({0,0,1});config.dualSource=true;copyAuth=false;ws_test_copy_caller(reinterpret_cast<void*>(fakeHero.transform),&vanilla);assert(!state.copyWrites&&!ws_native_source_enabled);copyAuth=true;
 assert(MH_DisableHook(reinterpret_cast<void*>(&ws_test_copy_writer))==MH_OK);assert(MH_RemoveHook(reinterpret_cast<void*>(&ws_test_copy_writer))==MH_OK);
 // The public write helper independently refuses heap destinations.
 assert(!ws::interop::writeStackOrientation(p,vanilla,vanilla).success);
 msm2::native_source::stop();assert(MH_DisableHook(reinterpret_cast<void*>(&ws_test_native_source_writer))==MH_OK);assert(MH_RemoveHook(reinterpret_cast<void*>(&ws_test_native_source_writer))==MH_OK);assert(MH_Uninitialize()==MH_OK);assert(VirtualFree(mem,0,MEM_RELEASE));
 auto f=std::fopen("logs/windows_native_source_synthetic.log","wb");assert(f);std::fwrite(output.data(),1,output.size(),f);std::fclose(f);
 puts("PASS: independent actual MinHook native caller stack+0x28 input -> native writer, A/E transition/20distinctUpdates ownership, Position/metadata/parent input unchanged, original return; native Deinit/ABA/R8/wrongtarget/heap/stale/normal/identity/patch/F8/SDK gates, reset observation does not suppress late source calls; dual raw-entry sources share one transition, late same-update coverage, copy heap/caller/auth refusal, direct-Actor writes absent; synthetic NOT game evidence");
}
