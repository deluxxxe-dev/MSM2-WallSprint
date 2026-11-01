# Result — c47 WallRun scalar runtime accepted; V6 bounded cycle-aware owner trace ready

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C47 SCALAR OBSERVATION VERIFIED-RUNTIME / `WallRun.this+0x250` REMAINS CORRELATED DYNAMIC CANDIDATE / V3 NO-DIRECT-WRITER RESULT ACCEPTED / V4 NO-`.pdata` STRUCTURAL REFUSAL ACCEPTED / V5 REACHED A REAL BOUNDED CFG CYCLE AT `0x29C06F0` AND REFUSED BY POLICY / V6 CHANGES ONLY THAT CYCLE POLICY / NO WRITE AUTHORIZED.**

## Accepted evidence

C47 runtime remains accepted:

- `WallRun.this+0x250` changes during verified WallRun;
- changes correlate with ordinary reversals and down-hold left/right oscillation;
- both reversal directions produced negative pulses, so direct signed left/right-input semantics remain unsupported;
- exact field meaning, units, producer and writability remain UNKNOWN.

V3 accepted static result:

- exact target image and WallRun Update identities passed;
- no direct encoded `+0x250` writer in Update, `0x367DD10`, or `0x29BD5F0`;
- Path A: `0x367DD31 -> 0x29C06F0`, entry RCX = owner+`0x80`;
- Path B: `0x367DD3C -> 0x29C18B0`, entry RCX = owner+`0x58`.

V4 owner-local result:

`downstream_target_no_runtime_function_0x29c06f0`

This established only that Path A is not covered by PE runtime-function (`.pdata`) metadata.

V5 owner-local result:

`no_pdata_fallback_cfg_cycle_0x29c06f0`

Generated JSON:

`MSM2-WS-wallrun250-writers-20261101-131833.json`

V5 therefore successfully entered the strict no-`.pdata` fallback and discovered a reachable CFG cycle before any writer/no-writer conclusion. This is a structural tool refusal, not an engine result.

Detailed evidence:

- `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`
- `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`
- `analysis/WALLRUN250_V4_NO_PDATA_RESULT_20261101.md`
- `analysis/WALLRUN250_V5_CFG_CYCLE_RESULT_20261101.md`

## V6 implementation

Added:

- `source/tools/wallrun_scalar_writer_refs_v6.py`

Updated:

- `source/tools/windows/Inspect-WallRun250-Writers.ps1`
- `source/tests/wallrun_scalar_writer_tool_static.py`

V6 preserves V5's no-`.pdata` admission limits:

- maximum 128-byte window;
- maximum 32 entry-reachable instructions;
- no CALL;
- no Win64 nonvolatile-GPR or RSP write;
- no indirect jump;
- conditional branches must remain inside the bounded scope;
- unresolved successors/exits still refuse;
- external direct JMP is only a recorded tail exit;
- normal PE runtime-function decoding remains unchanged when `.pdata` exists.

The only demonstrated rule changed from V5 is CFG-cycle handling. A fully contained bounded cycle is now recorded as `cfg_cycle_present` instead of being rejected merely for existing.

Owner-relative state still uses the existing V4 monotone merge-by-intersection worklist. At each instruction/register, provenance can only stay equal or be discarded at a merge; it cannot become more specific after disagreement. This makes contained loops convergent. V6 additionally applies a 2048-worklist-visit cap to no-`.pdata` traces and refuses if that bound is exceeded.

V6 output schema is 6.

## Validation / safety

The authored V6 helper passed Python syntax compilation before publication. Repository static-contract source now pins V6 helper/wrapper presence, schema 6, bounded-cycle handling, retained V5 safety gates, no global scan, no gameplay write, and unchanged runtime/package.

Current runtime remains **v0.1c47 / package 0.1.72**.

- runtime/gameplay code unchanged;
- all gameplay writes remain OFF;
- no new native hook/watchpoint/page guard;
- no global executable writer scan;
- no adjacent-field scan;
- no SocialFeed or other MOD change;
- no Update-WS/build/deploy/game action is required for V6 acquisition.
