# c47 WallRun direct-scalar runtime result — 2026-11-01

Branch: `codex/v01c20-edi-value-provenance`.
Runtime: `v0.1c47`, package `0.1.72`.

## Owner test sequence

Owner supplied the c47 log plus video and identified the intended sequence as:

1. ordinary rightward WallRun;
2. reverse left;
3. reverse right;
4. hold down, producing the known short-period left/right alternation.

Owner noted visible slowdown/stutter is likely console-related and should not be treated as a movement result.

## Admission / instrumentation result

The exact target build was accepted and the c47 scalar path actually armed:

- `wallrun_update_scalar_plan enabled=1`;
- `wallrun_update_scalar_session wallrun_session=1 state=armed cadence_ms=100 offsets=5 surface_telemetry=0`;
- direct owner reads only, pointer dereference=0, game_write=0.

Thus c47 fixes the c46 telemetry-gate failure. This run is valid scalar runtime evidence.

The first successful sample at scalar elapsed 16 ms read all five selected direct fields:

| Offset | First raw/float |
| --- | --- |
| `0x208` | `0x3e800000` = `0.25` |
| `0x220` | `0x00000000` = `0` |
| `0x238` | `0x00000000` = `0` |
| `0x250` | `0x00000000` = `0` |
| `0x40C` | `0x00000000` = `0` |

No logged scalar value exactly matched the comparison-only serializer numbers `0.4 / 12 / 75 / 100 / 50`.

## 0x208 result

`0x208` changed once in the retained change log at elapsed 3516 ms:

- `0.25 -> ~1.00000002e+30` (`0x7149f2ca`).

This occurs near the first direction reversal, but one transition is insufficient to assign steering semantics. The very large value is compatible with a sentinel/threshold/timer-like state, but that interpretation is only a hypothesis.

## 0x250 result

`0x250` is the only selected field that repeatedly changed with the owner-reported direction changes / oscillation sequence.

Observed bounded change clusters before the global 64-change-log cap was reached:

1. elapsed about `4.938..6.907 s`: starts near `-0.007`, reaches about `-0.166`, returns to `0` (brief `+0.0057` sample before zero);
2. elapsed about `7.782..10.407 s`: negative pulse, reaching about `-0.162`, then returns to `0`;
3. elapsed about `11.282..12.375 s`: negative pulse reaching about `-0.208`;
4. later short pulses at about `13.141..13.250 s`, `14.125..14.344 s`, and `15.219..15.328 s` before the change-log cap.

Independent worker motion/Actor-axis observations establish the broad phase timing:

- initial travel has negative wall-tangent Z velocity and Actor Right approximately `(-1,0,0)`;
- around scalar elapsed `3.79 s`, travel reaches near zero and Actor basis flips to the opposite side;
- by about `7.07..7.62 s`, the second reversal is visible in the motion sign / Actor basis;
- from about `10.6 s` onward, Actor basis and tangent travel repeatedly flip, matching the owner's down-hold oscillation description.

The first `0x250` pulse follows the first reversal; the second follows the second reversal; repeated later pulses occur during the down-hold oscillation period.

Classification:

- `0x250` changes during verified WallRun: **VERIFIED-RUNTIME**;
- repeated timing association with reversal / down-oscillation: **OBSERVED-CORRELATION**;
- direct left/right input sign: **REJECTED as a simple interpretation** because both reversal directions produce negative-valued pulses rather than opposite signs;
- likely turn/angle/transition magnitude or related internal state: **HYPOTHESIS**;
- exact field name / units / producer / writer / ownership: **UNKNOWN**.

## Other selected offsets

`0x220`, `0x238`, and `0x40C` started at zero and emitted no change record before the global change-log cap was exhausted by `0x250` activity. This is bounded no-change evidence only through the retained window, not a lifetime proof.

## WallRun lifecycle / summary limitation

The verified session recorded active Update activity but did not close through a normal Deinit in the retained lifecycle record. It was eventually summarized by the existing emergency `Update`-timeout path with `Deinit=0`.

Because c47 scalar finalization is tied to the source finish/Deinit path, no `wallrun_update_scalar_summary` was emitted. This is an instrumentation-finalization limitation, not a scalar-read failure. The retained first/change samples remain valid. Do not repeat c47 solely to obtain a summary.

## Static relation to authenticated Update

The exact authenticated Update body `0x1424530..0x14246D6`, SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`, directly reads `this+0x250` at two sites:

- Update RVA `0x1424585`: threshold compare against a global scalar;
- Update RVA `0x142460E`: second threshold compare against another global scalar.

The same exact Update body directly reads the other four c47 candidates but does not directly write `+0x250`.

Two direct calls in authenticated Update receive the WallRun owner in entry RCX:

- call RVA `0x14245C3` -> `0x367DD10`;
- call RVA `0x1424630` -> `0x29BD5F0`.

Entry-RCX relation at those call sites is static fact; it does not prove RCX or an alias still denotes the owner at any later writer instruction inside the callee.

## Decision

Do not write `0x250` yet. Correlation is not producer/ownership/causality proof.

Next smallest step is read-only on-disk EXE writer discovery, not another game run:

- exact-build SHA pin;
- exact authenticated Update SHA/call-prefix self-check;
- executable-section prefilter for direct x64 memory operands with displacement `+0x250`;
- require complete PE runtime-function instruction-boundary decode before accepting a reference;
- report direct write candidates and whether they lie in the two authenticated owner-RCX callees;
- do not infer ownership from displacement, RCX-at-entry, or numeric coincidence alone.

New tooling:

- `source/tools/wallrun_scalar_writer_refs.py`;
- `source/tools/windows/Inspect-WallRun250-Writers.ps1`.

This operation reads only the pinned on-disk EXE and writes a small JSON report under WS logs. It does not launch the game, install a MOD, hook code, or change runtime/package version.
