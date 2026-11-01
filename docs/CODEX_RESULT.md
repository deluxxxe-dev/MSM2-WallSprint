# Result — c47 WallRun scalar runtime accepted; V7 producer chain exhausted; V8 consumer trace ready

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C47 SCALAR OBSERVATION VERIFIED-RUNTIME / `WallRun.this+0x250` REMAINS CORRELATED DYNAMIC CANDIDATE / V3-V7 OWNER-PROVENANCE PRODUCER CHAIN EXHAUSTED WITH ZERO DIRECT `owner+0x250` ACCESSES / PRODUCER STILL UNKNOWN / V8 BOUNDED UPDATE-CONSUMER TRACE IMPLEMENTED / NO WRITE AUTHORIZED.**

## Accepted runtime/static evidence

C47 runtime remains accepted:

- `WallRun.this+0x250` changes during verified WallRun;
- changes correlate with ordinary reversals and down-hold left/right oscillation;
- both reversal directions produced negative pulses, so direct signed left/right-input semantics remain unsupported;
- exact field meaning, units, producer and writability remain UNKNOWN.

Authenticated WallRun Update `0x1424530..0x14246D6`, SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`, directly reads `this+0x250` at:

- `0x1424585`;
- `0x142460E`.

## V6/V7 producer-path result — accepted

V6 schema 6:

`NO_OWNER_RELATIVE_0X250_ACCESS_IN_BOUNDED_DOWNSTREAM_TRACE`

- Path A `0x29C06F0`, entry `RCX=owner+0x80`: 30 reachable instructions, no `owner+0x250` access, no direct call, no tail jump.
- Path B `0x29C18B0`, entry `RCX=owner+0x58`: no `owner+0x250` access; exactly one owner-preserving direct call at `0x29C18D4 -> 0x308A0F0` with pre-call `RCX=owner+0x58`, `RDI=owner+0x58`.

V7 owner JSON:

`MSM2-WS-wallrun250-writers-20261101-133456.json`

Schema/result:

- schema 7;
- `NO_OWNER_RELATIVE_0X250_ACCESS_IN_V7_HANDOFF_CALLEE`.

V7 re-authenticated the Path-B call bytes/target/provenance and traced only `0x308A0F0`:

- strict no-`.pdata` bounded scope `0x308A0F0..0x308A120`;
- 8 reachable instructions / 8 worklist visits;
- no CFG cycle;
- no `owner+0x250` access;
- no reachable direct call;
- no external tail jump.

Therefore the selected authenticated RCX-owner callee chain has no remaining static handoff lead. This is bounded no-access evidence only; it does not prove the field is never written.

Detailed evidence:

- `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`
- `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`
- `analysis/WALLRUN250_V6_OWNER_RESULT_20261101.md`
- `analysis/WALLRUN250_V7_HANDOFF_RESULT_20261101.md`

## V8 implementation — consumer side

Added:

- `source/tools/wallrun250_consumer_v8.py`
- `source/tools/windows/Inspect-WallRun250-Consumers.ps1`
- `source/tests/wallrun250_consumer_v8_static.py`

Updated:

- `source/tools/test.sh`

V8 does not continue producer traversal. It authenticates the exact target image and WallRun Update, then inspects only the two known Update reads of `this+0x250`.

For each read site V8 records:

- exact read instruction;
- six preceding Update instructions;
- at most 32 entry-reachable Update instructions from that read;
- register read/write sets;
- memory operands and widths/access direction;
- direct CALL/JMP targets without traversing callees;
- file-backed RIP-relative operand bytes, with 32/64-bit numeric/float decoding when applicable.

RIP-relative values are explicitly **on-disk initial bytes only**, not runtime values. V8 does not assign field semantics.

V8 remains bounded:

- Update body only;
- no global executable scan;
- no adjacent-offset scan;
- no producer/deeper-callee traversal;
- no game process access;
- no memory write.

Expected V8 output schema is 8 and output name is:

`MSM2-WS-wallrun250-consumers-YYYYMMDD-HHMMSS.json`

## Safety / active runtime

Current runtime remains **v0.1c47 / package 0.1.72**.

- runtime/gameplay code unchanged;
- all gameplay writes remain OFF;
- no new hook/watchpoint/page guard;
- no Update-WS/build/deploy/game action is required for V8 acquisition;
- no SocialFeed or other MOD change.
