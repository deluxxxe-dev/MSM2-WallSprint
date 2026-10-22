#include "static_field_observation.hpp"
#include <cassert>
#include <cstdarg>
#include <string>
#include <cstdio>
static std::string out;
static void log(const char* fmt,...){char b[4096];va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);out+=b;out+='\n';}
static ws::diag::ActorAxes axes(ws::Vec3 up){ws::diag::ActorAxes a{};a.valid=true;a.up=up;return a;}
int main(){
 ws::fields::Field f;f.add({0,0,1},{10,20,30},100);f.add({0,0,1},{10,20,30},200);assert(f.stable());++f.attempts;assert(!f.stable());
 ws::fields::Observer o;o.begin(1,1,2,3,100);ws::Vec3 normal{0,0,1},point{10,20,29.5f};
 o.observe(0,&normal,{10,20,30},100,0,log);o.observe(1,&point,{10,20,30},100,0,log);
 o.observe(0,&normal,{10,20,30},200,0,log);o.observe(1,&point,{10,20,30},200,0,log);o.finish(1,300,log);
 o.tick(400,400,1,2,3,axes(normal),false,log);o.tick(400,400,1,2,3,axes(normal),false,log);assert(out.find("static_field_calibration_pair")==std::string::npos);
 o.tick(500,500,1,2,3,axes(normal),false,log);o.tick(600,600,1,2,3,axes(normal),false,log);
 assert(out.find("static_field_axis_score session=1")!=std::string::npos);assert(out.find("axis_distance=0.500000 lateral_error=0.000000")!=std::string::npos);
 out.clear();o.begin(2,1,2,3,1000);o.observe(0,&normal,{10,20,30},1000,0,log);o.finish(2,1013,log);assert(out.find("too_short_transition_or_failed_attach")!=std::string::npos);
 out.clear();o.begin(3,1,2,3,2000);o.observe(0,&normal,{10,20,30},2000,0,log);o.observe(0,&normal,{10,20,30},2100,0,log);o.finish(3,2200,log);for(unsigned ms=2300;ms<=2600;ms+=100)o.tick(ms,ms,9,2,3,axes(normal),false,log);assert(out.find("static_field_calibration_pair")==std::string::npos);
 out.clear();o.begin(4,1,2,3,3000);o.observe(ws::fields::roots,&normal,{10,20,30},3000,0x10000,log);o.observe(ws::fields::roots,&normal,{10,20,30},3100,0x20000,log);o.finish(4,3200,log);for(unsigned ms=3300;ms<=3500;ms+=100)o.tick(ms,ms,1,2,3,axes(normal),false,log);assert(out.find("child_pointer_changed=1")!=std::string::npos);assert(out.find("static_field_axis_score")==std::string::npos);
 puts("PASS: targeted fields, missing-sample stability, cached contact metrics, 3 fresh crawl samples, short-session/identity/child-change exclusion; no actor writes");
}
