# Result — c47 WallRun scalar runtime accepted; 0x250 writer discovery next

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C44 REQUEST ZERO-HIT ACCEPTED / C45 GRAVITY OWNER ZERO-HIT ACCEPTED / C46 SCALAR RUN REJECTED AS INSTRUMENTATION-GATED / C47 SCALAR OBSERVATION VERIFIED-RUNTIME / `WallRun.this+0x250` SELECTED AS CORRELATED DYNAMIC CANDIDATE / NO WRITE AUTHORIZED / OFFLINE DIRECT-WRITER SCAN PUBLISHED / OWNER JSON PENDING.**

## C47 ACTUAL-GAME RESULT

Owner supplied `MSM2-WS-v0.1c47-20261101-084650.log` plus video and identified the sequence as ordinary right WallRun -> reverse left -> reverse right -> down hold, where down produces the known short-period left/right alternation. Owner asked that visible slowdown/stutter be ignored as likely console-related.

Instrumentation acceptance:

- exact Spider-Man2.exe 2.810.0.0 target accepted;
- on-disk SHA256 remains `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- exact authenticated WallRun Update SHA remains `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`;
- `wallrun_update_scalar_plan enabled=1`;
- `wallrun_update_scalar_session ... state=armed ... surface_telemetry=0 direct_owner_reads_only=1 pointer_dereference=0 game_write=0` exists;
- therefore c47 successfully decoupled the five scalar reads from the disabled legacy surface telemetry gate.

First direct sample at scalar elapsed 16 ms:

- `+0x208 = 0.25`;
- `+0x220 = 0`;
- `+0x238 = 0`;
- `+0x250 = 0`;
- `+0x40C = 0`.

No logged sample exactly matched the comparison-only serializer numbers `0.4 / 12 / 75 / 100 / 50`.

## C47 CANDIDATE CLASSIFICATION

`+0x208` changed once around scalar elapsed 3.516 s from `0.25` to approximately `1e30`. This is insufficient for steering semantics; sentinel/threshold/timer interpretation remains only a hypothesis.

`+0x250` repeatedly changed with the reversal/down-oscillation sequence and is the only useful dynamic candidate in this run.

Bounded retained clusters before the global 64-change-log cap:

- about `4.938..6.907 s`: negative pulse to about `-0.166`, returns to zero;
- about `7.782..10.407 s`: negative pulse to about `-0.162`, returns to zero;
- about `11.282..12.375 s`: negative pulse to about `-0.208`;
- later short pulses around `13.141..13.250`, `14.125..14.344`, and `15.219..15.328 s`.

Independent Actor-basis / position-difference evidence shows the first two ordinary direction reversals and repeated later left/right flips during the owner's down-hold interval. The `+0x250` pulses occur in temporal association with those changes.

Classification:

- `+0x250` changes during verified WallRun: **VERIFIED-RUNTIME**;
- timing association with reversal/down oscillation: **OBSERVED-CORRELATION**;
- direct signed left/right input interpretation: **not supported**; both reversal directions use negative-valued pulses;
- turn/angle/transition magnitude or related internal state: **HYPOTHESIS**;
- exact field name, units, producer, writer, ownership and writability: **UNKNOWN**.

`+0x220`, `+0x238`, and `+0x40C` emitted no change before the retained change-log cap. This is bounded no-change evidence only.

Detailed derived evidence is in `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`.

## LIFECYCLE LIMITATION

The retained WallRun lifecycle later closed by the existing emergency Update-timeout path with `Deinit=0`, so c47 did not emit `wallrun_update_scalar_summary`. This is a scalar-finalization limitation, not a read failure. The first/change samples are valid and sufficient for the current discriminator. Do not repeat c47 solely for a summary.

## STATIC RELATION OF +0x250

Exact authenticated WallRun Update `0x1424530..0x14246D6` directly reads `this+0x250` at two verified sites:

- `0x1424585`;
- `0x142460E`.

Both are threshold comparisons. The authenticated Update body does not directly write `+0x250`.

Two direct calls in that exact Update receive the authenticated WallRun owner in RCX at call entry:

- call `0x14245C3` -> `0x367DD10`;
- call `0x1424630` -> `0x29BD5F0`.

This call-entry relation does not prove that RCX or an alias still denotes the owner at a later instruction inside either callee.

## NEW OFFLINE WRITER DISCOVERY

No runtime/package version bump is made. `v0.1c47 / 0.1.72` remains the current runtime package.

Added:

- `source/tools/wallrun_scalar_writer_refs.py`;
- `source/tools/windows/Inspect-WallRun250-Writers.ps1`;
- `source/tests/wallrun_scalar_writer_tool_static.py`;
- normal `source/tools/test.sh` static-contract wiring.

The offline helper:

- requires the exact pinned EXE SHA;
- requires Capstone 5.0.6;
- authenticates the exact Update SHA and both owner-RCX call prefixes;
- scans executable file bytes only for direct x64 memory operands with displacement `+0x250`;
- requires complete PE runtime-function linear instruction-boundary decode before accepting a reference;
- requires the two already-known Update `+0x250` reference sites as a self-check;
- reports direct write candidates and whether their containing function is one of the two authenticated owner-RCX callee entries;
- explicitly does **not** prove WallRun-owner provenance at any candidate writer instruction merely from displacement, entry RCX or numeric coincidence;
- does not launch the game, attach to a process, hook, install, build, package, deploy, read live values or write memory.

Computed-address writers, bulk-copy writers that do not encode displacement `0x250` directly, nested-pointer fields and indirect producer paths remain outside this first fixed scan.

## SAFETY / ACTIVE RUNTIME

No c47 gameplay implementation changed:

- `WALLSPRINT_ENABLE=0`;
- `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`;
- orientation / adhesion / movement / gravity / native-source writes OFF;
- `REQUEST_PRESENCE_DIAGNOSTIC=0`;
- no new native hook;
- no scalar write/A-B test authorized.

No SocialFeed or other MOD repository/file was changed.

## NEXT OWNER ACTION

Do **not** run another game test and do not rebuild/deploy merely for this analysis step.

Run the published offline writer scan against the local pinned Spider-Man2.exe and return only the generated:

`MSM2-WS-wallrun250-writers-YYYYMMDD-HHMMSS.json`

Chat then decides whether a direct writer has defensible owner provenance, whether one of the two authenticated Update owner-RCX callees is involved, or whether the fixed direct-displacement scan must stop without escalating to a write test.
