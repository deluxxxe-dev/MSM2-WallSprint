#pragma once
#include <cmath>
#include <algorithm>
namespace ws {
struct Vec3 { float x{},y{},z{}; Vec3 operator+(Vec3 b)const{return{x+b.x,y+b.y,z+b.z};} Vec3 operator-(Vec3 b)const{return{x-b.x,y-b.y,z-b.z};} Vec3 operator*(float s)const{return{x*s,y*s,z*s};} };
inline float dot(Vec3 a,Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
inline Vec3 cross(Vec3 a,Vec3 b){return{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
inline bool finite(Vec3 v){return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
inline bool unit(Vec3 v,Vec3& out){float n=dot(v,v);if(!finite(v)||!std::isfinite(n)||n<1e-8f)return false;out=v*(1/std::sqrt(n));return true;}
struct SurfaceFrame {Vec3 point,normal,up,down,forward,right;bool valid{};};
inline SurfaceFrame frame(Vec3 p,Vec3 n,Vec3 direction){SurfaceFrame f{}; f.point=p;if(!finite(p)||!unit(n,f.up))return f;f.normal=f.up;f.down=f.up*-1; if(!unit(direction-f.up*dot(direction,f.up),f.forward))return f; if(!unit(cross(f.up,f.forward),f.right))return f;f.valid=true;return f;}
// Basis columns are right/up/forward. Adapter converts handedness to the game convention.
struct Quaternion {float x{},y{},z{},w{1};};
inline Quaternion rotation(const SurfaceFrame& f){float m[3][3]={{f.right.x,f.up.x,f.forward.x},{f.right.y,f.up.y,f.forward.y},{f.right.z,f.up.z,f.forward.z}};Quaternion q;float t=m[0][0]+m[1][1]+m[2][2]; if(t>0){float s=2*std::sqrt(t+1);q={ (m[2][1]-m[1][2])/s,(m[0][2]-m[2][0])/s,(m[1][0]-m[0][1])/s,s/4};}else{int i=0;if(m[1][1]>m[0][0])i=1;if(m[2][2]>m[i][i])i=2;int j=(i+1)%3,k=(j+1)%3;float s=2*std::sqrt(1+m[i][i]-m[j][j]-m[k][k]);float v[3]{};v[i]=s/4;v[j]=(m[j][i]+m[i][j])/s;v[k]=(m[k][i]+m[i][k])/s;q={v[0],v[1],v[2],(m[k][j]-m[j][k])/s};}return q;}
enum class State { Vanilla, WallSprint };
enum class Exit {None,LostSurface,InvalidData,NormalChange,Forbidden,Unsupported,Disabled,WallRunEnded};
struct Config {float delay=.7f,minSpeed=2,speed=8,offset=.4f,probeLength=1.2f,minNormalDot=.95f,maxSpeed=100;bool debug=true;};
struct Sample {float dt{};bool wallRun{},contact{},combat{},cutscene{},dead{},fastTravel{},forced{},supported=true;Vec3 position,velocity,normal;float inputRight{},inputForward{};};
struct Hit {bool valid{};Vec3 point,normal;};
struct Debug {State state{};float elapsed{},distance{};SurfaceFrame surface;Vec3 velocity;bool entry{};Exit reason{};};
// No game memory access in this module. All methods must execute on the game update thread.
struct Host {virtual ~Host()=default;virtual Hit probe(Vec3 origin,Vec3 direction,float length)=0;virtual bool apply(const SurfaceFrame&,Quaternion,Vec3 velocity,Vec3 targetPosition)=0;virtual void releaseToVanillaAir(Exit) noexcept=0;virtual void log(const Debug&)=0;};
class Controller {
 Config c;State state=State::Vanilla;float elapsed{};SurfaceFrame surface{};Exit reason=Exit::None;
 void exit(Host& h,Exit r){if(state==State::WallSprint)h.releaseToVanillaAir(r);state=State::Vanilla;elapsed=0;surface={};reason=r;}
public:
 explicit Controller(Config cfg={}):c(cfg){}
 State current()const{return state;}
 void disable(Host& h){exit(h,Exit::Disabled);}
 void tick(const Sample& s,Host& h){
  reason=Exit::None;bool entry=false;float distance=0;
  auto emit=[&]{if(c.debug)h.log({state,elapsed,distance,surface,s.velocity,entry,reason});};
  if(!std::isfinite(s.dt)||s.dt<=0||s.dt>.1f||!finite(s.position)||!finite(s.velocity)||dot(s.velocity,s.velocity)>c.maxSpeed*c.maxSpeed||!std::isfinite(s.inputRight)||!std::isfinite(s.inputForward)){exit(h,Exit::InvalidData);emit();return;}
  if(s.combat||s.cutscene||s.dead||s.fastTravel||s.forced){exit(h,Exit::Forbidden);emit();return;}
  if(!s.supported){exit(h,Exit::Unsupported);emit();return;}
  if(state==State::WallSprint&&!s.wallRun){exit(h,Exit::WallRunEnded);emit();return;}
  if(state==State::Vanilla){
   if(!s.wallRun){elapsed=0;emit();return;} elapsed+=s.dt;
   if(elapsed<c.delay||!s.contact||dot(s.velocity,s.velocity)<c.minSpeed*c.minSpeed){emit();return;}
   surface=frame(s.position,s.normal,s.velocity);
   if(!surface.valid||std::abs(surface.up.y)>.3f){exit(h,Exit::InvalidData);emit();return;}
   entry=true;
  }
  Hit hit=h.probe(s.position,surface.down,c.probeLength);
  if(!hit.valid){exit(h,Exit::LostSurface);emit();return;}
  Vec3 n;if(!finite(hit.point)||!unit(hit.normal,n)){exit(h,Exit::InvalidData);emit();return;}
  if(dot(n,surface.up)<c.minNormalDot){exit(h,Exit::NormalChange);emit();return;}
  Vec3 separation=s.position-hit.point;distance=dot(separation,n);
  if(distance<0||distance>c.probeLength||dot(separation,separation)>c.probeLength*c.probeLength){exit(h,Exit::LostSurface);emit();return;}
  // Retain the input reference heading so holding strafe does not cause continuous turning.
  surface=frame(hit.point,n,surface.forward);
  Vec3 direction=surface.right*std::clamp(s.inputRight,-1.f,1.f)+surface.forward*std::clamp(s.inputForward,-1.f,1.f);
  float magnitude=std::sqrt(dot(direction,direction));if(magnitude>1)direction=direction*(1/magnitude);
  SurfaceFrame pose=surface;if(magnitude>1e-4f)pose=frame(hit.point,n,direction);
  state=State::WallSprint; // Claim ownership before apply so partial failures also release.
  if(!h.apply(pose,rotation(pose),direction*c.speed,hit.point+n*c.offset)){exit(h,Exit::Unsupported);emit();return;}
  emit();
 }
};
}
