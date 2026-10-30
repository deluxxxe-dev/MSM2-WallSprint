#pragma once
#include "Math/source_orientation.hpp"
namespace ws::heading {
inline constexpr double observationIntervalMs=33.;
inline constexpr double reversalTurnDurationMs=120.; // Experiment, not animation/input latency.
inline constexpr double pi=3.14159265358979323846;
// Named experiment thresholds; not engine facts. Speed/gap gates are unchanged.
inline constexpr float minimumTangentSpeed=2.f;
inline constexpr double maximumObservationGapMs=250.;
inline constexpr double headingDeadbandRadians=12.*pi/180.;
inline constexpr double candidateStabilityRadians=8.*pi/180.;
inline constexpr double oppositeTieEpsilon=1e-6;
inline double angle(Vec3 from,Vec3 to,Vec3 normal){
 double s=dot(normal,cross(from,to)),c=std::clamp(double(dot(from,to)),-1.,1.);
 // Deterministic positive half-turn only at the numerical antipode.
 if(c<0&&std::abs(s)<=oppositeTieEpsilon)return pi;
 return std::atan2(s,c);
}
struct Result {
 diag::ActorAxes basis{};Vec3 delta{},target{},candidate{};float speed{};
 double candidateAngle{},turnAngle{};const char* event="hold";
};
struct Reversal {
 Vec3 f0{},normal{},position{},forward{},pending{},from{};
 bool ready{},havePosition{},turning{};
 unsigned update{},pendingCount{};double observationMs{},pendingMs{},turnStart{},lastClock{},turnAngle{};
 bool haveSample{};double sampleWall{},seenWall{};unsigned sampleUpdate{},seenUpdate{};
 void clearObservation(){havePosition=false;pending={};pendingCount=0;}
 void reset(){*this={};}
 bool enter(diag::ActorAxes entry){reset();if(!entry.valid||!diag::measure({entry.right,entry.up,entry.forward}).properRotation)return false;ready=unit(entry.up,normal)&&unit(entry.forward,f0);forward=f0;return ready;}
 bool basis(Vec3 target,diag::ActorAxes& out) const {
  Vec3 r{},f{};if(!unit(cross(normal,target),r)||!unit(cross(r,normal),f))return false;
  out={r,normal,f,true};return diag::measure({r,normal,f}).properRotation;
 }
 Result sample(Vec3 p,unsigned nativeUpdate,double wallMs,double clock){
  Result out;out.target=forward;out.turnAngle=turnAngle;
  if(!ready||!finite(p)||!std::isfinite(wallMs)||!std::isfinite(clock)||clock<lastClock){clearObservation();return out;}
  const bool regression=haveSample&&(nativeUpdate<seenUpdate||wallMs<seenWall);
  const bool wait=haveSample&&!regression&&(turning?(nativeUpdate==sampleUpdate||wallMs==sampleWall):(havePosition&&(nativeUpdate==update||wallMs==observationMs)));
  if(regression){clearObservation();position=p;observationMs=wallMs;update=nativeUpdate;havePosition=true;out.event="observation_reset";}
  if(wait||regression)clock=lastClock;
  else lastClock=clock;
  // WAIT must not renew baseline, candidate or turn clock.
  seenWall=wallMs;seenUpdate=nativeUpdate;
  if(!wait){haveSample=true;sampleWall=wallMs;sampleUpdate=nativeUpdate;}
  if(turning){
   double t=std::clamp((clock-turnStart)/reversalTurnDurationMs,0.,1.);
   float a=float(t*turnAngle);Vec3 f=from*std::cos(a)+cross(normal,from)*std::sin(a);
   if(t>=1&&!wait&&!regression){turning=false;f=forward;clearObservation();out.event="turn_complete";}
   if(!basis(f,out.basis))return out;
   return out; // No mid-turn retarget or displacement accumulation.
  }
  if(!basis(forward,out.basis))return out;
  if(wait||regression)return out;
  auto rebase=[&]{position=p;observationMs=wallMs;update=nativeUpdate;havePosition=true;};
  if(!havePosition){rebase();return out;}
  if(nativeUpdate==update)return out;
  if(nativeUpdate<update||wallMs<observationMs||wallMs-observationMs>maximumObservationGapMs){clearObservation();rebase();out.event="observation_reset";return out;}
  double dt=wallMs-observationMs;if(dt<observationIntervalMs)return out;
  out.delta=p-position;Vec3 tangent=out.delta-normal*dot(out.delta,normal);out.speed=std::sqrt(dot(tangent,tangent))*float(1000./dt);rebase();
  Vec3 direction{};
  auto cancel=[&]{pending={};pendingCount=0;};
  if(!std::isfinite(out.speed)||out.speed<minimumTangentSpeed||!unit(tangent,direction)){cancel();out.event="low_speed_hold";return out;}
  out.candidate=direction;out.candidateAngle=angle(forward,direction,normal);
  if(std::abs(out.candidateAngle)<=headingDeadbandRadians){cancel();out.event="deadband_hold";return out;}
  out.event="candidate";
  // Anchor to the FIRST candidate, not the latest vote: rotating evidence cannot drift into acceptance.
  if(!pendingCount||std::abs(angle(pending,direction,normal))>candidateStabilityRadians){
   out.event=pendingCount?"candidate_unstable":"candidate";pending=direction;pendingCount=1;pendingMs=wallMs;return out;
  }
  ++pendingCount;
  if(pendingCount>=2&&wallMs-pendingMs>=observationIntervalMs){
   from=forward;forward=direction;turnAngle=angle(from,forward,normal);turning=true;turnStart=clock;
   out.target=forward;out.turnAngle=turnAngle;out.event="accepted";clearObservation();
  }
  return out;
 }
};
}
