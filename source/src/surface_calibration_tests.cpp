#include "surface_calibration.hpp"
#include <cassert>
#include <cstdarg>
#include <string>
#include <iostream>
#include <memory>
static std::string output;
void logLine(const char* fmt,...){char b[4096]{};va_list a;va_start(a,fmt);vsnprintf(b,sizeof(b),fmt,a);va_end(a);output+=b;output+='\n';}
using ws::Vec3;
static void vectorAt(unsigned char* raw,unsigned offset,Vec3 v){std::memcpy(raw+offset,&v,sizeof(v));}
static ws::discovery::Candidates dataFor(Vec3 axis,double start){
 unsigned char raw[512]{};
 for(unsigned off:{0x144u,0x158u,0x16cu,0x184u,0x198u,0x1acu})vectorAt(raw,off,{1,0,0});
 vectorAt(raw,0x1c4,axis);ws::discovery::Candidates data;
 Vec3 point{100,10,200},hero=point+axis*.4f;vectorAt(raw,0x80,point);ws::diag::ActorAxes axes{{0,0,1},{0,1,0},{1,0,0},true};
 data.observe(raw,axes,{0,20,0},true,&hero,std::uint64_t(start));
 point.y+=1;hero.y+=1;vectorAt(raw,0x80,point);data.observe(raw,axes,{0,20,0},true,&hero,std::uint64_t(start+100));return data;
}
static void crawl(ws::calibration::Tracker& tracker,double deinit,Vec3 axis,std::uintptr_t hero=1,std::uintptr_t transform=2,std::uint64_t epoch=1){
 ws::diag::ActorAxes axes{{0,1,0},axis,{0,0,1},true};
 for(unsigned i=1;i<=3;++i){auto ms=std::uint64_t(deinit+100*i);tracker.tick(double(ms),ms,hero,transform,epoch,axes,false,logLine);}
}
int main(){
 auto tracker=std::make_unique<ws::calibration::Tracker>();auto data=dataFor({1,0,0},1000);auto layout=ws::discovery::classify(data);
 assert(layout.cluster[0x13c/4]==layout.cluster[0x144/4]);assert(layout.overlap[0x144/4]);
 assert(layout.basisSuspect[0x144/4]&&layout.basisSuspect[0x158/4]);assert(layout.repeatRepresentative[0x158/4]==0x144/4);
 assert(layout.basisSuspect[0x1c4/4]); // exact +X also suspected; other walls must permit re-evaluation.
 const auto& point=data.points()[0x80/4];assert(point.samples==2&&std::abs(point.axialRms({1,0,0})-.4)<1e-4&&point.lateralRms({1,0,0})<1e-4);
 unsigned char matrixRaw[512]{};float identity[9]={1,0,0,0,1,0,0,0,1};std::memcpy(matrixRaw+0x40,identity,sizeof(identity));
 ws::discovery::Candidates matrix;matrix.observe(matrixRaw,{}, {},false);matrix.observe(matrixRaw,{}, {},false);auto ml=ws::discovery::classify(matrix);assert(ml.matrixBoundary[0x40/4]);assert(ml.kind[0x40/4]==ws::discovery::LayoutKind::ConstantBasis&&ml.basisSuspect[0x40/4]);
 ws::calibration::History history;history.add({{1,0,0},{1,0,0},true,false,1,1});history.add({{-1,0,0},{-1,0,0},true,false,2,1});assert(history.nonparallel(true)==1);
 history.add({{0,0,1},{0,0,1},true,false,3,1});assert(history.nonparallel(true)==2);
 history.add({{.7071068f,0,.7071068f},{.7071068f,0,.7071068f},true,false,4,1});assert(history.nonparallel(true)==3);
 // Three orientations, independent sessions; a constant +X cannot gain rank merely from one wall.
 Vec3 axes[3]={{1,0,0},{0,0,1},{.7071068f,0,.7071068f}};
 for(unsigned i=0;i<3;++i){double start=10000+i*10000;auto fields=dataFor(axes[i],start);
  tracker->begin(i+1,1,2,1,logLine);tracker->summarize(fields,i+1,"verified_deinit",logLine);tracker->finish(fields,i+1,start+500,logLine);crawl(*tracker,start+500,axes[i]);
  assert(tracker->context().axisAvailable&&!tracker->context().valid&&tracker->context().pointAvailable);
 }
 assert(output.find("surface_calibration_pair wallrun_session=3")!=std::string::npos);
 assert(output.find("lateral_error_rms=0.000000")!=std::string::npos);
 assert(output.find("qualified_nonparallel_walls_capped3=3")!=std::string::npos);
 assert(output.find("v01b_gate=BLOCKED")!=std::string::npos);
 for(float sign:{1.f,-1.f}){
  tracker->reset("four-face-regroup",logLine);
  for(unsigned i=0;i<4;++i){double start=50000+i*10000;auto axis=ws::faces::axes[i];auto fields=dataFor(axis*sign,start);tracker->begin(i+100,1,2,1,logLine);tracker->summarize(fields,i+100,"verified_deinit",logLine);tracker->finish(fields,i+100,start+500,logLine);crawl(*tracker,start+500,axis);}
  assert(output.find("object_repeated_basis_group")!=std::string::npos);
  auto scoreLine=output.rfind("surface_source_four_face_score session=103 child_offset=0x1c4");assert(scoreLine!=std::string::npos);assert(output.substr(scoreLine,output.find('\n',scoreLine)-scoreLine).find("strong_axis_candidate=1")!=std::string::npos);
 }
 // Same cached sample cannot count three times. New state, identity, expiry or invalid basis cannot pair.
 tracker->reset("test",logLine);tracker->begin(10,1,2,1,logLine);tracker->finish(data,10,2000,logLine);
 ws::diag::ActorAxes tilted{{0,1,0},{1,0,0},{0,0,1},true};
 for(unsigned i=0;i<5;++i)tracker->tick(2100,2100,1,2,1,tilted,false,logLine);
 assert(!tracker->context().axisAvailable);tracker->tick(4600,4600,1,2,1,tilted,false,logLine);assert(!tracker->context().axisAvailable);
 for(unsigned mismatch=0;mismatch<4;++mismatch){tracker->reset("test",logLine);tracker->begin(20,1,2,1,logLine);tracker->finish(data,20,2000,logLine);
  auto testAxes=tilted;if(mismatch==3)testAxes.valid=false;
  for(unsigned i=1;i<=3;++i)tracker->tick(2000+100*i,2000+100*i,mismatch==0?3:1,mismatch==1?4:2,1,testAxes,mismatch==2,logLine);
  assert(!tracker->context().axisAvailable);
 }
 tracker->reset("test",logLine);tracker->begin(30,1,2,1,logLine);tracker->finish(data,30,2000,logLine);crawl(*tracker,2000,{-1,0,0});assert(tracker->context().axis.x==-1&&!tracker->context().valid);
 assert(output.find("signed_dot=-1.000000")!=std::string::npos);
 tracker->reset("standalone",logLine);
 tracker->observeActor(10000,10000,1,2,1,tilted,{5,6,7},{},true,false,logLine);
 assert(output.find("pairing=not_asserted")!=std::string::npos&&!tracker->context().axisAvailable);
 std::cout<<"PASS: overlapping clusters/basis suppression/matrix hypotheses, paired point covariance, 3 nonparallel axes vs antiparallel controls, crawl window/identity/state/freshness/validity/sign, surface gate stays blocked\n";
}
