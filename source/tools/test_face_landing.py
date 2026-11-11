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
 for(int i=0;i<6;++i)assert(!r.step(true,0.56f,0.016f));
 assert(r.step(true,0.56f,0.016f)); // >100ms continuously off plane
 assert(!r.step(true,0.56f,0.05f));assert(!r.step(false,0.56f,0.05f)); // air/face jump breaks history
 assert(!r.step(true,0.56f,0.05f));assert(!r.step(true,0.15f,0.05f)); // <=15cm breaks history
 assert(!r.step(true,0.56f,0.05f));assert(!r.step(true,NAN,0.05f));assert(r.offTime==0);
 FacePlaneHistory h;assert(!h.changed(plane));assert(!h.changed(plane));
 float changed[3]={3.04f,4,5};assert(!h.changed(changed));
 changed[0]=3.06f;assert(h.changed(changed));assert(!h.changed(changed));
 puts("PASS: M12 stop push clamps first/repeated calls at0.9m, preserves tangent");
 puts("PASS: M12 reapproach sustained threshold/reset/air refusal, plane-change threshold");
}
'''
(root/'build').mkdir(exist_ok=True)
unit=root/'build/face_landing_tests.cpp'
unit.write_text('#include <cmath>\n#include <cassert>\n#include <cstdio>\nbool near(float a,float b){return std::fabs(a-b)<1e-5f;}\n'+helpers+tests)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined','-g',str(unit),'-o',str(root/'build/face_landing_tests')],check=True)
subprocess.run([str(root/'build/face_landing_tests')],check=True)
