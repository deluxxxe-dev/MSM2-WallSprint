# Result — c47 WallRun scalar runtime accepted; V6 owner trace succeeded; V7 single handoff ready

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C47 SCALAR OBSERVATION VERIFIED-RUNTIME / `WallRun.this+0x250` REMAINS CORRELATED DYNAMIC CANDIDATE / V3 NO-DIRECT-WRITER RESULT ACCEPTED / V4/V5 STRUCTURAL BLOCKERS RESOLVED / V6 TWO-ROOT OWNER-RELATIVE TRACE SUCCEEDED WITH ZERO `owner+0x250` ACCESSES / ONE PATH-B RCX-PRESERVING DIRECT CALL SELECTED / V7 SINGLE-CALLEE TRACE IMPLEMENTED / NO WRITE AUTHORIZED.**

## Accepted c47/V3 evidence

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

## V6 owner result — accepted

Owner returned:

`MSM2-WS-wallrun250-writers-20261101-132326.json`

Schema/result:

- schema 6;
- `NO_OWNER_RELATIVE_0X250_ACCESS_IN_BOUNDED_DOWNSTREAM_TRACE`.

Path A `0x29C06F0`:

- strict no-`.pdata` bounded V6 CFG scope;
- range `0x29C06F0..0x29C0760`;
- 30 reachable instructions / 30 worklist visits;
- contained CFG cycle accepted;
- `owner+0x250` accesses: 0;
- reachable direct calls: 0;
- external tail jumps: 0.

Path A therefore has no current next producer lead in the accepted scope and must not be reopened without new evidence.

Path B `0x29C18B0`:

- PE runtime-function backed;
- range `0x29C18B0..0x29C1A6C`;
- 136 reachable instructions / 141 worklist visits;
- `owner+0x250` accesses: 0;
- one reachable direct call retains unique RCX owner provenance:
  - call site `0x29C18D4`;
  - target `0x308A0F0`;
  - pre-call `RCX = owner+0x58`;
  - pre-call `RDI = owner+0x58`.

Combined V6 summary:

- owner-relative `+0x250` accesses 0;
- reads 0;
- writes 0;
- access-unknown 0.

This is a bounded no-access result, not proof that the field is never written.

Detailed evidence:

- `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`
- `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`
- `analysis/WALLRUN250_V4_NO_PDATA_RESULT_20261101.md`
- `analysis/WALLRUN250_V5_CFG_CYCLE_RESULT_20261101.md`
- `analysis/WALLRUN250_V6_OWNER_RESULT_20261101.md`

## V7 implementation

Added:

- `source/tools/wallrun_scalar_writer_refs_v7.py`

Updated:

- `source/tools/windows/Inspect-WallRun250-Writers.ps1`
- `source/tests/wallrun_scalar_writer_tool_static.py`

V7 does not broaden the search. It replays and authenticates the accepted Path-B source before following exactly one additional call.

Pinned handoff:

- source Path B target `0x29C18B0`;
- call site `0x29C18D4`;
- exact call bytes `E8 17 88 6C 00`;
- decoded target `0x308A0F0`;
- required pre-call unique RCX provenance `owner+0x58`.

If any of those conditions fail against the exact pinned EXE, V7 refuses.

Only after the handoff is re-authenticated does V7 trace target `0x308A0F0` with entry `RCX = owner+0x58`, using the existing V6 PE-runtime/no-`.pdata` bounded scope handling and V4 conservative owner-relative provenance model.

V7 output schema is 7.

## Safety / active runtime

Current runtime remains **v0.1c47 / package 0.1.72**.

- runtime/gameplay code unchanged;
- all gameplay writes remain OFF;
- no new native hook/watchpoint/page guard;
- no global executable writer scan;
- no adjacent-field scan;
- Path A is not reopened;
- only one deeper callee is authorized;
- no SocialFeed or other MOD change;
- no Update-WS/build/deploy/game action is required for V7 acquisition.
