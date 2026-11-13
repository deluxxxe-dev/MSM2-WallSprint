#include <windows.h>
#include <MinHook.h>
#include <x86intrin.h>
#include <cpuid.h>
#include <array>
#include <cstring>
#include <algorithm>
#include <map>
#include "msm2/object_observation.hpp"
#include "wallrun_observer.hpp"
#include "object_candidates.hpp"
#include "surface_calibration.hpp"
#include "msm2/surface_source.hpp"
#include "msm2/actor_writer.hpp"
#include "msm2/wallrun_hooks.hpp"
#include "msm2/hook_capture.hpp"
namespace {
using msm2::writeLog;
const char* names[]={"Init","Entry","Update","Deinit"};
constexpr unsigned char prefixes[4][16]={
 {0x48,0x89,0x5c,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0xe8,0x6e,0xe1},
 {0x48,0x8b,0xc4,0x48,0x89,0x58,0x18,0x48,0x89,0x70,0x20,0x57,0x41,0x54,0x41,0x55},
 {0x48,0x8b,0xc4,0x55,0x53,0x56,0x57,0x41,0x56,0x41,0x57,0x48,0x8d,0xa8,0xf8,0xfe},
 {0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0xe8,0x62,0xaf,0xd8,0xff,0x48,0x8b}};
struct Installed {void* target{};bool created{},enabled{};unsigned char native[32]{},patch[32]{};};
Installed installed[4];bool initialized{},queueInitialized{},resident{},verifiedAvailable{},objectEnabled{};
ws::observed::Observer observer;ws::discovery::Candidates objectFields;ws::calibration::Tracker surfaceTracker;
std::atomic<unsigned> lastNativeThread{0};
std::uint64_t lastOwnershipCheck{},lastObjectSample{},resetFloorTsc{};
struct Clock {std::uint64_t ticks{};double ms{},ticksPerMs{};bool calibrated{};} clockMap;
struct Session {
 std::uint64_t id{},calls[4]{},videoSamples{},videoFirstMs{},videoLastMs{};double first{},last{},lastSample{},positionSum[3]{};
 msm2::Snapshot videoFirst{},videoLast{};unsigned subtypeSamples[4]{};
};
std::map<std::uintptr_t,Session> sessions;std::uint64_t lastEpoch{},lastDrops{};
std::uint64_t readTsc(){_mm_lfence();return __rdtsc();}
void calibrate(){
 clockMap={};unsigned a,b,c,d;
 bool invariant=__get_cpuid(0x80000007,&a,&b,&c,&d)&&(d&(1u<<8));
 LARGE_INTEGER f{},q0{},q1{};QueryPerformanceFrequency(&f);QueryPerformanceCounter(&q0);auto t0=readTsc();Sleep(25);QueryPerformanceCounter(&q1);auto t1=readTsc();
 if(invariant&&f.QuadPart>0&&q1.QuadPart>q0.QuadPart&&t1>t0){clockMap.ticksPerMs=double(t1-t0)/(1000.*double(q1.QuadPart-q0.QuadPart)/double(f.QuadPart));clockMap.ticks=readTsc();clockMap.ms=double(GetTickCount64());clockMap.calibrated=clockMap.ticksPerMs>1000;}
 msm2::writeLog("hook_clock=%s ticks_per_ms=%.3f timestamp_delivery=worker_UTC event_monotonic=calibrated_estimate_not_native_timer",clockMap.calibrated?"invariant_TSC_calibrated_to_QPC":"receipt_time_fallback",clockMap.ticksPerMs);
}
double eventMs(std::uint64_t tsc,std::uint64_t received){
 if(!clockMap.calibrated)return double(received);
 double delta=tsc>=clockMap.ticks?double(tsc-clockMap.ticks):-double(clockMap.ticks-tsc);
 double result=clockMap.ms+delta/clockMap.ticksPerMs;
 if(!std::isfinite(result)||result<0||std::abs(result-double(received))>10000)return double(received);
 return result;
}
bool nativeSpan(std::uintptr_t target){
 DWORD64 image=0;auto* entry=RtlLookupFunctionEntry(target,&image,nullptr);
 auto game=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
 if(!entry||image!=game||image+entry->BeginAddress!=target||entry->EndAddress<=entry->BeginAddress||entry->EndAddress-entry->BeginAddress<32)return false;
 msm2::writeLog("hook_boundary begin_RVA=0x%lx end_RVA=0x%lx source=AMD64_runtime_function_table",entry->BeginAddress,entry->EndAddress);return true;
}
void videoSample(Session& s,const msm2::Snapshot& snapshot,const ws::diag::Motion& motion,std::uint64_t ms){
 if(s.videoSamples&&ms<=s.videoLastMs)return;
 ws::Vec3 p{snapshot.position[0],snapshot.position[1],snapshot.position[2]};if(!ws::finite(p))return;
 if(!s.videoSamples){s.videoFirst=snapshot;s.videoFirstMs=ms;}s.videoLast=snapshot;s.videoLastMs=ms;++s.videoSamples;
 for(unsigned i=0;i<3;++i)s.positionSum[i]+=snapshot.position[i];
 auto type=ws::observed::subtype(motion.velocity,motion.valid);unsigned index=std::strcmp(type,"WallRunUp")==0?0:std::strcmp(type,"WallRunSide")==0?1:std::strcmp(type,"Mixed")==0?2:3;++s.subtypeSamples[index];
}
void utcEstimate(double monotonicMs,char (&out)[40]){
 FILETIME file{};GetSystemTimeAsFileTime(&file);ULARGE_INTEGER ticks{};ticks.LowPart=file.dwLowDateTime;ticks.HighPart=file.dwHighDateTime;
 auto shift=static_cast<long long>(std::llround((monotonicMs-double(GetTickCount64()))*10000.));
 ticks.QuadPart=static_cast<ULONGLONG>(static_cast<long long>(ticks.QuadPart)+shift);file.dwLowDateTime=ticks.LowPart;file.dwHighDateTime=ticks.HighPart;SYSTEMTIME t{};
 if(!FileTimeToSystemTime(&file,&t)){std::strcpy(out,"unavailable");return;}
 std::snprintf(out,sizeof(out),"%04u-%02u-%02uT%02u:%02u:%02u.%03uZ",t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,t.wMilliseconds);
}
void videoSummary(std::uintptr_t owner,const Session& s,double end,const char* reason){
 if(!s.videoSamples)return;
 char first[40]{},last[40]{};utcEstimate(s.first,first);utcEstimate(end,last);
 const char* type=s.subtypeSamples[0]>s.subtypeSamples[1]&&s.subtypeSamples[0]>s.subtypeSamples[2]?"WallRunUp":s.subtypeSamples[1]>s.subtypeSamples[0]&&s.subtypeSamples[1]>s.subtypeSamples[2]?"WallRunSide":s.subtypeSamples[2]||s.subtypeSamples[0]||s.subtypeSamples[1]?"Mixed":"Unavailable";
 writeLog("session_timeline session_id=%llu start_utc=%s end_utc=%s duration=%.6f owner=0x%llx candidate=%s heuristic_sample_counts_Up_Side_Mixed_Unavailable=(%u,%u,%u,%u) start_position=(%.6f,%.6f,%.6f) mean_position=(%.6f,%.6f,%.6f) end_position=(%.6f,%.6f,%.6f) worker_samples=%llu start_snapshot_ms=%llu end_snapshot_ms=%llu end_reason=%s event_UTC=estimated_from_TSC_and_worker_clock positions=worker_samples_NOT_exact_hook_frame source=position_difference_heuristic",static_cast<unsigned long long>(s.id),first,last,std::max(0.,end-s.first)/1000.,static_cast<unsigned long long>(owner),type,s.subtypeSamples[0],s.subtypeSamples[1],s.subtypeSamples[2],s.subtypeSamples[3],s.videoFirst.position[0],s.videoFirst.position[1],s.videoFirst.position[2],s.positionSum[0]/s.videoSamples,s.positionSum[1]/s.videoSamples,s.positionSum[2]/s.videoSamples,s.videoLast.position[0],s.videoLast.position[1],s.videoLast.position[2],static_cast<unsigned long long>(s.videoSamples),static_cast<unsigned long long>(s.videoFirstMs),static_cast<unsigned long long>(s.videoLastMs),reason);
 for(unsigned phase=0;phase<2;++phase){auto axes=ws::diag::actorAxes(phase?s.videoLast.prefix:s.videoFirst.prefix);
  writeLog("session_timeline_axes session_id=%llu phase=%s valid=%u actor_right=(%.6f,%.6f,%.6f) actor_up=(%.6f,%.6f,%.6f) actor_forward=(%.6f,%.6f,%.6f) source=worker_snapshot_NOT_exact_native_event",static_cast<unsigned long long>(s.id),phase?"end":"start",unsigned(axes.valid),axes.right.x,axes.right.y,axes.right.z,axes.up.x,axes.up.y,axes.up.z,axes.forward.x,axes.forward.y,axes.forward.z);
 }
}
void summary(std::uintptr_t owner,const Session& s,double end,const char* reason){
 double duration=std::max(0.,end-s.first)/1000.;
 msm2::writeLog("WallRun verified session #%llu this=0x%llx Init=%llu Entry=%llu Update=%llu Deinit=%llu first_ms=%.3f last_ms=%.3f duration=%.6fs update_calls_per_sec=%.3f end_reason=%s source=verified_wallrun_hook semantic_identity=VERIFIED_WALLRUN_LIFECYCLE_FOR_BUILD_2_810_0_0",static_cast<unsigned long long>(s.id),static_cast<unsigned long long>(owner),static_cast<unsigned long long>(s.calls[0]),static_cast<unsigned long long>(s.calls[1]),static_cast<unsigned long long>(s.calls[2]),static_cast<unsigned long long>(s.calls[3]),s.first,s.last,duration,duration>0?s.calls[2]/duration:0,reason);
 videoSummary(owner,s,end,reason);
}

void objectSummary(const char* reason){
 auto ctx=observer.context(double(GetTickCount64()));
 surfaceTracker.summarize(objectFields,ctx.wallRunSessionId,reason,writeLog);
}

void clearSessions(const char* reason){objectSummary(reason);for(const auto& row:sessions)summary(row.first,row.second,row.second.last,reason);sessions.clear();objectFields={};surfaceTracker.reset(reason,writeLog);msm2::source::reset(reason);observer.reset(reason);}
void stateLog(double now){auto c=observer.context(now);
 writeLog("automatic_wallrun=%s lifecycle_verified=%u wallrun_session=%llu wallrun_owner=0x%llx wallrun_elapsed=%.6f update_calls=%u source=%s wallrun_exit_reason=%s surface_valid=0 surface_point=unavailable surface_normal=unavailable surface_distance=unavailable",verifiedAvailable?(c.wallRunActive?"Active":"Inactive"):"Unavailable",unsigned(c.lifecycleVerified),static_cast<unsigned long long>(c.wallRunSessionId),static_cast<unsigned long long>(c.wallRunOwner),c.elapsed,c.wallRunUpdateCalls,verifiedAvailable?"verified_wallrun_hook":"guards_or_complete_hook_set_unavailable",observer.exitReason());
}

}
namespace msm2::hooks {
std::array<msm2::writer::Patch,4> patchSnapshot(){std::array<msm2::writer::Patch,4> result{};for(unsigned i=0;i<4;++i){result[i].address=reinterpret_cast<std::uintptr_t>(installed[i].target);std::memcpy(result[i].bytes.data(),installed[i].patch,32);}return result;}
guards::ObservationStatus observationPlanStatus(){
 guards::ObservationStatus s{};s.verifiedAvailable=verifiedAvailable;s.fieldPlanEnabled=msm2::source::fieldPlanEnabled;
 for(unsigned i=0;i<4;++i)s.hooks[i]=guards::inspectPatch(installed[i].enabled,reinterpret_cast<std::uintptr_t>(installed[i].target),installed[i].patch,msm2::readMemory);
 return s;
}
void reportObservationPlanStatus(const guards::ObservationStatus& s){
 // Worker-only, independent of side-effect-free status reads. Log changes only.
 static bool seen=false;static unsigned previous=0,changes=0;static std::array<unsigned,4> differences{};
 unsigned mask=s.failureMask();std::array<unsigned,4> current{};for(unsigned i=0;i<4;++i)current[i]=s.hooks[i].firstDifference;
 if(seen&&mask==previous&&differences==current)return;
 bool failure=mask!=0;seen=true;previous=mask;differences=current;
 if(changes++>=16)return;
 auto ctx=observer.context(double(GetTickCount64()));
 writeLog("%s verified_available=%u field_plan_enabled=%u init_enabled=%u init_read_ok=%u init_patch_match=%u init_first_difference=%u entry_enabled=%u entry_read_ok=%u entry_patch_match=%u entry_first_difference=%u update_enabled=%u update_read_ok=%u update_patch_match=%u update_first_difference=%u deinit_enabled=%u deinit_read_ok=%u deinit_patch_match=%u deinit_first_difference=%u source_integrity=%u diagnostic_viable=%u failed_hook=%s failure_mask=0x%x wallrun_active=%u wallrun_session=%llu transition_record=%u record_cap=16 mismatch_offset_sentinel=32",
  failure?"OBSERVATION_PLAN_AUTH_FAILURE":"OBSERVATION_PLAN_AUTH_STATUS",unsigned(s.verifiedAvailable),unsigned(s.fieldPlanEnabled),
  unsigned(s.hooks[0].enabled),unsigned(s.hooks[0].readOk),unsigned(s.hooks[0].match),s.hooks[0].firstDifference,
  unsigned(s.hooks[1].enabled),unsigned(s.hooks[1].readOk),unsigned(s.hooks[1].match),s.hooks[1].firstDifference,
  unsigned(s.hooks[2].enabled),unsigned(s.hooks[2].readOk),unsigned(s.hooks[2].match),s.hooks[2].firstDifference,
  unsigned(s.hooks[3].enabled),unsigned(s.hooks[3].readOk),unsigned(s.hooks[3].match),s.hooks[3].firstDifference,
  unsigned(s.sourceIntegrity()),unsigned(s.diagnosticViable()),s.failedHook(),mask,unsigned(ctx.wallRunActive),(unsigned long long)ctx.wallRunSessionId,changes);
}
bool observationPlanAuthenticated(){return observationPlanStatus().diagnosticViable();}
const char* exitReason(){return observer.exitReason();}
unsigned nativeThreadId(){return lastNativeThread.load();}
void start(bool targetBuild,const Options& options){
 lastNativeThread=0;clearSessions("restart");msm2::source::prepare();verifiedAvailable=false;observer.configure(false);objectEnabled=options.objectDiagnostic;msm2::source::runtimeObservationEnabled=options.surfaceTelemetry;lastEpoch=0;lastDrops=ws::capture::dropped();lastObjectSample=lastOwnershipCheck=resetFloorTsc=0;calibrate();
 if(!targetBuild||!options.enabled){writeLog("wallrun_hooks=disabled reason=%s",!targetBuild?"build_gate_rejected":"INI_off");return;}
 if(!queueInitialized){ws::capture::initializeQueue();queueInitialized=true;}
 if(!initialized){auto code=MH_Initialize();if(code!=MH_OK){writeLog("MinHook initialize failed code=%d; no hooks",int(code));return;}initialized=true;}
 std::array<bool,4> admittedThisStart{};
 auto targets=hookCandidates();void (*thunks[])()={ws_hook_init,ws_hook_entry,ws_hook_update,ws_hook_deinit};
 ws_hook_recording.store(1,std::memory_order_release);
 for(unsigned i=0;i<4;++i){
  if(!options.individual[i]){writeLog("hook WallRun%s disabled reason=individual_INI",names[i]);continue;}
  auto candidate=targets[i];auto& item=installed[i];unsigned char native[32]{};
  if(candidate.matches!=1||!candidate.address||!readMemory(candidate.address,native,sizeof(native))||std::memcmp(native,prefixes[i],16)!=0||!nativeSpan(candidate.address)){
   writeLog("hook WallRun%s rejected matches=%zu prefix_or_function_boundary_failed=1 original_not_patched=1",names[i],candidate.matches);continue;
  }
  if(item.created&&item.target!=reinterpret_cast<void*>(candidate.address)){writeLog("hook WallRun%s rejected reason=target_changed_since_creation",names[i]);continue;}
  if(!resident){HMODULE pinned=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&start),&pinned)){writeLog("hook residency pin failed; no hooks installed");break;}resident=true;writeLog("hook_code_resident_until_process_exit=1 trampolines_retained_for_inflight_tailjumps=1");}
  if(objectEnabled)msm2::source::analyzeNative(names[i],candidate.address);
  if(!item.created){
   writeLog("hook WallRun%s BEFORE_CREATE target=0x%llx",names[i],static_cast<unsigned long long>(candidate.address));
   auto code=MH_CreateHook(reinterpret_cast<void*>(candidate.address),reinterpret_cast<void*>(thunks[i]),&ws_hook_originals[i]);
   writeLog("hook WallRun%s AFTER_CREATE status=%d",names[i],int(code));
   if(code!=MH_OK)continue;
   item.target=reinterpret_cast<void*>(candidate.address);item.created=true;std::memcpy(item.native,native,32);
  }
  unsigned char check[32]{};
  if(!readMemory(candidate.address,check,sizeof(check))||std::memcmp(check,item.native,sizeof(check))!=0){writeLog("hook WallRun%s rejected reason=prefix_changed_before_enable",names[i]);continue;}
  writeLog("hook WallRun%s BEFORE_ENABLE original_call=%s args_returns=win64_preserved no_typed_game_signature=1",names[i],i==2?"guarded_post_original_wrapper_or_tailjump":"tailjump");
  auto code=MH_EnableHook(item.target);item.enabled=code==MH_OK;
  writeLog("hook WallRun%s AFTER_ENABLE status=%d installed=%u",names[i],int(code),unsigned(item.enabled));
  if(item.enabled){if(!readMemory(candidate.address,item.patch,sizeof(item.patch))){MH_DisableHook(item.target);item.enabled=false;writeLog("hook WallRun%s disabled reason=cannot_capture_patch_ownership",names[i]);}else {admittedThisStart[i]=true;writeLog("hook WallRun%s installed semantic_identity=%s pending_complete_hook_set=1",names[i],ws::observed::semantic);}}
 }
 verifiedAvailable=targetBuild;for(bool admitted:admittedThisStart)verifiedAvailable=verifiedAvailable&&admitted;
 observer.configure(verifiedAvailable);msm2::source::finalize(verifiedAvailable,objectEnabled);
 reportObservationPlanStatus(observationPlanStatus());
 std::array<msm2::writer::Patch,4> writerPatches{};for(unsigned i=0;i<4;++i){writerPatches[i].address=reinterpret_cast<std::uintptr_t>(installed[i].target);std::memcpy(writerPatches[i].bytes.data(),installed[i].patch,32);}
 msm2::writer::admit(verifiedAvailable&&msm2::source::fieldPlanEnabled,writerPatches);writeLog("wallrun_lifecycle_verified=%u required=complete_four_guarded_hooks semantic_identity=%s object_diagnostic=%u",unsigned(verifiedAvailable),ws::observed::semantic,unsigned(objectEnabled));
}
void drain(const Snapshot* snapshot,const ws::diag::Motion& motion,std::uint64_t snapshotMs){
 auto now=GetTickCount64();
 if(motion.epoch!=lastEpoch){
  clearSessions("Hero_or_Transform_changed");lastEpoch=motion.epoch;resetFloorTsc=readTsc();
  ws::capture::Event old{};while(ws::capture::pop(old)){} // no previous-epoch event can start a new session
  stateLog(double(now));
 }
 if(now-lastOwnershipCheck>=500){
  lastOwnershipCheck=now;
  for(auto& item:installed)if(item.enabled){unsigned char patch[32]{};
   if(!readMemory(reinterpret_cast<std::uintptr_t>(item.target),patch,32)||std::memcmp(patch,item.patch,32)!=0){
    reportObservationPlanStatus(observationPlanStatus());
    clearSessions("hook_ownership_lost");verifiedAvailable=false;observer.configure(false);ws_hook_recording.store(0,std::memory_order_release);stateLog(double(now));break;
   }
  }
 }
 auto drops=ws::capture::dropped();if(drops!=lastDrops){
  writeLog("hook_queue_dropped=%llu state_reset=1 reason=incomplete_lifecycle",static_cast<unsigned long long>(drops));lastDrops=drops;
  clearSessions("hook_event_loss");resetFloorTsc=readTsc();ws::capture::Event discarded{};while(ws::capture::pop(discarded)){}stateLog(double(now));return;
 }
 ws::capture::Event e{};unsigned count=0;
 while(count++<ws::capture::capacity&&ws::capture::pop(e)){
  if(e.kind>=4||!verifiedAvailable||!snapshot||e.tsc<=resetFloorTsc)continue;
  double when=eventMs(e.tsc,now);ws::capture::observeFaceEvent(e,when);auto before=observer.context(when);
  if(e.kind==0&&before.wallRunActive){clearSessions(e.self==before.wallRunOwner?"unexpected_duplicate_init":"candidate_owner_mismatch");resetFloorTsc=readTsc();stateLog(when);continue;}
  if(before.wallRunActive&&e.self!=before.wallRunOwner){clearSessions("candidate_owner_mismatch");resetFloorTsc=readTsc();stateLog(when);continue;}
  if(e.kind==3&&before.wallRunActive){auto it=sessions.find(e.self);if(it!=sessions.end())videoSample(it->second,*snapshot,motion,snapshotMs);objectSummary("verified_deinit");surfaceTracker.finish(objectFields,before.wallRunSessionId,when,writeLog);msm2::source::finish(before.wallRunSessionId,when);}
  if(!observer.event(e.kind,e.self,when))continue;
  if(e.thread&&e.kind==2)lastNativeThread=e.thread;
  auto ctx=observer.context(when);
  bool resumed=observer.takeResumed();
  if(resumed)writeLog("WALLRUN_SESSION_RESUMED wallrun_session=%llu this=0x%llx reason=update_after_emergency_timeout_same_owner_no_deinit",static_cast<unsigned long long>(ctx.wallRunSessionId),static_cast<unsigned long long>(e.self));
  if(e.kind==0||resumed){objectFields={};lastObjectSample=0;Session s{};s.id=ctx.wallRunSessionId;s.first=when;sessions[e.self]=s;surfaceTracker.begin(ctx.wallRunSessionId,snapshot->hero,snapshot->transform,motion.epoch,writeLog);msm2::source::begin(ctx.wallRunSessionId,snapshot->hero,snapshot->transform,motion.epoch);}
  auto it=sessions.find(e.self);if(it==sessions.end())continue;auto& s=it->second;
  ++s.calls[e.kind];s.last=std::max(s.last,when);videoSample(s,*snapshot,motion,snapshotMs);
  if(e.kind!=2||when-s.lastSample>=500){s.lastSample=when;
   writeLog("WALLRUN_HOOK %s wallrun_session=%llu event_monotonic_ms_est=%.3f raw_TSC=%llu received_monotonic_ms=%llu this=0x%llx caller_return=0x%llx semantic_identity=%s",names[e.kind],static_cast<unsigned long long>(s.id),when,static_cast<unsigned long long>(e.tsc),static_cast<unsigned long long>(now),static_cast<unsigned long long>(e.self),static_cast<unsigned long long>(e.caller),ws::observed::semantic);
   stateLog(when);
   auto axes=ws::diag::actorAxes(snapshot->prefix);
   writeLog("hook_axes valid=%u R0=(%.6f,%.6f,%.6f) R1=(%.6f,%.6f,%.6f) R2=(%.6f,%.6f,%.6f) normal_hypothesis_up=-R2 side=+R0_or_-R0 NOT_surface_measurement=1 snapshot_source=worker_sample",unsigned(axes.valid),axes.right.x,axes.right.y,axes.right.z,axes.up.x,axes.up.y,axes.up.z,axes.forward.x,axes.forward.y,axes.forward.z);
   writeLog("wallrun_subtype candidate=%s source=position_difference_heuristic vertical_speed=%.6f horizontal_speed=%.6f NOT_physics_velocity NOT_control_state=1 snapshot_monotonic_ms=%llu snapshot_source=worker_sample NOT_exact_hook_frame",ws::observed::subtype(motion.velocity,motion.valid),std::abs(motion.velocity.y),std::hypot(motion.velocity.x,motion.velocity.z),static_cast<unsigned long long>(snapshotMs));
  }
  if(e.kind==3){summary(e.self,s,when,"verified_deinit");sessions.erase(it);objectFields={};}
 }
 auto before=observer.context(double(now));observer.expire(double(now));
 if(before.wallRunActive&&!observer.context(double(now)).wallRunActive){clearSessions("emergency_update_timeout_5s");resetFloorTsc=readTsc();stateLog(double(now));}
 auto ctx=observer.context(double(now));
 // Bounded asynchronous observation, NOT the object at exact hook entry. Never read after processed Deinit.
 auto session=sessions.find(ctx.wallRunOwner);if(snapshot&&session!=sessions.end())videoSample(session->second,*snapshot,motion,snapshotMs);bool recentNative=session!=sessions.end()&&double(now)-session->second.last<=250;
 if(objectEnabled&&snapshot&&ctx.wallRunActive&&ctx.wallRunUpdateCalls&&recentNative&&now-lastObjectSample>=100&&now-snapshotMs<=250){
  lastObjectSample=now;msm2::source::observe(ctx.wallRunOwner,*snapshot,motion,now);
 }
 if(snapshot){ws::Vec3 position{snapshot->position[0],snapshot->position[1],snapshot->position[2]};auto axes=ws::diag::actorAxes(snapshot->prefix);
  if(msm2::source::runtimeObservationEnabled)surfaceTracker.tick(double(now),snapshotMs,snapshot->hero,snapshot->transform,motion.epoch,axes,ctx.wallRunActive,writeLog,&position);
  if(msm2::source::runtimeObservationEnabled)surfaceTracker.observeActor(double(now),snapshotMs,snapshot->hero,snapshot->transform,motion.epoch,axes,position,motion.velocity,motion.valid,ctx.wallRunActive,writeLog);
  msm2::source::tick(double(now),snapshotMs,*snapshot,motion,ctx.wallRunActive);
 }
}
ws::observed::VerifiedWallContext context(std::uint64_t now){return observer.context(double(now));}
void logState(std::uint64_t now){stateLog(double(now));}

