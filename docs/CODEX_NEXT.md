# Next task — c72 continuation through accepted c71 exact boundaries

## Status

Corrected c71 owner OFFLINE result is accepted as **VERIFIED-STATIC**.

Canonical result:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Accepted c71 basis

Uploaded accepted checkpoint:

```text
MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
physical SHA256 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
size 1324640 bytes
checkpoint identity 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record stream SHA256 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
record count 102
```

Embedded c71 result:

```text
repository_result POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool v0.1c71 / schema 1
contract exact_c70_first_terminal_post_c69_boundary_full_prefix_provenance_diagnostic_v1
semantic result identity 18d61be59032514a4c74c9b015f422c379e2c05050e2d5819c1025048a2276d7
classifier identity 18cba768c4261886b70496e7f73abe3fe5276b44aecadb0d929802ae3dae9fd5
```

The standalone c71 result file was emitted by the owner run but its raw bytes were not separately uploaded. Its deterministic expected `atomic_json` representation is 424663 bytes / SHA256 `db524d81d101da4173bbdda802a6bbe65a0d1801192b37b6261ecf4ce3c37ada`. For c72 production authentication, pin the uploaded c71 checkpoint physical SHA and its embedded result/record identities; do not weaken authentication to filename-only evidence.

Exact executable remains SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.

## Measured c71 domain

```text
expected/classified terminal roots = 102 / 102
edge-backed = 102
transaction = 0
architectural successor edges = 109
model-only successor edges = 0
```

Provenance forms:

```text
used_c69_source_on_later_scan = 90
post_c69_reachable_context = 3
retired_c69_first_terminal_observer_distinct_edge = 9
```

Successor relations:

```text
exact_unique_runtime_begin = 102
inside_unique_runtime_span_nonentry = 6
executable_file_backed_outside_runtime_metadata = 1
```

Complete successor groups:

```text
all_successors_exact_runtime_begin = 95
contains_interior_successor = 6
other_relation_set = 1
```

The 95 all-successor-exact terminal instances contain exactly 99 exact architectural successor edges: 91 single-edge records and 4 two-edge records.

## Purpose

Implement c72 as the smallest OFFLINE/STATIC full-historical-replay continuation stage that reproduces accepted c71 exactly, authenticates each c71 terminal record against the current root/state/path/source/provenance, and conditionally continues only the 95 records whose complete architectural successor set is `exact_unique_runtime_begin`.

Recommended identity:

```text
repository_result POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c72 / schema 1
contract exact_c71_full_replay_all_successor_continuation_v1
```

Recommended next-boundary refusal label:

```text
uncertified_post_c71_runtime_boundary
```

## Required replay model

Use full historical replay. The accepted c71 checkpoint is an authenticated diagnostic record stream, not a mutable transaction resume snapshot.

Require:

