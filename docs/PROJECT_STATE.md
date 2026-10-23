# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Architecture is surface-relative locomotion, not an extended vanilla WallRun. Do not advance to camera until planar v0.1 locomotion is robust.

## Exact target / fail-closed gate
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve `AGENTS.md` and `docs/DEV_RULES.md`; no private game bodies/raw user logs in public source/package; implementation remains independent and suitable for Nexus Mods distribution.

## Verified orientation state
Planar Surface Up/orientation is accepted from the human c10/c11 A/E runs. c11 A/cardinal (4125 ms) and E/diagonal (4063 ms) both reached verified Deinit with `retention_pass=1`, no hard kill and `Position_writes_by_mod=0`. c12 does not modify the c10/c11 orientation admission/hot paths.

## Verified c11 movement diagnostics
The human c11 A -> E run established:

```text
writer_function_rva=0x2CFC900
Position stores=0x2CFC936 / 0x2CFC93C / 0x2CFC942 / 0x2CFC945
Transform destinations=+0x30 / +0x34 / +0x38 / +0x3C
caller return=0x351EEB0
caller2 return≈0x351A020
```

This chain correlates with real Hero translation on both surfaces. Path A `source+0x30` can match the Position row but is still denied because downstream writers and exclusive semantics are not proven. No c11 candidate reached `writable_source_proven=1`.

## c12 implementation handoff

```text
branch: codex/v01c12-movement-source-lineage
implementation commit: 4c84d634df4d629b237d4cc311d5e5cd6de4398c
published/delivery head: c966d233787504dda88f65d1825c5c1c408a2d67
version: v0.1c12 / package 0.1.30
```

Cloud implementation/diagnostic validation: PASS. Actual-game c12 lineage gate: PENDING HUMAN A/E RUN.

c12 adds read-only, bounded diagnostics for the measured `0x351EEB0 -> 0x2CFC900` chain:
- exact runtime function boundary/hash/full decode and Position-store expressions;
- exact direct E8 callsite verification and caller/caller2 attribution;
- conservative caller-side symbolic source slice;
- bounded trap-time reconstruction/correlation using existing hardware-watch infrastructure;
- explicit unsupported-edge/reason reporting;
- no Position/source/input/movement/gravity/adhesion/camera/animation writes.

Validation reported by Codex:

```text
Windows x64 warnings-as-errors: PASS
Portable ASan/UBSan: 13 PASS
DLL smoke: 4 PASS
Full hooks/native/offline: 53 PASS / 4 historical private-input SKIP
Total unique: 70 PASS / 4 SKIP
```

Owned fixture orientation regressions remained clean through Deinit with Position writes 0. These fixtures are not actual-game c12 source-lineage evidence.

## c12 proof limits
c12 intentionally does NOT authenticate a writable locomotion source in cloud validation:

```text
entry_snapshot_captured=0
source_identity_stable=0
writable_source_proven=0
```

Still unproven until the actual game supplies the new c12 records:
- exact actual-game writer/caller source map and surviving aliases;
- true entry value where post-store observation cannot recover it;
- typed meaning of Position/delta/dt/direction/speed/root-motion candidates;
- W semantics;
- source lifetime/ownership;
- downstream and exclusive intended translation effects.

Arithmetic resemblance alone must not be promoted to velocity, displacement, direction, root motion or a writable contract.

## Deferred visual alignment
The user observed body/root lean toward apparent ground and excessive apparent wall standoff/floating feet. These remain later visual/contact tasks and are not grounds for active adhesion now.

## Current stage
1. WallRun lifecycle / surface data / SurfaceFrame ✅
2. Path A/B orientation ownership and stabilization ✅
3. planar A/E orientation milestone ✅ COMPLETE
4. bounded Hero Position writer attribution ✅ c11
5. SurfaceFrame translation decomposition ✅ c11
6. c12 read-only lineage instrumentation ✅ IMPLEMENTED
7. c12 actual-game lineage gate ▶ CURRENT — human A then E log required
8. safe upstream writable movement source ⬜ NOT AUTHORIZED
9. surface-relative input rewrite ⬜ NOT AUTHORIZED
10. adhesion / visual contact refinement ⏸ deferred
11. animation / camera / later traversal ⬜

## Current implementation base
Use c12 delivery head exactly:

```text
branch: codex/v01c12-movement-source-lineage
implementation commit: 4c84d634df4d629b237d4cc311d5e5cd6de4398c
published head: c966d233787504dda88f65d1825c5c1c408a2d67
package: 0.1.30
```

## Current blocker
The next evidence must come from an actual-game c12 A/cardinal then E/diagonal run. Until those records are adjudicated, do not create a movement rewrite, input hook, adhesion system or camera task. A diagnostic PASS with an exact missing edge is acceptable; do not weaken gates to force `writable_source_proven=1`.
