# WallSprint project state

## Current stage — c67 owner OFFLINE blocked; c67-only correction pending validation

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

The initial c67 implementation passed synthetic/regression/build/hooks/package validation, but the first real owner/private-image run failed closed with:

```text
POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED: converted c64 POST lacks exact original c65 lower capture
```

Canonical blocker analysis:

- `analysis/V01C67_OWNER_BLOCKED_RESULT.md`

Therefore the previous c67 TOOL READY status is **superseded**. No c67 owner artifact has been accepted or produced.

**VERIFIED-STATIC root cause:** accepted c66 converts an actual lower `c64.POST` into c66 `uncertified_post_c65_runtime_boundary` before c66's diagnostic-only lower c65 capture assignment. C67 incorrectly assumed that lower capture would already exist. This does not invalidate c66.

Current correction:

```text
source/tools/slot_publication_post_c65_boundary_fix.py
```

The correction is c67-only. It snapshots the exact lower c65 edge-entry state before the unchanged c66 callback and reconstructs the omitted lower diagnostic capture only after the actual callback proves `c66.POST` caused by `c64.POST`. The unchanged c65 capture/provenance validator must accept that exact current context. C66 and accepted c56-c66 evidence remain read-only.

No target traversal, edge admission, Fact propagation, candidate creation, qword read, pointer chase, native/game probe or gameplay/production write is authorized.

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

Validate the c67 correction under focused regression, then rerun the same owner OFFLINE classification of the exact accepted 458-root wave. If it produces a complete c67 JSON, ChatGPT analyzes it before any c68 task is defined.
