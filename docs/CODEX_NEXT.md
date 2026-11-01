# Next task — run V5 owner-relative provenance trace for c47 `WallRun.this+0x250`

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`, `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`, `analysis/WALLRUN250_V4_NO_PDATA_RESULT_20261101.md`, and current `docs/CODEX_RESULT.md`.

## Accepted evidence

Do not repeat c47 gameplay, RequestSimpleMotion, GravityOrientationOwner, or V1/V2 global `+0x250` scans.

Accepted runtime:

- `WallRun.this+0x250` changes during verified WallRun;
- changes correlate with ordinary reversals and down-hold left/right oscillation;
- direct signed left/right-input semantics are not supported;
- field meaning/units/producer/writability remain UNKNOWN.

Accepted V3 static result:

- exact EXE/Update identities passed;
- no direct encoded `+0x250` writer in Update, `0x367DD10`, or `0x29BD5F0`;
- Path A: `0x367DD31 -> 0x29C06F0`, entry RCX = owner+`0x80`;
- Path B: `0x367DD3C -> 0x29C18B0`, entry RCX = owner+`0x58`.

Accepted V4 owner result:

`downstream_target_no_runtime_function_0x29c06f0`

This means the first authenticated downstream call target is not covered by PE runtime-function (`.pdata`) metadata. It is not a no-function or no-writer result.

## Current V5 implementation

Run the Windows wrapper, which now stages:

- `source/tools/wallrun_scalar_writer_refs_v5.py`;
- `source/tools/wallrun_scalar_writer_refs.py` (V4 authentication/provenance core);
- `source/tools/discover_ground_support.py`.

V5 keeps normal PE runtime-function decoding when available. Only a pinned downstream target with no covering `.pdata` range may use the strict fallback:

- one unique executable section;
- stop before next runtime-function start;
- max 128 bytes;
- max 32 entry-reachable instructions;
- no CALL;
- no nonvolatile-GPR or RSP write;
- no indirect JMP;
- local conditional branches must remain inside scope;
- no CFG cycle;
- every reachable exit must be RET or an explicit external direct tail JMP.

After that gate, the existing V4 owner-relative provenance rules apply unchanged.

## Owner action

No Update-WS, build, deploy, Overstrike or game run is required.

Fast-forward the branch and run only:

`source/tools/windows/Inspect-WallRun250-Writers.ps1`

Return the newly generated:

`MSM2-WS-wallrun250-writers-YYYYMMDD-HHMMSS.json`

Expected schema is 5.

## Result handling

If V5 reports a proven owner-relative `+0x250` write:

- inspect only that writer and its value source/width;
- do not authorize a write test merely from address provenance.

If V5 finds only reads:

- use bounded reachable call/tail information to choose one next producer step.

If V5 finds no owner-relative access:

- use only bounded evidence from the two roots;
- do not return to global displacement or adjacent-offset scans.

If V5 refuses:

- accept the exact structural blocker;
- fix only that demonstrated condition if it can be bounded without weakening owner/runtime safety;
- do not broaden to whole-image search or runtime watchpoints automatically.

Current runtime remains `v0.1c47 / 0.1.72` and must not be rebuilt or game-tested for this offline step.
