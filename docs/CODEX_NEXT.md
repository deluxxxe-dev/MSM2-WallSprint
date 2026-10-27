# Next task — c58 exact-runtime-begin cross-row provenance continuation

## Status

Accepted owner c57 VERIFIED-STATIC result:

- `analysis/V01C57_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C57_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

C57 authenticated and completely classified the accepted c56 `unsupported_runtime_boundary` set:

```text
boundary_refused_rows_expected = 162472
boundary_refused_rows_classified = 162472
boundary_edges_total = 201788
architectural_boundary_edges_total = 201788
model_only_boundary_edges_total = 0
classification_complete = true
cross_runtime_fact_propagation = false
```

Exact target distribution:

```text
exact_unique_runtime_begin = 159734   # 79.1593%
inside_unique_runtime_span_nonentry = 29560   # 14.6490%
executable_file_backed_outside_runtime_metadata = 12494   # 6.1916%
all ambiguous/unbacked/nonexec/outside/deferred classes = 0
```

Exact adjacency is 90430 edges, but adjacency is diagnostic only. The remaining 69304 exact-runtime-begin edges are non-adjacent direct control transfers. C57 itself did not authorize any cross-row continuation.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, `analysis/V01C57_OWNER_OFFLINE_RESULT.md`, c53-c57 scanner evidence/tools/tests, and relevant Wisdoms references.

## Current bounded task

C58 is **OFFLINE/STATIC address-provenance continuation only**.

Implement a separate v0.1c58 tool/mode that can carry an already-proven c53-style GPR image-address fact across an authenticated architectural c57 boundary edge **only when the target relation is exactly `exact_unique_runtime_begin`**.

The purpose is to test whether the dominant 79.16% exact-entry boundary class can extend existing exact-slot candidate discovery without weakening any other refusal domain.

Do not broaden memory/value semantics. Do not make publication/writer-completeness claims.

### 1. Admitted cross-row edge contract

A cross-row continuation may be attempted only when all of the following are independently authenticated on the exact pinned image:

- source row identity/order/bounds/hash matches the accepted c56/c57 evidence;
- source instruction identity/bytes and architectural edge kind match c57 replay;
- `architectural_successor == true`;
- target relation is exactly `exact_unique_runtime_begin`;
- target row index/begin/end is exact and unique at that target point;
- target row code is the same immutable/file-backed executable domain already accepted by c53-c57;
- source/target are outside the deferred producer exclusion;
- no stale image/tool/schema/contract/caps evidence exists.

Adjacency is not required for a direct branch and never substitutes for edge authentication.

The following remain refused and must never be stitched in c58:

- `inside_unique_runtime_span_nonentry`;
- `executable_file_backed_outside_runtime_metadata`;
- overlapping metadata;
- unbacked executable targets;
- non-executable/writable targets;
- outside-image targets;
- deferred producer targets;
- indirect jump/call targets;
- EH/alternate entries not represented by an admitted exact architectural edge;
- any model-only/synthetic successor.

### 2. Entry-instance semantics are mandatory

Do **not** turn a predecessor-carried fact into a global fact for the target runtime row.

The target row's historical independent scan from an empty root remains separate and unchanged.

A carried state must be analyzed as a separate continuation instance keyed by an authenticated predecessor/root and exact incoming state identity (or an equivalent representation with the same safety property).

Required semantics:

- incoming predecessor state is not merged with the target row's independent EMPTY-root state;
- two different incoming carried states are not merged into a fabricated common fact;
- identical continuation states may be deterministically deduplicated;
- within one continuation instance, retain c53 must-join semantics exactly: a fact survives a join only when all reachable incoming states in that instance agree exactly;
- a candidate reached from a continuation instance is **path/entry-instance static candidate evidence**, not an all-entry invariant for the target row.

If a coherent bounded implementation cannot preserve this distinction, return BLOCKED rather than weakening it.

### 3. Fact language remains unchanged

Carry only the existing c53 accepted GPR address facts:

- RIP-relative LEA image origin;
- full64 register copy;
- checked full64 affine LEA constant displacement;
- exact constant memory displacement from a proven base.

Do not introduce:

- memory-loaded pointer/value propagation;
- slot qword reads;
- pointer chase;
- stack/local value facts;
- callee return-value facts;
- ABI preservation assumptions;
- immediate pointer materialization or unsupported arithmetic/shift generality;
- indexed/segment generality;
- partial-register recovery.

Crossing a row boundary does not change the underlying address `Fact`. Keep boundary-path proof metadata separate from the c53 address-derivation hop chain so c53 provenance-hop semantics are not silently changed.

### 4. CALL behavior remains c54/c55-only

C57 `call_fallthrough` denotes the modeled return site, not the callee target.

At a CALL:

- use only the already-authenticated c54/c55 per-register preservation result available to the existing scanner;
- RSP remains killed;
- any non-preserved address facts remain killed;
- c58 must not use the boundary classifier to infer callee preservation, return values, or a new call target rule.

The single measured c57 call-fallthrough boundary edge is not special-cased.

### 5. Bounded cross-row graph / fail-closed behavior

Use deterministic finite caps for cross-row continuation instances, row transitions, instructions, edges, states, iterations, cycles and retained proof-path records.

Required behavior:

- exact-entry row cycles are detected/deduplicated by authenticated state identity;
- state-changing cycles or cap exhaustion refuse the affected continuation instance;
- no candidate may escape from an incomplete/refused continuation instance;
- any stale/missing/corrupt c56/c57 evidence refuses rather than falling back to a weaker mode;
- historical c53-c57 source behavior and outputs remain unchanged.

Prefer read-only reuse of the owner's valid c56 checkpoints and c57 boundary JSON when available. Do not rewrite historical private evidence. A fresh in-memory recomputation is acceptable only if it authenticates the same exact contracts and produces the same accepted c56/c57 identities.

### 6. Candidate records / diagnostics

Keep the existing access classification and exact-slot effective-address proof.

Any c58 continuation candidate must additionally identify, directly or by bounded hashes/IDs:

- original root/runtime row;
- continuation target row;
- incoming state identity;
- cross-row edge kind and source instruction identity;
- bounded cross-row path/transition proof;
- ordinary c53 address provenance chain;
- access kind/width/effective RVA/overlap and instruction identity.

Emit compact exact diagnostics including at minimum:

```text
repository_result
image_authenticated / image_sha256
accepted_c57_boundary_identity
exact_runtime_begin_edges_available = 159734
exact_runtime_begin_edges_considered
exact_runtime_begin_edges_admitted
exact_runtime_begin_edges_refused
continuation_instances_created
continuation_instances_deduplicated
continuation_instances_refused
cross_row_cycles_seen
cross_row_cap_refusals
rows_reached_by_continuation
address_facts_crossing_runtime_rows
call_edges_with_preserved_facts_crossing_rows
remaining_boundary_refusal_relation_counts
reference/reader/writer/rmw/address_only candidate counts
new_cross_row_candidate_count
candidate_sweep_complete
writer_enumeration_complete
all publication/allocation/base/range/lifetime/ownership/destination/CFG/EH gates
```

Do not compare c56 shard-local additive CALL-clobber totals numerically with c55 single-process totals unless the scope is explicitly made identical.

### 7. Completeness boundaries remain false

C58 is not expected to close the overall candidate/writer domain. Preserve:

```text
candidate_sweep_complete = false
writer_enumeration_complete = false
publication_source_kind = unproven
allocation_proven = false
base_proven = false
lifetime_proven = false
ownership_exclusive = false
destination_contained = false
alternate_entries_EH_indirect_writers_closed = false
production_integrated = false
hardware_watch_enabled = false
live_qword_reads = 0
pointer_chase = false
game_used = false
```

Even if no new candidate appears, do not claim no writer. Interior targets, metadata-free RX, indirect calls/writers, EH/alternate entries, executable gaps, unsupported semantics and deferred producer remain open.

C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding.

### 8. Focused owned tests

Add a focused test, preferably:

```bash
python3 source/tools/test_slot_publication_stitched.py
```

Cover at minimum:

- ordinary fallthrough to exact adjacent unique runtime begin carries an exact fact;
- conditional fallthrough to exact adjacent begin;
- direct conditional taken to non-adjacent exact begin;
- direct unconditional branch to non-adjacent exact begin;
- CALL fallthrough carries only c54/c55-preserved facts;
- target interior refuses;
- metadata-free RX target refuses;
- overlap/unbacked/nonexec/outside/deferred target refuses;
- model-only successor refuses;
- independent target EMPTY-root analysis remains separate from predecessor continuation;
- two different incoming states remain separate and never fabricate a merge fact;
- identical continuation states deduplicate deterministically;
- within-instance branch merge keeps only exact agreement;
- exact-entry cross-row cycle converges/deduplicates safely;
- state-changing cycle/cap exhaustion refuses and emits no candidate;
- stale/wrong image/schema/tool/c56/c57/row/edge identity refuses;
- exact-slot reader/writer candidate after a safe cross-row path is reported with boundary provenance;
- adjacent non-slot access is not promoted;
- memory-loaded base still does not become a pointer fact;
- deferred producer and external c28 separation remain unchanged;
- historical c38/c53/c54/c55/c56/c57 tests remain green and unchanged.

Synthetic/model success is not real-image evidence.

## Repository result

Emit exactly one:

```text
EXACT_RUNTIME_BEGIN_STITCH_SCANNER_READY
EXACT_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED
```

`READY` means only that the exact-entry continuation mechanism is bounded, fail-closed and tested. It does not mean the private image has been rescanned, that a writer exists/does not exist, or that publication/production is solved.

If the exact image is unavailable to Codex, publish **one owner OFFLINE command** that runs the c58 scanner on the exact pinned image, preferably reusing the owner's c56 checkpoints and c57 JSON read-only, and emits one compact c58 JSON. No game launch, Update-WS, Overstrike, hardware watch, EXE upload or runtime log.

## Required validation

Run from repository root:

```bash
python3 source/tools/test_slot_publication.py
python3 source/tools/test_static_access_v2.py
python3 source/tools/test_slot_publication_cfg.py
python3 source/tools/test_slot_publication_callee.py
python3 source/tools/test_slot_publication_direct_entry.py
python3 source/tools/test_slot_publication_sharded.py
python3 source/tools/test_slot_publication_boundary.py
python3 source/tools/test_slot_publication_stitched.py
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If the focused test uses a different exact filename, run/report that exact command instead.

Expected:

- all historical c38-c57 behavior remains green and immutable;
- only authenticated architectural `exact_unique_runtime_begin` edges can carry existing address facts;
- independent target entry analysis never acquires predecessor facts;
- unsupported targets/cycles/caps refuse fail-closed;
- no value/pointer semantic broadening;
- no production/gameplay behavior change;
- all completeness/publication/ownership/lifetime gates remain false.

## Required outputs

Update/create:

- separate c58 offline tool/mode and focused tests;
- `analysis/V01C58_*` and source mirror as appropriate;
- `docs/PROJECT_STATE.md`: c57 owner VERIFIED-STATIC result historical, c58 current;
- `docs/CODEX_RESULT.md`;
- Wisdoms only if a reusable cross-boundary must/may-analysis rule is established;
- commit and push the implementation branch, then verify remote publication.

Do not overwrite `analysis/V01C57_OWNER_OFFLINE_RESULT.md`.

Final report must follow `docs/CODEX_REPORTING.md`: every executed command, expected/actual result, PASS/SKIP/UNAVAILABLE/BLOCKED, every unrun test with reason, branch + commit SHA, publication verification, and exact owner action if a private-image scan is needed.
