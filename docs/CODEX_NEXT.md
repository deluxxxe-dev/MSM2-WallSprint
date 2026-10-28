# Next task — c68 exact c67 authenticated continuation

## Status

Corrected owner c67 result is accepted as **VERIFIED-STATIC**.

Canonical result:

- `analysis/V01C67_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c67 artifact:

```text
MSM2-WS-v0.1c67-post-c65-runtime-boundary.json
SHA256 277cb83c3fbe36715876133079c99a1d5151634442ed842960a917b140ab8aee
repository_result POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
classifier_identity a130b082512449cd008a912b94e2e3a2bcabf15140906c4780983bac07b99f20
authenticated_record_set_sha256 f0b2ce8c91868e572a83d59fc425c4e40d3fe4fe9faa7957e26a252b65e85ed6
compact result identity ac043225b8b4464e8fe7e52a2c44a9a81c72bf0c2fad6e30df173fdab83b113d
```

Observed exact domain:

```text
458 instances / 507 architectural edges
exact_unique_runtime_begin = 432 edges
inside_unique_runtime_span_nonentry = 74 edges
executable_file_backed_outside_runtime_metadata = 1 edge
```

The relation values are edge counts, not unique roots.

## Purpose

Implement c68 as the smallest OFFLINE continuation using fully replayed exact-provenance c67 records.

C68 may admit a c67-recorded boundary only when the exact current root/state/path/source/full-prefix/phase/reason/provenance record matches and **every architectural successor** in that record is an exact `exact_unique_runtime_begin` with matching current bounds/code/preflight.

One bad sibling aborts the entire root. No partial/provisional root admission may survive.

## Required implementation

Create the c68 continuation tool and focused regression tests. Reuse the established c64/c66 all-successor continuation pattern rather than inventing a new framework.

Required semantics:

- authenticate/reconstruct the accepted c56→c67 chain read-only;
- accept only the exact accepted corrected c67 record stream;
- preserve the c67-only converted-c64 lower-capture correction as provenance observation only;
- retain source-local c67 terminal phase/reason/provenance, including:
  - `used_c65_source_later_instruction`;
  - natural post-c65 reachable cases;
  - dedup stored-earlier-path cases;
  - the 73 `prior_c65_authorization_converted_c64_post` cases;
- preflight every architectural successor before authorizing the instance;
- exact target must be a current unique runtime-row begin with matching bounds and code;
- interior-span and metadata-free-RX relations remain refused;
- overlapping source/target spans, model-only successors, stale/missing/mismatching records or provenance remain refused;
- cached siblings may reuse only the exact same authenticated instruction/prefix/proof set;
- mark only descendants reached through an admitted c67 proof as post-c67 context;
- any next unknown row-external boundary becomes a new fail-closed diagnostic reason, recommended name `uncertified_post_c67_runtime_boundary`;
- root transaction commits admission/candidates/Facts only if the whole root completes; later refusal rolls provisional work back.

Recommended repository result naming:

```text
POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY
Tool v0.1c68 / schema 1
contract exact_c67_full_prefix_terminal_reason_provenance_all_successor_continuation_v1
```

## Do not

- Do not infer admitted-root count from 432 exact edges.
- Do not promote the 74 interior or 1 metadata-free-RX targets to entries.
- Do not change c66/c67 accepted evidence.
- Do not widen caps.
- Do not read slot qwords or chase pointers.
- Do not traverse the deferred producer by assumption.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.

## Completion conditions

Focused c68 tests must cover at least:

- ordinary exact continuation;
- direct/conditional/CALL-fallthrough exact successors;
- multi-successor all-arms success;
- one interior arm causes whole-root rollback;
- metadata-free RX refusal;
- natural post-c65 phase/reason preservation;
- dedup stored-path separation;
- converted-c64 provenance preservation without relabeling;
- stale/mismatched c67 owner evidence refusal;
- next unknown post-c67 boundary rollback;
- different roots/path contexts remain separate;
- all safety gates remain false and candidate output is only from completed roots.

Then run the existing scanner regression set, Windows build/test/smoke/hooks/package validation. Do not run the real game.

After tool validation, the owner OFFLINE run should use the exact accepted c56-c67 evidence and output a new c68 JSON. ChatGPT must analyze that artifact before defining c69.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed. Zero candidates never proves writer absence.
