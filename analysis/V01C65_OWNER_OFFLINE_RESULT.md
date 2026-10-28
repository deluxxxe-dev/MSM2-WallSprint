# V01C65 owner offline result — accepted VERIFIED-STATIC

2026-10-28 JST. Owner supplied `MSM2-WS-v0.1c65-post-c63-runtime-boundary.json` from the pinned private image.

## Classification

**VERIFIED-STATIC** / `POST_C63_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY`.

This is authenticated OFFLINE/STATIC diagnostic evidence only. It is not runtime/gameplay evidence and grants no writer, publication, ownership, lifetime, CFG/EH, watch, pointer, qword-read, or production permission.

## Artifact identity

```text
file_sha256 = fee4291386e5c37204efa19e8afc68d0911f07910d5ba3730623dee792b4c202
file_bytes = 517860
tool_version = v0.1c65
schema_version = 1
contract = exact_c64_first_terminal_post_c63_boundary_full_prefix_provenance_diagnostic_v1
image_sha256 = c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
classifier_identity = e213e4e137efb57f36a3a716280ed9c83f9641e599fed26daeb2bedcc758bb99
authenticated_record_set_sha256 = dd2964a012eff48f01dfd2f3d49b6acb70e7e40ba44cf2cda3534f1f9e2eff22
```

The accepted chain identities through c64 match the supplied c65 artifact; `image_authenticated=true`, `c56_through_c64_evidence_read_only=true`, and classification is complete.

## Complete diagnostic domain

```text
expected refused roots = 998
classified refused roots = 998
boundary instances = 998
unique entry-instance states = 998
unique source rows = 969
boundary edges total = 1080
architectural edges = 1080
model-only edges = 0
classification_complete = true
```

Target relations:

```text
exact_unique_runtime_begin = 1021
inside_unique_runtime_span_nonentry = 51
executable_file_backed_outside_runtime_metadata = 8
all other relation classes = 0
```

Thus 1021/1080 = 94.537% of observed outgoing edges are exact unique runtime beginnings. This is edge availability only; it is not 1021 admissible roots or instances. Any later continuation must retain all-successor fail-closed admission.

Edge kinds:

```text
ordinary fallthrough = 444 (all 444 exact)
conditional fallthrough = 83 (all 83 exact)
direct conditional taken = 449 (411 exact, 38 interior)
direct unconditional branch = 103 (82 exact, 13 interior, 8 metadata-free RX)
CALL fallthrough = 1 (exact)
```

## Terminal provenance distribution

Phases:

```text
same_instance_after_c63_preflight_before_row_fixpoint = 836
post_c63_reachable_context = 77
post_c63_reachable_dedup_stored_earlier_path = 2
prior_c63_authorization_converted_earlier_refusal = 83
```

Reasons:

```text
used_c63_source_later_instruction = 836
current_context_post_c63_reachable = 79
prior_c63_authorization_converted_c62_post = 82
prior_c63_authorization_converted_pre_c59_later = 1
```

Underlying converted refusals are 82 `uncertified_post_c61_runtime_boundary` plus 1 `uncertified_later_runtime_boundary`. These converted cases are not equivalent to current post-c63 reachability and must remain exact-context/provenance specific.

## Preservation / safety

C65 admitted no edge and emitted no candidate:

```text
new_post_c63_boundaries_admitted = 0
new_cross_row_candidate_count = 0
c64_candidate_output_unchanged = true
candidate_sweep_complete = false
writer_enumeration_complete = false
production_integrated = false
hardware_watch_enabled = false
game_used = false
```

Zero candidates is not writer absence. C52 production review FAIL remains binding and all completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.

## Next bounded step

A c66 continuation may use only the complete authenticated c65 record stream, exact current root/state/path/source/phase/reason/provenance, and only when every required outgoing architectural successor for that exact boundary is `exact_unique_runtime_begin` and passes unchanged target preflight. Interior and metadata-free RX successors must refuse the whole root. Any later unknown row-external boundary must fail closed under a distinct post-c65 refusal; no recursive or row-global certificate inference.
