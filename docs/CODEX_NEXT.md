# Next task — c55 authenticated direct-CALL entry summaries for metadata-missing callees

## Status

C54 repository work is complete and the owner ran the published offline command against the exact pinned image.

Accepted owner result:

- `analysis/V01C54_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C54_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, c38/c53/c54 slot-publication evidence and tools/tests, c52 production-watch evidence, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c54 owner offline facts

Evidence class is `VERIFIED-STATIC`: exact authenticated on-disk image, no game/runtime execution.

```text
image_authenticated = true
image_sha256 = c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
slot_rva = 0xC2D2580
repository_result = CALLEE_PRESERVATION_SLOT_SCANNER_READY
callee_summaries_attempted = 24411
callee_summaries_admitted = 0
callee_summaries_refused = 24411
target_not_exact_authenticated_runtime_entry = 22538
all refusal chains containing target_not_exact_authenticated_runtime_entry = 23141
summary_requests = 44483
summary_cache_hits = 20072
summary_functions_loaded = 1872
summary_instructions_decoded = 74061
summary_work = 602704
summary_cycles_refused = 157
global_summary_budget_exhausted = false
call_edges_with_any_preserved_origin = 0
preserved_origin_register_facts_across_calls = 0
c53_call_clobber_all_address_origins_baseline = 324714
call_clobber_all_address_origins_remaining = 324714
reference_candidates = []
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
```

The direct exact-runtime-entry refusal alone is about 92.3% of all c54 summary refusals; including nested propagation it is about 94.8%. C54 therefore proved the preservation engine is fail-closed on the real image but admitted no real-image summaries and produced no provenance improvement over c53.

This remains incomplete zero-observation, not no-writer proof. Main scan still hit 5,000,000 instructions; 352,663 runtime functions were not considered; 560,919 memory operands remain unresolved. All completeness/publication/base/allocation/range/lifetime/ownership/destination/CFG-continuation/EH-indirect-writer gates remain false.

C52 production-watch FAIL remains binding. Do not reopen hardware watch, VCH production integration, slot-value reads, producer traversal, gameplay or runtime instrumentation in c55.

## Current bounded task

C55 is an **offline/static exact direct-CALL entry authentication task**. The goal is to determine whether metadata-missing direct callees can be summarized without weakening c54's never-written rule or pretending PE runtime-function metadata is complete function-entry coverage.

### 1. Preserve c53/c54 historical semantics and accepted results

Do not rewrite or relabel c53 or c54 owner evidence. Prefer a separate c55 tool/mode (`v0.1c55`) while keeping c38/c53/c54 default behavior and tests reproducible.

Do not weaken the current exact-runtime-entry c54 path. A metadata-relative fallback must be separately named and separately metered.

### 2. Classify every c55 direct-callee target before any fallback

For each unique requested direct target and for request/callsite totals, report a deterministic metadata relation at minimum:

```text
exact_runtime_entry
inside_unique_runtime_span_nonentry
overlapping_multiple_runtime_spans
executable_file_backed_outside_runtime_metadata
executable_not_fully_file_backed
non_executable_or_writable_section
outside_image
deferred_producer_excluded
```

Where useful report bounded nearest runtime begin/end distances for diagnostics only. **Nearest metadata is never admission proof.**

Report target-frequency summaries and bounded callsite examples without exporting whole proprietary function bodies.

Do not infer that an address inside a runtime span is a valid secondary entry merely because a CALL reaches it. Keep `inside_unique_runtime_span_nonentry` fail-closed in c55 unless the existing authenticated canonical partition independently proves the exact target is an instruction boundary and a separately explicit subentry contract is designed; that is not required for c55 READY.

### 3. Define a separate exact direct-CALL entry proof for targets outside runtime metadata

A decoded immediate direct CALL from an already authenticated caller establishes an **exact invocation entry edge**, not a function span or ABI contract.

For `executable_file_backed_outside_runtime_metadata` only, c55 may create a separate summary contract, preferably:

```text
direct_call_entry_normal_CFG_never_written_per_GPR_v1
```

Admission requires all of the following:

- exact pinned image identity is already authenticated;
- the target comes from an immediate direct CALL in an authenticated caller CFG; no guessed/search/nearest target;
- target is outside every runtime-function span, inside exactly one read-only executable file-backed section, and outside deferred exclusions;
- decoding starts exactly at that direct-CALL target and follows **reachable normal control flow only**; do not invent a contiguous function end from nearby metadata;
- every reached instruction byte is file-backed and immutable under the frozen input;
- reached instruction intervals form one consistent non-overlapping decode; any branch into the middle of an already decoded instruction, conflicting overlap, decode ambiguity/gap, or stale-byte mismatch refuses;
- every reached direct branch target stays in the admitted file-backed executable domain and is decoded under the same exact reachable graph contract;
- every reachable normal path is finite under explicit caps and ends in a plain normal RET; cycles, unresolved/tail exits outside the admitted domain, indirect call/jump, system transfer, unsupported boundary/effect, or cap exhaustion refuse;
- nested direct CALLs must be independently admitted either by the unchanged c54 exact-runtime-entry contract or this same c55 direct-entry contract; a refused child refuses the affected parent proof;
- every instruction effect is known well enough to decide GPR writes;
- a non-RSP GPR is `PRESERVED` only when never written on any admitted reachable path or child summary; write-then-restore still fails preservation;
- RSP is always killed;
- no ABI preservation, return-value provenance, loaded pointer, stack/local value, or callee-local address is imported into the caller.

A direct-CALL target does **not** prove alternate entries, EH closure, a conventional function boundary, ownership, publication, allocation or lifetime.

### 4. Identity, caching and bounds must remain fail-closed

For the metadata-free direct-entry contract, cache identity must bind at minimum:

- exact image SHA;
- exact direct entry RVA;
- contract version;
- deterministic hash of every reached instruction address+bytes or equivalent immutable reachable-code digest;
- deterministic reachable CFG/edge hash;
- child summary identities.

Do not cache by nearest runtime span or section alone.

Keep strict finite caps for depth, functions/entries, total work, decoded bytes/instructions, states, edges, blocks, and recursion/cycles. Positive cache reuse must not bypass the current depth/work contract. Global exhaustion remains closed.

### 5. Integrate narrowly into caller provenance

At CALL fallthrough preserve only already-existing exact non-RSP origins whose selected summary explicitly proves `PRESERVED`. Kill everything else as c53/c54 already do.

Do not create origins from return registers, constants, stack, memory loads or callee-local addresses. Must joins and access classification remain unchanged.

The c28 external reader remains separate and must not seed local summaries or caller facts.

### 6. Required diagnostics / real-image output

New c55 output must include at minimum:

```text
c54_runtime_entry_summary_attempted/admitted/refused
direct_entry_summary_attempted/admitted/refused
direct_entry_refusal_reasons
target_metadata_relation_request_counts
target_metadata_relation_unique_counts
direct_entry_unique_targets_considered
direct_entry_cache_hits
direct_entry_nested_summaries_used
direct_entry_cycles_refused
call_edges_with_any_preserved_origin
preserved_origin_register_facts_across_calls
call_clobber_all_address_origins_remaining
c53_call_clobber_all_address_origins_baseline = 324714
reference/reader/writer/rmw/address_only candidate counts
all existing completeness/publication gates
```

Preserve enough bounded provenance to audit admitted direct-entry summaries: entry RVA, reachable-code/CFG identity hashes, preserved-register set, child summary IDs, cap/refusal reason. Do not emit full proprietary bodies.

If the fallback admits nothing, report exactly which target classes dominate rather than weakening guards.

### 7. Keep unsupported classes unsupported

Do not broaden c55 into:

- runtime-span interior/subentry admission unless independently canonical and explicitly justified beyond the optional diagnostic above;
- ABI callee-saved assumptions;
- memory-loaded base/pointer chase;
- return-value provenance;
- indexed/segment generality;
- arbitrary arithmetic/shift/immediate-pointer materialization;
- indirect call/jump target recovery;
- EH/alternate-entry closure;
- producer `0x351EB00` traversal;
- slot qword reads/logging;
- hardware watch/PAGE_GUARD/broad interception;
- publication/allocation/base/lifetime/ownership/containment inference;
- production/gameplay changes.

### 8. Focused owned tests

Add a focused command, preferably:

```bash
python3 source/tools/test_slot_publication_direct_entry.py
```

Cover at minimum:

- metadata exact entry still uses unchanged c54 contract;
- metadata-free direct-call leaf with RCX never written => RCX origin may survive;
- one reachable branch writes RCX => RCX killed;
- multiple finite normal RET paths all preserve => preserve;
- write-then-restore still not preserved;
- direct target inside one runtime span but not exact begin remains refused by c55 fallback;
- target outside all runtime metadata but in unique RX file-backed section can be considered;
- target in writable/nonexec/unbacked/outside/deferred domain refuses;
- overlapping/conflicting instruction decode or branch-into-middle refuses;
- direct branch leaving admitted file-backed executable domain refuses;
- indirect call/jump/system transfer refuses;
- nested exact-runtime-entry child may be reused when admitted;
- nested metadata-free child may be independently admitted;
- refused child refuses parent;
- recursion/cycle and all per-summary/global caps refuse;
- RSP always killed;
- no return/local/load/stack origin creation;
- exact slot reader/writer after a safely admitted direct-entry call is classified correctly;
- adjacent/non-overlap remains excluded;
- external c28 anchor remains separate;
- deferred producer remains excluded;
- historical c38/c53/c54 tests remain unchanged/green.

Synthetic/model success is not real-image evidence.

### 9. Repository result and owner handoff

Emit exactly one repository result:

```text
DIRECT_CALL_ENTRY_SLOT_SCANNER_READY
DIRECT_CALL_ENTRY_SLOT_SCANNER_BLOCKED
```

`READY` means the separate direct-entry proof is bounded, fail-closed and tested. It does not mean a real-image callee is admitted or a writer/publication source is found.

If static review shows the metadata-free direct-entry proof cannot satisfy the obligations above without an unsafe inferred boundary, emit `BLOCKED` with the first exact unsatisfied guarantee; do not weaken the contract.

If the exact authenticated image is unavailable to Codex, publish one owner OFFLINE command and request only compact c55 JSON. No game launch, Update-WS or Overstrike is needed.

If the exact image is legitimately available, run OFFLINE/STATIC only and preserve incompleteness flags.

## Required validation

Run from repository root:

```bash
python3 source/tools/test_slot_publication.py
python3 source/tools/test_static_access_v2.py
python3 source/tools/test_slot_publication_cfg.py
python3 source/tools/test_slot_publication_callee.py
python3 source/tools/test_slot_publication_direct_entry.py
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If the focused c55 test uses a different exact filename, run and report that exact command.

Expected result:

- historical c38/c53/c54 behavior remains green and c54 owner real-image result remains immutable;
- metadata relation is explicitly classified before fallback;
- only exact direct-CALL-anchored, metadata-free, reachable-normal-CFG never-written proofs can preserve existing facts;
- no nearest-boundary heuristic, ABI assumption or inferred function span is used;
- unsupported/capped/cyclic/ambiguous cases refuse;
- no qword read, pointer chase, hardware watch, production integration or gameplay change occurs.

## Required outputs / handoff

Update as appropriate:

- `analysis/V01C55_*` evidence/gate/status;
- c55 tool/test files;
- `docs/PROJECT_STATE.md` so the accepted c54 owner offline result is recorded and c55 is current;
- `docs/CODEX_RESULT.md`;
- relevant Wisdoms reusable static-analysis rule if the exact direct-CALL entry contract is established or rejected.

Commit and push the intended branch before claiming completion.

The final report must follow `docs/CODEX_REPORTING.md` and include every executed command, expected/actual result, PASS/SKIP/UNAVAILABLE separately, every unrun test with reason, branch+SHA, and exact owner action if one remains.
