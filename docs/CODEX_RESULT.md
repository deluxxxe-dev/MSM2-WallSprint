# Result — c47 WallRun scalar runtime accepted; 0x250 writer discovery next

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C44 REQUEST ZERO-HIT ACCEPTED / C45 GRAVITY OWNER ZERO-HIT ACCEPTED / C46 SCALAR RUN REJECTED AS INSTRUMENTATION-GATED / C47 SCALAR OBSERVATION VERIFIED-RUNTIME / `WallRun.this+0x250` SELECTED AS CORRELATED DYNAMIC CANDIDATE / NO WRITE AUTHORIZED / OFFLINE DIRECT-WRITER SCAN V1 REFUSED ON CANDIDATE-START CAP / V2 RUNTIME-FUNCTION PREFILTER PUBLISHED / OWNER JSON RETRY PENDING.**

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

## OFFLINE WRITER DISCOVERY

No runtime/package version bump is made. `v0.1c47 / 0.1.72` remains the current runtime package.

Files:

- `source/tools/wallrun_scalar_writer_refs.py`;
- `source/tools/windows/Inspect-WallRun250-Writers.ps1`;
- `source/tests/wallrun_scalar_writer_tool_static.py`;
- normal `source/tools/test.sh` static-contract wiring.

The helper requires the exact pinned EXE SHA and Capstone 5.0.6, re-authenticates the exact Update SHA and both owner-RCX call prefixes, and never launches or attaches to the game.

### Owner V1 result and correction

The first owner-local run refused with:

`REFUSED: decoded_candidate_start_cap`

This is **not** an engine no-writer result. The V1 prefilter took every raw `0x00000250` byte hit and tried up to 14 possible instruction starts around each hit. False starts accumulated past the bounded candidate-start cap before the real fixed-scope function analysis completed.

V2 removes byte-local instruction-start guessing entirely:

1. raw literal `0x00000250` hits only select their containing PE runtime-function ranges;
2. each selected runtime function is decoded once from its authenticated PE runtime entry to end;
3. only instructions from a complete contiguous function decode are inspected for real memory operands with displacement `+0x250`;
4. global ordinary read references are counted but not all exported; retained evidence is limited to writers, the two known Update sites, and references inside the two authenticated Update owner-RCX callees;
5. candidate-function/decode-byte/writer/output caps remain fail-closed.

Current V2 bounds include max 16,384 candidate functions and 96 MiB aggregate decoded candidate-function bytes; the executable-file prefilter itself remains bounded to 192 MiB and 200,000 raw displacement hits.

The two already-known Update `+0x250` sites remain a mandatory self-check. Computed-address writers, bulk-copy effects without literal `+0x250`, nested pointers and indirect producer paths remain outside this fixed scan.

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

Fast-forward the current branch and rerun only `Inspect-WallRun250-Writers.ps1`. Return only the new generated:

`MSM2-WS-wallrun250-writers-YYYYMMDD-HHMMSS.json`

The earlier `...091919.json` contains only the V1 refusal reason and is not a writer result.
