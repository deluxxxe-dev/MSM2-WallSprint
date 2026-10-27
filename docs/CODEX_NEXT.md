# Next task — c53 CFG-aware offline exact-slot publication scan

## Status

C52 production-shape VCH witness integration review is complete and failed closed:

```text
PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL
```

Accepted c52 evidence is in:

- `analysis/V01C52_PRODUCTION_VCH_WITNESS_REVIEW.md`
- `analysis/V01C52_GATE.md`
- `analysis/V01C52_STATUS.json`
- `docs/CODEX_RESULT.md`

The accepted c51 native one-debt bridge PASS remains valid, but c52 established that production cannot currently guarantee a complete terminal/final-CONTEXT VCH boundary for every admitted saved-context obligation. Do not continue production witness integration or exact-slot hardware-watch work from that evidence.

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, `analysis/V01C38_SLOT_PUBLICATION_SCAN_RESULT.md`, current `source/tools/slot_publication.py`, `source/tools/test_slot_publication.py`, `source/tools/static_access_v2.py`, `source/tools/test_static_access_v2.py`, c52 evidence, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c38 static evidence

Exact target remains build-specific:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
slot RVA = 0xC2D2580
accepted external reader = 0x2CF7FE9
external origin RVA = 0xC2D2480
```

The c38 authenticated offline scanner found no local direct/register-affine writer candidate, but explicitly did **not** prove writer completeness. Its dominant unresolved category was control-flow provenance loss:

```text
control_flow_boundary_loss = 822382
runtime_region_boundary_loss = 208548
memory_loaded_base = 85282
writer_enumeration_complete = 0
publication_source_kind = unproven
reason = xref_coverage_incomplete_indirect_writers_unexcluded
```

C38 intentionally discarded exact address provenance at every branch/call/return/system boundary. Separately, `static_access_v2.py` already contains a bounded CFG must-analysis pattern whose merge keeps facts only when all incoming states agree exactly.

C53 returns to this static evidence path because production hardware-watch retirement remains blocked by c52. It must not weaken the c52 fail-closed result.

## Current blocker

Reduce the c38 `control_flow_boundary_loss` blind spot using conservative, bounded CFG-aware **address provenance only**, without reading the slot qword, propagating loaded pointer values, enabling a hardware watch, or claiming all-writer completeness.

C53 is an offline/static tooling task. It does not authorize production/gameplay changes.

## Current bounded task

### 1. Preserve c38 historical semantics

Do not silently reinterpret or overwrite c38 evidence.

Prefer a separate tool/mode such as:

```text
source/tools/slot_publication_cfg.py
```

with a separate schema/tool revision. Shared pure helpers may be extracted only if existing c38 tests prove the old scanner's semantics/results remain unchanged.

Keep the accepted c28 reader anchor separate from local scanner coverage. Do not inject the external anchor as an entry state or use it to claim local writer coverage.

### 2. Add bounded CFG must-analysis for exact static-image address provenance

Use only the exact authenticated executable and pinned Capstone version already required by c38.

For executable code covered by authenticated PE/runtime metadata, construct bounded local CFG states and propagate only exact static-image address origins.

Required supported control flow:

- normal fallthrough;
- direct conditional branch: both target and fallthrough;
- direct unconditional branch: target only;
- bounded loops/fixpoint convergence;
- direct CALL may continue at fallthrough only after conservatively invalidating all address-origin register facts unless an already-verified explicit summary is used; do not invent ABI preservation;
- RET, indirect jump/call, system transfer, unresolved target, branch into undecodable/middle bytes, decode gap, unsupported runtime boundary or cap exhaustion => fail closed for the affected state/path.

State merge must be a must-analysis: retain a register origin only when every incoming reachable state agrees on the exact same origin/provenance. Any disagreement, missing fact or unknown incoming state removes that fact. Never union alternatives into a speculative exact origin.

Use deterministic finite caps for decoded instructions, CFG edges, states, worklist iterations, blocks/functions and per-register provenance hops. Cap exhaustion must be reported and must keep completeness false.

### 3. Keep the address-provenance language narrow

C53 may retain the already accepted c38 forms:

- RIP-relative LEA exact image origin;
- full-64-bit register copy of an exact origin;
- checked full-64-bit LEA with constant displacement;
- exact constant memory displacement from a proven full-64-bit base.

Keep these fail closed:

- memory-loaded base / pointer chase;
- value propagation from the target qword or any other memory load;
- partial-register writes;
- unsupported arithmetic/shift/immediate pointer materialization;
- indexed addressing unless independently proven in this task with a constant, value-free rule; default is refuse;
- segment addressing;
- unknown register effects;
- unresolved indirect control flow;
- unsupported interprocedural assumptions.

Do not read or emit the slot's qword contents. `live_qword_reads` must remain zero and `pointer_chase` false.

### 4. Exact access classification

For every memory operand whose address is exactly proven to overlap the 8-byte target slot, report at minimum:

- instruction/site RVA;
- instruction bytes or stable bytes hash;
- decoded mnemonic/operands;
- operand width;
- READ / WRITE / RMW / ADDRESS_ONLY classification;
- exact effective RVA and overlap extent;
- address-proof chain/class;
- containing PE section;
- containing authenticated runtime-function span when available;
- CFG entry/block/path provenance summary sufficient to audit why the address is exact.

Encoding-verified stores must remain writes even if decoder metadata is incomplete. Unsupported access semantics must refuse rather than downgrade a possible write to read-only.

### 5. Coverage accounting and comparison with c38

The output must separately report:

- runtime functions/regions considered;
- CFG entries/blocks/edges traversed;
- fallthrough/direct-branch edges;
- merge count and conflicting facts discarded;
- loop/state revisits;
- decoded instruction/byte totals and caps;
- paths/states stopped for every unresolved reason;
- exact reader/writer/address-only candidates;
- legacy c38 unresolved categories where comparable;
- how many previously-control-boundary-lost states are now safely modeled versus still unresolved.

A smaller unresolved count is useful evidence, not proof of completeness.

If no writer is found, keep `writer_enumeration_complete=false` unless every relevant instruction-entry/control-flow/indirect-writer class is independently closed. C53 is not expected to establish such completeness.

### 6. Preserve deferred and safety boundaries

Keep the deferred producer `0x351EB00` / its authenticated runtime span excluded exactly as current policy requires. Report that exclusion explicitly so it cannot be mistaken for coverage.

Do not infer from a candidate or lack of candidates:

- publication ownership;
- allocation identity;
- base lifetime;
- exclusive writability;
- destination containment;
- downstream conflict absence;
- permission for gameplay writes.

C36 store/CFG barrier, c37 range proof, c38 external reader, c39-c52 retirement evidence and all existing gameplay/write gates remain unchanged.

### 7. Owned focused tests

Add focused synthetic tests for the CFG scanner. At minimum cover:

- exact origin surviving a direct conditional branch when both paths preserve the same fact;
- one branch mutates the base => merged origin becomes unknown;
- direct unconditional jump propagation;
- loop convergence with a stable fact;
- loop mutation/conflict => unknown;
- direct CALL fallthrough invalidates unsafe address facts;
- indirect jump/call refuses;
- branch into middle/undecodable bytes refuses;
- cap exhaustion remains incomplete;
- exact reader and writer detection after safe branch propagation;
- adjacent/non-overlapping storage excluded;
- memory-loaded base never becomes pointer provenance;
- partial/unsupported mutation invalidates origin;
- external c28 anchor remains separate from local coverage;
- deferred producer region remains excluded;
- legacy c38 tests stay unchanged/green.

Synthetic fixture success is not real-image evidence.

### 8. Repository result and optional owner offline run

After repository tests, emit one primary repository result:

```text
CFG_SLOT_PUBLICATION_SCANNER_READY
CFG_SLOT_PUBLICATION_SCANNER_BLOCKED
```

`READY` means the conservative scanner/tooling is implemented, bounded and tested. It does not mean a writer/publication source is found.

If the exact authenticated `Spider-Man2.exe` is unavailable in the Codex environment, do not fabricate a real-image result. Publish/document one exact owner **offline** command using the new scanner and request only its compact JSON output. No game launch is needed.

If the exact image is available and legitimately supplied in the task environment, a real-image scan may be run, but classify it as OFFLINE/STATIC and preserve all incompleteness flags.

## Required validation

Run from repository root:

```bash
python3 source/tools/test_slot_publication.py
python3 source/tools/test_static_access_v2.py
python3 source/tools/test_slot_publication_cfg.py
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If the new focused test uses a different exact filename, run and report that exact command instead of inventing a duplicate.

