# WallRun steering probe A/B runtime result — 2026-10-29

## Evidence

Owner completed the requested A/B comparison of the authenticated current-build Standard steering probe:

```text
Standard.TurnSpeedFast: 100 -> 10
```

Owner observations:

```text
A probe ON, ordinary outdoor WallRun: no visible steering/heading-response change
A probe ON, WS reversal: moonwalk-like visible facing mismatch = YES

B probe OFF, ordinary outdoor WallRun: no visible steering/heading-response change
B probe OFF, WS reversal: moonwalk-like visible facing mismatch = YES

Additional isolation:
WS disabled, probe ON only, ordinary WallRun: no visible steering/heading-response change
```

## Conclusions

**VERIFIED-RUNTIME (owner-observed):**

- The WS moonwalk-like mismatch is present with the steering probe both enabled and disabled, so it is not caused by the `TurnSpeedFast 100 -> 10` probe.
- No visible change in ordinary WallRun steering response was observed from this probe, including a run with WallSprint disabled.

Do not over-promote the second point to "TurnSpeedFast is unused". Remaining explanations include the tested maneuver not entering the Fast branch, runtime override/scaling, or another state/path selecting a different steering quantity.

The stage package format was independently checked against current Overstrike source: MSM2 `format_version=2` stage mods are routed to `StageInstaller_I29_V2`, and numeric-span asset paths such as `0/configs/...` are hashed/installed as assets. Therefore an invalid stage layout is not the leading explanation for the no-effect result.

## Next minimal discrimination

Test one further Standard steering field only:

```text
TurnSpeedSlow: 75 -> 5
file offset 0x219: 0x4B -> 0x05
```

Run this with WallSprint disabled and the prior TurnSpeedFast probe disabled. Test only ordinary outdoor WallRun heading response.

Stopping rule:

- If `TurnSpeedSlow 75 -> 5` also produces no visible ordinary-WallRun steering change, stop serial TurnSpeedFast/Slow asset probing. Do not continue stacking steering config changes. Move to the higher-level WS movement/facing/orientation investigation suggested by the baseline moonwalk symptom and Scripted Flight reference evidence.
- If it clearly changes heading response, record the narrow causal fact and continue only with one concrete config question at a time.

## WS symptom

The baseline WS behavior now has a stronger runtime characterization:

```text
movement/displacement direction can reverse
while visible body facing remains in the prior direction
```

This does not yet prove actor-transform orientation is numerically unchanged. Actor/mover facing and animation presentation remain separate candidate layers.

## Safety

No native gameplay-write gate is opened by this result. Runtime script remains v0.1c38; C52 remains binding. Direct-CFG c73 remains paused.
