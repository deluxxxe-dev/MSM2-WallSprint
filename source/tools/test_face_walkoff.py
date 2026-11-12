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
for name in ['M15_PHYSICS','M16_FALL','M17_CONTACT']:
 core+=s.split('// '+name+'_BEGIN:',1)[1].split('\n',1)[1].split('// '+name+'_END',1)[0]
test=r"""
int main(){
 FaceWalkoff w;float up[3]={1,0,0},move[3]={0,0.08f,0},zero[3]{};
 for(int i=0;i<10;++i)assert(w.step(true,true,1,move,0.016f,up)==0);
 for(int i=0;i<100;++i)assert(w.step(true,true,1,zero,0.016f,up)==0); // zero-step obstacle cannot release
 float fall[3]={0,0,0};
 for(int i=0;i<4;++i){fall[1]=-(6.4f+i*0.4f)*0.016f;int event=w.step(true,true,1,fall,0.016f,up);assert(event==(i==3 ? 1:0));}
 assert(w.active&&std::fabs(w.carry[1]-5)<1e-5f);
 for(int i=0;i<20;++i){fall[1]=-(10.7f+i*0.4f)*0.016f;assert(w.step(false,true,0,fall,0.016f,up)==0&&w.active);}
 float p[3]{},v[3]={0,5,0},dash[3]{};
 for(int i=0;i<90;++i){float q[3];generalGravityStep(v,up,33,0.016f,false);releasedAirStep(q,p,v,dash,0.016f);
  assert(std::hypot(q[0]-p[0],q[1]-p[1],q[2]-p[2])<=30*0.0161f);for(int j=0;j<3;++j)p[j]=q[j];}
 assert(p[0]<-20 && p[1]>5 && p[2]==0); // crosses old plane; no world-down gravity
 dash[0]=-18;float q[3];releasedAirStep(q,p,v,dash,0.016f);assert(std::hypot(q[0]-p[0],q[1]-p[1])<=30*0.0161f);
 for(int i=0;i<20;++i)assert(w.step(false,false,1,zero,0.016f,up)==0&&w.active&&w.quiet==0); // worker/no native evidence cannot land
 assert(w.step(false,true,1,zero,0.016f,up)==0);assert(w.step(false,true,1,zero,0.016f,up)==0);
 assert(w.step(false,true,1,zero,0.016f,up)==2&&!w.active);
 FaceWalkoff steady;for(int i=0;i<20;++i){fall[1]=-5.5f*0.016f;assert(steady.step(true,true,1,fall,0.016f,up)==0);}
 steady.step(true,false,1,fall,0.016f,up);assert(!steady.havePrevious);
 FaceContactProbe probe;float origin[3]{},normal[3]={1,0,0},at[3]{},wanted[3];probe.begin(10,origin,normal,at);
 for(int i=0;i<10;++i){for(int k=0;k<3;++k)wanted[k]=at[k];wanted[0]-=0.03f;wanted[1]+=0.01f;probe.limit(at,wanted,normal);for(int k=0;k<3;++k)at[k]=wanted[k];}
 assert(std::fabs(at[0]+0.1f)<1e-5f&&at[1]>0.09f&&probe.used<=0.10001f);
 assert(probe.contact(0.02f,-0.1f)==1&&probe.contact(0.01f,-0.1f)==0&&probe.contact(0.02f,0.1f)==0);
 assert(!probe.advance(10.19)&&!probe.advance(10.21));probe.observe(10.21);assert(probe.advance(10.21));assert(probe.contact(0.02f,-5)==2);
 float before=probe.before[0];wanted[0]=at[0]-1;probe.limit(at,wanted,normal);assert(wanted[0]<-1&&probe.before[0]==at[0]&&before>=at[0]);
 FaceWalkoff deep;deep.active=true;for(int i=0;i<10;++i)assert(deep.step(false,true,0,zero,0.016f,up,-1)==0&&deep.active);
 assert(deep.step(false,true,0,zero,0.016f,up,0)==2);
 puts("PASS: M17 probe0.20s/0.10m budget preserves planar travel, +u contact rollback class and deep quiet-landing refusal");
 puts("PASS: M16 four rising world-down frames release, steady/zero/invalid refuse, continued world fall never repins, three slow frames land");
 puts("PASS: M16 face fall ignores game displacement, carries pre-fall walking velocity, caps30m/s and contact probe retains original-plane rollback");
}
"""
(r/'build').mkdir(exist_ok=True);p=r/'build/face_walkoff_tests.cpp'
p.write_text('#include <algorithm>\n#include <cmath>\n#include <cassert>\n#include <cstdio>\n'+core+test)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(p),'-o',str(r/'build/face_walkoff_tests')],check=True)
subprocess.run([str(r/'build/face_walkoff_tests')],check=True)
