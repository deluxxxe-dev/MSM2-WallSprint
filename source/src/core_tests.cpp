#include "wallsprint.hpp"
#include <cassert>
#include <iostream>
#include <limits>
struct Mock:ws::Host {int probes{},writes{},releases{},logs{};bool lost{},reject{};ws::Vec3 n{1,0,0};ws::Debug last{};
 ws::Hit probe(ws::Vec3,ws::Vec3 d,float)override{++probes;assert(d.x<-.99f);return{!lost,{0,0,0},n};}
 bool apply(const ws::SurfaceFrame& f,ws::Quaternion q,ws::Vec3 v,ws::Vec3 p)override{++writes;assert(f.valid);assert(std::abs(ws::dot(v,f.up))<1e-4);assert(std::abs(p.x-.4f)<1e-4);assert(std::abs(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w-1)<1e-4);return !reject;}
 void releaseToVanillaAir(ws::Exit)noexcept override{++releases;}void log(const ws::Debug& d)override{++logs;last=d;}};
int main(){
 auto f=ws::frame({}, {1,0,0},{0,0,5});assert(f.valid&&std::abs(ws::dot(f.up,f.forward))<1e-6);assert(!ws::frame({}, {},{1,0,0}).valid);assert(!ws::frame({}, {1,0,0},{1,0,0}).valid);
 ws::Sample s;s.dt=.05f;s.wallRun=true;s.contact=true;s.position={.4f,0,0};s.velocity={0,0,8};s.normal={1,0,0};s.inputForward=1;
 Mock m;ws::Controller c;for(int i=0;i<13;++i)c.tick(s,m);assert(m.writes==0);for(int i=0;i<2;++i)c.tick(s,m);assert(c.current()==ws::State::WallSprint&&m.writes>0);
 for(auto input: {ws::Vec3{1,0,0},ws::Vec3{-1,0,0},ws::Vec3{0,1,0},ws::Vec3{0,-1,0}}){s.inputRight=input.x;s.inputForward=input.y;c.tick(s,m);}m.lost=true;c.tick(s,m);assert(m.releases==1&&m.last.reason==ws::Exit::LostSurface);s.wallRun=false;c.tick(s,m);assert(m.releases==1);
 for(int scenario=0;scenario<9;++scenario){Mock h;ws::Controller k;auto a=s;a.wallRun=true;for(int i=0;i<15;++i)k.tick(a,h);assert(k.current()==ws::State::WallSprint);
 switch(scenario){case 0:a.cutscene=true;break;case 1:a.dead=true;break;case 2:a.fastTravel=true;break;case 3:a.forced=true;break;case 4:a.supported=false;break;case 5:a.velocity.x=std::numeric_limits<float>::quiet_NaN();break;case 6:h.n={0,0,1};break;case 7:h.reject=true;break;case 8:a.wallRun=false;break;}k.tick(a,h);assert(k.current()==ws::State::Vanilla&&h.releases==1);}
 Mock ground;ws::Controller g;s.wallRun=false;for(int i=0;i<30;++i)g.tick(s,ground);assert(ground.probes==0&&ground.writes==0&&ground.releases==0);
 std::cout<<"PASS: basis/quaternion, delayed entry, four inputs, surface loss, nine unsafe exits, vanilla ground isolation\n";
}
