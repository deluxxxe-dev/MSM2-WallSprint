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
#include "NativeHooks/path_a_position_proof.hpp"
#include "Math/planar_standoff.hpp"
namespace {
using ws::Vec3;using ws::source_math::Matrix;using ws::native_source::Token;using msm2::writeLog;
constexpr std::uintptr_t callReturn=0x2cfc8f3,callBegin=0x2cfc850;constexpr unsigned callSize=168,capacity=1024;
constexpr unsigned char callerDigest[32]={0x88,0x50,0x29,0x96,0xd3,0x91,0x73,0xbf,0x38,0x08,0x6e,0x8d,0xda,0xfd,0x9e,0xff,0x28,0x75,0xda,0xda,0x30,0x71,0x7d,0x1c,0x9f,0x46,0x51,0x32,0x4f,0xbe,0x3b,0x3f};
std::array<msm2::writer::Patch,4> lifecyclePatches;unsigned char callerOriginal[168]{};
msm2::native_source::Config config;std::uintptr_t imageBase{};std::atomic<bool> killed{true},isActive{false};bool admitted{},configured{};std::atomic_flag stateLock=ATOMIC_FLAG_INIT;thread_local bool inside{},didOverride{};
struct Published {
 std::uint64_t session{},generation{},lifecycleEpoch{},acceptedEpoch{},publishedMs{},initMs{};std::uintptr_t hero{},transform{},owner{};
 unsigned thread{},nativeUpdate{};float elapsed{},t{};bool active{};ws::diag::ActorAxes basis{};Vec3 surface{};
};
ws::interop::AtomicPublication<Published> publication;
// Expected identity for attribution survives a soft-suspended empty permission.
// This publication is diagnostic metadata only; it never grants a source store.
ws::interop::AtomicPublication<Published> identityPublication;
// Hooks report an expired native token without touching the worker-owned state.
struct StaleSignal {Token token{};std::uint64_t session{},detectedMs{};};
ws::interop::AtomicPublication<StaleSignal> stalePublication;
enum class Ownership {Dormant,Active,StaleSuspended,HardClosed};
struct NativeClose {Token token{};Published source{};bool wasOpen{},sourceRead{};};
ws::interop::AtomicPublication<NativeClose> deinitPublication;
std::atomic<unsigned> pathAWrites{0},pathBWrites{0},pathARejects{0},pathBRejects{0},pathBReservations{0};
std::atomic<bool> resetRequested{false},lifecycleOpen{false};std::atomic<std::uint64_t> lifecycleEpoch{0};
ws::position_proof::Result positionProof;
struct Adhesion {unsigned attempts{},samples{},failClosed{};std::uint64_t lastMs{},lastUpdate{};float maxError{},maxRequested{},maxCandidateTangent{};bool finished{};};
struct Shared {
 std::uintptr_t hero{},transform{},owner{};std::uint64_t session{},generation{},published{};
 ws::source_math::Transition transition;std::uint64_t blocked{},lastWrite{},lastUpdate{},firstHeld{},lastObserveUpdate{};
 Matrix entryBasis{},currentBasis{};Vec3 entryForward{};std::uint64_t lastCommonUpdate{},lastObserveMs{},lastMetricMs{};
 unsigned writes{},commonWrites{},copyWrites{},heldUpdates{},otherWriters{};float elapsed{},lastT{},bestSurface{};unsigned rejects{};bool pass{};
 std::uint64_t initMs{},maxHeldMs{};unsigned maxHeldUpdates{};float lastActorDot{};bool entered{},summaryEmitted{},exitDiagEmitted{},lastActorValid{};const char* exitReason="not_entered";
 Ownership ownership{Ownership::Dormant};unsigned expectedThread{},staleSuspends{},staleResumes{};
 std::uint64_t permissionEpoch{},acceptedEpoch{},staleStart{},staleUpdate{},staleAge{},maxStaleMs{},handledStaleUpdate{};
 double pausedTransitionMs{},frozenTransitionMs{};
 Adhesion adhesion{};
} state;
struct Record {
 unsigned kind{},gateMask{};const char* reason="none";std::uint64_t session{},ms{},generation{},nativeUpdate{},heldMs{},commitQpc{};
 std::uintptr_t source{},caller{};float elapsed{},t{},actorDotSurface{},actorDotWorld{};unsigned writes{},heldUpdates{};Matrix original{},applied{};ws::source_math::CompactBlock compactOriginal{},compactApplied{};ws::diag::ActorAxes target{};Vec3 n{},actorUp{},rowLengths{},positionBefore{};bool positionSame{},stack{},success{},prearmed{},copy{},late{},sourceReadable{},sourceLayout{};unsigned nativeThread{},expectedThread{};const char* tokenStatus="not_read";bool tokenIdentity{};std::uintptr_t rcx{},r8{},r9{},rsp{},stackLimit{},stackBase{};
};
std::array<Record,capacity> records;std::atomic<unsigned> head{0},tail{0};
std::atomic_flag recordLock=ATOMIC_FLAG_INIT;std::atomic<unsigned> recordDrops{0};
struct SecondaryCounters {
 std::atomic<unsigned> seen{0},admitted{0},overridden{0},rejected{0},firstThread{0},lastThread{0},overflowEvents{0},inflight{0};
 std::array<std::atomic<unsigned>,16> threads{};
} secondary;
struct FirstSecondary {std::atomic<unsigned> ready{0};Record record{};};
FirstSecondary firstSecondaryOverride,firstSecondaryReject;
void secondaryLog(const Record& r){
 writeLog("PATH_B_SECONDARY_THREAD_EVENT session=%llu elapsed=%.6f thread_id=%u expected_thread=%u caller_rva=0x%llx target_transform=0x%llx rdx_minus_rsp=%lld stack_verified=%u compact_verified=%u token_status=%s identity_verified=%u source_up_before=(%.9g,%.9g,%.9g) target_up=(%.9g,%.9g,%.9g) action=%s reason=%s expected_native_thread_unchanged=1 position_source_write=0",
  (unsigned long long)r.session,r.elapsed,r.nativeThread,r.expectedThread,(unsigned long long)r.caller,(unsigned long long)r.rcx,(long long)(std::int64_t(r.source-r.rsp)),unsigned(r.stack),unsigned(r.sourceLayout),r.tokenStatus,unsigned(r.tokenIdentity),r.compactOriginal.rows[3],r.compactOriginal.rows[4],r.compactOriginal.rows[5],r.target.up.x,r.target.up.y,r.target.up.z,r.success?"override":"reject",r.reason);
}
void secondarySummary(std::uint64_t session,bool final){
 unsigned unique=0;for(auto& id:secondary.threads)if(id.load())++unique;
 writeLog("PATH_B_SECONDARY_THREAD_SUMMARY session=%llu scope=process seen=%u admitted=%u overridden=%u rejected=%u unique_threads=%u first_thread=%u last_thread=%u unique_thread_cap=16 thread_table_saturated=%u overflow_events=%u inflight=%u event_cap_per_action=16 final=%u",
  (unsigned long long)session,secondary.seen.load(),secondary.admitted.load(),secondary.overridden.load(),secondary.rejected.load(),unique,secondary.firstThread.load(),secondary.lastThread.load(),unsigned(secondary.overflowEvents.load()!=0),secondary.overflowEvents.load(),secondary.inflight.load(),unsigned(final));
}
unsigned reportedA=~0u,reportedB=~0u,reportedAR=~0u,reportedBR=~0u;
bool room(){return head.load(std::memory_order_relaxed)-tail.load(std::memory_order_acquire)<capacity;}
void emit(const Record& r){
 if(recordLock.test_and_set(std::memory_order_acquire)){++recordDrops;return;}
 auto h=head.load(std::memory_order_relaxed);if(h-tail.load(std::memory_order_acquire)<capacity){records[h%capacity]=r;head.store(h+1,std::memory_order_release);}else ++recordDrops;
 recordLock.clear(std::memory_order_release);
}
struct SecondaryCall {
 bool enabled{},admitted{};Record event{};
 void begin(const Published& p,unsigned thread,std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t ret,std::uintptr_t rsp,std::uint64_t now){
  enabled=true;++secondary.seen;++secondary.inflight;unsigned empty=0;secondary.firstThread.compare_exchange_strong(empty,thread);secondary.lastThread=thread;
  bool registered=false;for(auto& id:secondary.threads){auto value=id.load();if(value==thread){registered=true;break;}if(!value&&id.compare_exchange_strong(value,thread)){registered=true;break;}if(value==thread){registered=true;break;}}
  if(!registered)++secondary.overflowEvents;
  event.kind=12;event.session=p.session;event.elapsed=now>=p.initMs?float(now-p.initMs)/1000.f:p.elapsed;event.nativeThread=thread;event.expectedThread=p.thread;event.caller=ret>=imageBase?ret-imageBase:0;event.rcx=rcx;event.source=rdx;event.rsp=rsp;event.target=p.basis;event.reason="secondary_source_inactive";
 }
 void finish(){
  if(!enabled)return;
  enabled=false;
  if(admitted)++secondary.admitted;
  auto index=event.success?secondary.overridden.fetch_add(1):secondary.rejected.fetch_add(1);
  if(index==0){auto& slot=event.success?firstSecondaryOverride:firstSecondaryReject;slot.record=event;slot.ready.store(2,std::memory_order_release);}
  else if(index<16)emit(event);
  --secondary.inflight;
 }
};
bool reservePathB(){auto n=pathBReservations.load();while(n<4096){if(pathBReservations.compare_exchange_strong(n,n+1))return true;}return false;}
void publishInactive() noexcept {Published p{};publication.publish(p);}
void adhesionSummary(const char* reason){
 auto& a=state.adhesion;if(!state.entered||a.finished)return;a.finished=true;
 writeLog("ADHESION_SUMMARY session=%llu position_proof_admitted=0 samples=%u sample_attempts=%u corrections_applied=0 max_abs_error_before=%.9g max_abs_error_after=%.9g error_after_semantics=unmodified_same_observation max_abs_step=0 max_abs_tangent_delta=0 source_position_writes=0 fail_closed_events=%u max_abs_requested_step=%.9g max_abs_candidate_tangent_delta=%.9g sample_cap=64 end_reason=%s mode=position_read_only",
  (unsigned long long)state.session,a.samples,a.attempts,a.maxError,a.maxError,a.failClosed,a.maxRequested,a.maxCandidateTangent,reason);
}
void latchCause(const char*,const char*,const ws::observed::VerifiedWallContext* =nullptr) noexcept;
void end(const char* reason,std::uint64_t now){
 if(state.entered&&state.transition.active&&std::strcmp(reason,"verified_native_Deinit")&&std::strcmp(reason,"new_native_session"))latchCause(reason,"worker");
 if(state.ownership==Ownership::StaleSuspended&&now>=state.staleStart)state.maxStaleMs=std::max(state.maxStaleMs,now-state.staleStart);
 if(state.transition.active)adhesionSummary(reason);
 state.commonWrites=pathAWrites.load();state.copyWrites=pathBWrites.load();state.writes=state.commonWrites+state.copyWrites;
 if(state.transition.active)state.exitReason=reason;
 if(state.transition.active&&room()){Record r{};r.kind=2;r.reason=reason;r.session=state.session;r.generation=state.generation;r.ms=now;r.elapsed=float(now-state.transition.entryMs)/1000;r.writes=state.writes;r.heldUpdates=state.heldUpdates;r.heldMs=state.firstHeld?now-state.firstHeld:0;r.success=state.pass;emit(r);}
 state.transition.active=false;state.ownership=Ownership::HardClosed;state.blocked=state.generation;isActive=false;
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
struct SourceGuards {
 bool input{},copy{},compact{},callerRead{},callerMatch{};unsigned callerDifference{168};std::array<msm2::guards::PatchStatus,4> hooks{};
 const char* failedGate() const noexcept {
  if(!input)return "path_a_writer_auth";
  if(!copy)return "path_b_writer_auth";
  if(!compact)return "path_b_compact_proof";
  if(!callerRead)return "path_a_caller_read";
  if(!callerMatch)return "path_a_caller_patch";
  constexpr const char* names[]={"WallRun_Init_patch","WallRun_Entry_patch","WallRun_Update_patch","WallRun_Deinit_patch"};
  for(unsigned i=0;i<4;++i){if(!hooks[i].good())return names[i];}
  return "none";
 }
 bool good() const noexcept {return !std::strcmp(failedGate(),"none");}
};
SourceGuards sourceGuardStatus(){
 SourceGuards g{};g.input=msm2::input::authenticated();g.copy=!config.dualSource||msm2::reset_copy::authenticated();g.compact=!config.dualSource||msm2::reset_copy::compactProofAuthenticated();
 unsigned char bytes[168]{};g.callerRead=msm2::readMemory(imageBase+callBegin,bytes,sizeof(bytes));
 if(g.callerRead){g.callerMatch=true;for(unsigned i=0;i<sizeof(bytes);++i)if(bytes[i]!=callerOriginal[i]){g.callerMatch=false;g.callerDifference=i;break;}}
 for(unsigned i=0;i<4;++i){const auto& p=lifecyclePatches[i];g.hooks[i]=msm2::guards::inspectPatch(p.address!=0,p.address,p.bytes.data(),msm2::readMemory);}
 return g;
}
bool currentGuards(){return sourceGuardStatus().good();}
struct WorkerEvidence {ws::observed::VerifiedWallContext context{};msm2::guards::ObservationStatus observation{};bool available{},inputAuthenticated{};};
ws::interop::AtomicPublication<WorkerEvidence> workerEvidence;
struct EntryEvidence {std::uintptr_t rcx{},rdx{},rsp{},ret{};};
thread_local EntryEvidence entryEvidence;
struct HardCause {EntryEvidence entry{};const char* reason="none";const char* origin="none";std::uint64_t ms{};unsigned thread{};Published source{};Token token{};bool sourceRead{},identityRetained{},nativeRead{};WorkerEvidence worker{};SourceGuards guards{};};
HardCause firstCause;std::atomic<unsigned> causeState{0},killCalls{0}; // 0 empty,1 claimed,2 ready,3 logged; never reused in production.
void latchCause(const char* reason,const char* origin,const ws::observed::VerifiedWallContext* ctx) noexcept {
 ++killCalls;unsigned expected=0;if(!causeState.compare_exchange_strong(expected,1))return;
 // Close write permission before diagnostic reads; keep the identity snapshot
 // intact until it has been captured. A callback never waits for this record.
 lifecycleOpen=false;isActive=false;
 HardCause c{};c.entry=(!std::strcmp(origin,"path_a")||!std::strcmp(origin,"path_b"))?entryEvidence:EntryEvidence{};c.reason=reason;c.origin=origin;c.ms=GetTickCount64();c.thread=GetCurrentThreadId();c.sourceRead=publication.read(c.source);
 if(!c.source.session){Published retained{};if(identityPublication.read(retained)&&retained.session){c.source=retained;c.identityRetained=true;}}
 workerEvidence.read(c.worker);if(ctx){c.worker.context=*ctx;c.worker.available=true;}
 c.nativeRead=ws::native_source::lifecycle(c.token);c.guards=sourceGuardStatus();firstCause=c;causeState.store(2,std::memory_order_release);
}
void drainCause(){
 unsigned ready=2;if(!causeState.compare_exchange_strong(ready,3,std::memory_order_acq_rel))return;
 const auto& c=firstCause;const auto& p=c.source;const auto& w=c.worker;const auto& o=w.observation;const auto& g=c.guards;
 auto session=p.session?p.session:w.context.wallRunSessionId;double elapsed=p.initMs&&c.ms>=p.initMs?double(c.ms-p.initMs)/1000.:w.context.elapsed;
 writeLog("NATIVE_SOURCE_HARD_KILL session=%llu elapsed=%.6f reason=%s origin=%s thread_id=%u wallrun_active=%u wallrun_session=%llu owner=0x%llx hero=0x%llx transform=0x%llx native_update=%u native_thread=%u expected_thread=%u native_generation=%llu accepted_epoch=%llu collision_epoch=%llu token_read_status=%s source_snapshot_read=%u source_identity_retained=%u worker_context_available=%u verified_available=%u field_plan_enabled=%u observation_failed_hook=%s observation_failure_mask=0x%x input_auth=%u copy_auth=%u compact_proof=%u caller_read_ok=%u caller_patch_match=%u caller_first_difference=%u source_failed_gate=%s init_read_ok=%u init_patch_match=%u init_first_difference=%u entry_read_ok=%u entry_patch_match=%u entry_first_difference=%u update_read_ok=%u update_patch_match=%u update_first_difference=%u deinit_read_ok=%u deinit_patch_match=%u deinit_first_difference=%u native_owner=0x%llx expected_generation=%llu expected_accepted_epoch=%llu native_init_ms=%llu expected_init_ms=%llu token_phase=%u token_last_kind=%u entry_rcx=0x%llx entry_source=0x%llx entry_rsp=0x%llx entry_return=0x%llx immutable_first_cause=1 snapshot_scope=first_claim_separate_guarded_reads",
  (unsigned long long)session,elapsed,c.reason,c.origin,c.thread,unsigned(w.available?w.context.wallRunActive:c.token.phase==3),(unsigned long long)(w.available?w.context.wallRunSessionId:session),
  (unsigned long long)(p.owner?p.owner:c.token.owner),(unsigned long long)p.hero,(unsigned long long)p.transform,c.token.updates,c.token.thread,p.thread,(unsigned long long)c.token.generation,(unsigned long long)c.token.acceptedEventEpoch,(unsigned long long)c.token.collisionEpoch,ws::native_source::lifecycleReadName(c.token.readStatus),unsigned(c.sourceRead),unsigned(c.identityRetained),unsigned(w.available),unsigned(o.verifiedAvailable),unsigned(o.fieldPlanEnabled),w.available?o.failedHook():"unavailable",w.available?o.failureMask():0,
  unsigned(g.input),unsigned(g.copy),unsigned(g.compact),unsigned(g.callerRead),unsigned(g.callerMatch),g.callerDifference,g.failedGate(),
  unsigned(g.hooks[0].readOk),unsigned(g.hooks[0].match),g.hooks[0].firstDifference,unsigned(g.hooks[1].readOk),unsigned(g.hooks[1].match),g.hooks[1].firstDifference,unsigned(g.hooks[2].readOk),unsigned(g.hooks[2].match),g.hooks[2].firstDifference,unsigned(g.hooks[3].readOk),unsigned(g.hooks[3].match),g.hooks[3].firstDifference,(unsigned long long)c.token.owner,(unsigned long long)p.generation,(unsigned long long)p.acceptedEpoch,(unsigned long long)c.token.initMs,(unsigned long long)p.initMs,c.token.phase,c.token.lastKind,(unsigned long long)c.entry.rcx,(unsigned long long)c.entry.rdx,(unsigned long long)c.entry.rsp,(unsigned long long)c.entry.ret);
}
void disableRuntime() noexcept{msm2::path_b::kill();killed=true;lifecycleOpen=false;++lifecycleEpoch;ws_native_source_enabled=0;isActive=false;publishInactive();ws::native_source::armLifecycle(false);}
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
// Every owned entry checks the native publication, including immediately before
// the stack store. Worker publication age alone cannot establish native freshness.
struct EntryTokenStatus {const char* status="not_read";const char* failure="secondary_token_incoherent";};
bool entryToken(const Published& p,const char* path,EntryTokenStatus* evidence=nullptr) noexcept {
 Token t{};bool coherent=ws::native_source::lifecycle(t);auto now=GetTickCount64();
 if(evidence)evidence->status=ws::native_source::lifecycleReadName(t.readStatus);
 if(!coherent){if(t.observed)msm2::native_source::kill("entry_token_incoherent",path);return false;}
 if(t.owner!=p.owner||t.generation!=p.generation||t.initMs!=p.initMs||t.thread!=p.thread||
    t.acceptedEventEpoch!=p.acceptedEpoch||t.collisionEpoch!=p.acceptedEpoch||now<t.initMs||now<t.updateMs){if(evidence)evidence->failure="secondary_identity_epoch_mismatch";msm2::native_source::kill("token_identity_epoch_mismatch",path);return false;}
 if(!freshToken(t,now)){if(evidence)evidence->failure="secondary_token_stale";stalePublication.publish({t,p.session,now});return false;}
 if(evidence)evidence->failure="secondary_source_publication_changed";
 return now>=p.publishedMs&&now-p.publishedMs<=100&&!killed.load()&&lifecycleOpen.load()&&p.lifecycleEpoch==lifecycleEpoch.load();
}
bool secondaryContext(const Published& p,const char*& reason){
 WorkerEvidence w{};
 if(!workerEvidence.read(w)||!w.available){reason="secondary_context_unavailable";return false;}
 const auto& c=w.context;
 if(!c.lifecycleVerified||!c.wallRunActive||c.wallRunSessionId!=p.session||c.wallRunOwner!=p.owner){reason="secondary_identity_epoch_mismatch";msm2::native_source::kill(reason,"path_b");return false;}
 return true;
}
bool suspendStale(const Token& t,std::uint64_t detected){
 if(state.ownership!=Ownership::Active)return state.ownership==Ownership::StaleSuspended;
 // Only our own soft close can advance the permission epoch. Never adopt a
 // concurrent native Deinit/invalid sequence epoch as permission to resume.
 auto expected=state.permissionEpoch;
 if(!lifecycleEpoch.compare_exchange_strong(expected,expected+1))return false;
 ++state.permissionEpoch;lifecycleOpen=false;publishInactive();isActive=false;
 state.ownership=Ownership::StaleSuspended;++state.staleSuspends;
 state.staleStart=t.updateMs+100;state.staleUpdate=t.updates;state.handledStaleUpdate=t.updates;
 state.staleAge=detected>=t.updateMs?detected-t.updateMs:0;
 state.frozenTransitionMs=state.transition.entryMs+double(state.lastT)*config.rotateTime*1000.;
 state.firstHeld=0;state.heldUpdates=0;state.lastObserveMs=0;
 if(state.staleSuspends<=16)writeLog("NATIVE_SOURCE_STALE_SUSPEND session=%llu elapsed=%.6f update_age_ms=%llu last_native_update=%u last_native_update_ms=%llu owner=0x%llx generation=%llu thread=%u actor_up_dot_surface=%.9g actor_observation_valid=%u t=%.6f episode=%u",
  (unsigned long long)state.session,state.elapsed,(unsigned long long)state.staleAge,t.updates,(unsigned long long)t.updateMs,(unsigned long long)state.owner,(unsigned long long)state.generation,state.expectedThread,state.lastActorDot,unsigned(state.lastActorValid),state.lastT,state.staleSuspends);
 return true;
}
void configurePositionEvidence(bool verified,bool callerVerified){
 std::array<unsigned char,584> original{};bool writerVerified=verified&&msm2::input::originalWriterForPositionProof(original);positionProof={};
 if(callerVerified&&writerVerified)positionProof=ws::position_proof::inspect(original.data(),original.size(),callerOriginal,sizeof(callerOriginal));
 writeLog("PATH_A_POSITION_PROOF admitted=0 caller_verified=%u writer_verified=%u writer_decoded_bytes=%u caller_decoded_bytes=%u exact_direct_call_verified=%u source_offsets=%s destination_offsets=%s stack_relation_verified=%u target_relation_verified=0 target_relation_scope=runtime_exact_Hero_RCX_only position_w_semantics=unproven candidate_direct_component_mask=0x%x source_position_reads=%u target_position_stores=%u branches=%u calls=%u failure_offset=0x%x reason=%s position_source_write=0",
  unsigned(callerVerified),unsigned(writerVerified),positionProof.writerBytes,positionProof.callerBytes,unsigned(positionProof.callVerified),positionProof.sourceReads?"candidate_[0x30,0x40)":"unproven",positionProof.targetStores?"candidate_[0x30,0x40)":"unproven",unsigned(positionProof.stackRelation),positionProof.directMask,positionProof.sourceReads,positionProof.targetStores,positionProof.branches,positionProof.calls,positionProof.failureOffset,positionProof.reason);
 for(unsigned i=0;i<positionProof.accessCount;++i){auto& a=positionProof.accesses[i];writeLog("PATH_A_POSITION_ACCESS index=%u instruction_offset=0x%x base=%s displacement=0x%llx width=%u direction=%s source_offset_candidate=%lld direct_copy_candidate=%u after_control_flow=%u proof_status=CANDIDATE_LINEAR_SCAN_NOT_PATH_COMPLETE suppressed=%u",i,a.offset,a.target?"entry_RCX_alias_candidate":"entry_RDX_alias_candidate",(unsigned long long)a.displacement,a.width,a.write?"write":"read",a.sourceOffset,unsigned(a.directCandidate),unsigned(a.afterControlFlow),positionProof.suppressed);}
 for(unsigned i=0;i<positionProof.controlCount;++i){auto& c=positionProof.controls[i];writeLog("PATH_A_POSITION_CONTROL index=%u instruction_offset=0x%x kind=%s direct=%u target_rva=0x%llx effects_verified=0 suppressed=%u",i,c.offset,c.call?"call":"branch",unsigned(c.direct),(unsigned long long)(c.direct?c.targetRva:0),positionProof.suppressedControls);}
}
void adhesionSample(const Published& p,const msm2::Snapshot& hero,std::uint64_t now){
 auto& a=state.adhesion;
 if(!p.active||!state.entered||a.finished||a.attempts>=64||(a.attempts&&(now-a.lastMs<100||p.nativeUpdate==a.lastUpdate)))return;
 a.lastMs=now;a.lastUpdate=p.nativeUpdate;++a.attempts;
 Vec3 point{},normal{};const char* reason="lifecycle_or_identity_changed";bool identity=false;ws::adhesion::Evaluation e{};float tangent=0;
 auto open=[&]{return !killed.load()&&lifecycleOpen.load()&&lifecycleEpoch.load()==p.lifecycleEpoch&&hero.hero==p.hero&&hero.transform==p.transform&&p.owner==state.owner&&p.session==state.session&&p.generation==state.generation;};
 if(open()&&p.owner<=UINTPTR_MAX-0x3a4){
  bool pointOk=msm2::readMemory(p.owner+0x338,&point,12);bool normalOk=open()&&msm2::readMemory(p.owner+0x398,&normal,12);
  Token checked{};identity=open()&&ws::native_source::lifecycle(checked)&&freshToken(checked,GetTickCount64())&&checked.owner==p.owner&&checked.generation==p.generation&&checked.thread==p.thread;
  if(identity){reason=!pointOk?"P338_unreadable":!normalOk?"N398_unreadable":"invalid_plane_or_position";
   if(pointOk&&normalOk){e=ws::adhesion::evaluate({hero.position[0],hero.position[1],hero.position[2]},point,normal,p.t);reason=e.reason;
    if(e.valid&&ws::dot(e.normal,p.surface)<.995f){e.valid=false;reason="plane_normal_changed_from_owned_surface";}
    if(e.valid){++a.samples;a.maxError=std::max(a.maxError,std::abs(e.error));a.maxRequested=std::max(a.maxRequested,std::abs(e.requested));
     Vec3 forward{},right{};if(ws::unit(p.basis.forward-e.normal*ws::dot(p.basis.forward,e.normal),forward)&&ws::unit(ws::cross(e.normal,forward),right)){tangent=std::max(std::abs(ws::dot(e.delta,right)),std::abs(ws::dot(e.delta,forward)));a.maxCandidateTangent=std::max(a.maxCandidateTangent,tangent);}
    }
   }
  }
 }
 if(!identity||!e.valid||!e.inRange)++a.failClosed;
 writeLog("ADHESION_SAMPLE session=%llu elapsed=%.6f native_update=%u valid=%u identity_revalidated=%u HeroPosition=(%.9g,%.9g,%.9g) P338=(%.9g,%.9g,%.9g) N398=(%.9g,%.9g,%.9g) distance=%.9g target_distance=0.5 error=%.9g correction_requested=%.9g correction_applied=0 position_source_write=0 candidate_tangent_delta=%.9g reason=%s mode=position_read_only sampling=separate_guarded_observations",
  (unsigned long long)p.session,p.elapsed,p.nativeUpdate,unsigned(identity&&e.valid),unsigned(identity),hero.position[0],hero.position[1],hero.position[2],point.x,point.y,point.z,normal.x,normal.y,normal.z,e.distance,e.error,e.valid?e.requested:0,tangent,reason);
}
void exitDiagnostic(const ws::observed::VerifiedWallContext& ctx,const Token& t,bool native,std::uint64_t now,const char* gate){
 if(!state.transition.active||state.exitDiagEmitted)return;
 state.exitDiagEmitted=true;msm2::Snapshot s{};bool sampled=msm2::sample(s);
 writeLog("NATIVE_LIFECYCLE_EXIT_DIAG session=%llu elapsed=%.6f failing_sub_gate=%s ctx_wallrun_active=%u ctx_wallrun_session=%llu ctx_wallrun_owner=0x%llx lifecycle_return_native=%u token_observed=%u lifecycle_read_status=%s token_phase=%u token_last_kind=%u token_owner=0x%llx token_thread=%u token_updates=%u token_generation=%llu token_init_ms=%llu token_update_ms=%llu now_ms=%llu update_age_ms=%llu update_age_valid=%u fresh_phase_ok=%u fresh_owner_ok=%u fresh_updates_ok=%u fresh_time_order_ok=%u fresh_update_age_ok=%u owner_matches_ctx=%u hero_matches=%u transform_matches=%u hero_sample_valid=%u source_transition_active=%u accepted_event_epoch=%llu native_collision_epoch=%llu",
  (unsigned long long)state.session,ctx.elapsed,gate,unsigned(ctx.wallRunActive),(unsigned long long)ctx.wallRunSessionId,(unsigned long long)ctx.wallRunOwner,unsigned(native),unsigned(t.observed),ws::native_source::lifecycleReadName(t.readStatus),t.phase,t.lastKind,(unsigned long long)t.owner,t.thread,t.updates,(unsigned long long)t.generation,(unsigned long long)t.initMs,(unsigned long long)t.updateMs,(unsigned long long)now,(unsigned long long)(now>=t.updateMs?now-t.updateMs:0),unsigned(now>=t.updateMs),unsigned(t.phase==3),unsigned(t.owner!=0),unsigned(t.updates!=0),unsigned(now>=t.initMs&&now>=t.updateMs),unsigned(now>=t.updateMs&&now-t.updateMs<=100),unsigned(t.owner==ctx.wallRunOwner),unsigned(sampled&&s.hero==state.hero),unsigned(sampled&&s.transform==state.transform),unsigned(sampled),unsigned(state.transition.active),(unsigned long long)t.acceptedEventEpoch,(unsigned long long)t.collisionEpoch);
}
void lifecycleSummary(std::uint64_t finish,bool verifiedDeinit,bool untilDeinit){
 if(!state.entered||state.summaryEmitted)return;
 state.summaryEmitted=true;secondarySummary(state.session,false);
 writeLog("ORIENTATION_LIFECYCLE_SUMMARY session=%llu wallrun_duration_ms=%llu source_entry_elapsed=%.6f source_exit_reason=%s source_active_until_deinit=%u verified_deinit=%u path_a_writes=%u path_b_writes=%u retention_pass=%u max_held_ms=%llu max_held_updates=%u last_actor_up_dot_surface=%.9g last_actor_observation_valid=%u last_actor_observation_ms=%llu last_actor_observation_scope=active_source_worker_snapshot stale_suspend_count=%u stale_resume_count=%u max_stale_duration_ms=%llu stale_log_pair_cap=16 first_hard_kill=%s hard_kill_calls=%u",
  (unsigned long long)state.session,(unsigned long long)(finish>=state.initMs?finish-state.initMs:0),double(state.transition.entryMs-state.initMs)/1000.,state.exitReason,unsigned(verifiedDeinit&&untilDeinit),unsigned(verifiedDeinit),pathAWrites.load(),pathBWrites.load(),unsigned(state.pass),(unsigned long long)state.maxHeldMs,state.maxHeldUpdates,state.lastActorDot,unsigned(state.lastActorValid),(unsigned long long)state.lastObserveMs,state.staleSuspends,state.staleResumes,(unsigned long long)state.maxStaleMs,causeState.load(std::memory_order_acquire)>=2?firstCause.reason:"none",killCalls.load());
}
void drainVerifiedDeinit(){
 if(!state.entered||state.summaryEmitted)return;
 NativeClose closed{};if(!deinitPublication.read(closed)||closed.token.lastKind!=3||closed.token.phase!=0||closed.token.generation!=state.generation+1||closed.token.owner!=state.owner||closed.token.initMs!=state.initMs)return;
 bool until=closed.token.acceptedEventEpoch==state.acceptedEpoch&&closed.token.collisionEpoch==state.acceptedEpoch&&closed.token.thread==state.expectedThread&&closed.token.eventMs>=closed.token.updateMs&&closed.token.eventMs-closed.token.updateMs<=100&&state.transition.active&&closed.wasOpen&&closed.sourceRead&&closed.source.active&&closed.source.session==state.session&&closed.source.generation==state.generation&&closed.source.owner==state.owner&&closed.source.hero==state.hero&&closed.source.transform==state.transform&&closed.token.eventMs>=closed.source.publishedMs&&closed.token.eventMs-closed.source.publishedMs<=100;
 writeLog("NATIVE_SOURCE_CLOSE session=%llu reason=verified_Deinit normal_session_close=1 source_active_until_deinit=%u",(unsigned long long)state.session,unsigned(until));
 end("verified_native_Deinit",closed.token.eventMs);lifecycleSummary(closed.token.eventMs,true,until);
}
bool observe(const msm2::Snapshot& s,const Token& token,std::uint64_t now,bool mayAbort){
 if(!state.transition.active||!state.writes)return true;
 auto axes=ws::diag::actorAxes(s.prefix);Vec3 up{};
 bool valid=axes.valid&&ws::unit(axes.up,up);
 float ds=valid?ws::dot(up,state.transition.surface):0,dw=valid?up.y:0;state.bestSurface=std::max(state.bestSurface,ds);
 state.lastActorDot=ds;state.lastActorValid=valid;
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
 state.maxHeldMs=std::max(state.maxHeldMs,state.firstHeld?now-state.firstHeld:std::uint64_t(0));state.maxHeldUpdates=std::max(state.maxHeldUpdates,state.heldUpdates);
 // An observed reset invalidates retention, NOT source ownership. The old
 // abort here allowed subsequent exact valid WorldUp calls through untouched.
 if(room()&&(distinct||!valid||ds<.98f||now-state.lastMetricMs>=50)){state.lastMetricMs=now;Record r{};r.kind=3;r.reason=mayAbort?"verified_native_update_worker":"supplementary_worker";r.session=state.session;r.generation=state.generation;r.ms=now;r.nativeUpdate=token.updates;r.actorUp=up;r.n=state.transition.surface;r.actorDotSurface=ds;r.actorDotWorld=dw;r.heldMs=state.firstHeld?now-state.firstHeld:0;r.heldUpdates=state.heldUpdates;r.success=state.pass;emit(r);}
 return true;
}
}
extern "C" {std::atomic<unsigned> ws_native_source_enabled{0};}
namespace msm2::native_source {
void configure(Config c,bool verified,std::uintptr_t base,std::size_t size,const std::array<msm2::writer::Patch,4>& patches){
 disableRuntime();if(configured){writeLog("native_source_rearm_refused full_game_restart_required=1");return;}configured=true;config=c;lifecyclePatches=patches;imageBase=base;
 bool callerVerified=verified&&base&&size>callReturn&&callerAuthenticated();
 configurePositionEvidence(verified,callerVerified);
 admitted=verified&&c.enabled&&c.sourceOverride&&callerVerified&&std::isfinite(c.delay)&&c.delay>=.7f&&c.delay<=5&&std::isfinite(c.rotateTime)&&c.rotateTime>=.2f&&c.rotateTime<=1&&currentGuards()&&(!c.dualSource||msm2::reset_copy::compactProofAuthenticated());
 writeLog("native_source_config enabled=%u requested_override=%u admitted=%u direct_actor_transform_write=0 adhesion=0 movement=0 camera=0 animation=0 source_write_range=[0x00,0x30) position_write=0 caller_rva=0x2cfc8f3 caller_body_hash_verified=%u source_stack_relation=writer_entry_RSP+0x28 delay=%.3f rotate_time=%.3f",unsigned(c.enabled),unsigned(c.sourceOverride),unsigned(admitted),unsigned(callerVerified),c.delay,c.rotateTime);
 writeLog("DUAL_SOURCE_ADMISSION admitted=%u dual_source_required=%u common_caller=0x2cfc8f3 copy_caller=0x2cfc84b copy_hook_authenticated=%u",unsigned(admitted),unsigned(c.dualSource),unsigned(msm2::reset_copy::authenticated()));
 writeLog("NATIVE_SOURCE_GUARD_STATUS admitted=%u first_cause=none first_cause_scope=process source_code_integrity_required=1 diagnostic_field_plan_runtime_required=0 normal_Deinit_is_session_close=1",unsigned(admitted));
 writeLog("SOURCE_LIFECYCLE_CONTRACT readers_lock_native_events=0 native_event_collision_fail_closed=1 native_update_freshness_ms=100 worker_publication_freshness_ms=100 stale_policy=suspend_until_same_identity_real_Update transition_recovery=freeze_last_published_progress stale_episode_log_cap=16 clock_sample=after_token_acquisition verified_Deinit_summary=1 diagnostic_window_limits_source_ownership=0");
 path_b::configure(admitted,true);
 if(!admitted)return;
 state={};pathAWrites=0;pathBWrites=0;pathBReservations=0;pathARejects=0;pathBRejects=0;resetRequested=false;lifecycleOpen=false;publishInactive();killed=false;ws::native_source::armLifecycle(true);ws_native_source_enabled=1;
}
void prearm(const Token& token) noexcept {
 if(killed.load()||!admitted)return;
 auto now=GetTickCount64();if(freshToken(token,now))prearmPublished(token,now);
}
void lifecycleClosed(const Token& t) noexcept {
 NativeClose closed{};closed.token=t;closed.wasOpen=lifecycleOpen.exchange(false);auto epoch=lifecycleEpoch.fetch_add(1);
 if(t.lastKind==3){closed.sourceRead=publication.read(closed.source)&&closed.source.lifecycleEpoch==epoch;deinitPublication.publish(closed);}
 publishInactive();isActive=false;
}
void kill(const char* reason,const char* origin,const ws::observed::VerifiedWallContext* ctx) noexcept {
 // Never leave source permission open while collecting a fault record.
 killed=true;lifecycleOpen=false;isActive=false;latchCause(reason,origin,ctx);disableRuntime();
}
bool workerGuards(const ws::observed::VerifiedWallContext& ctx,const guards::ObservationStatus& status){
 WorkerEvidence evidence{};evidence.context=ctx;evidence.observation=status;evidence.available=true;evidence.inputAuthenticated=msm2::input::authenticated();workerEvidence.publish(evidence);
 // Field-plan/diagnostic viability has no role in authenticated source rows.
 // Exact original admission at startup and all runtime code identities remain.
 if(!status.diagnosticViable()){reset_trace::kill();trace::kill();}
 if(!status.sourceIntegrity()){kill("worker_observation_plan_lost","worker",&ctx);msm2::input::kill();return false;}
 if(!evidence.inputAuthenticated){kill("worker_input_auth_lost","worker",&ctx);return false;}
 return true;
}
bool active() noexcept{return !killed.load()&&isActive.load();}
bool observationGuardsAuthenticated() noexcept{return !killed.load()&&admitted&&currentGuards();}
bool entryOverridden() noexcept{return didOverride;}
void tick(const ws::observed::VerifiedWallContext& ctx,const char* reason,bool authenticated){
 (void)reason;
 if(!authenticated){kill("worker_tick_auth_lost","worker",&ctx);}
 if(stateLock.test_and_set(std::memory_order_acquire))return;
 auto now=GetTickCount64();drainVerifiedDeinit();auto epoch=lifecycleEpoch.load();
 if(killed.load()&&causeState.load(std::memory_order_acquire)<2){publishInactive();stateLock.clear(std::memory_order_release);return;}
 if(killed.load()||!admitted){end(causeState.load(std::memory_order_acquire)>=2?firstCause.reason:!admitted?"source_not_admitted":"disabled_before_cause_ready",now);state.published=0;stateLock.clear(std::memory_order_release);return;}
 Token token{};bool native=ws::native_source::lifecycle(token);now=GetTickCount64(); // Age is measured AFTER acquiring the token, never before a concurrent Update.
 if(!native&&!token.observed){
  // A bounded coherent-read miss cannot renew the snapshot. Existing writes
  // expire at100ms; a sustained miss is diagnosed and closes ownership.
  if(state.entered&&state.published&&now>=state.published&&now-state.published>100){exitDiagnostic(ctx,token,native,now,"lifecycle_publication_unavailable_over_100ms");end("native_lifecycle_publication_unavailable",now);state.published=0;}
  stateLock.clear(std::memory_order_release);return;
 }
 const char* failed=!ctx.lifecycleVerified?"ctx_lifecycle_unverified":!ctx.wallRunActive?"ctx_wallrun_inactive":!native?ws::native_source::lifecycleReadName(token.readStatus):
  token.phase!=3?"fresh_phase":!token.owner?"fresh_owner":!token.updates?"fresh_updates":now<token.initMs||now<token.updateMs?"fresh_time_order":token.owner!=ctx.wallRunOwner?"owner_matches_ctx":nullptr;
 auto hardClose=[&](const char* gate){exitDiagnostic(ctx,token,native,now,gate);end(gate,now);state.published=0;stateLock.clear(std::memory_order_release);};
 if(failed){hardClose(failed);return;}
 msm2::Snapshot s{};
 if(!msm2::sample(s)){hardClose("hero_snapshot_invalid");return;}
 if(state.session&&state.session==ctx.wallRunSessionId&&(state.generation!=token.generation||state.owner!=token.owner)){hardClose("same_session_native_generation_or_owner_changed");return;}
 if(state.session&&state.session!=ctx.wallRunSessionId&&state.generation==token.generation){hardClose("worker_session_changed_without_native_generation");return;}
 if(state.session!=ctx.wallRunSessionId){
  end("new_native_session",now);++epoch;lifecycleSummary(now,false,false);state={};
  state.generation=token.generation;state.session=ctx.wallRunSessionId;state.initMs=token.initMs;state.hero=s.hero;state.transform=s.transform;state.owner=token.owner;
  state.permissionEpoch=epoch;state.acceptedEpoch=token.acceptedEventEpoch;state.expectedThread=token.thread;
  pathAWrites=0;pathBWrites=0;pathBReservations=0;pathARejects=0;pathBRejects=0;resetRequested=false;
 }
 if(state.ownership==Ownership::HardClosed){stateLock.clear(std::memory_order_release);return;}
 if(s.hero!=state.hero||s.transform!=state.transform){hardClose(s.hero!=state.hero?"hero_identity_changed":"transform_identity_changed");return;}
 if(token.acceptedEventEpoch!=state.acceptedEpoch||token.collisionEpoch!=state.acceptedEpoch||token.thread!=state.expectedThread||token.initMs!=state.initMs){hardClose("native_epoch_thread_or_init_changed");return;}
 if(lifecycleEpoch.load()!=state.permissionEpoch){drainVerifiedDeinit();if(state.summaryEmitted){stateLock.clear(std::memory_order_release);return;}hardClose("native_permission_epoch_changed");return;}
 if(!currentGuards()){hardClose("current_hook_or_caller_integrity_lost");return;}
 Vec3 n{};bool normalOk=token.owner<=UINTPTR_MAX-0x3a4&&msm2::readMemory(token.owner+0x398,&n,12)&&ws::finite(n);
 if(state.entered){float length=normalOk?std::sqrt(ws::dot(n,n)):0;
  if(length<.95f||length>1.05f||std::abs(n.y)>length*.20f||ws::dot(n,state.transition.surface)<length*.995f){hardClose("invalid_owned_surface_normal");return;}}
 state.elapsed=float(ctx.elapsed);
 StaleSignal signal{};
 if(state.ownership==Ownership::Active&&stalePublication.read(signal)&&signal.session==state.session&&signal.token.owner==state.owner&&signal.token.generation==state.generation&&signal.token.updates>state.handledStaleUpdate&&signal.token.acceptedEventEpoch==state.acceptedEpoch&&signal.token.thread==state.expectedThread){
  if(!suspendStale(signal.token,signal.detectedMs)){hardClose("native_permission_epoch_changed");return;}
 }
 if(now-token.updateMs>100){
  if(state.ownership==Ownership::Active&&!suspendStale(token,now)){hardClose("native_permission_epoch_changed");return;}
  if(state.ownership==Ownership::StaleSuspended){state.staleAge=std::max(state.staleAge,now-token.updateMs);state.maxStaleMs=std::max(state.maxStaleMs,now-state.staleStart);}
  stateLock.clear(std::memory_order_release);return;
 }
 if(state.ownership==Ownership::StaleSuspended){
  if(token.lastKind!=2||token.updates<=state.staleUpdate){stateLock.clear(std::memory_order_release);return;}
  auto duration=now-state.staleStart;state.maxStaleMs=std::max(state.maxStaleMs,duration);++state.staleResumes;
  // Preserve the last interpolation output; only its virtual clock pauses.
  // Original entry/session timing and the real 5s budget remain unchanged.
  state.pausedTransitionMs=double(now)-state.frozenTransitionMs;
  state.ownership=Ownership::Active;isActive=true;
  if(state.staleResumes<=16)writeLog("NATIVE_SOURCE_STALE_RESUME session=%llu stale_duration_ms=%llu previous_update_age_ms=%llu new_update_age_ms=%llu native_update=%u native_gap_ms=%llu owner_match=1 hero_match=1 transform_match=1 generation_match=1 thread_match=1 accepted_epoch_match=1 transition_restarted=0 t=%.6f episode=%u",
   (unsigned long long)state.session,(unsigned long long)duration,(unsigned long long)state.staleAge,(unsigned long long)(now-token.updateMs),token.updates,(unsigned long long)(token.updateMs-(state.staleStart-100)),state.lastT,state.staleResumes);
 }
 epoch=state.permissionEpoch;
 state.published=now;state.elapsed=float(ctx.elapsed);
 if(!state.transition.active&&ctx.elapsed>=config.delay&&normalOk){
  Matrix baseline{};std::memcpy(baseline.v,s.prefix,48);baseline.v[12]=s.position[0];baseline.v[13]=s.position[1];baseline.v[14]=s.position[2];baseline.v[15]=1;
  if(!state.transition.enter(baseline,n,double(now))){end("invalid_entry_normal_or_forward",now);stateLock.clear(std::memory_order_release);return;}
  state.entryBasis=baseline;state.lastObserveUpdate=token.updates;state.entered=true;state.ownership=Ownership::Active;state.exitReason="active";isActive=true;
 }
 Published p{};p.session=state.session;p.generation=state.generation;p.lifecycleEpoch=epoch;p.acceptedEpoch=state.acceptedEpoch;p.publishedMs=now;p.initMs=token.initMs;p.hero=state.hero;p.transform=state.transform;p.owner=state.owner;p.thread=token.thread;p.nativeUpdate=token.updates;p.elapsed=float(now-token.initMs)/1000.f;
 if(state.transition.active&&!normalOk){end("owned_surface_normal_unreadable",now);}
 if(state.transition.active&&normalOk){Matrix sampled{};float t{};Matrix incoming=state.entryBasis;
  if(now-state.transition.entryMs>=5000){end("bounded_test_window_5seconds",now);}
  else if(state.transition.sample(incoming,n,double(now)-state.pausedTransitionMs,config.rotateTime,sampled,t)){p.active=true;p.t=t;p.surface=state.transition.surface;p.basis=ws::diag::actorAxes(sampled.v);state.currentBasis=sampled;state.lastT=t;lifecycleOpen=true;}
  else {end("invalid_owned_surface_or_transition",now);}
 }
 // A synchronous native close racing this worker must win. Its epoch cannot
 // be adopted for a token acquired before the close, even on first entry.
 if(killed.load()||lifecycleEpoch.load()!=epoch){publishInactive();isActive=false;stateLock.clear(std::memory_order_release);return;}
 identityPublication.publish(p);publication.publish(p);
 // Retention is sampled only from a fresh verified lifecycle update after both
 // source paths have committed. Distinct native updates cannot be inflated by calls.
 state.commonWrites=pathAWrites.load();state.copyWrites=pathBWrites.load();state.writes=state.commonWrites+state.copyWrites;
 if(p.active&&p.t>=1&&state.commonWrites&&(!config.dualSource||state.copyWrites))observe(s,token,now,true);
 adhesionSample(p,s,now);
 stateLock.clear(std::memory_order_release);
}
void capturePath(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t ret,std::uintptr_t originalRsp) noexcept {
 didOverride=false;if(inside||killed.load()||!ws_native_source_enabled.load())return;
 auto error=GetLastError();inside=true;auto release=[&]{inside=false;SetLastError(error);};auto now=GetTickCount64();Published p{};entryEvidence={rcx,rdx,originalRsp,ret};
 if(ret!=imageBase+callReturn){
  if(ret>=imageBase+callBegin&&ret<imageBase+callBegin+callSize&&publication.read(p)&&p.active&&rcx==p.transform)kill("path_a_known_caller_mismatch","path_a");
  release();return;
 }
 auto rejectEntry=[&](const char* reason,unsigned mask){auto count=pathARejects.fetch_add(1);if(count<64){Record r{};r.kind=10;r.reason=reason;r.session=p.session;r.elapsed=p.elapsed;r.gateMask=mask;emit(r);}};
 if(!lifecycleOpen.load()||!publication.read(p)||!p.active){++pathARejects;release();return;}
 if(rcx!=p.transform){release();return;}
 if(p.thread!=GetCurrentThreadId()){kill("path_a_wrong_thread","path_a");release();return;}
 if(!entryToken(p,"path_a")){++pathARejects;release();return;}
 if(p.lifecycleEpoch!=lifecycleEpoch.load()||now<p.publishedMs||now-p.publishedMs>100){++pathARejects;release();return;}
 if(pathAWrites.load()>=4096){rejectEntry("path_A_write_budget_exhausted",1|16|32);release();return;}
 if(rcx!=p.transform){rejectEntry("current_Hero_target_mismatch",1|16|32);release();return;}
 if(r8!=0){kill("path_a_unexpected_R8","path_a");rejectEntry("unexpected_R8_not_null",1|2|16|32);release();return;}
 msm2::Snapshot hero{};if(!msm2::sample(hero)||hero.hero!=p.hero||hero.transform!=p.transform){kill("path_a_hero_transform_mismatch","path_a");rejectEntry("fresh_Hero_identity_unavailable_or_changed",1|2|16|32);release();return;}
 if(!currentGuards()){rejectEntry("entry_hook_or_caller_integrity_lost",1|2|16|32);kill("path_a_current_guards_lost","path_a");release();return;}
 Matrix incoming{};auto stack=ws::interop::currentStack();if(!ws::interop::stackMatrix(rdx,originalRsp,stack,incoming)||!(rdx+64<=rcx||rcx+64<=rdx)){kill("path_a_stack_or_source_layout_failed","path_a");rejectEntry("path_A_stack_provenance_or_source_layout_failed",1|2|16|32);release();return;}
 Matrix applied=incoming;Vec3 rows[]={p.basis.right,p.basis.up,p.basis.forward};for(unsigned i=0;i<3;++i){applied.v[4*i]=rows[i].x;applied.v[4*i+1]=rows[i].y;applied.v[4*i+2]=rows[i].z;applied.v[4*i+3]=0;}
 if(!ws::source_math::valid(applied)){rejectEntry("published_basis_invalid",1|2|4|8|16|32);release();return;}
 if(!currentGuards()){kill("path_a_current_guards_lost","path_a");release();return;}
 Published final{};if(!publication.read(final)||final.session!=p.session||final.generation!=p.generation||final.lifecycleEpoch!=p.lifecycleEpoch||final.lifecycleEpoch!=lifecycleEpoch.load()||final.publishedMs!=p.publishedMs||!final.active||!currentGuards()||!entryToken(final,"path_a")){rejectEntry("lockfree_snapshot_or_integrity_changed_before_commit",1|2|4|8|16|32);release();return;}
 LARGE_INTEGER begin{};QueryPerformanceCounter(&begin);auto result=ws::interop::writeStackOrientation(rdx,incoming,applied);
 if(!result.success){rejectEntry(result.rollbackFailed?"path_A_write_failed_rollback_failed":"path_A_write_failed_restored_or_untouched",1|2|4|8|16|32);if(result.rollbackFailed)kill("path_a_rollback_failed","path_a");release();return;}
 didOverride=true;auto count=pathAWrites.fetch_add(1)+1;Vec3 appliedUp{applied.v[4],applied.v[5],applied.v[6]};
 msm2::reset_trace::Request committed{};committed.prearmed=true;committed.verifyDual=config.dualSource;committed.characterize=true;committed.initMs=p.initMs;committed.session=p.session;committed.generation=p.generation;committed.nativeUpdate=p.nativeUpdate;committed.commitQpc=result.commitQpc;committed.overrideBeginQpc=begin.QuadPart;committed.hero=p.hero;committed.transform=p.transform;committed.owner=p.owner;committed.thread=p.thread;committed.normal=p.surface;committed.appliedUp=appliedUp;bool prearmed=msm2::reset_trace::committed(committed);
 Record r{};r.kind=1;r.late=count>1&&incoming.v[5]>=.98f;r.rcx=rcx;r.r8=r8;r.r9=r9;r.rsp=originalRsp;r.stackLimit=stack.limit;r.stackBase=stack.base;r.prearmed=prearmed;r.nativeThread=p.thread;r.commitQpc=result.commitQpc;r.session=p.session;r.generation=p.generation;r.ms=now;r.nativeUpdate=p.nativeUpdate;r.source=rdx;r.caller=callReturn;r.elapsed=p.elapsed;r.t=p.t;r.original=incoming;r.applied=applied;r.target=p.basis;r.n=p.surface;r.positionSame=result.positionUnchanged;r.stack=true;r.writes=count;r.positionBefore={hero.position[0],hero.position[1],hero.position[2]};emit(r);release();
}
void capture(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t ret,std::uintptr_t rsp) noexcept {capturePath(rcx,rdx,r8,r9,ret,rsp);}
void captureCopy(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t ret,std::uintptr_t rsp) noexcept {
 didOverride=false;auto error=GetLastError();trace::beginResetCopyEntry();if(inside||killed.load()||!ws_native_source_enabled.load()){SetLastError(error);return;}
 inside=true;SecondaryCall secondaryCall{};auto release=[&]{secondaryCall.finish();inside=false;SetLastError(error);};Published p{};entryEvidence={rcx,rdx,rsp,ret};
 bool coherent=publication.read(p);bool active=coherent&&p.active&&lifecycleOpen.load();
 // Retained identity is attribution only. Inactive/stale permission never admits.
 Published identity=p;if(!identity.session)identityPublication.read(identity);
 auto thread=GetCurrentThreadId();bool otherThread=identity.active&&identity.thread&&identity.thread!=thread;
 auto now=GetTickCount64();if(otherThread)secondaryCall.begin(identity,thread,rcx,rdx,ret,rsp,now);
 auto rejectEntry=[&](const char* why){pathBRejects.fetch_add(1);if(otherThread)secondaryCall.event.reason=why;};
 if(ret!=imageBase+0x2cfc84b){
  if(otherThread){rejectEntry("secondary_wrong_caller");}
  else if(ret>=imageBase+0x2cfc7d0&&ret<imageBase+0x2cfc850&&coherent&&p.active&&rcx==p.transform)kill("path_b_known_caller_mismatch","path_b");
  release();return;
 }
 if(!active){rejectEntry("secondary_source_inactive");release();return;}
 if(rcx!=p.transform){if(otherThread)rejectEntry("secondary_wrong_target");release();return;}
 // Primary A/B ownership and the token still belong to p.thread. Only this B
 // source store may run on another independently verified current-thread stack.
 EntryTokenStatus tokenStatus{};
 auto tokenOK=[&](const Published& current){bool ok=entryToken(current,"path_b",otherThread?&tokenStatus:nullptr);if(otherThread){secondaryCall.event.tokenStatus=tokenStatus.status;if(!ok)secondaryCall.event.tokenIdentity=false;if(!ok)secondaryCall.event.reason=tokenStatus.failure;}return ok;};
 if(!tokenOK(p)){rejectEntry(otherThread?tokenStatus.failure:"primary_token_rejected");release();return;}
 if(otherThread&&!secondaryContext(p,secondaryCall.event.reason)){secondaryCall.event.tokenIdentity=false;rejectEntry(secondaryCall.event.reason);release();return;}
 if(p.lifecycleEpoch!=lifecycleEpoch.load()||now<p.publishedMs||now-p.publishedMs>100){rejectEntry("secondary_source_publication_changed");release();return;}
 auto integrityLost=[&]{const char* why=otherThread?"secondary_source_integrity_lost":"path_b_compact_or_guard_lost";kill(why,"path_b");rejectEntry(why);};
 if(!msm2::reset_copy::compactProofAuthenticated()||!currentGuards()){integrityLost();release();return;}
 if(pathBWrites.load()>=4096){rejectEntry("secondary_write_budget_exhausted");release();return;}
 msm2::Snapshot hero{};if(!msm2::sample(hero)||hero.hero!=p.hero||hero.transform!=p.transform){const char* why=otherThread?"secondary_identity_epoch_mismatch":"path_b_hero_transform_mismatch";kill(why,"path_b");rejectEntry(why);release();return;}
 if(otherThread)secondaryCall.event.tokenIdentity=true;
 auto stack=ws::interop::currentStack();ws::source_math::CompactBlock incoming{},applied{};Vec3 lengths{};
 if(otherThread&&(rsp>UINTPTR_MAX-0x28||rdx!=rsp+0x28)){rejectEntry("secondary_stack_relation_failed");release();return;}
 if(!ws::interop::stackCompact(rdx,rsp,stack,incoming)){if(otherThread)rejectEntry("secondary_source_span_failed");else {kill("path_b_stack_or_source_layout_failed","path_b");rejectEntry("primary_stack_or_source_layout_failed");}release();return;}
 if(otherThread){secondaryCall.event.stack=true;secondaryCall.event.compactOriginal=incoming;}
 if(!ws::source_math::compactReplacement(incoming,p.basis,applied,lengths)){if(otherThread)rejectEntry("secondary_layout_invalid");else {kill("path_b_stack_or_source_layout_failed","path_b");rejectEntry("primary_layout_invalid");}release();return;}
 if(otherThread)secondaryCall.event.sourceLayout=true;
 path_b::Entry e{};e.session=p.session;e.generation=p.generation;e.update=p.nativeUpdate;e.owner=p.owner;e.thread=thread;e.elapsed=p.elapsed;e.rcx=rcx;e.rdx=rdx;e.r8=r8;e.r9=r9;e.rsp=rsp;e.caller=0x2cfc84b;e.stackLimit=stack.limit;e.stackBase=stack.base;e.normal=p.surface;e.actorBefore={hero.prefix[4],hero.prefix[5],hero.prefix[6]};e.referenceValid=pathAWrites.load()>0;e.pathAUp=p.basis.up;e.readable=path_b::readBounded(rdx,e.raw,128);
 if(!currentGuards()){integrityLost();release();return;}
 Published final{};
 if(!publication.read(final)||final.session!=p.session||final.generation!=p.generation||final.lifecycleEpoch!=p.lifecycleEpoch||final.lifecycleEpoch!=lifecycleEpoch.load()||final.publishedMs!=p.publishedMs||!final.active){secondaryCall.event.tokenIdentity=false;path_b::capture(e);rejectEntry("secondary_source_publication_changed");release();return;}
 if(!currentGuards()){integrityLost();release();return;}
 if(!tokenOK(final)){path_b::capture(e);rejectEntry(otherThread?tokenStatus.failure:"primary_token_rejected");release();return;}
 if(otherThread&&!secondaryContext(final,secondaryCall.event.reason)){secondaryCall.event.tokenIdentity=false;rejectEntry(secondaryCall.event.reason);release();return;}
 if(!reservePathB()){rejectEntry("secondary_write_budget_exhausted");release();return;}
 secondaryCall.admitted=otherThread;
 LARGE_INTEGER begin{};QueryPerformanceCounter(&begin);auto result=ws::interop::writeStackCompact(rdx,incoming,applied);e.overridden=result.success;path_b::capture(e);
 if(!result.success){rejectEntry("secondary_write_failed");if(result.rollbackFailed)kill("path_b_rollback_failed","path_b");release();return;}
 didOverride=true;auto count=pathBWrites.fetch_add(1)+1;
 // The old bounded primary watch must not adopt a secondary call as its owner.
 if(!otherThread){msm2::reset_trace::Request committed{};committed.prearmed=true;committed.resetCopy=true;committed.verifyDual=true;committed.characterize=true;committed.initMs=p.initMs;committed.session=p.session;committed.generation=p.generation;committed.nativeUpdate=p.nativeUpdate;committed.commitQpc=result.commitQpc;committed.overrideBeginQpc=begin.QuadPart;committed.hero=p.hero;committed.transform=p.transform;committed.owner=p.owner;committed.thread=p.thread;committed.normal=p.surface;committed.appliedUp=p.basis.up;msm2::reset_trace::committed(committed);}
 if(otherThread){secondaryCall.event.success=true;secondaryCall.event.reason="secondary_verified";}
 Record r{};r.kind=11;r.copy=true;r.session=p.session;r.generation=p.generation;r.nativeUpdate=p.nativeUpdate;r.nativeThread=thread;r.expectedThread=p.thread;r.ms=now;r.elapsed=p.elapsed;r.source=rdx;r.rcx=rcx;r.rsp=rsp;r.stackLimit=stack.limit;r.stackBase=stack.base;r.caller=0x2cfc84b;r.n=p.surface;r.compactOriginal=incoming;r.compactApplied=applied;r.rowLengths=lengths;r.writes=count;r.commitQpc=result.commitQpc;r.positionSame=result.positionUnchanged;r.stack=true;emit(r);release();
}
void drainLogs(){
 drainCause();
 for(auto* slot:{&firstSecondaryOverride,&firstSecondaryReject}){unsigned ready=2;if(slot->ready.compare_exchange_strong(ready,3,std::memory_order_acq_rel))secondaryLog(slot->record);}
 path_b::drainLogs();
 auto lost=recordDrops.exchange(0);if(lost)writeLog("NATIVE_SOURCE_LOG_DROPS count=%u writes_unaffected=1",lost);
 auto a=pathAWrites.load(),b=pathBWrites.load(),ar=pathARejects.load(),br=pathBRejects.load();
 if(reportedA==~0u||a-reportedA>=64||b-reportedB>=64||ar-reportedAR>=64||br-reportedBR>=64){writeLog("PATH_A_OVERRIDE_COUNT=%u PATH_A_REJECT_COUNT=%u PATH_A_STATE_LOCK_CONTENTION=0 PATH_B_OVERRIDE_COUNT=%u PATH_B_REJECT_COUNT=%u",a,ar,b,br);reportedA=a;reportedB=b;reportedAR=ar;reportedBR=br;}
 for(unsigned n=0;n<capacity;++n){auto t=tail.load(std::memory_order_relaxed);if(t==head.load(std::memory_order_acquire))break;auto r=records[t%capacity];tail.store(t+1,std::memory_order_release);
  if(r.kind==12)secondaryLog(r);
  if(r.kind==10)writeLog("COMMON_OVERRIDE_REJECT session=%llu elapsed=%.6f reason=%s caller_ok=%u target_ok=%u stack_ok=%u source_layout_ok=%u lifecycle_ok=%u elapsed_ok=%u",(unsigned long long)r.session,r.elapsed,r.reason,unsigned(bool(r.gateMask&1)),unsigned(bool(r.gateMask&2)),unsigned(bool(r.gateMask&4)),unsigned(bool(r.gateMask&8)),unsigned(bool(r.gateMask&16)),unsigned(bool(r.gateMask&32)));
  if(r.kind==7)writeLog("ORIENTATION_RETENTION_PASS=1 session=%llu held_ms=%llu held_updates=%u game_verification=requires_accepted_build_runtime_log",(unsigned long long)r.session,(unsigned long long)r.heldMs,r.heldUpdates);
  if(r.kind==1){
    if(r.writes<=2)writeLog("PATH_A_POSITION_RUNTIME session=%llu elapsed=%.6f native_update=%llu native_thread=%u caller_rva=0x2cfc8f3 target_transform=0x%llx runtime_target_verified=1 current_thread_stack_verified=%u rdx_minus_entry_rsp=0x%llx source_position=(%.9g,%.9g,%.9g) source_w=%.9g HeroPosition_before=(%.9g,%.9g,%.9g) position_row_preserved=%u position_source_write=0 causality_proven=0 destination_W_semantics=unproven",
     (unsigned long long)r.session,r.elapsed,(unsigned long long)r.nativeUpdate,r.nativeThread,(unsigned long long)r.rcx,unsigned(r.stack),(unsigned long long)(r.source-r.rsp),r.original.v[12],r.original.v[13],r.original.v[14],r.original.v[15],r.positionBefore.x,r.positionBefore.y,r.positionBefore.z,unsigned(r.positionSame));
    writeLog("COMMON_SOURCE_OVERRIDE session=%llu elapsed=%.6f caller_rva=0x%llx target_transform=0x%llx source_ptr=0x%llx source_up_before=(%.9g,%.9g,%.9g) source_up_after=(%.9g,%.9g,%.9g) late_worldup_source_detected=%u position_unchanged=1",(unsigned long long)r.session,r.elapsed,(unsigned long long)r.caller,(unsigned long long)r.rcx,(unsigned long long)r.source,r.original.v[4],r.original.v[5],r.original.v[6],r.applied.v[4],r.applied.v[5],r.applied.v[6],unsigned(r.late));
    if(r.late)writeLog("LATE_COMMON_WORLDUP_OVERRIDE=1 session=%llu native_update=%llu reason=repeated_native_update_worldup_source",(unsigned long long)r.session,(unsigned long long)r.nativeUpdate);
  }
  if(r.kind==1&&r.writes==1)writeLog("PATH_A_LOCKFREE_SNAPSHOT_READ session=%llu generation=%llu coherent=1",(unsigned long long)r.session,(unsigned long long)r.generation);
  if(r.kind==11)writeLog("PATH_B_COMPACT3X3_OVERRIDE session=%llu generation=%llu elapsed=%.6f source_ptr=0x%llx rdx_minus_rsp=0x%llx original_row_lengths=(%.9g,%.9g,%.9g) original_up=(%.9g,%.9g,%.9g) applied_up=(%.9g,%.9g,%.9g) applied_up_dot_surface=%.9g bytes_written=0x24 metadata_untouched=1 caller_verified=1 target_verified=1 stack_verified=1 write_index=%u override_commit_qpc=%llu thread_id=%u expected_thread=%u",(unsigned long long)r.session,(unsigned long long)r.generation,r.elapsed,(unsigned long long)r.source,(unsigned long long)(r.source-r.rsp),r.rowLengths.x,r.rowLengths.y,r.rowLengths.z,r.compactOriginal.rows[3],r.compactOriginal.rows[4],r.compactOriginal.rows[5],r.compactApplied.rows[3],r.compactApplied.rows[4],r.compactApplied.rows[5],ws::dot(Vec3{r.compactApplied.rows[3],r.compactApplied.rows[4],r.compactApplied.rows[5]}*(1/r.rowLengths.y),r.n),r.writes,(unsigned long long)r.commitQpc,r.nativeThread,r.expectedThread);
  if(r.kind==1&&r.writes==1)writeLog("PREARM_STATUS session=%llu elapsed=%.6f primary_thread=%u DR0_installed=%u DR1_installed=%u trace_already_armed_before_override=%u first_override=1",(unsigned long long)r.session,r.elapsed,r.nativeThread,unsigned(r.prearmed),unsigned(r.prearmed),unsigned(r.prearmed));
  if(r.kind==1&&!r.copy)writeLog("NATIVE_SOURCE_OVERRIDE session=%llu generation=%llu native_ms=%llu native_update=%llu elapsed=%.6f caller_rva=0x%llx source_ptr=0x%llx original_right=(%.9g,%.9g,%.9g) original_up=(%.9g,%.9g,%.9g) original_forward=(%.9g,%.9g,%.9g) target_right=(%.9g,%.9g,%.9g) target_up=(%.9g,%.9g,%.9g) target_forward=(%.9g,%.9g,%.9g) applied_right=(%.9g,%.9g,%.9g) applied_up=(%.9g,%.9g,%.9g) applied_forward=(%.9g,%.9g,%.9g) t=%.6f position_unchanged=%u source_is_stack_local=%u direct_actor_transform_write=0 write_index=%u override_commit_qpc=%llu",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.generation),static_cast<unsigned long long>(r.ms),static_cast<unsigned long long>(r.nativeUpdate),r.elapsed,static_cast<unsigned long long>(r.caller),static_cast<unsigned long long>(r.source),r.original.v[0],r.original.v[1],r.original.v[2],r.original.v[4],r.original.v[5],r.original.v[6],r.original.v[8],r.original.v[9],r.original.v[10],r.target.right.x,r.target.right.y,r.target.right.z,r.target.up.x,r.target.up.y,r.target.up.z,r.target.forward.x,r.target.forward.y,r.target.forward.z,r.applied.v[0],r.applied.v[1],r.applied.v[2],r.applied.v[4],r.applied.v[5],r.applied.v[6],r.applied.v[8],r.applied.v[9],r.applied.v[10],r.t,unsigned(r.positionSame),unsigned(r.stack),r.writes,static_cast<unsigned long long>(r.commitQpc));
  else if(r.kind==2)writeLog("NATIVE_SOURCE_EXIT session=%llu reason=%s duration_since_source_entry=%.6f applied_writes=%u held_updates=%u held_ms=%llu stability_observed=%u direct_actor_transform_write=0",static_cast<unsigned long long>(r.session),r.reason,r.elapsed,r.writes,r.heldUpdates,static_cast<unsigned long long>(r.heldMs),unsigned(r.success));
  else if(r.kind==3)writeLog("NATIVE_SOURCE_ACTOR_OBSERVATION session=%llu native_update=%llu native_ms=%llu actor_up=(%.9g,%.9g,%.9g) actor_up_dot_surface=%.9g actor_up_dot_world=%.9g held_distinct_updates=%u held_ms=%llu stability_observed=%u source=read_only_snapshot_NOT_POST_writer_callback lifecycle_active=1 observation_phase=%s",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.nativeUpdate),static_cast<unsigned long long>(r.ms),r.actorUp.x,r.actorUp.y,r.actorUp.z,r.actorDotSurface,r.actorDotWorld,r.heldUpdates,static_cast<unsigned long long>(r.heldMs),unsigned(r.success),r.reason);
  else if(r.kind==5)writeLog("NATIVE_SOURCE_ENTRY_REJECT session=%llu generation=%llu reason=%s source_override=0",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.generation),r.reason);
  else if(r.kind==4)writeLog("NATIVE_SOURCE_OTHER_WRITER session=%llu caller_return_rva=0x%llx source_up=(%.9g,%.9g,%.9g) override=0",static_cast<unsigned long long>(r.session),static_cast<unsigned long long>(r.caller),r.original.v[4],r.original.v[5],r.original.v[6]);
 }
}
void stop(){kill("script_stop","shutdown");if(!stateLock.test_and_set(std::memory_order_acquire)){drainVerifiedDeinit();end("script_stop",GetTickCount64());lifecycleSummary(GetTickCount64(),false,false);stateLock.clear(std::memory_order_release);}drainLogs();secondarySummary(state.session,true);writeLog("PATH_A_OVERRIDE_COUNT=%u PATH_A_REJECT_COUNT=%u PATH_A_STATE_LOCK_CONTENTION=0 PATH_B_OVERRIDE_COUNT=%u PATH_B_REJECT_COUNT=%u final=1",pathAWrites.load(),pathARejects.load(),pathBWrites.load(),pathBRejects.load());writeLog("native_source_stop source_override=OFF direct_actor_transform_write=0 vanilla_passthrough=1");}
}
