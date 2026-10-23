#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <array>
#include <atomic>
#include <cstring>
#include <bcrypt.h>
#include "NativeHooks/native_source_override.hpp"
#include "NativeHooks/source_lifecycle.hpp"
#include "NativeHooks/immediate_reset_trace.hpp"
#include "NativeHooks/reset_copy_source.hpp"
#include "NativeHooks/path_b_layout.hpp"
#include "msm2/orientation_trace.hpp"
#include "GameInterop/stack_source.hpp"
#include "GameInterop/atomic_publication.hpp"
#include "msm2/sdk_bridge.hpp"
#include "msm2/orientation_source.hpp"
namespace {
using ws::Vec3;using ws::source_math::Matrix;using ws::native_source::Token;using msm2::writeLog;
constexpr std::uintptr_t callReturn=0x2cfc8f3,callBegin=0x2cfc850;constexpr unsigned callSize=168,capacity=1024;
constexpr unsigned char callerDigest[32]={0x88,0x50,0x29,0x96,0xd3,0x91,0x73,0xbf,0x38,0x08,0x6e,0x8d,0xda,0xfd,0x9e,0xff,0x28,0x75,0xda,0xda,0x30,0x71,0x7d,0x1c,0x9f,0x46,0x51,0x32,0x4f,0xbe,0x3b,0x3f};
std::array<msm2::writer::Patch,4> lifecyclePatches;unsigned char callerOriginal[168]{};
msm2::native_source::Config config;std::uintptr_t imageBase{};std::atomic<bool> killed{true},isActive{false};bool admitted{},configured{};std::atomic_flag stateLock=ATOMIC_FLAG_INIT;thread_local bool inside{},didOverride{};
struct Published {
 std::uint64_t session{},generation{},lifecycleEpoch{},publishedMs{},initMs{};std::uintptr_t hero{},transform{},owner{};
 unsigned thread{},nativeUpdate{};float elapsed{},t{};bool active{};ws::diag::ActorAxes basis{};Vec3 surface{};
};
ws::interop::AtomicPublication<Published> publication;
std::atomic<unsigned> pathAWrites{0},pathBWrites{0},pathARejects{0},pathBRejects{0};
std::atomic<bool> resetRequested{false},lifecycleOpen{false};std::atomic<std::uint64_t> lifecycleEpoch{0};
struct Shared {
 std::uintptr_t hero{},transform{},owner{};std::uint64_t session{},generation{},published{};
 ws::source_math::Transition transition;std::uint64_t blocked{},lastWrite{},lastUpdate{},firstHeld{},lastObserveUpdate{};
 Matrix entryBasis{},currentBasis{};Vec3 entryForward{};std::uint64_t lastCommonUpdate{},lastObserveMs{},lastMetricMs{};
 unsigned writes{},commonWrites{},copyWrites{},heldUpdates{},otherWriters{};float elapsed{},lastT{},bestSurface{};unsigned rejects{};bool pass{};
} state;
struct Record {
 unsigned kind{},gateMask{};const char* reason="none";std::uint64_t session{},ms{},generation{},nativeUpdate{},heldMs{},commitQpc{};
 std::uintptr_t source{},caller{};float elapsed{},t{},actorDotSurface{},actorDotWorld{};unsigned writes{},heldUpdates{};Matrix original{},applied{};ws::source_math::CompactBlock compactOriginal{},compactApplied{};ws::diag::ActorAxes target{};Vec3 n{},actorUp{},rowLengths{};bool positionSame{},stack{},success{},prearmed{},copy{},late{},sourceReadable{},sourceLayout{};unsigned nativeThread{};std::uintptr_t rcx{},r8{},r9{},rsp{},stackLimit{},stackBase{};
};
std::array<Record,capacity> records;std::atomic<unsigned> head{0},tail{0};
std::atomic_flag recordLock=ATOMIC_FLAG_INIT;std::atomic<unsigned> recordDrops{0};
unsigned reportedA=~0u,reportedB=~0u,reportedAR=~0u,reportedBR=~0u;
bool room(){return head.load(std::memory_order_relaxed)-tail.load(std::memory_order_acquire)<capacity;}
void emit(const Record& r){
 if(recordLock.test_and_set(std::memory_order_acquire)){++recordDrops;return;}
 auto h=head.load(std::memory_order_relaxed);if(h-tail.load(std::memory_order_acquire)<capacity){records[h%capacity]=r;head.store(h+1,std::memory_order_release);}else ++recordDrops;
 recordLock.clear(std::memory_order_release);
}
void publishInactive() noexcept {Published p{};publication.publish(p);}
void end(const char* reason,std::uint64_t now){
 if(state.transition.active&&room()){Record r{};r.kind=2;r.reason=reason;r.session=state.session;r.generation=state.generation;r.ms=now;r.elapsed=float(now-state.transition.entryMs)/1000;r.writes=state.writes;r.heldUpdates=state.heldUpdates;r.heldMs=state.firstHeld?now-state.firstHeld:0;r.success=state.pass;emit(r);}
 state.transition.active=false;state.blocked=state.generation;isActive=false;
 lifecycleOpen=false;++lifecycleEpoch;
 publishInactive();
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
 if((config.dualSource&&!msm2::reset_copy::authenticated())||!msm2::input::authenticated()||!msm2::readMemory(imageBase+callBegin,bytes,168)||std::memcmp(bytes,callerOriginal,168))return false;
 for(const auto& p:lifecyclePatches)if(!p.address||!msm2::readMemory(p.address,bytes,32)||std::memcmp(bytes,p.bytes.data(),32))return false;
 return true;
}
void prearmPublished(const Token& token,std::uint64_t now){
 Published p{};if(!publication.read(p)||p.generation!=token.generation||p.owner!=token.owner||p.thread!=token.thread)return;
 if(now<token.initMs||now-token.initMs<600||now-token.initMs>=1100)return;
 bool expected=false;
 if(resetRequested.compare_exchange_strong(expected,true)&&currentGuards()){
  Vec3 normal{};
  if(token.owner<=UINTPTR_MAX-0x3a4&&msm2::readMemory(token.owner+0x398,&normal,12)&&ws::finite(normal)&&ws::dot(normal,normal)>=.95f*.95f&&ws::dot(normal,normal)<=1.05f*1.05f&&std::abs(normal.y)<=.20f){
   msm2::reset_trace::Request r{};r.prearmed=true;r.verifyDual=config.dualSource;r.characterize=true;r.initMs=token.initMs;r.session=p.session;r.generation=token.generation;r.nativeUpdate=token.updates;r.hero=p.hero;r.transform=p.transform;r.owner=p.owner;r.thread=token.thread;r.normal=normal;msm2::reset_trace::request(r);
  }
 }
}

bool freshToken(const Token& t,std::uint64_t now){return t.phase==3&&t.owner&&t.updates&&now>=t.initMs&&now>=t.updateMs&&now-t.updateMs<=100;}
bool observe(const msm2::Snapshot& s,const Token& token,std::uint64_t now,bool mayAbort){
 if(!state.transition.active||!state.writes)return true;
 auto axes=ws::diag::actorAxes(s.prefix);Vec3 up{};
 bool valid=axes.valid&&ws::unit(axes.up,up);
 float ds=valid?ws::dot(up,state.transition.surface):0,dw=valid?up.y:0;state.bestSurface=std::max(state.bestSurface,ds);
 bool distinct=token.updates!=state.lastObserveUpdate;
 // Any observed loss breaks continuity, even between two calls in the SAME
 // update. Only good distinct native updates are allowed to increment it.
 if(state.lastT>=1&&(!valid||ds<.98f)){state.firstHeld=0;state.heldUpdates=0;}
 if(mayAbort&&state.lastT>=1&&distinct){
  state.lastObserveUpdate=token.updates;
  if(!valid||ds<.98f||(state.lastObserveMs&&now-state.lastObserveMs>100)) {state.firstHeld=0;state.heldUpdates=0;}
  state.lastObserveMs=now;
  if(valid&&ds>=.98f&&(!config.dualSource||(state.commonWrites&&state.copyWrites))){
   if(!state.firstHeld)state.firstHeld=now;
   ++state.heldUpdates;
   if(!state.pass&&(state.heldUpdates>=20||now-state.firstHeld>=300)){
    state.pass=true;
    if(room()){Record pass{};pass.kind=7;pass.session=state.session;pass.heldUpdates=state.heldUpdates;pass.heldMs=now-state.firstHeld;emit(pass);}
   }
  }
 }
 // An observed reset invalidates retention, NOT source ownership. The old
 // abort here allowed subsequent exact valid WorldUp calls through untouched.
 if(room()&&(distinct||!valid||ds<.98f||now-state.lastMetricMs>=50)){state.lastMetricMs=now;Record r{};r.kind=3;r.reason=mayAbort?"verified_native_update_worker":"supplementary_worker";r.session=state.session;r.generation=state.generation;r.ms=now;r.nativeUpdate=token.updates;r.actorUp=up;r.n=state.transition.surface;r.actorDotSurface=ds;r.actorDotWorld=dw;r.heldMs=state.firstHeld?now-state.firstHeld:0;r.heldUpdates=state.heldUpdates;r.success=state.pass;emit(r);}
 return true;
}
}
extern "C" {std::atomic<unsigned> ws_native_source_enabled{0};}
namespace msm2::native_source {
void configure(Config c,bool verified,std::uintptr_t base,std::size_t size,const std::array<msm2::writer::Patch,4>& patches){
 kill();if(configured){writeLog("native_source_rearm_refused full_game_restart_required=1");return;}configured=true;config=c;lifecyclePatches=patches;imageBase=base;
 bool callerVerified=verified&&base&&size>callReturn&&callerAuthenticated();
 admitted=verified&&c.enabled&&c.sourceOverride&&callerVerified&&std::isfinite(c.delay)&&c.delay>=.7f&&c.delay<=5&&std::isfinite(c.rotateTime)&&c.rotateTime>=.2f&&c.rotateTime<=1&&currentGuards()&&(!c.dualSource||msm2::reset_copy::compactProofAuthenticated());
 writeLog("native_source_config enabled=%u requested_override=%u admitted=%u direct_actor_transform_write=0 adhesion=0 movement=0 camera=0 animation=0 source_write_range=[0x00,0x30) position_write=0 caller_rva=0x2cfc8f3 caller_body_hash_verified=%u source_stack_relation=writer_entry_RSP+0x28 delay=%.3f rotate_time=%.3f",unsigned(c.enabled),unsigned(c.sourceOverride),unsigned(admitted),unsigned(callerVerified),c.delay,c.rotateTime);
 writeLog("DUAL_SOURCE_ADMISSION admitted=%u dual_source_required=%u common_caller=0x2cfc8f3 copy_caller=0x2cfc84b copy_hook_authenticated=%u",unsigned(admitted),unsigned(c.dualSource),unsigned(msm2::reset_copy::authenticated()));
 path_b::configure(admitted,true);
 if(!admitted)return;
 state={};pathAWrites=0;pathBWrites=0;pathARejects=0;pathBRejects=0;resetRequested=false;lifecycleOpen=false;publishInactive();killed=false;ws::native_source::armLifecycle(true);ws_native_source_enabled=1;
}
void prearm(const Token& token) noexcept {
 if(killed.load()||!admitted)return;
 auto now=GetTickCount64();if(freshToken(token,now))prearmPublished(token,now);
}
void lifecycleClosed(const Token&) noexcept {lifecycleOpen=false;++lifecycleEpoch;publishInactive();isActive=false;}
void kill() noexcept{path_b::kill();killed=true;lifecycleOpen=false;++lifecycleEpoch;ws_native_source_enabled=0;isActive=false;publishInactive();ws::native_source::armLifecycle(false);}
bool active() noexcept{return isActive.load();}
bool observationGuardsAuthenticated() noexcept{return !killed.load()&&admitted&&currentGuards();}
bool entryOverridden() noexcept{return didOverride;}
void tick(const ws::observed::VerifiedWallContext& ctx,const char* reason,bool authenticated){
 if(!authenticated){kill();}
 if(stateLock.test_and_set(std::memory_order_acquire))return;
 auto now=GetTickCount64(),epoch=lifecycleEpoch.load();
 if(killed.load()||!admitted){end("disabled_kill_or_patch_gate",now);state.published=0;stateLock.clear(std::memory_order_release);return;}
 Token token{};bool native=ws::native_source::lifecycle(token);
 if(!native&&!token.observed){stateLock.clear(std::memory_order_release);return;} // contention is missing evidence, not Deinit
 if(!ctx.lifecycleVerified||!ctx.wallRunActive||!native||!freshToken(token,now)||token.owner!=ctx.wallRunOwner){end(!ctx.wallRunActive?reason:token.lastKind==3?"native_verified_Deinit_before_worker_drain":"native_lifecycle_inactive_or_stale",now);state.published=0;stateLock.clear(std::memory_order_release);return;}
 msm2::Snapshot s{};
 if(!msm2::sample(s)){end("fresh_Hero_snapshot_invalid",now);state.published=0;stateLock.clear(std::memory_order_release);return;}
 if(state.generation!=token.generation||state.session!=ctx.wallRunSessionId){end("new_native_session",now);state={};state.generation=token.generation;state.session=ctx.wallRunSessionId;state.hero=s.hero;state.transform=s.transform;state.owner=token.owner;pathAWrites=0;pathBWrites=0;pathARejects=0;pathBRejects=0;resetRequested=false;}
 if(s.hero!=state.hero||s.transform!=state.transform){end("Hero_Transform_identity_changed",now);state.published=0;stateLock.clear(std::memory_order_release);return;}
 state.published=now;state.elapsed=float(ctx.elapsed);Vec3 n{};
 bool normalOk=token.owner<=UINTPTR_MAX-0x3a4&&msm2::readMemory(token.owner+0x398,&n,12)&&ws::finite(n);
 if(!state.transition.active&&ctx.elapsed>=config.delay&&normalOk){
  Matrix baseline{};std::memcpy(baseline.v,s.prefix,48);baseline.v[12]=s.position[0];baseline.v[13]=s.position[1];baseline.v[14]=s.position[2];baseline.v[15]=1;
  if(!state.transition.enter(baseline,n,double(now))){end("invalid_entry_normal_or_forward",now);stateLock.clear(std::memory_order_release);return;}
  state.entryBasis=baseline;state.lastObserveUpdate=token.updates;isActive=true;
 }
 Published p{};p.session=state.session;p.generation=state.generation;p.lifecycleEpoch=epoch;p.publishedMs=now;p.initMs=token.initMs;p.hero=state.hero;p.transform=state.transform;p.owner=state.owner;p.thread=token.thread;p.nativeUpdate=token.updates;p.elapsed=float(now-token.initMs)/1000.f;
 if(state.transition.active&&normalOk){Matrix sampled{};float t{};Matrix incoming=state.entryBasis;
  if(now-state.transition.entryMs>=5000){end("bounded_test_window_5seconds",now);}
  else if(state.transition.sample(incoming,n,double(now),config.rotateTime,sampled,t)){p.active=true;p.t=t;p.surface=state.transition.surface;p.basis=ws::diag::actorAxes(sampled.v);state.currentBasis=sampled;state.lastT=t;lifecycleOpen=true;}
 }
 publication.publish(p);
 // Retention is sampled only from a fresh verified lifecycle update after both
 // source paths have committed. Distinct native updates cannot be inflated by calls.
 state.commonWrites=pathAWrites.load();state.copyWrites=pathBWrites.load();state.writes=state.commonWrites+state.copyWrites;
 if(p.active&&p.t>=1&&state.commonWrites&&(!config.dualSource||state.copyWrites))observe(s,token,now,true);
 stateLock.clear(std::memory_order_release);
}
void capturePath(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t ret,std::uintptr_t originalRsp) noexcept {
 didOverride=false;if(inside||killed.load()||!ws_native_source_enabled.load())return;
 auto error=GetLastError();inside=true;auto release=[&]{inside=false;SetLastError(error);};auto now=GetTickCount64();Published p{};
 if(ret!=imageBase+callReturn){release();return;}
 auto rejectEntry=[&](const char* reason,unsigned mask){auto count=pathARejects.fetch_add(1);if(count<64){Record r{};r.kind=10;r.reason=reason;r.session=p.session;r.elapsed=p.elapsed;r.gateMask=mask;emit(r);}};
 if(!lifecycleOpen.load()||!publication.read(p)||p.lifecycleEpoch!=lifecycleEpoch.load()||!p.active||now<p.publishedMs||now-p.publishedMs>100||p.thread!=GetCurrentThreadId()){rejectEntry("lockfree_snapshot_inactive_stale_or_incoherent",1);release();return;}
 if(pathAWrites.load()>=4096){rejectEntry("path_A_write_budget_exhausted",1|16|32);release();return;}
 if(rcx!=p.transform){rejectEntry("current_Hero_target_mismatch",1|16|32);release();return;}
 if(r8!=0){rejectEntry("unexpected_R8_not_null",1|2|16|32);release();return;}
 msm2::Snapshot hero{};if(!msm2::sample(hero)||hero.hero!=p.hero||hero.transform!=p.transform){rejectEntry("fresh_Hero_identity_unavailable_or_changed",1|2|16|32);release();return;}
 if(!currentGuards()){rejectEntry("entry_hook_or_caller_integrity_lost",1|2|16|32);kill();release();return;}
 Matrix incoming{};auto stack=ws::interop::currentStack();if(!ws::interop::stackMatrix(rdx,originalRsp,stack,incoming)||!(rdx+64<=rcx||rcx+64<=rdx)){rejectEntry("path_A_stack_provenance_or_source_layout_failed",1|2|16|32);release();return;}
 Matrix applied=incoming;Vec3 rows[]={p.basis.right,p.basis.up,p.basis.forward};for(unsigned i=0;i<3;++i){applied.v[4*i]=rows[i].x;applied.v[4*i+1]=rows[i].y;applied.v[4*i+2]=rows[i].z;applied.v[4*i+3]=0;}
 if(!ws::source_math::valid(applied)){rejectEntry("published_basis_invalid",1|2|4|8|16|32);release();return;}
 Published final{};if(!publication.read(final)||final.session!=p.session||final.generation!=p.generation||final.lifecycleEpoch!=p.lifecycleEpoch||final.lifecycleEpoch!=lifecycleEpoch.load()||final.publishedMs!=p.publishedMs||!final.active||!currentGuards()){rejectEntry("lockfree_snapshot_or_integrity_changed_before_commit",1|2|4|8|16|32);release();return;}
 LARGE_INTEGER begin{};QueryPerformanceCounter(&begin);auto result=ws::interop::writeStackOrientation(rdx,incoming,applied);
 if(!result.success){rejectEntry(result.rollbackFailed?"path_A_write_failed_rollback_failed":"path_A_write_failed_restored_or_untouched",1|2|4|8|16|32);if(result.rollbackFailed)kill();release();return;}
 didOverride=true;auto count=pathAWrites.fetch_add(1)+1;Vec3 appliedUp{applied.v[4],applied.v[5],applied.v[6]};
 msm2::reset_trace::Request committed{};committed.prearmed=true;committed.verifyDual=config.dualSource;committed.characterize=true;committed.initMs=p.initMs;committed.session=p.session;committed.generation=p.generation;committed.nativeUpdate=p.nativeUpdate;committed.commitQpc=result.commitQpc;committed.overrideBeginQpc=begin.QuadPart;committed.hero=p.hero;committed.transform=p.transform;committed.owner=p.owner;committed.thread=p.thread;committed.normal=p.surface;committed.appliedUp=appliedUp;bool prearmed=msm2::reset_trace::committed(committed);
 Record r{};r.kind=1;r.late=count>1&&incoming.v[5]>=.98f;r.rcx=rcx;r.r8=r8;r.r9=r9;r.rsp=originalRsp;r.stackLimit=stack.limit;r.stackBase=stack.base;r.prearmed=prearmed;r.nativeThread=p.thread;r.commitQpc=result.commitQpc;r.session=p.session;r.generation=p.generation;r.ms=now;r.nativeUpdate=p.nativeUpdate;r.source=rdx;r.caller=callReturn;r.elapsed=p.elapsed;r.t=p.t;r.original=incoming;r.applied=applied;r.target=p.basis;r.n=p.surface;r.positionSame=result.positionUnchanged;r.stack=true;r.writes=count;emit(r);release();
}
void capture(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t ret,std::uintptr_t rsp) noexcept {capturePath(rcx,rdx,r8,r9,ret,rsp);}
void captureCopy(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t ret,std::uintptr_t rsp) noexcept {
 auto error=GetLastError();trace::beginResetCopyEntry();if(inside||killed.load()||!ws_native_source_enabled.load()){SetLastError(error);return;}
 inside=true;auto release=[&]{inside=false;SetLastError(error);};auto now=GetTickCount64();Published p{};
 if(ret!=imageBase+0x2cfc84b){release();return;}
 auto rejectEntry=[&]{pathBRejects.fetch_add(1);};
 if(!lifecycleOpen.load()||!publication.read(p)||p.lifecycleEpoch!=lifecycleEpoch.load()||!p.active||now<p.publishedMs||now-p.publishedMs>100||p.thread!=GetCurrentThreadId()||rcx!=p.transform||!msm2::reset_copy::compactProofAuthenticated()||!currentGuards()){rejectEntry();release();return;}
 if(pathBWrites.load()>=4096){rejectEntry();release();return;}
 msm2::Snapshot hero{};if(!msm2::sample(hero)||hero.hero!=p.hero||hero.transform!=p.transform){rejectEntry();release();return;}
 auto stack=ws::interop::currentStack();ws::source_math::CompactBlock incoming{},applied{};Vec3 lengths{};
 if(!ws::interop::stackCompact(rdx,rsp,stack,incoming)||!ws::source_math::compactReplacement(incoming,p.basis,applied,lengths)){rejectEntry();release();return;}
 path_b::Entry e{};e.session=p.session;e.generation=p.generation;e.update=p.nativeUpdate;e.owner=p.owner;e.thread=p.thread;e.elapsed=p.elapsed;e.rcx=rcx;e.rdx=rdx;e.r8=r8;e.r9=r9;e.rsp=rsp;e.caller=0x2cfc84b;e.stackLimit=stack.limit;e.stackBase=stack.base;e.normal=p.surface;e.actorBefore={hero.prefix[4],hero.prefix[5],hero.prefix[6]};e.referenceValid=pathAWrites.load()>0;e.pathAUp=p.basis.up;e.readable=path_b::readBounded(rdx,e.raw,128);
 Published final{};if(!publication.read(final)||final.session!=p.session||final.generation!=p.generation||final.lifecycleEpoch!=p.lifecycleEpoch||final.lifecycleEpoch!=lifecycleEpoch.load()||final.publishedMs!=p.publishedMs||!final.active||!currentGuards()){path_b::capture(e);rejectEntry();release();return;}
 LARGE_INTEGER begin{};QueryPerformanceCounter(&begin);auto result=ws::interop::writeStackCompact(rdx,incoming,applied);e.overridden=result.success;path_b::capture(e);
 if(!result.success){rejectEntry();if(result.rollbackFailed)kill();release();return;}
 didOverride=true;auto count=pathBWrites.fetch_add(1)+1;msm2::reset_trace::Request committed{};committed.prearmed=true;committed.resetCopy=true;committed.verifyDual=true;committed.characterize=true;committed.initMs=p.initMs;committed.session=p.session;committed.generation=p.generation;committed.nativeUpdate=p.nativeUpdate;committed.commitQpc=result.commitQpc;committed.overrideBeginQpc=begin.QuadPart;committed.hero=p.hero;committed.transform=p.transform;committed.owner=p.owner;committed.thread=p.thread;committed.normal=p.surface;committed.appliedUp=p.basis.up;msm2::reset_trace::committed(committed);
 Record r{};r.kind=11;r.copy=true;r.session=p.session;r.generation=p.generation;r.nativeUpdate=p.nativeUpdate;r.ms=now;r.elapsed=p.elapsed;r.source=rdx;r.rcx=rcx;r.rsp=rsp;r.stackLimit=stack.limit;r.stackBase=stack.base;r.caller=0x2cfc84b;r.n=p.surface;r.compactOriginal=incoming;r.compactApplied=applied;r.rowLengths=lengths;r.writes=count;r.commitQpc=result.commitQpc;r.positionSame=result.positionUnchanged;r.stack=true;emit(r);release();
}
void drainLogs(){
 path_b::drainLogs();
 auto lost=recordDrops.exchange(0);if(lost)writeLog("NATIVE_SOURCE_LOG_DROPS count=%u writes_unaffected=1",lost);
 auto a=pathAWrites.load(),b=pathBWrites.load(),ar=pathARejects.load(),br=pathBRejects.load();
 if(reportedA==~0u||a-reportedA>=64||b-reportedB>=64||ar-reportedAR>=64||br-reportedBR>=64){writeLog("PATH_A_OVERRIDE_COUNT=%u PATH_A_REJECT_COUNT=%u PATH_A_STATE_LOCK_CONTENTION=0 PATH_B_OVERRIDE_COUNT=%u PATH_B_REJECT_COUNT=%u",a,ar,b,br);reportedA=a;reportedB=b;reportedAR=ar;reportedBR=br;}
 for(unsigned n=0;n<capacity;++n){auto t=tail.load(std::memory_order_relaxed);if(t==head.load(std::memory_order_acquire))break;auto r=records[t%capacity];tail.store(t+1,std::memory_order_release);
  if(r.kind==10)writeLog("COMMON_OVERRIDE_REJECT session=%llu elapsed=%.6f reason=%s caller_ok=%u target_ok=%u stack_ok=%u source_layout_ok=%u lifecycle_ok=%u elapsed_ok=%u",(unsigned long long)r.session,r.elapsed,r.reason,unsigned(bool(r.gateMask&1)),unsigned(bool(r.gateMask&2)),unsigned(bool(r.gateMask&4)),unsigned(bool(r.gateMask&8)),unsigned(bool(r.gateMask&16)),unsigned(bool(r.gateMask&32)));
  if(r.kind==7)writeLog("ORIENTATION_RETENTION_PASS=1 session=%llu held_ms=%llu held_updates=%u game_verification=requires_accepted_build_runtime_log",(unsigned long long)r.session,(unsigned long long)r.heldMs,r.heldUpdates);
  if(r.kind==1){
    writeLog("COMMON_SOURCE_OVERRIDE session=%llu elapsed=%.6f caller_rva=0x%llx target_transform=0x%llx source_ptr=0x%llx source_up_before=(%.9g,%.9g,%.9g) source_up_after=(%.9g,%.9g,%.9g) late_worldup_source_detected=%u position_unchanged=1",(unsigned long long)r.session,r.elapsed,(unsigned long long)r.caller,(unsigned long long)r.rcx,(unsigned long long)r.source,r.original.v[4],r.original.v[5],r.original.v[6],r.applied.v[4],r.applied.v[5],r.applied.v[6],unsigned(r.late));
    if(r.late)writeLog("LATE_COMMON_WORLDUP_OVERRIDE=1 session=%llu native_update=%llu reason=repeated_native_update_worldup_source",(unsigned long long)r.session,(unsigned long long)r.nativeUpdate);
  }
  if(r.kind==1&&r.writes==1)writeLog("PATH_A_LOCKFREE_SNAPSHOT_READ session=%llu generation=%llu coherent=1",(unsigned long long)r.session,(unsigned long long)r.generation);
  if(r.kind==11)writeLog("PATH_B_COMPACT3X3_OVERRIDE session=%llu generation=%llu elapsed=%.6f source_ptr=0x%llx rdx_minus_rsp=0x%llx original_row_lengths=(%.9g,%.9g,%.9g) original_up=(%.9g,%.9g,%.9g) applied_up=(%.9g,%.9g,%.9g) applied_up_dot_surface=%.9g bytes_written=0x24 metadata_untouched=1 caller_verified=1 target_verified=1 stack_verified=1 write_index=%u override_commit_qpc=%llu",(unsigned long long)r.session,(unsigned long long)r.generation,r.elapsed,(unsigned long long)r.source,(unsigned long long)(r.source-r.rsp),r.rowLengths.x,r.rowLengths.y,r.rowLengths.z,r.compactOriginal.rows[3],r.compactOriginal.rows[4],r.compactOriginal.rows[5],r.compactApplied.rows[3],r.compactApplied.rows[4],r.compactApplied.rows[5],ws::dot(Vec3{r.compactApplied.rows[3],r.compactApplied.rows[4],r.compactApplied.rows[5]}*(1/r.rowLengths.y),r.n),r.writes,(unsigned long long)r.commitQpc);
  if(r.kind==1&&r.writes==1)writeLog("PREARM_STATUS session=%llu elapsed=%.6f primary_thread=%u DR0_installed=%u DR1_installed=%u trace_already_armed_before_override=%u first_override=1",(unsigned long long)r.session,r.elapsed,r.nativeThread,unsigned(r.prearmed),unsigned(r.prearmed),unsigned(r.prearmed));
  if(r.kind==1&&!r.copy)writeLog("NATIVE_SOURCE_OVERRIDE session=%llu generation=%llu native_ms=%llu native_update=%llu elapsed=%.6f caller_rva=0x%llx source_ptr=0x%llx original_right=(%.9g,%.9g,%.9g) original_up=(%.9g,%.9g,%.9g) original_forward=(%.9g,%.9g,%.9g) target_right=(%.9g,%.9g,%.9g) target_up=(%.9g,%.9g,%.9g) target_forward=(%.9g,%.9g,%.9g) applied_right=(%.9g,%.9g,%.9g) applied_up=(%.9g,%.9g,%.9g) applied_forward=(%.9g,%.9g,%.9g) t=%.6f position_unchanged=%u source_is_stack_local=%u direct_actor_transform_write=0 write_index=%u override_commit_qpc=%llu",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.generation),static_cast<unsigned long long>(r.ms),static_cast<unsigned long long>(r.nativeUpdate),r.elapsed,static_cast<unsigned long long>(r.caller),static_cast<unsigned long long>(r.source),r.original.v[0],r.original.v[1],r.original.v[2],r.original.v[4],r.original.v[5],r.original.v[6],r.original.v[8],r.original.v[9],r.original.v[10],r.target.right.x,r.target.right.y,r.target.right.z,r.target.up.x,r.target.up.y,r.target.up.z,r.target.forward.x,r.target.forward.y,r.target.forward.z,r.applied.v[0],r.applied.v[1],r.applied.v[2],r.applied.v[4],r.applied.v[5],r.applied.v[6],r.applied.v[8],r.applied.v[9],r.applied.v[10],r.t,unsigned(r.positionSame),unsigned(r.stack),r.writes,static_cast<unsigned long long>(r.commitQpc));
  else if(r.kind==2)writeLog("NATIVE_SOURCE_EXIT session=%llu reason=%s duration_since_source_entry=%.6f applied_writes=%u held_updates=%u held_ms=%llu stability_observed=%u direct_actor_transform_write=0",static_cast<unsigned long long>(r.session),r.reason,r.elapsed,r.writes,r.heldUpdates,static_cast<unsigned long long>(r.heldMs),unsigned(r.success));
  else if(r.kind==3)writeLog("NATIVE_SOURCE_ACTOR_OBSERVATION session=%llu native_update=%llu native_ms=%llu actor_up=(%.9g,%.9g,%.9g) actor_up_dot_surface=%.9g actor_up_dot_world=%.9g held_distinct_updates=%u held_ms=%llu stability_observed=%u source=read_only_snapshot_NOT_POST_writer_callback lifecycle_active=1 observation_phase=%s",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.nativeUpdate),static_cast<unsigned long long>(r.ms),r.actorUp.x,r.actorUp.y,r.actorUp.z,r.actorDotSurface,r.actorDotWorld,r.heldUpdates,static_cast<unsigned long long>(r.heldMs),unsigned(r.success),r.reason);
  else if(r.kind==5)writeLog("NATIVE_SOURCE_ENTRY_REJECT session=%llu generation=%llu reason=%s source_override=0",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.generation),r.reason);
  else if(r.kind==4)writeLog("NATIVE_SOURCE_OTHER_WRITER session=%llu caller_return_rva=0x%llx source_up=(%.9g,%.9g,%.9g) override=0",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.caller),r.original.v[4],r.original.v[5],r.original.v[6]);
 }
}
void stop(){kill();if(!stateLock.test_and_set(std::memory_order_acquire)){end("script_stop",GetTickCount64());stateLock.clear(std::memory_order_release);}drainLogs();writeLog("PATH_A_OVERRIDE_COUNT=%u PATH_A_REJECT_COUNT=%u PATH_A_STATE_LOCK_CONTENTION=0 PATH_B_OVERRIDE_COUNT=%u PATH_B_REJECT_COUNT=%u final=1",pathAWrites.load(),pathARejects.load(),pathBWrites.load(),pathBRejects.load());writeLog("native_source_stop source_override=OFF direct_actor_transform_write=0 vanilla_passthrough=1");}
}
