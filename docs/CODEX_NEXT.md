# Next task — WallSprint v0.1c17 Callee Alias Contract + isolated logging

## Base
```text
source branch: codex/v01c16-writer-call-effect
source implementation: 0a00bdeee05ffdae593d6963abd5c24424a2d8ef
source delivery head: 8f66b15a109e503cdfc115eb5a48ebadd7bc4113
next branch: codex/v01c17-callee-alias-contract
version target: v0.1c17
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `docs/LOGGING_CONVENTION.md`, `analysis/V01C16_RUNTIME_RESULT.md`, `analysis/V01C16_WRITER_CALL_EFFECT.md`, `analysis/RDX_XYZ_SOURCE_CONTRACT.md`, and c16 `docs/CODEX_RESULT.md` first.

## Accepted c16 actual-game evidence
```text
A/session1 3875 ms
E/session2 4203 ms
both verified Deinit / retention pass / stale0 / hard0 / Position writes0
A Up dot surface=1
E Up dot surface=0.99999994
```
Both sessions finalized 8 true entries / 8 complete maps with XYZ mask0x7, source W0 and destination W1.

Authenticated writer call:
```text
writer=0x2CFC900..0x2CFC9B7
call=0x2CFC927
return=0x2CFC92C
kind=direct_call
target=0x2B23560
boundary=0x2B23560..0x2B235E9
hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
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
Actual `[entryRDX] XYZ == Position XYZ` remains corroboration only; `xyz_causality_proven=0`.

## Objective A — callee alias contract
Resolve only memory-write aliasing inside authenticated callee `0x2B23560..0x2B235E9` as used by writer call `0x2CFC927`.

For every relevant callee memory write, prove or reject disjointness from:
1. `[entryRDX, entryRDX+0x10)` — the 16-byte source object;
2. the Hero Transform destination object/range needed by exact Position stores.

Preserve exact executable/SHA, writer/current-code identity, exact call/return, target boundary and target hash. Extend the c16 effect classifier only enough to classify effective-address provenance for writes in this exact 137-byte callee.

Allowed classes:
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
Unknown/conflicting pointer provenance fails closed. No arbitrary interprocedural traversal, recursion, nested-call expansion or open-ended fixpoint. Map call-time RCX/RDX/R8/R9 and needed stack args while keeping writer-entry RDX distinct from call-time RDX.

Emit/update:
```text
MOVEMENT_WRITER_CALL_ARGUMENT_CONTRACT
MOVEMENT_CALLEE_ALIAS_CONTRACT
MOVEMENT_WRITER_CALL_CONTRACT
MOVEMENT_RDX_BRANCH_CONTRACT
MOVEMENT_RDX_XYZ_PROOF
MOVEMENT_POSITION_W_PROOF
```

Only after:
```text
all_relevant_write_aliases_resolved=1
source_span_no_write_proven=1
transform_no_write_proven=1
```
may the writer proof resume from `0x2CFC92C` toward exact stores `0x2CFC936/93C/942/945`. Otherwise report one exact callee RVA/mnemonic/address-provenance blocker and stop.

Do not inspect/mutate upstream producer `0x351EB00`. Do not implement movement.

## Objective B — dedicated timestamped WallSprint logging
Implement `docs/LOGGING_CONVENTION.md` in c17.

Required directory relative to the game root:
```text
log_wallsprint\
```
For the current user install this resolves to:
```text
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\log_wallsprint\
```
Do not hard-code that absolute path. Derive the game root and append `log_wallsprint`.

If missing, attempt to create the directory safely. Do **not** silently fall back to the old shared `logs` folder.

Required filename at process/run log creation:
```text
WallSprint_YYYYMMDD-HHMMSS.log
```
Example:
```text
WallSprint_20261024-034812.log
```
Use host local wall-clock time, zero-padded. Keep A→E sessions from the same run in one file. If a same-second name collision occurs, append `_01`, `_02`, etc. without losing the timestamp.

Update README/README_TEST/docs/package notes so human tests use the newest file from `log_wallsprint`. Add tests for path construction, directory creation/failure, timestamp formatting, zero padding, collision handling, and explicit refusal to silently use the old shared `logs` folder.

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
After installing c17:
```text
1. confirm a new file appears in game-root\log_wallsprint\
2. confirm WallSprint_YYYYMMDD-HHMMSS.log naming
3. A/cardinal ~4 s
4. fully end WallRun
5. move/fast-travel
6. E/diagonal ~4 s
7. send that one complete timestamped log
```
No special input. Video only for visible anomaly.

## PASS
A. Callee aliases resolve and writer proof advances or reports a strictly later exact blocker, with dedicated timestamped logging working.

B. One narrower exact callee blocker is reported with all safety gates preserved, with dedicated timestamped logging still working.

Do not implement movement automatically.

## Required outputs
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
Package c17 diagnostic release/source per existing rules.