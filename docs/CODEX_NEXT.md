# Next task — run V4 owner-relative provenance trace for c47 `WallRun.this+0x250`

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`, `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`, and current `docs/CODEX_RESULT.md`.

## Accepted evidence

Do not repeat c47 gameplay, RequestSimpleMotion, GravityOrientationOwner, or V1/V2 global `+0x250` scans.

Accepted runtime:

- `WallRun.this+0x250` changes during verified WallRun;
- changes correlate with ordinary reversals and down-hold left/right oscillation;
- direct signed left/right-input semantics are not supported;
- field meaning/units/producer/writability remain UNKNOWN.

Accepted V3 static result:

- exact EXE/Update identities passed;
- known Update `+0x250` reads rediscovered;
- no direct encoded `+0x250` writer in Update, `0x367DD10`, or `0x29BD5F0`;
- Path A: `0x367DD31 -> 0x29C06F0`, entry RCX = owner+`0x80`;
- Path B: `0x367DD3C -> 0x29C18B0`, entry RCX = owner+`0x58`.

## Current V4 implementation

`source/tools/wallrun_scalar_writer_refs.py` now performs bounded CFG-aware owner-relative GPR provenance from exactly those two roots.

It supports only:

- full-64-bit register MOV;
- full-64-bit LEA of proven base + constant, no dynamic index;
- full-64-bit ADD/SUB immediate.

It kills provenance on unsupported/partial writes and memory-loaded destinations, intersects state at CFG merges, kills Win64 volatile GPR provenance after calls, refuses reachable indirect jumps, and never traverses deeper callees.

A memory access is reported only when a proven current base owner-offset plus the encoded displacement equals original owner+`0x250` exactly.

Reachable direct calls are also reported with the uniquely proven RCX owner offset, if one survives at that call site.

## Owner action

No Update-WS, build, deploy, Overstrike or game run is required.

Fast-forward the branch and run only:

`source/tools/windows/Inspect-WallRun250-Writers.ps1`

Return the newly generated:

`MSM2-WS-wallrun250-writers-YYYYMMDD-HHMMSS.json`

Expected schema is 4.

## Result handling

If V4 reports a proven owner-relative `+0x250` write:

- inspect only that writer and its value source/width;
- do not authorize a write test merely from address provenance.

If V4 finds only reads:

- use the reachable direct-call list with proven RCX owner offsets to choose one next bounded producer step.

If V4 finds no owner-relative access:

- use only the bounded call list from the two roots;
- do not return to global displacement scans or adjacent offsets.

If V4 refuses on a concrete decode/CFG/identity condition:

- fix only that demonstrated condition;
- do not broaden scope.

Current runtime remains `v0.1c47 / 0.1.72` and must not be rebuilt or game-tested for this offline step.
