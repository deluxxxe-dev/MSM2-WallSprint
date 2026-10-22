#pragma once
#include "diagnostics.hpp"
#include <array>
#include <cstring>
namespace ws::discovery {
constexpr unsigned bytes=0x200, offsets=(bytes-12)/4+1;
struct Candidate {
 unsigned samples{},up{},side{},positive{},negative{};
 Vec3 first{},last{};double minNorm=2,maxNorm{},minStability=1,absDotSum{},signedDotSum{};
};
struct PositionCandidate {
 unsigned samples{};Vec3 first{},last{},firstHero{},lastHero{},sumDelta{};
 // Symmetric E[delta delta^T] accumulator: xx, yy, zz, xy, xz, yz.
 double outer[6]{},minDistance=5,maxDistance{};
 void add(Vec3 point,Vec3 hero){
  auto d=hero-point;double distance=std::sqrt(diag::preciseDot(d,d));
  if(!finite(point)||!finite(hero)||!std::isfinite(distance)||distance>=5)return;
  if(!samples){first=point;firstHero=hero;}last=point;lastHero=hero;++samples;sumDelta=sumDelta+d;
  outer[0]+=double(d.x)*d.x;outer[1]+=double(d.y)*d.y;outer[2]+=double(d.z)*d.z;
  outer[3]+=double(d.x)*d.y;outer[4]+=double(d.x)*d.z;outer[5]+=double(d.y)*d.z;
  minDistance=std::min(minDistance,distance);maxDistance=std::max(maxDistance,distance);
 }
 double axialRms(Vec3 a)const {if(!samples)return 0;double q=double(a.x)*a.x*outer[0]+double(a.y)*a.y*outer[1]+double(a.z)*a.z*outer[2]+2.*(double(a.x)*a.y*outer[3]+double(a.x)*a.z*outer[4]+double(a.y)*a.z*outer[5]);return std::sqrt(std::max(0.,q/samples));}
 double lateralRms(Vec3 a)const {double q=axialRms(a);return samples?std::sqrt(std::max(0.,(outer[0]+outer[1]+outer[2])/samples-q*q)):0;}
};
class Candidates {
 std::array<Candidate,offsets> values{};unsigned observations{};
 std::array<PositionCandidate,offsets> positions{};
 std::array<unsigned char,bytes> firstRaw{},lastRaw{};
 std::uint64_t firstMs{},lastMs{};
public:
 unsigned count()const{return observations;}
 const auto& fields()const{return values;}
 const auto& points()const{return positions;}
 const auto& firstBlock()const{return firstRaw;}
 const auto& lastBlock()const{return lastRaw;}
 std::uint64_t firstTime()const{return firstMs;}
 std::uint64_t lastTime()const{return lastMs;}
 void observe(const unsigned char* raw,const diag::ActorAxes& axes,Vec3 velocity,bool motionValid,const Vec3* heroPosition=nullptr,std::uint64_t ms=0,unsigned excludedUnitOffset=~0u){
  if(!observations){std::memcpy(firstRaw.data(),raw,bytes);firstMs=ms;}
  std::memcpy(lastRaw.data(),raw,bytes);lastMs=ms;
  ++observations;
  auto type=observedSubtype(velocity,motionValid);
  for(unsigned i=0;i<offsets;++i){
   Vec3 v;std::memcpy(&v,raw+i*4,sizeof(v));if(heroPosition)positions[i].add(v,*heroPosition);
   if(i*4==excludedUnitOffset)continue;
   double norm=std::sqrt(diag::preciseDot(v,v));
   if(!finite(v)||std::abs(v.x)>1.25f||std::abs(v.y)>1.25f||std::abs(v.z)>1.25f||norm<.9||norm>1.1)continue;
   auto& c=values[i];Vec3 n=v*float(1./norm);
   if(!c.samples)c.first=n;
   c.last=n;++c.samples;c.minNorm=std::min(c.minNorm,norm);c.maxNorm=std::max(c.maxNorm,norm);
   c.minStability=std::min(c.minStability,diag::preciseDot(c.first,n));
   if(axes.valid&&type){Vec3 h=type==1?axes.forward*-1:axes.right;Vec3 unitH;
    if(unit(h,unitH)){double d=diag::preciseDot(n,unitH);c.absDotSum+=std::abs(d);c.signedDotSum+=d;if(d>=0)++c.positive;else ++c.negative;if(type==1)++c.up;else ++c.side;}
   }
  }
 }
private:
 static unsigned observedSubtype(Vec3 v,bool valid){
  if(!valid||!finite(v))return 0;
  double y=std::abs(v.y),h=std::hypot(v.x,v.z);if(std::max(y,h)<1)return 0;
  return y>2*h?1:h>2*y?2:0;
 }
};
}
