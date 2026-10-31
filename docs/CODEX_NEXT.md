# Next task — replace Request presence target with GravityOrientationOwner presence

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/REQUEST_PRESENCE_C44_ZERO_HIT_20261031.md`, `analysis/MOTION_GRAVITY_CURRENT_CONTRACT_REPORT.md`, current `docs/CODEX_RESULT.md`, and the accepted presence-hook audit.

## Accepted c44 runtime result

Do not repeat RequestSimpleMotion presence testing.

Actual-game `v0.1c44 / package 0.1.69` fully admitted the exact Request entry and continuously wrote 335 worker batches across ~169 seconds. Every batch retained 0 records with overflow=0/contention=0/session_retained=0. Eight verified WallRun lifecycle sessions occurred inside that interval; main session #8 lasted 17.240649 s with 1377 Update calls. Therefore the exact `0x11A1BD0` entry is valid bounded zero-hit evidence for the tested ordinary movement/WallRun sequence.

The Request route is stopped as a vanilla locomotion observation source. Do not enlarge buffers, extend capture duration, alter shutdown persistence, decode nonexistent arguments, search adjacent Request targets, or rerun the same game test.

## Current next question

Determine only whether the already accepted `GravityOrientationOwner` entry executes during ordinary ground Sprint and ordinary WallRun.

Accepted prior identity from the owner audit/current static resolution:

- begin RVA `0x134EF00` (`20246272`);
- end RVA `0x134EFBB` (`20246459`);
- whole-function SHA256 `bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53`;
- exact `.pdata` UnwindData RVA `0x94A1CB8` (`155851960`);
- accepted stolen prefix: 5 bytes, one instruction, original-prefix SHA256 `0666a96c5b537b54fb43e8d983f54c516a379f17f302825c7f047c7e9ff4cab0`;
- the accepted function contains the previously verified direct call at `0x134EF67` to the partial SetGravityVector target, but this task does not observe or call that setter.

## Implementation scope

Create the next read-only diagnostic version by reusing the c44 observer/streaming architecture and replacing only the observed native site/identity contract needed for GravityOrientationOwner.

Required behavior:

- exact target build guard unchanged;
- exact GravityOrientationOwner whole-function hash and `.pdata` identity required;
- validate the 5-byte whole-instruction stolen prefix against the accepted prefix identity;
- validate MinHook trampoline/relay/native restoration as c44 does;
- preserve split observer unwind validation;
- callback remains no-I/O / no-lock / no-allocation / no native game call;
- retain automatic capture on admission;
- retain bounded worker-side periodic persistence so short tests do not depend on shutdown;
- capture only opaque entry argument register bits / return PC / thread token / timestamp as needed for presence proof;
- no pointer dereference;
- no semantic field labels;
- no SetGravityVector call or hook;
- no gravity/orientation/adhesion/movement/native-source write;
- F8 emergency close remains available;
- F11/F12 remain unnecessary.

Prefer preserving the current 500 ms worker drain and small hot buffer unless a compile/static reason requires a change. Do not introduce a second simultaneous unknown target: RequestSimpleMotion observation should be disabled/replaced for this diagnostic rather than running Request and Gravity together.

Synchronize version/package/INI/readme/package metadata/static tests. Static tests must pin production Gravity begin/end/SHA/UnwindData/prefix identity to the accepted audit and reject stale Request-as-active-target constants.

## Completion condition

Codex performs build / full automated regression / smoke / hooks / package validation only.

Report only:

```text
STATUS: success / partial / blocked

CHANGED:
- files

RESULT:
- implementation result
- build / automated test result

NEED:
- Chat-side decision only if blocked
```

Do not write the user runtime procedure in the Codex response.

## Human runtime after successful publication

ChatGPT will ask for one short actual-game sequence only after the full pipeline/deploy succeeds:

- ground Sprint straight -> reverse -> neutral;
- ordinary WallRun -> left -> right -> down;
- no F11/F12 and no post-test waiting.

Runtime acceptance question is only whether `GravityOrientationOwner` produces persisted presence records, and in which gameplay interval. Do not interpret argument semantics until presence is established.
