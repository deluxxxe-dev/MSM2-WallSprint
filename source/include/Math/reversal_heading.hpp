#pragma once
#include "Math/source_orientation.hpp"
namespace ws::heading {
inline constexpr double observationIntervalMs=33.;
inline constexpr double reversalTurnDurationMs=120.; // Chosen experiment; not animation/input latency.
// Experimental displacement thresholds, not engine constants. No memory access.
struct Result {diag::ActorAxes basis{};Vec3 delta{},target{};float speed{};int candidate{},accepted{1};const char* event="hold";};
struct Reversal {
 Vec3 f0{},normal{},position{};bool ready{},havePosition{},turning{};int sign{1},pending{};
 unsigned update{},pendingCount{};double observationMs{},pendingMs{},turnStart{},lastClock{};int from{1};
 bool haveSample{};double sampleWall{},seenWall{};unsigned sampleUpdate{},seenUpdate{};
 void clearObservation(){havePosition=false;pending=pendingCount=0;}
 void reset(){*this={};}
 bool enter(diag::ActorAxes entry){reset();if(!entry.valid||!diag::measure({entry.right,entry.up,entry.forward}).properRotation)return false;ready=unit(entry.up,normal)&&unit(entry.forward,f0);return ready;}
 bool basis(Vec3 forward,diag::ActorAxes& out,bool endpoint=false) const {Vec3 r{},f{};if(!unit(cross(normal,forward),r)||!unit(cross(r,normal),f))return false;out={r,normal,endpoint?forward:f,true};return diag::measure({r,normal,f}).properRotation;}
 Result sample(Vec3 p,unsigned nativeUpdate,double wallMs,double clock){
  Result out;out.accepted=sign;out.target=f0*float(sign);
  if(!ready||!finite(p)||!std::isfinite(wallMs)||!std::isfinite(clock)||clock<lastClock){clearObservation();return out;}
  const bool regression=haveSample&&(nativeUpdate<seenUpdate||wallMs<seenWall);
  const bool wait=haveSample&&!regression&&(turning?(nativeUpdate==sampleUpdate||wallMs==sampleWall):(havePosition&&(nativeUpdate==update||wallMs==observationMs)));
  if(regression){clearObservation();position=p;observationMs=wallMs;update=nativeUpdate;havePosition=true;out.event="observation_reset";}
  if(wait||regression)clock=lastClock;
  else lastClock=clock;
  // Transport ordering only: never renew observation baseline, candidate or clock on WAIT.
  seenWall=wallMs;seenUpdate=nativeUpdate;
  if(!wait){haveSample=true;sampleWall=wallMs;sampleUpdate=nativeUpdate;}
  if(turning){
   double t=std::clamp((clock-turnStart)/reversalTurnDurationMs,0.,1.);float angle=float(t*3.14159265358979323846);
   Vec3 start=f0*float(from);Vec3 f=start*std::cos(angle)+cross(normal,start)*std::sin(angle);
   if(t>=1&&!wait&&!regression){turning=false;f=f0*float(sign);clearObservation();out.event="turn_complete";}
   if(!basis(f,out.basis,t>=1))return out;
   return out; // No retargeting or displacement accumulation during a turn.
  }
  if(!basis(out.target,out.basis,true))return out;
  if(wait||regression)return out;
  auto rebase=[&]{position=p;observationMs=wallMs;update=nativeUpdate;havePosition=true;};
  if(!havePosition){rebase();return out;}
  if(nativeUpdate==update)return out;
  if(nativeUpdate<update||wallMs<observationMs||wallMs-observationMs>250){clearObservation();rebase();out.event="observation_reset";return out;}
  double dt=wallMs-observationMs;if(dt<observationIntervalMs)return out;
  out.delta=p-position;Vec3 tangent=out.delta-normal*dot(out.delta,normal);out.speed=std::sqrt(dot(tangent,tangent))*float(1000./dt);rebase();
  Vec3 direction{};if(!std::isfinite(out.speed)||out.speed<2||!unit(tangent,direction)){pending=pendingCount=0;return out;}
  float d=dot(direction,f0);out.candidate=d>=.8f?1:d<=-.8f?-1:0;
  if(!out.candidate||out.candidate==sign){pending=pendingCount=0;return out;}
  out.event="candidate";
  if(pending!=out.candidate){pending=out.candidate;pendingCount=1;pendingMs=wallMs;return out;}
  ++pendingCount;
  if(pendingCount>=2&&wallMs-pendingMs>=observationIntervalMs){from=sign;sign=pending;turning=true;turnStart=clock;out.accepted=sign;out.target=f0*float(sign);out.event="accepted";clearObservation();}
  return out;
 }
};
}
