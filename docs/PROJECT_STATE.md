# WallSprint project state

## Current stage — c66 continuation TOOL READY / owner OFFLINE result pending

Branch: `codex/v01c20-edi-value-provenance`

Current repository result:

```text
POST_C63_RUNTIME_BEGIN_STITCH_SCANNER_READY
Tool v0.1c66 / schema 1
```

C66 is an OFFLINE/STATIC continuation tool only. It reconstructs/authenticates the accepted c56→c65 evidence chain and may cross a c65-recorded terminal boundary only for the exact authenticated root/state/path/source/full-prefix/phase/reason/provenance context. Every required architectural successor must independently be an exact unique runtime beginning and pass the existing immutable/file-backed executable preflight. One bad successor aborts the whole root transaction. Later unknown row-external boundaries remain fail-closed as `uncertified_post_c65_runtime_boundary`.

Owner automated validation is complete: focused c66 tests PASS; all historical/static scanner regressions through c66 PASS; Windows build, portable tests, Windows smoke, hooks regression and package validation all exit 0. The first Windows build attempt exposed a case-insensitive WSL include collision in WallSprint's `tools/compat/Windows.h`; the WallSprint-owned shim was fixed and the entire remaining suite then passed. No vendor MinHook source or gameplay/runtime behavior was changed. Details: `analysis/V01C66_TOOL_READY.md` and `docs/CODEX_RESULT.md`.

Actual c66 private-image counts are still UNKNOWN until the owner OFFLINE scan creates:

```text
logs/MSM2-WS-v0.1c66-post-c63-stitched.json
```

Do not choose or implement c67 before that JSON is reviewed.

## Accepted c65 owner VERIFIED-STATIC diagnostic result

Canonical detailed evidence: `analysis/V01C65_OWNER_OFFLINE_RESULT.md`.

```text
repository_result = POST_C63_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
expected/classified roots = 998 / 998
boundary instances = 998
unique entry-instance states = 998
unique source rows = 969
architectural boundary edges = 1080
model-only edges = 0
exact_unique_runtime_begin edges = 1021
inside_unique_runtime_span_nonentry edges = 51
executable_file_backed_outside_runtime_metadata edges = 8
classification_complete = true
new boundaries admitted = 0
new cross-row candidates = 0
```

The `1021` value is an edge count, not a safe root/instance count. All-successor fail-closed semantics remain mandatory.

C65 terminal provenance remains distinct:

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

The 83 converted-earlier-refusal records are not equivalent to post-c63 reachability and may not borrow earlier c61/c63 authorization. C66 permission comes only from the exact authenticated c65 record for the current context.

## Accepted continuation/classification chain

### c64 VERIFIED-STATIC

```text
root attempts = 162472
completed/refused = 105083 / 57389
c63 instances considered/admitted/refused = 2923 / 1150 / 1773
c63 exact edges considered/admitted/refused = 3049 / 1346 / 1703
rows reached = 1549
address Facts crossing rows = 2881
useful c63 crossings/Facts = 3 / 4
next post-c63 unknown wave = 998
candidates = 0
```

### c63 VERIFIED-STATIC

```text
roots/instances = 2923
source rows = 2880
architectural edges = 3167
exact entries = 3049
interior = 109
metadata-free RX = 9
converted c60 POST records = 15
candidates = 0
```

The repaired c63 observer resolved 15 legitimate converted-context terminals; they are not post-c61 reachability and may not borrow prior authorization.

### c62 VERIFIED-STATIC

```text
root attempts = 162472
completed/refused = 103933 / 58539
c61 instances considered/admitted/refused = 6566 / 3204 / 3362
c61 exact edges considered/admitted/refused = 6946 / 3765 / 3181
rows reached = 1317
crossing Facts = 2635
useful c61 crossings/Facts = 13 / 16
next post-c61 unknown wave = 2923
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

`6946` is edge availability, not instance permission.

### c60 VERIFIED-STATIC

```text
root attempts = 162472
completed/refused = 100729 / 61743
c59 instances considered/admitted/refused = 33719 / 18833 / 14886
c59 exact edges considered/admitted/refused = 28201 / 20570 / 7631
rows reached = 794
Facts = 2062
next post-c59 unknown wave = 6566
candidates = 0
```

### c59 VERIFIED-STATIC

```text
refused roots classified = 33719 / 33719
architectural edges = 35921
exact entries = 28201
interior = 1566
metadata-free RX = 6154
candidates = 0
```

### c58 VERIFIED-STATIC

```text
root attempts = 162472
completed/refused = 81896 / 80576
exact edges considered/admitted = 147338 / 1299
rows reached = 482
crossing Facts = 1686
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

Full metadata-row coverage is not writer enumeration. Executable gaps, refused/unsupported paths, indirect/EH/alternate entries and publication/ownership/lifetime domains remain open.

## Binding production safety result

C52 remains binding:

```text
PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL
```

Later c53-c66 OFFLINE/STATIC work does not supersede that production failure.

No current evidence establishes complete publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/writer enumeration. Zero candidates never proves writer absence.

## Exact target / accepted earlier reverse-engineering facts

Exact target executable:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

Accepted movement/write chain includes the existing game-side store path through `0x2CF7FFB` and the static slot at RVA `0xC2D2580`. The exact c37 relative R8 offset range is proven, while the base publication/writer/allocation/lifetime chain remains unresolved. Producer `0x351EB00` remains deferred.

Historical c36-c52 detailed proofs remain canonical in their `analysis/V01Cxx_*` files and repository history. They must not be reinterpreted as production permission.

## Runtime / deployment state

Runtime remains `v0.1c38`; package line remains `0.1.56`. C53-c66 are offline/static analysis tooling and do not constitute a new game release.

Canonical development workspace remains:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS
```

Shared Overstrike remains the official environment. WS operations must not modify SocialFeed or other MOD repositories.

## Safety gates

All remain false/unproven:

```text
candidate_sweep_complete
writer_enumeration_complete
publication/allocation/base/range/lifetime proven
caller/storage/producer ownership
source lifetime exclusivity
downstream conflicts absent
destination containment
CFG/EH completeness
production integration
exact-slot watch permission
gameplay write permission
```

No slot qword read, pointer chase, broad memory interception, PAGE_GUARD, new hardware watch, producer traversal or gameplay write is authorized.

## Current action

Run exactly one owner OFFLINE c66 scan using the existing c56 checkpoints and accepted c57-c65 JSON files read-only. Return only:

```text
logs/MSM2-WS-v0.1c66-post-c63-stitched.json
```

ChatGPT will authenticate and analyze that JSON, preserve the detailed result in GitHub, and only then choose the smallest c67 investigation. No Update-WS, game, Overstrike, runtime capture or native probe is required for this step.
