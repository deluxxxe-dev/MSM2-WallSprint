# Next task — c54 authenticated direct-callee preservation summaries for offline exact-slot CFG scan

## Status

C53 repository scanner work is complete and the owner ran the published offline command against the exact pinned image.

Accepted owner result:

- `analysis/V01C53_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C53_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, c38/c53 slot-publication evidence, current `source/tools/slot_publication.py`, `source/tools/slot_publication_cfg.py`, their tests, `source/tools/static_access_v2.py`, c52 production-watch evidence, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c53 owner offline facts

Evidence class is `VERIFIED-STATIC`: exact authenticated on-disk image, no game/runtime execution.

```text
image_authenticated = true
image_sha256 = c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
slot_rva = 0xC2D2580
repository_result = CFG_SLOT_PUBLICATION_SCANNER_READY
reference_candidates = []
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
live_qword_reads = 0
pointer_chase = false
hardware_watch_enabled = false
production_integrated = false
game_used = false
```

This is **not** no-writer proof. Completeness/publication/ownership gates remain false:

```text
candidate_sweep_complete = false
writer_enumeration_complete = false
publication_source_kind = unproven
base/allocation/range/lifetime/ownership/destination/CFG continuation = false
alternate_entries_EH_indirect_writers_closed = false
```

The accepted c28 reader was revalidated separately at `0x2CF7FE9` with origin `0xC2D2480`; it remains external reader evidence only and is not local CFG coverage.

C53 real-image coverage reached the global instruction cap:

```text
runtime_functions_in_metadata = 527161
runtime_functions_not_considered = 352663
regions_considered = 174498
regions_modeled = 65455
regions_refused = 109043
instructions_decoded = 5000000 (cap reached)
states_discovered = 3687652
edges_traversed = 3716504
previously_control_lost_edges_safely_modeled = 10471
control_edges_without_agreed_origin = 179106
unresolved_memory_operands = 560919
```

Largest unresolved address-provenance categories:

```text
call_clobber_all_address_origins = 324714
register_base_without_exact_origin = 118219
unsupported_register_mutation = 62079
memory_loaded_base = 20287
cfg_merge_unknown = 16749
indexed_address = 14775
```

The dominant newly exposed blind spot is therefore the intentionally conservative c53 rule that destroys every address-origin register fact after every direct CALL.

C52 production-watch FAIL remains binding. Do not reopen hardware-watch or production witness integration in c54.

## Current bounded task

C54 is an **offline/static direct-callee preservation-summary task**. Reduce only the `call_clobber_all_address_origins` class where preservation can be proven from authenticated code. Do not use ABI assumptions and do not propagate return values or loaded pointers.

### 1. Preserve c53 semantics and historical output

Do not rewrite or relabel the accepted c53 owner result. Prefer a new tool revision/mode (`v0.1c54`) while keeping c38/c53 tests and semantics reproducible.

The c28 external anchor stays separate from local coverage and must not seed callee summaries.

### 2. Define a strict per-register direct-callee preservation summary

For a direct CALL, an incoming exact static-image origin may survive to the caller fallthrough only for a specific non-RSP GPR when all of the following are proven:

- call target resolves exactly to one authenticated code entry/span;
- the entire admitted callee summary domain is canonically decoded with no byte gaps or middle-entry ambiguity;
- every reachable normal path is bounded and terminates in a normal RET;
- every reachable instruction effect is understood well enough to decide whether that GPR is written;
- the GPR is never written on any reachable path;
- every nested direct CALL is either independently summarized under the same rule or causes refusal for that register;
- indirect call/jump, system transfer, unresolved target, unsupported runtime boundary, decode gap, cap exhaustion, unknown register effects, recursion/cycle beyond the explicit bounded summary contract, or any write to the register causes refusal/kill;
- deferred producer exclusion and all current authenticated-image boundaries are preserved.

Do not infer preservation from Windows x64 ABI callee-saved conventions. Static code proof is required.

Do not attempt value-transform summaries in c54. A register that is written and later restored still counts as **not proven preserved** for this task unless a separately explicit exact restoration proof is designed and authorized later.

Always kill any image-address fact held in RSP across CALL in c54; do not introduce stack-balance reasoning merely to increase coverage.

### 3. Bounded recursive summaries only

Nested direct-callee summaries may be reused through a deterministic cache, but the summary graph must have strict finite caps for at least:

- callee depth;
- total summary functions;
- instructions/blocks/edges/states per summary;
- total summary work;
- recursion/SCC detection.

A cycle or cap exhaustion must fail closed. Do not approximate a recursive component as preserved.

Summary identity must include authenticated image identity and exact callee entry/span/code hash sufficient to prevent stale reuse.

### 4. Integrate summaries narrowly into caller CFG provenance

At a direct CALL:

- preserve only incoming exact address facts for registers whose callee summary explicitly says `PRESERVED`;
- kill all other address-origin register facts as c53 does;
- do not create any new origin from return registers;
- do not propagate memory-loaded values, stack values, constants, or callee-local addresses back to the caller;
- keep merge semantics as a must-analysis.

Candidate classification remains identical: exact address proof and READ/WRITE/RMW/ADDRESS_ONLY semantics stay separate.

### 5. Report summary evidence and real coverage impact

New output must include at minimum:

```text
callee_summaries_attempted
callee_summaries_admitted
callee_summaries_refused
callee_summary_refusal_reasons
nested_summaries_used
summary_cycles_refused
call_edges_with_any_preserved_origin
preserved_origin_register_facts_across_calls
call_clobber_all_address_origins_remaining
c53_call_clobber_all_address_origins_baseline = 324714
reference/reader/writer/rmw/address_only candidate counts
all existing completeness/publication gates
```

Also report enough per-summary provenance for audit without emitting whole proprietary function bodies: entry RVA, bounded span identity/hash, preserved-register set, nested-summary identities, cap/refusal reason.

Do not claim that a reduced unresolved count closes writer completeness.

### 6. Keep unsupported classes unsupported

Do not broaden into these c54 areas:

- memory-loaded base or pointer chase;
- return-value provenance;
- indexed addressing;
- partial-register restoration reasoning;
- arbitrary arithmetic/shift/immediate-pointer materialization;
- indirect call/jump target recovery;
- EH/alternate-entry closure beyond what is already independently authenticated;
- producer `0x351EB00` traversal;
- slot qword reads/logging;
- hardware watch/PAGE_GUARD/broad interception;
- publication/allocation/base/lifetime/ownership/containment inference;
- production/gameplay changes.

### 7. Focused owned tests

Add a focused command, preferably:

```bash
python3 source/tools/test_slot_publication_callee.py
```

Cover at minimum:

- leaf direct callee that never writes RCX => RCX origin survives;
- callee writes RCX on one branch => RCX killed;
- multiple normal RET paths all preserve => preserve;
- one path with unsupported effect => refuse;
- nested direct callee preserved => caller may preserve;
- nested callee writes register => caller kills;
- recursion/cycle => refuse;
- indirect call/jump/system transfer => refuse;
- decode gap/middle entry/out-of-span target => refuse;
- per-summary/global cap exhaustion => refuse;
- RSP fact always killed across CALL;
- write-then-restore register remains refused under c54's narrow rule;
- exact slot reader/writer after a safely summarized CALL is classified correctly;
- adjacent/non-overlap still excluded;
- memory-loaded base still unknown;
- deferred producer span remains excluded;
- external c28 anchor remains separate;
- c38 and c53 tests remain unchanged/green.

Synthetic/model success is not real-image evidence.

### 8. Repository result and owner handoff

Emit exactly one repository result:

```text
CALLEE_PRESERVATION_SLOT_SCANNER_READY
CALLEE_PRESERVATION_SLOT_SCANNER_BLOCKED
```

`READY` means the summary rule is implemented, bounded, fail-closed and tested. It does not mean a writer/publication source is found.

If the exact authenticated image is unavailable to Codex, publish one owner offline command and request only the compact c54 JSON. No game launch or Update-WS is needed.

If the exact image is legitimately available, run the scan as OFFLINE/STATIC only and preserve incompleteness flags.

## Required validation

Run from repository root:

```bash
python3 source/tools/test_slot_publication.py
python3 source/tools/test_static_access_v2.py
python3 source/tools/test_slot_publication_cfg.py
python3 source/tools/test_slot_publication_callee.py
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If the focused c54 test uses a different exact filename, run and report that exact command.

