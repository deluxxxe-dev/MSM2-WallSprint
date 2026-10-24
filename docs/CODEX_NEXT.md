# Next task — WallSprint v0.1c20 EDI Value Provenance

Branch: `codex/v01c20-edi-value-provenance`.

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `docs/LOGGING_CONVENTION.md`, `analysis/V01C19_RUNTIME_RESULT.md`, `analysis/V01C19_TRANSFORM_WRITE_CONTRACT.md`, `analysis/RDX_XYZ_SOURCE_CONTRACT.md`, and c19 `docs/CODEX_RESULT.md` first.

Accepted c19 actual-game result:
- A/session1 3844 ms and E/session2 4094 ms; both verified Deinit, retention PASS, Surface Up PASS, stale0, hard0, Position writes0.
- Both sessions finalized 8 true entry snapshots / 8 complete maps and XYZ mask `0x7`.
- `0x2B2358E` is exactly a 4-byte `mov` to Hero Transform + `0x10C` via `[RCX+0x10C]`.
- The destination does not overlap Position `+0x30..+0x3F` or the Transform identity/pointer-carrier range.
- Current blocker: the written value comes from `EDI`, but its reaching origin is unresolved. Reason: `exact_write_value_unknown_computed_or_reaching_origins_conflict`.

Objective: trace only EDI reaching definitions at `0x2B2358E` across the authenticated callee CFG and determine whether the Transform+0x10C write can affect any later Position/source-carrier read. Preserve separate branch states and refuse conflicting/unknown origins. If the value/effect becomes exact and irrelevant, resume the existing writer XYZ/W proof only until the next exact blocker.

Do not traverse producer `0x351EB00`. Do not implement movement, Position/source writes, input rewrite, adhesion, camera, animation or traversal. Keep `writable_source_proven=0` unless a later separately authorized task changes the gate.

Preserve dedicated `log_wallsprint/WallSprint_YYYYMMDD-HHMMSS.log` logging with no shared fallback.

User-facing install ZIP naming is now mandatory:
```text
MSM2-WS-v0.1c20.zip
```
Do not append task-description suffixes. Future versions must follow `MSM2-WS-v<version>.zip` per `docs/DEV_RULES.md`.

If a human test is needed: A/cardinal ~4 s -> fully end WallRun -> move/fast-travel -> E/diagonal ~4 s; send the newest complete timestamped WallSprint log. Video only for a visible anomaly.