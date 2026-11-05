#include "Math/reversal_heading.hpp"
#include "Math/compact_basis.hpp"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace ws;
static diag::ActorAxes entry{{0,1,0},{0,0,1},{1,0,0},true};
static void check(const heading::Result& r){assert(r.basis.valid&&diag::measure({r.basis.right,r.basis.up,r.basis.forward}).properRotation);assert(dot(r.basis.up,entry.up)>.99999f);}
static heading::Reversal ready(){heading::Reversal h;assert(h.enter(entry));h.sample({},1,0,0);return h;}
static void reverse(heading::Reversal& h){auto a=h.sample({-.4f,0,0},2,100,100);check(a);assert(dot(a.candidate,entry.forward)<-.99999f&&!h.turning);auto b=h.sample({-.8f,0,0},3,200,200);check(b);assert(h.turning&&dot(h.forward,entry.forward)<-.99999f);}
static Vec3 rotated(Vec3 f,Vec3 n,double angle){return f*float(std::cos(angle))+cross(n,f)*float(std::sin(angle));}
static void near(Vec3 a,Vec3 b){assert(dot(a,b)>.99999f);}
static void wallPlaneCases(){
 // World-space walls at arbitrary azimuth, not screen-space axes.
 for(Vec3 n:{Vec3{0,0,1},Vec3{1,0,0},Vec3{.6f,0,.8f}}){
  Vec3 f{};assert(unit(cross(Vec3{0,1,0},n),f));Vec3 r=cross(n,f);
  diag::ActorAxes e{r,n,f,true};
  for(double a:{heading::pi/2,-heading::pi/2,heading::pi/4,-heading::pi/4,20*heading::pi/180,-20*heading::pi/180,3*heading::pi/4,-3*heading::pi/4,heading::pi}){
   heading::Reversal h;assert(h.enter(e));Vec3 d=rotated(f,n,a);h.sample({},1,0,0);
   auto c=h.sample(d*.4f,2,100,100);assert(h.pendingCount==1&&!h.turning);near(c.basis.forward,f);
   auto accepted=h.sample(d*.8f,3,200,200);assert(h.turning);near(accepted.target,d);near(accepted.basis.forward,f);
   double expected=std::abs(a)==heading::pi?heading::pi:a;assert(std::abs(h.turnAngle-expected)<1e-5);
   // Ignore a conflicting target during the turn: continuity and frozen endpoint.
   auto mid=h.sample(d*-100.f,4,260,260);near(mid.basis.forward,rotated(f,n,expected/2));near(mid.basis.up,n);
   auto end=h.sample(d*-200.f,5,320,320);near(end.basis.forward,d);near(end.basis.up,n);assert(!h.turning);
   assert(diag::measure({end.basis.right,end.basis.up,end.basis.forward}).properRotation);
  }
 }
 puts("PASS: vertical/four diagonal/reversal world-space targets on three wall normals; shortest rotation, deterministic antipode, coherent endpoints and no mid-turn retarget");
 // Signed shortest turns across +/-pi seam, not a 340deg route.
 auto h=ready();Vec3 p{};unsigned u=2;double time=0;
 auto go=[&](double a){Vec3 d=rotated(entry.forward,entry.up,a);h.sample(p,u++,time,time);time+=100;p=p+d*.4f;h.sample(p,u++,time,time);time+=100;p=p+d*.4f;h.sample(p,u++,time,time);assert(h.turning);time+=120;auto out=h.sample(p,u++,time,time);near(out.basis.forward,d);time+=100;return out;};
 go(170*heading::pi/180);go(-170*heading::pi/180);assert(std::abs(h.turnAngle-20*heading::pi/180)<1e-5);
 // Repeated complete loops derive targets from measured direction, not accumulated matrices.
 for(unsigned loop=0;loop<30;++loop)for(double a:{0.,heading::pi/2,heading::pi,-heading::pi/2}){auto out=go(a);check(out);}
 puts("PASS: +/-pi seam takes20deg shortest path and120 repeated quarter turns retain fixed Up/proper basis without drift");
 // Dominant descent/ascent with alternating tiny lateral sign must not binary-flip.
 for(float vertical:{-1.f,1.f}){
  h=ready();p={};u=2;time=0;
  for(unsigned i=0;i<80;++i){time+=40;Vec3 d{(i%2?.02f:-.02f),vertical,0};p=p+d*.2f;auto out=h.sample(p,u++,time,time);check(out);}
  assert(!h.turning);assert(dot(h.forward,Vec3{0,vertical,0})>.999f);
  // Genuine full reversal must still be admitted.
  Vec3 opposite=h.forward*-1;time+=40;p=p+opposite*.2f;h.sample(p,u++,time,time);time+=40;p=p+opposite*.2f;h.sample(p,u++,time,time);assert(h.turning&&std::abs(h.turnAngle-heading::pi)<1e-5);
 }
 puts("PASS: descent/ascent with alternating lateral noise stays vertical; genuine full reversal still accepted");
 // Candidate directions cannot mix their evidence, even when every sample is fast.
 h=ready();p={};time=0;u=2;
 for(unsigned i=0;i<50;++i){time+=40;Vec3 d=rotated(entry.forward,entry.up,(i%2?40.:80.)*heading::pi/180);p=p+d*.2f;h.sample(p,u++,time,time);assert(h.pendingCount==1&&!h.turning);}
 h=ready();for(unsigned i=1;i<20;++i){auto out=h.sample({float(i)*.2f,float(i)*.01f,0},i+1,i*40,i*40);check(out);assert(!h.turning&&!h.pendingCount);}
 // Below-speed hold remains exactly2, including vertical descent (not patched faster).
 h=ready();for(unsigned i=1;i<20;++i){auto out=h.sample({0,-float(i)*.01f,0},i+1,i*40,i*40);check(out);assert(out.speed<heading::minimumTangentSpeed&&!h.turning&&!h.pendingCount);near(h.forward,entry.forward);}
 puts("PASS: incompatible candidates cannot accumulate; angular deadband and below2-speed descent hold last valid heading");
 // Every directional basis fits actual A and B math without Position/W/metadata changes.
 source_math::Matrix m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;m.v[12]=12;m.v[13]=34;m.v[14]=56;
 for(double a:{heading::pi/2,-heading::pi/2,heading::pi/4,-3*heading::pi/4}){
  h=ready();Vec3 d=rotated(entry.forward,entry.up,a);h.sample(d*.4f,2,100,100);h.sample(d*.8f,3,200,200);auto out=h.sample({},4,320,320);check(out);
  auto applied=m;Vec3 rows[]={out.basis.right,out.basis.up,out.basis.forward};for(unsigned i=0;i<3;++i){applied.v[4*i]=rows[i].x;applied.v[4*i+1]=rows[i].y;applied.v[4*i+2]=rows[i].z;}
  assert(source_math::valid(applied)&&!std::memcmp(applied.v+12,m.v+12,16));
  source_math::CompactBlock b{},replacement{};b.rows[1]=1;b.rows[5]=1.5;b.rows[6]=.75;b.metadata.fill(0x5a);Vec3 scales{};assert(source_math::compactReplacement(b,out.basis,replacement,scales));assert(b.metadata==replacement.metadata&&scales.x==1&&scales.y==1.5&&scales.z==.75);
 }
 puts("PASS: multidirectional outputs meet A/B proper-basis contract, Position/W and compact metadata/scales preserved");
}
int main(){
 // A: retained r2 same-clock regression timestamps; production threshold remains33ms.
 auto equal=ready();equal.sample({-.4f,0,0},2,100,100);
 auto savedPosition=equal.position;auto savedUpdate=equal.update;
 for(unsigned u=3;u<100;++u){auto r=equal.sample({-100,0,0},u,100,200);check(r);assert(dot(equal.pending,entry.forward)<-.99999f&&equal.pendingCount==1&&!equal.turning&&equal.pendingMs==100&&equal.observationMs==100&&equal.update==savedUpdate&&equal.position.x==savedPosition.x);}
 equal.sample({-.8f,0,0},100,200,200);assert(equal.turning);auto frozen=equal.sample({},101,250,250);
 for(unsigned u=102;u<130;++u){auto r=equal.sample({},u,250,1000);check(r);assert(equal.turning&&dot(r.basis.forward,frozen.basis.forward)>.99999f&&equal.lastClock==250);}
 auto gap=ready();gap.sample({-.4f,0,0},2,100,100);gap.sample({-1,0,0},3,100,100);gap.sample({-2,0,0},4,351,351);assert(!gap.pendingCount&&!gap.turning&&gap.observationMs==351);
 auto back=ready();back.sample({-.4f,0,0},2,100,100);back.sample({-.8f,0,0},3,99,101);assert(!back.pendingCount&&!back.turning&&back.observationMs==99);
 puts("PASS: A same-clock reproduction preserves candidate/baseline; duplicate time cannot vote, turn or extend freshness; backward/gap reset");
 // B: tuning-only threshold after A passed at original100ms.
 static_assert(heading::observationIntervalMs==33.&&heading::reversalTurnDurationMs==120.);
 auto tuned=ready();tuned.sample({-.132f,0,0},2,33,33);assert(tuned.pendingCount==1&&!tuned.turning);
 tuned.sample({-50,0,0},3,33,66);assert(tuned.pendingCount==1&&!tuned.turning&&tuned.observationMs==33&&tuned.update==2);
 tuned.sample({-.264f,0,0},4,66,66);assert(tuned.turning&&tuned.turnStart==66);
 auto tunedMid=tuned.sample({},5,126,126);check(tunedMid);assert(dot(tunedMid.basis.forward,Vec3{0,1,0})>.9999f);
 auto beforeEnd=tuned.sample({},6,185,185);check(beforeEnd);assert(tuned.turning);
 auto tunedEnd=tuned.sample({},7,186,186);check(tunedEnd);assert(tunedEnd.basis.forward.x==-tuned.f0.x&&!tuned.turning);
 auto regressed=ready();regressed.sample({-.132f,0,0},2,33,33);regressed.sample({-.264f,0,0},1,66,66);assert(!regressed.pendingCount&&!regressed.turning);
 auto seenRegression=ready();seenRegression.sample({-.132f,0,0},2,33,33);seenRegression.sample({-.2f,0,0},3,33,33);seenRegression.sample({-.3f,0,0},2,66,66);assert(!seenRegression.pendingCount&&!seenRegression.turning);
 puts("PASS: B two33ms observations accept at66ms; equal-time update cannot add vote; no early completion and exact120ms proper turn and native regression remain closed");
 source_math::Matrix m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;m.v[12]=12;m.v[13]=34;m.v[14]=56;source_math::Transition t;assert(t.enter(m,{0,0,1},0)); // parallel forward: entry heading falls back to the projected actor up
 m.v[0]=0;m.v[2]=-1;m.v[8]=1;m.v[10]=0;assert(t.enter(m,{0,0,1},0));source_math::Matrix a{};float progress{};assert(t.sample(m,{0,0,1},125,.25f,a,progress)&&progress==.5f);assert(!std::memcmp(a.v+12,m.v+12,16));
 puts("PASS: unchanged entry interpolation and Position/W preservation");
 auto h=ready();for(unsigned i=2;i<10;++i){auto r=h.sample({float(i)*.4f,0,0},i,(i-1)*100,(i-1)*100);check(r);assert(!h.turning&&dot(h.forward,entry.forward)>.99999f);}puts("PASS: straight displacement holds exact entry sign");
 h=ready();reverse(h);auto mid=h.sample({},4,260,260);check(mid);assert(dot(mid.basis.forward,Vec3{0,1,0})>.9999f);auto end=h.sample({},5,320,320);check(end);assert(dot(end.basis.forward,entry.forward)<-.99999f&&!h.turning);puts("PASS: confirmed positive signed 180-degree turn has finite midpoint, fixed Up and exact final sign");
 for(Vec3 p:{Vec3{-.01f,0,0},Vec3{0,0,1}}){h=ready();h.sample(p,2,100,100);h.sample(p*2,3,200,200);assert(!h.turning&&dot(h.forward,entry.forward)>.99999f);}puts("PASS: slow and normal-only motion hold");
 h=ready();h.sample({-.4f,0,0},2,100,100);h.sample({-.8f,0,0},2,200,200);assert(!h.turning);h.sample({0,1,0},3,200,200);assert(h.pendingCount==1&&!h.turning);puts("PASS: distinct native updates and consecutive eligible observations required; different target replaces pending without accepting");
 h=ready();h.sample({-.4f,0,0},2,100,100);h.sample({-2,0,0},3,500,500);assert(!h.pendingCount&&!h.turning);h.sample({-2.4f,0,0},4,600,600);assert(h.pendingCount==1&&!h.turning);puts("PASS: over-250ms gap rebases without carrying confirmation");
 h=ready();reverse(h);auto before=h.sample({},4,250,250);h.clearObservation();auto after=h.sample({50,0,0},5,1500,250);check(after);assert(dot(before.basis.forward,after.basis.forward)>.99999f&&h.turning);h.sample({},6,1700,320);assert(!h.turning&&!h.havePosition);puts("PASS: stale clear freezes pause-aware turn clock and completion rebases motion");
 h=ready();reverse(h);h.sample({},4,320,320);double now=420;Vec3 p{};unsigned update=5;
 for(unsigned i=0;i<12;++i){h.sample(p,update++,now,now);int next=dot(h.forward,entry.forward)>0?-1:1;p=p+entry.forward*(.4f*next);now+=100;h.sample(p,update++,now,now);p=p+entry.forward*(.4f*next);now+=100;h.sample(p,update++,now,now);assert(h.turning);now+=120;auto r=h.sample(p,update++,now,now);check(r);assert(!h.turning&&dot(r.basis.forward,entry.forward)*next>.99999f);now+=100;}
 puts("PASS: repeated opposite turns end at f0/-f0 without drift or mid-turn retarget");
 h=ready();h.sample({-.4f,0,0},2,100,100);h.clearObservation();h.sample({-10,0,0},3,1000,100);assert(!h.pendingCount);assert(h.enter(entry)&&dot(h.forward,entry.forward)>.99999f&&!h.havePosition&&!h.turning);h.sample({},1,0,0);h.sample({-.4f,0,0},2,100,100);h.sample({std::numeric_limits<float>::quiet_NaN(),0,0},3,200,200);assert(!h.pendingCount&&!h.havePosition);puts("PASS: pause invalid data and session/identity reset cannot bridge evidence");
 // Strict diagonal endpoint equality, not merely an angular tolerance.
 heading::Reversal diagonal;diag::ActorAxes diagonalEntry{{-.70710677f,.70710677f,0},{0,0,1},{.70710677f,.70710677f,0},true};assert(diagonal.enter(diagonalEntry));
 diagonal.sample({},1,0,0);diagonal.sample(diagonal.f0*-.4f,2,100,100);diagonal.sample(diagonal.f0*-.8f,3,200,200);
 auto exact=diagonal.sample({},4,320,320);assert(exact.basis.valid&&exact.basis.forward.x==-diagonal.f0.x&&exact.basis.forward.y==-diagonal.f0.y&&exact.basis.forward.z==-diagonal.f0.z);
 source_math::CompactBlock block{},replacement{};block.rows[1]=1;block.rows[5]=1.5;block.rows[6]=.75;block.metadata.fill(0x5a);Vec3 scales{};assert(source_math::compactReplacement(block,end.basis,replacement,scales));assert(block.metadata==replacement.metadata&&scales.x==1&&scales.y==1.5&&scales.z==.75);puts("PASS: resulting orientation fits existing A/B proper-basis scale/metadata contract");
 wallPlaneCases();
}
