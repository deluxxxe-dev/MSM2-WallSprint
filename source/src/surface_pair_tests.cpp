#include "static_field_observation.hpp"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <limits>
static std::string output;
static void log(const char* fmt,...){char b[4096];va_list args;va_start(args,fmt);vsnprintf(b,sizeof(b),fmt,args);va_end(args);output+=b;output+='\n';}
static bool close(double a,double b,double e=1e-4){return std::abs(a-b)<e;}
int main(){
 using ws::Vec3;Vec3 n{.6f,0,.8f},t{.8f,0,-.6f},p{10,20,30},near=p+n*.5f;auto hero=near+t*100;
 auto r=ws::pair::evaluate(near,p,n,hero,t*8,true);
 assert(r.valid&&r.pairSamplePass&&r.frameValid&&close(r.normalSeparation,.5)&&close(r.planeDistance,.5));
 assert(close(ws::dot(r.surface.up,r.surface.forward),0)&&close(ws::dot(r.surface.right,r.surface.forward),0)&&close(ws::dot(r.surface.right,r.surface.up),0));
 assert(close(ws::dot(r.surface.right,r.surface.right),1)&&close(ws::dot(r.surface.down,r.surface.up),-1));assert(ws::dot(r.correction,r.correction)<1e-8);
 // Plane reference can be far along the plane: no near-contact/lateral-position rule.
 r=ws::pair::evaluate(near,p,n,hero+n*.1f,t*8,true);assert(r.valid&&close(r.planeDistance,.6)&&close(ws::dot(r.correction,n),-.1));
 for(Vec3 invalid: {Vec3{},Vec3{0,2,0},Vec3{0,1,0},Vec3{std::numeric_limits<float>::quiet_NaN(),0,1}})assert(!ws::pair::evaluate(near,p,invalid,hero,t,true).valid);
 r=ws::pair::evaluate(p+n*.7f,p,n,hero,t,true);assert(r.valid&&!r.pairSamplePass&&!r.frameValid);
 r=ws::pair::evaluate(near+t*.2f,p,n,hero,t,true);assert(r.valid&&!r.pairSamplePass&&r.tangentError>.1);
 assert(!ws::pair::evaluate(near,p,n,hero,{},true).frameValid);assert(!ws::pair::evaluate(near,p,n,hero,n*10,true).frameValid);assert(!ws::pair::evaluate(near,p,n,hero,t,false).frameValid);
 // Batch isolation: a missing primary must not reuse a prior sample's vector.
 ws::fields::Observer o;o.begin(1,1,2,1,100);
 auto field=[&](unsigned off,Vec3 v,std::uint64_t ms){unsigned i=0;while(i<ws::fields::roots&&ws::fields::offset(i)!=off)++i;assert(i<ws::fields::roots);o.observe(i,&v,hero,ms,0,log);};
 field(0x320,near,100);field(0x338,p,100);field(0x398,n,100);o.endBatch(hero,t*8,true,100,log);assert(output.find("surface_pair_sample session=1 sample_ms=100 valid=1")!=std::string::npos);
 field(0x320,near,200);field(0x338,p,200);o.endBatch(hero,t*8,true,200,log);assert(output.find("sample_ms=200 valid=0 reason=incomplete_same_worker_batch")!=std::string::npos);
 o.reset();output.clear();o.endBatch(hero,t,true,300,log);assert(output.empty());
 // Replay primary fields from the actual v0.1a8 log, never execute native blobs.
 auto f=std::fopen("tools/fixtures/v01a8_surface_pair.tsv","rb");assert(f);unsigned session=0;unsigned long long ms=0;Vec3 pn,ps,axis,h,v;unsigned valid=0,count=0;double sumTangent=0,minSep=1,maxSep=0;unsigned frameCount=0;
 while(std::fscanf(f,"%u %llu %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %u",&session,&ms,&pn.x,&pn.y,&pn.z,&ps.x,&ps.y,&ps.z,&axis.x,&axis.y,&axis.z,&h.x,&h.y,&h.z,&v.x,&v.y,&v.z,&valid)==18){
  auto q=ws::pair::evaluate(pn,ps,axis,h,v,valid);assert(q.valid);assert(q.normalSeparation>=.4&&q.normalSeparation<=.6);++count;sumTangent+=q.tangentError;minSep=std::min(minSep,q.normalSeparation);maxSep=std::max(maxSep,q.normalSeparation);frameCount+=q.frameValid;
 }
 std::fclose(f);assert(count==431&&sumTangent/count<.01&&minSep<.44&&maxSep>.53&&frameCount>390);
 std::printf("PASS: Surface Pair/SurfaceFrame/adhesion local-only math, diagonal basis/negative controls/far-tangent plane/missing-batch reset; replayed%u real telemetry samples, %u valid dry-run frames, no game writes\n",count,frameCount);
}
