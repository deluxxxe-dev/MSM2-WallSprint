# Next task — validate c71 owner provenance correction, then rerun owner classifier once

## Status

Corrected c70 owner OFFLINE result remains accepted as **VERIFIED-STATIC**.

The initial c71 focused suite passed, but the first real c71 owner run is **BLOCKED** and produced no c71 artifact.

Canonical detail:

- `analysis/V01C70_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C71_TOOL_READY.md`
- `analysis/V01C71_OWNER_BLOCKED_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Accepted c70 basis

```text
MSM2-WS-v0.1c70-post-c69-stitched.json
physical SHA256 2780ca89a98ad22d12e2252f53cd202830bc8228edd67d0b0c7fb8eccf7f15ac
semantic result identity 9b213db28b05f9d1fc8baefa37f981aa521cd7ee45110a7cfe6b122567f0b9e1
post_c69_uncertified_boundary_refusals = 102
```

The c71 purpose is unchanged: classify exactly those 102 terminal events without traversing them.

## First owner failure

Observed owner progress:

```text
c67 unchanged c66 terminal observer replay: 162472/162472
c70 full-replay c69 continuation: 26624/162472
POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED: unrecognized c70 edge terminal provenance
```

No c71 result JSON or c71 checkpoint existed after failure.

## Static diagnosis

Corrected c70 has three relevant ways an edge-backed post-c69 terminal can be observed:

1. current key is explicitly in `tx.post_c69`;
2. current key is an authenticated `view.c69_source_keys` later scan;
3. c69's first-terminal observer has already been authenticated/retired and a later distinct edge is routed directly to c70 POST even though the current key is in neither of the first two sets.

Initial c71 `_capture_edge` implemented only 1 and 2. The real owner run encountered 3.

This is a c71 diagnostic coverage bug, not evidence that accepted c70 is wrong.

## Correction contract

`source/tools/slot_publication_post_c69_boundary_fix.py` may recognize case 3 only when:

- an accepted/current `c69_record` is already present;
- the callback is not the same authenticated c69 source sibling;
- the exact current root/state/path/source instruction exists;
- complete architectural successors can still be classified under the existing relation vocabulary.

Record the origin explicitly as:

```text
phase  = retired_c69_first_terminal_observer_distinct_edge
reason = c70_retired_c69_first_terminal_observer_later_distinct_edge
post_c69 = false
used_c69_source = false
```

Do not relabel this as current post-c69 reachability. Preserve the active accepted c69 record identity and current context separately.

C71 remains diagnostic-only: no target traversal or admission.

## Immediate validation

Run only:

```text
source/tools/test_slot_publication_post_c69_boundary_fix_quick.py
```

Required PASS:

```text
PASS: c71 owner-observed retired c69 observer distinct edge is recorded without relabeling current reachability
```

Do not rerun the expensive owner replay unless this quick regression passes.

## Owner rerun after quick PASS

After quick PASS, run exactly one corrected c71 owner OFFLINE classifier against the exact accepted c56-c70 evidence.

The corrected owner result must:

- reproduce exactly 102/102 c70 post-c69 terminal roots;
- preserve edge-backed vs transaction origin;
- preserve the new retired-observer distinct-edge origin separately;
- classify complete architectural successor sets without traversal;
- emit `new_post_c69_boundaries_admitted = 0`;
- emit `new_cross_row_candidate_count = 0`;
- keep `live_qword_reads = 0`;
- keep all safety gates false;
- keep accepted c56-c70 evidence read-only.

If another unknown provenance form or evidence mismatch appears, stop and return only the observed fact. Do not widen the classifier by assumption.

## Change scope

Allowed:

- `source/tools/slot_publication_post_c69_boundary_fix.py`;
- focused/quick c71 test registration;
- c71 blocked/correction analysis and status docs.

Do not change accepted c56-c70 evidence semantics.

## Do not

- Do not traverse a c71 target.
- Do not define c72 continuation behavior before c71 owner classification is analyzed.
- Do not invent current post-c69 reachability for the retired-observer case.
- Do not invent transaction-terminal instructions or successors.
- Do not infer writer absence from zero candidates.
- Do not read slot qwords or chase pointers.
- Do not traverse the deferred producer by assumption.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed unless later independently proven and explicitly authorized.
