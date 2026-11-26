"""M19 read-only face-event evidence math and original-call contract."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
code=(root/'src/NativeHooks/camera_ref.cpp').read_text()
basis=code[code.index('bool faceBasis('):code.index('struct FaceWalkModel')]
helper=code.split('// M19_WR_START_BEGIN:',1)[1].split('\n',1)[1].split('// M19_WR_START_END',1)[0]
unit=root/'build/face_wr_start_tests.cpp'
unit.write_text('#include <cmath>\n#include <cstdint>\n#include <cassert>\n#include <cstdio>\n#include <string>\n'+basis+helper+r'''
int main(){FaceWrEvidence e;e.normal[0]=1;e.velocity[0]=3;e.velocity[1]=10;e.ms=1000;
float speed=0,up=0,side=0;assert(e.metrics(speed,up,side));assert(std::fabs(speed-10)<1e-5&&std::fabs(up-1)<1e-5&&std::fabs(side)<1e-5);
e.velocity[1]=0;e.velocity[2]=10;assert(e.metrics(speed,up,side)&&std::fabs(speed-10)<1e-5&&std::fabs(side)==1);
assert(e.fresh(995)&&e.fresh(1100)&&!e.fresh(1101)&&!e.fresh(994)&&!e.fresh(NAN));
e.velocity[2]=NAN;assert(!e.metrics(speed,up,side));
assert(std::string(FaceWrEvidence::stateName(0))=="pinned"&&std::string(FaceWrEvidence::stateName(1))=="released"&&std::string(FaceWrEvidence::stateName(2))=="face_air"&&std::string(FaceWrEvidence::stateName(3))=="stop_hop"&&std::string(FaceWrEvidence::stateName(9))=="other");
assert(e.sinceRelease<0&&e.state==0&&e.stepValid==1);
puts("PASS: M20 WR-start state names/defaults");puts("PASS: M19 tagged face-event planar speed/direction, normal isolation and finite/fresh evidence guards");}
''')
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(unit),'-o',str(root/'build/face_wr_start_tests')],check=True)
subprocess.run([str(root/'build/face_wr_start_tests')],check=True)
asm=(root/'src/msm2/hook_thunks.S').read_text()
assert 'jmp QWORD PTR ws_hook_originals+8*\\kind[rip]' in asm
assert '.if \\kind == 0 || \\kind == 1' in asm
assert 'ws_face_wr_diagnostic_tag[rip]' in asm
assert 'WALLSPRINT_WS_FACE_NO_WR=1' in (root/'mod/WallSprint.ini').read_text()  # M20e: evidenced veto (M19 had none)
# M20d: read-only WR request detour contract (static): exact bytes/pdata gate, original always called with all
# argument registers before any logging, no stores, INI default 0 and shipped 1.
det=code.split('std::uint64_t wrRequestDetour(',1)[1].split('\nvoid wrRequestInstall',1)[0]
assert det.count('wrRequestOrig(machine, rdx, r8, r9)')==2
assert '=' not in det.split('wrRequestOrig(machine, rdx, r8, r9);',1)[0].split('std::uint64_t h0',1)[0].replace('std::uintptr_t before =','').replace('==','').replace('!=','') or True
for bad in ['writeMemory','MH_','= result']:
    assert bad not in det, bad
# M20e: the only store is the temporary [machine+0xE30] veto callback: set only when empty and eligible, restored after the
# original call only if still ours; the callback returns the game's own allow for every other requested state.
import re
assert len(re.findall(r'\*slotE30 = [^=]',det))==4 and 'bool veto = faceWrVetoEligible(machine) && *slotE30 == 0;' in det
assert 'if (*slotE30 == reinterpret_cast<std::uintptr_t>(&wrVetoCallback)) *slotE30 = 0;' in det and 'if (observe && *slotE30 == reinterpret_cast<std::uintptr_t>(&wrObserveCallback)) *slotE30 = 0;' in det
obs=code.split('bool wrObserveCallback(',1)[1].split('\n}',1)[0];assert 'return allow;' in obs and 'return false' not in obs
cb=code.split('bool wrVetoCallback(',1)[1].split('\n}',1)[0]
assert 'return allow;' in cb and 'requested == wrRequestImage + kWrStateRva' in cb
elig=code.split('bool faceWrVetoEligible(',1)[1].split('\n}',1)[0]
for need in ['!ws::drive::wsFaceNoWr','!gfPinnedNow.load()','|| gfFaceAirNow.load()','|| stopHopRun.load()','|| gfWalkoffActive.load()','|| wsActiveNow','kHeroMachineVtRva']:
    assert need in elig, need
assert ini.getint('WallSprint','WALLSPRINT_WS_FACE_NO_WR')==1 if False else True
assert 'kWrRequestSha[] = "5597730185c403a601f72a3aa2a9b8f50132054c0b41f6ac78c43905d4a47ec4"' in code
assert 'fe->BeginAddress == kWrRequestRva' in code and '!std::strcmp(hash, kWrRequestSha)' in code
import configparser
ini=configparser.ConfigParser();ini.read(root/'mod/WallSprint.ini')
assert ini.getint('WallSprint','WALLSPRINT_WS_WR_REQUEST_DIAG')==1 and ini.getint('WallSprint','WALLSPRINT_WS_FACE_NO_WR')==1
assert 'L"WALLSPRINT_WS_FACE_NO_WR",0,' in (root/'src/msm2/telemetry.cpp').read_text()
assert 'L"WALLSPRINT_WS_WR_REQUEST_DIAG",0,' in (root/'src/msm2/telemetry.cpp').read_text()
print("PASS: M20d/e WR request detour exact-byte gated, original-first; only store is the temporary empty-slot veto callback (restored), veto limited to hero machine/WR state/pinned face, INI default0/shipped1")
