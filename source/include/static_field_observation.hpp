#pragma once
#include "surface_calibration.hpp"
#include "static_observation_plan.hpp"
#include "surface_pair.hpp"
namespace ws::fields {
inline constexpr unsigned roots=sizeof(static_plan::rootOffsets)/sizeof(unsigned),count=roots+3;
inline constexpr unsigned childOffsets[3]={0x60,0xf8,0x2d8};
inline unsigned offset(unsigned i){return i<roots?static_plan::rootOffsets[i]:childOffsets[i-roots];}
inline const char* scope(unsigned i){return i<roots?"this":"child128";}
struct Field {
 unsigned attempts{},readSamples{},unitSamples{};Vec3 first{},last{},unitFirst{},unitLast{};std::uint64_t firstMs{},lastMs{};
 double minStability=1;discovery::PositionCandidate point;
 void add(Vec3 v,Vec3 hero,std::uint64_t ms){
  ++attempts;if(!finite(v))return;if(!readSamples){first=v;firstMs=ms;}last=v;lastMs=ms;++readSamples;
  if(std::max({std::abs(v.x),std::abs(v.y),std::abs(v.z)})<1000000)point.add(v,hero);
  double norm=std::sqrt(diag::preciseDot(v,v));if(norm<.9||norm>1.1)return;
  Vec3 n=v*float(1./norm);if(!unitSamples)unitFirst=n;unitLast=n;++unitSamples;minStability=std::min(minStability,diag::preciseDot(unitFirst,n));
 }
 bool stable()const{return unitSamples>=2&&unitSamples==attempts&&minStability>.98;}
};
class Observer {
 pair::Observer pairValidation;std::array<Vec3,3> batch{};unsigned batchMask{};std::uint64_t batchMs{};
 std::array<Field,count> active{},pending{};std::array<faces::History,count> histories{};
 std::uint64_t id{},pendingId{},epoch{},lastMs{},startMs{};std::uintptr_t hero{},transform{},child{};double deinit{};
 bool armed{},childChanged{};unsigned consistent{},up{},side{},mixed{};Vec3 previous{};
public:
 void reset(){pairValidation.reset();batchMask=0;batchMs=0;active={};pending={};histories={};id=pendingId=epoch=lastMs=startMs=0;hero=transform=child=0;deinit=0;armed=childChanged=false;consistent=up=side=mixed=0;}
 void begin(std::uint64_t s,std::uintptr_t h,std::uintptr_t t,std::uint64_t e,std::uint64_t ms){
  if(hero!=h||transform!=t||epoch!=e)histories={};
  pairValidation.begin(s);batchMask=0;batchMs=0;
  id=s;hero=h;transform=t;epoch=e;startMs=ms;active={};pending={};armed=false;child=0;childChanged=false;consistent=up=side=mixed=0;
 }
 void motion(Vec3 velocity,bool valid){if(!valid)return;double v=std::abs(velocity.y),h=std::hypot(velocity.x,velocity.z);if(v>2*h&&v>1)++up;else if(h>2*v&&h>1)++side;else ++mixed;}
 void observe(unsigned i,const Vec3* value,Vec3 position,std::uint64_t ms,std::uintptr_t ptr,calibration::Logger log){
  if(i>=count||!id)return;
  if(i<roots){if(ms!=batchMs){batchMs=ms;batchMask=0;}
   unsigned off=offset(i),slot=off==pair::nearOffset?0:off==pair::planeOffset?1:off==pair::axisOffset?2:3;
   if(slot<3&&value){batch[slot]=*value;batchMask|=1u<<slot;}
  }
  if(i>=roots&&ptr){if(child&&child!=ptr)childChanged=true;child=ptr;}
  auto& f=active[i];if(!value){++f.attempts;return;}f.add(*value,position,ms);
  log("static_field_sample session=%llu scope=%s offset=0x%x phase=%s sample_ms=%llu value=(%.9g,%.9g,%.9g) hero_position=(%.9g,%.9g,%.9g) child_pointer=0x%llx evidence=%s source_path=%s+0x%x asynchronous_worker_sample=1 game_write=0",static_cast<unsigned long long>(id),scope(i),offset(i),f.attempts==1?"first_after_entry_worker_sample":"periodic_100ms",static_cast<unsigned long long>(ms),value->x,value->y,value->z,position.x,position.y,position.z,static_cast<unsigned long long>(ptr),i<roots?"authenticated_AVX_vector_plan":"native_child_access_neighborhood_NOT_vector_field_proof",i<roots?"WallRun.this":"WallRun.this+0x128->child",offset(i));
 }
 void endBatch(Vec3 position,Vec3 velocity,bool valid,std::uint64_t ms,calibration::Logger log){
  if(!id)return;
  pairValidation.sample(batchMs==ms&&batchMask==7?batch.data():nullptr,position,velocity,valid,ms,log);batchMask=0;
 }
 void finish(std::uint64_t s,double when,calibration::Logger log){
  if(s!=id)return;
  pairValidation.finish(log,when-double(startMs)>=100);
  for(unsigned i=0;i<count;++i){const auto& f=active[i];if(!f.readSamples)continue;
   log("static_field_summary session=%llu scope=%s offset=0x%x phase=last_active_sample first_ms=%llu last_ms=%llu attempts=%u finite_samples=%u unit_samples=%u first=(%.9g,%.9g,%.9g) last=(%.9g,%.9g,%.9g) min_dot_first=%.6f stable=%u point_samples=%u subtype_Up_Side_Mixed=(%u,%u,%u) child_pointer_changed=%u NOT_exact_Deinit=1",static_cast<unsigned long long>(s),scope(i),offset(i),static_cast<unsigned long long>(f.firstMs),static_cast<unsigned long long>(f.lastMs),f.attempts,f.readSamples,f.unitSamples,f.first.x,f.first.y,f.first.z,f.last.x,f.last.y,f.last.z,f.minStability,unsigned(f.stable()),f.point.samples,up,side,mixed,unsigned(childChanged));
  }
  if(when-double(startMs)<100){log("static_field_session_excluded session=%llu excluded_reason=too_short_transition_or_failed_attach duration_ms_est=%.3f",static_cast<unsigned long long>(s),when-double(startMs));armed=false;id=0;return;}
  pending=active;pendingId=s;deinit=when;armed=true;consistent=0;lastMs=0;id=0;
 }
 void tick(double now,std::uint64_t ms,std::uintptr_t h,std::uintptr_t t,std::uint64_t e,const diag::ActorAxes& axes,bool wallrun,calibration::Logger log){
  if(!armed)return;
  if(wallrun||h!=hero||t!=transform||e!=epoch||now-deinit>2500){armed=false;return;}
  if(ms==lastMs||double(ms)<deinit||now<double(ms)||now-double(ms)>250)return;
  lastMs=ms;Vec3 axis;if(!axes.valid||!unit(axes.up,axis)||std::abs(axis.y)>=.35){consistent=0;return;}
  if(consistent&&diag::preciseDot(previous,axis)<.995)consistent=0;
  previous=axis;if(++consistent<3)return;
  armed=false;pairValidation.crawl(axis,pendingId,log);log("static_field_calibration_pair wallrun_session=%llu crawl_axis=(%.6f,%.6f,%.6f) deinit_to_crawl_ms=%.3f source=post_Deinit_R1_crawl_like_NOT_engine_geometry sign_verified=0",static_cast<unsigned long long>(pendingId),axis.x,axis.y,axis.z,double(ms)-deinit);
  for(unsigned i=0;i<count;++i){const auto& f=pending[i];if(i>=roots)continue; // Native +60/+f8/+2d8 are pointer slots; raw neighborhoods only, never vectors.
   if(f.unitSamples>=2){double d=diag::preciseDot(f.unitLast,axis),alignment=std::min(std::abs(d),std::abs(diag::preciseDot(f.unitFirst,axis)));histories[i].add(axis,f.unitLast,f.stable());auto r=histories[i].result();
    log("static_field_axis_score session=%llu scope=%s offset=0x%x candidate=(%.6f,%.6f,%.6f) crawl_axis=(%.6f,%.6f,%.6f) first_last_min_abs_dot=%.6f signed_dot=%.6f within_session_stable=%u face_candidate_group=%c mean_abs_dot=%.6f min_abs_dot=%.6f opposite_A_C_dot=%.6f opposite_B_D_dot=%.6f complete_four_faces=%u signed_pattern=%s strong_axis_candidate=%u face_labels=R1_heuristic_NOT_user_mapping surface_verified=0",static_cast<unsigned long long>(pendingId),scope(i),offset(i),f.unitLast.x,f.unitLast.y,f.unitLast.z,axis.x,axis.y,axis.z,alignment,d,unsigned(f.stable()),faces::group(axis),r.mean,r.minimum,r.ac,r.bd,unsigned(r.complete),r.signedPattern,unsigned(r.strongAxis));
   }
   const auto& p=f.point;if(p.samples>=2){Vec3 delta=p.lastHero-p.last;
    log("static_field_point_score session=%llu scope=%s offset=0x%x candidate_point=(%.6f,%.6f,%.6f) hero_minus_point=(%.6f,%.6f,%.6f) axis_distance=%.6f lateral_error=%.6f axis_distance_rms=%.6f lateral_error_rms=%.6f endpoint_tracking_error=%.6f samples=%u contact_verified=0",static_cast<unsigned long long>(pendingId),scope(i),offset(i),p.last.x,p.last.y,p.last.z,delta.x,delta.y,delta.z,std::abs(diag::preciseDot(delta,axis)),std::sqrt(diag::preciseDot(delta-axis*float(diag::preciseDot(delta,axis)),delta-axis*float(diag::preciseDot(delta,axis)))),p.axialRms(axis),p.lateralRms(axis),std::sqrt(diag::preciseDot((p.lastHero-p.firstHero)-(p.last-p.first),(p.lastHero-p.firstHero)-(p.last-p.first))),p.samples);
   }
  }
 }
};
}
