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
bool committed(const Request& r) noexcept {assert(r.commitQpc);++resetRequests;auto now=GetTickCount64();return now>=r.initMs&&now-r.initMs<1500;} // Model the bounded diagnostic ending; source ownership must continue.
}
static msm2::Snapshot fakeHero;static std::string output;static bool sdkAvailable=true,sourceAuth=true,copyAuth=true,deinitOnSample=false,pointReadable=true;static unsigned planeReads{};
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
bool readMemory(std::uintptr_t p,void* out,std::size_t n){if(p==token.owner+0x338){++planeReads;if(!pointReadable)return false;}SIZE_T done=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&done)&&done==n;}
bool sample(Snapshot& s){s=fakeHero;if(!sdkAvailable)return false;if(deinitOnSample){deinitOnSample=false;ws::native_source::event(3,token.owner);}return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char b[4096];va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);output+=b;output+='\n';}
}
namespace msm2::input {bool authenticated(){unsigned char b[32]{};return sourceAuth&&msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_native_source_writer),b,32)&&!std::memcmp(b,installedBytes,32);}}
namespace msm2::input {bool originalWriterForPositionProof(std::array<unsigned char,584>&){return false;}} // No game bytes or false startup permission in this fixture.
static ws::source_math::Matrix vanilla;static ws::source_math::CompactBlock compact;static std::uintptr_t owner{};static unsigned session{};
static ws::observed::VerifiedWallContext context;
static ws::source_math::Matrix identity(){ws::source_math::Matrix m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;m.v[12]=123;m.v[13]=45;m.v[14]=67;return m;}
static void waitMs(ULONGLONG duration){auto begin=GetTickCount64();do {Sleep(1);}while(GetTickCount64()-begin<duration);}
static void workerTick(bool advance=true){if(advance)ws::native_source::event(2,owner);context.wallRunUpdateCalls=token.updates;context.elapsed=double(GetTickCount64()-token.initMs)/1000.;msm2::native_source::tick(context,"none",true);}
static void prepare(ws::Vec3 normal,bool full=true){
 killed=false;admitted=true;ws_native_source_enabled=1;config={};config.dualSource=true;state={};pathAWrites=0;pathBWrites=0;pathARejects=0;pathBRejects=0;resetRequested=false;publishInactive();
 ws::native_source::armLifecycle(true);ws::native_source::event(0,owner);ws::native_source::event(1,owner);token.initMs=GetTickCount64()-800;ws::native_source::event(2,owner);
 context={};context.lifecycleVerified=context.wallRunActive=true;context.wallRunOwner=owner;context.wallRunSessionId=++session;context.elapsed=.8;
 vanilla=identity();vanilla.v[0]=0;vanilla.v[2]=-1;vanilla.v[8]=1;vanilla.v[10]=0;
 compact={};compact.rows[2]=-1;compact.rows[4]=1;compact.rows[6]=1;for(unsigned i=0;i<compact.metadata.size();++i)compact.metadata[i]=static_cast<unsigned char>(0xa0+i);
 pointReadable=true;fakeHero.position[0]=normal.x*.5f;fakeHero.position[1]=normal.y*.5f;fakeHero.position[2]=normal.z*.5f;ws::Vec3 point{};std::memcpy(reinterpret_cast<void*>(owner+0x338),&point,12);
 std::memcpy(reinterpret_cast<void*>(fakeHero.transform),&vanilla,64);std::memcpy(reinterpret_cast<void*>(owner+0x398),&normal,12);workerTick(false);
 // Publish normal native cadence until admission/full interpolation instead of
 // assuming the first worker tick survived host scheduling within100ms.
 Published p{};auto prepareStart=GetTickCount64();bool read=publication.read(p);
 while((!read||!p.active||(full&&p.t<1.f))&&!killed.load()&&state.ownership!=Ownership::HardClosed&&GetTickCount64()-prepareStart<2000){Sleep(16);workerTick();read=publication.read(p);}
 if(!full){assert(read&&p.active);return;}

 if(!read||!p.active||p.t<.99f||p.generation!=token.generation)std::fprintf(stderr,"prepare failed session=%u state=%u reason=%s killed=%u p_active=%u t=%g token_age=%llu entry_age=%g\n",session,unsigned(state.ownership),state.exitReason,unsigned(killed.load()),unsigned(p.active),p.t,(unsigned long long)(GetTickCount64()-token.updateMs),double(GetTickCount64())-state.transition.entryMs);
 assert(read&&p.active&&p.t>=.99f&&p.generation==token.generation);
}
static void callA(){auto original=vanilla;auto result=ws_test_stack_caller(reinterpret_cast<void*>(fakeHero.transform),&vanilla);assert(result==0xabcdef0011223344ull);assert(!std::memcmp(&original,&vanilla,64));}
static void callB(){auto original=compact;auto result=ws_test_copy_caller(reinterpret_cast<void*>(fakeHero.transform),&compact);assert(result==0xabcdef0011223344ull);assert(!std::memcmp(&original,&compact,64));assert(!std::memcmp(reinterpret_cast<unsigned char*>(ws_test_copy_received)+36,compact.metadata.data(),28));}
static unsigned countText(const std::string& text,const std::string& key){unsigned count=0;std::size_t pos=0;while((pos=text.find(key,pos))!=std::string::npos){++count;pos+=key.size();}return count;}
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
 // The production source token/worker/A-B path runs through a real multi-second
 // interval; diagnostic lifetime is not consulted by this orientation contract.
 ws::native_source::setObserver([](const Token& t) noexcept {if(t.phase!=3)msm2::native_source::lifecycleClosed(t);});
 prepare({0,0,1});auto sustainedSession=state.session;std::atomic<bool> readerDone{false};
 auto reader=CreateThread(nullptr,0,[](void* raw)->DWORD{auto& done=*static_cast<std::atomic<bool>*>(raw);while(!done.load()){Token t{};ws::native_source::lifecycle(t);Sleep(0);}return 0;},&readerDone,0,nullptr);assert(reader);
 unsigned atTwoSeconds=0;do {
  callA();callB();Sleep(16);workerTick();assert(state.transition.active&&msm2::native_source::active());msm2::Snapshot actual{};assert(msm2::sample(actual)&&ws::dot(ws::diag::actorAxes(actual.prefix).up,ws::Vec3{0,0,1})>.999f);
  if(context.elapsed>=2&&atTwoSeconds==0)atTwoSeconds=pathAWrites.load();
  msm2::native_source::drainLogs();
 }while(context.elapsed<4.1);
 readerDone=true;assert(WaitForSingleObject(reader,2000)==WAIT_OBJECT_0);CloseHandle(reader);
 assert(state.pass&&state.maxHeldMs>2000&&state.maxHeldUpdates>=100&&pathAWrites.load()>atTwoSeconds+80&&pathBWrites.load()==pathAWrites.load());
 auto heldA=pathAWrites.load(),heldB=pathBWrites.load();ws::native_source::event(3,owner);assert(!msm2::native_source::active());callA();callB();assert(pathAWrites==heldA&&pathBWrites==heldB);
 auto readsAtDeinit=planeReads;context.wallRunActive=false;msm2::native_source::tick(context,"verified_deinit",true);msm2::native_source::tick(context,"verified_deinit",true);assert(planeReads==readsAtDeinit);
 auto summaryKey="ORIENTATION_LIFECYCLE_SUMMARY session="+std::to_string(sustainedSession)+" ";assert(countText(output,summaryKey)==1);auto summary=output.substr(output.find(summaryKey));assert(summary.find("source_exit_reason=verified_native_Deinit source_active_until_deinit=1 verified_deinit=1")!=std::string::npos&&state.summaryEmitted);
 auto adhesionKey="ADHESION_SUMMARY session="+std::to_string(sustainedSession)+" ";assert(countText(output,adhesionKey)==1&&state.adhesion.samples>20&&state.adhesion.attempts<=64&&output.find("mode=position_read_only")!=std::string::npos);
 assert(output.find("PATH_A_POSITION_PROOF admitted=0")!=std::string::npos&&output.find("PATH_A_POSITION_RUNTIME")!=std::string::npos);
 prepare({0,0,1});auto beforeInvalid=state.adhesion.failClosed;pointReadable=false;waitMs(130);workerTick();assert(state.transition.active&&state.adhesion.failClosed>beforeInvalid&&output.find("reason=P338_unreadable")!=std::string::npos);pointReadable=true;
 fakeHero.position[2]=.1f;waitMs(130);workerTick();assert(state.transition.active&&output.find("reason=out_of_range")!=std::string::npos);
 fakeHero.position[2]=.4f;waitMs(130);workerTick();assert(state.transition.active&&state.adhesion.maxRequested<=.02f&&state.adhesion.maxRequested>.019f);callA();callB();assert(!std::memcmp(ws_test_source_received+12,vanilla.v+12,16));
 auto planeBeforeKill=planeReads;msm2::native_source::kill();workerTick(false);assert(planeReads==planeBeforeKill);
 // Reproduce the measured c6 gap with production native publication/worker
 // and both real MinHook source paths. No fake clock or synthetic keepalive.
 prepare({0,0,1});callA();callB();auto recoveredSession=state.session;
 while(context.elapsed<2.4){callA();callB();Sleep(16);workerTick();msm2::native_source::drainLogs();}
 auto originalEntry=state.transition.entryMs;auto originalTarget=state.transition.targetQ;
 auto beforeA=pathAWrites.load(),beforeB=pathBWrites.load();waitMs(270);
 callA();callB();assert(pathAWrites==beforeA&&pathBWrites==beforeB); // before worker detects stale
 workerTick(false);assert(state.transition.active&&state.ownership==Ownership::StaleSuspended&&!msm2::native_source::active());
 for(unsigned i=0;i<10;++i){callA();callB();workerTick(false);}
 assert(pathAWrites==beforeA&&pathBWrites==beforeB&&state.staleSuspends==1&&state.staleResumes==0);
 workerTick();assert(state.ownership==Ownership::Active&&state.session==recoveredSession&&state.lastT==1&&state.transition.entryMs==originalEntry&&!std::memcmp(&originalTarget,&state.transition.targetQ,sizeof(originalTarget)));
 callA();callB();assert(pathAWrites==beforeA+1&&pathBWrites==beforeB+1&&ws_test_source_received[6]>.999f&&ws_test_copy_received[6]>.999f);
 do {Sleep(16);workerTick();callA();callB();msm2::native_source::drainLogs();}while(context.elapsed<4.1);
 assert(state.pass&&state.staleSuspends==1&&state.staleResumes==1&&state.maxStaleMs>=150);
 auto recoveredExit="NATIVE_SOURCE_EXIT session="+std::to_string(recoveredSession)+" ";msm2::native_source::drainLogs();assert(output.find(recoveredExit)==std::string::npos);
 ws::native_source::event(3,owner);context.wallRunActive=false;msm2::native_source::tick(context,"verified_deinit",true);
 auto recoverySummary="ORIENTATION_LIFECYCLE_SUMMARY session="+std::to_string(recoveredSession)+" ";auto recovered=output.substr(output.find(recoverySummary));
 assert(countText(output,recoverySummary)==1&&recovered.find("source_active_until_deinit=1 verified_deinit=1")!=std::string::npos&&recovered.find("stale_suspend_count=1 stale_resume_count=1")!=std::string::npos);
 auto suspendKey="NATIVE_SOURCE_STALE_SUSPEND session="+std::to_string(recoveredSession)+" ";auto resumeKey="NATIVE_SOURCE_STALE_RESUME session="+std::to_string(recoveredSession)+" ";
 assert(countText(output,suspendKey)==1&&countText(output,resumeKey)==1);
 // Fresh worker snapshot cannot extend a native token's 100ms lease. A hook
 // signal survives a real Update arriving before the next worker poll.
 prepare({0,0,1});waitMs(75);workerTick(false);waitMs(55);callA();callB();assert(pathAWrites==0&&pathBWrites==0);
 workerTick();assert(state.staleSuspends==1&&state.staleResumes==1&&state.lastT==1);callA();callB();assert(pathAWrites==1&&pathBWrites==1);
 // Before t=1 freeze at the last published quaternion and continue from there.
 prepare({0,0,1},false);waitMs(65);workerTick();float partialT=state.lastT;auto partialBasis=state.currentBasis;auto partialEntry=state.transition.entryMs;
 assert(partialT>0&&partialT<1);waitMs(270);workerTick(false);workerTick();
 assert(state.session==context.wallRunSessionId&&state.transition.entryMs==partialEntry&&std::abs(state.lastT-partialT)<.0001f);
 for(unsigned i=0;i<12;++i)assert(std::abs(state.currentBasis.v[i]-partialBasis.v[i])<.0001f);
 waitMs(65);workerTick();assert(state.lastT>partialT&&state.lastT<1);waitMs(220);workerTick();assert(state.lastT==1);
 // Repeated gaps retain target and produce bounded episode pairs, full metrics.
 prepare({0,0,1});for(unsigned i=0;i<18;++i){waitMs(130);workerTick(false);workerTick();}
 assert(state.staleSuspends==18&&state.staleResumes==18);
 auto boundedKey="NATIVE_SOURCE_STALE_SUSPEND session="+std::to_string(state.session)+" ";assert(countText(output,boundedKey)==16);
 // A hard invalidation after stale is irreversible, even if identity is restored
 // and a real Update later arrives in the same worker session.
 for(unsigned faultCase=0;faultCase<11;++faultCase){
  prepare({0,0,1});waitMs(130);workerTick(false);assert(state.ownership==Ownership::StaleSuspended);
  auto hero=fakeHero.hero,transform=fakeHero.transform;auto expectedOwner=context.wallRunOwner;auto expectedSession=context.wallRunSessionId;
  if(faultCase==0){context.wallRunOwner+=16;workerTick();}
  if(faultCase==1){fakeHero.hero+=16;workerTick();}
  if(faultCase==2){fakeHero.transform+=64;workerTick();}
  if(faultCase==3){ws::native_source::event(3,owner);workerTick(false);}
  if(faultCase==4){assert(!lifecycleWriteGate.test_and_set());ws::native_source::event(2,owner);lifecycleWriteGate.clear();workerTick();}
  if(faultCase==5){msm2::native_source::kill();workerTick(false);}
  if(faultCase==6){sourceAuth=false;workerTick();sourceAuth=true;}
  if(faultCase==7){auto th=CreateThread(nullptr,0,[](void*)->DWORD{ws::native_source::event(2,owner);return 0;},nullptr,0,nullptr);assert(th&&WaitForSingleObject(th,2000)==WAIT_OBJECT_0);CloseHandle(th);workerTick(false);}
  if(faultCase==8){ws::native_source::event(0,owner);ws::native_source::event(1,owner);workerTick();}
  if(faultCase==9){++lifecycleEpoch;workerTick();}
  if(faultCase==10){++context.wallRunSessionId;workerTick();}
  fakeHero.hero=hero;fakeHero.transform=transform;context.wallRunOwner=expectedOwner;context.wallRunSessionId=expectedSession;
  workerTick();callA();callB();assert(state.ownership==Ownership::HardClosed&&!state.transition.active&&state.staleResumes==0&&pathAWrites==0&&pathBWrites==0);
 }
 prepare({0,0,1});assert(!lifecycleWriteGate.test_and_set());ws::native_source::event(2,owner);lifecycleWriteGate.clear();workerTick();assert(!state.transition.active&&output.find("failing_sub_gate=native_event_collision")!=std::string::npos);
 Token future{};future.phase=3;future.owner=owner;future.updates=1;future.initMs=future.updateMs=GetTickCount64()+1000;assert(!freshToken(future,GetTickCount64()));
 prepare({0,0,1});auto beforeTokenRead=GetTickCount64();for(unsigned i=0;i<100&&GetTickCount64()==beforeTokenRead;++i)Sleep(1);ws::native_source::event(2,owner);Token later{};assert(ws::native_source::lifecycle(later)&&later.updateMs>beforeTokenRead);assert(!freshToken(later,beforeTokenRead)&&freshToken(later,GetTickCount64()));workerTick(false);assert(state.transition.active);
 prepare({0,0,1});deinitOnSample=true;workerTick();assert(!msm2::native_source::active());callA();callB();assert(pathAWrites==0&&pathBWrites==0);context.wallRunActive=false;msm2::native_source::tick(context,"verified_deinit",true);assert(state.summaryEmitted&&state.exitReason==std::string("verified_native_Deinit"));
 prepare({0,0,1});auto oldHero=fakeHero.hero;fakeHero.hero+=16;workerTick();assert(!state.transition.active&&output.find("failing_sub_gate=hero_identity_changed")!=std::string::npos);fakeHero.hero=oldHero;
 prepare({0,0,1});compact.rows[4]=0;auto before=compact;callB();assert(pathBWrites==0&&!std::memcmp(&before,&compact,64));prepare({0,0,1});compact.rows[4]=3;before=compact;callB();assert(pathBWrites==0&&!std::memcmp(&before,&compact,64));
 auto ptr=reinterpret_cast<std::uintptr_t>(&compact);msm2::native_source::captureCopy(fakeHero.transform+16,ptr,0,0,imageBase+0x2cfc84b,ptr-0x28);msm2::native_source::captureCopy(fakeHero.transform,ptr,0,0,imageBase+0x2cfc84c,ptr-0x28);assert(pathBWrites==0);
 prepare({0,0,1});copyAuth=false;callB();copyAuth=true;workerTick();assert(pathBWrites==0&&!msm2::native_source::active());prepare({0,0,1});sdkAvailable=false;callA();sdkAvailable=true;assert(pathAWrites==0);
 prepare({0,0,1});auto thread=CreateThread(nullptr,0,[](void*)->DWORD{callA();callB();return 0;},nullptr,0,nullptr);assert(thread&&WaitForSingleObject(thread,2000)==WAIT_OBJECT_0);CloseHandle(thread);assert(pathAWrites==0&&pathBWrites==0);
 // Known caller-body mismatch and wrong source provenance close ownership;
 // unrelated engine callers remain passthrough and cannot claim this source.
 for(unsigned badEntry=0;badEntry<4;++badEntry){
  prepare({0,0,1});auto src=reinterpret_cast<std::uintptr_t>(&compact);
  if(badEntry==0)msm2::native_source::captureCopy(fakeHero.transform,src,0,0,imageBase+0x2cfc84c,src-0x28);
  if(badEntry==1)msm2::native_source::capture(fakeHero.transform,src,0,0,imageBase+callReturn-1,src-0x28);
  if(badEntry==2)msm2::native_source::captureCopy(fakeHero.transform,src,0,0,imageBase+0x2cfc84b,src-0x30);
  if(badEntry==3)msm2::native_source::capture(fakeHero.transform,src,0,0,imageBase+callReturn,src-0x30);
  workerTick();callA();callB();assert(killed.load()&&!state.transition.active&&pathAWrites==0&&pathBWrites==0);
 }
 prepare({0,0,1});auto src=reinterpret_cast<std::uintptr_t>(&compact);msm2::native_source::captureCopy(fakeHero.transform,src,0,0,imageBase+0x100,src-0x28);assert(!killed.load());callA();callB();assert(pathAWrites==1&&pathBWrites==1);
 prepare({0,0,1});
 auto closed=token;closed.phase=0;msm2::native_source::lifecycleClosed(closed);callA();callB();assert(pathAWrites==0&&pathBWrites==0);
 msm2::native_source::kill();callA();callB();assert(!ws_native_source_enabled&&pathAWrites==0&&pathBWrites==0);
 assert(MH_DisableHook(reinterpret_cast<void*>(&ws_test_copy_writer))==MH_OK);assert(MH_RemoveHook(reinterpret_cast<void*>(&ws_test_copy_writer))==MH_OK);assert(MH_DisableHook(reinterpret_cast<void*>(&ws_test_native_source_writer))==MH_OK);assert(MH_RemoveHook(reinterpret_cast<void*>(&ws_test_native_source_writer))==MH_OK);assert(MH_Uninitialize()==MH_OK);assert(VirtualFree(mem,0,MEM_RELEASE));
 auto f=std::fopen("logs/windows_native_source_synthetic.log","wb");assert(f);std::fwrite(output.data(),1,output.size(),f);std::fclose(f);
 puts("PASS: lock-free coherent A/B shared basis, A exact stack source, B compact36-only scale/metadata preservation, contended worker lock cannot reject hooks, A/E retention, identity/thread/lifecycle/auth/kill/degenerate gates; independent MinHook fixtures only");
 puts("PASS: production native publication/worker and real MinHook A/B commits persist beyond2s through a >=4s WallRun with concurrent reader, SurfaceUp and sustained retention; synchronous Deinit passthrough and once/session lifecycle summary; hard collision/Hero/time-order gates diagnose and remain closed");
 puts("PASS: c7 real270ms stale gap blocks A/B before and after worker poll, same-session real Update resumes full target through verified Deinit; fresh-worker/stale-native gate and missed-worker signal; partial transition freeze/continue, bounded18 episodes/16 pairs, eleven stale-to-hard invalidations never revive");
 puts("PASS: c6 Position stays read-only with explicit proof rejection, authenticated-source provenance records and bounded valid plane samples/one summary; unreadable plane/out-of-range refuse candidate correction without stopping orientation, in-range request<=0.02, Position16 preserved; no plane reads after Deinit/kill");
}
