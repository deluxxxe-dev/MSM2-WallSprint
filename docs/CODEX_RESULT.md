# Result — c47 WallRun scalar runtime accepted; targeted 0x250 writer provenance next

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C44 REQUEST ZERO-HIT ACCEPTED / C45 GRAVITY OWNER ZERO-HIT ACCEPTED / C46 SCALAR RUN REJECTED AS INSTRUMENTATION-GATED / C47 SCALAR OBSERVATION VERIFIED-RUNTIME / `WallRun.this+0x250` SELECTED AS CORRELATED DYNAMIC CANDIDATE / NO WRITE AUTHORIZED / OFFLINE WRITER V1+V2 GLOBAL SEARCHES RETIRED / V3 TARGETED AUTHENTICATED OWNER-CALLEE INSPECTION PUBLISHED / OWNER JSON PENDING.**

## C47 ACTUAL-GAME RESULT

Owner supplied `MSM2-WS-v0.1c47-20261101-084650.log` plus video and identified the sequence as ordinary right WallRun -> reverse left -> reverse right -> down hold, where down produces the known short-period left/right alternation.

Verified runtime facts:

- exact Spider-Man2.exe 2.810.0.0 target accepted;
- exact authenticated WallRun Update SHA accepted;
- scalar session armed while legacy surface telemetry remained disabled;
- all five bounded direct fields were readable;
- `+0x250` repeatedly produced negative pulses associated in time with both ordinary reversals and the later down-hold left/right oscillation;
- direct signed left/right-input semantics are not supported because both reversal directions used negative pulses;
- exact field meaning, units, producer, writer, owner-at-writer provenance and writability remain UNKNOWN.

Do not repeat c47 merely for more samples or a summary. The missing summary is explained by the existing emergency Update-timeout close with Deinit=0; retained first/change samples remain valid.

Detailed runtime evidence: `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`.

## STATIC RELATION OF +0x250

Authenticated WallRun Update `0x1424530..0x14246D6`, SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`:

- directly reads `this+0x250` at `0x1424585` and `0x142460E`;
- contains no accepted direct `+0x250` write;
- passes the authenticated WallRun owner in RCX into two exact direct callees:
  - `0x14245C3 -> 0x367DD10`;
  - `0x1424630 -> 0x29BD5F0`.

Call-entry RCX relation does not prove that a later memory operand inside a callee still uses the owner.

## OFFLINE WRITER SEARCH RESULTS

No runtime/package version bump. Current runtime remains `v0.1c47 / 0.1.72`.

### V1

Owner run refused with `decoded_candidate_start_cap` because byte-local backtracking generated too many false instruction-start candidates around common literal `0x250` bytes. No engine conclusion accepted.

### V2

Owner reran the corrected runtime-function prefilter. It refused with:

`REFUSED: writer_cap`

This is also **not** a WallRun no-writer or many-WallRun-writers result. `+0x250` is a common object displacement across the whole game image; the global executable scan found more than the bounded number of direct write references before owner identity could distinguish them. Increasing the cap would only enlarge unrelated evidence.

Decision: retire whole-EXE/global `+0x250` writer enumeration for this task.

## V3 TARGETED INSPECTION

`source/tools/wallrun_scalar_writer_refs.py` now decodes exactly three authenticated PE runtime functions only:

1. WallRun Update `0x1424530`;
2. direct owner-RCX callee `0x367DD10`;
3. direct owner-RCX callee `0x29BD5F0`.

V3:

- requires exact EXE SHA and Capstone 5.0.6;
- re-authenticates exact Update SHA/range and both direct-call prefixes/targets;
- requires both callee targets to be exact PE runtime-function entries;
- requires complete contiguous entry-to-end linear decode for each of the three functions;
- requires the two known Update `+0x250` sites as a self-check;
- rejects an unexpected direct Update writer;
- records every direct `+0x250` reference/write in the two authenticated owner-entry callees;
- records a bounded local instruction context around any direct writer candidate;
- records each direct call made by those two callees so a no-direct-writer result still gives the next bounded provenance leads;
- does not claim owner-at-writer merely from entry RCX;
- performs no global executable scan, adjacent-offset scan, game process operation, hook or write.

Computed-address writers, bulk-copy effects, nested-pointer fields and deeper call paths remain outside this V3 scope.

## SAFETY / ACTIVE RUNTIME

No c47 gameplay implementation changed:

- `WALLSPRINT_ENABLE=0`;
- `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`;
- orientation / adhesion / movement / gravity / native-source writes OFF;
- no new native hook;
- no scalar write/A-B test authorized;
- no SocialFeed or other MOD changes.

## NEXT OWNER ACTION

Do not run the game, Overstrike or Update-WS for this analysis step.

Fast-forward the branch and rerun only `Inspect-WallRun250-Writers.ps1`. Return the new `MSM2-WS-wallrun250-writers-*.json`.
