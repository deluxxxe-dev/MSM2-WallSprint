# Result — WallSprint v0.1c44 streaming Request presence diagnostic

2026-10-31. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C43 ACTUAL-GAME HOOK ADMISSION VERIFIED / C43 PERSISTENCE DESIGN FAILED / C44 STREAMING CORRECTION PUBLISHED / READ-ONLY CONTRACT PRESERVED / FULL OWNER PIPELINE PENDING / C44 ACTUAL-GAME RUNTIME PENDING.**

## C43 ACTUAL RUNTIME RESULT

The owner installed and ran `v0.1c43 / package 0.1.68` on the pinned target build and supplied `MSM2-WS-v0.1c43-20261031-221920.log` after completing the requested ground Sprint -> reverse -> neutral -> ordinary WallRun left -> right -> down sequence.

The exact target build was accepted (`Spider-Man2.exe` 2.810.0.0, on-disk SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`). RequestSimpleMotion admission fully passed:

- whole-site SHA / prefix / stolen-span gates: PASS;
- exact runtime-function `.pdata`: PASS (`0x11A1BD0..0x11A1CA2`, UnwindData `0x94697E0`);
- all split observer/restore unwind entries: PASS;
- MinHook trampoline validation: PASS;
- final `REQUEST_PRESENCE_ADMISSION admitted=1`;
- automatic capture start: PASS.

This is actual-game proof that the current exact Request hook site/admission mechanism can be installed on the pinned build.

However c43 serialized Request records only when its single large phase finalized. The supplied session exited before the 5-minute phase deadline and the log has no `REQUEST_PRESENCE_RECORD`, no phase/session summary, and no `WallSprint exited reason=TelemetryStopped` marker. Therefore the expected graceful finalization path was not observed before process/log termination. The missing records do **not** prove zero Request calls; c43's persistence/lifecycle design was unsuitable for normal short user tests.

The independent verified WallRun hooks recorded one complete main session: Init=1 / Entry=1 / Update=1177 / Deinit=1, duration approximately 15.401750 s (`13:21:43.367Z` to `13:21:58.769Z`). This proves the requested WallRun gameplay occurred while c43 capture was active, but no Request argument semantics can be inferred because no Request records were persisted.

Detailed evidence: `analysis/REQUEST_PRESENCE_C43_RUNTIME_PERSISTENCE_FAILURE_20261031.md`.

## C44 IMPLEMENTATION

Version is now `v0.1c44`, package `0.1.69`.

The c43-admitted hook identity and all read-only gameplay gates are preserved. Capture persistence changes as follows:

- capture auto-starts immediately after exact Request admission;
- capture lifetime is the script session, not a 5-minute wait-to-finalize phase;
- hot callback buffer is bounded to 16 records;
- worker drains every 500 ms;
- each drain closes only the capture gate, waits for active callbacks to quiesce, logs the completed batch, then resets/re-arms;
- overflow and contention are logged per batch;
- retained session cap is 65536 records;
- prior periodic batches are already serialized and do not depend on shutdown flush;
- normal stop still attempts the final partial batch;
- an Interlocked close-request flag prevents a periodic worker drain from re-arming capture after shutdown/F8 starts;
- abrupt process termination can lose only the currently open <=500 ms batch; prior batches have already been written;
- F11/F12 remain unnecessary and disabled.

Gameplay write state remains:

- `WALLSPRINT_ENABLE=0`;
- `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`;
- orientation write OFF;
- adhesion write OFF;
- movement write OFF;
- captured Request pointer dereference OFF;
- Request argument edits/suppression OFF.

## STATIC / PACKAGE CONTRACT

`source/tests/request_presence_static.py` now requires:

- production begin/end/SHA/UnwindData exact equality with accepted audit constants;
- `kBatchRecordCap = 16`;
- `kDrainIntervalMs = 500`;
- `kSessionRecordCap = 65536`;
- ASM cap `cmp eax, 16` and no old 131072 cap;
- periodic worker drain and stop drain call sites;
- synchronized close-request gate preventing shutdown re-arm;
- no old 300000 ms / 131072 phase constants;
- c44 / package 0.1.69;
- both INIs, both READMEs and package manifest metadata synchronized to no-wait streaming capture.

The existing native observer invariants remain checked: no native `call` in the assembly observer, no XMM/YMM/ZMM modification, GPR/flags preservation, split unwind entries and tail forwarding.

## FILES CHANGED FOR C44

- `source/src/msm2/request_presence.cpp`
- `source/src/msm2/request_presence.S`
- `source/tests/request_presence_static.py`
- `source/include/version.hpp`
- `source/mod/info.json`
- `source/mod/WallSprint.ini`
- `source/mod/presets/request_presence_read_only.ini`
- `source/tools/package.py`
- `README_TEST.md`
- `source/README_TEST.md`
- `analysis/REQUEST_PRESENCE_C43_RUNTIME_PERSISTENCE_FAILURE_20261031.md`
- `docs/CODEX_RESULT.md`

No SocialFeed or other MOD repository/file is in scope.

## VALIDATION STILL REQUIRED

The c44 source is published, but the normal owner Windows/WSL `Update-WS.cmd` full build/test/package/deploy pipeline has not yet been run against c44. Do not claim c44 build/package/runtime success yet.

Next action:

1. Run `Update-WS.cmd` with the game/shared Overstrike closed and require the full pipeline PASS.
2. Install mods and run the game normally.
3. Perform the same short gameplay sequence; no F11/F12 and no post-test wait.
4. Close the game normally when the sequence is complete.
5. In the returned c44 log, require `REQUEST_PRESENCE_ADMISSION admitted=1`, `REQUEST_PRESENCE_CAPTURE_START`, at least one `REQUEST_PRESENCE_BATCH_SUMMARY`, and actual `REQUEST_PRESENCE_RECORD` lines written before game exit.
