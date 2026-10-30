#include "Math/reversal_heading.hpp"
#include "Math/compact_basis.hpp"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace ws;
static diag::ActorAxes entry{{0,1,0},{0,0,1},{1,0,0},true};
static void check(const heading::Result& r){assert(r.basis.valid&&diag::measure({r.basis.right,r.basis.up,r.basis.forward}).properRotation);assert(dot(r.basis.up,entry.up)>.99999f);}
static heading::Reversal ready(){heading::Reversal h;assert(h.enter(entry));h.sample({},1,0,0);return h;}
static void reverse(heading::Reversal& h){auto a=h.sample({-.4f,0,0},2,100,100);check(a);assert(a.candidate==-1&&!h.turning);auto b=h.sample({-.8f,0,0},3,200,200);check(b);assert(h.turning&&h.sign==-1);}
int main(){
 // A: retained r2 same-clock regression timestamps; production threshold remains33ms.
 auto equal=ready();equal.sample({-.4f,0,0},2,100,100);
 auto savedPosition=equal.position;auto savedUpdate=equal.update;
 for(unsigned u=3;u<100;++u){auto r=equal.sample({-100,0,0},u,100,200);check(r);assert(equal.pending==-1&&equal.pendingCount==1&&!equal.turning&&equal.pendingMs==100&&equal.observationMs==100&&equal.update==savedUpdate&&equal.position.x==savedPosition.x);}
 equal.sample({-.8f,0,0},100,200,200);assert(equal.turning);auto frozen=equal.sample({},101,250,250);
 for(unsigned u=102;u<130;++u){auto r=equal.sample({},u,250,1000);check(r);assert(equal.turning&&dot(r.basis.forward,frozen.basis.forward)>.99999f&&equal.lastClock==250);}
 auto gap=ready();gap.sample({-.4f,0,0},2,100,100);gap.sample({-1,0,0},3,100,100);gap.sample({-2,0,0},4,351,351);assert(!gap.pending&&!gap.turning&&gap.observationMs==351);
 auto back=ready();back.sample({-.4f,0,0},2,100,100);back.sample({-.8f,0,0},3,99,101);assert(!back.pending&&!back.turning&&back.observationMs==99);
 puts("PASS: A same-clock reproduction preserves candidate/baseline; duplicate time cannot vote, turn or extend freshness; backward/gap reset");
 // B: tuning-only threshold after A passed at original100ms.
 static_assert(heading::observationIntervalMs==33.&&heading::reversalTurnDurationMs==120.);
 auto tuned=ready();tuned.sample({-.132f,0,0},2,33,33);assert(tuned.pendingCount==1&&!tuned.turning);
 tuned.sample({-50,0,0},3,33,66);assert(tuned.pendingCount==1&&!tuned.turning&&tuned.observationMs==33&&tuned.update==2);
 tuned.sample({-.264f,0,0},4,66,66);assert(tuned.turning&&tuned.turnStart==66);
 auto tunedMid=tuned.sample({},5,126,126);check(tunedMid);assert(dot(tunedMid.basis.forward,Vec3{0,1,0})>.9999f);
 auto beforeEnd=tuned.sample({},6,185,185);check(beforeEnd);assert(tuned.turning);
 auto tunedEnd=tuned.sample({},7,186,186);check(tunedEnd);assert(tunedEnd.basis.forward.x==-tuned.f0.x&&!tuned.turning);
 auto regressed=ready();regressed.sample({-.132f,0,0},2,33,33);regressed.sample({-.264f,0,0},1,66,66);assert(!regressed.pending&&!regressed.turning);
 auto seenRegression=ready();seenRegression.sample({-.132f,0,0},2,33,33);seenRegression.sample({-.2f,0,0},3,33,33);seenRegression.sample({-.3f,0,0},2,66,66);assert(!seenRegression.pending&&!seenRegression.turning);
 puts("PASS: B two33ms observations accept at66ms; equal-time update cannot add vote; no early completion and exact120ms proper turn and native regression remain closed");
 source_math::Matrix m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;m.v[12]=12;m.v[13]=34;m.v[14]=56;source_math::Transition t;assert(t.enter(m,{0,0,1},0)==false); // parallel forward cannot define an entry heading
 m.v[0]=0;m.v[2]=-1;m.v[8]=1;m.v[10]=0;assert(t.enter(m,{0,0,1},0));source_math::Matrix a{};float progress{};assert(t.sample(m,{0,0,1},125,.25f,a,progress)&&progress==.5f);assert(!std::memcmp(a.v+12,m.v+12,16));
 puts("PASS: unchanged entry interpolation and Position/W preservation");
 auto h=ready();for(unsigned i=2;i<10;++i){auto r=h.sample({float(i)*.4f,0,0},i,(i-1)*100,(i-1)*100);check(r);assert(!h.turning&&h.sign==1);}puts("PASS: straight displacement holds exact entry sign");
 h=ready();reverse(h);auto mid=h.sample({},4,260,260);check(mid);assert(dot(mid.basis.forward,Vec3{0,1,0})>.9999f);auto end=h.sample({},5,320,320);check(end);assert(dot(end.basis.forward,entry.forward)<-.99999f&&!h.turning);puts("PASS: confirmed positive signed 180-degree turn has finite midpoint, fixed Up and exact final sign");
 for(Vec3 p:{Vec3{-.01f,0,0},Vec3{-.05f,1,0},Vec3{0,0,1}}){h=ready();h.sample(p,2,100,100);h.sample(p*2,3,200,200);assert(!h.turning&&h.sign==1);}puts("PASS: slow normal-only and oblique motion hold");
 h=ready();h.sample({-.4f,0,0},2,100,100);h.sample({-.8f,0,0},2,200,200);assert(!h.turning);h.sample({0,1,0},3,200,200);assert(!h.pending);puts("PASS: distinct native updates and consecutive eligible observations required; ambiguity clears pending");
 h=ready();h.sample({-.4f,0,0},2,100,100);h.sample({-2,0,0},3,500,500);assert(!h.pending&&!h.turning);h.sample({-2.4f,0,0},4,600,600);assert(h.pendingCount==1&&!h.turning);puts("PASS: over-250ms gap rebases without carrying confirmation");
 h=ready();reverse(h);auto before=h.sample({},4,250,250);h.clearObservation();auto after=h.sample({50,0,0},5,1500,250);check(after);assert(dot(before.basis.forward,after.basis.forward)>.99999f&&h.turning);h.sample({},6,1700,320);assert(!h.turning&&!h.havePosition);puts("PASS: stale clear freezes pause-aware turn clock and completion rebases motion");
 h=ready();reverse(h);h.sample({},4,320,320);double now=420;Vec3 p{};unsigned update=5;
 for(unsigned i=0;i<12;++i){h.sample(p,update++,now,now);int next=-h.sign;p=p+entry.forward*(.4f*next);now+=100;h.sample(p,update++,now,now);p=p+entry.forward*(.4f*next);now+=100;h.sample(p,update++,now,now);assert(h.turning);now+=120;auto r=h.sample(p,update++,now,now);check(r);assert(!h.turning&&dot(r.basis.forward,entry.forward)*next>.99999f);now+=100;}
 puts("PASS: repeated opposite turns end at f0/-f0 without drift or mid-turn retarget");
 h=ready();h.sample({-.4f,0,0},2,100,100);h.clearObservation();h.sample({-10,0,0},3,1000,100);assert(!h.pending);assert(h.enter(entry)&&h.sign==1&&!h.havePosition&&!h.turning);h.sample({},1,0,0);h.sample({-.4f,0,0},2,100,100);h.sample({std::numeric_limits<float>::quiet_NaN(),0,0},3,200,200);assert(!h.pending&&!h.havePosition);puts("PASS: pause invalid data and session/identity reset cannot bridge evidence");
 // Strict diagonal endpoint equality, not merely an angular tolerance.
 heading::Reversal diagonal;diag::ActorAxes diagonalEntry{{-.70710677f,.70710677f,0},{0,0,1},{.70710677f,.70710677f,0},true};assert(diagonal.enter(diagonalEntry));
 diagonal.sample({},1,0,0);diagonal.sample(diagonal.f0*-.4f,2,100,100);diagonal.sample(diagonal.f0*-.8f,3,200,200);
 auto exact=diagonal.sample({},4,320,320);assert(exact.basis.valid&&exact.basis.forward.x==-diagonal.f0.x&&exact.basis.forward.y==-diagonal.f0.y&&exact.basis.forward.z==-diagonal.f0.z);
 source_math::CompactBlock block{},replacement{};block.rows[1]=1;block.rows[5]=1.5;block.rows[6]=.75;block.metadata.fill(0x5a);Vec3 scales{};assert(source_math::compactReplacement(block,end.basis,replacement,scales));assert(block.metadata==replacement.metadata&&scales.x==1&&scales.y==1.5&&scales.z==.75);puts("PASS: resulting orientation fits existing A/B proper-basis scale/metadata contract");
}
