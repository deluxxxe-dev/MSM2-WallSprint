# Next task — build and run c48 exact Update-entry `WallRun.this+0x250` threshold discriminator

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`, `analysis/WALLRUN250_V7_HANDOFF_RESULT_20261101.md`, `analysis/WALLRUN250_V8_CONSUMER_RESULT_20261101.md`, and current `docs/CODEX_RESULT.md`.

## Accepted evidence

Do not repeat RequestSimpleMotion, GravityOrientationOwner, V1/V2 global scans, V3-V7 producer-path traversal, or V8 offline consumer acquisition.

Accepted V8 static consumer facts:

- authenticated read `0x1424585` compares `this+0x250` against a file-backed initial `0.5f` and stores the comparison result with `SETB`;
- authenticated read `0x142460E` compares the same field against a file-backed initial approximately `0.71f` and `JAE` skips the following block when the finite field is <= that threshold;
- both are threshold/control consumers: VERIFIED-STATIC;
- field meaning, producer and writability remain UNKNOWN;
- dot-product/alignment interpretation remains HYPOTHESIS only.

Accepted c47 runtime:

- negative/near-zero `+0x250` pulses correlated with reversals/down oscillation;
- every retained 100 ms sample was below both V8 thresholds;
- short between-sample positive crossings remain unexcluded.

## Current c48 implementation

Runtime `v0.1c48`, package `0.1.73`.

No new native hook is added. c48 adds only a dedicated read-only gate/callback inside the existing authenticated WallRun Update thunk.

Admission:

- complete verified WallRun observation plan required;
- read live threshold globals at `0x5303DE8` and `0x5337098`;
- log whether live bits equal the V8 on-disk bits `0x3F000000` and `0x3F35C28F`;
- enable `ws_update_probe_enabled` only; keep `ws_write_dispatch_enabled=0`.

For each admitted Update thunk invocation before original Update:

- native CPU state is already preserved by the existing thunk;
- direct local `ws_update_probe_sample(owner)` callback performs one guarded 4-byte read of `owner+0x250`;
- count read success/failure;
- count field NaN vs non-NaN;
- count consumer-A true condition (`field > live threshold0` or unordered);
- count consumer-B fallthrough condition (`field > live threshold1` or unordered);
- no logging is performed on the native callback.

The dedicated probe callback occurs before the separate old write-dispatch check. Since c48 never enables `ws_write_dispatch_enabled`, the writer bridge / `ws_update_prepare`, caller unwind, argument clone, old write dispatch, and post callback are not entered by c48.

Worker-side `drainLogs` emits bounded cumulative/final records and rechecks live threshold globals.

All gameplay writes remain OFF. Direct Actor writer remains hard-disabled. No pointer dereference, game call, field write, gravity write or native-source write is authorized.

## Owner action

1. Close game and shared Overstrike.
2. Run installed `MSM2-WS\Update-WS.cmd`.
3. Require full updater PASS for `v0.1c48 / 0.1.73` including build/tests/smoke/hooks/package/deploy.
4. In shared Overstrike: `Install mods` -> `Run modded game`.
5. Perform one WallRun sequence: right -> reverse left -> reverse right -> hold down until the known short left/right oscillation -> end WallRun.
6. Close game normally.
7. Return only newest `MSM2-WS-v0.1c48-YYYYMMDD-HHMMSS.log`.

No F11/F12, markers, post-test wait or video is required unless behavior differs materially from c47.

## Runtime acceptance

Require:

- `wallrun_0x250_threshold_probe_admission complete_verified=1 admitted=1 update_probe_gate=1 write_dispatch_gate=0`;
- `v8_expected_threshold_bits_match=1`;
- cumulative/final `live_thresholds_unchanged_from_admission=1`;
- `read_fail=0`;
- no `hook_queue_dropped` increase during the selected session;
- for a run containing only the requested WallRun, compare exact-entry sample count with verified `Update=` count; a mismatch makes result partial;
- inspect `consumer_gt_threshold0_or_unordered` and `consumer_gt_threshold1_or_unordered`.

If both counters are zero with complete coverage, classify that the c47 reversal/down sequence did not activate either authenticated threshold consumer. Then stop treating the negative pulse timing alone as evidence that these gates cause reversal/down behavior.

If either counter is nonzero, preserve exact count/coverage and choose the next minimum discriminator; do not write the field yet.

If threshold bits do not match or live values change, treat c48 as partial and resolve only that mismatch before semantic conclusions.
