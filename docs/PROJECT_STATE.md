# WallSprint project state

## Current stage — corrected c71 owner OFFLINE result accepted; c72 continuation design next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, c68, corrected c69, corrected c70, and corrected c71 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical detail:

- `analysis/V01C66_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C67_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C68_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C70_CHECKPOINT_SUFFICIENCY_BLOCKED.md`
- `analysis/V01C70_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C70_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C71_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C71_PROVENANCE_CORRECTION_VALIDATED.md`
- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`

## Accepted c71 evidence

Corrected owner replay completed:

```text
c70 full-replay c69 continuation: 162472/162472
POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
C71_AUTHENTICATED_PREFIX_CHECKPOINT_READY
```

Uploaded accepted checkpoint:

```text
file: MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
physical SHA256: 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
size: 1324640 bytes
checkpoint identity: 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record stream SHA256: 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
record count: 102
```

Embedded exact c71 result:

```text
repository_result: POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool: v0.1c71 / schema 1
contract: exact_c70_first_terminal_post_c69_boundary_full_prefix_provenance_diagnostic_v1
semantic result identity: 18d61be59032514a4c74c9b015f422c379e2c05050e2d5819c1025048a2276d7
classifier identity: 18cba768c4261886b70496e7f73abe3fe5276b44aecadb0d929802ae3dae9fd5
```

The standalone c71 result bytes were not separately uploaded. From the exact checkpoint-embedded result and deterministic `c56.atomic_json()` serialization, its expected standalone representation is 424663 bytes with SHA256 `db524d81d101da4173bbdda802a6bbe65a0d1801192b37b6261ecf4ce3c37ada`. C72 should authenticate the accepted checkpoint physical identity and embedded result/record identities.

Exact executable remains:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## C71 measured domain

C71 classified the exact c70 domain:

```text
expected/classified roots = 102 / 102
boundary instances = 102
unique entry-instance states = 102
unique terminal-context rows = 99
edge-backed terminals = 102
transaction terminals = 0
architectural edges = 109
model-only edges = 0
```

Observed provenance forms:

```text
used_c69_source_on_later_scan = 90
post_c69_reachable_context = 3
retired_c69_first_terminal_observer_distinct_edge = 9
```

The nine retired-observer records prove the owner-discovered c71 correction was required in real data. They remain explicitly distinct from current post-c69 reachability.

Successor relation counts:

```text
exact_unique_runtime_begin = 102
inside_unique_runtime_span_nonentry = 6
executable_file_backed_outside_runtime_metadata = 1
```

Complete per-terminal groups:

```text
all_successors_exact_runtime_begin = 95
contains_interior_successor = 6
other_relation_set = 1
```

The 95 all-successor-exact terminal instances contain exactly 99 architectural exact-runtime-begin edges.

## Next continuation domain

The exact bounded c72 preflight domain is therefore:

```text
95 c71 terminal instances
99 exact architectural successor edges
```

The following seven c71 terminal instances remain wholly fail-closed:

```text
6 containing inside_unique_runtime_span_nonentry
1 containing executable_file_backed_outside_runtime_metadata
```

Do not treat 95 as committed admission. Any later refusal still rolls back provisional continuation for that root.

## Candidates and safety

C71 remained diagnostic-only:

```text
new_post_c69_boundaries_admitted = 0
new_cross_row_candidate_count = 0
live_qword_reads = 0
c56_through_c70_evidence_read_only = true
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

Historical c70 candidate counts remain zero. Zero candidates does not prove writer absence.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

The following remain false/unproven:

```text
candidate_sweep_complete
writer_enumeration_complete
allocation_proven
base_proven
range_proven
lifetime_proven
ownership_exclusive
destination_contained
CFG_continuation
alternate_entries_EH_indirect_writers_closed
production_integrated
hardware_watch_enabled
pointer_chase
game_used
```

Runtime remains `v0.1c38`; c53-c72 work is OFFLINE/STATIC unless separately stated. Do not modify SocialFeed or other MOD repositories.

## Current bounded action

Implement and focused-test c72 as the smallest full-historical-replay continuation through only the accepted 95 all-successor-exact c71 terminal records / 99 exact edges.

C72 must authenticate the accepted c71 checkpoint physical SHA, checkpoint identity, embedded c71 result identity, classifier identity, record-stream identity, exact 102-record domain, and c70 ancestry. It must preserve all existing transaction/cap/cycle/candidate semantics and stop at the first later unknown boundary.

Do not define any wider continuation or runtime/game behavior until the c72 owner result is analyzed.
