// TEST ONLY: independent native callers/writers and owned buffers. No game bytes.
#include "../NativeHooks/source_lifecycle.cpp"
#include "../NativeHooks/native_source_override.cpp"
#include <MinHook.h>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <functional>
static unsigned diagnosticStops{},inputStops{};
#ifndef WALLSPRINT_REAL_POSITION_WATCH_TEST
namespace msm2::trace {void kill() noexcept {++diagnosticStops;} void beginResetCopyEntry() noexcept {} void resetCopyEntry(const path_b::Entry&) noexcept {}}
#endif
static unsigned resetRequests{},prearmRequests{};
namespace msm2::reset_trace {
void kill() noexcept {++diagnosticStops;}
bool requestPosition(const Request& r) noexcept {assert(r.position&&r.basis.valid);return true;}
void request(const Request& r) noexcept {assert(r.prearmed&&!r.commitQpc);++prearmRequests;}
bool committed(const Request& r) noexcept {assert(r.commitQpc);++resetRequests;auto now=GetTickCount64();return now>=r.initMs&&now-r.initMs<1500;} // Model the bounded diagnostic ending; source ownership must continue.
}
static msm2::Snapshot fakeHero;static std::string output;static bool sdkAvailable=true,sourceAuth=true,copyAuth=true,compactAuth=true,deinitOnSample=false,pointReadable=true;static unsigned planeReads{};
static unsigned char installedBytes[32]{},copyInstalledBytes[32]{};
extern "C" {
alignas(16) float ws_test_copy_received[16];std::uint64_t ws_test_copy_calls{};
void ws_test_copy_writer();std::uint64_t ws_test_copy_caller(void*,const void*);void ws_reset_copy_thunk();void* ws_reset_copy_original{};std::atomic<unsigned> ws_reset_copy_recording{0};
alignas(16) float ws_test_source_received[16];std::uint64_t ws_test_source_calls{};
std::uint64_t ws_test_stack_caller(void*,const void*);void ws_test_native_source_writer();void ws_source_thunk();void* ws_source_original{};std::atomic<unsigned> ws_source_recording{0};
}
struct CaptureFrame {std::uintptr_t r11,r10,r9,r8,rdx,rcx,rax,flags,ret;};
extern "C" void ws_source_capture(const CaptureFrame* f) noexcept {msm2::native_source::capture(f->rcx,f->rdx,f->r8,f->r9,f->ret,reinterpret_cast<std::uintptr_t>(&f->ret));
#ifdef WALLSPRINT_REAL_POSITION_WATCH_TEST
 msm2::trace::resetEntry(f->rcx,f->rdx,f->r8,f->r9,f->ret,msm2::native_source::entryOverridden(),token.updates,token.generation,token.owner);
#endif
}
extern "C" void ws_reset_copy_capture(const CaptureFrame* f) noexcept {msm2::native_source::captureCopy(f->rcx,f->rdx,f->r8,f->r9,f->ret,reinterpret_cast<std::uintptr_t>(&f->ret));
#ifdef WALLSPRINT_REAL_POSITION_WATCH_TEST
 if(msm2::native_source::entryOverridden())msm2::trace::positionCopyCommit(f->rcx);
#endif
}
namespace msm2::reset_copy {
bool authenticated() noexcept {unsigned char b[32]{};return copyAuth&&msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_copy_writer),b,32)&&!std::memcmp(b,copyInstalledBytes,32);}
bool compactProofAuthenticated() noexcept {return compactAuth&&authenticated();}
}
// Test-only interposition in the existing guarded reader: pause the real hook
// after it built the first compact payload, while the main owner republishes.
static std::atomic<std::uintptr_t> raceSource{0};static std::atomic<unsigned> raceThread{0};static HANDLE racePaused{},raceResume{};static unsigned racePauseAt=2;static bool racePauseTwice=false;
static thread_local unsigned raceGuardReads{};
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){
 if(raceThread.load()==GetCurrentThreadId()&&p==imageBase+callBegin){++raceGuardReads;if(raceGuardReads==racePauseAt||(racePauseTwice&&raceGuardReads==racePauseAt+1)){raceSource=entryEvidence.rdx;SetEvent(racePaused);assert(WaitForSingleObject(raceResume,2000)==WAIT_OBJECT_0);}}
 if(causeState.load()==1){assert(!msm2::native_source::active()&&(killed.load()||!lifecycleOpen.load()));}
 if(p==token.owner+0x338){++planeReads;if(!pointReadable)return false;}SIZE_T done=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&done)&&done==n;}
