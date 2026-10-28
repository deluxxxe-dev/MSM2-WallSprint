# WallSprint project state

## Current stage — c66 owner result accepted; c67 boundary diagnostic next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66 owner OFFLINE result is **VERIFIED-STATIC**.
Canonical detail:

- `analysis/V01C66_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C66_TOOL_READY.md`

C66 repository result:

```text
POST_C63_RUNTIME_BEGIN_STITCH_SCANNER_READY
Tool v0.1c66 / schema 1
```

Exact owner artifact SHA256:

```text
eb4e44fddb67dafd64700e19e846173e60cbaddce95860686e24e543e650cc1c
```

Exact image remains authenticated:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

C66 authenticated/replayed the full accepted c56→c65 chain read-only. Accepted c65 classifier/record-set identities match the owner c65 artifact and the independently recomputed c65 compact result identity is:

```text
dd4ff3451faa9735f88756e16222a36fc80cafbf51f113bc12f6f06b6dc62780
```

### Accepted c66 totals

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

Relative to accepted c64, c66 adds 458 completed roots, 134 reached rows and 163 cross-row address Facts. This is bounded mechanism/coverage progress only, not writer/publication/ownership completeness.

C65 all-successor relation refusals observed during c66 preflight remain edge-level diagnostics:

```text
inside_unique_runtime_span_nonentry = 51
executable_file_backed_outside_runtime_metadata = 8
```

The 1021 exact targets are edge availability only. Multi-successor all-successor fail-closed semantics remain mandatory.

C65 phase admission remains provenance-specific:

```text
same_instance_after_c63_preflight_before_row_fixpoint: 836 considered / 417 admitted / 419 refused
post_c63_reachable_context: 77 / 27 / 50
post_c63_reachable_dedup_stored_earlier_path: 2 / 1 / 1
prior_c63_authorization_converted_earlier_refusal: 83 / 13 / 70
```

Converted-earlier-refusal records are not natural post-c63 reachability and may not borrow earlier c61/c63 authorization.

The exact next bounded unknown is:

```text
uncertified_post_c65_runtime_boundary = 458 roots
```

C67 is therefore limited to classifying the first such terminal boundary for those exact 458 refused roots. C67 must not admit new edges, propagate new Facts or create candidates.

## Accepted c65 owner VERIFIED-STATIC diagnostic result

Canonical detail: `analysis/V01C65_OWNER_OFFLINE_RESULT.md`.

```text
expected/classified roots = 998 / 998
boundary instances = 998
unique source rows = 969
architectural boundary edges = 1080
exact_unique_runtime_begin = 1021
inside_unique_runtime_span_nonentry = 51
executable_file_backed_outside_runtime_metadata = 8
classification_complete = true
candidates = 0
```

Terminal provenance:

```text
same-instance c63 preflight = 836
post-c63 reachable = 77
post-c63 dedup stored-earlier-path = 2
converted earlier refusal = 83
```

## Accepted continuation/classification chain

### c64 VERIFIED-STATIC

```text
root attempts = 162472
completed/refused = 105083 / 57389
c63 instances considered/admitted/refused = 2923 / 1150 / 1773
c63 exact edges considered/admitted/refused = 3049 / 1346 / 1703
rows reached = 1549
address Facts = 2881
next post-c63 unknown wave = 998
candidates = 0
```

### c63 VERIFIED-STATIC

```text
roots/instances = 2923
architectural edges = 3167
exact entries = 3049
interior = 109
metadata-free RX = 9
converted c60 POST records = 15
candidates = 0
```

### c62 VERIFIED-STATIC

```text
completed/refused = 103933 / 58539
c61 instances considered/admitted/refused = 6566 / 3204 / 3362
c61 exact edges considered/admitted/refused = 6946 / 3765 / 3181
rows reached = 1317
Facts = 2635
next post-c61 wave = 2923
candidates = 0
```

### c61 VERIFIED-STATIC

```text
refused roots classified = 6566 / 6566
architectural edges = 7206
exact entries = 6946
interior = 245
metadata-free RX = 15
candidates = 0
```

### c60 VERIFIED-STATIC

```text
completed/refused = 100729 / 61743
c59 instances considered/admitted/refused = 33719 / 18833 / 14886
c59 exact edges considered/admitted/refused = 28201 / 20570 / 7631
rows reached = 794
Facts = 2062
next post-c59 wave = 6566
candidates = 0
```

### c59 VERIFIED-STATIC

```text
roots classified = 33719 / 33719
architectural edges = 35921
exact entries = 28201
interior = 1566
metadata-free RX = 6154
candidates = 0
```

### c58 VERIFIED-STATIC

```text
completed/refused = 81896 / 80576
exact edges considered/admitted = 147338 / 1299
rows reached = 482
Facts = 1686
next later-boundary wave = 33719
candidates = 0
```

### c57 VERIFIED-STATIC

```text
roots classified = 162472 / 162472
architectural edges = 201788
exact entries = 159734
interior = 29560
metadata-free RX = 12494
candidates = 0
```

### c56 VERIFIED-STATIC

```text
runtime metadata rows = 527161 / 527161
shards = 4119
modeled = 204770
refused = 322390
excluded = 1
local candidates = 0
```

Full metadata-row coverage is not writer enumeration.

## Binding production safety result

C52 remains binding:

```text
PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL
```

C53-c66 OFFLINE/STATIC analysis does not supersede that production failure.

Current c66 result still reports:

```text
candidate_sweep_complete = false
writer_enumeration_complete = false
allocation_proven = false
base_proven = false
range_proven = false
lifetime_proven = false
ownership_exclusive = false
destination_contained = false
CFG_continuation = false
alternate_entries_EH_indirect_writers_closed = false
production_integrated = false
hardware_watch_enabled = false
pointer_chase = false
game_used = false
```

Zero candidates therefore does not prove writer absence.

No slot qword read, pointer chase, broad memory interception, PAGE_GUARD, new hardware watch, producer traversal or gameplay write is authorized.

Producer `0x351EB00` remains deferred. The existing game-side store path through `0x2CF7FFB`, the static slot RVA `0xC2D2580`, and the c37 offset range remain accepted historical facts; base publication/writer/allocation/lifetime remain unresolved.

## Runtime / deployment state

Runtime remains `v0.1c38`; package line remains `0.1.56`. C53-c67 work is offline/static analysis tooling, not a new game release.

Canonical workspace remains:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS
```

WS work must not modify SocialFeed or other MOD repositories.

## Current action

Implement c67 as a separate OFFLINE/STATIC diagnostic that authenticates accepted c56→c66 evidence and classifies only the first terminal `uncertified_post_c65_runtime_boundary` for the exact 458-root wave. No new boundary admission, target traversal, Fact propagation, candidate creation, game use or native probe.
