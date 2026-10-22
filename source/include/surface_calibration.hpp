#pragma once
#include "object_layout.hpp"
#include "four_face.hpp"
#include <cstdio>
namespace ws::calibration {
using Logger=void (*)(const char*,...);
inline constexpr double windowMs=2500.;
enum class SurfaceSource {None,WallRunObjectField,WallRunObjectPoint,CrawlCalibration};
struct SurfaceCalibration {
 bool valid{},axisAvailable{},pointAvailable{};Vec3 axis{},point{};float distance{},confidence{};
 SurfaceSource source=SurfaceSource::None;
 // All diagnostic data; actual geometry, sign and field boundary remain unverified.
};
struct Evidence {Vec3 axis{},candidate{};bool stable{},basisSuspect{};std::uint64_t session{};double correlation{};};
struct History {
 std::array<Evidence,12> values{};unsigned count{},next{};
 void add(Evidence e){values[next]=e;next=(next+1)%values.size();count=std::min(count+1,unsigned(values.size()));}
 unsigned nonparallel(bool qualifyingOnly,bool allowBasis=false)const {
  // At most 12 entries: exhaustive triples avoid order-dependent greedy results.
  auto eligible=[&](unsigned i){auto e=values[i];return !qualifyingOnly||(e.stable&&(allowBasis||!e.basisSuspect)&&e.correlation>=.98);};
  auto different=[&](unsigned a,unsigned b){return std::abs(diag::preciseDot(values[a].axis,values[b].axis))<.98;};
  unsigned result=0;
  for(unsigned a=0;a<count;++a)if(eligible(a)){result=std::max(result,1u);
   for(unsigned b=a+1;b<count;++b)if(eligible(b)&&different(a,b)){result=std::max(result,2u);
    for(unsigned c=b+1;c<count;++c)if(eligible(c)&&different(a,c)&&different(b,c))return 3;
   }
  }
  return result;
 }
 double variability()const {double result=0;for(unsigned a=0;a<count;++a)for(unsigned b=a+1;b<count;++b)result=std::max(result,std::sqrt(diag::preciseDot(values[a].candidate-values[b].candidate,values[a].candidate-values[b].candidate)));return result;}
};
class Tracker {
 struct Pending {
  bool armed{},paired{};std::uint64_t session{},epoch{};std::uintptr_t hero{},transform{};double deinitMs{};
  discovery::Candidates data;discovery::Layout layout;
 } pending;
 std::uint64_t activeSession{},activeEpoch{},lastSample{};std::uintptr_t activeHero{},activeTransform{};
 unsigned consistent{};Vec3 previousAxis{};std::array<History,discovery::offsets> histories{};
 std::array<faces::History,discovery::offsets> fourFaceHistories{};
 SurfaceCalibration contextValue{};
 std::uint64_t actorHero{},actorTransform{},actorEpoch{},actorSample{},lastCrawlLog{};double idleStart{};bool idleAnnounced{};
 static bool stable(const discovery::Candidate& c,unsigned samples){return c.samples>=2&&c.samples*5>=samples*4&&c.minStability>.98;}
 static void dumpRaw(const discovery::Candidates& data,std::uint64_t session,Logger log){
  if(!data.count())return;
  for(unsigned phase=0;phase<2;++phase){const auto& block=phase?data.lastBlock():data.firstBlock();
   log("wallrun_object_raw_snapshot session=%llu phase=%s captured_monotonic_ms=%llu range=[0x120,0x1e0) source=cached_active_worker_sample NOT_exact_Init_or_Deinit=1",static_cast<unsigned long long>(session),phase?"last_active_sample":"first_active_sample",static_cast<unsigned long long>(phase?data.lastTime():data.firstTime()));
   for(unsigned offset=0x120;offset<0x1e0;offset+=16){float f[4];std::memcpy(f,block.data()+offset,16);
    log("wallrun_object_raw_floats session=%llu phase=%s offset=0x%x values=(%.9g,%.9g,%.9g,%.9g)",static_cast<unsigned long long>(session),phase?"last":"first",offset,f[0],f[1],f[2],f[3]);}
  }
 }
public:
 const SurfaceCalibration& context()const{return contextValue;}
 void reset(const char* reason,Logger log){
  if(pending.armed&&!pending.paired)log("surface_calibration_cancel session=%llu reason=%s",static_cast<unsigned long long>(pending.session),reason);
  pending={};activeSession=activeEpoch=lastSample=0;activeHero=activeTransform=0;consistent=0;contextValue={};histories={};fourFaceHistories={};actorHero=actorTransform=actorEpoch=actorSample=lastCrawlLog=0;idleStart=0;idleAnnounced=false;
 }
 void begin(std::uint64_t session,std::uintptr_t hero,std::uintptr_t transform,std::uint64_t epoch,Logger log){
  if(pending.armed&&!pending.paired)log("surface_calibration_cancel session=%llu reason=new_WallRun_before_crawl",static_cast<unsigned long long>(pending.session));
  pending={};consistent=0;lastSample=0;contextValue={};activeSession=session;activeHero=hero;activeTransform=transform;activeEpoch=epoch;
 }
 void summarize(const discovery::Candidates& data,std::uint64_t session,const char* reason,Logger log)const {
  if(!data.count())return;
  auto layout=discovery::classify(data);const auto& fields=data.fields();
  log("wallrun_object_summary session=%llu samples=%u max_bytes=0x200 reason=%s surface_valid=0 overlapping_fields_NOT_independent=1",static_cast<unsigned long long>(session),data.count(),reason);
  dumpRaw(data,session,log);
  for(unsigned i=0;i<discovery::offsets;++i){if(fields[i].samples<2)continue;
   bool first=true;char members[1024]{};unsigned n=0;
   for(unsigned j=0;j<discovery::offsets;++j)if(fields[j].samples>=2&&layout.cluster[j]==layout.cluster[i]){
    if(j<i){first=false;}
    int wrote=std::snprintf(members+n,sizeof(members)-n,"%s0x%x",n?",":"",j*4);if(wrote>0)n=std::min(unsigned(sizeof(members)-1),n+unsigned(wrote));
   }
   if(first)log("object_layout_cluster session=%llu cluster=0x%x members=%s classification=%s interpretation=numeric_hypothesis_NOT_native_field_boundaries",static_cast<unsigned long long>(session),layout.cluster[i],members,discovery::layoutName(layout.kind[i]));
  }
  // Preserve grouped member identities so a field equal to a basis on one wall can
  // still be re-evaluated when that SAME offset rotates on other walls.
  for(unsigned i=0;i<discovery::offsets;++i)if(layout.basisSuspect[i]&&layout.repeatRepresentative[i]==i){
   char members[1024]{};unsigned length=0;for(unsigned j=0;j<discovery::offsets;++j)if(layout.basisSuspect[j]&&layout.repeatRepresentative[j]==i){int n=std::snprintf(members+length,sizeof(members)-length,"%s0x%x",length?",":"",j*4);if(n>0)length=std::min(unsigned(sizeof(members)-1),length+unsigned(n));}
   const auto& c=fields[i];log("object_repeated_basis_group session=%llu representative=0x%x members=%s candidate=(%.6f,%.6f,%.6f) stable=1 relationship=numerically_equal_NOT_independent_native_fields",static_cast<unsigned long long>(session),i*4,members,c.last.x,c.last.y,c.last.z);
  }
  unsigned emitted=0,suppressed=0;
  for(unsigned i=0;i<discovery::offsets;++i){const auto& c=fields[i];if(c.samples<2)continue;
   if(layout.basisSuspect[i]&&layout.repeatRepresentative[i]!=i){++suppressed;continue;}
   log("object_vec3_candidate session=%llu offset=0x%x cluster=0x%x classification=%s overlapping=%u matrix_row_boundary_hypothesis=%u samples=%u observations=%u first=(%.6f,%.6f,%.6f) last=(%.6f,%.6f,%.6f) min_dot_first=%.6f stable_for_observations=%u identity_or_basis_suspect=%u identity_or_basis_status=%s rank_penalty=%u NOT_verified_surface_normal=1",static_cast<unsigned long long>(session),i*4,layout.cluster[i],discovery::layoutName(layout.kind[i]),unsigned(layout.overlap[i]),unsigned(layout.matrixBoundary[i]),c.samples,data.count(),c.first.x,c.first.y,c.first.z,c.last.x,c.last.y,c.last.z,c.minStability,unsigned(stable(c,data.count())),unsigned(layout.basisSuspect[i]),layout.basisSuspect[i]?"IDENTITY_OR_BASIS_SUSPECT":"UNRESOLVED",layout.basisSuspect[i]?100u:0u);++emitted;
  }
  log("object_candidate_summary session=%llu representative_or_cluster_members=%u repeated_basis_members_suppressed=%u candidates_are_NOT_independent=1",static_cast<unsigned long long>(session),emitted,suppressed);
  const auto& points=data.points();unsigned pointCount=0;
  for(unsigned i=0;i<discovery::offsets;++i){const auto& p=points[i];if(p.samples<2)continue;++pointCount;
   log("object_world_position_candidate session=%llu offset=0x%x samples=%u first_point=(%.6f,%.6f,%.6f) last_point=(%.6f,%.6f,%.6f) first_hero=(%.6f,%.6f,%.6f) last_hero=(%.6f,%.6f,%.6f) distance_range=(%.6f,%.6f) point_motion=%.6f hero_motion=%.6f NOT_verified_contact=1",static_cast<unsigned long long>(session),i*4,p.samples,p.first.x,p.first.y,p.first.z,p.last.x,p.last.y,p.last.z,p.firstHero.x,p.firstHero.y,p.firstHero.z,p.lastHero.x,p.lastHero.y,p.lastHero.z,p.minDistance,p.maxDistance,std::sqrt(diag::preciseDot(p.last-p.first,p.last-p.first)),std::sqrt(diag::preciseDot(p.lastHero-p.firstHero,p.lastHero-p.firstHero)));
  }
  log("object_world_position_summary session=%llu candidate_count=%u max_distance_m=5 cluster_and_overlap_review_required=1",static_cast<unsigned long long>(session),pointCount);
 }
 void finish(const discovery::Candidates& data,std::uint64_t session,double deinitMs,Logger log){
  if(session!=activeSession)return;
  pending={};pending.armed=true;pending.session=session;pending.epoch=activeEpoch;pending.hero=activeHero;pending.transform=activeTransform;pending.deinitMs=deinitMs;pending.data=data;pending.layout=discovery::classify(data);
  consistent=0;lastSample=0;activeSession=0;
  log("surface_calibration_window session=%llu window_ms=2500 source=verified_deinit read_object_after_Deinit=0 candidates_cached=%u",static_cast<unsigned long long>(session),data.count());
 }
 void tick(double now,std::uint64_t snapshotMs,std::uintptr_t hero,std::uintptr_t transform,std::uint64_t epoch,const diag::ActorAxes& axes,bool wallRunActive,Logger log,const Vec3* position=nullptr){
  if(!pending.armed||pending.paired)return;
  if(wallRunActive||hero!=pending.hero||transform!=pending.transform||epoch!=pending.epoch){
   log("surface_calibration_cancel session=%llu reason=identity_changed_or_new_WallRun",static_cast<unsigned long long>(pending.session));pending.armed=false;consistent=0;return;
  }
  if(now-pending.deinitMs>windowMs){log("surface_calibration_expired session=%llu paired=0",static_cast<unsigned long long>(pending.session));pending.armed=false;return;}
  if(snapshotMs==lastSample||double(snapshotMs)<pending.deinitMs||now-double(snapshotMs)>250||now<double(snapshotMs))return;
  lastSample=snapshotMs;Vec3 axis{};
  if(!axes.valid||!unit(axes.up,axis)||std::abs(diag::preciseDot(axis,diag::worldUp))>=.35){consistent=0;return;}
  if(consistent&&diag::preciseDot(previousAxis,axis)<.995)consistent=0;
  previousAxis=axis;++consistent;
  log("crawl_like_orientation=1 wallrun_session=%llu crawl_axis=(%.6f,%.6f,%.6f) consistent_samples=%u source=post_deinit_actor_R1 NOT_VERIFIED_WALLCRAWL=1 sign_is_NOT_outward_normal_verified=1",static_cast<unsigned long long>(pending.session),axis.x,axis.y,axis.z,consistent);
  if(consistent<3)return;
  pending.paired=true;contextValue={};contextValue.axisAvailable=true;contextValue.axis=axis;contextValue.confidence=.5f;contextValue.source=SurfaceSource::CrawlCalibration;
  log("surface_calibration_pair wallrun_session=%llu deinit_to_crawl_ms=%.3f crawl_axis=(%.6f,%.6f,%.6f) same_Hero_Transform=1 paired_crawl_detected=1 source=crawl_like_orientation temporal_pair_NOT_geometry_proof=1",static_cast<unsigned long long>(pending.session),double(snapshotMs)-pending.deinitMs,axis.x,axis.y,axis.z);
  if(position)log("surface_calibration_crawl_position wallrun_session=%llu crawl_detected_position=(%.6f,%.6f,%.6f) sample_monotonic_ms=%llu",static_cast<unsigned long long>(pending.session),position->x,position->y,position->z,static_cast<unsigned long long>(snapshotMs));
  const auto& fields=pending.data.fields();const auto& layout=pending.layout;
  for(unsigned i=0;i<discovery::offsets;++i){const auto& c=fields[i];if(c.samples<2)continue;
   double signedDot=diag::preciseDot(c.last,axis),alignment=std::min(std::abs(signedDot),std::abs(diag::preciseDot(c.first,axis)));
   bool isStable=stable(c,pending.data.count());auto& h=histories[i];h.add({axis,c.last,isStable,layout.basisSuspect[i],pending.session,alignment});
   auto& four=fourFaceHistories[i];four.add(axis,c.last,isStable);auto fourResult=four.result();
   if(layout.basisSuspect[i]&&layout.repeatRepresentative[i]!=i&&!fourResult.strongAxis)continue;
   const auto& fd=four.data();
   log("surface_source_four_face_score session=%llu child_offset=0x%x face_candidate_group=%c face_A_abs_dot=%.6f face_B_abs_dot=%.6f face_C_abs_dot=%.6f face_D_abs_dot=%.6f mean_abs_dot=%.6f min_abs_dot=%.6f complete_four_faces=%u signed_pattern=%s opposite_A_C_dot=%.6f opposite_B_D_dot=%.6f between_face_rotation=%u strong_axis_candidate=%u strong_signed_candidate=%u face_labels=R1_orientation_heuristic_NOT_video_confirmed surface_verified=0",static_cast<unsigned long long>(pending.session),i*4,faces::group(axis),fd[0].samples?fd[0].minAbs:-1.,fd[1].samples?fd[1].minAbs:-1.,fd[2].samples?fd[2].minAbs:-1.,fd[3].samples?fd[3].minAbs:-1.,fourResult.complete?fourResult.mean:-1.,fourResult.complete?fourResult.minimum:-1.,unsigned(fourResult.complete),fourResult.signedPattern,fourResult.ac,fourResult.bd,unsigned(fourResult.betweenRotation),unsigned(fourResult.strongAxis),unsigned(fourResult.strongSigned));
   unsigned walls=h.nonparallel(false);double spread=h.variability();bool reconsider=spread>.25&&(h.nonparallel(true,true)>=3||fourResult.strongAxis);unsigned qualified=h.nonparallel(true,reconsider);
   double score=isStable?alignment*std::min(1.,spread/.25)*double(qualified)/3.:0;if(layout.basisSuspect[i]&&!reconsider)score=0;if(layout.overlap[i]&&!layout.matrixBoundary[i])score*=.25;
   log("surface_candidate_score session=%llu offset=0x%x cluster=0x%x layout=%s candidate=(%.6f,%.6f,%.6f) crawl_axis=(%.6f,%.6f,%.6f) abs_dot=%.6f signed_dot=%.6f within_session_stable=%u between_wall_variability=%.6f nonparallel_wall_axes_capped3=%u qualified_nonparallel_walls_capped3=%u identity_or_basis_suspect=%u basis_reconsidered_across_walls=%u is_constant_across_observed_sessions=%u constant_world_axis_status=%s overlapping=%u discovery_score=%.6f field_boundary_verified=0 surface_sign_verified=0 surface_verified=0",static_cast<unsigned long long>(pending.session),i*4,layout.cluster[i],discovery::layoutName(layout.kind[i]),c.last.x,c.last.y,c.last.z,axis.x,axis.y,axis.z,alignment,signedDot,unsigned(isStable),spread,walls,qualified,unsigned(layout.basisSuspect[i]),unsigned(reconsider),unsigned(h.count>=2&&spread<.0001),(walls>=2&&spread<.0001)?"CONSTANT_WORLD_AXIS_SUSPECT":"UNRESOLVED",unsigned(layout.overlap[i]),score);
  }
  const auto& points=pending.data.points();double best=1e9;
  for(unsigned i=0;i<discovery::offsets;++i){const auto& p=points[i];if(p.samples<2)continue;
   double lateral=p.lateralRms(axis),axial=p.axialRms(axis),signedMean=diag::preciseDot(p.sumDelta,axis)/p.samples;
   double trackingError=std::sqrt(diag::preciseDot((p.lastHero-p.firstHero)-(p.last-p.first),(p.lastHero-p.firstHero)-(p.last-p.first)));
   log("surface_contact_candidate_score session=%llu offset=0x%x last_point=(%.6f,%.6f,%.6f) lateral_error_rms=%.6f abs_axial_distance_rms=%.6f signed_distance_mean=%.6f endpoint_tracking_error=%.6f samples=%u source=paired_WallRun_point_Hero_samples_and_later_crawl_axis NOT_verified_contact=1",static_cast<unsigned long long>(pending.session),i*4,p.last.x,p.last.y,p.last.z,lateral,axial,signedMean,trackingError,p.samples);
   if(lateral<.25&&axial>.05&&axial<2&&trackingError<.25&&lateral<best){best=lateral;contextValue.pointAvailable=true;contextValue.point=p.last;contextValue.distance=float(axial);}
  }
  log("surface_calibration_context valid=0 axis_available=1 point_candidate_available=%u confidence=%.2f source=CrawlCalibration confidence_is_heuristic=1 lifecycle_verified=1 surface_axis_verified=0 surface_sign_verified=0 contact_verified=0 v01b_gate=BLOCKED",unsigned(contextValue.pointAvailable),contextValue.confidence);
 }
 void observeActor(double now,std::uint64_t ms,std::uintptr_t hero,std::uintptr_t transform,std::uint64_t epoch,const diag::ActorAxes& axes,Vec3 position,Vec3 velocity,bool motionValid,bool wallRunActive,Logger log){
  if(actorHero!=hero||actorTransform!=transform||actorEpoch!=epoch){actorHero=hero;actorTransform=transform;actorEpoch=epoch;actorSample=lastCrawlLog=0;idleStart=0;idleAnnounced=false;}
  if(ms==actorSample||now<double(ms)||now-double(ms)>250){return;}
  actorSample=ms;
  if(axes.valid&&!wallRunActive&&std::abs(diag::preciseDot(axes.up,diag::worldUp))<.35&&ms-lastCrawlLog>=500){Vec3 axis{};if(unit(axes.up,axis)){
   log("crawl_like_orientation_sample sample_monotonic_ms=%llu crawl_axis=(%.6f,%.6f,%.6f) position=(%.6f,%.6f,%.6f) pairing=not_asserted source=Actor_R1 NOT_VERIFIED_WALLCRAWL=1",static_cast<unsigned long long>(ms),axis.x,axis.y,axis.z,position.x,position.y,position.z);lastCrawlLog=ms;
  }}
  bool idle=axes.valid&&!wallRunActive&&motionValid&&diag::preciseDot(axes.up,diag::worldUp)>.98&&diag::preciseDot(velocity,velocity)<.09;
  if(idle){if(!idleStart)idleStart=double(ms);if(!idleAnnounced&&double(ms)-idleStart>=1500){log("idle_like_interval sample_monotonic_ms=%llu duration_ms=%.3f position=(%.6f,%.6f,%.6f) source=position_difference_and_world_up NOT_verified_Ground=1",static_cast<unsigned long long>(ms),double(ms)-idleStart,position.x,position.y,position.z);idleAnnounced=true;}}
  else {idleStart=0;idleAnnounced=false;}
 }

};
}
