# Next task — c39 bounded exact-slot non-value runtime access-site capture

## Status

The owner completed the c38 exact-image offline scan and returned:

```text
MSM2-WS-v0.1c38-slot-publication.json
```

Accepted derived evidence is in:

```text
analysis/V01C38_SLOT_PUBLICATION_SCAN_RESULT.md
```

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before implementation read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, this file, c28/c36/c37/c38 analysis, current offline scanner, and the existing hardware-watch/reset/Position diagnostic implementation under `source/src/msm2/orientation_trace.cpp` and `source/src/NativeHooks/immediate_reset_trace.cpp`. Read relevant Wisdoms references.

## Accepted c38 owner scan

Exact-image/tool identity:

```text
slot_rva=0xC2D2580
image_authenticated=1
tool_version=v0.1c38
schema_version=2
slot readable=1
slot writable=1
scan_scope=bounded_offline_direct_and_register_affine_candidates_not_all_writes
```

Local exact-slot candidate result:

```text
reference_candidates=[]
direct_writer_candidates=0
register_affine_writer_candidates=0
writer_candidates=0
register_affine_resolved_operands=1360
```

The 1360 resolved operands prove the c38 register-affine representation executed on the authenticated actual image. None overlapped the target slot.

Coverage remains explicitly incomplete:

```text
selected_byte_sweep_complete=0
candidate_sweep_complete=0
writer_enumeration_complete=0
publication_source_kind=unproven
allocation_proven=0
range_proven=0
lifetime_proven=0
base_proven=0
destination_contained=0
CFG_continuation=0
live_qword_reads=0
pointer_chase=0
reason=xref_coverage_incomplete_indirect_writers_unexcluded
```

Coverage diagnostics:

```text
instructions_decoded=5000000
cap_instructions=5000000
bytes_scanned=17859464
decode_gaps=737
indirect_or_unresolved_operands=1278483
control_flow_boundary_loss=822382
runtime_region_boundary_loss=208548
unsupported_register_mutation=97883
memory_loaded_base=85282
indexed_address=36092
provenance_window_exhausted=20336
partial_register_mutation=4900
segment_unsupported=1985
```

Do **not** interpret zero local writers as no writer. The global instruction cap was reached, control-flow/interprocedural state is intentionally discarded, and memory-loaded bases remain unknown by policy.

## Accepted external positive-control reader

C38 revalidated the already-accepted c28 reader anchor against current exact image code:

```text
site_rva=0x2CF7FE9
width=8
decoded_access=READ
may_write=0
effective_rva=0xC2D2580
address_proof_class=authenticated_external_anchor
origin_rva=0xC2D2480
local_scanner_coverage=0
```

This is a reader and a coverage sanity check only. It is not publication/writer evidence.

## Why the next evidence changes class

Do not respond to c38 by building a generic cross-function symbolic executor or by pointer-chasing the 85,282 `memory_loaded_base` cases. The current branch already has a guarded hardware debug-register watch engine used by reset/Position diagnostics. It includes bounded thread enumeration, per-thread DR lease ownership, foreign-DR refusal, VEH handling, cleanup/restore, and fail-closed admission.

The next useful question is narrower:

> During a bounded authenticated runtime window, which instruction sites actually access the exact static 8-byte slot at `module_base + 0xC2D2580`, without reading or logging the qword value?

A captured write site is only a runtime writer candidate. Absence of a hit is not no-writer proof.

## Current bounded Codex task

### 1. Reuse existing hardware-watch infrastructure; do not create broad interception

Inspect the existing DR/VEH engine first. Reuse its ownership, suspend/resume, thread filtering, foreign-debug-register refusal and cleanup semantics where sound.

Do not add PAGE_GUARD, broad exception interception, detours on arbitrary game code, or a new generic debugger framework merely to make progress.

If the existing engine cannot safely support the exact slot without weakening cleanup/ownership rules, stop fail-closed and document the exact missing mechanism instead of approximating.

### 2. Watch only the exact static slot

Target:

```text
address = authenticated module base + 0xC2D2580
width = 8 bytes
```

This is the address of the slot itself, not the qword value stored in the slot.

Strictly forbidden:

- reading/logging the slot's qword contents;
- logging a numeric pointer loaded from the slot;
- pointer chasing;
- interpreting the slot as pool/array/allocation/owner;
- using the watch to mutate the slot.

No MOD write to the watched slot is allowed.

### 3. Prefer a two-phase bounded diagnostic if the hardware engine supports it safely

**Phase A — positive-control access self-check**

Use an exact 8-byte read/write watch only long enough to prove the mechanism can observe the already-authenticated c28 reader when that path executes.