bool sample(Snapshot& s){s=fakeHero;if(!sdkAvailable)return false;if(deinitOnSample){deinitOnSample=false;ws::native_source::event(3,token.owner);}return readMemory(s.transform,s.prefix,48);}
void writeLog(const char* fmt,...){char b[4096];va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);output+=b;output+='\n';}
}
namespace msm2::input {bool authenticated(){unsigned char b[32]{};return sourceAuth&&msm2::readMemory(reinterpret_cast<std::uintptr_t>(&ws_test_native_source_writer),b,32)&&!std::memcmp(b,installedBytes,32);}}
namespace msm2::input {void kill() noexcept {++inputStops;sourceAuth=false;} bool originalWriterForPositionProof(std::array<unsigned char,584>&){return false;}} // No game bytes or false startup permission in this fixture.
static ws::source_math::Matrix vanilla;static ws::source_math::CompactBlock compact;static std::uintptr_t owner{};static unsigned session{};
static ws::observed::VerifiedWallContext context;
static ws::source_math::Matrix identity(){ws::source_math::Matrix m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;m.v[12]=123;m.v[13]=45;m.v[14]=67;return m;}
static void waitMs(ULONGLONG duration){auto begin=GetTickCount64();do {Sleep(1);}while(GetTickCount64()-begin<duration);}
static msm2::guards::ObservationStatus observationStatus();
static void workerTick(bool advance=true){if(advance)ws::native_source::event(2,owner);context.wallRunUpdateCalls=token.updates;context.elapsed=double(GetTickCount64()-token.initMs)/1000.;WorkerEvidence w{};w.available=true;w.context=context;w.observation=observationStatus();w.inputAuthenticated=sourceAuth;workerEvidence.publish(w);msm2::native_source::tick(context,"none",true);}
static void prepare(ws::Vec3 normal,bool full=true,unsigned backdate=800){
 secondary.seen=0;secondary.admitted=0;secondary.overridden=0;secondary.rejected=0;secondary.firstThread=0;secondary.lastThread=0;secondary.overflowEvents=0;secondary.inflight=0;for(auto& id:secondary.threads)id=0;
 secondary.refreshSeen=0;secondary.refreshAdmitted=0;secondary.refreshIdentity=0;secondary.refreshStale=0;secondary.refreshIntegrity=0;secondary.refreshOther=0;firstSecondaryRefresh.ready=0;
 firstSecondaryOverride.ready=0;firstSecondaryReject.ready=0;pathBReservations=0;compactAuth=true;
 causeState=0;killCalls=0;workerEvidence.publish({});identityPublication.publish({});firstCause={};
 killed=false;admitted=true;ws_native_source_enabled=1;config={};config.dualSource=true;state={};pathAWrites=0;pathBWrites=0;pathARejects=0;pathBRejects=0;resetRequested=false;publishInactive();
 ws::native_source::armLifecycle(true);ws::native_source::event(0,owner);ws::native_source::event(1,owner);token.initMs=GetTickCount64()-backdate;ws::native_source::event(2,owner);
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
static msm2::guards::ObservationStatus observationStatus(){
 msm2::guards::ObservationStatus s{};s.verifiedAvailable=s.fieldPlanEnabled=true;
 for(unsigned i=0;i<4;++i)s.hooks[i]=msm2::guards::inspectPatch(true,lifecyclePatches[i].address,lifecyclePatches[i].bytes.data(),msm2::readMemory);
 return s;
}
static void assertFirst(const char* reason,const char* origin){
 msm2::native_source::drainLogs();assert(causeState==3&&firstCause.reason==std::string(reason)&&firstCause.origin==std::string(origin));
 auto key="NATIVE_SOURCE_HARD_KILL session="+std::to_string(state.session)+" ";assert(countText(output,key)==1);
 msm2::native_source::kill("script_stop","shutdown");msm2::native_source::drainLogs();assert(countText(output,key)==1&&firstCause.reason==std::string(reason));
}
struct SecondaryJob {
 HANDLE ready{},go{};std::uintptr_t target{};unsigned mode{},thread{};bool changed{},race{};ws::source_math::CompactBlock before{},after{};
};
static DWORD WINAPI secondaryJob(void* raw){
 auto& j=*static_cast<SecondaryJob*>(raw);j.thread=GetCurrentThreadId();if(j.race){raceGuardReads=0;raceThread=j.thread;}alignas(16) ws::source_math::CompactBlock local=j.before;
 SetEvent(j.ready);assert(WaitForSingleObject(j.go,2000)==WAIT_OBJECT_0);
 SetLastError(0x12345);
 if(j.mode==0||j.mode==1){
  auto transform=j.target+(j.mode==1?2048:0);
  auto result=ws_test_copy_caller(reinterpret_cast<void*>(transform),&local);assert(result==0xabcdef0011223344ull);
  assert(!std::memcmp(&local,&j.before,64));std::memcpy(&j.after,ws_test_copy_received,64);j.changed=msm2::native_source::entryOverridden();
 }else{
  auto src=reinterpret_cast<std::uintptr_t>(&local);auto rsp=src-0x28;auto ret=imageBase+0x2cfc84b;
  if(j.mode==2)++ret;
  if(j.mode==3)rsp-=8;
  if(j.mode==4){src=reinterpret_cast<std::uintptr_t>(&j.before);rsp=src-0x28;}
  msm2::native_source::captureCopy(j.target,src,0x1234,0x5678,ret,rsp);j.changed=msm2::native_source::entryOverridden();j.after=local;
 }
 assert(GetLastError()==0x12345);return 0;
}
static SecondaryJob secondaryCall(unsigned mode=0,bool refresh=true,const ws::source_math::CompactBlock* input=nullptr,const std::function<void()>& beforeGo={}){
 SecondaryJob job{};job.target=fakeHero.transform;job.mode=mode;job.before=input?*input:compact;job.ready=CreateEventW(nullptr,TRUE,FALSE,nullptr);job.go=CreateEventW(nullptr,TRUE,FALSE,nullptr);assert(job.ready&&job.go);
 auto th=CreateThread(nullptr,0,secondaryJob,&job,0,nullptr);assert(th&&WaitForSingleObject(job.ready,2000)==WAIT_OBJECT_0);
 if(refresh)workerTick();
 if(beforeGo)beforeGo();
 SetEvent(job.go);assert(WaitForSingleObject(th,2000)==WAIT_OBJECT_0);
 CloseHandle(th);CloseHandle(job.go);CloseHandle(job.ready);return job;
}
static SecondaryJob publicationRace(const std::function<void()>& during,const std::function<void()>& second={},const ws::source_math::CompactBlock* input=nullptr){
 SecondaryJob job{};job.target=fakeHero.transform;job.before=input?*input:compact;job.race=true;job.ready=CreateEventW(nullptr,TRUE,FALSE,nullptr);job.go=CreateEventW(nullptr,TRUE,FALSE,nullptr);
 racePaused=CreateEventW(nullptr,FALSE,FALSE,nullptr);raceResume=CreateEventW(nullptr,FALSE,FALSE,nullptr);assert(job.ready&&job.go&&racePaused&&raceResume);racePauseTwice=bool(second);
 auto th=CreateThread(nullptr,0,secondaryJob,&job,0,nullptr);assert(th&&WaitForSingleObject(job.ready,2000)==WAIT_OBJECT_0);workerTick();SetEvent(job.go);
 assert(WaitForSingleObject(racePaused,2000)==WAIT_OBJECT_0);during();SetEvent(raceResume);
 if(second){assert(WaitForSingleObject(racePaused,2000)==WAIT_OBJECT_0);second();SetEvent(raceResume);}
 assert(WaitForSingleObject(th,2000)==WAIT_OBJECT_0);raceThread=0;racePauseTwice=false;
 for(auto handle:{th,job.ready,job.go,racePaused,raceResume})CloseHandle(handle);
 return job;
}
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
#ifdef WALLSPRINT_REAL_POSITION_WATCH_TEST
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);assert(msm2::trace::configureReset(true,base,nt->OptionalHeader.SizeOfImage,true,true));
 ws::native_source::setObserver([](const Token& t) noexcept {if(t.phase!=3){msm2::native_source::lifecycleClosed(t);msm2::trace::cancelReset();}});
 for(ws::Vec3 normal:{ws::Vec3{0,0,1},ws::Vec3{-.70710678f,0,.70710678f}}){
  prepare(normal);while(context.elapsed<1.60){callA();callB();waitMs(16);workerTick();}
  assert(state.pass&&state.positionRequested);Published p{};assert(publication.read(p));msm2::reset_trace::Request r{};r.position=r.prearmed=r.multithread=true;r.session=p.session;r.generation=p.generation;r.initMs=p.initMs;r.hero=p.hero;r.transform=p.transform;r.owner=p.owner;r.thread=p.thread;r.normal=p.surface;r.basis=p.basis;r.acceptedEpoch=p.acceptedEpoch;r.permissionEpoch=p.lifecycleEpoch;assert(msm2::native_source::diagnosticIdentity(r));
  struct Arm {msm2::reset_trace::Request request;bool ok{};} arm{r};
  auto helper=CreateThread(nullptr,0,[](void* raw)->DWORD {auto& a=*static_cast<Arm*>(raw);msm2::Snapshot hero{};assert(msm2::sample(hero));a.ok=msm2::trace::beginPosition(a.request,hero);return 0;},&arm,0,nullptr);assert(helper&&WaitForSingleObject(helper,2000)==WAIT_OBJECT_0&&arm.ok);CloseHandle(helper);
  auto beforeA=pathAWrites.load();for(unsigned i=0;i<12;++i){callA();callB();waitMs(16);workerTick();msm2::Snapshot actual{};assert(msm2::sample(actual)&&ws::dot(ws::diag::actorAxes(actual.prefix).up,normal)>.999f);msm2::trace::drainLogs();}
  assert(pathAWrites>beforeA+10&&causeState==0);auto closer=CreateThread(nullptr,0,[](void*)->DWORD {msm2::trace::pollReset(false,"fixture_end");return 0;},nullptr,0,nullptr);assert(closer&&WaitForSingleObject(closer,2000)==WAIT_OBJECT_0);CloseHandle(closer);msm2::trace::drainLogs();
  for(unsigned fault=0;fault<7;++fault){auto wrong=r;if(fault==0)++wrong.session;if(fault==1)wrong.owner+=16;if(fault==2)wrong.hero+=16;if(fault==3)wrong.transform+=16;if(fault==4)++wrong.thread;if(fault==5)++wrong.generation;if(fault==6)++wrong.permissionEpoch;assert(!msm2::native_source::diagnosticIdentity(wrong));}
  waitMs(130);assert(!msm2::native_source::diagnosticIdentity(r));workerTick();callA();callB();
  do{waitMs(16);workerTick();callA();callB();msm2::trace::drainLogs();}while(context.elapsed<4.1);assert(state.pass&&causeState==0);
  ws::native_source::event(3,owner);assert(!msm2::native_source::diagnosticIdentity(r));context.wallRunActive=false;msm2::native_source::tick(context,"verified_deinit",true);assert(state.summaryEmitted);
 }
 {auto f=std::fopen("logs/windows_position_orientation_integration.log","wb");assert(f);std::fwrite(output.data(),1,output.size(),f);std::fclose(f);}
 assert(output.find("POSITION_WRITE_EVENT ")!=std::string::npos&&output.find("POSITION_WRITER_SUMMARY ")!=std::string::npos&&output.find("cleanup_restored=1 cleanup_pending=0")!=std::string::npos);
 puts("PASS: c11 production c10 primary A/B orientation retains cardinal/diagonal SurfaceUp with real Position hardware watch active, observed unchanged Position rows; diagnostic identity/session/owner/Hero/Transform/thread/generation/epoch/stale/Deinit rejects and normal close, zero Position source writes");return 0;
