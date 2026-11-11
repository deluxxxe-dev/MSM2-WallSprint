"""Actual M14 frame clock, with owned call timing instead of game execution."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
s=(root/'src/NativeHooks/camera_ref.cpp').read_text()
helper=s.split('// M14_FRAME_CLOCK_BEGIN:',1)[1].split('\n',1)[1].split('// M14_FRAME_CLOCK_END',1)[0]
test=r'''
int main(){
 HeroFrameClock c;c.rebase(10,true);
 for(double t:{10.002,10.0041,10.0063})assert(!c.step(t,true));
 assert(c.step(10.0167,true));assert(near(c.dt,0.0167));
 assert(!c.step(10.0189,true));assert(!c.step(10.021,true));
 assert(c.step(10.0334,true));assert(near(c.dt,0.0167));
 assert(near(c.maxIntra,0.0022)&&near(c.minDt,0.0167));
 c.rebase(20,true);assert(c.step(20.008,true));assert(near(c.minDt,0.008)); // rebase preserves hold statistics
 assert(!c.step(21,false));for(int i=1;i<5;++i)assert(!c.step(21+i*0.001,false));
 assert(c.step(21.006,false));assert(near(c.dt,0.006)); // worker fallback still advances
 assert(!c.step(22,true));assert(!c.step(22.0021,true));assert(c.step(22.016,true));
 assert(!c.step(NAN,true));
 puts("PASS: M14 2.0-2.2ms calls share a frame, first-call dt and hold statistics,5ms grouping/fallback");
}
'''
(root/'build').mkdir(exist_ok=True)
p=root/'build/face_frame_tests.cpp';p.write_text('#include <cmath>\n#include <cassert>\n#include <cstdio>\n#include <initializer_list>\nbool near(double a,double b){return std::fabs(a-b)<1e-7;}\n'+helper+test)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(p),'-o',str(root/'build/face_frame_tests')],check=True)
subprocess.run([str(root/'build/face_frame_tests')],check=True)
