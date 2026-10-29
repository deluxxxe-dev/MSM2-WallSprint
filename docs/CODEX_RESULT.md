# Codex result — current-build vanilla WallRun steering configs verified

2026-10-29 JST. Branch **codex/v01c20-edi-value-provenance**.

## STATUS

success

The owner supplied the two current-build vanilla WallRun steering assets requested by `docs/CODEX_NEXT.md`. Both were decoded and authenticated as **VERIFIED-STATIC (current-build vanilla asset)**.

## CHANGED

- `analysis/VANILLA_WALLRUN_STEERING_CONFIG_20261029.md`
- `docs/CODEX_RESULT.md`
- shared Wisdoms: `docs/WALLSPRINT_REFERENCE_MODS.md`

`docs/PROJECT_STATE.md` and `docs/CODEX_NEXT.md` were not modified by this result write.

## RESULT

Extraction context:

```text
MSM2 vanilla backup TOC SHA1 = D18940B7DED9CF83B6E7AB57BBD0FA43F5FD5290
Overstrike mapping = v2.810.0.0
Modding Tool used = v1.4.5
```

Assets:

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

Both decode as:

```text
WallRunSteeringSetupConfig
```

Standard:

```text
InTimeMax              0.35
InTimeMin              0.15
MoveSpeedFast          16
TurnDampFast           -40
TurnDampSlow           -20
TurnGainFast           -3.5
TurnGainSlow           -2.25
TurnMaxDown            12
TurnSpeedDecelScaleMax 2.5
TurnSpeedFast          100
TurnSpeedSlow          75
TurnSpeedSpeedScaleMax 0.72
```

Indoor:

```text
InTimeMax              0
InTimeMin              0
MoveSpeedFast          5
MoveSpeedSlow          1
TurnSpeedDecelScaleMax 3
TurnSpeedFast          180
TurnSpeedSlow          80
TurnSpeedSpeedScaleMax 0.8
```

Standard additionally has explicit damping/gain and `TurnMaxDown` fields. Standard and Indoor are not identical serialized layouts with only value changes.

## INTERPRETATION

Current-build vanilla evidence now independently confirms that WallRun steering is at least partly data-driven through separate Standard/Indoor config assets.

This does **not** establish field units or runtime causality, and it does not prove config-only control can implement arbitrary surface-relative WallSprint orientation/motion.

A minimal one-variable asset-only experiment is worth attempting before reopening another direct-CFG continuation. The best first candidate is:

```text
Standard.TurnSpeedFast
vanilla = 100
```

Only one field should change in the first experiment; restore vanilla immediately afterward and compare wall-run turning response.

## SAFETY

No asset was edited/deployed in this task. No runtime/game/native probe, hook, qword read, pointer chase or gameplay write was performed. Runtime remains `v0.1c38`. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

No SocialFeed or other MOD repository was modified.

## NEXT

Before any c73 direct-CFG continuation, decide whether to authorize the single-field Standard steering asset experiment. If config control is insufficient, compare the higher-level motion/gravity/orientation route already documented from the Scripted Flight reference evidence.
