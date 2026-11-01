# Result — c47 WallRun scalar runtime accepted; transformed owner-relative provenance next

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C47 SCALAR OBSERVATION VERIFIED-RUNTIME / `WallRun.this+0x250` SELECTED AS CORRELATED DYNAMIC CANDIDATE / V1+V2 GLOBAL SEARCHES RETIRED / V3 AUTHENTICATED OWNER-CALLEE SCAN SUCCEEDED WITH NO DIRECT +0x250 WRITER / NEXT SCOPE IS TWO TRANSFORMED OWNER-RCX CALLEES ONLY / NO WRITE AUTHORIZED.**

## C47 RUNTIME FACT

`WallRun.this+0x250` repeatedly changes during verified WallRun and is temporally associated with both ordinary direction reversals and the later down-hold left/right oscillation. Both reversal directions use negative pulses, so direct signed left/right-input semantics are not supported. Exact field meaning, units, producer and writability remain UNKNOWN.

Detailed runtime evidence: `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`.

## V3 OWNER RESULT

Owner supplied `MSM2-WS-wallrun250-writers-20261101-092733.json`.

The offline scan completed successfully on the exact pinned Spider-Man2.exe 2.810.0.0 image with Capstone 5.0.6.

Revalidated:

- EXE SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- WallRun Update `0x1424530..0x14246D6`, SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`;
- Update `this+0x250` reads `0x1424585` and `0x142460E`;
- direct owner-RCX calls `0x14245C3 -> 0x367DD10` and `0x1424630 -> 0x29BD5F0`.

V3 result:

`NO_DIRECT_DISP250_WRITER_IN_AUTHENTICATED_OWNER_CALLEES`

No direct encoded `+0x250` writer exists in the fixed V3 scope:

- WallRun Update;
- `0x367DD10`;
- `0x29BD5F0`.

This is a bounded static no-direct-writer result only. It is not evidence that the field is never written.

Detailed V3 evidence: `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`.

## NEXT STATIC LEADS

`0x367DD10` receives the authenticated WallRun owner in RCX and contains two useful entry-reachable transformed-pointer calls.

Path A:

- `sub rcx, -0x80` => call-entry RCX = `WallRun owner + 0x80`;
- `0x367DD31 -> 0x29C06F0`;
- original owner field `+0x250` corresponds to target-entry-relative `+0x1D0` if provenance remains unchanged.

Path B:

- `add rcx, 0x58` => call-entry RCX = `WallRun owner + 0x58`;
- `0x367DD3C -> 0x29C18B0`;
- original owner field `+0x250` corresponds to target-entry-relative `+0x1F8` if provenance remains unchanged.

The arithmetic relations are exact at the two call entries. Provenance inside the targets is still UNKNOWN.

`0x29BD5F0` has no direct `+0x250` reference/writer. Its later linearly decoded call region is not accepted as an owner path: reported context crosses a `ret` and replaces RCX with a RIP-relative address. Do not follow it without separate entry-reachability proof.

## NEXT IMPLEMENTATION SCOPE

Implement only a bounded offline control-flow-aware GPR provenance trace rooted at:

- `0x29C06F0`, entry RCX = owner+`0x80`;
- `0x29C18B0`, entry RCX = owner+`0x58`.

Accept only clearly provable full-64-bit pointer-preserving transforms: register copy, base+constant LEA, and ADD/SUB immediate. Kill provenance on unsupported/partial/memory-loaded pointer writes. Intersect provenance conservatively at CFG merges. Treat calls as clobbering volatile GPR provenance unless separately proven.

Report only reachable memory accesses proven to resolve to original `WallRun owner+0x250`, plus direct calls where RCX retains a unique owner-relative provenance.

Do not perform a global executable scan, adjacent-field search, runtime watchpoint, page guard, hook or scalar write.

## SAFETY / ACTIVE RUNTIME

Current runtime remains `v0.1c47 / 0.1.72`.

- gameplay implementation unchanged;
- all WallSprint movement/orientation/adhesion/gravity/native-source writes remain OFF;
- no new native hook;
- no game/Overstrike/Update-WS action is required for this offline implementation step;
- no SocialFeed or other MOD changes.
