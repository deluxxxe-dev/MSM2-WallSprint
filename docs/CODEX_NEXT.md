# Next task — WallSprint v0.1c17 Callee Alias Contract + isolated logging

## Base
```text
source branch: codex/v01c16-writer-call-effect
source implementation: 0a00bdeee05ffdae593d6963abd5c24424a2d8ef
source delivery head: 8f66b15a109e503cdfc115eb5a48ebadd7bc4113
next branch: codex/v01c17-callee-alias-contract
version target: v0.1c17
```

Read first:
- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/PROJECT_STATE.md`
- `analysis/V01C16_RUNTIME_RESULT.md`
- `analysis/V01C16_WRITER_CALL_EFFECT.md`
- `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
- c16 `docs/CODEX_RESULT.md`
- `docs/LOGGING_CONVENTION.md` if present

## Accepted c16 actual-game evidence
C16 is an actual-game Diagnostic PASS / narrower blocker.

```text
A/session1 wallrun_duration_ms=3875
E/session2 wallrun_duration_ms=4203
both verified Deinit
both retention_pass=1
both stale_suspend_count=0
both hard_kill_calls=0
both Position_writes_by_mod=0
A final Up dot surface=1
E final Up dot surface=0.99999994
```

Each session finalized 8 true entry snapshots / 8 complete invocation maps with XYZ mask `0x7`, source W=0 and destination W=1.

Authenticated writer-local call:
```text
writer=0x2CFC900..0x2CFC9B7
call_rva=0x2CFC927
return_rva=0x2CFC92C
call_kind=direct_call
target_rva=0x2B23560
target_boundary=0x2B23560..0x2B235E9
target_code_hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
exact_build_verified=1
current_code_verified=1
target_boundary_verified=1
target_complete_decode=1
```

Exact c16 blocker:
```text
effect=source_span_effect_unknown
source_span_no_write_proven=0
source_span_may_write=0
source_span_effect_unknown=1
next_callee_barrier_rva=0x2B23560
next_callee_instruction=mov
reason=callee_memory_write_alias_to_source_stack_or_destination_unresolved
```

Per invocation `[entry RDX] XYZ` still matches exact Position XYZ stores (`mask=0x7`), but `xyz_causality_proven=0`. Historical upstream fields stay unpromoted: `exact_next_producer_rva=0x351EB00`, `caller_owned=0`, `ownership_exclusive=0`, `source_lifetime_exclusive=0`, `downstream_conflicts_proven_absent=0`, `writable_source_proven=0`.

## Objective A — resolve only the c16 callee alias blocker
Resolve only memory-write aliasing inside authenticated callee `0x2B23560..0x2B235E9` as used by writer call `0x2CFC927`.

For every relevant callee memory write, prove or reject disjointness from:
1. `[entryRDX, entryRDX+0x10)` — the 16-byte source object / writer-entry stack object;
2. the Hero Transform destination object/range needed by exact Position stores.

If all relevant writes are proven disjoint, resume the existing writer proof from `0x2CFC92C`. Otherwise emit one exact callee RVA/mnemonic/address-provenance blocker and stop.

Do not inspect or mutate upstream producer `0x351EB00`. Do not implement movement.

### Required bounded alias analysis
Preserve exact executable/SHA, writer current bytes/hash, exact call/return, target boundary and target hash. Extend the c16 effect classifier only enough to classify effective-address provenance for writes in this exact 137-byte callee.

Allowed provenance classes:
```text
callee_private_stack
caller_shadow_or_return_area
source_span_exact
source_span_possible_alias
hero_transform_exact
hero_transform_possible_alias
rip_relative_global
known_disjoint_object
unknown_pointer
```

Track only exact/simple provenance from call-time args, preserved aliases, RSP/allocation, RIP-relative addresses, LEA/MOV/add/sub/copy chains and bounded local branches. Unknown/conflicting pointer provenance fails closed. No recursion, arbitrary interprocedural traversal, nested-call expansion or open-ended fixpoint.