Expected result:

- historical c38/c53 behavior remains green;
- c53 owner real-image result remains preserved as incomplete zero-local-candidate evidence;
- only statically proven per-register direct-callee preservation is added;
- unsupported/capped/cyclic summaries refuse;
- no qword read, pointer chase, hardware watch, production integration or gameplay change occurs.

## Required outputs / handoff

Update as appropriate:

- `analysis/V01C54_*` evidence/gate/status;
- c54 tool/test files;
- `docs/PROJECT_STATE.md` so the accepted c53 owner offline result is recorded and c54 is current;
- `docs/CODEX_RESULT.md`;
- relevant Wisdoms reusable static-analysis rule if a generally useful direct-callee preservation contract is established.

Commit and push the intended branch before claiming completion.

The final report must follow `docs/CODEX_REPORTING.md` and include:

- every executed test/check;
- exact command;
- expected result;
- actual result;
- PASS / SKIP / UNAVAILABLE / BLOCKED separately;
- every unperformed test and reason;
- branch and commit SHA;
- whether owner action is required.

## Forbidden scope

C54 does not authorize:

- production `orientation_trace.cpp` behavior changes;
- production watch/retirement/witness integration;
- exact-slot hardware watch;
- PAGE_GUARD or broad memory interception;
- slot qword reads/logging or pointer chasing;
- indirect-target guessing;
- ABI-based callee preservation assumptions;
- deferred producer `0x351EB00` traversal;
- mitigation changes;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes;
- game release.

Runtime remains `v0.1c38 / package0.1.56`.

## Owner action

No owner action is required while Codex implements c54. If Codex reaches `CALLEE_PRESERVATION_SLOT_SCANNER_READY` without access to the exact image, the only next owner action should be one offline c54 scan and return of its compact JSON output.