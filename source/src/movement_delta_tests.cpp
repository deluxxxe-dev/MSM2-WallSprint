#include "Math/movement_delta.hpp"
#include <cassert>
#include <cstdio>
#include <limits>
int main(){
 for(auto n:{ws::Vec3{0,0,1},ws::Vec3{-.70710678f,0,.70710678f}}){ws::Vec3 right{},forward{};assert(ws::unit(ws::cross(n,{0,1,0}),right)&&ws::unit(ws::cross(right,n),forward));ws::diag::ActorAxes frame{right,n,forward,true};auto delta=n*2+forward*3+right*4;auto a=ws::movement::decompose({11,13,17},ws::Vec3{11,13,17}+delta,frame,100);auto b=ws::movement::decompose({},delta,frame,200);assert(a.valid&&b.valid&&std::abs(a.normal-2)<.0001f&&std::abs(a.forward-3)<.0001f&&std::abs(a.right-4)<.0001f&&std::abs(a.speed-2*b.speed)<.001f);assert(!ws::movement::decompose({},delta,frame,0).valid&&!ws::movement::decompose({},delta,frame,501).valid);frame.right={};assert(!ws::movement::decompose({},delta,frame,10).valid);}
 ws::diag::ActorAxes identity{{1,0,0},{0,1,0},{0,0,1},true};assert(!ws::movement::decompose({std::numeric_limits<float>::infinity(),0,0},{},identity,100).valid);
 puts("PASS: c11 cardinal/diagonal SurfaceFrame displacement projection, time-scaled velocity and world-translation invariance; nonfinite/bad basis/invalid time refuse diagnostic");
}
