#pragma once
#include "wallsprint.hpp"
#include <array>
#include <cstdint>
#include <limits>
namespace ws::diag {
using Basis=std::array<Vec3,3>;
struct Metrics {
 std::array<double,3> length{},pairDot{};
 double determinant{},orthogonalityError{};
 bool finite{},orthonormal{},properRotation{};
};
inline double preciseDot(Vec3 a,Vec3 b){return double(a.x)*b.x+double(a.y)*b.y+double(a.z)*b.z;}
inline Metrics measure(const Basis& axes){
 Metrics result{};result.finite=true;
 for(auto v:axes)result.finite=result.finite&&ws::finite(v);
 if(!result.finite){result.determinant=result.orthogonalityError=std::numeric_limits<double>::quiet_NaN();return result;}
 double error=0;
 for(unsigned i=0;i<3;++i){double squared=preciseDot(axes[i],axes[i]);result.length[i]=std::sqrt(squared);error+=(squared-1)*(squared-1);}
 unsigned k=0;for(unsigned i=0;i<3;++i)for(unsigned j=i+1;j<3;++j){double d=preciseDot(axes[i],axes[j]);result.pairDot[k++]=d;error+=2*d*d;}
 const auto a=axes[0],b=axes[1],c=axes[2];
 result.determinant=double(a.x)*(double(b.y)*c.z-double(b.z)*c.y)-double(a.y)*(double(b.x)*c.z-double(b.z)*c.x)+double(a.z)*(double(b.x)*c.y-double(b.y)*c.x);
 result.orthogonalityError=std::sqrt(error);
 result.orthonormal=result.orthogonalityError<.02&&std::abs(std::abs(result.determinant)-1)<.02;
 result.properRotation=result.orthonormal&&result.determinant>0;
 return result;
}
struct Orientation {Basis rows{},columns{};Metrics rowMetrics{},columnMetrics{};};
inline Orientation analyze(const float* raw){
 Orientation out;
 for(unsigned i=0;i<3;++i){out.rows[i]={raw[i*4],raw[i*4+1],raw[i*4+2]};out.columns[i]={raw[i],raw[4+i],raw[8+i]};}
 out.rowMetrics=measure(out.rows);out.columnMetrics=measure(out.columns);return out;
}
inline constexpr Vec3 worldUp{0,1,0};
struct ActorAxes {Vec3 right,up,forward;bool valid;};
inline ActorAxes actorAxes(const float* raw){auto o=analyze(raw);return {o.rows[0],o.rows[1],o.rows[2],o.rowMetrics.properRotation};}
constexpr const char* markers[]={"MARK_GROUND_IDLE","MARK_GROUND_RUN","MARK_AIR","MARK_WALLRUN_UP","MARK_WALLRUN_SIDE","MARK_WALLCRAWL"};
class MarkerKeys {
 unsigned previous{};
public:
 unsigned update(bool foreground,unsigned numberKeys){
  unsigned chord=foreground?(numberKeys&63u):0;
  unsigned pressed=chord&~previous;previous=chord;return pressed;
 }
};
struct Motion {bool changed{},valid{};Vec3 velocity{};std::uint64_t epoch{};};
class History {
 std::uintptr_t hero{},transform{};Vec3 position{};std::uint64_t time{},epoch{};bool present{};
public:
 Motion update(bool available,std::uintptr_t newHero,std::uintptr_t newTransform,Vec3 p,std::uint64_t now){
  if(!newHero||!newTransform||!ws::finite(p))available=false;
  Motion out;out.changed=available!=present||(available&&(newHero!=hero||newTransform!=transform));
  if(out.changed)++epoch;
  out.epoch=epoch;
  if(available&&present&&!out.changed&&now>time&&now-time<=500){
   out.velocity=(p-position)*(1000.f/float(now-time));out.valid=ws::finite(out.velocity)&&preciseDot(out.velocity,out.velocity)<=1000.*1000.;
  }
  present=available;hero=available?newHero:0;transform=available?newTransform:0;position=available?p:Vec3{};time=available?now:0;
  return out;
 }
};
}
