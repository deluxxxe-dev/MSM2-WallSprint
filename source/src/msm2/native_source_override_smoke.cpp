// TEST ONLY: independent native callers/writers and owned buffers. No game bytes.
#include "../NativeHooks/source_lifecycle.cpp"
#include "../NativeHooks/native_source_override.cpp"
#include <MinHook.h>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <string>
namespace msm2::trace {void beginResetCopyEntry() noexcept {} void resetCopyEntry(const path_b::Entry&) noexcept {}}
static unsigned resetRequests{},prearmRequests{};
namespace msm2::reset_trace {
void request(const Request& r) noexcept {assert(r.prearmed&&!r.commitQpc);++prearmRequests;}
bool committed(const Request& r) noexcept {assert(r.commitQpc);++resetRequests;return true;}
}
static msm2::Snapshot fakeHero;static std::string output;static bool sdkAvailable=true,sourceAuth=true,copyAuth=true;
static unsigned char installedBytes[32]{},copyInstalledBytes[32]{};
extern "C" {
alignas(16) float ws_test_copy_received[16];std::uint64_t ws_test_copy_calls{};
void ws_test_copy_writer();std::uint64_t ws_test_copy_caller(void*,const void*);void ws_reset_copy_thunk();void* ws_reset_copy_original{};std::atomic<unsigned> ws_reset_copy_recording{0};
alignas(16) float ws_test_source_received[16];std::uint64_t ws_test_source_calls{};
std::uint64_t ws_test_stack_caller(void*,const void*);void ws_test_native_source_writer();void ws_source_thunk();void* ws_source_original{};std::atomic<unsigned> ws_source_recording{0};
}
struct CaptureFrame {std::uintptr_t r11,r10,r9,r8,rdx,rcx,rax,flags,ret;};
extern "C" void ws_source_capture(const CaptureFrame* f) noexcept {msm2::native_source::capture(f->rcx,f->rdx,f->r8,f->r9,f->ret,reinterpret_cast<std::uintptr_t>(&f->ret));}
extern "C" void ws_reset_copy_capture(const CaptureFrame* f) noexcept {msm2::native_source::captureCopy(f->rcx,f->rdx,f->r8,f->r9,f->ret,reinterpret_cast<std::uintptr_t>(&f->ret));}
namespace msm2::reset_copy {
bool authenticated() noexcept {unsigned char b[32]{};return copyAuth&&msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_copy_writer),b,32)&&!std::memcmp(b,copyInstalledBytes,32);}
bool compactProofAuthenticated() noexcept {return authenticated();}
}
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){SIZE_T done=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&done)&&done==n;}
bool sample(Snapshot& s){s=fakeHero;if(!sdkAvailable)return false;return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char b[4096];va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);output+=b;output+='\n';}
}
namespace msm2::input {bool authenticated(){unsigned char b[32]{};return sourceAuth&&msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_native_source_writer),b,32)&&!std::memcmp(b,installedBytes,32);}}
static ws::source_math::Matrix vanilla;static ws::source_math::CompactBlock compact;static std::uintptr_t owner{};static unsigned session{};
static ws::observed::VerifiedWallContext context;
static ws::source_math::Matrix identity(){ws::source_math::Matrix m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;m.v[12]=123;m.v[13]=45;m.v[14]=67;return m;}
static void workerTick(bool advance=true){if(advance)ws::native_source::event(2,owner);context.wallRunUpdateCalls=token.updates;context.elapsed=double(GetTickCount64()-token.initMs)/1000.;msm2::native_source::tick(context,"none",true);}
static void prepare(ws::Vec3 normal){
 killed=false;admitted=true;ws_native_source_enabled=1;config={};config.dualSource=true;state={};pathAWrites=0;pathBWrites=0;pathARejects=0;pathBRejects=0;resetRequested=false;publishInactive();
 ws::native_source::armLifecycle(true);ws::native_source::event(0,owner);ws::native_source::event(1,owner);token.initMs=GetTickCount64()-800;ws::native_source::event(2,owner);
 context={};context.lifecycleVerified=context.wallRunActive=true;context.wallRunOwner=owner;context.wallRunSessionId=++session;context.elapsed=.8;
 vanilla=identity();vanilla.v[0]=0;vanilla.v[2]=-1;vanilla.v[8]=1;vanilla.v[10]=0;
 compact={};compact.rows[2]=-1;compact.rows[4]=1;compact.rows[6]=1;for(unsigned i=0;i<compact.metadata.size();++i)compact.metadata[i]=static_cast<unsigned char>(0xa0+i);
 std::memcpy(reinterpret_cast<void*>(fakeHero.transform),&vanilla,64);std::memcpy(reinterpret_cast<void*>(owner+0x398),&normal,12);workerTick(false);Sleep(300);workerTick();
 Published p{};assert(publication.read(p)&&p.active&&p.t>=.99f&&p.generation==token.generation);
}
static void callA(){auto original=vanilla;auto result=ws_test_stack_caller(reinterpret_cast<void*>(fakeHero.transform),&vanilla);assert(result==0xabcdef0011223344ull);assert(!std::memcmp(&original,&vanilla,64));}
static void callB(){auto original=compact;auto result=ws_test_copy_caller(reinterpret_cast<void*>(fakeHero.transform),&compact);assert(result==0xabcdef0011223344ull);assert(!std::memcmp(&original,&compact,64));assert(!std::memcmp(reinterpret_cast<unsigned char*>(ws_test_copy_received)+36,compact.metadata.data(),28));}
int main(){
 alignas(16) ws::source_math::CompactBlock local{},changed{};local.rows[0]=local.rows[4]=local.rows[8]=1;changed=local;changed.metadata[0]=1;
 auto localBefore=local;assert(!ws::interop::writeStackCompact(reinterpret_cast<std::uintptr_t>(&local),local,changed).success&&!std::memcmp(&local,&localBefore,64));
 auto* mem=static_cast<unsigned char*>(VirtualAlloc(nullptr,8192,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(mem);owner=reinterpret_cast<std::uintptr_t>(mem+1024);fakeHero.hero=reinterpret_cast<std::uintptr_t>(mem+256);fakeHero.transform=reinterpret_cast<std::uintptr_t>(mem);
 msm2::native_source::configure({},false,1,1,{});assert(!admitted&&!ws_native_source_enabled);
 assert(MH_Initialize()==MH_OK);assert(MH_CreateHook(reinterpret_cast<void*>(&ws_test_native_source_writer),reinterpret_cast<void*>(&ws_source_thunk),&ws_source_original)==MH_OK);assert(MH_EnableHook(reinterpret_cast<void*>(&ws_test_native_source_writer))==MH_OK);assert(msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_native_source_writer),installedBytes,32));
 assert(MH_CreateHook(reinterpret_cast<void*>(&ws_test_copy_writer),reinterpret_cast<void*>(&ws_reset_copy_thunk),&ws_reset_copy_original)==MH_OK);assert(MH_EnableHook(reinterpret_cast<void*>(&ws_test_copy_writer))==MH_OK);assert(msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_copy_writer),copyInstalledBytes,32));
 imageBase=reinterpret_cast<std::uintptr_t>(&ws_test_stack_caller)-callBegin;assert(msm2::readMemory(imageBase+callBegin,callerOriginal,168));
 for(unsigned i=0;i<4;++i){lifecyclePatches[i].address=reinterpret_cast<std::uintptr_t>(mem+4096+i*64);for(unsigned j=0;j<32;++j)lifecyclePatches[i].bytes[j]=static_cast<unsigned char>(i*32+j);std::memcpy(reinterpret_cast<void*>(lifecyclePatches[i].address),lifecyclePatches[i].bytes.data(),32);}
 msm2::path_b::configure(true,true);
 for(ws::Vec3 n:{ws::Vec3{0,0,1},ws::Vec3{-.70710678f,0,.70710678f}}){
  prepare(n);callA();assert(pathAWrites==1);auto a=ws::diag::actorAxes(ws_test_source_received);assert(a.valid&&ws::dot(a.up,n)>.999f);
  auto metadata=compact.metadata;callB();assert(pathBWrites==1&&compact.metadata==metadata);msm2::sample(fakeHero);auto b=ws::diag::actorAxes(fakeHero.prefix);assert(b.valid&&ws::dot(b.up,n)>.999f);
  assert(!stateLock.test_and_set());callA();callB();stateLock.clear();assert(pathAWrites==2&&pathBWrites==2);
  for(unsigned i=0;i<22;++i){callA();callB();workerTick();}
  assert(state.pass&&state.heldUpdates>=20);msm2::native_source::drainLogs();
  assert(output.find("PATH_A_LOCKFREE_SNAPSHOT_READ")!=std::string::npos&&output.find("PATH_B_COMPACT3X3_OVERRIDE")!=std::string::npos&&output.find("bytes_written=0x24 metadata_untouched=1")!=std::string::npos&&output.find("PATH_A_STATE_LOCK_CONTENTION=0")!=std::string::npos&&output.find("ORIENTATION_RETENTION_PASS=1")!=std::string::npos);
 }
 prepare({0,0,1});compact.rows[4]=0;auto before=compact;callB();assert(pathBWrites==0&&!std::memcmp(&before,&compact,64));compact=before;compact.rows[4]=3;before=compact;callB();assert(pathBWrites==0&&!std::memcmp(&before,&compact,64));
 auto ptr=reinterpret_cast<std::uintptr_t>(&compact);msm2::native_source::captureCopy(fakeHero.transform+16,ptr,0,0,imageBase+0x2cfc84b,ptr-0x28);msm2::native_source::captureCopy(fakeHero.transform,ptr,0,0,imageBase+0x2cfc84c,ptr-0x28);assert(pathBWrites==0);
 copyAuth=false;callB();copyAuth=true;assert(pathBWrites==0);sdkAvailable=false;callA();sdkAvailable=true;assert(pathAWrites==0);
 auto thread=CreateThread(nullptr,0,[](void*)->DWORD{callA();callB();return 0;},nullptr,0,nullptr);assert(thread&&WaitForSingleObject(thread,2000)==WAIT_OBJECT_0);CloseHandle(thread);assert(pathAWrites==0&&pathBWrites==0);
 auto closed=token;closed.phase=0;msm2::native_source::lifecycleClosed(closed);callA();callB();assert(pathAWrites==0&&pathBWrites==0);
 msm2::native_source::kill();callA();callB();assert(!ws_native_source_enabled&&pathAWrites==0&&pathBWrites==0);
 assert(MH_DisableHook(reinterpret_cast<void*>(&ws_test_copy_writer))==MH_OK);assert(MH_RemoveHook(reinterpret_cast<void*>(&ws_test_copy_writer))==MH_OK);assert(MH_DisableHook(reinterpret_cast<void*>(&ws_test_native_source_writer))==MH_OK);assert(MH_RemoveHook(reinterpret_cast<void*>(&ws_test_native_source_writer))==MH_OK);assert(MH_Uninitialize()==MH_OK);assert(VirtualFree(mem,0,MEM_RELEASE));
 auto f=std::fopen("logs/windows_native_source_synthetic.log","wb");assert(f);std::fwrite(output.data(),1,output.size(),f);std::fclose(f);
 puts("PASS: lock-free coherent A/B shared basis, A exact stack source, B compact36-only scale/metadata preservation, contended worker lock cannot reject hooks, A/E retention, identity/thread/lifecycle/auth/kill/degenerate gates; independent MinHook fixtures only");
}
