# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until the required runtime gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve `AGENTS.md` and `docs/DEV_RULES.md`, exact-build/fail-closed behavior, independent implementation, and Nexus-compatible distribution constraints.

## Accepted orientation state
Planar Surface Up/orientation is accepted. Human c12 A/E preserved it through verified Deinit with hard kill 0 and `Position_writes_by_mod=0`.

A/cardinal: 4266 ms, retention pass, final Up dot surface=1.
E/diagonal: 4297 ms, retention pass, final Up dot surface≈1. E had one 172 ms native Update gap with a 72 ms stale suspend/resume and recovered normally.

## Verified movement chain
```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
exact callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```
The direct call is authenticated and correlates with real Hero translation on A and E. Path A Position remains denied.

## c12 actual-game verdict
Diagnostic PASS / exact missing edge. Both surfaces ended with `first_unsupported_offset=0xA`, `source_capture_ready=0`, `true_entry_snapshot=0`, `caller2_direct_relation_verified=0`, and `writable_source_proven=0`. See `analysis/V01C12_RUNTIME_RESULT.md`.

## c13 implementation handoff
```text
branch: codex/v01c13-writer-entry-snapshot
implementation commit: eb257e340be347d6ac17358efeb61eaa6a83066e
published delivery: 77c8a072c5401ced0dd427e764c142ab0780bf76
publication refresh head: f3b34fc7a9b011fb4076c28c4134e3492246ce06
version: v0.1c13 / package 0.1.31
```

C13 implements a narrow diagnostic entry detour for `0x2CFC900`, true pre-body GPR/XMM/stack snapshots, explicit invocation-to-hardware-store pairing, bounded entry-source candidates, and conservative caller2 runtime classification. The detour is gated to the exact accepted build and active verified Hero/WallRun Position-watch context; guarded reads remain bounded and do not change page protection. It does not write Position, movement source, input, movement, gravity, adhesion, animation, or camera.

Cloud validation reported by Codex:
```text
Windows x64 warnings-as-errors: PASS
Portable ASan/UBSan: 13 PASS
DLL smoke: 4 PASS
Full hooks/native/offline: 58 PASS / 4 historical private-input SKIP
Unique total: 75 PASS / 4 SKIP
```
Owned fixtures include concurrent/reentrant invocation pairing and cleanup. Final composed orientation/Position-watch fixtures retained verified Deinit, hard kill 0, clean debug restoration, and Position writes 0. These fixtures do not prove actual-game c13 source semantics.

GitHub publication recovered after credential reconnection. A fresh HTTPS clone verified the published branch and release ZIP bytes against release metadata. The final publication refresh does not change the validated c13 code.

## Current evidence boundary
Actual-game c13 installation, true entry/caller/store/candidate association, A/E stability, hitch/crash behavior, and cleanup remain PENDING.

```text
writable_source_proven=0
Position/source/input/movement writes=0
```
A complete four-store invocation map is not yet a producer-lifetime, downstream-conflict, exclusive-effect, or writable-source certificate. Do not infer velocity, displacement, direction, or root motion from arithmetic/correlation alone.

## Current stage
1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅ COMPLETE
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact `0x351EEAB -> 0x2CFC900` callsite ✅
7. c12 source-lineage diagnostic ✅ exact blocker identified
8. c13 true writer-entry diagnostic implementation ✅ cloud validated
9. c13 actual-game A/E entry-source gate ▶ CURRENT
10. safe writable upstream movement source ⬜ NOT AUTHORIZED
11. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
12. adhesion/contact refinement ⏸ deferred
13. animation/camera/later traversal ⬜

## Current action
Run `WallSprint_v0.1c13_writer_entry_snapshot.zip` in the actual game with ordinary A/cardinal WallRun for ~4 s, fully end WallRun, then E/diagonal for ~4 s in the same log. Video is optional unless a visible anomaly occurs. Adjudicate the private startup-through-cleanup log before defining any c14 implementation.