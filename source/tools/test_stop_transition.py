"""M18: compile the production stop-only helpers on owned synthetic rows/input."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
code=(root/'src/NativeHooks/camera_ref.cpp').read_text()
def helper(mark):return code.split('// '+mark+'_BEGIN:',1)[1].split('\n',1)[1].split('// '+mark+'_END',1)[0]
helpers=helper('M18_STOP')+helper('M16_AIR_FACING')+helper('M18_ROTATION')
tests=r'''
int main(){StopEpisode s;assert(!s.block(1,0,1));s.begin(10,3);
assert(s.block(10.1,1,4));s.landing=10.35;assert(s.block(11.849,1,4));
assert(s.block(11.85,0,3));assert(s.report(11.86));assert(!s.report(12));
assert(!s.block(12,.2f,3));assert(!s.block(12,0,3));
s.begin(20,4);s.landing=20.35;assert(!s.block(22,0,5));
s.begin(30,9);s.landing=30.35;assert(s.block(100,0,9));
float n[3]={1,0,0},f[3]={0,0,1};FaceAirFacing facing;
assert(facing.begin(f,n));float rows[12]={1,0,0,7, 0,1,0,8, 0,0,1,9},out[12];
assert(!facing.prepare(rows,out,true,true));assert(facing.prepare(rows,out,true,true,true));
assert(out[4]==1&&out[5]==0&&out[7]==8&&out[11]==9&&out[3]==7);
facing.committed();facing.holdWrites=4000;assert(!facing.prepare(rows,out,true,true,true));
StopRotation rotation;rotation.begin(f,n);rotation.landing=10;float goal[12]={0,0,1,7, 1,0,0,8, 0,1,0,9},v[3];
assert(rotation.target(10.399,goal,v)&&v[2]==1);assert(rotation.target(10.5,goal,v));assert(std::fabs(v[1]-.70710678f)<1e-5f&&std::fabs(v[2]-.70710678f)<1e-5f);
assert(!rotation.target(10.601,goal,v));
puts("PASS: M18 stop rotation corrects world-up rows with preserved handedness/W/cap, holds0.4s then normalised0.2s blend");
puts("PASS: M18 stop release guard covers hop plus1.5s, neutral/no-A continuation, motion/A expiry and per-stop count");}
'''
unit=root/'build/stop_transition_tests.cpp'
unit.write_text('#include <algorithm>\n#include <cmath>\n#include <cstring>\n#include <cassert>\n#include <cstdio>\n'+helpers+tests)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined','-g',str(unit),'-o',str(root/'build/stop_transition_tests')],check=True)
subprocess.run([str(root/'build/stop_transition_tests')],check=True)
