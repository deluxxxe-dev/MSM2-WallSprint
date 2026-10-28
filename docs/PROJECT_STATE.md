# WallSprint project state

## Current stage — c68 TOOL READY; owner OFFLINE result pending

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66 and corrected c67 owner OFFLINE results are **VERIFIED-STATIC**.
C68 implementation and automated validation are **TOOL READY**. No c68 owner artifact has been accepted yet.

Canonical detail:

- `analysis/V01C66_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C67_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C67_CORRECTED_OWNER_RUN.md`
- `analysis/V01C67_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C68_TOOL_READY.md`

## Accepted c67 input

```text
file: MSM2-WS-v0.1c67-post-c65-runtime-boundary.json
SHA256: 277cb83c3fbe36715876133079c99a1d5151634442ed842960a917b140ab8aee
repository_result: POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool: v0.1c67 / schema 1
classifier_identity: a130b082512449cd008a912b94e2e3a2bcabf15140906c4780983bac07b99f20
authenticated_record_set_sha256: f0b2ce8c91868e572a83d59fc425c4e40d3fe4fe9faa7957e26a252b65e85ed6
compact_result_identity: ac043225b8b4464e8fe7e52a2c44a9a81c72bf0c2fad6e30df173fdab83b113d
```

Exact image remains authenticated:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

C67 exact domain:

```text
458 instances / 507 architectural edges
exact_unique_runtime_begin                      = 432
inside_unique_runtime_span_nonentry             = 74
executable_file_backed_outside_runtime_metadata = 1
```

These are edge counts, not safe-root counts.

## C68 TOOL READY

Implementation:

```text
source/tools/slot_publication_post_c65_stitched.py
source/tools/test_slot_publication_post_c65_stitched.py
```

Contract:

```text
repository_result POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c68 / schema 1
contract exact_c67_full_prefix_terminal_reason_provenance_all_successor_continuation_v1
next unknown uncertified_post_c67_runtime_boundary
```

C68 authenticates/replays the exact corrected c67 record stream, authorizes only complete architectural sibling sets whose targets are current unique runtime-row begins, and rolls back the whole root on any bad sibling or later unknown boundary.

The initial focused run exposed a c68-only sibling-prefix self-reference bug: `c67_prefix` was included in the hash used to validate itself, so the second callback of a conditional instruction could never match. C68 was corrected by excluding that derived field from the hash. No c56-c67 accepted evidence changed.

Focused c68 regression then passed all groups, including multi-successor, interior/free-RX refusal, provenance preservation, stale evidence refusal, next-boundary rollback and safety gates.

Owner Windows/WSL validation passed:

```text
build_windows.sh PASS
test.sh PASS
smoke_windows.sh PASS
test_hooks_windows.sh PASS
package.py PASS
C68_FULL_VALIDATION_PASS
```

## Current bounded action

Run one owner OFFLINE c68 scan and produce:

```text
logs/MSM2-WS-v0.1c68-post-c65-stitched.json
```

Do not define c69 until that exact artifact is analyzed.

No real-game test is required for c68.

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

Runtime remains `v0.1c38`; c53-c68 are OFFLINE/STATIC analysis tooling unless separately stated. Do not modify SocialFeed or other MOD repositories.