Positive control:

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
expected effective slot RVA=0xC2D2580
expected access=READ
```

The hardware exception occurs post-instruction. Do not equate `Context.Rip` with the triggering instruction address. Authenticate the triggering site from current code. For this accepted no-unwind target, the pinned c28 current instruction may be used as an exact positive-control mapping only when the post-instruction/current-code relation is revalidated.

If the positive control cannot be observed/authenticated, report that and do not claim writer-watch coverage.

**Phase B — write-only candidate capture**

If safe re-arm is supported, switch the exact same 8-byte target to write-only observation so the known repeated reader cannot consume the event budget.

Capture a bounded set of unique writer-site candidates. Do not treat write-watch silence as proof of no writers.

If safe re-arm from read/write to write-only is not supported by the existing ownership model, use the minimum alternative that preserves cleanup; do not weaken debug-register ownership checks.

### 4. Log only compact non-value metadata

For each accepted slot access event, log only what is needed to identify the instruction and observation coverage, such as:

- session/window identifier;
- thread id;
- post-instruction RIP/RVA;
- triggering instruction RVA if uniquely authenticated;
- module identity;
- decoded access class: read / write / RMW / unresolved;
- decoded memory width;
- exact-slot-overlap verified yes/no;
- current-code verified yes/no;
- watch phase (self-check or writer-only);
- hit count / unique-site count;
- thread coverage/install/foreign-DR/failure summary.

Do **not** log:

- watched qword before/after;
- loaded/stored qword value;
- arbitrary GPR/XMM register dumps;
- raw pointer values;
- full native function bodies;
- arbitrary stack contents.

A thread id and instruction RVA are acceptable diagnostic identity metadata; they do not establish ownership/lifetime.

### 5. Triggering-instruction identity must be fail-closed

The existing DR engine records a post-instruction RIP. For a writer candidate, prove the actual triggering instruction rather than guessing `rip_after - N`.

Allowed bounded approaches include:

- current authenticated runtime-function boundary plus complete bounded decode to the unique instruction ending at post-RIP;
- an existing exact pinned site mapping whose current bytes/form are revalidated;
- a strictly bounded backward candidate decode only if it yields a unique exact-slot memory instruction ending at post-RIP.

If identity is ambiguous, log the post-RIP event as unresolved and do not promote it to a writer-site candidate.

Do not follow arbitrary calls or reconstruct whole functions merely to resolve a hit.

### 6. Thread/watch coverage remains bounded and explicit

Reuse the existing process-thread snapshot/lease model when practical. Preserve its foreign-DR and debugger refusal behavior.

Record coverage honestly:

- threads seen;
- threads installed;
- failed/busy/foreign threads;
- cap reached;
- snapshot-only/rescan policy;
- cleanup restored/pending.

Do not claim complete process writer coverage unless independently proven. New threads or pre-watch writes may remain unobserved.

A writer hit proves only that this instruction wrote the slot during the observed window. No hit proves only `not observed in this bounded window`.

### 7. Keep existing diagnostic modes isolated

Do not run the new slot watch concurrently with existing reset/Position DR windows unless the current ownership model explicitly proves the combination safe.

Prefer serialization or admission refusal over sharing DR slots by assumption.

Do not weaken:

- debugger detection;
- foreign DR refusal;
- suspend/resume cleanup;
- lease identity checks;
- event/window/thread caps;
- existing reset/Position tests.

### 8. Minimum runtime window / owner burden

The implementation should request the shortest real-game action that can exercise the accepted c28 reader and then allow a short write-only observation window.

Do not require A/E directional comparison, multiple wall faces, or repeated identical WallRun captures.

If one ordinary WallRun/WallSprint session is enough for the positive control, use that. The user does not need to land/stand before ending the test; verified WallRun Deinit is the lifecycle closure of interest. Do not require fast travel unless the implemented diagnostic explicitly needs a post-Deinit/loading stimulus and documents why.

### 9. Interpretation after runtime

Possible outcomes:

- **reader self-check + one or more writer candidates:** next task may inspect only those exact writer sites and their bounded source/publication chain;
- **reader self-check + no writer hit:** retain publication unknown; do not claim no writer; decide whether a different bounded stimulus/window or static evidence is justified;
- **reader self-check fails:** fix/diagnose watch coverage before any writer inference;
- **ambiguous triggering instruction:** keep event unresolved.

No writer candidate by itself proves source allocation, extent, lifetime, uniqueness, ownership or critical non-overlap.

## Existing c37/c36 gates remain unchanged

Relative offset proof:

```text
R8D fallthrough=0..0x3FFF
MOVSXD=0..0x3FFF
SHL6 offset=0..0xFFFC0 inclusive
stride=64
32-byte endExtentExclusive=0xFFFE0
overflow_wrap_excluded=1
offset_proven=1
```

Store barrier:

```text
memory_escape=1
pending_unknown_write=1
store_destination_contained=0
control_alias_unexcluded=1
CFG_blocked=1
continuation_allowed=0
```

Safety state:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Producer `0x351EB00` remains deferred. No gameplay/adhesion/camera/animation/physics changes.

## Required owned validation

Add focused regression coverage for at least:

- exact slot address derivation from authenticated module base + RVA;
- 8-byte DR encoding/configuration under the existing Windows watch model;
- read/write self-check classification without reading watched data;
- write-only classification without watched-data reads;
- post-RIP to exact triggering-site authentication success;
- ambiguous triggering-site refusal;
- exact-slot overlap vs adjacent access;
- read vs write vs RMW classification;
- foreign DR occupied refusal;
- debugger-present refusal;
- thread snapshot/install failure and partial coverage reporting;
- event/unique-site cap handling;
- cleanup/restore on normal close, timeout, kill and failure;
- no overlap with existing reset/Position watch ownership;
- c28 positive-control current-code mismatch refusal;
- all c19-c38/native/workspace/offline-scanner regressions retained.

Do not weaken existing deadlines, caps, guard checks or cleanup assertions.

## Required outputs / handoff

Update:

- the minimum native diagnostic implementation/tests;
- `analysis/` with the c39 watch contract and limitations;
- `docs/PROJECT_STATE.md`;
- `docs/CODEX_RESULT.md`;
- reusable Wisdoms facts if appropriate.

Do not advance to publication-chain interpretation until c39 is committed/pushed and owner runtime evidence is reviewed.

After verified push, expected owner action should be Update-WS, Install mods / Run modded game, perform the shortest documented single-session test, and return the versioned c39 runtime log. Do not request the offline scanner again unless c39 itself establishes a reason.
