"""Integrate actual production general/face-air steps, never game code."""
from pathlib import Path
import subprocess
r=Path(__file__).resolve().parents[1]
s=(r/'src/NativeHooks/camera_ref.cpp').read_text()
core=s.split('// M15_PHYSICS_BEGIN:',1)[1].split('\n',1)[1].split('// M15_PHYSICS_END',1)[0]
test=r"""
int main(){
 for(float height:{1.8f,0.5f}){
  float n[3]={0.8f,0,0.6f},p[3]{},v[3]={n[0]*std::sqrt(66*height),0,n[2]*std::sqrt(66*height)},dash[3]{},planar[3]{};
  float peak=0,peakTime=0,landing=0;
  for(int frame=1;frame<1000;++frame){float q[3];float dt=0.001f;
   generalGravityStep(v,n,33,dt,true);faceAirStep(q,p,v,n,planar,dash,33,dt);
   float d=q[0]*n[0]+q[2]*n[2];if(d>peak){peak=d;peakTime=frame*dt;}
   for(int i=0;i<3;++i)p[i]=q[i];
   if(frame>100&&d<0){landing=frame*dt;break;}
  }
  assert(std::fabs(peak-height)<0.05f);
  assert(std::fabs(peakTime-(height>1 ? 0.33f:0.175f))<0.02f);
  assert(std::fabs(landing-2*std::sqrt(2*height/33))<0.005f);
 }
 float v[3]{},up[3]={1,0,0};generalGravityStep(v,up,33,0.016f,false);assert(std::fabs(v[0]+0.528f)<1e-5f);
 for(int i=0;i<1000;++i)generalGravityStep(v,up,33,0.016f,false);
 assert(std::fabs(v[0]+30)<1e-5f);
 puts("PASS: M15 actual general/face path single gravity,1.8m/.33s jump,.5m/.175s hop and30m/s general cap");
}
"""
(r/'build').mkdir(exist_ok=True)
p=r/'build/face_physics_tests.cpp';p.write_text('#include <cmath>\n#include <cassert>\n#include <cstdio>\n#include <initializer_list>\n'+core+test)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(p),'-o',str(r/'build/face_physics_tests')],check=True)
subprocess.run([str(r/'build/face_physics_tests')],check=True)
