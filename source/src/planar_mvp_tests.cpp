#include "planar_mvp.hpp"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace ws;
static planar::Input input(){planar::Input i;i.session=1;i.hero=1;i.transform=2;i.owner=3;i.now=1;i.dt=.016;i.elapsed=.69;i.lifecycle=i.entrySeen=i.motionValid=true;i.normal={0,0,1};i.point={0,0,0};i.reference={0,0,.5f};i.position={0,0,.5f};i.velocity={0,8,0};i.actor={{1,0,0},{0,1,0},{0,0,1},true};return i;}
int main(){
 planar::Controller c;auto i=input();assert(!c.update(i).active);i.elapsed=.7;auto a=c.update(i);assert(a.active&&a.entered&&a.orientation&&!a.adhesion&&!a.movement);assert(diag::preciseDot(a.pose.up,i.actor.up)>.999);c.committed(a);
 for(unsigned k=0;k<18;++k){i.actor=a.pose;i.now+=.016;i.elapsed+=.016;a=c.update(i);assert(a.active&&diag::measure({a.pose.right,a.pose.up,a.pose.forward}).properRotation);assert(!a.adhesion&&!a.movement);c.committed(a);}
 assert(diag::preciseDot(a.pose.up,i.normal)>.999);i.normal={1,0,0};a=c.update(i);assert(a.exited);assert(!c.update(i).active);
 i=input();i.session=2;i.elapsed=.8;i.normal={-.7071068f,0,.7071068f};i.reference=i.normal*.5f;i.position=i.reference;a=c.update(i);assert(a.active);c.committed(a);for(unsigned k=0;k<18;++k){i.actor=a.pose;i.now+=.016;a=c.update(i);assert(a.active);c.committed(a);}assert(diag::preciseDot(a.pose.up,i.normal)>.999);
 i.kill=true;assert(c.update(i).exited);assert(!c.update(i).active);
 planar::Config cfg;cfg.adhesion=true;planar::Controller ad(cfg);i=input();i.elapsed=.8;i.position={500,700,.3f};a=ad.update(i);assert(a.active&&a.adhesion&&!a.movement&&std::abs(a.adhesionCorrection.z-.05f)<1e-5&&a.adhesionCorrection.x==0&&a.adhesionCorrection.y==0); // tangential distance is not contact rejection
 planar::Controller far(cfg);i.position.z=1.1f;assert(!far.update(i).active);
 cfg.movement=true;planar::Controller move(cfg);i=input();i.elapsed=.8;i.inputForward=1;a=move.update(i);assert(a.active&&a.movement&&dot(a.projectedMovement,i.normal)==0&&a.movementPosition.y>0&&std::abs(a.projectedMovement.y-5.6)<1e-5);
 for(Vec3 bad: {Vec3{},Vec3{0,1,0},Vec3{0,0,1.2f},Vec3{std::numeric_limits<float>::quiet_NaN(),0,1}}){planar::Controller x;i=input();i.elapsed=.8;i.normal=bad;assert(!x.update(i).active);}
 planar::Controller o;i=input();i.elapsed=.8;a=o.update(i);o.committed(a);i.survivalKnown=true;i.previousWriteSurvived=false;bool overwriteAbort=false;for(unsigned k=0;k<40;++k){i.now+=.016;auto b=o.update(i);if(b.exited){overwriteAbort=true;break;}o.committed(b);}assert(overwriteAbort);
 // Native resets inside Update are allowed when the preceding POST survived to PRE.
 planar::Controller post;i=input();i.elapsed=.8;a=post.update(i);post.committed(a);
 for(unsigned k=0;k<22;++k){i.now+=.016;i.actor=input().actor;i.survivalKnown=true;i.previousWriteSurvived=true;a=post.update(i);assert(a.active);post.committed(a);}
 assert(diag::preciseDot(a.pose.up,i.normal)>.999); // fixed entry basis, no per-frame restart
 planar::Controller identity;i=input();i.elapsed=.8;a=identity.update(i);identity.committed(a);++i.hero;assert(identity.update(i).exited);
 auto q=planar::slerp({0,0,0,1},{0,0,0,-1},.5f);assert(std::abs(q.w-1)<1e-6);
 puts("PASS: delayed entry, smooth proper quaternion A/E basis, phase1 no position requests, bounded normal-only adhesion/far tangent plane, surface input movement, bad fields/corner/identity/kill/native overwrite exits");
}
