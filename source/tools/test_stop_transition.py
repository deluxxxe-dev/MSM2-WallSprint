"""M18: compile the production stop-only helpers on owned synthetic rows/input."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
code=(root/'src/NativeHooks/camera_ref.cpp').read_text()
def helper(mark):return code.split('// '+mark+'_BEGIN:',1)[1].split('\n',1)[1].split('// '+mark+'_END',1)[0]
helpers=helper('M18_STOP')
tests=r'''
int main(){StopEpisode s;assert(!s.block(1,0,1));s.begin(10,3);
assert(s.block(10.1,1,4));s.landing=10.35;assert(s.block(11.849,1,4));
assert(s.block(11.85,0,3));assert(s.report(11.86));assert(!s.report(12));
assert(!s.block(12,.2f,3));assert(!s.block(12,0,3));
s.begin(20,4);s.landing=20.35;assert(!s.block(22,0,5));
s.begin(30,9);s.landing=30.35;assert(s.block(100,0,9));
puts("PASS: M18 stop release guard covers hop plus1.5s, neutral/no-A continuation, motion/A expiry and per-stop count");}
'''
unit=root/'build/stop_transition_tests.cpp'
unit.write_text('#include <algorithm>\n#include <cmath>\n#include <cstring>\n#include <cassert>\n#include <cstdio>\n'+helpers+tests)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined','-g',str(unit),'-o',str(root/'build/stop_transition_tests')],check=True)
subprocess.run([str(root/'build/stop_transition_tests')],check=True)
