# Result — c47 WallRun scalar runtime accepted; V5 strict no-pdata owner trace ready

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C47 SCALAR OBSERVATION VERIFIED-RUNTIME / `WallRun.this+0x250` REMAINS CORRELATED DYNAMIC CANDIDATE / V3 NO-DIRECT-WRITER RESULT ACCEPTED / V4 OWNER-RELATIVE TRACE REFUSED ONLY BECAUSE `0x29C06F0` HAS NO COVERING PE RUNTIME-FUNCTION ENTRY / V5 STRICT NO-`.pdata` FALLBACK IMPLEMENTED / OWNER OFFLINE JSON PENDING / NO WRITE AUTHORIZED.**

## Accepted evidence

C47 runtime remains accepted:

- `WallRun.this+0x250` changes during verified WallRun;
- changes correlate with ordinary reversals and down-hold left/right oscillation;
- both reversal directions produced negative pulses, so direct signed left/right-input semantics remain unsupported;
- exact field meaning, units, producer and writability remain UNKNOWN.

V3 accepted static result:

- exact target image and WallRun Update identities passed;
- known Update `+0x250` reads rediscovered;
- no direct encoded `+0x250` writer in Update, `0x367DD10`, or `0x29BD5F0`;
- Path A: `0x367DD31 -> 0x29C06F0`, entry RCX = owner+`0x80`;
- Path B: `0x367DD3C -> 0x29C18B0`, entry RCX = owner+`0x58`.

Detailed prior evidence:

- `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`
- `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`

## V4 owner result

Owner ran V4 against the pinned local EXE. It refused with:

`downstream_target_no_runtime_function_0x29c06f0`

Generated JSON:

`MSM2-WS-wallrun250-writers-20261101-130029.json`

Helper source commit reported by that run:

`b0abfcac761fe5dda617a3df57317fba5327e2a7`

Interpretation:

- the authenticated direct call target `0x29C06F0` is not covered by a PE runtime-function (`.pdata`) range;
- V4 therefore lacked a normal function-boundary/unwind proof and stopped fail-closed;
- this is **not** evidence that the function is absent, that `+0x250` has no writer, or that the accepted owner+`0x80` relation is wrong.

Detailed result:

`analysis/WALLRUN250_V4_NO_PDATA_RESULT_20261101.md`

## V5 implementation

Added:

- `source/tools/wallrun_scalar_writer_refs_v5.py`

Updated:

- `source/tools/windows/Inspect-WallRun250-Writers.ps1`
- `source/tests/wallrun_scalar_writer_tool_static.py`

V5 reuses the existing V4 exact-image, Update, source-callee, root-byte, root-target and owner-relative provenance checks. It changes only downstream target boundary handling.

When a downstream target is covered by `.pdata`, V5 continues to use the existing complete PE runtime-function decode.

When one of the two pinned roots has no covering `.pdata` range, V5 permits a strict bounded fallback only if all of the following hold:

- target is inside one unique executable PE section;
- scope does not cross the next PE runtime-function start;
- maximum 128 bytes;
- maximum 32 entry-reachable instructions;
- no CALL;
- no write to Win64 nonvolatile GPRs or RSP;
- no indirect jump;
- conditional target and fallthrough remain inside the bounded scope;
- external direct JMP is accepted only as a recorded tail exit;
- no CFG cycle;
- every reachable leaf terminates by RET or accepted external direct tail JMP.

Only after that structural gate passes does the existing V4 provenance logic inspect memory operands that resolve exactly to original `WallRun owner+0x250`.

The existing provenance rules remain conservative:

- full-64-bit MOV / constant LEA / ADD-SUB immediate only;
- unsupported/partial writes kill destination provenance;
- memory-loaded pointer provenance is not inferred;
- CFG merges retain only identical owner-relative offsets;
- CALL return kills Win64 volatile GPR provenance;
- deeper callees are not traversed.

## Validation

The V5 helper was syntax-checked with Python AST before publication. The repository static contract now pins:

- V5 helper presence;
- 128-byte / 32-instruction no-`.pdata` caps;
- CALL / nonvolatile-RSP / indirect-jump / CFG-cycle refusal;
- no global scan, adjacent-field scan, deeper traversal or gameplay write;
- unchanged runtime version/package.

The Windows wrapper stages V5 + V4 + the PE helper together and runs `py_compile` on all three before EXE analysis.

## Safety / active runtime

Current runtime remains **v0.1c47 / package 0.1.72**.

- runtime/gameplay code unchanged;
- all gameplay writes remain OFF;
- no new native hook/watchpoint/page guard;
- no global executable writer scan;
- no adjacent-field scan;
- no SocialFeed or other MOD change;
- no Update-WS/build/deploy/game action is required for V5 acquisition.
