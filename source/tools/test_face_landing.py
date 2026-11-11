"""Owned M12 stop/plane tests compile the actual pure production helpers."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
code=(root/'src/NativeHooks/camera_ref.cpp').read_text()
helpers=code.split('// M12_LANDING_BEGIN:',1)[1].split('\n',1)[1].split('// M12_LANDING_END',1)[0]
tests=r'''
int main() {
 float plane[3]={3,4,5}, n[3]={1,0,0}, q[3]={5.7f,10,20};
 clampStopPush(q,plane,n); assert(near(facePlaneDistance(q,plane,n),0.9f));
 assert(q[1]==10 && q[2]==20); // preserve tangent
 for(int i=0;i<4;++i){q[0]+=0.9f;clampStopPush(q,plane,n);assert(near(facePlaneDistance(q,plane,n),0.9f));}
 float tilted[3]={0.8f,0,0.6f}, q2[3]={-10,8,12};
 clampStopPush(q2,plane,tilted);assert(near(facePlaneDistance(q2,plane,tilted),0.9f));
 FaceReapproach r;
 for(int i=0;i<12;++i)assert(!r.step(true,0.56f,0.016f,10+i*0.016));
 assert(r.step(true,0.56f,0.016f,10.192));
 for(int i=0;i<15;++i)assert(!r.step(true,0.56f,0.05f,10.25+i*0.05));
 assert(r.step(true,0.56f,0.05f,11.192)); // at most once per second
 assert(!r.step(true,0.18f,0.05f,12.2)); // previous0.18m loop never admitted
 assert(!r.step(false,0.56f,0.05f,12.3));assert(r.offTime==0);
 FacePlaneFollow f;
 for(int i=0;i<4;++i)assert(f.step(true,true,0,0.18f)==0);
 assert(near(f.step(true,true,0,0.18f),0.045f));
 assert(near(f.step(true,true,0,2.f),0.05f));assert(near(f.step(true,true,0,-2.f),-0.05f));
 assert(f.step(true,true,0.003f,0.18f)==0 && f.quiet==0);
 assert(f.step(false,true,0,0.18f)==0);assert(f.step(true,false,0,0.18f)==0);
 float distance=0.18f;
 for(int i=0;i<50;++i)distance-=f.step(true,true,0,distance);
 assert(distance<0.0001f); // quiet rest is learned, no reapproach loop
 FacePlaneHistory h;assert(!h.changed(plane));assert(!h.changed(plane));
 float changed[3]={3.04f,4,5};assert(!h.changed(changed));
 changed[0]=3.06f;assert(h.changed(changed));assert(!h.changed(changed));
 puts("PASS: M12 stop push clamps first/repeated calls at0.9m, preserves tangent");
 puts("PASS: M13 quiet-plane follow/caps/air refusal,0.18m rest, reapproach threshold/cooldown");
}
'''
(root/'build').mkdir(exist_ok=True)
unit=root/'build/face_landing_tests.cpp'
unit.write_text('#include <algorithm>\n#include <cmath>\n#include <cassert>\n#include <cstdio>\nbool near(float a,float b){return std::fabs(a-b)<1e-5f;}\n'+helpers+tests)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined','-g',str(unit),'-o',str(root/'build/face_landing_tests')],check=True)
subprocess.run([str(root/'build/face_landing_tests')],check=True)
