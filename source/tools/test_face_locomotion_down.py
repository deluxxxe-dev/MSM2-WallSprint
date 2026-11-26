"""M20 actual helpers: the game's walk/sprint toward world down on a face is not a fall."""
from pathlib import Path
import subprocess
r=Path(__file__).resolve().parents[1];s=(r/'src/NativeHooks/camera_ref.cpp').read_text()
def section(name):
    return s.split('// '+name+'_BEGIN:',1)[1].split('\n',1)[1].split('// '+name+'_END',1)[0]
core=section('M15_PHYSICS')+section('M16_FALL')+section('M13_FALL_GUARD')
assert '!gfL.locoDown.active&&!stopBlocked' in s
assert 'FaceLocomotionDown::kFrameLimit : -1.f' in s
test=r"""
static void frame(FaceLocomotionDown& d,const float* up,float vy,float vz,float stick,double& t,float dt=0.016f,bool expect=false,bool check=false){
 float step[3]={0,vy*dt,vz*dt};bool a=d.update(true,true,stick,step,dt,up,t);t+=dt;if(check)assert(a==expect);
}
int main(){
 float up[3]={1,0,0};double t=100;
 // sprint start toward the ground: 0 -> 10.2 m/s in 0.5 s, then steady; exempt once three moving frames exist.
 FaceLocomotionDown d;bool seen=false;
 for(int i=0;i<32;++i){float v=std::fmin(10.2f,0.6f+20.f*i*0.016f);frame(d,up,-v,0,1,t);if(i>=6){assert(d.active);seen=true;}}
 assert(seen&&std::fabs(d.lastCos-1)<1e-4f&&d.lastSpeed<=10.21f);
 for(int i=0;i<60;++i)frame(d,up,-10.2f,0,1,t,0.016f,true,true);
 // stick released: the game's own sprint stop decelerates 10.2 -> 0 m/s in ~0.6 s and stays exempt (coasting).
 { FaceLocomotionDown c=d;double tc=t;for(int i=1;i<=36;++i){float v=std::fmax(0.f,10.2f-17.f*i*0.016f);frame(c,up,-v,0,0,tc);if(v>1.2f)assert(c.active&&c.coasting);}
   // neutral stick and accelerating toward world down (real fall) ends the coasting at once.
   FaceLocomotionDown e=d;double te=t;bool stopped=false;for(int i=1;i<=20&&!stopped;++i){frame(e,up,-(10.2f+26.f*i*0.016f),0,0,te);stopped=!e.active;}assert(stopped);
   // coasting is limited to 1 s.
   FaceLocomotionDown l=d;double tl=t;for(int i=0;i<70;++i)frame(l,up,-6.f,0,0,tl);assert(!l.active); }
 // single jitter frames (13-16 m/s) inside a ~10 m/s sprint keep the exemption (0.1 s window).
 frame(d,up,-16.4f,0,1,t,0.016f,true,true);for(int i=0;i<10;++i)frame(d,up,-10.2f,0,1,t,0.016f,true,true);
 // above the sprint envelope over 0.1 s (a real fall continuing) the exemption ends.
 { FaceLocomotionDown z=d;double tz=t;bool ended=false;for(int i=0;i<12&&!ended;++i){frame(z,up,-13.f,0,1,tz);ended=!z.active;}assert(ended); }
 // neutral stick from the start: never exempt.
 FaceLocomotionDown n;double tn=200;for(int i=0;i<20;++i)frame(n,up,-5,0,0.1f,tn,0.016f,false,true);
 // walking sideways then falling world down (walk-off a side edge): the 0.3 s direction is sideways -> not exempt.
 FaceLocomotionDown w;double tw=300;for(int i=0;i<30;++i)frame(w,up,0,5,1,tw);
 for(int i=1;i<=8;++i)frame(w,up,-26.f*i*0.016f,0,1,tw,0.016f,false,true);
 // walking toward the roof: not exempt (world up).
 FaceLocomotionDown u;double tu=400;for(int i=0;i<30;++i)frame(u,up,10,0,1,tu,0.016f,false,true);
 // diagonal 30 deg off world down: exempt; 60 deg: not.
 FaceLocomotionDown g;double tg=500;for(int i=0;i<30;++i)frame(g,up,-8.66f,5,1,tg);assert(g.active);
 FaceLocomotionDown h;double th=600;for(int i=0;i<30;++i)frame(h,up,-5,8.66f,1,th);assert(!h.active);
 // horizontal face (floor/ceiling, up = world up): world down has no in-plane part -> never exempt.
 float floorUp[3]={0,1,0};FaceLocomotionDown f;double tf=700;float st[3]={0.16f,0,0};
 for(int i=0;i<30;++i){assert(!f.update(true,true,1,st,0.016f,floorUp,tf));tf+=0.016;}
 // invalid evidence resets.
 FaceLocomotionDown v;double tv=800;for(int i=0;i<30;++i)frame(v,up,-6,0,1,tv);assert(v.active);
 float bad[3]={0,NAN,0};assert(!v.update(true,true,1,bad,0.016f,up,tv)&&!v.active&&v.count==0);
 assert(!v.update(false,true,1,st,0.016f,up,tv+0.016));
 assert(!v.update(true,true,1,st,0.2f,up,tv+0.032));
 puts("PASS: M20 locomotion-down: sprint start/steady/stop-coast exempt, coast refuses acceleration and >1 s, >11.5 m/s, neutral stick, side walk-off, roof, 60 deg, floor and invalid refuse");
 // the fall guard keeps the game's 10.2 m/s sprint step when exempt and still undoes beyond the limit.
 FaceFallGuard g2;g2.begin(0.016f,true);float want=10.f-10.2f*0.016f;assert(g2.apply(10,want,true,FaceLocomotionDown::kFrameLimit)==want);
 g2={};g2.begin(0.016f,true);assert(g2.apply(10,want,true)==10); // without exemption: speed_over_9 undo
 g2={};g2.begin(0.016f,true);float fast=10.f-21.f*0.016f;assert(g2.apply(10,fast,true,FaceLocomotionDown::kFrameLimit)==10);
 assert(std::string(g2.reason)=="speed_over_9");
 g2={};g2.begin(0.016f,true);float spike=10.f-16.4f*0.016f;assert(g2.apply(10,spike,true,FaceLocomotionDown::kFrameLimit)==spike); // measured jitter frame
 puts("PASS: M20 fall guard keeps exempt sprint steps <= 11.5 m/s keeps 16.4 m/s jitter frames and undoes >20 m/s frames");
 // the walk-off detector would have released on the same sprint start (reason for the exemption gate).
 FaceWalkoff wk;float upw[3]={1,0,0};bool released=false;double tk=900;
 for(int i=0;i<32&&!released;++i){float v=std::fmin(10.2f,0.6f+20.f*i*0.016f);float stp[3]={0,-v*0.016f,0};released=wk.step(true,true,1,stp,0.016f,upw,0,tk)==1;tk+=0.016;}
 assert(released);
 puts("PASS: M20 without the gate the walk-off window releases on a sprint start toward the ground (measured failure)");
}
"""
(r/'build').mkdir(exist_ok=True);p=r/'build/face_locomotion_down_tests.cpp'
p.write_text('#include <algorithm>\n#include <cmath>\n#include <cassert>\n#include <cstdio>\n#include <string>\n'+core+test)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(p),'-o',str(r/'build/face_locomotion_down_tests')],check=True)
subprocess.run([str(r/'build/face_locomotion_down_tests')],check=True)
