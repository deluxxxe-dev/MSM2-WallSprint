# Next task — run V6 cycle-aware owner-relative provenance trace for c47 `WallRun.this+0x250`

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`, `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`, `analysis/WALLRUN250_V4_NO_PDATA_RESULT_20261101.md`, `analysis/WALLRUN250_V5_CFG_CYCLE_RESULT_20261101.md`, and current `docs/CODEX_RESULT.md`.

## Accepted evidence

Do not repeat c47 gameplay, RequestSimpleMotion, GravityOrientationOwner, or V1/V2 global `+0x250` scans.

Accepted runtime:

- `WallRun.this+0x250` changes during verified WallRun;
- changes correlate with ordinary reversals and down-hold left/right oscillation;
- direct signed left/right-input semantics are not supported;
- field meaning/units/producer/writability remain UNKNOWN.

Accepted V3 static result:

- no direct encoded `+0x250` writer in Update, `0x367DD10`, or `0x29BD5F0`;
- Path A: `0x367DD31 -> 0x29C06F0`, entry RCX = owner+`0x80`;
- Path B: `0x367DD3C -> 0x29C18B0`, entry RCX = owner+`0x58`.

Accepted V4/V5 structural results:

- V4: `0x29C06F0` has no covering `.pdata` runtime-function range;
- V5: strict no-`.pdata` fallback found a reachable CFG cycle at Path A and refused solely because V5 prohibited every cycle.

Neither is a writer/no-writer result.

## Current V6 implementation

The Windows wrapper now stages:

- `source/tools/wallrun_scalar_writer_refs_v6.py`;
- `source/tools/wallrun_scalar_writer_refs_v5.py`;
- `source/tools/wallrun_scalar_writer_refs.py`;
- `source/tools/discover_ground_support.py`.

V6 retains all V5 no-`.pdata` bounds and refusal gates except the blanket CFG-cycle rejection:

- max 128 bytes;
- max 32 entry-reachable instructions;
- no CALL;
- no nonvolatile-GPR or RSP write;
- no indirect JMP;
- local conditional branches remain in scope;
- unresolved successors/exits refuse;
- external direct JMP is only a recorded tail exit.

Contained cycles are recorded as `cfg_cycle_present` and analyzed with V4's merge-by-intersection provenance worklist. A separate 2048-visit cap applies to no-`.pdata` traces.

Expected output schema is 6.

## Owner action

No Update-WS, build, deploy, Overstrike or game run is required.

Fast-forward the branch and run only:

`source/tools/windows/Inspect-WallRun250-Writers.ps1`

Return the newly generated:

`MSM2-WS-wallrun250-writers-YYYYMMDD-HHMMSS.json`

## Result handling

If V6 reports a proven owner-relative `+0x250` write:

- inspect only that writer and its value source/width;
- do not authorize a write test merely from address provenance.

If V6 finds only reads or no direct owner-relative access:

- use only the bounded reachable call/tail information from the two roots;
- do not return to global displacement or adjacent-offset scans.

If V6 refuses:

- accept the exact structural blocker and fix only that demonstrated condition if it can remain bounded;
- do not weaken unrelated admission rules or switch to runtime watchpoints automatically.

Current runtime remains `v0.1c47 / 0.1.72` and must not be rebuilt or game-tested for this offline step.
