"""Compile actual M12 walking-fit and preserved M11 budget helpers with owned data."""
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
code = (root / 'src/NativeHooks/camera_ref.cpp').read_text()
model = code.split('// M12_WALK_MODEL_BEGIN:', 1)[1].split('\n', 1)[1].split('// M12_WALK_MODEL_END', 1)[0]
budget = code.split('// M13_FALL_GUARD_BEGIN:', 1)[1].split('\n', 1)[1].split('// M13_FALL_GUARD_END', 1)[0]
tests = r'''
void train(FaceWalkModel& m,double time,double angle,int hand,double theta,float speed=4.f) {
 float n[3]={1,0,0}, step[3]={0,float(std::cos(hand*angle+theta)*speed*0.02),float(std::sin(hand*angle+theta)*speed*0.02)};
 assert(m.observe(time,float(std::sin(angle)),float(std::cos(angle)),step,0.02,n));
}
void checkDirection(const FaceWalkModel& m,double now,double angle,double expected) {
 float n[3]={1,0,0},out[3]; assert(m.direction(now,float(std::sin(angle)),float(std::cos(angle)),n,out));
 assert(near(out[0],0) && near(out[1],float(std::cos(expected))) && near(out[2],float(std::sin(expected))));
}
int main() {
 float n[3]={1,0,0}, up[3],side[3],out[3]; assert(faceBasis(n,up,side));
 assert(near(up[1],1) && near(side[2],1));
 float tilted[3]={0.8f,0.1f,float(std::sqrt(0.35))};assert(faceBasis(tilted,up,side));
 assert(near(up[0]*side[0]+up[1]*side[1]+up[2]*side[2],0));
 FaceWalkModel empty; assert(!empty.direction(10,0,1,n,out));assert(std::string(empty.takeoff(10,0,1,n,out))=="none");
 float slow[3]={0,0.02f,0};assert(!empty.observe(10,0,0.5f,slow,0.02,n));
 assert(!empty.observe(10,0,1,slow,0.02,n)); // speed = 1 is insufficient
 slow[1]=NAN;assert(!empty.observe(10,0,1,slow,0.02,n));slow[1]=0.08f;
 assert(!empty.observe(10,0,1,slow,0.001,n));assert(!empty.observe(10,0,1,slow,0.2,n));
 puts("PASS: M12 face basis, untrained/slow/weak stick/nonfinite/gap refusal");
 for(int hand : {1,-1}) {
  FaceWalkModel m;
  for(int i=0;i<48;++i) train(m,10+i*0.02,(i%4)*kFacePi/2,hand,0.7,3.7f);
  assert(m.hand==hand && m.count(10.94)==48);
  for(int i=0;i<4;++i) checkDirection(m,11.0,i*kFacePi/2,hand*i*kFacePi/2+0.7);
  assert(std::string(m.takeoff(11.0,0,1,n,out))=="walk");
  assert(near(std::hypot(out[1],out[2]),5.f)); // pushed directional jump floor
  assert(std::string(m.takeoff(11.3,0,1,n,out))=="model");assert(near(std::hypot(out[1],out[2]),5.f));
  assert(std::string(m.takeoff(31.0,0,1,n,out))=="none");assert(m.count(31.0)==0);
 }
 puts("PASS: M12 both mirrors/cardinal directions, measured walking takeoff,5m/s model and20s expiry");
 FaceWalkModel narrow;
 for(int i=0;i<8;++i) train(narrow,10+i*0.02,i*0.05,-1,0.5);
 assert(narrow.hand==1); // <60deg stick span cannot choose the other mirror
 FaceWalkModel oldMirror;oldMirror.hand=-1;train(oldMirror,10,0.3,-1,0.4);assert(oldMirror.hand==-1);
 FaceWalkModel threshold;train(threshold,10,0,-1,0.5);train(threshold,10.02,kFacePi/3,-1,0.5);assert(threshold.hand==-1);
 FaceWalkModel wrap;train(wrap,10,0,1,179*kFacePi/180);train(wrap,10.02,0,1,-179*kFacePi/180);
 assert(std::fabs(std::fabs(wrap.theta)-kFacePi)<1e-5);
 FaceWalkModel turning;
 for(int i=0;i<4;++i) train(turning,10+i*0.02,i*kFacePi/2,1,0.1);
 train(turning,13,0,1,1.0); assert(std::fabs(turning.theta-1.0)<1e-5); // theta uses only last2s
 checkDirection(turning,16,0,1.0); // no new walking => no camera-based update while still/airborne
 for(int i=0;i<100;++i)train(turning,14+i*0.02,(i%4)*kFacePi/2,1,1.0);
 assert(turning.used==64 && turning.count(16)==64);
 puts("PASS: M12 60deg mirror gate, previous mirror, angle wrap,2s rotation window,64-ring and stationary freeze");
 FaceWalkFrame frame;FaceWalkModel frameModel;
 float pos[3]={0,0,0};frame.advance(10,pos,true,0,1,n,frameModel);
 float desired[3]={0,0.02f,0};frame.applied(desired,desired,true);
 float laterDesired[3]={0,0.04f,0},laterCorrected[3]={0,0.07f,0};frame.applied(laterDesired,laterCorrected,true);
 pos[1]=0.09f;frame.advance(10.02,pos,true,1,0,n,frameModel);
 assert(frameModel.used==1 && near(frameModel.velocity[1],3.f)); // all calls once; subtract +.03 own store
 assert(std::fabs(frameModel.ring[0].stick)<1e-6); // stick belongs to measured interval, not next frame
 frame.applied(desired,laterCorrected,false);pos[1]+=0.08f;
 frame.advance(10.04,pos,true,0,1,n,frameModel);assert(frameModel.used==1); // faceAir/unpinned interval excluded
 pos[1]+=0.08f;frame.advance(10.06,pos,true,0,1,n,frameModel);assert(frameModel.used==2);
 puts("PASS: M12 per-frame game walking includes later calls, excludes own correction/air interval and pairs stick");
 bool fallback=false; float velocity[3]={5,0,3};
 assert(faceZipDirection(turning,16,0,1,n,velocity,out,fallback) && !fallback);
 assert(near(out[1],float(std::cos(1.0))) && near(out[2],float(std::sin(1.0))));
 assert(faceZipDirection(empty,20,0,0,n,velocity,out,fallback) && fallback);
 assert(near(out[0],0) && near(out[1],0) && near(out[2],1)); // neutral current velocity, normal component removed
 assert(!faceZipDirection(empty,20,0,1,n,velocity,out,fallback)); // untrained active stick never invents a direction
 float zero[3]={0,0,0};assert(!faceZipDirection(empty,20,0,0,n,zero,out,fallback));
 puts("PASS: M12 zip model direction, neutral velocity fallback, no-direction/no-velocity refusal");
  FaceWalkModel mean;train(mean,10,0,1,0,1.5f);train(mean,10.02,0,1,0,3.5f);train(mean,10.04,0,1,0,4.f);
  assert(near(mean.meanSpeed(10.1),3.f));
  assert(std::string(mean.takeoff(10.1,0,0,n,out))=="walk" && near(out[1],3.f));
  assert(std::string(mean.takeoff(10.1,0,1,n,out))=="walk" && near(out[1],5.f));
  float air[3]={0,5.3f,0},target[3]={0,0,1};
  for(int i=0;i<100;++i)steerFaceVelocity(air,target,0.016f,n);
  assert(near(std::hypot(air[1],air[2]),5.3f));assert(air[2]>5.29f);
  FaceJumpTrace trace;float origin[3]={2,3,4},launch[3]={10,5,0},at[3]={3,5,7},d[3],v[3];
  trace.start(10,origin,launch,0,1,"walk");assert(trace.split(origin,n,d,v));assert(near(d[0],0)&&near(d[1],0)&&near(d[2],0));
  assert(trace.split(at,n,d,v));assert(near(d[0],2)&&near(d[1],3)&&near(d[2],1));assert(v[0]==5&&v[1]==0&&v[2]==10);
  puts("PASS: M13 0.3s mean/floor, constant air steering speed, takeoff-relative jump trace");
  FaceFallGuard g;
  for(int i=0;i<20;++i) { g.begin(0.016f,true); assert(g.apply(10,9.92f,true)==9.92f); assert(g.apply(9.92f,9.89f,true)==9.89f); }
  g.begin(0.08f,true);assert(g.apply(10,9.4f,true)==9.4f); // longer frame,7.5m/s allowed
  g={};g.begin(0.016f,true);assert(g.apply(10,9.92f,true)==9.92f);
  assert(near(g.apply(9.92f,9.83f,true),10)); // >9m/s restores earlier + current downward parts
  assert(std::string(g.reason)=="speed_over_9");
  assert(g.apply(10,9.99f,true)==10); // blocked remainder of same frame
  g={};
  for(int i=0;i<6;++i) { g.begin(0.02f,true); float want=10.f-(2.f+i*0.3f)*0.02f; assert(g.apply(10,want,true)==want); }
  g.begin(0.02f,true);assert(g.apply(10,10.f-3.8f*0.02f,true)==10);
  assert(std::string(g.reason)=="accelerating_six_frames");
  g.begin(0.02f,false);assert(g.apply(10,9.99f,false)==10);assert(g.apply(10,10.01f,false)==10.01f);
  g.begin(0.02f,true);assert(g.apply(10,9.90f,true)==9.90f && g.rises==0);
  puts("PASS: M13 unrestricted ordinary downward frames, fall speed/6 rises, neutral and whole-frame undo");
}
'''
(root / 'build').mkdir(exist_ok=True)
unit = root / 'build/face_control_tests.cpp'
unit.write_text('#include <algorithm>\n#include <cmath>\n#include <cassert>\n#include <cstdio>\n#include <string>\nbool near(float a,float b){return std::fabs(a-b)<1e-5f;}\n' + model + budget + tests)
subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', '-pedantic', '-fsanitize=address,undefined', '-g', str(unit), '-o', str(root/'build/face_control_tests')], check=True)
subprocess.run([str(root/'build/face_control_tests')], check=True)
