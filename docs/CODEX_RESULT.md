# Codex result — WallSprint v0.1c1

Branch: **codex/v01c1-compact-path-b**. Implementation commit: **71437fac6c0640af01725a46e1b49c5627c739bb**. The branch preserves the tested b9 handoff and the checked-in b9 runtime conclusions. No main rewrite or force push.

## Changes

Package **0.1.19**, output **WallSprint_v0.1c1_compact_path_b_override.zip**.

- Added an accepted-build startup proof for the Path B writer at `0x2CFC3C0..0x2CFC57C`: exact 444-byte SHA, unwind boundaries, direct caller ending at `0x2CFC84B`, full-function instruction decode, and all nine compact RDX-to-padded-RCX copy pairs. A mismatch prevents the hook from installing.
- Added a coherent atomic publication containing session/generation/lifecycle epoch, current Hero/Transform/owner, native thread/update, elapsed/progress, shared Right/Up/Forward, and Surface N398. Path A and Path B read it without taking or trying the worker state lock. Lifecycle close, kill, stale publication, and concurrent publication change fail closed.
- Path A handles every exact qualifying `0x2CFC8F3 -> 0x2CFC580` call, supplies the shared padded basis, and preserves Position.
- Path B handles exact `0x2CFC84B -> 0x2CFC3C0` stack sources at `RDX=entry RSP+0x28`. It validates a proper compact basis and sane row scales, writes exactly 0x24 bytes, preserves all bytes at and after +0x24, and verifies the complete 64-byte block after the guarded write.
- Bounded diagnostics now report proof admission, lock-free Path A reads and counters, compact Path B commits with row lengths/metadata status, retention, and any hardware-observed reset after both overrides.
- Retention requires `dot(ActorUp,N398) >= 0.98` for 20 distinct native updates or 300 ms after both paths commit in the same active session.
- Version, build, packaging, README_TEST, worklog, analysis mirrors, and release metadata were updated for c1.

Direct Actor Transform, Position, adhesion, movement, animation, camera, physics, gravity, parkour, swing, and combat writes remain disabled. Private game-function captures and raw user logs are excluded from Git and release ZIPs.

## Validation

Completed with exit 0:

- Windows x64 warnings-as-errors build.
- Portable ASan/UBSan suite: 11 PASS.
- Windows DLL smoke suite: 4 PASS.
- Full Windows hook/offline suite: 27 PASS and 4 explicit optional private-fixture SKIPs.
- Targeted final regressions after the lifecycle-epoch guard: lock-free A/B MinHook fixture PASS; reset/hardware continuation suite PASS.
- Tests cover the compact-map proof and negative cases, exact caller/stack/target gates, 36-byte write boundary, row-scale and metadata preservation, a deliberately contended worker lock, shared A/B basis, A/E retention accounting, lifecycle/identity/thread/auth/kill/degenerate rejection, ABI state preservation, and `NEW_RESET_AFTER_DUAL_OVERRIDE`.
- Release validation checks version/INI, PE32+, exports/imports, no WriteProcessMemory, archive CRC/hash, source/licenses, and private-capture exclusions.

Synthetic fixtures validate implementation behavior only and never promote game evidence.

## Runtime status and next test

The c1 implementation gate is PASS. The accepted-build runtime gate is **PENDING** until the human A Control and E diagonal runs produce an authoritative game log.

Use README_TEST: A left-to-right WallRun about 1.5 s, then E left-to-right about 2 s. Expected visible behavior is a clear sideways rotation while the camera remains world-up. Runtime PASS is `ORIENTATION_RETENTION_PASS=1` on at least one wall with no crash or transform/position corruption. Return the complete WallSprint.log and note which wall visibly rotated. A `NEW_RESET_AFTER_DUAL_OVERRIDE` line identifies the next reset writer; it does not authorize direct Actor spam.

## Handoff

The final delivery commit contains this result, refreshed root `WallSprint.script` and `BUILD_MANIFEST.json`, both c1 ZIPs, and release hashes. The published branch/ref is verified after push.
