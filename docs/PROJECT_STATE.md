# WallSprint project state

## Current stage — c67 owner OFFLINE result accepted; c68 bounded continuation next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66 and corrected c67 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical detail:

- `analysis/V01C66_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C67_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C67_CORRECTED_OWNER_RUN.md`
- `analysis/V01C67_OWNER_OFFLINE_RESULT.md`

## Accepted c67 artifact

```text
file: MSM2-WS-v0.1c67-post-c65-runtime-boundary.json
SHA256: 277cb83c3fbe36715876133079c99a1d5151634442ed842960a917b140ab8aee
size: 600196 bytes
repository_result: POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool: v0.1c67
schema: 1
contract: exact_c66_first_terminal_post_c65_boundary_full_prefix_provenance_diagnostic_v1
classifier_identity: a130b082512449cd008a912b94e2e3a2bcabf15140906c4780983bac07b99f20
authenticated_record_set_sha256: f0b2ce8c91868e572a83d59fc425c4e40d3fe4fe9faa7957e26a252b65e85ed6
compact_result_identity: ac043225b8b4464e8fe7e52a2c44a9a81c72bf0c2fad6e30df173fdab83b113d
```

Exact image remains authenticated:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

C67 exact domain/classification:

```text
expected/classified roots = 458 / 458
instances = 458
unique states = 458
unique source rows = 445
boundary edges = 507
architectural edges = 507
model-only edges = 0
```

Target relations:

```text
exact_unique_runtime_begin                      = 432
inside_unique_runtime_span_nonentry             = 74
executable_file_backed_outside_runtime_metadata = 1
all other relations                             = 0
```

These are edge counts, not safe-root counts. C68 must retain all-successor fail-closed behavior and must measure its actual completed/refused roots instead of inferring them from 432/74/1.

C67 terminal provenance:

```text
phase:
  same_instance_after_c65_preflight_before_row_fixpoint = 348
  post_c65_reachable_context                            = 34
  post_c65_reachable_dedup_stored_earlier_path          = 3
  prior_c65_authorization_converted_earlier_refusal     = 73
reason:
  used_c65_source_later_instruction          = 348
  current_context_post_c65_reachable          = 37
  prior_c65_authorization_converted_c64_post = 73
underlying:
  uncertified_post_c63_runtime_boundary = 73
```

C67 is diagnostic only:

```text
new_post_c65_boundaries_admitted = 0
new_cross_row_candidate_count = 0
cross_runtime_fact_propagation_unchanged = true
c66_candidate_output_unchanged = true
live_qword_reads = 0
c56_through_c66_evidence_read_only = true
```

Historical candidate output remains zero with SHA256:

`4f53cda18c2baa0c0354bb5f9a3ecbe5ed12ab4d8e11ba873c2f11161202b945`

The c67 correction fixed only observation of a converted c64 POST lower capture. It did not alter c66 or accepted c56-c66 evidence.

## Current bounded action

Implement c68 as the smallest continuation of the accepted c67 records:

- authenticate/replay the exact accepted c67 classifier result;
- authorize only exact `exact_unique_runtime_begin` architectural successors;
- require every architectural sibling of an instance to pass before any root can commit;
- preserve exact c67 source/root/state/path/phase/reason/provenance including the 73 converted-c64 class;
- roll back the whole root on any non-exact sibling or later unknown boundary;
- measure the next unknown wave and candidate/Facts result; do not infer it from aggregate edge counts.

No broad scan, native/game probe or production/gameplay write is authorized.

## Safety

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

Zero candidates never proves writer absence.

Runtime remains `v0.1c38`; c53-c68 work is OFFLINE/STATIC analysis tooling unless separately stated. The shared workspace remains `<game>\Mods Library\Mods-Developing\MSM2-WS`. Do not modify SocialFeed or other MOD repositories.
