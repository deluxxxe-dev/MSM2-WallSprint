# WallSprint project state

## Current stage — corrected c71 owner accepted; c72 TOOL READY; owner OFFLINE run next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, c68, corrected c69, corrected c70, and corrected c71 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical recent detail:

- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C70_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C71_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C71_PROVENANCE_CORRECTION_VALIDATED.md`
- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_PREPARED.md`
- `analysis/V01C72_TOOL_READY.md`

## Accepted c71 evidence

```text
MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
physical SHA256 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
checkpoint identity 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record stream SHA256 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
record count 102
```

Embedded result:

```text
repository_result POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool v0.1c71 / schema 1
semantic result identity 18d61be59032514a4c74c9b015f422c379e2c05050e2d5819c1025048a2276d7
classifier identity 18cba768c4261886b70496e7f73abe3fe5276b44aecadb0d929802ae3dae9fd5
```

Exact image remains:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## C71 measured domain

```text
expected/classified terminal roots = 102 / 102
edge-backed = 102
transaction = 0
architectural edges = 109
model-only edges = 0
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

Complete terminal groups:

```text
all_successors_exact_runtime_begin = 95
contains_interior_successor = 6
other_relation_set = 1
```

The 95 all-successor-exact records contain exactly 99 exact architectural successor edges.

## C72 status

C72 implementation exists:

```text
source/tools/slot_publication_post_c71_stitched.py
source/tools/test_slot_publication_post_c71_stitched.py
```

The owner ran the focused suite and all c72 checks passed, including:

```text
production contract pins accepted c71 identities and 95/99 domain
exact live-record/all-successor authentication before continuation
later distinct edge -> uncertified_post_c71_runtime_boundary with rollback
interior c71 terminal remains refused
mutated c71 checkpoint record fails closed
focused full-replay c71 continuation groups complete
```

Therefore c72 is **TOOL READY**, not yet an accepted owner result.

## C72 bounded continuation domain

```text
c71 terminal records total = 102
all-successor-exact records = 95
continuable exact architectural edges = 99
non-continuable records = 7
```

The seven non-continuable records remain fail-closed:

```text
inside_unique_runtime_span_nonentry = 6
executable_file_backed_outside_runtime_metadata = 1
```

Preflight eligibility is provisional. Any later refusal rolls back c71 admission for that root.

## Safety

C71 emitted no new candidate and c72 focused validation adds no game interaction.

```text
live_qword_reads = 0
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

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

Zero candidates does not prove writer absence.

Runtime remains `v0.1c38`; c53-c72 work is OFFLINE/STATIC unless separately stated. Do not modify SocialFeed or other MOD repositories.

## Current bounded action

Run exactly one c72 owner OFFLINE continuation against accepted c56-c71 evidence.

If READY, analyze the resulting admission counts, next-boundary count, candidates and safety before defining c73.

If BLOCKED, stop and analyze that exact observed fact before changing anything.
