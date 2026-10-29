# WallSprint project state

## Current stage — c72 accepted; direct-CFG paused; steering Fast probe no-effect A/B complete; Slow probe next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

## Accepted c72 outcome

Corrected c72 owner OFFLINE result remains **VERIFIED-STATIC**:

```text
repository_result = POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
c71 records = 102/102
all-successor-exact = 95/95
exact edges = 99/99/99
post_c71_uncertified_boundary_refusals = 54
reader/writer/rmw/address-only candidates = 0/0/0/0
live_qword_reads = 0
```

Direct-CFG continuation is paused. Do not automatically open c73.

Canonical detail:

- `analysis/V01C72_OWNER_OFFLINE_RESULT.md`
- `analysis/VANILLA_WALLRUN_STEERING_CONFIG_20261029.md`
- `analysis/WALLRUN_STEERING_PROBE_AB_RESULT_20261029.md`

## Current-build vanilla steering evidence

Authenticated MSM2 v2.810.0.0 vanilla assets:

```text
configs/hero/hero_WallRunSteeringSetup_Standard.config
asset id 948FA063808F7E18
size 544
SHA256 32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b

configs/hero/hero_WallRunSteeringSetup_Indoor.config
asset id 83DF4507531061B1
size 428
SHA256 921a3b95b522f7bbebadf53c003d5569a0ae9f6cc740d1935d682fb9e3fe9c05
```

Both decode as `WallRunSteeringSetupConfig`.

Relevant Standard values include:

```text
MoveSpeedFast = 16
TurnDampFast = -40
TurnDampSlow = -20
TurnGainFast = -3.5
TurnGainSlow = -2.25
TurnMaxDown = 12
TurnSpeedDecelScaleMax = 2.5
TurnSpeedFast = 100
TurnSpeedSlow = 75
TurnSpeedSpeedScaleMax = 0.72
```

## Completed TurnSpeedFast runtime A/B

Probe:

```text
Standard.TurnSpeedFast: 100 -> 10
file offset 0x218: 0x64 -> 0x0A
```

Owner runtime result:

```text
probe ON ordinary WallRun: no visible heading-response change
probe OFF ordinary WallRun: no visible heading-response change
WS reversal with probe ON: moonwalk-like visible facing mismatch YES
WS reversal with probe OFF: moonwalk-like visible facing mismatch YES
WS disabled + probe ON ordinary WallRun: no visible heading-response change
```

**VERIFIED-RUNTIME, narrowly scoped:** the WS moonwalk symptom is baseline WS behavior, not caused by this probe. No visible ordinary-WallRun steering response was observed from `TurnSpeedFast 100 -> 10` in the tested maneuvers. This does not prove the field is unused.

Current Overstrike source confirms the probe package shape is valid for MSM2 format-version-2 stage installation, so invalid stage layout is not the leading no-effect explanation.

## Next minimal steering discrimination

One further one-field probe is prepared:

```text
Standard.TurnSpeedSlow: 75 -> 5
file offset 0x219: 0x4B -> 0x05
```

Repository-owned reproducible files:

```text
source/tools/build_wallrun_steering_slow_probe.py
source/tools/test_build_wallrun_steering_slow_probe.py
```

Runtime test must disable WallSprint and the prior Fast probe, enable only the Slow probe, install mods, and test ordinary outdoor WallRun heading response.

Stopping rule: if this also has no visible effect, stop serial TurnSpeedFast/Slow config probing and move to the higher-level movement/facing/orientation investigation. Do not open c73 merely because the config route did not yield a visible effect.

## Shared Overstrike recurrence mitigation

Two separate loader failure modes are now handled by the owner-operated `MSM2 - Overstrike` launcher preflight:

```text
1. <game>\Overstrike\winmm.dll exists even though loaders belong at game root.
2. <game>\scripts_proxy.dll or <game>\winmm.dll is marked ReadOnly.
```

The second mode explains the observed recurrence pattern if `scripts_proxy.dll` is ReadOnly: Overstrike deletes/recreates game-root `winmm.dll` during Install mods by copying `scripts_proxy.dll`, so a ReadOnly source can recreate a ReadOnly destination and make the next install fail at `File.Delete(winmm.dll)` with `UnauthorizedAccessException`.

`environment/Start-Overstrike.cmd` now performs a bounded installed-launch preflight before the Python environment checker:

- misplaced `Overstrike\winmm.dll` is preserved under `Mod-Tools\_quarantine` (or only the duplicate copy is removed when identical bytes are already preserved);
- game-root `scripts_proxy.dll` and `winmm.dll` have only their ReadOnly attribute cleared when present; no bytes are changed;
- `.MSM2-Mod-Tools.lock` is removed only when its recorded PID is demonstrably not running;
- any unexpected repair failure stops launch rather than widening cleanup.

This repair does not modify SocialFeed, ordinary MODs, WS runtime code, Profiles, or asset data.

## Safety

Runtime script remains `v0.1c38`. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All native publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch gates remain closed.
