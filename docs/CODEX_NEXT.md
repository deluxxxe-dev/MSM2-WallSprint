# Next task — owner runtime test of TurnSpeedSlow steering probe; no c73 yet

## Status

Corrected c72 remains accepted **VERIFIED-STATIC**; direct-CFG continuation is paused.

The first Standard steering probe is complete:

```text
TurnSpeedFast: 100 -> 10
```

Owner A/B result:

```text
probe ON ordinary WallRun: unchanged
probe OFF ordinary WallRun: unchanged
WS reversal probe ON: moonwalk YES
WS reversal probe OFF: moonwalk YES
WS disabled + probe ON ordinary WallRun: unchanged
```

Canonical detail:

- `analysis/V01C72_OWNER_OFFLINE_RESULT.md`
- `analysis/VANILLA_WALLRUN_STEERING_CONFIG_20261029.md`
- `analysis/WALLRUN_STEERING_PROBE_AB_RESULT_20261029.md`
- `docs/PROJECT_STATE.md`

## Current interpretation

Narrow **VERIFIED-RUNTIME** facts:

- the moonwalk-like WS facing mismatch is baseline WS behavior, not caused by the Fast steering probe;
- `TurnSpeedFast 100 -> 10` produced no visible ordinary-WallRun heading-response change in the tested maneuvers, including a run with WS disabled.

Do not call `TurnSpeedFast` unused. The Fast branch may simply not have been selected, or runtime logic may override/scale the value.

Current Overstrike source independently confirms the MSM2 `format_version=2` stage layout/path mechanism used by the probe, so package-layout failure is not the leading explanation.

## Next owner runtime test

Test exactly one further field:

```text
Standard.TurnSpeedSlow: 75 -> 5
file offset 0x219: 0x4B -> 0x05
```

Repository builder/test:

```text
source/tools/build_wallrun_steering_slow_probe.py
source/tools/test_build_wallrun_steering_slow_probe.py
```

Procedure:

1. Disable/remove the previous `TurnSpeedFast 10` probe.
2. Disable WallSprint for this isolation test.
3. Enable only `WS Steering Probe - TurnSpeedSlow 5` among WS steering probes.
4. Run `Install mods`.
5. Launch the modded game.
6. Perform an ordinary outdoor WallRun and make strong left/right heading changes.
7. Report `clearly slower / subtle / unchanged / unstable / other`.
8. Disable/remove the Slow probe and run `Install mods` again before unrelated testing.

No runtime log is required unless load/install/crash failure occurs.

## Stopping rule

If `TurnSpeedSlow 75 -> 5` also yields no visible ordinary-WallRun steering change, stop serial Fast/Slow config probing. Do not stack more steering fields. Move next to the higher-level WS movement/facing/orientation investigation suggested by:

- the baseline moonwalk symptom;
- the existing Scripted Flight reference evidence around motion request, actor/mover facing and gravity/up orientation.

Do not reopen c73 merely because this config discrimination is negative.

## Shared Overstrike recurrence mitigation

`environment/Start-Overstrike.cmd` now self-heals the known misplaced `<game>\Overstrike\winmm.dll` case on installed shortcut launch by preserving it under `Mod-Tools\_quarantine` before normal validation. It also removes the Mod-Tools lock only when the recorded PID is demonstrably not running.

## Binding safety

Runtime remains `v0.1c38`; C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

No native gameplay-write gate is opened by this asset-only test.
