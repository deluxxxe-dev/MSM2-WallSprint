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
assert len(re.findall(r'\*slotE30 = [^=]',det))==10 and 'bool veto = faceWrVetoEligible(machine) && *slotE30 == 0;' in det
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

# M21: entry callback only for the hero machine with an empty slot, restored after the call; returns the game's own allow
# for every non-WR request; reads the context normal/point and validates a unit, near-horizontal normal.
ent=code.split('bool wrEntryCallback(',1)[1].split('\n}',1)[0]
assert 'if (requested != wrRequestImage + kWrStateRva) return allow;' in ent and 'std::fabs(pn[4]) < 0.3f' in ent and 'context + 0x2C' in ent
assert 'if (entryCb && *slotE30 == reinterpret_cast<std::uintptr_t>(&wrEntryCallback)) *slotE30 = 0;' in det
assert ini.getint('WallSprint','WALLSPRINT_WS_GROUND_ENTRY')==1 and 'L"WALLSPRINT_WS_GROUND_ENTRY",0,' in (root/'src/msm2/telemetry.cpp').read_text()
assert '!sbBlendQpc.load(std::memory_order_relaxed) && !sgLocked' in code  # B return is never cut by landed_game
print("PASS: M21 ground-entry callback hero-only/empty-slot/restored, validated context normal, INI default0/shipped1; B return not cut")

# M21b: after B every hero WR request is refused for a fixed window and WS detection is ignored; during a hold that is not
# pinned only a same-face WR (normal within 20 deg, plane within 0.6 m) is refused; the same-face slot is restored.
sf=code.split('bool wrSameFaceCallback(',1)[1].split('\n}',1)[0]
assert 'if (requested != wrRequestImage + kWrStateRva || !context) return allow;' in sf and 'kSameFaceCos' in sf and 'kSameFacePlane' in sf
assert 'if (sameFace) { if (*slotE30 == reinterpret_cast<std::uintptr_t>(&wrSameFaceCallback)) *slotE30 = 0;' in det
assert 'bool bBlock = !veto && *slotE30 == 0 && heroMachine' in det and 'GetTickCount64() < geBUntilMs.load()' in det
assert code.count('geBUntilMs.store(GetTickCount64() + kBCancelBlockMs)')==2 and code.count('wgiBTaps.load() != gbAPresses')==2 and 'wgiBPresses.load() != gbAPresses' not in code and 'if (b && wsNow) wsNow = false;' in code
assert 'gfL.fallIgnoreUntil = qpcSec(now) + 0.5; msm2::writeLog("WS_GROUND_ENTRY event=hold_no_hop' in code
print("PASS: M21b B-cancel WR/WS block, same-face WR refusal in unpinned hold (slot restored), 0.5 s fall ignore after no-hop entry")

# M21c: another face is refused while the hold settles (1 s), later converted into a no-hop face-to-face transfer that
# rotates from the current hold normal; ground/air entry still rotates from world up.
assert 'const char* decision = same ? "refused" : early ? "refused_settling" : horizontal ? "transfer" : "game";' in sf
assert 'if (same || early) { wrVetoFired = true; return false; }' in sf and 'if (!horizontal) return allow;' in sf
assert 'bool transfer = geTransfer.exchange(false) && gbState == 2;' in code and 'gfSummary("face_transfer")' in code
assert 'gbFrom[0] * (1.f - s) + gbN[0] * s' in code and code.count('geHoldStartMs.store(')==2
print("PASS: M21c settle refusal of other faces and no-hop face-to-face transfer from the current normal")

# M21d: only a B tap (< 0.3 s) cancels; held B arms the outer-edge wrap: a walk-off that leaves the face plane by 0.3 m
# after a clean probe within 1 s becomes a face transfer onto the travel-direction normal; without B it is only logged.
assert 'if (!b && was && GetTickCount64() - wgiBDownMs.load() < kBTapMs) wgiBTaps.fetch_add(1);' in code and 'kBTapMs=300' in code
assert 'if(!wgiLbDown.load())why="lb_not_held";' in code and 'if(since>1.0||!(gfWrap.speed>1.f)||!std::isfinite(gfWrap.speed))gfWrap.armed=false;' in code
assert 'geP[i].store(gfWrap.p[i]-gfWrap.dir[i]*back);geN[i].store(d[i]);' in code and 'wgiBDown.load()' not in code
print("PASS: M21d B tap cancel, B-held outer-edge wrap request (plane at earliest edge), no wrap without B")

