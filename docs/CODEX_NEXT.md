# Next task — validate v0.1c45 GravityOrientationOwner presence

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/REQUEST_PRESENCE_C44_ZERO_HIT_20261031.md`, `analysis/MOTION_GRAVITY_CURRENT_CONTRACT_REPORT.md`, and current `docs/CODEX_RESULT.md`.

## Accepted c44 result

Do not repeat RequestSimpleMotion presence testing.

Actual-game `v0.1c44 / package 0.1.69` fully admitted the exact Request entry and continuously persisted 335 worker batches across ~169 seconds. Every batch retained 0 records with overflow=0/contention=0/session_retained=0 while eight verified WallRun sessions occurred, including session #8 at 17.240649 s / 1377 Update calls. This is bounded zero-hit evidence for the exact Request entry in the tested ordinary movement/WallRun sequence.

The Request route is stopped as a vanilla locomotion observation source.

## Current implementation

`v0.1c45 / package 0.1.70` is published on the active branch.

C45 reuses the c44 runtime-proven observer/streaming architecture and changes only the active native target contract to `GravityOrientationOwner`:

- begin RVA `0x134EF00`;
- end RVA `0x134EFBB`;
- whole-function SHA256 `bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53`;
- exact `.pdata` UnwindData RVA `0x94A1CB8`;
- accepted prefix `48 89 5c 24 08`, 5 bytes / one instruction;
- accepted prefix SHA256 `0666a96c5b537b54fb43e8d983f54c516a379f17f302825c7f047c7e9ff4cab0`.

Unchanged safety contract:

- exact target-build gate;
- exact whole-function/pdata/prefix/trampoline/relay checks;
- split observer unwind validation;
- callback has no native call / I/O / lock / allocation;
- 16-record hot buffer;
- 500 ms worker drain;
- 65536 retained-session cap;
- automatic capture on admission;
- no F11/F12 and no post-test waiting;
- opaque RCX/RDX/R8/R9 / return PC / thread token / timestamp only;
- no pointer dereference or semantic field labels;
- no SetGravityVector hook or MOD-originated call;
- no gravity/orientation/adhesion/movement/native-source write;
- F8 remains emergency close.

Internal `request_presence` symbols and `REQUEST_PRESENCE_*` log keys remain intentionally reused to minimize code change. C45 records include `target_role=GravityOrientationOwner`.

## Next owner action

1. Run installed `MSM2-WS/Update-WS.cmd` with game/shared Overstrike closed.
2. Require the complete build/test/smoke/hooks/package/deploy PASS for c45.
3. Only after that PASS, install mods and run one short actual-game sequence:
   - ground Sprint straight -> reverse -> neutral;
   - ordinary WallRun -> left -> right -> down;
   - no F11/F12; no post-test wait.
4. Return newest `MSM2-WS-v0.1c45-*.log` and the short video.

## Runtime acceptance question

Determine only whether persisted `REQUEST_PRESENCE_RECORD target_role=GravityOrientationOwner` records exist and whether their timestamps overlap ground-Sprint and/or verified WallRun intervals.

If zero records persist across the complete bounded session while periodic batches and verified gameplay are present, accept that as bounded zero-hit evidence for this exact owner entry and stop this route.

If records exist, first establish call frequency/thread/return-PC grouping and correlation to ground Sprint vs WallRun. Do not interpret RCX/RDX/R8/R9 semantics or proceed to SetGravityVector/field writes until ChatGPT reviews the runtime evidence.

Do not broaden into another target, gravity setter hook, movement write, or generic reverse-engineering sweep before this runtime result.
