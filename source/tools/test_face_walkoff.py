"""Owned native-frame walk-off proxy and actual released physics."""
from pathlib import Path
import subprocess
r=Path(__file__).resolve().parents[1];s=(r/'src/NativeHooks/camera_ref.cpp').read_text()
import configparser
ini=configparser.ConfigParser();ini.read(r/'mod/WallSprint.ini')
assert ini.getint('WallSprint','WALLSPRINT_WS_FACE_WALKOFF')==1
assert 'wsFaceWalkoff{false}' in (r/'include/msm2/drive_command.hpp').read_text()
assert 'L"WALLSPRINT_WS_FACE_WALKOFF",0,' in (r/'src/msm2/telemetry.cpp').read_text()
core=''
for name in ['M15_PHYSICS','M15_WALKOFF']:
 core+=s.split('// '+name+'_BEGIN:',1)[1].split('\n',1)[1].split('// '+name+'_END',1)[0]
test=r"""
int main(){
 FaceWalkoff w;float up[3]={1,0,0},move[3]={0,0.08f,0},zero[3]{};
 for(int i=0;i<5;++i)assert(w.step(true,true,1,move,0.016f,up)==0);
 for(int i=0;i<7;++i)assert(w.step(true,true,1,zero,0.016f,up)==0);
 assert(w.step(true,true,1,zero,0.016f,up)==1 && w.active && std::fabs(w.carry[1]-5)<1e-5f);
 float p[3]{},v[3]={0,5,0},dash[3]{};
 for(int i=0;i<90;++i){float q[3];generalGravityStep(v,up,33,0.016f,false);releasedAirStep(q,p,v,dash,0.016f);
  assert(std::hypot(q[0]-p[0],q[1]-p[1],q[2]-p[2])<=30*0.0161f);for(int j=0;j<3;++j)p[j]=q[j];}
 assert(p[0]<-20 && p[1]>5 && p[2]==0); // crosses old plane; no world-down gravity
 dash[0]=-18;float q[3];releasedAirStep(q,p,v,dash,0.016f);assert(std::hypot(q[0]-p[0],q[1]-p[1])<=30*0.0161f);
 assert(w.step(false,true,1,move,0.016f,up)==0);assert(w.step(false,true,1,move,0.016f,up)==0);
 assert(w.step(false,true,1,move,0.016f,up)==2&&!w.active);
 FaceWalkoff obstacle;for(int i=0;i<100;++i)assert(obstacle.step(true,true,1,zero,0.016f,up)==0); // no preceding walk
 for(int i=0;i<5;++i)obstacle.step(true,true,1,move,0.016f,up);
 obstacle.step(true,false,1,zero,0.016f,up);for(int i=0;i<20;++i)assert(obstacle.step(true,true,1,zero,0.016f,up)==0);
 for(int i=0;i<5;++i)obstacle.step(true,true,1,move,0.016f,up);
 obstacle.step(true,true,0,zero,0.016f,up);for(int i=0;i<20;++i)assert(obstacle.step(true,true,1,zero,0.016f,up)==0);
 puts("PASS: M15 five-of-ten then eight-zero native proxy, invalid/neutral/no-walk refusal and three-step land");
 puts("PASS: M15 released air crosses old plane along changed gravity, carries planar speed and caps total zip motion30m/s");
}
"""
(r/'build').mkdir(exist_ok=True);p=r/'build/face_walkoff_tests.cpp'
p.write_text('#include <algorithm>\n#include <cmath>\n#include <cassert>\n#include <cstdio>\n'+core+test)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(p),'-o',str(r/'build/face_walkoff_tests')],check=True)
subprocess.run([str(r/'build/face_walkoff_tests')],check=True)
