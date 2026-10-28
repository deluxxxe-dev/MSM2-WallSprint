# WallSprint project state

## Current stage — corrected c67 owner READY observed; exact artifact validation pending

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66 owner OFFLINE result remains **VERIFIED-STATIC**.
Canonical detail:

- `analysis/V01C66_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C66_TOOL_READY.md`

C66 repository result:

```text
POST_C63_RUNTIME_BEGIN_STITCH_SCANNER_READY
Tool v0.1c66 / schema 1
```

Exact accepted c66 owner artifact SHA256:

```text
eb4e44fddb67dafd64700e19e846173e60cbaddce95860686e24e543e650cc1c
```

Exact image remains authenticated:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

Accepted c66 totals:

```text
root attempts = 162472
completed/refused = 105541 / 56931
c65 boundary instances available/considered = 998 / 998
c65 instances admitted/refused = 458 / 540
c65 exact edges available/considered = 1021 / 1021
c65 exact edges admitted/refused = 513 / 508
rows reached = 1683
address Facts crossing runtime rows = 3044
useful c65 crossings/Facts = 8 / 11
next post-c65 unknown wave = 458
candidates = 0
```

## C67 owner run

The initial c67 owner run failed closed with:

```text
POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED: converted c64 POST lacks exact original c65 lower capture
```

Canonical blocker analysis:

- `analysis/V01C67_OWNER_BLOCKED_RESULT.md`

The root cause was a c67 assumption: accepted c66 converts an actual lower `c64.POST` into c66 `uncertified_post_c65_runtime_boundary` before c66 stores its diagnostic-only lower c65 capture. Accepted c66 evidence remains valid and read-only.

A c67-only correction was added:

```text
source/tools/slot_publication_post_c65_boundary_fix.py
source/tools/test_slot_publication_post_c65_boundary_fix.py
```

The correction snapshots the exact lower c65 edge-entry state before the unchanged c66 callback and reconstructs the omitted diagnostic capture only after the actual callback proves `c66.POST` caused by `c64.POST`, then reuses unchanged c65 provenance validation.

The owner reran the correction regression and corrected owner classifier. Observed console result:

```text
c67 unchanged c66 terminal observer replay: 162472/162472
POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY: .../logs/MSM2-WS-v0.1c67-post-c65-runtime-boundary.json
C67_CORRECTED_OWNER_PASS
```

Canonical run record:

- `analysis/V01C67_CORRECTED_OWNER_RUN.md`

This confirms the prior blocker is resolved. However, the exact c67 JSON bytes/content have not yet been ingested by ChatGPT, so classification totals, record-set identity, target-relation distribution and any c68 behavior remain unaccepted.

Current bounded unknown remains the c66 458-root post-c65 wave until the exact c67 artifact is validated.

## Safety

C52 remains binding:

```text
PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL
```

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

Runtime remains `v0.1c38`; C53-c67 are OFFLINE/STATIC analysis tooling only. The shared workspace remains `<game>\Mods Library\Mods-Developing\MSM2-WS`. Do not modify SocialFeed or other MOD repositories.

## Current action

Ingest and validate the exact owner artifact:

```text
logs/MSM2-WS-v0.1c67-post-c65-runtime-boundary.json
```

Only after the exact c67 result is accepted may c68 be defined.