# Codex result — corrected c71 owner OFFLINE classification accepted

2026-10-29 JST. Branch **codex/v01c20-edi-value-provenance**.

## Status

Corrected c71 owner OFFLINE result is accepted as **VERIFIED-STATIC**.

Canonical detail:

- `analysis/V01C70_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C71_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C71_PROVENANCE_CORRECTION_VALIDATED.md`
- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`

The first c71 owner attempt remains blocked historical evidence only. The corrected owner run completed successfully.

## Corrected owner completion

Observed log end:

```text
c70 full-replay c69 continuation: 162472/162472
POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
C71_AUTHENTICATED_PREFIX_CHECKPOINT_READY
```

Corrected owner log:

```text
c71_owner_corrected.log
size 198812 bytes
SHA256 b583c1d01883f0f08fd0011d7e2219473a7d17dc37501c5544e096d1f3ed918c
```

Accepted uploaded c71 checkpoint:

```text
MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
size 1324640 bytes
physical SHA256 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
checkpoint identity 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record count 102
record stream SHA256 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
```

Embedded result identity:

```text
repository_result POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool v0.1c71 / schema 1
contract exact_c70_first_terminal_post_c69_boundary_full_prefix_provenance_diagnostic_v1
semantic result identity 18d61be59032514a4c74c9b015f422c379e2c05050e2d5819c1025048a2276d7
classifier identity 18cba768c4261886b70496e7f73abe3fe5276b44aecadb0d929802ae3dae9fd5
```

The standalone result file was emitted but not separately uploaded. Deterministic `atomic_json` serialization of the exact result embedded in the checkpoint yields expected size 424663 bytes and expected SHA256 `db524d81d101da4173bbdda802a6bbe65a0d1801192b37b6261ecf4ce3c37ada`.

## C71 result

Exact domain:

```text
expected/classified roots 102 / 102
boundary instances 102
unique entry-instance states 102
unique terminal-context rows 99
edge-backed 102
transaction 0
architectural edges 109
model-only edges 0
```

Observed terminal provenance:

```text
used_c69_source_on_later_scan 90
post_c69_reachable_context 3
retired_c69_first_terminal_observer_distinct_edge 9
```

Thus the correction made after the first blocked owner run was required by real owner data and is now validated in the full replay.

Successor relations:

```text
exact_unique_runtime_begin 102
inside_unique_runtime_span_nonentry 6
executable_file_backed_outside_runtime_metadata 1
```

Complete terminal grouping:

```text
all_successors_exact_runtime_begin 95
contains_interior_successor 6
other_relation_set 1
```

The 95 all-successor-exact records contain exactly 99 exact architectural successor edges.

## Safety / candidates

C71 remained diagnostic-only:

```text
new_post_c69_boundaries_admitted 0
new_cross_row_candidate_count 0
live_qword_reads 0
full_historical_replay_used true
checkpoint_accelerated_resume_used false
c56_through_c70_evidence_read_only true
```

Historical candidate counts remain zero. All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

Zero candidates does not prove writer absence.

## Next

C72 is now authorized only as a minimum full-historical-replay continuation through the 95 accepted c71 records whose complete architectural successor set is exact-runtime-begin, covering 99 exact edges.

The six interior-containing records and one executable-file-backed-outside-runtime-metadata record remain wholly fail-closed. Do not define c73 before c72 owner evidence is analyzed.
