#include "Math/planar_standoff.hpp"
#include <cassert>
#include <cstdio>
#include <limits>
int main(){
 using namespace ws;using namespace ws::adhesion;
 for(Vec3 n:{Vec3{0,0,1},Vec3{-.70710678f,0,.70710678f}}){
  Vec3 point{12,-2,4},forward{},right{};assert(unit(cross(n,{0,1,0}),forward)&&unit(cross(n,forward),right));
  for(float distance:{.1f,.14f,.16f,.4f,.48f,.5f,.52f,.6f,.9f}){
   auto hero=point+n*distance;auto copy=hero;auto e=evaluate(hero,point,n,1);
   assert(e.valid&&std::abs(e.distance-distance)<2e-6f&&std::abs(e.requested)<=.02f&&hero.x==copy.x&&hero.y==copy.y&&hero.z==copy.z);
   assert(std::abs(dot(e.delta,right))<1e-6f&&std::abs(dot(e.delta,forward))<1e-6f);
   auto moved=evaluate(hero+forward*3.f+right*2.f,point,n,1);assert(moved.valid&&std::abs(moved.distance-e.distance)<2e-6f&&std::abs(moved.requested-e.requested)<2e-6f);
   if(!e.inRange)assert(e.requested==0);else assert(std::abs(e.error-e.requested)<=std::abs(e.error)+1e-6f);
  }
  auto transition=evaluate(point+n*.4f,point,n,.99f);assert(transition.valid&&!transition.transitionReady&&transition.requested==0);
  auto dead=evaluate(point+n*.5f,point,n,1);assert(dead.valid&&std::abs(dead.error)<1e-6f&&dead.requested==0);
 }
 auto nan=std::numeric_limits<float>::quiet_NaN();assert(!evaluate({0,0,.5f},{nan,0,0},{0,0,1},1).valid);
 assert(!evaluate({0,0,nan},{0,0,0},{0,0,1},1).valid&&!evaluate({0,0,.5f},{0,0,0},{0,0,0},1).valid);
 assert(!evaluate({0,0,.5f},{0,0,0},{0,1,0},1).valid&&!evaluate({0,0,.5f},{0,0,0},{0,0,2},1).valid);
 std::puts("PASS: read-only A/E planar standoff candidate control: target0.5/deadzone0.01/gain0.25/cap0.02/error0.35, no pre-transition request, normal-only delta and tangent preservation, invalid/nonfinite/steep/nonunit refusal; no Position mutation");
}
