#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <array>
#include <atomic>
#include <cstring>
#include <bcrypt.h>
#include "NativeHooks/native_source_override.hpp"
#include "NativeHooks/source_lifecycle.hpp"
#include "NativeHooks/immediate_reset_trace.hpp"
#include "GameInterop/stack_source.hpp"
#include "msm2/sdk_bridge.hpp"
#include "msm2/orientation_source.hpp"
namespace {
using ws::Vec3;using ws::source_math::Matrix;using ws::native_source::Token;using msm2::writeLog;
constexpr std::uintptr_t callReturn=0x2cfc8f3,callBegin=0x2cfc850;constexpr unsigned callSize=168,capacity=1024;
constexpr unsigned char callerDigest[32]={0x88,0x50,0x29,0x96,0xd3,0x91,0x73,0xbf,0x38,0x08,0x6e,0x8d,0xda,0xfd,0x9e,0xff,0x28,0x75,0xda,0xda,0x30,0x71,0x7d,0x1c,0x9f,0x46,0x51,0x32,0x4f,0xbe,0x3b,0x3f};
std::array<msm2::writer::Patch,4> lifecyclePatches;unsigned char callerOriginal[168]{};
msm2::native_source::Config config;std::uintptr_t imageBase{};std::atomic<bool> killed{true},isActive{false};bool admitted{},configured{};std::atomic_flag stateLock=ATOMIC_FLAG_INIT;thread_local bool inside{},didOverride{};
struct Shared {
 std::uintptr_t hero{},transform{},owner{};std::uint64_t session{},generation{},published{};
 ws::source_math::Transition transition;std::uint64_t blocked{},lastWrite{},lastUpdate{},firstHeld{},lastObserveUpdate{};
 unsigned writes{},heldUpdates{},otherWriters{};float elapsed{},lastT{},bestSurface{};bool pass{},resetRequested{};
} state;
struct Record {
 unsigned kind{};const char* reason="none";std::uint64_t session{},ms{},generation{},nativeUpdate{},heldMs{},commitQpc{};
 std::uintptr_t source{},caller{};float elapsed{},t{},actorDotSurface{},actorDotWorld{};unsigned writes{},heldUpdates{};Matrix original{},applied{};ws::diag::ActorAxes target{};Vec3 n{},actorUp{};bool positionSame{},stack{},success{};
};
std::array<Record,capacity> records;std::atomic<unsigned> head{0},tail{0};
bool room(){return head.load(std::memory_order_relaxed)-tail.load(std::memory_order_acquire)<capacity;}
void emit(const Record& r){auto h=head.load(std::memory_order_relaxed);records[h%capacity]=r;head.store(h+1,std::memory_order_release);}
void end(const char* reason,std::uint64_t now){
 if(state.transition.active&&room()){Record r{};r.kind=2;r.reason=reason;r.session=state.session;r.generation=state.generation;r.ms=now;r.elapsed=float(now-state.transition.entryMs)/1000;r.writes=state.writes;r.heldUpdates=state.heldUpdates;r.heldMs=state.firstHeld?now-state.firstHeld:0;r.success=state.pass;emit(r);}
 state.transition.active=false;state.blocked=state.generation;isActive=false;
}
void reject(const char* reason,std::uint64_t now){
 bool wasActive=state.transition.active;end(reason,now);
 if(!wasActive&&state.session&&room()){Record r{};r.kind=5;r.reason=reason;r.session=state.session;r.ms=now;r.generation=state.generation;emit(r);}
}
bool callerAuthenticated(){
 DWORD64 image=0;auto* f=RtlLookupFunctionEntry(imageBase+callReturn-1,&image,nullptr);
 if(!f||image!=imageBase||f->BeginAddress!=callBegin||f->EndAddress!=callBegin+callSize)return false;
 unsigned char code[callSize]{},digest[32]{};if(!msm2::readMemory(imageBase+callBegin,code,sizeof(code)))return false;
 BCRYPT_ALG_HANDLE alg=nullptr;BCRYPT_HASH_HANDLE hash=nullptr;unsigned char object[512]{};DWORD size=0,done=0;bool ok=BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)>=0;
 if(ok)ok=BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&size),sizeof(size),&done,0)>=0&&size<=sizeof(object);
 if(ok)ok=BCryptCreateHash(alg,&hash,object,size,nullptr,0,0)>=0;
 if(ok)ok=BCryptHashData(hash,code,sizeof(code),0)>=0;
 if(ok)ok=BCryptFinishHash(hash,digest,32,0)>=0;
 if(hash)BCryptDestroyHash(hash);
 if(alg)BCryptCloseAlgorithmProvider(alg,0);
 if(ok)std::memcpy(callerOriginal,code,sizeof(code));
 return ok&&!std::memcmp(digest,callerDigest,32);
}
bool currentGuards(){
 unsigned char bytes[168]{};
 if(!msm2::input::authenticated()||!msm2::readMemory(imageBase+callBegin,bytes,168)||std::memcmp(bytes,callerOriginal,168))return false;
 for(const auto& p:lifecyclePatches)if(!p.address||!msm2::readMemory(p.address,bytes,32)||std::memcmp(bytes,p.bytes.data(),32))return false;
 return true;
}
bool freshToken(const Token& t,std::uint64_t now){return t.phase==3&&t.owner&&t.updates&&now>=t.initMs&&now>=t.updateMs&&now-t.updateMs<=100;}
bool observe(const msm2::Snapshot& s,const Token& token,std::uint64_t now,bool mayAbort){
 if(!state.transition.active||!state.writes)return true;
 auto axes=ws::diag::actorAxes(s.prefix);Vec3 up{};if(!axes.valid||!ws::unit(axes.up,up)){end("invalid_actor_basis",now);return false;}
 float ds=ws::dot(up,state.transition.surface),dw=up.y;state.bestSurface=std::max(state.bestSurface,ds);
 if(mayAbort&&state.lastT>=1&&token.updates!=state.lastObserveUpdate){
  state.lastObserveUpdate=token.updates;
  if(ds>=.98f){if(!state.firstHeld)state.firstHeld=now;++state.heldUpdates;if(state.heldUpdates>=20||now-state.firstHeld>=300)state.pass=true;}
  else {state.firstHeld=0;state.heldUpdates=0;if(mayAbort){end(state.bestSurface>=.98f?"B_returned_from_surface_orientation":"A_or_B_source_applied_but_not_retained",now);return false;}}
 }
 if(room()){Record r{};r.kind=3;r.reason=mayAbort?"next_matching_native_PRE":"worker";r.session=state.session;r.generation=state.generation;r.ms=now;r.nativeUpdate=token.updates;r.actorUp=up;r.n=state.transition.surface;r.actorDotSurface=ds;r.actorDotWorld=dw;r.heldMs=state.firstHeld?now-state.firstHeld:0;r.heldUpdates=state.heldUpdates;r.success=state.pass;emit(r);}
 return true;
}
}
extern "C" {std::atomic<unsigned> ws_native_source_enabled{0};}
namespace msm2::native_source {
void configure(Config c,bool verified,std::uintptr_t base,std::size_t size,const std::array<msm2::writer::Patch,4>& patches){
 kill();if(configured){writeLog("native_source_rearm_refused full_game_restart_required=1");return;}configured=true;config=c;lifecyclePatches=patches;imageBase=base;admitted=verified&&c.enabled&&c.sourceOverride&&base&&size>callReturn&&std::isfinite(c.delay)&&c.delay>=.7f&&c.delay<=5&&std::isfinite(c.rotateTime)&&c.rotateTime>=.2f&&c.rotateTime<=1&&callerAuthenticated()&&currentGuards();
 writeLog("native_source_config enabled=%u requested_override=%u admitted=%u direct_actor_transform_write=0 adhesion=0 movement=0 camera=0 animation=0 source_write_range=[0x00,0x30) position_write=0 caller_rva=0x2cfc8f3 caller_body_hash_verified=%u source_stack_relation=writer_entry_RSP+0x28 delay=%.3f rotate_time=%.3f",unsigned(c.enabled),unsigned(c.sourceOverride),unsigned(admitted),unsigned(admitted),c.delay,c.rotateTime);
 if(!admitted)return;
 state={};killed=false;ws::native_source::armLifecycle(true);ws_native_source_enabled=1;
}
void kill() noexcept{killed=true;ws_native_source_enabled=0;isActive=false;ws::native_source::armLifecycle(false);}
bool active() noexcept{return isActive.load();}
bool observationGuardsAuthenticated() noexcept{return !killed.load()&&admitted&&currentGuards();}
bool entryOverridden() noexcept{return didOverride;}
void tick(const ws::observed::VerifiedWallContext& ctx,const char* reason,bool authenticated){
 if(!authenticated){kill();}
 if(stateLock.test_and_set(std::memory_order_acquire))return;
 auto now=GetTickCount64();
 if(killed.load()||!admitted){end("disabled_kill_or_patch_gate",now);state.published=0;stateLock.clear(std::memory_order_release);return;}
 Token token{};bool native=ws::native_source::lifecycle(token);
 if(!native&&!token.observed){stateLock.clear(std::memory_order_release);return;} // contention is missing evidence, not Deinit
 if(!ctx.lifecycleVerified||!ctx.wallRunActive||!native||!freshToken(token,now)||token.owner!=ctx.wallRunOwner){end(!ctx.wallRunActive?reason:token.lastKind==3?"native_verified_Deinit_before_worker_drain":"native_lifecycle_inactive_or_stale",now);state.published=0;stateLock.clear(std::memory_order_release);return;}
 msm2::Snapshot s{};
 if(!msm2::sample(s)||!ws::diag::actorAxes(s.prefix).valid){end("fresh_Hero_snapshot_invalid",now);state.published=0;stateLock.clear(std::memory_order_release);return;}
 if(state.generation!=token.generation||state.session!=ctx.wallRunSessionId){end("new_native_session",now);state={};state.generation=token.generation;state.session=ctx.wallRunSessionId;state.hero=s.hero;state.transform=s.transform;state.owner=token.owner;}
 if(s.hero!=state.hero||s.transform!=state.transform){end("Hero_Transform_identity_changed",now);state.published=0;stateLock.clear(std::memory_order_release);return;}
 state.published=now;state.elapsed=float(ctx.elapsed);
 // Only native PRE of the next matching call diagnoses overwrites. Worker samples
 // can see a thread paused before original writer runs, so they cannot abort it.
 observe(s,token,now,false);
 stateLock.clear(std::memory_order_release);
}
void capture(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t,std::uintptr_t ret,std::uintptr_t originalRsp) noexcept {
 didOverride=false;
 if(inside||killed.load()||!ws_native_source_enabled.load())return;
 inside=true;auto lastError=GetLastError();
 if(stateLock.test_and_set(std::memory_order_acquire)){inside=false;SetLastError(lastError);return;}
 auto release=[&]{stateLock.clear(std::memory_order_release);inside=false;SetLastError(lastError);};
 auto now=GetTickCount64();Token token{};
 if(!state.published||now<state.published||now-state.published>100||!ws::native_source::lifecycle(token)||!freshToken(token,now)||token.generation!=state.generation||token.owner!=state.owner||token.thread!=GetCurrentThreadId()){release();return;}
 if(rcx!=state.transform){release();return;}
 if(ret!=imageBase+callReturn){
  if(state.writes&&state.otherWriters<16&&room()){Matrix in{};if(msm2::readMemory(rdx,&in,64)&&ws::source_math::valid(in)){Record r{};r.kind=4;r.session=state.session;r.ms=now;r.caller=ret-imageBase;r.original=in;emit(r);++state.otherWriters;}}
  release();return;
 }
 if(state.blocked==state.generation||now-token.initMs<config.delay*1000||state.elapsed<config.delay){release();return;}
 if(r8!=0){reject("unexpected_R8_not_null",now);release();return;}
 msm2::Snapshot hero{};
 if(!msm2::sample(hero)||hero.hero!=state.hero||hero.transform!=state.transform||rcx!=hero.transform||!ws::diag::actorAxes(hero.prefix).valid){reject("native_fresh_Hero_identity_or_basis_invalid",now);release();return;}
 if(!currentGuards()){reject("entry_hook_ownership_lost",now);killed=true;ws_native_source_enabled=0;release();return;}
 Matrix incoming{};auto stack=ws::interop::currentStack();
 if(!ws::interop::stackMatrix(rdx,originalRsp,stack,incoming)||!(rdx+64<=rcx||rcx+64<=rdx)){reject("source_not_verified_stack_local_matrix",now);release();return;}
 Vec3 n{};if(token.owner>UINTPTR_MAX-0x3a4||!msm2::readMemory(token.owner+0x398,&n,12)){reject("surface_read_failed",now);release();return;}
 if(!state.transition.active){if(!state.transition.enter(incoming,n,double(now))){reject("invalid_entry_normal_or_projected_forward",now);release();return;}state.lastObserveUpdate=token.updates;isActive=true;}
 if(!observe(hero,token,now,true)){release();return;}
 if(state.writes>=512||now-state.transition.entryMs>=5000){end("bounded_test_budget_512writes_or_5seconds",now);release();return;}
 Matrix applied{};float t=0;if(!state.transition.sample(incoming,n,double(now),config.rotateTime,applied,t)){end("invalid_normal_corner_or_source_basis",now);release();return;}
 Token final{};
 if(!room()||!currentGuards()||killed.load()||!ws_native_source_enabled.load()||!ws::native_source::lifecycle(final)||final.generation!=token.generation||final.owner!=token.owner||final.updates!=token.updates||final.thread!=token.thread){end("commit_admission_lost_or_log_overflow",now);release();return;}
 auto result=ws::interop::writeStackOrientation(rdx,incoming,applied);
 if(!result.success){end(result.rollbackFailed?"source_write_failed_rollback_failed":"source_write_failed_restored_or_untouched",now);killed=true;ws_native_source_enabled=0;release();return;}
 didOverride=true;
 Vec3 appliedUp{applied.v[4],applied.v[5],applied.v[6]};
 if(!state.resetRequested&&(appliedUp.y<=.85f||ws::dot(appliedUp,state.transition.surface)>=.50f)){
  state.resetRequested=true;msm2::reset_trace::Request request{};request.session=state.session;request.generation=token.generation;request.nativeUpdate=token.updates;request.commitQpc=result.commitQpc;request.hero=state.hero;request.transform=state.transform;request.owner=token.owner;request.thread=token.thread;request.normal=state.transition.surface;request.appliedUp=appliedUp;msm2::reset_trace::request(request);
 }
 ++state.writes;state.lastWrite=now;state.lastUpdate=token.updates;state.lastT=t;
 Record r{};r.kind=1;r.commitQpc=result.commitQpc;r.session=state.session;r.generation=state.generation;r.ms=now;r.nativeUpdate=token.updates;r.source=rdx;r.caller=callReturn;r.elapsed=float(now-token.initMs)/1000;r.t=t;r.original=incoming;r.applied=applied;r.target=state.transition.target;r.n=state.transition.surface;r.positionSame=result.positionUnchanged;r.stack=true;r.writes=state.writes;emit(r);release();
}
void drainLogs(){
 for(unsigned n=0;n<capacity;++n){auto t=tail.load(std::memory_order_relaxed);if(t==head.load(std::memory_order_acquire))break;auto r=records[t%capacity];tail.store(t+1,std::memory_order_release);
  if(r.kind==1)writeLog("NATIVE_SOURCE_OVERRIDE session=%llu generation=%llu native_ms=%llu native_update=%llu elapsed=%.6f caller_rva=0x%llx source_ptr=0x%llx original_right=(%.9g,%.9g,%.9g) original_up=(%.9g,%.9g,%.9g) original_forward=(%.9g,%.9g,%.9g) target_right=(%.9g,%.9g,%.9g) target_up=(%.9g,%.9g,%.9g) target_forward=(%.9g,%.9g,%.9g) applied_right=(%.9g,%.9g,%.9g) applied_up=(%.9g,%.9g,%.9g) applied_forward=(%.9g,%.9g,%.9g) t=%.6f position_unchanged=%u source_is_stack_local=%u direct_actor_transform_write=0 write_index=%u override_commit_qpc=%llu",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.generation),static_cast<unsigned long long>(r.ms),static_cast<unsigned long long>(r.nativeUpdate),r.elapsed,static_cast<unsigned long long>(r.caller),static_cast<unsigned long long>(r.source),r.original.v[0],r.original.v[1],r.original.v[2],r.original.v[4],r.original.v[5],r.original.v[6],r.original.v[8],r.original.v[9],r.original.v[10],r.target.right.x,r.target.right.y,r.target.right.z,r.target.up.x,r.target.up.y,r.target.up.z,r.target.forward.x,r.target.forward.y,r.target.forward.z,r.applied.v[0],r.applied.v[1],r.applied.v[2],r.applied.v[4],r.applied.v[5],r.applied.v[6],r.applied.v[8],r.applied.v[9],r.applied.v[10],r.t,unsigned(r.positionSame),unsigned(r.stack),r.writes,static_cast<unsigned long long>(r.commitQpc));
  else if(r.kind==2)writeLog("NATIVE_SOURCE_EXIT session=%llu reason=%s duration_since_source_entry=%.6f applied_writes=%u held_updates=%u held_ms=%llu stability_observed=%u direct_actor_transform_write=0",static_cast<unsigned long long>(r.session),r.reason,r.elapsed,r.writes,r.heldUpdates,static_cast<unsigned long long>(r.heldMs),unsigned(r.success));
  else if(r.kind==3)writeLog("NATIVE_SOURCE_ACTOR_OBSERVATION session=%llu native_update=%llu native_ms=%llu actor_up=(%.9g,%.9g,%.9g) actor_up_dot_surface=%.9g actor_up_dot_world=%.9g held_distinct_updates=%u held_ms=%llu stability_observed=%u source=read_only_snapshot_NOT_POST_writer_callback lifecycle_active=1 observation_phase=%s",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.nativeUpdate),static_cast<unsigned long long>(r.ms),r.actorUp.x,r.actorUp.y,r.actorUp.z,r.actorDotSurface,r.actorDotWorld,r.heldUpdates,static_cast<unsigned long long>(r.heldMs),unsigned(r.success),r.reason);
  else if(r.kind==5)writeLog("NATIVE_SOURCE_ENTRY_REJECT session=%llu generation=%llu reason=%s source_override=0",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.generation),r.reason);
  else if(r.kind==4)writeLog("NATIVE_SOURCE_OTHER_WRITER session=%llu caller_return_rva=0x%llx source_up=(%.9g,%.9g,%.9g) override=0",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.caller),r.original.v[4],r.original.v[5],r.original.v[6]);
 }
}
void stop(){kill();if(!stateLock.test_and_set(std::memory_order_acquire)){end("script_stop",GetTickCount64());stateLock.clear(std::memory_order_release);}drainLogs();writeLog("native_source_stop source_override=OFF direct_actor_transform_write=0 vanilla_passthrough=1");}
}
