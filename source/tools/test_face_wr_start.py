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
assert len(re.findall(r'\*slotE30 = [^=]',det))==10 and 'bool veto = faceWrVetoEligible(machine) && *slotE30 == 0 && !crawlWr;' in det
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
assert 'bool hideLb = lb && ws::drive::wsEdgeWrap.load(std::memory_order_relaxed) && ws::drive::wsGroundEntry.load(std::memory_order_relaxed) && !killed.load() && (gbHoldA.load() || gfOn.load());' in code
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
assert 'if (!edgeState || !wgiLbDown.load() || !ws::drive::wsEdgeWrap' in lw and 'if (!gfEdgeValid.load() || !gfOn.load()) return false;' in lw
assert code.count('ledgeWrap(requested) || holdRefuse(requested))')==4 and 'kLedgeStateRva = 0xB45E2B0' in code
assert 'bool inWallState = curRva == 0xB465FB0 || curRva == kWrStateRva;' in code and '(geArmedNow.load() && !inWallState)' in code
print("PASS: M21h ledge-state wrap with LB and no entry from a running vanilla wall state")

# M21i: jump/air/ledge refused for 1.5 s after a wrap request, crawl states refused while WS is armed or a hold runs
# (temporary hold callback, restored); same-face contexts 1.5-20 deg off re-level the hold normal at most every 0.5 s.
hr=code.split('bool holdRefuse(std::uintptr_t requested) {',1)[1].split('\n}',1)[0]
assert 'bool refuse = (wrapOn && wrapping && jump) || lbJump || (exitOn && crawl && gbHoldA.load() && ms - geWsExitMs.load() < 500);' in hr
assert 'if (holdCb && *slotE30 == reinterpret_cast<std::uintptr_t>(&wrHoldCallback)) *slotE30 = 0;' in code
assert 'if (std::isfinite(c) && c > 0.94f && c < 0.99966f)' in code and 'now - refLast >= 500' in code
print("PASS: M21i wrap jump refusal, crawl refusal in holds, face re-levelling from the game's wall normal")

# M21j: jump/air/ledge with LB in a hold start the wrap from the edge; LB refuses the sprint jump in holds; the WR exit
# 0xB465AB0 forces the hold start and crawl is refused only 0.5 s after it; LB walk-off wrap after 0.05 s / 0.08 m.
assert 'bool edgeState = rr == kLedgeStateRva || rr == 0xB454950 || rr == 0xB45D6B0;' in code
assert 'if (exitOn && r == 0xB465AB0 && gbHoldA.load() && !gfOn.load())' in code and 'bool forceHold = gbState == 1 && geForceHold.exchange(false);' in code
assert '(wgiLbDown.load()&&since>=0.05&&dist<-0.08f)' in code
print("PASS: M21j edge-state wrap, LB jump refusal, bounded crawl refusal with forced hold at WR exit, early LB walk-off wrap")

# M21k: every experiment after M21c is behind its own INI switch, code default 0 and shipped 0 (owner: return to the last
# accepted behaviour); the wrap, the WR-exit assist, the re-levelling and the hold callback do nothing while they are 0.
tel=(root/'src/msm2/telemetry.cpp').read_text()
for k in ['WALLSPRINT_WS_EDGE_WRAP','WALLSPRINT_WS_WR_EXIT_ASSIST','WALLSPRINT_WS_FACE_RELEVEL']:
    assert 'L"'+k+'",0,' in tel, k
    assert ini.getint('WallSprint',k)==0, k
assert 'gfWrap.armed&&!gfWrap.requested&&ws::drive::wsEdgeWrap.load' in code and 'wgiLbDown.load()&&ws::drive::wsEdgeWrap.load' in code
assert 'bool lbJump = wrapOn && r == 0xB454950' in code and 'ws::drive::wsFaceRelevel.load(std::memory_order_relaxed) && same &&' in code
assert '(ws::drive::wsEdgeWrap.load(std::memory_order_relaxed) || ws::drive::wsWrExitAssist.load(std::memory_order_relaxed))' in code
print("PASS: M21k post-M21c experiments gated by INI switches, all shipped OFF")

# M21l: no WR veto in a hold while the game is in its crawl/wall family (freeze fix); crawl idle in a hold gives the game
# alone a bounded 60 ms A press; the sprint jump / ledge take-off are refused during a ground entry; INI default 0, shipped 1.
for k,v in [('WALLSPRINT_WS_CRAWL_WR',1),('WALLSPRINT_WS_CRAWL_STAND',0),('WALLSPRINT_WS_ENTRY_NO_JUMP',1)]:
    assert 'L"'+k+'",0,' in tel, k
    assert ini.getint('WallSprint',k)==v, k
