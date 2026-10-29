# WallSprint project state

## Current stage — c72 accepted VERIFIED-STATIC; direct-CFG paused; steering probe runtime A/B discrimination in progress

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, c68, corrected c69, corrected c70, corrected c71, and corrected c72 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical recent detail:

- `analysis/V01C72_OWNER_OFFLINE_RESULT.md`
- `analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md`
- `analysis/VANILLA_WALLRUN_STEERING_CONFIG_20261029.md`
- `analysis/VANILLA_WALLRUN_STEERING_PROBE_PREPARED_20261029.md`
- `analysis/WALLRUN_STEERING_PROBE_RUNTIME_RESULT_20261029.md`

## Accepted c72 outcome

```text
MSM2-WS-v0.1c72-post-c71-stitched.json
SHA256 = 077955562f1cdd9ba966fd1104ecd2b3b258f3aa25384418032fa5099149d32d
repository_result = POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
c71 records = 102/102
all-successor-exact = 95/95
exact edges = 99/99/99
post_c71_uncertified_boundary_refusals = 54
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
live_qword_reads = 0
```

Zero candidates does not prove writer absence. The direct-CFG route is paused. Full replay retained material out-of-model refusal classes including `indirect_call = 5569` and `indirect_jump = 258`; these are refusal-event/root counts, not unique functions or writer evidence.

Do **not** automatically open c73. Reopen direct-CFG continuation only if a concrete later question specifically requires the remaining 54 frontier observations.

## Current-build vanilla WallRun steering evidence

Authenticated current-build assets from MSM2 `v2.810.0.0` vanilla TOC:

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

Both decode as `WallRunSteeringSetupConfig`.

Relevant Standard fields include:

```text
InTimeMax = 0.35
InTimeMin = 0.15
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

This is **VERIFIED-STATIC (current-build vanilla)** and proves WallRun steering is at least partly data-driven. It does not prove field units, runtime state selection or causality for any individual field.

## First asset-only steering probe

Prepared probe changes exactly one field:

```text
Standard.TurnSpeedFast: 100 -> 10
file offset 0x218: 0x64 -> 0x0A
```

The package is not committed because it contains a modified proprietary game asset. Repository-owned generator/tests remain:

```text
source/tools/build_wallrun_steering_probe.py
source/tools/test_build_wallrun_steering_probe.py
```

## First owner runtime observation

Owner tested with the probe active but reported behavior from the **MOD WS state**, not the intended ordinary vanilla WallRun state.

Narrow runtime observation:

```text
- no visible movement-speed change in the tested WS state;
- reversing movement direction in WS reversed displacement while visible body facing stayed in the prior direction, producing a moonwalk-like appearance.
```

Treat this as a visible runtime fact only. It does **not** prove actor-transform orientation remained unchanged, because animation/root/facing presentation may be a separate layer.

The original question — whether `Standard.TurnSpeedFast` causally changes ordinary outdoor/Standard WallRun steering — remains **UNKNOWN** because that state was not yet isolated.

The moonwalk symptom is important because it shows a visible separation between WS movement direction and visible facing. Whether it is baseline WS behavior or introduced by the steering probe is not yet known.

## Current bounded action

Perform one A/B runtime discrimination using the already-prepared probe; no new package is needed.

### A — probe enabled

- ordinary outdoor vanilla WallRun: strong left/right steering, record heading response;
- WS state: reverse direction, record whether moonwalk-like facing mismatch occurs.

### B — probe disabled

- exit game;
- disable/remove only the steering probe;
- run `Install mods` again;
- repeat the same ordinary outdoor WallRun steering test;
- repeat the same WS direction-reversal maneuver.

Interpretation:

```text
ordinary WallRun differs A vs B -> TurnSpeedFast has runtime causal influence on ordinary Standard WallRun steering.
WS moonwalk in both A and B -> baseline WS behavior, not caused by this probe.
WS moonwalk only in A -> config probe participates in mismatch; investigate config interaction.
ordinary WallRun unchanged A vs B -> next discriminate Standard selection / Fast branch / override/conflict.
```

If A/B establishes the moonwalk as baseline WS behavior, prioritize the higher-level motion/facing/orientation route already suggested by Scripted Flight evidence before reopening long direct-CFG continuation.

## Safety

Runtime script remains `v0.1c38`; c53-c72 work remains OFFLINE/STATIC. The steering probe is a separate asset-only owner test.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All native publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch gates remain closed. Do not modify SocialFeed or other MOD repositories.