void stop() noexcept {
 msm2::writer::kill("script_disable");msm2::writer::drainLogs();
 ws_hook_recording.store(0,std::memory_order_release);
 for(unsigned i=0;i<4;++i){auto& item=installed[i];if(!item.enabled)continue;
  unsigned char current[32]{};
  if(!readMemory(reinterpret_cast<std::uintptr_t>(item.target),current,sizeof(current))||std::memcmp(current,item.patch,sizeof(current))!=0){writeLog("hook WallRun%s NOT_restored ownership_lost=1 module_and_trampoline_retained=1",names[i]);continue;}
  writeLog("hook WallRun%s BEFORE_DISABLE",names[i]);auto code=MH_DisableHook(item.target);if(code==MH_OK)item.enabled=false;writeLog("hook WallRun%s AFTER_DISABLE status=%d restored=%u",names[i],int(code),unsigned(!item.enabled));
 }
 ws::capture::Event pending{};unsigned omitted=0;while(ws::capture::pop(pending))++omitted;
 if(omitted)writeLog("hook_shutdown pending_events_omitted=%u candidate_data_incomplete=1",omitted);
 // Do not remove/uninitialize: a preempted thunk can still tail-jump to its trampoline.
 clearSessions("script_disable");stateLog(double(GetTickCount64()));verifiedAvailable=false;observer.configure(false);
 bool restored=true;for(const auto& item:installed)restored=restored&&!item.enabled;
 writeLog("hook_shutdown all_owned_patches_restored=%u retained_trampolines=1 actor_overrides=none",unsigned(restored));
}
}
