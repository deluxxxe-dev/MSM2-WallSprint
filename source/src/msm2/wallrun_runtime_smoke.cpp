// Test-only translation unit includes production worker to inject events without
// relaxing any game-build admission guard in the shipped DLL.
#include "wallrun_hooks.cpp"
#include <cassert>
#include <cstdarg>
#include <string>
static std::string output;
namespace msm2 {
std::array<HookCandidate,4> hookCandidates(){return {};}
bool sample(Snapshot&){return false;}
bool readMemory(std::uintptr_t p,void* out,std::size_t size){SIZE_T n=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,size,&n)&&n==size;}
void writeLog(const char* fmt,...){char b[4096]{};va_list v;va_start(v,fmt);vsnprintf(b,sizeof(b),fmt,v);va_end(v);SYSTEMTIME t{};GetSystemTime(&t);char stamp[40]{};std::snprintf(stamp,sizeof(stamp),"%04u-%02u-%02uT%02u:%02u:%02u.%03uZ ",t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,t.wMilliseconds);output+=stamp;output+=b;output+='\n';}
}
static void emit(unsigned kind,std::uintptr_t owner){
 auto n=ws_hook_head.fetch_add(1);auto& slot=ws_hook_slots[n&(ws::capture::capacity-1)];assert(slot.sequence.load()==n);
 slot.tsc=readTsc();slot.kind=kind;slot.self=owner;slot.caller=0;slot.sequence.store(n+1,std::memory_order_release);
}
__attribute__((noinline,used)) float native_source_fixture(const unsigned char* self){
 auto pointer=*reinterpret_cast<const float* const volatile*>(self+0x88);return pointer[0x54/4];
}
int main(){
 // Test-only authenticate private captures when present; never execute them.
 bool privateCaptures=GetFileAttributesA("tools/fixtures/native_v01a7/Init.bin")!=INVALID_FILE_ATTRIBUTES;
 auto admit=[&](unsigned index,bool corrupt=false){
  const auto& meta=ws::static_plan::functions[index];char path[256];std::snprintf(path,sizeof(path),"tools/fixtures/native_v01a7/%s.bin",meta.name);auto input=std::fopen(path,"rb");assert(input||!privateCaptures);
  std::vector<unsigned char> bytes(meta.bytes,0x90);if(input){assert(std::fread(bytes.data(),1,bytes.size(),input)==bytes.size());std::fclose(input);}if(corrupt)bytes.back()^=1;msm2::source::authenticatePlan(meta.name,bytes);
 };
 msm2::source::prepare();for(unsigned i=0;i<3;++i)admit(i);msm2::source::finalize(true,true);assert(!msm2::source::fieldPlanEnabled);admit(3);msm2::source::finalize(true,true);
 if(privateCaptures){assert(msm2::source::fieldPlanEnabled);admit(1,true);msm2::source::finalize(true,true);assert(!msm2::source::fieldPlanEnabled);}
 else {assert(!msm2::source::fieldPlanEnabled);puts("SKIP: private original-byte positive replay unavailable; independent wrong-hash rejection executed; later bounded-reader admission is TEST-ONLY");}
 msm2::source::prepare();msm2::source::analyzeNative("SyntheticSource",reinterpret_cast<std::uintptr_t>(&native_source_fixture));assert(msm2::source::evidence.count(0x88)==1);msm2::source::finalize(true,true);
 ws::capture::initializeQueue();queueInitialized=true;verifiedAvailable=true;observer.configure(true);objectEnabled=true;
 msm2::Snapshot s{};s.hero=1;s.transform=2;s.prefix[0]=s.prefix[5]=s.prefix[10]=1;s.position[0]=10;s.position[1]=20;s.position[2]=30;
 ws::diag::Motion m{};m.epoch=1;m.valid=true;m.velocity={0,20,0};
 auto drain=[&](){msm2::hooks::drain(&s,m,GetTickCount64());};drain();
 alignas(16) unsigned char object[512]{};ws::Vec3 v{0,0,-1};std::memcpy(object+0x40,&v,sizeof(v));auto child=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(child);std::memcpy(child+0x40,&v,sizeof(v));ws::Vec3 contact{9.6f,20,30};std::memcpy(child+0x80,&contact,sizeof(contact));auto childPointer=reinterpret_cast<std::uintptr_t>(child);std::memcpy(object+0x88,&childPointer,8);auto owner=reinterpret_cast<std::uintptr_t>(object);
 emit(2,owner);drain();assert(!observer.context(GetTickCount64()).wallRunActive);
 emit(0,owner);emit(1,owner);emit(2,owner);drain();assert(observer.context(GetTickCount64()).wallRunActive);
 // Wine's coarse GetTickCount64 can advance <100ms after Sleep(110).
 // Wait on the same clock as the production sampling gate, with a bounded deadline.
 auto deadline=GetTickCount64()+1000;while(GetTickCount64()-lastObjectSample<110&&GetTickCount64()<deadline)Sleep(10);
 assert(GetTickCount64()-lastObjectSample>=110);emit(2,owner);drain();assert(objectFields.count()==0&&msm2::source::channels[0].data.count()==2);
 emit(3,owner);drain();assert(!observer.context(GetTickCount64()).wallRunActive);
 assert(output.find("offset=0x40")!=std::string::npos);assert(output.find("wallrun_exit_reason=verified_deinit")!=std::string::npos);
 assert(output.find("session_timeline session_id=1")!=std::string::npos);
 assert(output.find("mean_position=(10.000000,20.000000,30.000000)")!=std::string::npos);
 s.prefix[0]=0;s.prefix[1]=-1;s.prefix[4]=1;s.prefix[5]=0;
 for(unsigned i=0;i<3;++i){Sleep(110);drain();}
 assert(surfaceTracker.context().axisAvailable&&!surfaceTracker.context().valid&&!surfaceTracker.context().pointAvailable&&msm2::source::channels[0].tracker.context().pointAvailable);
 assert(output.find("surface_calibration_pair wallrun_session=1")!=std::string::npos);
 assert(output.find("crawl_detected_position=(10.000000,20.000000,30.000000)")!=std::string::npos);
 emit(2,owner);drain();assert(!observer.context(GetTickCount64()).wallRunActive);
 emit(0,owner);drain();emit(2,owner+16);drain();assert(!observer.context(GetTickCount64()).wallRunActive);
 emit(0,owner);drain();++m.epoch;drain();assert(!observer.context(GetTickCount64()).wallRunActive);
 emit(0,owner);drain();ws_hook_dropped.fetch_add(1);emit(3,owner);drain();assert(!observer.context(GetTickCount64()).wallRunActive);
 emit(2,owner);drain();assert(!observer.context(GetTickCount64()).wallRunActive);
 emit(0,owner);drain();msm2::hooks::stop();assert(!observer.context(GetTickCount64()).wallRunActive);
 assert(output.find("wallrun_exit_reason=script_disable")!=std::string::npos);
 verifiedAvailable=true;observer.configure(true);emit(0,owner);drain();assert(observer.context(GetTickCount64()).wallRunActive);
 installed[0].enabled=true;installed[0].target=object;std::memset(installed[0].patch,0xff,32);lastOwnershipCheck=0;
 drain();assert(!observer.context(GetTickCount64()).wallRunActive&&!verifiedAvailable);
 assert(output.find("hook_ownership_lost")!=std::string::npos);installed[0].enabled=false;
 // c8 actual production detailed API: no early exit hides another hook's state.
 alignas(16) unsigned char ownedPatches[4][32]{};verifiedAvailable=true;msm2::source::fieldPlanEnabled=true;
 for(unsigned i=0;i<4;++i){installed[i].enabled=true;installed[i].target=ownedPatches[i];std::memcpy(installed[i].patch,ownedPatches[i],32);}
 auto auth=msm2::hooks::observationPlanStatus();assert(auth.diagnosticViable()&&msm2::hooks::observationPlanAuthenticated());msm2::hooks::reportObservationPlanStatus(auth);
 for(unsigned i=0;i<4;++i){
  ownedPatches[i][9+i]=1;auto bad=msm2::hooks::observationPlanStatus();assert(!bad.sourceIntegrity()&&bad.hooks[i].readOk&&!bad.hooks[i].match&&bad.hooks[i].firstDifference==9+i&&bad.failedHook()==std::string(names[i]));
  msm2::hooks::reportObservationPlanStatus(bad);msm2::hooks::reportObservationPlanStatus(bad);ownedPatches[i][9+i]=0;msm2::hooks::reportObservationPlanStatus(msm2::hooks::observationPlanStatus());
 }
 installed[1].target=nullptr;auth=msm2::hooks::observationPlanStatus();assert(!auth.hooks[1].readOk&&!auth.hooks[1].match);installed[1].target=ownedPatches[1];
 installed[2].enabled=false;auth=msm2::hooks::observationPlanStatus();assert(!auth.sourceIntegrity()&&auth.hooks[2].readOk&&auth.hooks[2].match);installed[2].enabled=true;
 msm2::source::fieldPlanEnabled=false;auth=msm2::hooks::observationPlanStatus();assert(!auth.diagnosticViable()&&auth.sourceIntegrity());msm2::hooks::reportObservationPlanStatus(auth);
 assert(output.find("OBSERVATION_PLAN_AUTH_FAILURE")!=std::string::npos&&output.find("failed_hook=Update")!=std::string::npos&&output.find("update_first_difference=11")!=std::string::npos);
 for(auto& item:installed)item={};
 // Old enabled bookkeeping cannot bypass fresh AOB/prefix admission on restart.
 for(auto& item:installed)item.enabled=true;
 msm2::hooks::start(true,{});assert(!verifiedAvailable&&!msm2::source::enabled);
 for(auto& item:installed)item={};
 auto f=std::fopen("logs/windows_surface_runtime_synthetic.log","wb");assert(f);assert(std::fwrite(output.data(),1,output.size(),f)==output.size());std::fclose(f);
 unsigned char bounds[1024]{};assert(msm2::source::readable(childPointer,bounds,512,true));assert(!msm2::source::readable(childPointer,bounds,1025,true));assert(!msm2::source::readable(reinterpret_cast<std::uintptr_t>(&native_source_fixture),bounds,16,true));
 DWORD protection=0;assert(VirtualProtect(child,4096,PAGE_READONLY|PAGE_GUARD,&protection));assert(!msm2::source::readable(childPointer,bounds,512,true));assert(VirtualProtect(child,4096,PAGE_READONLY,&protection));assert(msm2::source::readable(childPointer,bounds,512,true));
 // Full production observer with TEST-ONLY admission + synthetic bounded objects.
 msm2::source::prepare();
 if(privateCaptures){for(unsigned i=0;i<4;++i)admit(i);}
 else msm2::source::planAdmitted.fill(true); // TEST ONLY: exercise bounded reader, NOT game/hash evidence.
 msm2::source::evidence[0x128]={0x128,1,0,0x2e0,0};msm2::source::finalize(true,true);assert(msm2::source::fieldPlanEnabled);
 auto root=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(root);std::memcpy(root+0x128,&childPointer,8);std::memcpy(root+0x298,&v,sizeof(v));assert(VirtualProtect(child,4096,PAGE_READWRITE,&protection));std::memcpy(child+0x2d8,&v,sizeof(v));ws::Vec3 pairAxis{1,0,0},pairPoint{9.5f,20,30},pairNear{10,20,30};std::memcpy(root+0x398,&pairAxis,12);std::memcpy(root+0x338,&pairPoint,12);std::memcpy(root+0x320,&pairNear,12);auto rootOwner=reinterpret_cast<std::uintptr_t>(root);
 auto time=GetTickCount64();msm2::source::begin(100,s.hero,s.transform,m.epoch);msm2::source::observe(rootOwner,s,m,time);msm2::source::observe(rootOwner,s,m,time+110);msm2::source::finish(100,double(time+200));
 for(unsigned i=1;i<=3;++i)msm2::source::tick(double(time+200+i*100),time+200+i*100,s,m,false);
 assert(output.find("static_field_sample session=100 scope=this offset=0x298")!=std::string::npos);assert(output.find("static_field_sample session=100 scope=child128 offset=0x2d8")!=std::string::npos);assert(output.find("child_bytes=0x300")!=std::string::npos);
 assert(output.find("surface_pair_sample session=100")!=std::string::npos);assert(output.find("surface_frame_dryrun session=100")!=std::string::npos);assert(output.find("dryrun_adhesion_correction=(0,0,0)")!=std::string::npos);assert(output.find("v01b_gate=WAITING_DIAGONAL_GROUND_TRUTH applied=0")!=std::string::npos);
 msm2::source::begin(101,s.hero,s.transform,m.epoch);msm2::source::observe(rootOwner,s,m,time+700);assert(output.find("persistent_child_across_wallrun_sessions=1")!=std::string::npos);
 // Reader must reject a child block extending into a non-readable next page.
 auto pages=static_cast<unsigned char*>(VirtualAlloc(nullptr,8192,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(pages);assert(VirtualProtect(pages+4096,4096,PAGE_NOACCESS,&protection));auto edge=reinterpret_cast<std::uintptr_t>(pages+4096-0x300);assert(msm2::source::readable(edge,bounds,0x300,true));assert(!msm2::source::readable(edge,bounds,0x301,true));assert(VirtualFree(pages,0,MEM_RELEASE));assert(VirtualFree(root,0,MEM_RELEASE));
 // Refresh the synthetic log after targeted-field/persistence assertions.
 f=std::fopen("logs/windows_surface_runtime_synthetic.log","wb");assert(f);assert(std::fwrite(output.data(),1,output.size(),f)==output.size());std::fclose(f);
 assert(VirtualFree(child,0,MEM_RELEASE));
 std::puts("PASS: c8 actual observation-plan status API reports every verified/field/enabled/read/match gate, independent Init/Entry/Update/Deinit patch IDs/offsets, unreadable/disabled negatives, diagnostic-only field loss and bounded transition diagnostics without full code bytes");
 std::puts("PASS: production one-level source child pairing/point metrics/raw bounds/video timeline and event drain: verified Active/Inactive, Deinit/crawl isolation, bounded field summary, owner/epoch/drop/disable/ownership resets; TEST-ONLY injected events, no game execution");
}
