# V01C66 TOOL READY

Date: 2026-10-28 JST
Branch: `codex/v01c20-edi-value-provenance`
Evidence class: implementation/automated validation only; owner private-image result still UNKNOWN until the c66 offline scan is run.

## Result

`POST_C63_RUNTIME_BEGIN_STITCH_SCANNER_READY` (`v0.1c66`, schema 1).

C66 reconstructs/authenticates the accepted c56→c65 chain and continues across a c65-recorded terminal boundary only for the exact current root/state/path/source/full-prefix/phase/reason/provenance record. Every required architectural successor must independently be `exact_unique_runtime_begin` and pass the existing immutable/file-backed executable preflight. One bad successor aborts the whole root transaction. The next unapproved row-external boundary remains fail-closed as `uncertified_post_c65_runtime_boundary`.

Accepted c65 input remains:

- 998 boundary instances
- 1080 architectural edges
- 1021 exact unique runtime-begin edges
- 51 runtime-span interior edges
- 8 metadata-free executable edges
- 0 c65 candidates

The 1021 value is edge availability, not admissible instance/root count.

## Validation

Owner Windows/WSL validation completed after fetching the published branch.

Focused c66 test:

`python3 source/tools/test_slot_publication_post_c63_stitched.py`

Result: 13 focused c66 groups PASS, exit 0. Covered ordinary/direct/conditional/CALL-fallthrough continuation, all-successor admission and rollback, interior/free-RX refusal, source-local/dedup/converted provenance separation, stale owner evidence refusal, next-wave rollback, root/path separation and closed safety gates.

Regression scanner suite:

All 16 slot-publication/static-access commands through c66 PASS, including historical c38-c65 scanners and the new c66 scanner.

Build/integration/package:

- `bash source/tools/build_windows.sh`: PASS, exit 0 after the compatibility-header fix below.
- `bash source/tools/test.sh`: PASS, exit 0.
- `bash source/tools/smoke_windows.sh`: PASS, exit 0.
- `bash source/tools/test_hooks_windows.sh`: PASS, exit 0.
- `python3 source/tools/package.py`: PASS, exit 0; produced the existing validation package output `MSM2-WS-v0.1c38.script.disabled`.

No game/runtime/native owner test was performed or required for c66 tool validation.

## Windows/WSL build compatibility finding

The first owner build on the canonical `_source` clone under `/mnt/c/...` failed while compiling MinHook because `tools/compat/Windows.h` collided case-insensitively with MinHook's lowercase `<windows.h>` include. The repository MinHook files themselves were not corrupt.

WallSprint-owned compatibility shim `source/tools/compat/Windows.h` was changed to include the next real system Windows header rather than recursively resolving itself on case-insensitive WSL mounts. The owner repeated the build and the complete remaining validation suite successfully. No MinHook vendor source or gameplay/runtime behavior was modified.

## Safety

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false. C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding. No slot qword read, pointer chase, native probe, producer traversal, gameplay write, Update-WS, game or Overstrike use is authorized by this result. Zero candidates still does not prove writer absence.

## Owner next action

Run one c66 OFFLINE scan against the exact pinned `Spider-Man2.exe`, reusing existing c56 checkpoints and accepted c57-c65 JSONs read-only. Output only:

`logs/MSM2-WS-v0.1c66-post-c63-stitched.json`

The resulting JSON must be reviewed before any c67 task is chosen.