Expected result:

- historical c38 scanner/tests remain green;
- new CFG must-analysis preserves exact provenance only across supported direct CFG edges;
- conflicting merges and unsupported edges refuse;
- no memory-value propagation/pointer chase/live qword read is introduced;
- c52 production-watch FAIL remains unchanged;
- no production/exact-slot/gameplay behavior is enabled.

## Required outputs / handoff

Update as appropriate:

- new CFG-aware offline scanner/tool and focused tests;
- `analysis/V01C53_*` design/gate/status evidence;
- `docs/PROJECT_STATE.md` so c52 FAIL is historical and c53 is current;
- `docs/CODEX_RESULT.md`;
- relevant Wisdoms rule only if a reusable CFG/static-provenance rule is established.

Commit and push the intended branch before claiming completion.

The final report must follow `docs/CODEX_REPORTING.md` and include:

- every executed test/check;
- exact command;
- expected result;
- actual result;
- PASS / SKIP / UNAVAILABLE / BLOCKED separately;
- every unperformed test and reason;
- branch and commit SHA;
- whether owner action is required and, if so, the single exact offline command.

## Forbidden scope

C53 does not authorize:

- production continuation-witness/VCH integration;
- any new hardware-breakpoint watch, including the exact-slot watch;
- reading/logging the slot qword;
- pointer chasing or interpreting loaded qwords as pointers;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- production `orientation_trace.cpp` behavior changes;
- mitigation changes;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c53.

## Owner action

No owner action is required until c53 repository work has built, tested, committed and pushed. If a real-image offline scan remains necessary, request exactly one offline command and compact JSON. Do not request Update-WS or game launch.
