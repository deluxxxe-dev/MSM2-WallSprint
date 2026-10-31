# Next task — validate v0.1c44 streaming RequestSimpleMotion runtime capture

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/REQUEST_PRESENCE_C43_RUNTIME_PERSISTENCE_FAILURE_20261031.md`, current `docs/CODEX_RESULT.md`, and the accepted Request audit/runtime evidence.

## Accepted evidence

Actual-game c43 proved the exact RequestSimpleMotion site can be admitted on the pinned build: whole-site SHA/prefix/stolen span/exact `.pdata` (`0x11A1BD0..0x11A1CA2`, UnwindData `0x94697E0`), all split observer unwind entries, MinHook trampoline and final `REQUEST_PRESENCE_ADMISSION admitted=1` all passed. Do not repeat identity/unwind discovery unless a new concrete mismatch appears.

C43 persistence failed because records were emitted only at phase finalization; the supplied short normal session ended with no Request record/phase/session output. Absence of c43 records is NOT zero-call evidence.

## Current implementation

`v0.1c44 / package 0.1.69` preserves the admitted read-only hook and changes persistence only:

- automatic capture immediately after admission;
- no F11/F12 and no post-test waiting;
- callback buffer cap 16;
- worker-side drain every 500 ms;
- drain closes the capture gate, waits for active callbacks to quiesce, serializes completed opaque records plus loss counts, resets, then re-arms only if shutdown has not begun;
- retained session cap 65536;
- already-written periodic batches do not depend on shutdown flush;
- normal stop only attempts the final partial batch;
- all gameplay/orientation/adhesion/movement/native-source writes remain OFF;
- no captured-pointer dereference, native game call, argument edit or request suppression.

## Next owner action / acceptance

Do not make another implementation change before the owner pipeline/runtime result unless the build exposes a concrete issue.

1. Owner runs installed `MSM2-WS/Update-WS.cmd` with game/shared Overstrike closed.
2. Require the complete build/test/smoke/hooks/package/deploy PASS for c44.
3. Owner installs mods and runs the same short sequence: ground Sprint straight -> reverse -> neutral -> ordinary WallRun -> left -> right -> down. No F11/F12 and no extra waiting; close normally when finished.
4. Return newest `MSM2-WS-v0.1c44-*.log` (and video for correlation).

Runtime acceptance requires, in order:

- pinned build accepted;
- `REQUEST_PRESENCE_ADMISSION admitted=1`;
- `REQUEST_PRESENCE_CAPTURE_START ... periodic_worker_drain_ms=500 batch_record_cap=16`;
- at least one `REQUEST_PRESENCE_BATCH_SUMMARY` written during the run;
- actual `REQUEST_PRESENCE_RECORD` lines written before game exit;
- inspect overflow/contention and correlate opaque argument patterns to video only after the above pass.

If c44 fails, fix only the concrete build/runtime failure. Do not broaden into gravity/setter/gameplay writes, new ABI assumptions, adjacent-target exploration or another generic audit. Keep SocialFeed and every other MOD repository untouched.