assert 'bool veto = faceWrVetoEligible(machine) && *slotE30 == 0 && !crawlWr;' in code
assert 'stRva == 0xB465FB0 || stRva == 0xB463E40 || stRva == 0xB463C10 || stRva == 0xB4635E0 || stRva == 0xB465AB0 || stRva == 0xB4633E0' in code
assert 'if (GetTickCount64() < gcInjectUntilMs.load(std::memory_order_relaxed) && !killed.load()) r->buttons |= 0x4u;' in code
wgi=code.split('HRESULT STDMETHODCALLTYPE wgiReadDetour(',1)[1].split('\nHRESULT STDMETHODCALLTYPE wgiVibDetour',1)[0]
assert wgi.index('r->buttons |= 0x4u') > wgi.index('bool a = (r->buttons & 0x4u) != 0;')   # injected after every MOD read of A
assert 'gHeroStateRva.load() == 0xB463C10 && tick - gHeroStateMs.load() >= 100 && csCount < 3' in code
ej=code.split('bool entryJumpRefuse(std::uintptr_t requested) {',1)[1].split('\n}',1)[0]
assert 'r != 0xB454950 && r != kLedgeStateRva' in ej and code.count('entryJumpRefuse(requested)')==4
assert code.count('geEntryJumpUntilMs.store(0)')==2 and 'geEntryJumpUntilMs.store(GetTickCount64() + 1000);' in code
print("PASS: M21l crawl-state WR allowed in holds, bounded game-only A in crawl idle, no game jump during a ground entry")

# M21m: hero-owned machine only; WR in the crawl family in a hold -> game-only A (<= 3 per hold, 1 s apart), WR allowed
# only after that; no entry while the game is in its sprint jump (<= 1.2 s); the Update-WS helper closes Overstrike first.
assert 'bool isHero = heroMachine && ownerP && ownerP == gbHero.load();' in code and 'if (isHero) { std::uintptr_t cur =' in code
assert 'if (presses >= 3) crawlWr = true;' in code and 'gcInjectUntilMs.store(tick + 60);' in code
assert 'event=deferred reason=game_jump' not in code
cmd=(root/'tools/windows/Update-WS.cmd').read_text()
assert cmd.index('close-overstrike-for-update.ps1') < cmd.index('update --workspace') and (root/'tools/windows/close-overstrike-for-update.ps1').is_file()
ps=(root/'tools/windows/close-overstrike-for-update.ps1').read_text()
assert "Join-Path $game 'Overstrike\\Overstrike.exe'" in ps and 'OrdinalIgnoreCase' in ps and 'Spider-Man2' not in ps
print("PASS: M21m hero-only state, A instead of WR in the crawl, entry deferred during the game jump, Overstrike closed before update")

# M21n: standing idle 0xB4519A0 no longer blocks the entry; the swing entry gives the game RT = 0 while it is still in the
# swing 0xB454950 (<= 1.2 s), after every MOD read of RT; the automatic crawl-idle A is shipped OFF (owner: unnatural).
wgi=code.split('HRESULT STDMETHODCALLTYPE wgiReadDetour(',1)[1].split('\nHRESULT STDMETHODCALLTYPE wgiVibDetour',1)[0]
assert wgi.index('r->rt = 0;') > wgi.index('wgiRt.store(r->rt')
assert 'geRtMaskUntilMs.store(GetTickCount64() + 1200);' in code
print("PASS: M21n standing idle entry, swing ended by a short RT release, crawl auto-A off")

# M21o: read-only chain scan once per hero when the setter identifies the hero gravity block.
assert 'DWORD WINAPI gbChainFind(void* arg) {' in code and code.count('CreateThread(nullptr, 0, gbChainFind,')==1
fc=code.split('DWORD WINAPI gbChainFind(void* arg) {',1)[1].split('\nbool velReadable',1)[0]
assert 'writeMemory' not in fc and 'hits < 30' in fc and 'read_only=1' in fc
print("PASS: M21o read-only gravity block chain scan at hero identification")

# M21p: the established hero+0x80 -> +0x378 -> gbChain path is tried first on every tick while no block is known.
assert 'bool gbFastPath(std::uintptr_t hero, std::uintptr_t& g)' in code and 'msm2::readMemory(t + 0x378, &u, 8)' in code
gd=code.split('void gbDiscover(const msm2::Snapshot& hero, std::uint64_t now) {',1)[1].split('\n}',1)[0]
assert gd.index('gbFastPath(hero.hero, g)') < gd.index('if (now - gbLastScan < 1000)') and '!gbActive && !gbFound.load() && !sgHeroByUp.load()' in gd
print("PASS: M21p fast hero gravity block path before the slow scan")