```text
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

Do not reset historical transaction counters, graph/cycle state, provisional candidates, path proofs, or authorization state at c71.

## C71 authentication

Before any c71 continuation, require exact:

- executable SHA256;
- accepted c71 checkpoint physical SHA256;
- checkpoint identity;
- checkpoint contract/version;
- c71 embedded result semantic identity;
- c71 classifier identity;
- authenticated record-stream identity;
- exact record count = 102;
- c71 repository result/tool/schema/contract;
- exact c70 ancestry embedded in c71;
- c71 tool-source identities;
- all c71 safety gates still closed;
- `classification_complete = true`;
- `terminal_origin_classification_complete = true`;
- `new_post_c69_boundaries_admitted = 0`;
- `new_cross_row_candidate_count = 0`;
- `live_qword_reads = 0`.

Any stale/mutated/missing/mismatched evidence fails closed.

## Exact c72 preflight domain

Only records whose **complete architectural successor set** is exact may be preflight eligible.

Production constants:

```text
c71 terminal records = 102
all-successor-exact records = 95
continuable exact edges = 99
interior-containing records = 6
file-backed-outside-runtime-metadata records = 1
```

The six interior-containing records and the one executable-file-backed-outside-runtime-metadata record remain wholly fail-closed. Do not partially continue the exact arm of a multi-successor record if another arm is non-exact.

## Exact record matching

For a live replay terminal, reproduce the complete c71 diagnostic record under the current root transaction and require exact equality with the accepted c71 checkpoint record before authorizing anything.

The match must bind at least:

- original root identity;
- current entry row/state identity;
- boundary path/full-prefix identity;
- source row/code/instruction identity;
- exact terminal phase/reason/event kind;
- c70/c69 provenance carried by the record;
- the retired-c69-observer distinct-edge fields where present;
- complete successor set and target relation metadata;
- record identity and accepted c71 classifier/result/checkpoint ancestry.

A record from another root/state/path/source/provenance must not be borrowable.

## All-successor preflight and continuation

For an accepted all-successor-exact record:

1. authenticate the complete current c71 record first;
2. preflight every architectural successor before admitting any arm;
3. require each target to remain the exact unique runtime begin recorded by c71;
4. require current target row bounds/code identity and source/target non-overlap to match;
5. only after all arms pass, continue under the unchanged historical transaction semantics;
6. mark c71 authorization provenance on the continued path strongly enough to distinguish it from historical c69 authorization;
7. any later unknown boundary becomes `uncertified_post_c71_runtime_boundary` and fails closed.

Admission remains root-transactional. Preflight eligibility is provisional; if the root later refuses, no provisional c71 admission or candidate may commit.

## Provenance lifecycle

C71 observed three terminal provenance forms. C72 must preserve them rather than collapsing them:

```text
used_c69_source_on_later_scan
post_c69_reachable_context
retired_c69_first_terminal_observer_distinct_edge
```

Once a specific c71 terminal is authenticated and retired for continuation, the inherited c71 first-terminal observer must not misclassify a later distinct edge as a second copy of the first c71 terminal. Exact sibling callbacks for the same authenticated multi-successor source must still be matched against the accepted sibling set.

Add an explicit lifecycle regression for this before any owner run.

## Output

Report at least:

- c71 records available/considered = 102/102;
- all-successor-exact available/preflight-eligible = 95/95;
- exact edges available/considered/preflight = 99/99/99;
- c71 record mismatch counts;
- c71 record instances admitted/refused;
- c71 exact edges admitted/refused;
- non-continuable refusal counts for 6 interior + 1 metadata-outside-runtime;
- root transactions attempted/completed/refused;
- rows reached by continuation;
- address Facts crossing rows;
- next `uncertified_post_c71_runtime_boundary` count;
- candidate counts by READ/WRITE/RMW/ADDRESS_ONLY;
- full replay / checkpoint acceleration flags;
- live qword reads;
- all safety gates.

Zero candidates must not be interpreted as writer absence.

## Focused tests

Cover at least:

- exact c71 checkpoint physical/semantic identities pinned;
- exact 102 / 95 / 99 / 6 / 1 production constants pinned;
- one exact single-successor c71 terminal continues;
- exact two-successor c71 terminal preflights both arms atomically;
- one interior arm refuses the whole root with no partial admission;
- metadata-free/file-backed-outside-runtime record remains refused;
- all three c71 provenance forms can be authenticated without relabeling;
- retired c71 first-terminal observer does not see a later distinct edge as a duplicate first terminal;
- mutated c71 checkpoint/result/record/root/state/path/source/phase/reason/origin/successor evidence fails closed;
- historical candidate accounting is unchanged unless a newly continued exact path itself produces a valid candidate;
- no slot qword read, pointer chase, native/game probe or gameplay write;
- all safety gates remain false.

Use small fixtures first. Do not run the production owner replay merely to debug focused-test mistakes.

## Change scope

Allowed:

- new c72 OFFLINE continuation tool/test files;
- minimal c71 helper exposure/correction layer strictly required for exact replay matching without changing accepted c71 semantics;
- test registration;
- c72 analysis/result docs.

Do not change accepted c56-c71 evidence semantics to make c72 pass.

## Do not

- Do not continue the six interior-containing c71 records.
- Do not continue the one executable-file-backed-outside-runtime-metadata record.
- Do not partially admit one exact arm of a non-exact multi-successor record.
- Do not define c73 before c72 owner evidence is analyzed.
- Do not infer writer absence from zero candidates.
- Do not read slot qwords or chase pointers.
- Do not traverse the deferred producer by assumption.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed unless later independently proven and explicitly authorized.
