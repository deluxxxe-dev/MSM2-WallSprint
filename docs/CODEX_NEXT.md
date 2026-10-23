# Next task — WallSprint v0.1c19 Actual-Game Transform Write Gate

## Base
```text
branch: codex/v01c19-transform-write-contract
implementation: b3185cd240a16446a2424ecab9fa0c2605cb00e8
version: v0.1c19 / package 0.1.37
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `docs/LOGGING_CONVENTION.md`, `analysis/V01C18_RUNTIME_RESULT.md`, `analysis/V01C19_TRANSFORM_WRITE_CONTRACT.md`, `analysis/RDX_XYZ_SOURCE_CONTRACT.md`, and c19 `docs/CODEX_RESULT.md` first.

## Status
C19 cloud implementation is complete and validated, but its exact Transform+0x10C model is not actual-game evidence. No c20 implementation is authorized yet.

C18 actual game identified `0x2B2358E` as the first unresolved 4-byte write, root1 +0x10C, possible Hero Transform alias. C19 now records exact operand/value/reaching-state provenance and permits writer continuation only if the actual instruction is exactly the narrowly modeled non-overlapping effect and all dependency checks pass.

Safety remains:
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```
Producer `0x351EB00`, movement, adhesion and camera remain deferred.

## Human runtime gate
Install `WallSprint_v0.1c19_transform_write_contract.zip`. Confirm v0.1c19/package0.1.37, exact supported exe/SHA, and a fresh dedicated timestamped log under:
```text
game-root\log_wallsprint\WallSprint_YYYYMMDD-HHMMSS.log
```
No shared `logs` fallback.

Run normal input:
```text
A/cardinal ~4 s
fully end WallRun
move or fast-travel
E/diagonal ~4 s
```
Then provide the newest complete log. Video is needed only for a visible anomaly.

## Review requirements
Evaluate the actual log for:
- exact `0x2B2358E` MOV destination base/index/scale/displacement and width;
- proven root identity and whether destination is exactly Transform +0x10C;
- value-source provenance and all-reaching-state agreement;
- Position +0x30..+0x3F non-overlap;
- transform identity/reload-carrier/read dependency checks;
- `transform_position_protected_range_unmodified` and `relevant_transform_effects_resolved`;
- next exact callee/writer blocker, or completed XYZ/W causality if reached;
- 8 finalized entry snapshots/maps where available;
- verified Deinit/retention/Surface Up and clean watch restoration;
- zero MOD Position/source/input/movement writes.

## Decision after log
If the actual write/effect contract passes, update project state with only what the log proves and define the next narrow writer-causality or ownership task. If it fails, define only the exact narrower blocker. Do not automatically traverse producer `0x351EB00` or implement movement/adhesion/camera.

Preserve all exact-build/fail-closed, lifecycle, orientation, stale/hard-kill, direct-call, callee identity/CFG, logging and cleanup protections. Preserve independent implementation suitable for Nexus Mods.