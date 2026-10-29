# WallRun steering probe runtime result — 2026-10-29

## Scope

Owner tested the prepared current-build asset-only probe:

```text
Standard.TurnSpeedFast: 100 -> 10
```

The active WallSprint runtime script remained the existing runtime build; the probe itself only replaced the authenticated Standard WallRun steering config asset.

The owner also supplied gameplay video. Raw video is not committed to the repository. Evidence below is the owner's direct runtime observation plus the already-authenticated probe/package identity.

## Completed owner A/B result

**VERIFIED-RUNTIME (owner-observed visible behavior), narrowly scoped:**

```text
A — probe ON, WS ON
ordinary outdoor WallRun steering: no visible change
WS direction reversal: moonwalk-like visible facing mismatch YES

B — probe OFF, WS ON
ordinary outdoor WallRun steering: no visible change
WS direction reversal: moonwalk-like visible facing mismatch YES

Additional isolation
WS OFF, probe ON
ordinary outdoor WallRun steering: no visible change
```

No visible movement-speed change was observed during the earlier WS observation either.

## What this proves

The visible WS moonwalk-like symptom is **not caused by the TurnSpeedFast probe**. It reproduces with the probe disabled.

The tested `Standard.TurnSpeedFast: 100 -> 10` change produced **no owner-visible steering response change** in ordinary outdoor WallRun under the tested conditions, including a run with WS disabled.

This is enough to close the first probe as **NO VISIBLE EFFECT UNDER TESTED CONDITIONS**.

It does **not** prove that:

- `TurnSpeedFast` is unused globally;
- the Standard steering asset is never selected;
- the field has no effect in another speed/state branch;
- the field unit or semantics are known;
- actor transform orientation numerically stayed unchanged during WS reversal.

## WS movement/facing symptom

The WS state currently permits a visible separation between:

```text
movement/displacement direction
```

and

```text
visible character/body facing direction
```

when direction is reversed.

This is directly relevant to WallSprint. The responsible layer is still **UNKNOWN**. Plausible classes to distinguish later include:

- movement vector changes while actor/mover facing is not updated;
- actor orientation changes but animation/root presentation does not follow;
- WS custom motion bypasses vanilla WallRun steering/facing logic;
- another higher-level facing/orientation controller owns the visible result.

Do not promote the visible symptom into a claim about exact actor-transform state without numeric/runtime evidence.

## Package-format verification

The first probe's `.stage` layout matches current Overstrike MSM2 format-v2 handling:

```text
0/configs/hero/hero_WallRunSteeringSetup_Standard.config
info.json (game=MSM2, format_version=2)
```

Overstrike's StageInstaller hashes the asset path after the numeric span directory and overwrites that asset. Therefore the lack of visible effect is not presently explained by an obviously invalid stage layout.

## Next single-field discrimination

The smallest config-first follow-up is:

```text
Standard.TurnSpeedSlow: 75 -> 5
file offset 0x219: 0x4B -> 0x05
```

Purpose: distinguish the hypothesis that the tested ordinary WallRun did not enter the `Fast` steering branch. Keep the test isolated to ordinary outdoor WallRun first, with WS disabled if practical. Do not stack Fast and Slow changes in one package.

If the Slow probe also has no visible effect, do not continue enumerating steering fields indefinitely. Reassess Standard asset selection/runtime override and prioritize the higher-level motion/facing/orientation route suggested by current WS behavior and the Scripted Flight reference evidence.

## Environment incident discovered during testing

Repeated Overstrike `Install mods` attempts have failed at:

```text
Deleting 'winmm.dll'...
System.UnauthorizedAccessException
```

Static cause chain:

1. Overstrike deletes existing game-root `winmm.dll` without first clearing its ReadOnly attribute.
2. It then recreates `winmm.dll` with `File.Copy("scripts_proxy.dll", scriptsProxyPath)`.
3. .NET `File.Copy` preserves source file attributes.
4. Therefore a ReadOnly `scripts_proxy.dll` can recreate a ReadOnly `winmm.dll`, causing the next Install run to fail at deletion.

WallSprint Mod-Tools now normalizes only the ReadOnly attribute on game-root `scripts_proxy.dll` and `winmm.dll` immediately before opening/reusing shared Overstrike. It does not modify loader bytes or other mods.

If the same UnauthorizedAccessException occurs after this normalization is installed and observed, stop repeating Install and investigate an active file lock or Windows ACL instead.

## Safety

No new native hook, runtime memory write, pointer chase, qword read or direct-CFG continuation is authorized by this result. C52 remains binding. No SocialFeed or other MOD repository is modified.
