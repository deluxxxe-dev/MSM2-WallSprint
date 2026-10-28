# Codex result — c70 owner first run blocked, observer-retirement correction focused PASS; owner rerun next

2026-10-29 JST. Branch **codex/v01c20-edi-value-provenance**.

## Status

Corrected c69 owner OFFLINE result and its authenticated record checkpoint remain accepted as **VERIFIED-STATIC**.

C70 implementation is present. Its first owner OFFLINE run was **BLOCKED** by a c70 observer-retirement bug after reaching real post-c69 continuation. The bug has been corrected and the focused c70 fixture suite now passes including the exact owner-discovered failure class.

This is **not yet a c70 owner result** and is not runtime evidence.

Canonical detail:

- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C70_CHECKPOINT_SUFFICIENCY_BLOCKED.md`
- `analysis/V01C70_TOOL_READY.md`
- `analysis/V01C70_OWNER_BLOCKED_RESULT.md`

## Accepted c69 source evidence

```text
result file MSM2-WS-v0.1c69-post-c67-runtime-boundary.json
physical SHA256 e57b7f39ea476e356397f944d8aadb14841b27874736a60c1a82b982eb86537f
semantic result identity 08002e4c3aef2c7cfa037040b6f09f7d1b6e247d0b446daea1ffdb2569aa01fb
classifier identity 4f42b0d04346eb5c7b10c8fe0592edd3a3f19fcb463d66a0adf40be01fd25049
record-set SHA256 20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed

checkpoint file MSM2-WS-v0.1c69-authenticated-prefix-checkpoint.json
physical SHA256 d7b8dae21e9e0478e8ea5d8136c7f19156cd5afca2ff80f167dce48a2e61b6b5
checkpoint identity b734f8ab2f0a24e002c8b5c2ed1dbbfe75c9411cc773ec1394a1cf330c9b3a28
record count 191
```

C69 continuation domain:

```text
all records = 191
edge-backed = 190
transaction-only = 1
all-successor-exact = 177
continuable exact edges = 191
interior-containing records = 13
```

## C70 implementation

Files:

- `source/tools/slot_publication_post_c69_stitched.py`
- `source/tools/slot_publication_post_c69_stitched_fix.py`
- `source/tools/test_slot_publication_post_c69_stitched.py`

Identity:

```text
repository_result POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c70 / schema 1
contract exact_c69_full_replay_all_successor_continuation_v1
```

C70 performs the existing full historical transaction replay and authenticates the accepted c69 result/checkpoint at the first post-c67 terminal. It continues only complete all-successor-exact c69 records and stops at the next unknown post-c69 boundary.

The c69 checkpoint is **not** sufficient for exact mutable transaction resume. Therefore C70 explicitly reports:

```text
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

Do not claim replay acceleration.

## First owner BLOCKED result

The first c70 owner run reached:

```text
c70 full-replay c69 continuation: 26624/162472
POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: multiple distinct first terminal c68 source events
```

This did not invalidate accepted c68/c69 evidence. Static inspection showed that after c70 authenticates the accepted first c69 boundary and deliberately continues it, the inherited c69 diagnostic observer was still active. When the same root later reached the intended next unknown boundary, that observer treated the later edge as a second candidate for the original first c68 terminal and raised its historical single-terminal guard.

The correction is c70-only:

- before first c69 authorization, unchanged c69 first-terminal observation remains exact;
- after that accepted c69 record has been authenticated for the root, later distinct row-external edges bypass the retired c69 observer;
- the later edge is immediately refused as `uncertified_post_c69_runtime_boundary`;
- no later edge is classified/traversed as c69 evidence;
- accepted c69 bytes/identities and c68 semantics are unchanged.

## Focused validation result after correction

Final local focused run passed through:

```text
PASS: c70 production contract pins accepted c69 continuation-domain counts
PASS: c70 exact single-successor c69 terminal continues under unchanged full-root replay
PASS: c70 reports inherited c69 candidate totals without mutating accepted c69 result identity
PASS: c70 exact two-successor c69 terminal preflights both arms before admission
PASS: c70 later distinct edge bypasses retired c69 first-terminal observer and stops as post-c69 unknown
PASS: c70 interior c69 terminal remains wholly fail-closed
PASS: c70 rejects mutated c69 checkpoint record evidence
PASS: c70 transaction-origin c69 terminal validates then refuses without synthetic successor
PASS: c70 focused full-replay c69 continuation groups complete
```

The suite also reran the c68 focused continuation groups successfully.

Earlier focused failures corrected before this pass:

- c70 candidate-count reporting incorrectly expected top-level c69 candidate keys; the correction projects the accepted `historical_c68_candidate_counts` without changing c69 serialized bytes or identity.
- the synthetic transaction-origin fixture originally changed behavior only after its c68 baseline was built; the fixture now applies the same condition to baseline creation, c69 classification and c70 replay.
- the first owner run exposed inherited c69 observer lifetime beyond the first authenticated terminal; the correction retires only that diagnostic role after c69 authorization and preserves c70 fail-closed stopping at the next unknown boundary.

## Regression registration

`source/tools/test_hooks_windows.sh` now keeps the prior c69 checkpoint correction test and additionally registers:

```text
test_slot_publication_post_c67_boundary_fix2_quick.py
test_slot_publication_post_c69_stitched.py
```

The heavy full c69 fix2 replay fixture is not added to the hook chain; the quick transaction-origin smoke covers that correction without repeating the expensive historical replay.

## Safety

No gameplay/runtime behavior was changed.

```text
slot qword reads = 0
pointer chase = false
native/game probe = false
gameplay write = false
production integration = false
```

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed. Zero candidates never proves writer absence. SocialFeed and other MOD repositories were not changed.

## Validation still pending

- corrected c70 owner OFFLINE artifact generation/acceptance
- final aggregate build/test/smoke/hooks/package validation after owner acceptance

## Next

Rerun only the corrected c70 owner OFFLINE analysis against the exact accepted c56-c69 artifacts and exact executable. Analyze that result before defining c71 behavior.