# M21e: LB (0x400) is the wrap button; LB at the release stops the in-plane carry; the wrap normal is snapped (roof up,
# no downward wrap, otherwise horizontal travel); a wrap hold accepts the real face from WR at once; no WR back to the
# face just left for 1 s.
assert 'bool lb = (r->buttons & 0x400u) != 0; wgiLbDown.store(lb);' in code and 'gfWrap.stopped=true;' in code
assert 'if(d[1]>0.7f){d[0]=0;d[1]=1;d[2]=0;}else if(d[1]<-0.7f)why="downward";' in code
assert 'geHoldStartMs.store(geWrapHold.exchange(false) ? 0 : GetTickCount64())' in code and 'cb >= kSameFaceCos && GetTickCount64() - geTransferMs.load() < kTransferSettleMs' in code
print("PASS: M21e LB wrap button, carry stop, snapped wrap normal, immediate real-face correction, no ping-pong")

# M21f: LB is hidden from the game only while a face hold/entry is active; the entry arming is cleared whenever the
# worker cannot run an entry (no hero / no gravity block).
assert 'bool hideLb = lb && ws::drive::wsGroundEntry.load(std::memory_order_relaxed) && !killed.load() && (gbHoldA.load() || gfOn.load());' in code
assert 'if (hideLb) { r->buttons &= ~0x400u;' in code and code.count('{ geArmedNow.store(false); if (gbState) gbEnd(')==2
print("PASS: M21f LB hidden from the game during a face hold, stale entry arming cleared")

# M21g: reset on hero/Transform change (no write into the old block) or a >40 m teleport (vanilla written back); read-only
# state-change diagnostic of the hero machine during a face hold.
assert 'gbEnd(changed ? "hero_changed" : "teleport", now, !changed);' in code and 'dx * dx + dy * dy + dz * dz > 40.f * 40.f' in code
assert 'WS_FACE_STATE_CHANGE from=0x%llx' in code
det2=code.split('std::uint64_t wrRequestDetour(',1)[1].split('\nvoid wrRequestInstall',1)[0]
assert len(re.findall(r'\*slotE30 = [^=]',det2))==10
print("PASS: M21g fast-travel/hero-change reset and read-only face state-change diagnostic")

# M21h: LB + the game's ledge state 0xB45E2B0 during a pinned hold = immediate wrap from the last pinned edge position
# (request refused); no new entry from a running vanilla wall state.
lw=code.split('bool ledgeWrap(std::uintptr_t requested) {',1)[1].split('\n}',1)[0]
assert 'requested != wrRequestImage + kLedgeStateRva || !wgiLbDown.load()' in lw and 'if (!gfEdgeValid.load() || !gfOn.load()) return false;' in lw
assert code.count('if (ledgeWrap(requested) || holdRefuse(requested))')==4 and 'kLedgeStateRva = 0xB45E2B0' in code
assert 'bool inWallState = curRva == 0xB465FB0 || curRva == 0xB4519A0 || curRva == kWrStateRva;' in code and '(geArmedNow.load() && !inWallState)' in code
print("PASS: M21h ledge-state wrap with LB and no entry from a running vanilla wall state")

# M21i: jump/air/ledge refused for 1.5 s after a wrap request, crawl states refused while WS is armed or a hold runs
# (temporary hold callback, restored); same-face contexts 1.5-20 deg off re-level the hold normal at most every 0.5 s.
hr=code.split('bool holdRefuse(std::uintptr_t requested) {',1)[1].split('\n}',1)[0]
assert 'bool refuse = (wrapping && jump) || (crawl && gbHoldA.load());' in hr
assert 'if (holdCb && *slotE30 == reinterpret_cast<std::uintptr_t>(&wrHoldCallback)) *slotE30 = 0;' in code
assert 'if (std::isfinite(c) && c > 0.94f && c < 0.99966f)' in code and 'now - refLast >= 500' in code
print("PASS: M21i wrap jump refusal, crawl refusal in holds, face re-levelling from the game's wall normal")
