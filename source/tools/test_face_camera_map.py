"""Owned camera matrices validate eight actual walking prediction hypotheses."""
from pathlib import Path
import subprocess,configparser
r=Path(__file__).resolve().parents[1];s=(r/'src/NativeHooks/camera_ref.cpp').read_text();core=''
for tag in ['M16_AIR_FACING','M17_CAMERA_MAP']:core+=s.split('// '+tag+'_BEGIN:',1)[1].split('\n',1)[1].split('// '+tag+'_END',1)[0]
ini=configparser.ConfigParser();ini.read(r/'mod/WallSprint.ini');assert ini.getint('WallSprint','WALLSPRINT_WS_FACE_CAMERA_MAP')==1
assert 'wsFaceCameraMap{false}' in (r/'include/msm2/drive_command.hpp').read_text()
assert 'L"WALLSPRINT_WS_FACE_CAMERA_MAP",0,' in (r/'src/msm2/telemetry.cpp').read_text()
tests=r'''
void matrix(float angle,float* m){for(int i=0;i<16;++i)m[i]=0;float f[3]={0,std::cos(angle),std::sin(angle)},u[3]={1,0,0},right[3];FaceAirFacing::cross(u,f,right);for(int i=0;i<3;++i){m[i]=right[i];m[4+i]=u[i];m[8+i]=f[i];}m[15]=1;}
int main(){
 const float pi=3.14159265358979323846f;float n[3]={1,0,0},m[16],actual[3],errors[8],out[3];const char* why="";
 CameraFaceMap map;map.sync(100,n);
 for(int i=0;i<60;++i){matrix((i/20)*pi/3,m);float sx=std::sin(i*pi/4),sy=std::cos(i*pi/4);assert(CameraFaceMap::predict(1,m,sx,sy,n,actual));assert(map.observe(m,sx,sy,actual,n,errors));if(i<40)assert(!map.validMask);}
 assert(map.used==60&&map.threeDirections()&&(map.validMask&2));assert(map.choose(m,1,0,n,out,why)==1&&std::string(why)=="validated");
 CameraFaceMap onePose;onePose.sync(100,n);matrix(0,m);for(int i=0;i<60;++i){CameraFaceMap::predict(1,m,1,0,n,actual);assert(onePose.observe(m,1,0,actual,n,errors));}assert(!onePose.validMask&&!onePose.threeDirections());
 CameraFaceMap ambiguous;ambiguous.sync(100,n);for(int i=0;i<60;++i){matrix((i/20)*pi/3,m);CameraFaceMap::predict(1,m,0,1,n,actual);assert(ambiguous.observe(m,0,1,actual,n,errors));}
 assert((ambiguous.validMask&3)==3);assert(ambiguous.choose(m,0,1,n,out,why)>=0);assert(ambiguous.choose(m,1,0,n,out,why)<0&&std::string(why)=="ambiguous_current_direction");
 map.sync(101,n);assert(map.used==0&&!map.validMask);float zero[3]{};assert(!map.observe(m,1,0,zero,n,errors));m[5]=NAN;assert(!map.observe(m,1,0,actual,n,errors));
 matrix(0,m);for(int i=0;i<300;++i){matrix((i%3)*pi/3,m);CameraFaceMap::predict(1,m,1,1,n,actual);assert(map.observe(m,1,1,actual,n,errors));}assert(map.used==256&&map.threeDirections());
 puts("PASS: M17 eight camera hypotheses, median<10 over40/three separated directions, live prediction and single-pose refusal");
 puts("PASS: M17 ambiguous handedness refusal for unobserved directions, source reset/invalid guards and256 sample bound");
}
'''
(r/'build').mkdir(exist_ok=True);p=r/'build/face_camera_map_tests.cpp';p.write_text('#include <algorithm>\n#include <cmath>\n#include <cstring>\n#include <cstdint>\n#include <cassert>\n#include <cstdio>\n#include <string>\n'+core+tests)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(p),'-o',str(r/'build/face_camera_map_tests')],check=True)
subprocess.run([str(r/'build/face_camera_map_tests')],check=True)
