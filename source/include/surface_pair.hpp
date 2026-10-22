#pragma once
#include "diagnostics.hpp"
#include <cstdint>
namespace ws::pair {
// Empirical all-sample median from v0.1a8 (6 sessions), not an applied offset.
inline constexpr float targetStandoff=.5f;
inline constexpr unsigned nearOffset=0x320,planeOffset=0x338,axisOffset=0x398;
struct Result {
 bool valid{},pairSamplePass{},frameValid{};const char* reason="invalid_data";
 Vec3 nearPoint{},planePoint{},axis{},delta{},tangentResidual{},correction{};
 double normalSeparation{},tangentError{},planeDistance{},normalLength{};
 SurfaceFrame surface{};
};
inline Result evaluate(Vec3 nearPoint,Vec3 planePoint,Vec3 normal,Vec3 hero,Vec3 velocity,bool motionValid){
 Result r;r.nearPoint=nearPoint;r.planePoint=planePoint;
 auto reasonable=[](Vec3 p){return finite(p)&&std::max({std::abs(p.x),std::abs(p.y),std::abs(p.z)})<1000000;};
 if(!reasonable(nearPoint)||!reasonable(planePoint)||!reasonable(hero)||!finite(normal))return r;
 r.normalLength=std::sqrt(diag::preciseDot(normal,normal));if(!std::isfinite(r.normalLength)||r.normalLength<.9||r.normalLength>1.1){r.reason="invalid_unit_axis";return r;}
 r.axis=normal*float(1./r.normalLength);if(std::abs(r.axis.y)>.35){r.reason="unsupported_nonvertical_plane";return r;}
 r.delta=nearPoint-planePoint;r.normalSeparation=diag::preciseDot(r.delta,r.axis);r.tangentResidual=r.delta-r.axis*float(r.normalSeparation);r.tangentError=std::sqrt(diag::preciseDot(r.tangentResidual,r.tangentResidual));
 r.planeDistance=diag::preciseDot(hero-planePoint,r.axis);r.correction=r.axis*float(double(targetStandoff)-r.planeDistance);r.valid=true;
 r.pairSamplePass=r.normalSeparation>=.4&&r.normalSeparation<=.6&&r.tangentError<=.1;
 if(!r.pairSamplePass){r.reason="pair_threshold_transient_or_mismatch";return r;}
 if(!motionValid||!finite(velocity)){r.reason="position_difference_velocity_unavailable";return r;}
 Vec3 projected=velocity-r.axis*float(diag::preciseDot(velocity,r.axis));if(diag::preciseDot(projected,projected)<.01){r.reason="projected_velocity_too_small";return r;}
 r.surface=ws::frame(planePoint,r.axis,projected);r.frameValid=r.surface.valid;r.reason=r.frameValid?"read_only_candidate":"invalid_basis";return r;
}
using Logger=void (*)(const char*,...);
class Observer {
 std::uint64_t session{};unsigned attempts{},samples{},thresholdPass{},frames{},positive{},negative{},planePositive{},sepPositive{};
 double sumSep{},minSep=1e30,maxSep=-1e30,sumTangent{},maxTangent{},sumDistance{},minDistance=1e30,maxDistance=-1e30,minAxisDot=1;
 Vec3 firstAxis{},lastAxis{},heading{};bool headingValid{};
public:
 void reset(){*this=Observer{};}
 void begin(std::uint64_t s){reset();session=s;}
 void sample(const Vec3* values,Vec3 hero,Vec3 velocity,bool motionValid,std::uint64_t ms,Logger log){
  ++attempts;if(!values){log("surface_pair_sample session=%llu sample_ms=%llu valid=0 reason=incomplete_same_worker_batch writes=0",static_cast<unsigned long long>(session),static_cast<unsigned long long>(ms));return;}
  auto r=evaluate(values[0],values[1],values[2],hero,velocity,motionValid);
  log("surface_pair_sample session=%llu sample_ms=%llu valid=%u pair_sample_pass=%u P320=(%.9g,%.9g,%.9g) P338=(%.9g,%.9g,%.9g) N398=(%.9g,%.9g,%.9g) normal_separation=%.9g tangent_error=%.9g plane_distance=%.9g target_standoff=%.6f target_source=V01A8_all_sample_median surface_sign_candidate=%s reason=%s sample_source=non_atomic_same_worker_batch writes=0",static_cast<unsigned long long>(session),static_cast<unsigned long long>(ms),unsigned(r.valid),unsigned(r.pairSamplePass),values[0].x,values[0].y,values[0].z,values[1].x,values[1].y,values[1].z,values[2].x,values[2].y,values[2].z,r.normalSeparation,r.tangentError,r.planeDistance,targetStandoff,r.valid&&r.normalSeparation>0&&r.planeDistance>0?"OUTWARD_FROM_SURFACE":"UNRESOLVED",r.reason);
  if(!r.valid)return;
  if(!samples)firstAxis=r.axis;
  lastAxis=r.axis;minAxisDot=std::min(minAxisDot,diag::preciseDot(firstAxis,r.axis));++samples;thresholdPass+=r.pairSamplePass;frames+=r.frameValid;
  sumSep+=r.normalSeparation;minSep=std::min(minSep,r.normalSeparation);maxSep=std::max(maxSep,r.normalSeparation);sumTangent+=r.tangentError;maxTangent=std::max(maxTangent,r.tangentError);sumDistance+=r.planeDistance;minDistance=std::min(minDistance,r.planeDistance);maxDistance=std::max(maxDistance,r.planeDistance);planePositive+=r.planeDistance>0;sepPositive+=r.normalSeparation>0;
  // Reversal evidence is horizontal travel along this vertical wall, not Up/Down.
  if(motionValid&&finite(velocity)){
   if(!headingValid)headingValid=unit(cross(diag::worldUp,r.axis),heading);
   if(headingValid){double speed=diag::preciseDot(velocity,heading);if(speed>1)++positive;else if(speed<-1)++negative;}
  }
  const auto& f=r.surface;
  log("surface_frame_dryrun session=%llu sample_ms=%llu valid=%u point=(%.9g,%.9g,%.9g) up=(%.9g,%.9g,%.9g) down=(%.9g,%.9g,%.9g) forward=(%.9g,%.9g,%.9g) right=(%.9g,%.9g,%.9g) movement_direction=(%.9g,%.9g,%.9g) movement_source=position_difference_NOT_physics_velocity reason=%s applied=0",static_cast<unsigned long long>(session),static_cast<unsigned long long>(ms),unsigned(r.frameValid),r.planePoint.x,r.planePoint.y,r.planePoint.z,r.axis.x,r.axis.y,r.axis.z,-r.axis.x,-r.axis.y,-r.axis.z,f.forward.x,f.forward.y,f.forward.z,f.right.x,f.right.y,f.right.z,velocity.x,velocity.y,velocity.z,r.reason);
  log("adhesion_dryrun session=%llu sample_ms=%llu plane_distance=%.9g target_standoff=%.6f dryrun_adhesion_correction=(%.9g,%.9g,%.9g) candidate_quality=%u plane_reference_status=surface_plane_reference_candidate contact_verified=0 surface_sign_verified=0 applied=0",static_cast<unsigned long long>(session),static_cast<unsigned long long>(ms),r.planeDistance,targetStandoff,r.correction.x,r.correction.y,r.correction.z,unsigned(r.pairSamplePass));
 }
 void finish(Logger log,bool usableSession){
  bool pairCandidate=usableSession&&samples>=3&&samples==attempts&&minSep>=.4&&maxSep<=.6&&sumTangent/samples<=.1&&minAxisDot>.98;
  log("surface_pair_summary session=%llu attempts=%u valid_samples=%u pair_sample_pass_count=%u frame_valid_samples=%u normal_separation_mean=%.9g normal_separation_min=%.9g normal_separation_max=%.9g tangent_error_mean=%.9g tangent_error_max=%.9g plane_distance_mean=%.9g plane_distance_min=%.9g plane_distance_max=%.9g target_standoff=%.6f min_axis_dot_first=%.9g forward_samples=%u reverse_samples=%u movement_direction_invariant_candidate=%u surface_plane_reference_candidate=%u surface_sign_candidate=%s surface_axis_verified=0 surface_sign_verified=0 plane_reference_verified=0 diagonal_surface_verified=0 v01b_gate=WAITING_DIAGONAL_GROUND_TRUTH applied=0",static_cast<unsigned long long>(session),attempts,samples,thresholdPass,frames,samples?sumSep/samples:0,samples?minSep:0,samples?maxSep:0,samples?sumTangent/samples:0,maxTangent,samples?sumDistance/samples:0,samples?minDistance:0,samples?maxDistance:0,targetStandoff,minAxisDot,positive,negative,unsigned(pairCandidate&&positive>=2&&negative>=2),unsigned(pairCandidate),pairCandidate&&sepPositive==samples&&planePositive==samples?"OUTWARD_FROM_SURFACE":"UNRESOLVED");
 }
 void crawl(Vec3 axis,std::uint64_t id,Logger log){if(id==session&&samples)log("surface_pair_crawl_score session=%llu primary_offset=0x398 signed_dot_first=%.9g signed_dot_last=%.9g source=post_Deinit_R1_crawl_like_NOT_engine_geometry surface_axis_verified=0 sign_verified=0",static_cast<unsigned long long>(id),diag::preciseDot(firstAxis,axis),diag::preciseDot(lastAxis,axis));}
};
}
