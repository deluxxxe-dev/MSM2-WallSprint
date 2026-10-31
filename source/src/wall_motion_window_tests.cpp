#include "Math/wall_motion_window.hpp"
#include <cassert>
#include <cstdio>
#include <string_view>
using namespace ws;
int main(){
 motion_diag::Window w;motion_diag::Frame f{};f.normal={1,0,0};f.event="low_speed_hold";
 assert(!w.sample(f,{}));
 for(unsigned i=1;i<=21;++i){f.ms=i*100;f.update=i;f.nativeMs=f.ms-1;bool ready=w.sample(f,{.1f*float(i),-.1f*float(i),.02f*float(i)});assert(ready==(i==21));}
 assert(w.count==16&&w.overwritten==5);assert(w.at(0).ms==600&&w.at(15).ms==2100);
 for(unsigned i=0;i<w.count;++i){auto& a=w.at(i);assert(a.tangent.x==0&&a.tangent.y<0&&a.tangent.z>0&&a.dt==100);assert(a.event==f.event&&a.nativeMs+1==a.ms);}
 std::puts("PASS: bounded ring retains coherent down/diagonal/low-speed values beyond eight holds with explicit overwrites");
 w.flushed();auto ms=w.lastMs;auto pos=w.position;assert(!w.sample(f,{}));assert(w.throttled==1&&w.lastMs==ms&&w.position.y==pos.y);
 f.ms=100;f.update=1;assert(!w.sample(f,{}));assert(w.lastMs==100&&w.position.y==0&&w.count==0);
 std::puts("PASS: duplicate/time regression cannot fabricate displacement and diagnostic baseline never affects heading");
 motion_diag::Window decisions;f={};f.normal={1,0,0};decisions.sample(f,{});
 f.ms=33;f.update=1;f.event="low_speed_hold";decisions.sample(f,{});
 f.ms=100;f.update=2;f.event="hold";decisions.sample(f,{});
 assert(decisions.count==1&&decisions.at(0).decisionMs==33&&decisions.at(0).decisionUpdate==1&&std::string_view(decisions.at(0).decision)=="low_speed_hold");
 f.ms=1;f.update=1;decisions.sample(f,{});assert(decisions.count==0&&decisions.overwritten==1);
 std::puts("PASS: sub-sample low-speed rejection survives throttling with separate decision timestamp/update, never mislabeled as current input");
 w.windows=w.maxWindows;assert(!w.sample(f,{}));assert(w.suppressed==1);w.finished=true;assert(!w.sample(f,{}));assert(w.suppressed==1);
 std::puts("PASS: session budget and final close bound total logging; suppression remains explicit");
 // Owner session4 values: normal X, full horizontal observations are NOT tiny noise.
 heading::Reversal h;diag::ActorAxes e{{0,-1,0},{1,0,0},{0,0,1},true};assert(h.enter(e));
 h.sample({},1,0,0);auto a=h.sample({0,0,-.161712646f},2,47,47);assert(a.speed>3&&a.candidate.z<-.99f);
 auto b=h.sample({0,0,-.323425292f},3,94,94);assert(b.event==std::string_view("accepted")&&b.target.z<-.99f);
 h.sample({0,0,-.323425292f},4,214,214);h.sample({},5,314,314);
 auto down=h.sample({0,-.204101562f,-.960220337f},6,361,361);assert(down.candidate.y<-.2f&&down.candidate.z<-.97f);
 std::puts("PASS: supplied session4 horizontal and down components replay as displacement targets, not inferred down-input");
}