#endif
 for(ws::Vec3 n:{ws::Vec3{0,0,1},ws::Vec3{-.70710678f,0,.70710678f}}){
  prepare(n);callA();assert(pathAWrites==1);auto a=ws::diag::actorAxes(ws_test_source_received);assert(a.valid&&ws::dot(a.up,n)>.999f);
  auto metadata=compact.metadata;callB();assert(pathBWrites==1&&compact.metadata==metadata);msm2::sample(fakeHero);auto b=ws::diag::actorAxes(fakeHero.prefix);assert(b.valid&&ws::dot(b.up,n)>.999f);
  assert(!stateLock.test_and_set());callA();callB();stateLock.clear();assert(pathAWrites==2&&pathBWrites==2);
  for(unsigned i=0;i<22;++i){callA();callB();workerTick();}
  assert(state.pass&&state.heldUpdates>=20);msm2::native_source::drainLogs();
  assert(output.find("PATH_A_LOCKFREE_SNAPSHOT_READ")!=std::string::npos&&output.find("PATH_B_COMPACT3X3_OVERRIDE")!=std::string::npos&&output.find("bytes_written=0x24 metadata_untouched=1")!=std::string::npos&&output.find("PATH_A_STATE_LOCK_CONTENTION=0")!=std::string::npos&&output.find("ORIENTATION_RETENTION_PASS=1")!=std::string::npos);
 }
 // c8 production worker policy at the measured first-activation boundary.
 // Only fieldPlan changes: diagnostics stop, actual ownership and writers survive.
 prepare({0,0,1},false,700);callA();callB();waitMs(16);workerTick();callA();callB();
 assert(state.lastT>0&&state.lastT<1&&pathAWrites==2&&pathBWrites==2);
 auto obs=observationStatus();assert(obs.diagnosticViable());auto stoppedBefore=diagnosticStops;auto inputBefore=inputStops;
 obs.fieldPlanEnabled=false;assert(!obs.diagnosticViable()&&obs.sourceIntegrity());
 bool guard=msm2::native_source::workerGuards(context,obs);assert(guard&&diagnosticStops>stoppedBefore&&inputStops==inputBefore&&causeState==0&&sourceAuth);
 msm2::native_source::tick(context,"none",guard);
 do {waitMs(16);workerTick();assert(msm2::native_source::workerGuards(context,obs));callA();callB();}while(state.lastT<1);
 for(unsigned i=0;i<23;++i){waitMs(16);workerTick();callA();callB();}
 assert(state.pass&&pathAWrites>25&&pathBWrites>25&&state.ownership==Ownership::Active&&causeState==0);
 // A changed live hook patch is a true ownership failure, independently for all4.
 const char* hookNames[]={"Init","Entry","Update","Deinit"};
 for(unsigned i=0;i<4;++i){
  prepare({0,0,1},false,700);callA();callB();waitMs(16);workerTick();callA();callB();
  auto* changed=reinterpret_cast<unsigned char*>(lifecyclePatches[i].address);changed[7+i]^=1;
  auto bad=observationStatus();assert(!bad.sourceIntegrity()&&bad.failedHook()==std::string(hookNames[i])&&!bad.hooks[i].match&&bad.hooks[i].firstDifference==7+i);
  auto a=pathAWrites.load(),b=pathBWrites.load();assert(!msm2::native_source::workerGuards(context,bad));
  callA();callB();assert(pathAWrites==a&&pathBWrites==b);msm2::native_source::tick(context,"none",false);
  assertFirst("worker_observation_plan_lost","worker");assert(firstCause.guards.hooks[i].firstDifference==7+i&&firstCause.worker.observation.failedHook()==std::string(hookNames[i]));
  changed[7+i]^=1;sourceAuth=true;workerTick();callA();callB();assert(!state.transition.active&&pathAWrites==a&&pathBWrites==b);
 }
 prepare({0,0,1});sourceAuth=false;assert(!msm2::native_source::workerGuards(context,observationStatus()));sourceAuth=true;assertFirst("worker_input_auth_lost","worker");
 // An owned hot entry can beat the worker to the first cause; retain its patch ID.
 prepare({0,0,1});auto* patch=reinterpret_cast<unsigned char*>(lifecyclePatches[2].address);patch[11]^=1;callA();
 assertFirst("path_a_current_guards_lost","path_a");assert(firstCause.guards.failedGate()==std::string("WallRun_Update_patch")&&firstCause.guards.hooks[2].firstDifference==11);patch[11]^=1;
 prepare({0,0,1});copyAuth=false;callB();copyAuth=true;assertFirst("path_b_compact_or_guard_lost","path_b");

 prepare({0,0,1});assert(!lifecycleWriteGate.test_and_set());ws::native_source::event(2,owner);lifecycleWriteGate.clear();callA();assertFirst("entry_token_incoherent","path_a");
 prepare({0,0,1});auto nativeThread=CreateThread(nullptr,0,[](void*)->DWORD{ws::native_source::event(2,owner);return 0;},nullptr,0,nullptr);assert(nativeThread&&WaitForSingleObject(nativeThread,2000)==WAIT_OBJECT_0);CloseHandle(nativeThread);callA();assertFirst("token_identity_epoch_mismatch","path_a");
 // The atomic first-claim record is immutable even for racing explicit kills.
 prepare({0,0,1});auto th=CreateThread(nullptr,0,[](void*)->DWORD{msm2::native_source::kill("racing_owned_fault","path_a");return 0;},nullptr,0,nullptr);assert(th);
 msm2::native_source::kill("F8_kill_switch","shutdown");assert(WaitForSingleObject(th,2000)==WAIT_OBJECT_0);CloseHandle(th);msm2::native_source::drainLogs();
 auto saved=firstCause.reason;msm2::native_source::kill("script_stop","shutdown");assert(firstCause.reason==saved&&killCalls>=3&&causeState==3);
 // The production source token/worker/A-B path runs through a real multi-second
 // interval; diagnostic lifetime is not consulted by this orientation contract.
 ws::native_source::setObserver([](const Token& t) noexcept {if(t.phase!=3)msm2::native_source::lifecycleClosed(t);});
 // Exact c8 sequence: retention, secondary B at~1.75, then primary A/B
 // continue to a normal~4.4s Deinit. Secondary never publishes a native event.
 prepare({0,0,1});auto secondarySession=state.session;auto nativeOwnerThread=state.expectedThread;
 while(context.elapsed<1.75){callA();callB();waitMs(16);workerTick();msm2::native_source::drainLogs();}
 assert(state.pass&&state.lastT==1);auto entry=state.transition.entryMs;auto target=state.transition.targetQ;auto aBefore=pathAWrites.load(),bBefore=pathBWrites.load();
 auto beforeUpdates=token.updates;auto job=secondaryCall(0,false);
 assert(job.changed&&job.thread!=nativeOwnerThread&&job.after.rows[5]>.999f&&job.after.metadata==compact.metadata&&pathAWrites==aBefore&&pathBWrites==bBefore+1);
 assert(causeState==0&&!killed.load()&&state.expectedThread==nativeOwnerThread&&token.thread==nativeOwnerThread&&token.updates==beforeUpdates&&state.transition.entryMs==entry&&!std::memcmp(&state.transition.targetQ,&target,sizeof(target)));
 assert(secondary.seen==1&&secondary.admitted==1&&secondary.overridden==1&&secondary.rejected==0&&secondary.firstThread==job.thread);
 do {waitMs(16);workerTick();callA();callB();msm2::native_source::drainLogs();}while(context.elapsed<4.4);
 assert(pathAWrites>aBefore+100&&pathBWrites>bBefore+100&&state.pass&&state.expectedThread==nativeOwnerThread);
 ws::native_source::event(3,owner);context.wallRunActive=false;msm2::native_source::tick(context,"verified_deinit",true);msm2::native_source::drainLogs();
 auto c9Key="ORIENTATION_LIFECYCLE_SUMMARY session="+std::to_string(secondarySession)+" ";auto c9Summary=output.substr(output.find(c9Key));
 assert(c9Summary.find("source_active_until_deinit=1 verified_deinit=1")!=std::string::npos&&c9Summary.find("first_hard_kill=none")!=std::string::npos);
 auto eventKey="PATH_B_SECONDARY_THREAD_EVENT session="+std::to_string(secondarySession)+" ";assert(countText(output,eventKey)==1&&output.find("action=override reason=secondary_verified")!=std::string::npos);
 // Shared partial interpolation is applied directly, no new delay/transition.
 prepare({0,0,1},false);waitMs(65);workerTick();Published partial{};assert(publication.read(partial)&&partial.t>0&&partial.t<1);auto c9PartialEntry=state.transition.entryMs;
 job=secondaryCall(0,false);assert(job.changed&&std::abs(job.after.rows[4]-partial.basis.up.y)<.0001f&&std::abs(job.after.rows[5]-partial.basis.up.z)<.0001f&&state.lastT==partial.t&&state.transition.entryMs==c9PartialEntry&&state.expectedThread==GetCurrentThreadId());
 // c10 exact diagonal race: a real primary Update/worker publication occurs
 // between the secondary's computed old payload and final publication read.
 const ws::Vec3 diagonal{-.70710678f,0,.70710678f};
 prepare(diagonal);auto raceSession=state.session;
 while(context.elapsed<1.75){callA();callB();waitMs(16);workerTick();msm2::native_source::drainLogs();}
 assert(state.pass&&state.lastT==1);auto raceEntry=state.transition.entryMs;auto raceTarget=state.transition.targetQ;auto raceThreadOwner=state.expectedThread;auto raceA=pathAWrites.load(),raceB=pathBWrites.load();Published newest{};
 job=publicationRace([&]{waitMs(2);workerTick();assert(publication.read(newest));});
 assert(job.changed&&ws::dot(ws::Vec3{job.after.rows[3],job.after.rows[4],job.after.rows[5]},diagonal)>.9999f&&job.after.metadata==compact.metadata);
 assert(causeState==0&&secondary.refreshSeen==1&&secondary.refreshAdmitted==1&&secondary.refreshIdentity==0&&pathAWrites==raceA&&pathBWrites==raceB+1);
 assert(state.expectedThread==raceThreadOwner&&token.thread==raceThreadOwner&&token.updates==newest.nativeUpdate&&state.transition.entryMs==raceEntry&&!std::memcmp(&state.transition.targetQ,&raceTarget,sizeof(raceTarget)));
 do {waitMs(16);workerTick();callA();callB();assert(ws::dot(ws::Vec3{ws_test_copy_received[3],ws_test_copy_received[4],ws_test_copy_received[5]},diagonal)>.9999f);msm2::native_source::drainLogs();}while(context.elapsed<4.2);
 assert(pathAWrites>raceA+100&&pathBWrites>raceB+100&&state.pass);
 ws::native_source::event(3,owner);context.wallRunActive=false;msm2::native_source::tick(context,"verified_deinit",true);msm2::native_source::drainLogs();
 auto c10Key="ORIENTATION_LIFECYCLE_SUMMARY session="+std::to_string(raceSession)+" ";auto c10Summary=output.substr(output.find(c10Key));assert(c10Summary.find("source_active_until_deinit=1 verified_deinit=1")!=std::string::npos&&c10Summary.find("first_hard_kill=none")!=std::string::npos);
 assert(output.find("result=secondary_publication_refresh_admitted")!=std::string::npos);
 auto raceStart=output.find("PATH_B_SECONDARY_THREAD_EVENT session="+std::to_string(raceSession)+" ");auto raceEnd=output.find(c10Key);
 assert(raceStart<raceEnd&&output.substr(raceStart,raceEnd-raceStart).find("actor_up=(0,1,0)")==std::string::npos);
 // New partial basis (not old p.basis) must be used after refresh. Scales and
 // all 28 metadata bytes survive the same production replacement/store helper.
 prepare(diagonal,false);waitMs(35);workerTick();Published oldPartial{};assert(publication.read(oldPartial));
 auto refreshScaled=compact;for(unsigned i=0;i<3;++i){refreshScaled.rows[i]*=.75f;refreshScaled.rows[3+i]*=1.25f;refreshScaled.rows[6+i]*=1.5f;}
 job=publicationRace([&]{waitMs(45);workerTick();assert(publication.read(newest));},{},&refreshScaled);
 assert(job.changed&&newest.t>oldPartial.t&&newest.t<1&&std::abs(job.after.rows[4]-newest.basis.up.y*1.25f)<.0001f&&std::abs(job.after.rows[4]-oldPartial.basis.up.y*1.25f)>.01f&&job.after.metadata==compact.metadata);
 // The revalidation rereads this callback's current stack block, including new
 // metadata, rather than keeping the initial expected block for guarded store.
 prepare(diagonal);auto latestIncoming=compact;for(unsigned i=0;i<3;++i)latestIncoming.rows[3+i]*=1.25f;latestIncoming.metadata[12]^=0x55;
 job=publicationRace([&]{waitMs(1);workerTick();std::memcpy(reinterpret_cast<void*>(raceSource.load()),&latestIncoming,64);});
 assert(job.changed&&job.after.metadata==latestIncoming.metadata&&std::abs(job.after.rows[3]-diagonal.x*1.25f)<.0001f);
 // Even equal timestamps cannot hide an advancing update/basis payload.
 prepare(diagonal);job=publicationRace([&]{Published next{};assert(publication.read(next));++next.nativeUpdate;publication.publish(next);});assert(job.changed&&secondary.refreshAdmitted==1);
 // Strict one-refresh bound: a second genuine refresh rejects without a store.
 prepare(diagonal);job=publicationRace([&]{waitMs(1);workerTick();},[&]{waitMs(1);workerTick();});
 assert(!job.changed&&pathBWrites==0&&pathBReservations==0&&causeState==0&&secondary.refreshOther==1);workerTick();callA();callB();assert(pathAWrites==1&&pathBWrites==1);
 // Real changes during the same validation window never receive orientation.
 // Direct publication mutations isolate each field; token mutations isolate
 // live native checks. Every executed function is still an owned fixture.
 for(unsigned faultCase=0;faultCase<26;++faultCase){
  prepare(diagonal);auto savedHero=fakeHero.hero,savedTransform=fakeHero.transform;unsigned char* mutatedPatch=nullptr;
  job=publicationRace([&]{
   waitMs(1);workerTick();Published next{};assert(publication.read(next));
   if(faultCase==0)++next.session;
   if(faultCase==1)next.owner+=16;
   if(faultCase==2)next.hero+=16;
   if(faultCase==3)next.transform+=64;
   if(faultCase==4)++next.generation;
   if(faultCase==5)++next.lifecycleEpoch;
   if(faultCase==6)++next.acceptedEpoch;
   if(faultCase==7)++next.thread;
   if(faultCase==8)++next.initMs;
   if(faultCase==9)next.active=false;
   if(faultCase<=9)publication.publish(next);
   if(faultCase==10)ws::native_source::event(3,owner);
   if(faultCase>=11&&faultCase<=16){auto changed=token;if(faultCase==11)++changed.thread;if(faultCase==12)changed.owner+=16;if(faultCase==13)++changed.generation;if(faultCase==14)++changed.acceptedEventEpoch;if(faultCase==15)changed.phase=2;if(faultCase==16){assert(!lifecycleWriteGate.test_and_set());ws::native_source::event(2,owner);lifecycleWriteGate.clear();}else lifecyclePublication.publish(changed);}

   if(faultCase==19){waitMs(130);}
   if(faultCase==20){waitMs(130);ws::native_source::event(2,owner);}
   if(faultCase==21)msm2::native_source::kill("F8_kill_switch","shutdown");
   if(faultCase==22)fakeHero.hero+=16;
   if(faultCase==23)fakeHero.transform+=64;
   if(faultCase>=24){WorkerEvidence w{};assert(workerEvidence.read(w));if(faultCase==24)++w.context.wallRunSessionId;else w.context.wallRunOwner+=16;workerEvidence.publish(w);}
  },(faultCase==17||faultCase==18)?std::function<void()>([&]{if(faultCase==17){mutatedPatch=reinterpret_cast<unsigned char*>(lifecyclePatches[2].address);mutatedPatch[9]^=1;}else compactAuth=false;}):std::function<void()>{});
  if(faultCase==17||faultCase==18)assert(secondary.refreshSeen==1&&secondary.refreshIntegrity==1);
  assert(!job.changed&&pathBWrites==0&&pathBReservations==0&&state.expectedThread==GetCurrentThreadId());
  if(faultCase==19||faultCase==20){assert(!killed.load()&&causeState==0&&secondary.refreshStale==1);workerTick(false);workerTick();job=secondaryCall();assert(job.changed);}
  else {if(faultCase==10)assert(causeState==0);else assert(killed.load()&&causeState.load()>=2);}
  if(mutatedPatch)mutatedPatch[9]^=1;
  compactAuth=true;fakeHero.hero=savedHero;fakeHero.transform=savedTransform;
  if(faultCase!=19&&faultCase!=20){workerTick();callA();callB();assert(pathAWrites==0&&pathBWrites==0);}
  msm2::native_source::drainLogs();
 }
 // Refresh evidence is separately capped; first survives a full shared ring.
 prepare(diagonal);head=tail+capacity;
 job=publicationRace([&]{waitMs(1);workerTick();});assert(job.changed&&firstSecondaryRefresh.ready==2);tail=head.load();msm2::native_source::drainLogs();assert(firstSecondaryRefresh.ready==3);
 prepare(diagonal);for(unsigned i=0;i<20;++i){job=publicationRace([&]{waitMs(1);workerTick();});assert(job.changed);}
 msm2::native_source::drainLogs();auto refreshKey="PATH_B_SECONDARY_PUBLICATION_REFRESH_EVENT session="+std::to_string(state.session)+" ";assert(countText(output,refreshKey)==16&&secondary.refreshSeen==20&&secondary.refreshAdmitted==20);
 // Foreign target/caller/relation/span/layout rejects never kill or migrate.
 for(unsigned mode=1;mode<=5;++mode){
  prepare({0,0,1});auto input=compact;if(mode==5)input.rows[4]=0;job=secondaryCall(mode,false,&input);
  assert(!job.changed&&causeState==0&&!killed.load()&&pathBWrites==0&&state.expectedThread==GetCurrentThreadId());
  workerTick();callA();callB();assert(pathAWrites==1&&pathBWrites==1);
  msm2::native_source::drainLogs();const char* reasons[]={"unused","secondary_wrong_target","secondary_wrong_caller","secondary_stack_relation_failed","secondary_source_span_failed","secondary_layout_invalid"};
  auto key="PATH_B_SECONDARY_THREAD_EVENT session="+std::to_string(state.session)+" ";auto event=output.substr(output.find(key));assert(event.find(std::string("action=reject reason=")+reasons[mode])!=std::string::npos);
 }
 // Row scale and all metadata survive secondary policy stores (owned stack only).
 prepare({0,0,1});auto scaled=compact;for(unsigned i=0;i<3;++i){scaled.rows[i]*=.75f;scaled.rows[3+i]*=1.25f;scaled.rows[6+i]*=1.5f;}
 job=secondaryCall(6,false,&scaled);assert(job.changed&&job.after.metadata==scaled.metadata&&std::abs(job.after.rows[5]-1.25f)<.0001f);
 // Proof loss and every real lifecycle patch mutation remain process-fatal.
 for(unsigned i=0;i<5;++i){
  prepare({0,0,1});unsigned char* changed=nullptr;
  job=secondaryCall(0,true,nullptr,[&]{if(i==4)compactAuth=false;else {changed=reinterpret_cast<unsigned char*>(lifecyclePatches[i].address);changed[13+i]^=1;}});assert(!job.changed&&killed.load()&&pathBWrites==0);assertFirst("secondary_source_integrity_lost","path_b");
  if(changed)changed[13+i]^=1;
  compactAuth=true;workerTick();callA();callB();assert(pathAWrites==0&&pathBWrites==0);
 }
 // Fresh Hero/Transform, worker session/owner, generation/accepted epoch/collision
 // are independent requirements. Later primary updates cannot revive failure.
 for(unsigned i=0;i<7;++i){
  prepare({0,0,1});auto hero=fakeHero.hero,transform=fakeHero.transform;
  job=secondaryCall(0,true,nullptr,[&]{
  if(i==0)fakeHero.hero+=16;
  if(i==1)fakeHero.transform+=64;
  if(i==2||i==3){WorkerEvidence w{};assert(workerEvidence.read(w));if(i==2)++w.context.wallRunSessionId;else w.context.wallRunOwner+=16;workerEvidence.publish(w);}
  if(i==4){auto wrong=token;++wrong.generation;lifecyclePublication.publish(wrong);}
  if(i==5){auto wrong=token;++wrong.acceptedEventEpoch;lifecyclePublication.publish(wrong);}
  if(i==6){assert(!lifecycleWriteGate.test_and_set());ws::native_source::event(2,owner);lifecycleWriteGate.clear();}
  });assert(!job.changed&&killed.load()&&pathBWrites==0&&state.expectedThread==GetCurrentThreadId());
  fakeHero.hero=hero;fakeHero.transform=transform;workerTick();callA();callB();assert(pathAWrites==0&&pathBWrites==0);
 }
 // Stale secondary calls signal the same c7 recovery path, never keep it alive.
 prepare({0,0,1});waitMs(270);job=secondaryCall(0,false);assert(!job.changed&&causeState==0&&pathBWrites==0);
 workerTick(false);assert(state.ownership==Ownership::StaleSuspended);job=secondaryCall(0,false);assert(!job.changed&&causeState==0);
 workerTick();assert(state.staleSuspends==1&&state.staleResumes==1);job=secondaryCall();assert(job.changed&&causeState==0&&state.expectedThread==GetCurrentThreadId());
 // Both accepted/rejected evidence caps are process-bounded; counters continue.
 prepare({0,0,1});for(unsigned i=0;i<20;++i){job=secondaryCall(0);assert(job.changed);job=secondaryCall(2);assert(!job.changed);}
 msm2::native_source::drainLogs();auto capKey="PATH_B_SECONDARY_THREAD_EVENT session="+std::to_string(state.session)+" ";assert(countText(output,capKey)==32&&secondary.seen==40&&secondary.overridden==20&&secondary.rejected==20);
 // The common atomic B attempt budget cannot be oversubscribed by threads.
 prepare({0,0,1});std::array<SecondaryJob,8> contenders{};std::array<HANDLE,8> threads{},ready{};auto go=CreateEventW(nullptr,TRUE,FALSE,nullptr);assert(go);
 for(unsigned i=0;i<8;++i){auto& c=contenders[i];c.target=fakeHero.transform;c.mode=6;c.before=compact;c.go=go;c.ready=ready[i]=CreateEventW(nullptr,TRUE,FALSE,nullptr);assert(c.ready);threads[i]=CreateThread(nullptr,0,secondaryJob,&c,0,nullptr);assert(threads[i]);}
 assert(WaitForMultipleObjects(8,ready.data(),TRUE,2000)==WAIT_OBJECT_0);workerTick();pathBReservations=4095;SetEvent(go);
 assert(WaitForMultipleObjects(8,threads.data(),TRUE,2000)==WAIT_OBJECT_0);unsigned winners=0;for(unsigned i=0;i<8;++i){winners+=unsigned(contenders[i].changed);CloseHandle(threads[i]);CloseHandle(ready[i]);}CloseHandle(go);
 unsigned uniqueThreads=0;for(auto& id:secondary.threads)if(id.load())++uniqueThreads;assert(uniqueThreads==8&&secondary.inflight==0);
 assert(winners==1&&pathBReservations==4096&&pathBWrites==1&&secondary.overridden==1&&secondary.rejected==7&&causeState==0);callB();assert(pathBWrites==1);
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
 auto planeBeforeKill=planeReads;msm2::native_source::kill("fixture_kill","shutdown");workerTick(false);assert(planeReads==planeBeforeKill);
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
 if(!state.pass||state.staleSuspends!=1||state.staleResumes!=1||state.maxStaleMs<150)std::fprintf(stderr,"recovery failed: pass=%u suspends=%u resumes=%u max_stale_ms=%llu owner_state=%u first_reason=%s\n",unsigned(state.pass),state.staleSuspends,state.staleResumes,(unsigned long long)state.maxStaleMs,unsigned(state.ownership),firstCause.reason);
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
  if(faultCase==5){msm2::native_source::kill("fixture_kill","shutdown");workerTick(false);}
  if(faultCase==6){sourceAuth=false;workerTick();sourceAuth=true;}
  if(faultCase==7){auto th=CreateThread(nullptr,0,[](void*)->DWORD{ws::native_source::event(2,owner);return 0;},nullptr,0,nullptr);assert(th&&WaitForSingleObject(th,2000)==WAIT_OBJECT_0);CloseHandle(th);workerTick(false);}
  if(faultCase==8){ws::native_source::event(0,owner);ws::native_source::event(1,owner);workerTick();}
  if(faultCase==9){++lifecycleEpoch;workerTick();}
  if(faultCase==10){++context.wallRunSessionId;workerTick();}
  fakeHero.hero=hero;fakeHero.transform=transform;context.wallRunOwner=expectedOwner;context.wallRunSessionId=expectedSession;
  workerTick();callA();callB();assert(state.ownership==Ownership::HardClosed&&!state.transition.active&&state.staleResumes==0&&pathAWrites==0&&pathBWrites==0);
  if(faultCase==3)assert(causeState==0); // Verified Deinit is a normal session close.
  else assert(causeState.load()>=2&&firstCause.source.hero==hero&&firstCause.source.transform==transform);
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
  const char* reasons[]={"path_b_known_caller_mismatch","path_a_known_caller_mismatch","path_b_stack_or_source_layout_failed","path_a_stack_or_source_layout_failed"};assertFirst(reasons[badEntry],badEntry%2?"path_a":"path_b");
 }
 prepare({0,0,1});auto src=reinterpret_cast<std::uintptr_t>(&compact);msm2::native_source::captureCopy(fakeHero.transform,src,0,0,imageBase+0x100,src-0x28);assert(!killed.load());callA();callB();assert(pathAWrites==1&&pathBWrites==1);
 prepare({0,0,1});
 auto closed=token;closed.phase=0;msm2::native_source::lifecycleClosed(closed);callA();callB();assert(pathAWrites==0&&pathBWrites==0);
 msm2::native_source::kill("fixture_kill","shutdown");callA();callB();assert(!ws_native_source_enabled&&pathAWrites==0&&pathBWrites==0);
 // Real elapsed observation/turn time exercises publication and MinHook A/B stores. No backdated candidate clocks.
 prepare({0,0,1});assert(state.heading.ready);waitMs(34);
 fakeHero.position[0]-=.5f;workerTick();assert(state.heading.pendingCount==1&&!state.heading.turning);
 waitMs(34);
 fakeHero.position[0]-=.5f;workerTick();assert(state.heading.turning);
 while(state.headingClock<state.heading.turnStart+50&&!killed.load()){waitMs(16);workerTick();}assert(state.heading.turning);
 Published headingMid{};assert(publication.read(headingMid)&&headingMid.active);assert(ws::dot(headingMid.basis.up,ws::Vec3{0,0,1})>.9999f);
 callA();callB();for(unsigned i=0;i<3;++i){ws::Vec3 v=i==0?headingMid.basis.right:i==1?headingMid.basis.up:headingMid.basis.forward;
  assert(std::abs(ws_test_source_received[4*i]-v.x)<.0001f&&std::abs(ws_test_source_received[4*i+1]-v.y)<.0001f&&std::abs(ws_test_source_received[4*i+2]-v.z)<.0001f);
  assert(std::abs(ws_test_copy_received[3*i]-v.x)<.0001f&&std::abs(ws_test_copy_received[3*i+1]-v.y)<.0001f&&std::abs(ws_test_copy_received[3*i+2]-v.z)<.0001f);}
 auto frozenClock=state.headingClock;waitMs(120);workerTick(false);assert(state.headingWallMs==0);fakeHero.position[0]+=100;workerTick();
 assert(state.headingClock==frozenClock&&!state.heading.havePosition&&state.heading.turning);
 while(state.heading.turning&&!killed.load()){waitMs(16);workerTick();}Published headingEnd{};assert(publication.read(headingEnd));
 assert(ws::dot(headingEnd.basis.forward,state.heading.f0)<-.99999f&&!state.heading.turning);callA();callB();
 assert(!std::memcmp(ws_test_source_received+12,vanilla.v+12,16));
 ws::native_source::event(3,owner);context.wallRunActive=false;workerTick(false);
 puts("PASS: r3 real-time confirmed reversal publishes one coherent midpoint/final basis through real A/B stores, preserves Position/W and compact metadata, freezes turn and clears displacement across stale resume, closes on Deinit");
 assert(MH_DisableHook(reinterpret_cast<void*>(&ws_test_copy_writer))==MH_OK);assert(MH_RemoveHook(reinterpret_cast<void*>(&ws_test_copy_writer))==MH_OK);assert(MH_DisableHook(reinterpret_cast<void*>(&ws_test_native_source_writer))==MH_OK);assert(MH_RemoveHook(reinterpret_cast<void*>(&ws_test_native_source_writer))==MH_OK);assert(MH_Uninitialize()==MH_OK);assert(VirtualFree(mem,0,MEM_RELEASE));
 auto f=std::fopen("logs/windows_native_source_synthetic.log","wb");assert(f);std::fwrite(output.data(),1,output.size(),f);std::fclose(f);
 puts("PASS: lock-free coherent A/B shared basis, A exact stack source, B compact36-only scale/metadata preservation, contended worker lock cannot reject hooks, A/E retention, identity/thread/lifecycle/auth/kill/degenerate gates; independent MinHook fixtures only");
 puts("PASS: production native publication/worker and real MinHook A/B commits persist beyond2s through a >=4s WallRun with concurrent reader, SurfaceUp and sustained retention; synchronous Deinit passthrough and once/session lifecycle summary; hard collision/Hero/time-order gates diagnose and remain closed");
 puts("PASS: c10 deterministic secondary publication refresh at diagonal t1/retention and latest partial basis, fixed native ownership with primary continuation/clean4.2s Deinit; same-ms update, one-refresh bound, 26 ownership/token/Deinit/integrity/stale/kill/context races, no hard revival, first ring-independent refresh event and16-record cap");
 puts("PASS: c9 real secondary-thread MinHook B at1.75s after full retention, unchanged native thread/token/session/transition, primary A-B continue to clean4.4s Deinit; partial progress, foreign negatives, proof/four-patch/Hero/Transform/session/owner/generation/epoch/collision hard gates, stale recovery, scale/metadata, bounded32 events and shared B budget; Path A wrong-thread still fatal");
 puts("PASS: c8 production worker diagnostic-only field-plan loss at first0.7s A/B interpolation stops diagnostics without killing source, reaches t1 and retention; four independent real patch mutations stop A/B with precise first hook/offset, hot-entry and input attribution, immutable/racing first cause and no hard-close revival");
 puts("PASS: c7 real270ms stale gap blocks A/B before and after worker poll, same-session real Update resumes full target through verified Deinit; fresh-worker/stale-native gate and missed-worker signal; partial transition freeze/continue, bounded18 episodes/16 pairs, eleven stale-to-hard invalidations never revive");
 puts("PASS: c6 Position stays read-only with explicit proof rejection, authenticated-source provenance records and bounded valid plane samples/one summary; unreadable plane/out-of-range refuse candidate correction without stopping orientation, in-range request<=0.02, Position16 preserved; no plane reads after Deinit/kill");
}
