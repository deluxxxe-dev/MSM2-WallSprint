#pragma once
#include "diagnostics.hpp"
namespace ws::faces {
inline constexpr Vec3 axes[4]={{0,0,1},{1,0,0},{0,0,-1},{-1,0,0}};
inline char group(Vec3 axis){int best=-1;double score=.9;for(unsigned i=0;i<4;++i){double d=diag::preciseDot(axis,axes[i]);if(d>score){best=int(i);score=d;}}return best<0?'?':char('A'+best);}
struct Face {unsigned samples{};double minAbs=1,sumAbs{};Vec3 sum{};bool allStable=true;unsigned positive{},negative{},ambiguous{};};
struct Result {bool complete{},betweenRotation{},strongAxis{},strongSigned{};double mean{},minimum{},ac{},bd{};const char* signedPattern="incomplete";};
class History {
 std::array<Face,4> faces{};
public:
 const auto& data()const{return faces;}
 void add(Vec3 crawl,Vec3 candidate,bool stable){char label=group(crawl);Vec3 v;if(label=='?'||!unit(candidate,v))return;auto i=unsigned(label-'A');auto& f=faces[i];double d=diag::preciseDot(v,axes[i]);++f.samples;f.sum=f.sum+v;f.minAbs=std::min(f.minAbs,std::abs(d));f.sumAbs+=std::abs(d);f.allStable=f.allStable&&stable;if(d>=.97)++f.positive;else if(d<=-.97)++f.negative;else ++f.ambiguous;}
 Result result()const {
  Result r;r.minimum=1;bool positive=true,negative=true,stable=true;Vec3 means[4];
  for(unsigned i=0;i<4;++i){const auto& f=faces[i];if(!f.samples||!unit(f.sum,means[i]))return r;r.mean+=f.minAbs/4;r.minimum=std::min(r.minimum,f.minAbs);stable=stable&&f.allStable;positive=positive&&f.positive==f.samples;negative=negative&&f.negative==f.samples;}
  r.complete=true;r.ac=diag::preciseDot(means[0],means[2]);r.bd=diag::preciseDot(means[1],means[3]);r.betweenRotation=std::abs(diag::preciseDot(means[0],means[1]))<.3&&std::abs(r.ac)>=.97&&std::abs(r.bd)>=.97;
  r.signedPattern=positive?"same_as_crawl_axes":negative?"opposite_to_crawl_axes":"mixed_or_mismatch";r.strongAxis=stable&&r.minimum>=.97&&r.mean>=.985&&r.betweenRotation;r.strongSigned=r.strongAxis&&(positive||negative)&&r.ac<=-.97&&r.bd<=-.97;return r;
 }
};
}
