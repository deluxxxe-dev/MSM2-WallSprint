"""M19 read-only face-event evidence math and original-call contract."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
code=(root/'src/NativeHooks/camera_ref.cpp').read_text()
basis=code[code.index('bool faceBasis('):code.index('struct FaceWalkModel')]
helper=code.split('// M19_WR_START_BEGIN:',1)[1].split('\n',1)[1].split('// M19_WR_START_END',1)[0]
unit=root/'build/face_wr_start_tests.cpp'
unit.write_text('#include <cmath>\n#include <cstdint>\n#include <cassert>\n#include <cstdio>\n'+basis+helper+r'''
int main(){FaceWrEvidence e;e.normal[0]=1;e.velocity[0]=3;e.velocity[1]=10;e.ms=1000;
float speed=0,up=0,side=0;assert(e.metrics(speed,up,side));assert(std::fabs(speed-10)<1e-5&&std::fabs(up-1)<1e-5&&std::fabs(side)<1e-5);
e.velocity[1]=0;e.velocity[2]=10;assert(e.metrics(speed,up,side)&&std::fabs(speed-10)<1e-5&&std::fabs(side)==1);
assert(e.fresh(995)&&e.fresh(1100)&&!e.fresh(1101)&&!e.fresh(994)&&!e.fresh(NAN));
e.velocity[2]=NAN;assert(!e.metrics(speed,up,side));
puts("PASS: M19 tagged face-event planar speed/direction, normal isolation and finite/fresh evidence guards");}
''')
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(unit),'-o',str(root/'build/face_wr_start_tests')],check=True)
subprocess.run([str(root/'build/face_wr_start_tests')],check=True)
asm=(root/'src/msm2/hook_thunks.S').read_text()
assert 'jmp QWORD PTR ws_hook_originals+8*\\kind[rip]' in asm
assert '.if \\kind == 0 || \\kind == 1' in asm
assert 'ws_face_wr_diagnostic_tag[rip]' in asm
assert 'WALLSPRINT_WS_FACE_NO_WR' not in (root/'mod/WallSprint.ini').read_text()
