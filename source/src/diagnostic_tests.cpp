#include "diagnostics.hpp"
#include <cassert>
#include <iostream>
#include <limits>
static bool close(double a,double b){return std::abs(a-b)<1e-5;}
int main(){
 float identity[]={1,0,0,0,0,1,0,0,0,0,1,0};
 auto a=ws::diag::analyze(identity);assert(a.rowMetrics.properRotation&&a.columnMetrics.properRotation);assert(close(a.rowMetrics.determinant,1));assert(close(a.rowMetrics.orthogonalityError,0));
 // Nonsymmetric rotation: both layouts orthogonal, different directional hypotheses.
 float rotated[]={0,-1,0,0,1,0,0,0,0,0,1,0};a=ws::diag::analyze(rotated);assert(a.rows[0].y==-1&&a.columns[0].y==1);assert(a.rowMetrics.properRotation&&a.columnMetrics.properRotation);
 float reflected[]={-1,0,0,0,0,1,0,0,0,0,1,0};a=ws::diag::analyze(reflected);assert(a.rowMetrics.orthonormal&&!a.rowMetrics.properRotation&&close(a.rowMetrics.determinant,-1));
 float shear[]={1,.5f,0,0,0,1,0,0,0,0,1,0};a=ws::diag::analyze(shear);assert(!a.rowMetrics.orthonormal&&a.rowMetrics.orthogonalityError>.5);assert(close(a.rowMetrics.pairDot[0],.5));
 float zero[12]{};assert(!ws::diag::analyze(zero).rowMetrics.orthonormal);identity[0]=std::numeric_limits<float>::quiet_NaN();assert(!ws::diag::analyze(identity).rowMetrics.finite);
 ws::diag::MarkerKeys keys;
 for(unsigned i=0;i<6;++i){unsigned bit=1u<<i;assert(keys.update(true,bit)==bit);assert(keys.update(true,bit)==0);assert(keys.update(true,0)==0);assert(keys.update(true,bit)==bit);assert(keys.update(true,0)==0);}
 assert(keys.update(false,63)==0);assert(keys.update(true,63)==63);assert(keys.update(true,63)==0);assert(keys.update(true,64)==0);

 ws::diag::History history;auto m=history.update(true,1,2,{0,0,0},100);assert(m.changed&&!m.valid);m=history.update(true,1,2,{1,0,0},200);assert(!m.changed&&m.valid&&close(m.velocity.x,10));
 m=history.update(true,3,4,{2,0,0},300);assert(m.changed&&!m.valid);m=history.update(true,3,5,{2,0,0},400);assert(m.changed&&!m.valid);
 m=history.update(false,0,0,{},500);assert(m.changed&&!m.valid);auto epoch=m.epoch;m=history.update(false,0,0,{},600);assert(!m.changed&&m.epoch==epoch);
 m=history.update(true,3,5,{2,0,0},700);assert(m.changed&&!m.valid);m=history.update(true,3,5,{10000,0,0},800);assert(!m.valid);m=history.update(true,3,5,{10000,0,0},1500);assert(!m.valid);
 std::cout<<"PASS: basis metrics/layout/reflection/shear/NaN, six number-only marker press/hold/release/focus cases, identity resets and velocity gaps\n";
}
