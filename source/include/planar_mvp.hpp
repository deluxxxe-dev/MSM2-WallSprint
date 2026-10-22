#pragma once
#include "diagnostics.hpp"
#include <cstdint>
namespace ws::planar {
struct Config {
 bool enable=true,orientation=true,adhesion=false,movement=false,debug=true;
 bool postUpdate=true,postDiagnostic=true;
 float delay=.7f,rotateTime=.25f,standoff=.5f,maxAdhesion=.05f,speedScale=.7f;
};
inline bool valid(const Config& c){return std::isfinite(c.delay)&&c.delay>=.7f&&c.delay<=5&&std::isfinite(c.rotateTime)&&c.rotateTime>=.2f&&c.rotateTime<=1&&std::isfinite(c.standoff)&&c.standoff>=.1f&&c.standoff<=1&&std::isfinite(c.maxAdhesion)&&c.maxAdhesion>0&&c.maxAdhesion<=.08f&&std::isfinite(c.speedScale)&&c.speedScale>0&&c.speedScale<=1&&(!c.adhesion||c.orientation)&&(!c.movement||(c.orientation&&c.adhesion));}
inline Quaternion normalized(Quaternion q){float n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);return {q.x/n,q.y/n,q.z/n,q.w/n};}
inline Quaternion slerp(Quaternion a,Quaternion b,float t){a=normalized(a);b=normalized(b);float d=a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;if(d<0){d=-d;b={-b.x,-b.y,-b.z,-b.w};}d=std::clamp(d,0.f,1.f);float x=1-t,y=t;if(d<.9995f){float theta=std::acos(d),s=std::sin(theta);x=std::sin((1-t)*theta)/s;y=std::sin(t*theta)/s;}return normalized({a.x*x+b.x*y,a.y*x+b.y*y,a.z*x+b.z*y,a.w*x+b.w*y});}
inline diag::ActorAxes axes(Quaternion q){q=normalized(q);float x=q.x,y=q.y,z=q.z,w=q.w;return {{1-2*(y*y+z*z),2*(x*y+w*z),2*(x*z-w*y)},{2*(x*y-w*z),1-2*(x*x+z*z),2*(y*z+w*x)},{2*(x*z+w*y),2*(y*z-w*x),1-2*(x*x+y*y)},true};}
inline Quaternion quaternion(diag::ActorAxes a){SurfaceFrame f{};f.right=a.right;f.up=a.up;f.forward=a.forward;return normalized(rotation(f));}
struct Input {
 std::uint64_t session{};std::uintptr_t hero{},transform{},owner{};double now{},elapsed{},dt{};
 Vec3 normal{},point{},reference{},position{},velocity{};bool motionValid{},lifecycle{},entrySeen{},kill{};
 diag::ActorAxes actor{};float inputRight{},inputForward{};
 // Measured at next PRE, never infer survival from a POST_NATIVE basis.
 bool survivalKnown{},previousWriteSurvived=true;
};
struct Action {
 bool entered{},exited{},active{},orientation{},adhesion{},movement{},nativeOverwrite{};const char* reason="none";
 SurfaceFrame frame{};diag::ActorAxes before{},pose{};Vec3 position{},adhesionPosition{},movementPosition{},adhesionCorrection{},movementCorrection{},projectedMovement{};
 double elapsed{},duration{},distance{},error{},maxPlaneError{},maxNormalDelta{};unsigned phase{};
};
class Controller {
 Config config;bool active{},blocked{},wrote{};std::uint64_t session{};std::uintptr_t hero{},transform{},owner{};
 double started{};Vec3 entryNormal{},lastNormal{},entryPoint{},heading{},right{},lastPosition{};Quaternion startRotation{},lastRotation{};diag::ActorAxes lastPose{};float baseSpeed{};
 unsigned overwrites{};double maxError{},maxDelta{};
 Action end(const char* why,double now){Action a;a.exited=active;a.reason=why;a.duration=active?std::max(0.,now-started):0;a.maxPlaneError=maxError;a.maxNormalDelta=maxDelta;active=false;blocked=true;wrote=false;return a;}
public:
 explicit Controller(Config c={}):config(c){}
 void configure(Config c){config=c;active=blocked=wrote=false;session=0;}
 bool isActive()const{return active;}
 Action abort(const char* why,double now){return end(why,now);}
 Action update(const Input& in){
  Action a;a.elapsed=in.elapsed;a.before=in.actor;a.position=a.adhesionPosition=a.movementPosition=in.position;a.phase=config.movement?3:config.adhesion?2:1;
  if(in.session!=session){active=blocked=wrote=false;session=in.session;overwrites=0;maxError=maxDelta=0;}
  auto reject=[&](const char* why){if(active)return end(why,in.now);auto b=a;b.reason=why;return b;};
  if(in.kill||!config.enable||!valid(config))return end(in.kill?"kill_switch":"config_or_master_disabled",in.now);
  if(!in.lifecycle||!in.entrySeen)return reject("verified_lifecycle_or_Entry_lost");
  if(blocked)return a;
  if(active&&(in.hero!=hero||in.transform!=transform||in.owner!=owner))return end("Hero_Transform_or_WallRun_owner_changed",in.now);
  if(!in.hero||!in.transform||!in.owner||!in.actor.valid||!finite(in.position)||!finite(in.point)||!finite(in.reference)||!finite(in.normal))return reject("invalid_actor_or_surface_data");
  for(auto value:{in.position,in.point,in.reference})if(std::max({std::abs(value.x),std::abs(value.y),std::abs(value.z)})>=1000000)return reject("unreasonable_world_coordinate");
  double len=std::sqrt(diag::preciseDot(in.normal,in.normal));if(len<.95||len>1.05||!std::isfinite(len))return reject("invalid_normal_length");
  Vec3 n=in.normal*float(1/len);if(std::abs(n.y)>.2f)return reject("unsupported_slope_floor_or_ceiling");
  a.distance=diag::preciseDot(in.position-in.point,n);a.error=config.standoff-a.distance;maxError=std::max(maxError,std::abs(a.error));
  double separation=diag::preciseDot(in.reference-in.point,n);
  if(!std::isfinite(a.distance)||a.distance<=.05||std::abs(a.error)>.5||separation<.4||separation>.6)return reject("invalid_plane_distance_or_surface_pair");
  if(active){double d=diag::preciseDot(lastNormal,n);maxDelta=std::max(maxDelta,std::acos(std::clamp(d,-1.,1.)));if(d<.95||diag::preciseDot(entryNormal,n)<.95||std::abs(diag::preciseDot(in.point-entryPoint,entryNormal))>.1)return end("corner_or_invalid_surface",in.now);}
  if(!std::isfinite(in.dt)||in.dt<=0||in.dt>.1||!std::isfinite(in.now))return reject("native_update_time_gap");
  if(!in.motionValid||!finite(in.velocity)||diag::preciseDot(in.velocity,in.velocity)>10000)return reject("velocity_unavailable_or_abnormal");
  Vec3 direction=in.velocity-n*float(diag::preciseDot(in.velocity,n));
  if(!active&&in.elapsed<config.delay)return a;
  if(!active){
   if(!unit(direction,heading)){direction=in.actor.forward-n*float(diag::preciseDot(in.actor.forward,n));if(!unit(direction,heading)){a.reason="no_planar_velocity_or_forward";return a;}}
   if(!unit(cross(n,heading),right)||!unit(cross(right,n),heading))return a;
   active=true;started=in.now;hero=in.hero;transform=in.transform;owner=in.owner;entryNormal=lastNormal=n;entryPoint=in.point;startRotation=lastRotation=quaternion(in.actor);baseSpeed=std::clamp(float(std::sqrt(diag::preciseDot(in.velocity,in.velocity))),2.f,30.f);lastPosition=in.position;wrote=false;overwrites=0;a.entered=true;
  }
  if(wrote&&config.orientation&&in.survivalKnown){a.nativeOverwrite=!in.previousWriteSurvived;if(a.nativeOverwrite)++overwrites;else overwrites=0;if(overwrites>=4)return end("native_orientation_overwrite_4_updates",in.now);}
  float ix=std::clamp(in.inputRight,-1.f,1.f),iy=std::clamp(in.inputForward,-1.f,1.f);if(!std::isfinite(ix)||!std::isfinite(iy))return end("invalid_movement_input",in.now);
  Vec3 desired=right*ix+heading*iy;float magnitude=std::sqrt(dot(desired,desired));if(magnitude>1)desired=desired*(1/magnitude);
  a.projectedMovement=config.movement?desired*(baseSpeed*config.speedScale):direction;
  Vec3 forward;
  if(config.movement&&dot(desired,desired)>.0001f)direction=desired;
  if(!unit(direction,forward)){direction=in.actor.forward-n*float(diag::preciseDot(in.actor.forward,n));if(!unit(direction,forward))forward=heading;}
  a.frame=frame(in.point,n,forward);if(!a.frame.valid)return end("invalid_target_basis",in.now);
  auto target=rotation(a.frame);float t=std::clamp(float((in.now-started)/config.rotateTime),0.f,1.f);auto q=t<1?slerp(startRotation,target,t):slerp(lastRotation,target,std::min(1.f,float(in.dt/config.rotateTime)*4));
  a.pose=axes(q);if(!diag::measure({a.pose.right,a.pose.up,a.pose.forward}).properRotation)return end("invalid_interpolated_basis",in.now);
  a.orientation=config.orientation;
  if(config.adhesion){a.adhesionCorrection=n*std::clamp(float(a.error),-config.maxAdhesion,config.maxAdhesion);a.adhesionPosition=in.position+a.adhesionCorrection;a.adhesion=true;}
  a.movementPosition=a.adhesionPosition;
  if(config.movement){Vec3 targetPosition=lastPosition+a.projectedMovement*float(in.dt);auto correction=targetPosition-a.adhesionPosition;correction=correction-n*float(diag::preciseDot(correction,n));float length=std::sqrt(dot(correction,correction));if(!finite(correction)||length>.5)return end("movement_correction_too_large",in.now);if(length>.2f)correction=correction*(.2f/length);a.movementCorrection=correction;a.movementPosition=a.adhesionPosition+correction;a.movement=true;}
  a.active=true;a.duration=in.now-started;a.maxPlaneError=maxError;a.maxNormalDelta=maxDelta;lastNormal=n;lastRotation=q;return a;
 }
 void committed(const Action& a){if(a.active){wrote=true;lastPose=a.pose;lastPosition=a.movementPosition;}}
};
}