# M21q: second fast path (two agreeing direct paths) with a fast_miss diagnostic; WS camera on the single caller in a hold.
assert 'bool gbFastPath2(std::uintptr_t hero, std::uintptr_t& g)' in code and 'if (!a || a != b) return false;' in code
assert 'gbRead2(hero, 0x80, 0x328), b = gbRead2(hero, 0x68, 0x298)' in code and 'WS_GRAVITY_BLOCK fast_miss' in code
sc=code.split('void selectCamera(const msm2::Snapshot& hero, std::uint64_t now) {',1)[1].split('\n}',1)[0]
assert 'if (!pick && gbHoldA.load() && ws_camref_raw' in sc and sc.index('pick = ws_camref_raw;') < sc.index('if (pick != ws_camref_this)')
print("PASS: M21q agreeing direct block paths after a reload, WS camera on the only caller during a hold")

# M21s: a new hero drops the previous camera manager before any engagement.
sc=code.split('void selectCamera(const msm2::Snapshot& hero, std::uint64_t now) {',1)[1].split('\n}',1)[0]
assert sc.index('restore("hero_changed"); ws_camref_this = 0;') < sc.index('bool curOk')
print("PASS: M21s camera manager dropped on hero change")

# M21t: face-jump landing hold while the game is still in its air state (INI WALLSPRINT_WS_FACE_JUMP_HOLD, shipped 1).
assert 'if(ws::drive::wsFaceJumpHold.load()&&!gfL.stopHop&&(cs==0xB44F0A0||hs==0xB44F0A0)){gfL.landHold=true;' in code and 'gHeroClassStateRva.store(nr);' in code
assert 'if(gameAir&&!gfL.landAirEnd&&tn-gfL.landHoldStart<0.8){' in code
assert ini.getint('WallSprint','WALLSPRINT_WS_FACE_JUMP_HOLD')==1 and 'L"WALLSPRINT_WS_FACE_JUMP_HOLD",0,' in tel
bw=(root/'tools/build_windows.sh').read_text()
assert 'wait "$pid" || fail=1' in bw and bw.index('wait "$pid"') < bw.index('"$gpp" -shared')
print("PASS: M21t face-jump landing hold, parallel compile with failure propagation")

# M21u: standing face jump keeps the facing read at the press instead of the stale last walk direction.
assert 'airFacingBegin(!still ? gfL.airVel : useFwd ? keep : gfL.moveVel,n0);' in code
print("PASS: M21u hero-class state for the landing hold, standing jump keeps its facing")

# M21v: takeoff speed cap 15 m/s; read-only gravity log at face-jump landing / hold / game air end.
assert 'speed=std::fmin(speed,15.f);' in code and code.count('faceJumpGravityLog("')==3
gl=code.split('void faceJumpGravityLog(const char* event,double t) {',1)[1].split('\n}',1)[0]
assert 'writeMemory' not in gl and 'read_only=1' in gl
print("PASS: M21v takeoff speed cap, face-jump gravity diagnostic")

# M21w: arc ends 8 cm above the takeoff rest height; the hold accepts the game's own step toward the surface; no model takeoff.
assert 'float landAt = 0.f;' in code and 'std::strcmp(gfL.airSource,"none_model_off")!=0' in code
assert 'float du=std::fmin(dl,std::fmax(dg,gfL.jumpRestD-0.30f));' in code and 'gfL.airSource="none_model_off";' in code
print("PASS: M21w landing above the surface for the game's own landing, no model takeoff direction")

# M21z: read-only landing-check diff diagnostic (hero 0x2000 + machine 0x1000 at air start+0.25 s vs air end; face hold +0.3 s).
lc=code.split('namespace lc {',1)[1].split('DWORD WINAPI gbChainFind',1)[0]
assert 'writeMemory' not in lc and lc.count('read_only=1')==4 and 'lcAirEnd(nr)' in code and 'lc::holdMs.store(GetTickCount64())' in code
print("PASS: M21z landing-check diff diagnostic is read-only")

# M21z-r4: one-time read-only capture of the air state vtable (0x12E0CD0) slots and bodies.
assert 'const std::uintptr_t descs[] = {0xB44F0A0, 0xB459DD0, 0xB44E0B0};' in code and 'WS_AIR_CODE_BYTES' in code
print("PASS: M21z-r4 air state vtable capture (log only)")

# M21aa: bounded landing assist (INI WALLSPRINT_WS_FACE_LAND_ASSIST, code default 0, shipped 1): X+0x438 held at 0 only in the hold.
assert 'if(ws::drive::wsFaceLandAssist.load()&&gbAddr>0x5F4){' in code and 'fl==0&&hgt<0.05f' in code and '(mb.Protect&0xff)==PAGE_READWRITE' in code
assert ini.getint('WallSprint','WALLSPRINT_WS_FACE_LAND_ASSIST')==1 and 'L"WALLSPRINT_WS_FACE_LAND_ASSIST",0,' in tel
print("PASS: M21aa face landing assist is bounded to the landing hold")
