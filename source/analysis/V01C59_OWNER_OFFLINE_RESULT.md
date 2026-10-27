# c59 owner offline result — later runtime-boundary classification

2026-10-27 JST. Evidence class: **VERIFIED-STATIC**. Owner supplied the compact output from `slot_publication_later_boundary.py` against the exact pinned `Spider-Man2.exe`.

## Authentication

- repository result: `LATER_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY`
- tool/schema: `v0.1c59` / `1`
- image authenticated: true
- image SHA256: `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- accepted c57 boundary identity: `b990e0d13ed735418fda2669a8e54fec4f7e0b5647d846a4fbaf99eb80639158`
- accepted c58 proof identity: `9fbdd2552a4e42fe592ebc41ab2f5b9feb65d67349aee43742364140fda10d8f`
- accepted c58 result identity: `718013be12205027c1e0ae777e70bd2a49602ad002bc6f26242f35fece148827`
- c59 classifier identity: `d4c9d6ad7e9403444ba97d5d181be56fd687b6c58cf401bed88940a2af7bb2e6`
- supplied compact JSON SHA256: `89cea850d23fe38ef60a8f47799a75fef12683a72919ad73ff3e36f85ba5511c`

The classifier reports the c56/c57/c58 evidence as read-only and `classification_complete=true`.

## Complete c58 later-boundary domain

The accepted c58 first terminal later-boundary set was classified exactly:

```text
later_boundary_refused_roots_expected   = 33719
later_boundary_refused_roots_classified = 33719
later_boundary_instances_total          = 33719
unique_entry_instance_states            = 33719
unique_source_rows                      = 33582
later_boundary_edges_total              = 35921
architectural_later_boundary_edges_total= 35921
model_only_later_boundary_edges_total   = 0
```

Entry-instance seed diagnostics:

```text
instances with empty entry seed = 33582
instances in original root      = 33582
```

Therefore 137 instances have a non-empty entry seed and, separately, 137 instances are non-root. These counts must not be interpreted as source-instruction fact counts: c59 explicitly records the **entry seed before local must-fixpoint**, not the state at the refusing instruction.

## Target distribution

Exact target relation totals over all 35,921 architectural edges:

```text
exact_unique_runtime_begin                    = 28201  (78.5084%)
inside_unique_runtime_span_nonentry           =  1566  ( 4.3596%)
executable_file_backed_outside_runtime_metadata= 6154  (17.1320%)
all overlap/unbacked/nonexec/outside/deferred =     0
```

Exact-runtime-begin edge kinds:

```text
ordinary_fallthrough       = 16975
conditional_fallthrough    = 2309
direct_conditional_taken   = 7425
direct_unconditional_branch= 1471
call_fallthrough           = 21
```

Exact adjacency totals 20,863 edges. Adjacency remains diagnostic only; non-adjacent direct edges can still be authenticated exact-runtime-begin edges.

The non-metadata target class is structurally concentrated: all 6,154 metadata-free RX edges are direct unconditional branches. Interior targets are 1,368 direct conditional taken plus 198 direct unconditional branches.

## Interpretation

This result closes only the c59 diagnostic question. It establishes that the dominant first terminal later-boundary class reached by c58 is again an authenticated architectural edge to an exact unique runtime begin.

It does **not** prove that a useful address fact is present at the refusing instruction, because graph construction aborts before the row-local must-fixpoint is computed. It also does not authorize the 28,201 exact edges for continuation by itself.

The natural next bounded experiment is therefore to extend c58 entry-instance semantics with a second authenticated edge source: c57 certificates for first boundaries plus c59 certificates for the measured first later boundary. Only c59 `exact_unique_runtime_begin` architectural edges may be added. Any subsequent uncertified boundary must fail closed and become the next measured refusal domain.

## Unchanged safety/completeness boundaries

c59 admits no new boundary and emits no candidate. The supplied result keeps:

```text
new_later_boundaries_admitted = 0
new_cross_row_candidate_count = 0
c58_candidate_output_unchanged = true
candidate_sweep_complete = false
writer_enumeration_complete = false
publication_source_kind = unproven
allocation/base/range/lifetime/ownership/destination/CFG/EH gates = false
production_integrated = false
hardware_watch_enabled = false
live_qword_reads = 0
pointer_chase = false
game_used = false
```

C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding. No writer absence, publication source, ownership/lifetime, production permission, or gameplay permission follows from this result.