Map call-time RCX/RDX/R8/R9 and needed stack arguments at `0x2CFC927`; keep writer-entry RDX distinct from call-time RDX.

Emit compact diagnostics including:
```text
MOVEMENT_WRITER_CALL_ARGUMENT_CONTRACT
MOVEMENT_CALLEE_ALIAS_CONTRACT
```
with counts and the exact next blocker if unresolved.

Only after:
```text
all_relevant_write_aliases_resolved=1
source_span_no_write_proven=1
transform_no_write_proven=1
```
may the existing writer proof resume from `0x2CFC92C` toward exact stores `0x2CFC936/93C/942/945`. W remains independent. A later blocker must be reported exactly and stop the task.

## Objective B — change WallSprint log path/naming now
This is a user-requested operational change and is part of c17.

### New dedicated folder
WallSprint must stop writing its own diagnostic log into the shared game-root `logs` folder.

Use this dedicated path relative to the Spider-Man 2 game root:
```text
log_wallsprint\
```

For the user's current install this resolves to:
```text
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\log_wallsprint\
```
Do not hard-code the absolute install path; derive the game root exactly as the existing logger already does and append `log_wallsprint`.

If `log_wallsprint` does not exist, attempt to create it safely. If creation/open fails, emit a clear failure through the safest existing fallback channel available, but **do not silently fall back to the shared `logs` directory**, because separating WallSprint from other MOD logs is the purpose of this change.

### Timestamped filename
Create one log filename when the WallSprint log is opened for the process/run:
```text
WallSprint_YYYYMMDD-HHMMSS.log
```
Example:
```text
WallSprint_20261024-034812.log
```
Use the host PC's local wall-clock time. Zero-pad every component. The timestamp is fixed for that log file; do not rename it per WallRun session. A combined A→E test in one game run must remain in the same log.

Do not overwrite an existing same-name file. If a same-second collision is possible, use a deterministic collision-safe suffix such as `_01`, `_02`, etc. while retaining the timestamp.

Update README/README_TEST/docs/package notes so future human tests request the newest file from `log_wallsprint`, not the old generic `logs` location. Preserve log contents/telemetry format unless required by c17 diagnostics.

Add tests for path construction, local timestamp formatting, zero padding, collision handling, directory creation/failure behavior, and explicit refusal to silently use the old shared `logs` path.

## Safety boundary
Even if writer causality completes:
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```
must remain.

Preserve all c7-c16 lifecycle/orientation/stale/hard-kill/secondary/publication-refresh/DR-watch/direct-call/entry-pairing/accounting/branch/call-target/integrity/cleanup protections.

No Position/RDX/source/input/movement/velocity/gravity/adhesion/camera/animation writes, producer traversal, PAGE_GUARD, guessed native game calls, raw user-log publication, or full private game-body distribution.

## Human test
After cloud validation/install:
```text
1. Confirm a new file appears under game-root\log_wallsprint\
2. Confirm filename matches WallSprint_YYYYMMDD-HHMMSS.log
3. A/cardinal ~4 s
4. fully end WallRun
5. move/fast-travel
6. E/diagonal ~4 s
7. send that one complete timestamped WallSprint log
```
No special input. Video only for a visible anomaly.

## PASS
A. All relevant callee write aliases resolve, source/Transform no-write is proven, writer proof advances or reports a strictly later exact blocker; **and** dedicated timestamped logging works.

B. One narrower exact callee address-provenance blocker is reported with all safety/regression gates preserved; dedicated timestamped logging must still work.

Do not implement movement automatically.

## Required outputs
Update/create:
```text
analysis/V01C16_RUNTIME_RESULT.md
analysis/V01C17_CALLEE_ALIAS_CONTRACT.md
analysis/V01C17_GATE.md
analysis/RDX_XYZ_SOURCE_CONTRACT.md
README_TEST.md
README.md
docs/PROJECT_STATE.md
docs/LOGGING_CONVENTION.md
docs/CODEX_RESULT.md
```
Package c17 diagnostic release/source per existing project rules.