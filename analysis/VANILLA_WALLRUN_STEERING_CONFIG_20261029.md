# Current-build vanilla WallRun steering config review — 2026-10-29

## Scope

Owner supplied the two current-build vanilla assets requested by `docs/CODEX_NEXT.md` after extracting them with Modding Tool v1.4.5 from the game installation whose vanilla backup TOC SHA1 is:

```text
D18940B7DED9CF83B6E7AB57BBD0FA43F5FD5290
```

Current Overstrike source maps that TOC SHA1 to MSM2 `v2.810.0.0`.

The Modding Tool load path prefers `toc.BAK` when present, so this review treats the supplied files as extracted against that authenticated vanilla TOC rather than the modded live `toc`.

No asset was edited or deployed. No game/runtime test was performed.

Evidence label for decoded file structure/values below: **VERIFIED-STATIC (current-build vanilla asset)**.

## Asset identities

```text
configs/hero/hero_WallRunSteeringSetup_Standard.config
asset id 948FA063808F7E18
size 544 bytes
SHA256 32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b

configs/hero/hero_WallRunSteeringSetup_Indoor.config
asset id 83DF4507531061B1
size 428 bytes
SHA256 921a3b95b522f7bbebadf53c003d5569a0ae9f6cc740d1935d682fb9e3fe9c05
```

Both are STG-wrapped DAT1 config files with:

```text
DAT1 type magic = 0x35F7AFA5
Config Type section = 0x4A128222
Config Built section = 0xE501186F
Type = WallRunSteeringSetupConfig
```

Decoding follows the public DAT1 serialized-section format used by Overstrike/DAT1 tooling. Field semantics beyond names/data are not promoted beyond static evidence.

## Standard decoded fields

```text
Type = WallRunSteeringSetupConfig

InTimeMax                  = 0.35        (float)
InTimeMin                  = 0.15        (float)
MoveSpeedFast              = 16          (u8)
TurnDampFast               = -40         (i8)
TurnDampSlow               = -20         (i8)
TurnGainFast               = -3.5        (float)
TurnGainSlow               = -2.25       (float)
TurnMaxDown                = 12          (u8)
TurnSpeedDecelScaleMax     = 2.5         (float)
TurnSpeedFast              = 100         (u8)
TurnSpeedSlow              = 75          (u8)
TurnSpeedSpeedScaleMax     = 0.72        (float; stored ~0.7200000286)
```

## Indoor decoded fields

```text
Type = WallRunSteeringSetupConfig

InTimeMax                  = 0           (u8)
InTimeMin                  = 0           (u8)
MoveSpeedFast              = 5           (u8)
MoveSpeedSlow              = 1           (u8)
TurnSpeedDecelScaleMax     = 3           (u8)
TurnSpeedFast              = 180         (u8)
TurnSpeedSlow              = 80          (u8)
TurnSpeedSpeedScaleMax     = 0.8         (float; stored ~0.8000000119)
```

## Structural comparison

Shared field names:

```text
InTimeMax
InTimeMin
MoveSpeedFast
TurnSpeedDecelScaleMax
TurnSpeedFast
TurnSpeedSlow
TurnSpeedSpeedScaleMax
```

Indoor-only field:

```text
MoveSpeedSlow
```

Standard-only fields:

```text
TurnDampFast
TurnDampSlow
TurnGainFast
TurnGainSlow
TurnMaxDown
```

Important static observation: Standard and Indoor are not merely identical layouts with different values. Some shared names use different serialized numeric types (`InTimeMax`, `InTimeMin`, `TurnSpeedDecelScaleMax`), and Standard contains extra damping/gain/down-limit fields absent from Indoor.

## What this establishes

**VERIFIED-STATIC (current-build vanilla):**

1. `WallRunSteeringSetupConfig` exists in the authenticated current vanilla build as separate Standard and Indoor resources.
2. The steering config contains explicit timing, movement-speed and turn-speed fields.
3. Standard additionally contains turn damping/gain and `TurnMaxDown` fields.
4. Therefore WallRun steering behavior is at least partly data-driven through current-build vanilla config assets; the earlier third-party reference finding is now independently confirmed for this build.

## What this does not establish

- It does not prove units for any field.
- It does not prove which runtime state selects Standard vs Indoor.
- It does not prove a particular field directly controls wall-relative heading, facing, orientation, gravity/up-vector or arbitrary surface-relative locomotion.
- It does not prove config-only control can implement the complete intended WallSprint behavior.
- Field names are semantic hints; causality still requires a minimal runtime experiment.

## Minimal asset-only experiment assessment

A one-variable asset-only experiment is worth attempting before reopening native direct-CFG continuation.

Best first candidate:

```text
Standard.TurnSpeedFast
vanilla = 100
```

Reason:

- it is an explicit steering-named field;
- it exists in both Standard and Indoor, which gives later comparison leverage;
- changing only the Standard value keeps the experiment scoped to one asset and one unknown quantity;
- a visible change in wall-run turning response would establish runtime causality for one config field without native hooks.

Do not infer units or choose final gameplay values from the name alone. The next step should be a deliberately exaggerated but bounded single-field Standard-only test, followed by immediate restoration and a comparison against vanilla behavior. That experiment remains separately authorized work; this review performs no modification.

## Strategy implication

This current-build evidence materially strengthens the config-first alternate route after c72. Do not automatically open c73 for the remaining direct-CFG frontier. First test whether one steering field has observable runtime control. If the config route proves insufficient for the desired surface-relative direction/orientation behavior, then compare the higher-level motion/gravity/orientation route suggested by the Scripted Flight reference evidence.
