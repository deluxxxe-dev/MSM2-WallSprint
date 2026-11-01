# Next task — run V7 single Path-B handoff trace for c47 `WallRun.this+0x250`

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`, `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`, `analysis/WALLRUN250_V6_OWNER_RESULT_20261101.md`, and current `docs/CODEX_RESULT.md`.

## Accepted evidence

Do not repeat c47 gameplay, RequestSimpleMotion, GravityOrientationOwner, V1/V2 global scans, or V4/V5 structural debugging.

V6 succeeded on the exact pinned EXE with schema 6:

`NO_OWNER_RELATIVE_0X250_ACCESS_IN_BOUNDED_DOWNSTREAM_TRACE`

Accepted V6 facts:

- Path A `0x29C06F0`, entry RCX owner+`0x80`: 30 reachable instructions, contained CFG cycle, no `owner+0x250` access, no reachable direct call, no external tail jump. Do not reopen Path A without new evidence.
- Path B `0x29C18B0`, entry RCX owner+`0x58`: no `owner+0x250` access; exactly one reachable direct call preserves unique RCX owner provenance.
- selected handoff: `0x29C18D4 -> 0x308A0F0` with pre-call `RCX = owner+0x58` and `RDI = owner+0x58`.
- combined V6 access/read/write/unknown counts are all zero.

This is a bounded no-access result only. Field meaning/units/producer/writability remain UNKNOWN.

## Current V7 implementation

The Windows wrapper now stages:

- `source/tools/wallrun_scalar_writer_refs_v7.py`;
- V6/V5/V4 helpers;
- `discover_ground_support.py`.

V7 re-authenticates before following the handoff:

- Path B target `0x29C18B0`;
- call site `0x29C18D4`;
- exact call bytes `E8 17 88 6C 00`;
- decoded target `0x308A0F0`;
- Path-B trace must reproduce exactly one call at that site with unique `RCX = owner+0x58`.

Only then trace target `0x308A0F0` with entry RCX owner+`0x58`.

Reuse existing V6 target-scope rules:

- complete PE runtime-function decode when `.pdata` covers the target;
- otherwise strict max-128-byte / max-32-reachable-instruction no-`.pdata` fallback;
- bounded contained CFG cycles allowed under monotone intersection;
- all V5 non-call/nonvolatile/RSP/indirect-jump fallback admission rules retained;
- no deeper callee traversal in this task.

Expected output schema is 7.

## Owner action

No Update-WS, build, deploy, Overstrike or game run is required.

Fast-forward the branch and run only:

`source/tools/windows/Inspect-WallRun250-Writers.ps1`

Return the newly generated:

`MSM2-WS-wallrun250-writers-YYYYMMDD-HHMMSS.json`

## Result handling

If V7 finds a proven `owner+0x250` writer:

- inspect only that writer and value source/width;
- do not authorize a game write yet.

If V7 finds only reads/no direct access:

- inspect only V7's bounded reachable call/tail information for one next producer step;
- do not reopen Path A or global scans.

If V7 refuses:

- accept the exact structural blocker;
- fix only that demonstrated condition if it remains bounded.

Current runtime remains `v0.1c47 / 0.1.72`; no runtime rebuild or gameplay test is part of this static step.
