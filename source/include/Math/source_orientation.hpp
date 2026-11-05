#pragma once
#include "diagnostics.hpp"
#include <cstring>
namespace ws::source_math {
struct alignas(16) Matrix {float v[16]{};};
static_assert(sizeof(Matrix)==64);
inline bool valid(const Matrix& m){
 for(float x:m.v)if(!std::isfinite(x))return false;
 if(!diag::actorAxes(m.v).valid)return false;
 for(unsigned i: {3u,7u,11u})if(std::abs(m.v[i])>1e-5f)return false;
 for(unsigned i: {12u,13u,14u})if(std::abs(m.v[i])>=1e6f)return false;
 return std::abs(m.v[15]-1.f)<1e-5f;
}
inline Quaternion normalize(Quaternion q){float n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);return{q.x/n,q.y/n,q.z/n,q.w/n};}
inline Quaternion quaternion(diag::ActorAxes a){SurfaceFrame f{};f.right=a.right;f.up=a.up;f.forward=a.forward;return normalize(rotation(f));}
inline diag::ActorAxes axes(Quaternion q){q=normalize(q);auto x=q.x,y=q.y,z=q.z,w=q.w;return{{1-2*(y*y+z*z),2*(x*y+w*z),2*(x*z-w*y)},{2*(x*y-w*z),1-2*(x*x+z*z),2*(y*z+w*x)},{2*(x*z+w*y),2*(y*z-w*x),1-2*(x*x+y*y)},true};}
inline Quaternion slerp(Quaternion a,Quaternion b,float t){a=normalize(a);b=normalize(b);float d=a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;if(d<0){d=-d;b={-b.x,-b.y,-b.z,-b.w};}d=std::clamp(d,0.f,1.f);float x=1-t,y=t;if(d<.9995f){float theta=std::acos(d),s=std::sin(theta);x=std::sin((1-t)*theta)/s;y=std::sin(t*theta)/s;}return normalize({a.x*x+b.x*y,a.y*x+b.y*y,a.z*x+b.z*y,a.w*x+b.w*y});}
struct Transition {
 bool active{};double entryMs{};Vec3 surface{};diag::ActorAxes target{};Quaternion entryQ{},targetQ{};
 bool enter(const Matrix& original,Vec3 rawNormal,double now){
  active=false;if(!valid(original)||!finite(rawNormal)||!std::isfinite(now))return false;
  float len=std::sqrt(dot(rawNormal,rawNormal));if(len<.95f||len>1.05f)return false;surface=rawNormal*(1/len);if(std::abs(surface.y)>.2f)return false;
  auto basis=diag::actorAxes(original.v);Vec3 f,r;
  // The actor forward can point straight into the wall (fast swing head-on into a wall), leaving no
  // in-plane remainder. Fall back to the actor up, then right, projected onto the wall.
  auto planar=[&](Vec3 v,Vec3& out){Vec3 p=v-surface*dot(v,surface);return dot(p,p)>.04f&&unit(p,out);};
  if(!planar(basis.forward,f)&&!planar(basis.up,f)&&!planar(basis.right,f))return false;
  if(!unit(cross(surface,f),r)||!unit(cross(r,surface),f))return false;
  target={r,surface,f,true};if(!diag::measure({r,surface,f}).properRotation)return false;
  entryQ=quaternion(basis);targetQ=quaternion(target);entryMs=now;active=true;return true;
 }
 bool sample(const Matrix& incoming,Vec3 rawNormal,double now,float rotateSeconds,Matrix& applied,float& t){
  if(!active||!valid(incoming)||!finite(rawNormal)||!std::isfinite(now)||now<entryMs||!std::isfinite(rotateSeconds)||rotateSeconds<.2f||rotateSeconds>1)return false;
  float n=std::sqrt(dot(rawNormal,rawNormal));if(n<.95f||n>1.05f)return false;auto up=rawNormal*(1/n);
  if(std::abs(up.y)>.2f||dot(up,surface)<.995f)return false; // No corner steering.
  t=std::clamp(float((now-entryMs)/(rotateSeconds*1000.)),0.f,1.f);auto a=axes(slerp(entryQ,targetQ,t));if(!diag::measure({a.right,a.up,a.forward}).properRotation)return false;
  applied=incoming;Vec3 rows[]={a.right,a.up,a.forward};for(unsigned i=0;i<3;++i){applied.v[i*4]=rows[i].x;applied.v[i*4+1]=rows[i].y;applied.v[i*4+2]=rows[i].z;applied.v[i*4+3]=0;}
  return valid(applied)&&!std::memcmp(applied.v+12,incoming.v+12,16);
 }
};
}
