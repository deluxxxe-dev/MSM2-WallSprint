# WallSprint v0.1c7 real-game runtime result

## Result

Real-game c7 is a FAIL/regression. The user reported that the actor remained visually in vanilla orientation for the test WallRun.

Target build remained accepted:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## What actually happened

The source override did begin, but survived for only two Path A writes / roughly 16 ms:

```text
elapsed=0.703  Path A write #1  t=0.000
elapsed=0.719  Path A write #2  t=0.064
NATIVE_SOURCE_EXIT reason=disabled_kill_or_patch_gate
```

The transition therefore never became visually meaningful. The verified WallRun remained active after the hard close.

This is not the c6 `fresh_update_age` failure. c7 was killed before a stale-suspend/recovery episode could matter.

## Adjacent diagnostic evidence

Immediately after the first Path A write, the bounded orientation/reset watch stopped with:

```text
stop_reason=observation_guards_lost
```

The second Path A write occurred immediately afterward, followed by the hard source close. This temporal adjacency is important but does not by itself prove that the trace stop caused the kill.

## Code-level coupling found after the run

The c7 worker loop contains a broad hard-kill path:

```cpp
if(!msm2::hooks::observationPlanAuthenticated()) {
    msm2::reset_trace::kill();
    msm2::trace::kill();
    msm2::input::kill();
    msm2::native_source::kill();
}
```

and then calls `native_source::tick(..., observationPlanAuthenticated() && input::authenticated())`; `tick()` also calls `kill()` if that combined authentication flag is false.

`observationPlanAuthenticated()` currently collapses several states into one boolean: verified WallRun hook availability, `fieldPlanEnabled`, and byte-for-byte ownership of all four installed WallRun hook patches.

`native_source::kill()` also has multiple other callers in Path A/B entry validation. The runtime log only exposes the generic final reason `disabled_kill_or_patch_gate`, so the exact first kill site remains unproven.

## Required conclusion

Do not weaken all integrity checks and do not advance adhesion.

The next build must first attribute the exact first hard-kill cause and split diagnostic/observation-only guard loss from true source-integrity loss where semantically justified.

Required evidence includes:

- first kill call site / reason;
- worker `observationPlanAuthenticated()` sub-gates;
- per-WallRun-hook patch ownership/readability;
- `fieldPlanEnabled` / verifiedAvailable;
- input Path A authentication;
- Path B compact proof/authentication;
- source caller/writer/current guard state;
- session/owner/Hero/Transform/thread/epoch state at kill.

Only a demonstrated observation-only/transient guard may be decoupled from orientation ownership. Genuine build/hook/caller/writer/identity/stack/thread/rollback corruption remains fail-closed.

## User-facing state

c5 remains the last authoritative real full-WallRun orientation PASS. c6 and c7 are regression/diagnostic branches. c7 transient freshness recovery remains useful code but has not passed real-game validation because the session was hard-killed almost immediately.