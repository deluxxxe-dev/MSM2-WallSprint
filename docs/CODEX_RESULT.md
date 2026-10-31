# Result — WallSprint v0.1c45 GravityOrientationOwner presence diagnostic

2026-10-31. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C44 ACTUAL-GAME ZERO-HIT ACCEPTED / REQUEST ROUTE STOPPED / C45 GRAVITY OWNER PRESENCE IMPLEMENTED AND PUBLISHED / OWNER FULL PIPELINE + ACTUAL-GAME RUNTIME PENDING.**

## C44 ACCEPTED RUNTIME RESULT

The owner ran `v0.1c44 / package 0.1.69` on the pinned target build and supplied `MSM2-WS-v0.1c44-20261031-224811.log`.

Runtime verified:

- exact target build accepted: Spider-Man2.exe `2.810.0.0`, SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- exact RequestSimpleMotion hook admission passed including whole SHA, 7-byte/2-instruction prefix, `.pdata` UnwindData `0x94697E0`, split observer unwind and MinHook trampoline;
- automatic capture and 500 ms worker persistence ran continuously;
- 335 periodic batches were written across the supplied session;
- all batches retained 0 records, overflow=0, contention=0, session_retained=0;
- no `REQUEST_PRESENCE_RECORD` existed;
- eight verified WallRun sessions occurred during the same capture interval;
- main WallRun #8 lasted `17.240649 s` with `1377` Update calls and surrounding Request batches still retained 0.

This is valid bounded zero-hit evidence for the exact Request entry during the tested ordinary movement/WallRun sequence. It supersedes c43's persistence ambiguity. The Request route is stopped as a useful vanilla locomotion observation source; do not repeat it or enlarge its capture.

Detailed evidence: `analysis/REQUEST_PRESENCE_C44_ZERO_HIT_20261031.md`.

## C45 IMPLEMENTATION

Version: `v0.1c45`; package: `0.1.70`.

C45 deliberately reuses the c44 runtime-proven observer, split unwind layout, automatic capture, 16-record hot buffer, 500 ms worker drain, 65536 retained-session cap, shutdown-independent prior-batch persistence and F8 close behavior.

Only the observed native target contract changes to the already accepted `GravityOrientationOwner` identity:

- begin RVA `0x134EF00`;
- end RVA `0x134EFBB`;
- whole-function SHA256 `bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53`;
- exact `.pdata` UnwindData RVA `0x94A1CB8`;
- stolen prefix `48 89 5c 24 08` = 5 bytes / one instruction;
- accepted prefix SHA256 `0666a96c5b537b54fb43e8d983f54c516a379f17f302825c7f047c7e9ff4cab0`.

The existing internal `request_presence` module/symbol names and `REQUEST_PRESENCE_*` log keys are intentionally retained to minimize code change. Every c45 runtime record includes `target_role=GravityOrientationOwner`, and config/README/package metadata state that the legacy `REQUEST_PRESENCE_DIAGNOSTIC` switch now activates this Gravity target.

No second native presence target runs simultaneously. RequestSimpleMotion active constants are removed from production observer identity.

## SAFETY CONTRACT

Unchanged / required:

- exact target-build gate;
- exact whole-function SHA and `.pdata` identity;
- exact 5-byte prefix and one-instruction stolen span;
- MinHook trampoline identity and relay verification;
- split observer unwind verification;
- callback has no native `call`, I/O, lock or allocation;
- GPR/flags preserve; no XMM/YMM/ZMM modification;
- captured RCX/RDX/R8/R9 remain opaque bits only;
- no captured-pointer dereference;
- no semantic field labels;
- no SetGravityVector hook or MOD-originated call;
- no gravity/orientation/adhesion/movement/native-source write;
- F11/F12 unnecessary; F8 remains emergency close.

## STATIC / PACKAGE CONTRACT

`source/tests/request_presence_static.py` now fails before the long regression suites unless:

- production Gravity begin/end/SHA/UnwindData match the accepted audit tuple/known exact identity;
- production prefix is exactly `48 89 5c 24 08` and hashes to the accepted Gravity prefix SHA;
- the stale c44 Request RVA/end/UnwindData/SHA/prefix are absent from the active production target;
- stolen-span expectation is exactly 5 bytes / 1 instruction;
- c44 streaming safety invariants remain present;
- assembly still has no native `call` or vector-register use and retains the 16-record cap/split unwind forwarding;
- both INIs/READMEs/info/package metadata identify c45 / 0.1.70 / GravityOrientationOwner;
- all gameplay write gates remain OFF.

`source/tools/audit_presence_hook_sites.py` now also pins `EXPECTED_GRAVITY_UNWIND_RVA=155851960 (0x94A1CB8)`. The c45 static test reads that accepted-audit constant and requires exact equality with the production begin/end/unwind/SHA tuple, preventing a one-sided UnwindData transcription error.

`source/tools/package.py` now emits package mode `gravity_owner_presence_read_only` and explicitly records `setter_hooked_or_called_by_mod=false`.

## CHANGED FOR C45

- `source/src/msm2/request_presence.cpp`
- `source/tests/request_presence_static.py`
- `source/tools/audit_presence_hook_sites.py`
- `source/include/version.hpp`
- `source/mod/info.json`
- `source/mod/WallSprint.ini`
- `source/mod/presets/request_presence_read_only.ini`
- `source/tools/package.py`
- `README_TEST.md`
- `source/README_TEST.md`
- `docs/CODEX_RESULT.md`
- `docs/CODEX_NEXT.md`

`source/src/msm2/request_presence.S` is intentionally unchanged from the c44 runtime-proven observer implementation.

No SocialFeed or other MOD repository/file is in scope.

## VALIDATION STILL REQUIRED

Chat/GitHub-side publication and source consistency do not substitute for the owner Windows/WSL build chain. Before game runtime testing, run installed `MSM2-WS/Update-WS.cmd` with game/shared Overstrike closed and require the complete build/test/smoke/hooks/package/deploy PASS for c45.

After that PASS, one short actual-game sequence is sufficient. The only runtime question is whether persisted records with `target_role=GravityOrientationOwner` occur during ground Sprint and/or verified WallRun intervals. Do not interpret the opaque arguments until presence is established.
