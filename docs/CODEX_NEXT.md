# Next task — repair c63 owner-blocked terminal provenance observer

## Status

Accepted owner c62 remains **VERIFIED-STATIC** and unchanged.

The published c63 owner run is **BLOCKED** before producing an accepted c63 JSON:

```text
c63 unchanged c62 terminal observer replay: 20480/162472
POST_C61_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED: terminal POST not justified by original c62 state
```

Canonical analysis:

- `analysis/V01C63_OWNER_OFFLINE_BLOCKED.md`
- `source/analysis/V01C63_OWNER_OFFLINE_BLOCKED.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Current bounded task

Repair **c63 observer/provenance modeling only**. Do not change c62 behavior, c62 admission, facts, candidates, caps, root outcomes, accepted c62 owner counts, or any historical evidence.

### Static problem to resolve

`slot_publication_post_c61_boundary.py::capture_for()` currently accepts a c62 `POST` edge callback only if the current key is already in `tx.post_c61` or `view.c61_source_keys`.

Unchanged c62 `View.edge()` can also convert an underlying c60 POST / c61 record lookup-or-full-prefix mismatch to `uncertified_post_c61_runtime_boundary` after a prior c61 authorization exists, while the current key is outside both sets. The private owner scan reached such a shape. The exact private root/subtype remains UNKNOWN.

Do not weaken c62. Instead make c63 observe and bind the exact c62 conversion provenance that already exists.

### Required behavior

For every edge-sourced c62 `POST`, c63 must be able to distinguish at least:

1. current context already post-c61 reachable;
2. current context is a previously used c61 source key on a later scan/instruction;
3. c62 converted an underlying c60 POST / c61 lookup or full-prefix mismatch after prior c61 authorization, with current key outside both sets.

Bind the actual root/current row/state/path/source and a deterministic terminal-reason/provenance field. Do not relabel case 3 as post-c61 reachability when it is not.

If c62 terminates by invalidation/propagation without an edge callback and no unique source instruction exists, c63 must remain fail-closed/BLOCKED rather than inventing a PC.

Keep the intended c63 domain exactly the accepted c62 `post_c61_uncertified_boundary_refusals = 2923`. Preserve complete c62 output/candidate bytes and second ordered replay requirements.

No edge admission, new Fact, new candidate, recursive certificate, qword read, pointer chase, ABI inference, EH/alternate-entry inference, producer traversal, mitigation change, production/gameplay write, native probe, Update-WS, game, or Overstrike.

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false; C52 FAIL remains binding.

## Tests

Focused command:

```bash
python3 source/tools/test_slot_publication_post_c61_boundary.py
```

Expected: all focused groups PASS, exit 0.

Add/retain owned fixtures proving:

- existing post-c61 reachable edge-source terminal is classified;
- later scan/instruction from an existing c61 source key is classified;
- converted c60 POST / c61 lookup-or-full-prefix mismatch after prior c61 authorization is classified even when current key is neither `post_c61` nor `c61_source_keys`;
- each conversion subtype preserves exact root/state/path/source and deterministic reason;
- invalidation-only/no-edge-source terminal still BLOCKS;
- no c62 state/counter/candidate mutation;
- complete expected 2923-root aggregate still requires one authenticated record per root and second replay;
- stale/wrong source/path/reason/provenance/evidence poisons aggregation;
- all historical gates remain false.

Then run the same required regression/build/package suite used by the published c63 task. Record exact commands, expected results, actual results, branch/commit/push verification, and any unrun test with reason in `docs/CODEX_RESULT.md` per `docs/CODEX_REPORTING.md`.

## Required owner handoff

After implementation is pushed and independently verified, provide one owner OFFLINE rerun command only. It must reuse existing c56 checkpoints and accepted c57-c62 JSONs read-only and output:

```text
logs/MSM2-WS-v0.1c63-post-c61-runtime-boundary.json
```

No evidence editing/truncation/regeneration unless a separately proven stale/missing input is found. No game/runtime operation.
