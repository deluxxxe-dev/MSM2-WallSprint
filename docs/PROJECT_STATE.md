# WallSprint project state

## Current stage — corrected c69 owner OFFLINE result accepted; c70 full-replay continuation next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, c68, and corrected c69 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical detail:

- `analysis/V01C66_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C67_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C68_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C70_CHECKPOINT_SUFFICIENCY_BLOCKED.md`

## Accepted c69 artifacts

Result:

```text
file: MSM2-WS-v0.1c69-post-c67-runtime-boundary.json
physical SHA256: e57b7f39ea476e356397f944d8aadb14841b27874736a60c1a82b982eb86537f
size: 683026 bytes
repository_result: POST_C67_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool: v0.1c69 / schema 1
contract: exact_c68_first_terminal_post_c67_boundary_full_prefix_provenance_diagnostic_v1
classifier identity: 4f42b0d04346eb5c7b10c8fe0592edd3a3f19fcb463d66a0adf40be01fd25049
semantic result identity: 08002e4c3aef2c7cfa037040b6f09f7d1b6e247d0b446daea1ffdb2569aa01fb
authenticated record set: 20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed
```

Checkpoint:

```text
file: MSM2-WS-v0.1c69-authenticated-prefix-checkpoint.json
physical SHA256: d7b8dae21e9e0478e8ea5d8136c7f19156cd5afca2ff80f167dce48a2e61b6b5
size: 3506330 bytes
checkpoint contract: authenticated_c68_prefix_first_post_c67_records_v1
checkpoint identity: b734f8ab2f0a24e002c8b5c2ed1dbbfe75c9411cc773ec1394a1cf330c9b3a28
record count: 191
record stream: 20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed
```

Exact image remains authenticated:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## C69 measured terminal classification

```text
expected/classified terminal instances = 191 / 191
unique entry-instance states           = 191
unique terminal context/source rows    = 186
edge-backed terminal instances         = 190
transaction terminal instances         = 1
architectural edges                    = 209
exact_unique_runtime_begin             = 196
inside_unique_runtime_span_nonentry    = 13
```

Per-terminal complete-successor grouping:

```text
177 terminal instances: every architectural successor exact runtime begin
191 exact edges: belong to those 177 complete instances
13 edge-backed instances: contain at least one interior successor
1 transaction-only instance: no authenticated instruction/edge
```

The five mixed exact+interior cases are wholly fail-closed; no partial exact-arm admission.

## Checkpoint continuation sufficiency

The c69 checkpoint is accepted as an authenticated diagnostic record checkpoint, but static review of the existing continuation engine proved that it is not an exact resume snapshot for the full-root transaction semantics.

Missing continuation-prefix state includes the historical context graph, already-consumed work/cap counters, provisional root candidates, processed/todo position and full mutable authorization/path state. Resetting these at c69 would weaken or change the existing contract.

Therefore:

```text
c69 checkpoint authentication: accepted
c69 checkpoint direct full-root resume: BLOCKED
checkpoint acceleration claimed: false
```

Canonical detail: `analysis/V01C70_CHECKPOINT_SUFFICIENCY_BLOCKED.md`.

## Current bounded action

C70 must do one explicit full historical replay and use the accepted c69 result/checkpoint only as the exact c69 boundary authorization source.

C70 may:

- consider all 191 c69 terminal records exactly;
- continue only the 177 records whose complete architectural successor set is exact runtime begin;
- preflight all successors atomically;
- preserve the existing full-root transaction graph/caps/cycles/provisional-candidate semantics;
- stop at the first new post-c69 boundary;
- report that full historical replay was used.

C70 must keep the 13 interior-containing records and the one transaction-only record fail-closed.

Do not define behavior beyond the first new post-c69 boundary until the c70 owner result is analyzed.

## Candidates and safety

C69 added no admission or candidates. Historical c68 candidate counts remain zero. Zero candidates does not prove writer absence.

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

Runtime remains `v0.1c38`; c53-c70 work is OFFLINE/STATIC unless separately stated. Do not modify SocialFeed or other MOD repositories.
