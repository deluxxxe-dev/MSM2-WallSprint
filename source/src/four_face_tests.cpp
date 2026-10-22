#include "four_face.hpp"
#include <cassert>
#include <iostream>
int main(){
 for(float sign:{1.f,-1.f}){ws::faces::History h;for(unsigned i=0;i<4;++i)h.add(ws::faces::axes[i],ws::faces::axes[i]*sign,true);auto r=h.result();assert(r.complete&&r.strongAxis&&r.strongSigned&&r.minimum>.999&&r.ac<-.999&&r.bd<-.999);}
 ws::faces::History constant;for(auto axis:ws::faces::axes)constant.add(axis,{1,0,0},true);assert(!constant.result().strongAxis&&constant.result().minimum==0);
 ws::faces::History bad;for(unsigned i=0;i<4;++i)bad.add(ws::faces::axes[i],i==2?ws::Vec3{1,0,0}:ws::faces::axes[i],true);assert(!bad.result().strongAxis);
 ws::faces::History mixed;for(unsigned i=0;i<4;++i)mixed.add(ws::faces::axes[i],ws::faces::axes[i]*(i==2?-1.f:1.f),true);assert(mixed.result().strongAxis&&!mixed.result().strongSigned);
 ws::faces::History missing;for(unsigned i=0;i<3;++i)missing.add(ws::faces::axes[i],ws::faces::axes[i],true);assert(!missing.result().complete&&!missing.result().strongAxis);
 ws::faces::History unstable;for(unsigned i=0;i<4;++i)unstable.add(ws::faces::axes[i],ws::faces::axes[i],i!=2);assert(!unstable.result().strongAxis);
 assert(ws::faces::group({0,1,0})=='?');
 std::cout<<"PASS: four-face min/mean/opposites, global sign inversion, constant/missing/bad/unstable face rejection, mixed-sign axis vs signed distinction\n";
}
