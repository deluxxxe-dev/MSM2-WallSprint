# WallSprint project state

## Current stage — c72 accepted VERIFIED-STATIC; direct-CFG paused; current-build WallRun steering probe prepared for runtime test

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, c68, corrected c69, corrected c70, corrected c71, and corrected c72 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical recent detail:

- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C70_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_OWNER_OFFLINE_RESULT.md`
- `analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md`
- `analysis/VANILLA_WALLRUN_STEERING_CONFIG_20261029.md`
- `analysis/VANILLA_WALLRUN_STEERING_PROBE_PREPARED_20261029.md`

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

Zero candidates does not prove writer absence. It means the bounded direct-CFG route did not produce a usable candidate. Full replay still recorded material out-of-model refusal classes including `indirect_call = 5569` and `indirect_jump = 258`; these are refusal-event/root counts, not unique functions or writer evidence.

Do **not** automatically open c73. Reopen direct-CFG continuation only if a concrete later question specifically requires the remaining 54 frontier observations.

## Current-build vanilla WallRun steering evidence

Owner extracted the current vanilla Standard and Indoor steering resources using Modding Tool v1.4.5 from the game installation whose `toc.BAK` SHA1 is the known MSM2 `v2.810.0.0` vanilla TOC:

```text
D18940B7DED9CF83B6E7AB57BBD0FA43F5FD5290
```

Authenticated current-build assets:

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

Relevant Indoor fields include:

```text
InTimeMax = 0
InTimeMin = 0
MoveSpeedFast = 5
MoveSpeedSlow = 1
TurnSpeedDecelScaleMax = 3
TurnSpeedFast = 180
TurnSpeedSlow = 80
TurnSpeedSpeedScaleMax = 0.8
```

This is **VERIFIED-STATIC (current-build vanilla)** and proves that WallRun steering is at least partly data-driven. It does not prove field units, state selection, or runtime causality for any individual field.

## First asset-only runtime probe

The owner explicitly authorized proceeding without waiting for another instruction, so the next minimal test has been prepared.

Only this field changes:

```text
Standard.TurnSpeedFast: 100 -> 10
```

Authenticated file-level delta:

```text
file offset 0x218
0x64 -> 0x0A
```

`0x218` is an asset-file offset, not a game-memory/runtime offset.

Generated owner-facing package (not committed because it contains a modified proprietary game asset):

```text
MSM2-WS-SteeringProbe-TurnSpeedFast10.zip
SHA256 a61362d305f2d9fff90f941e1e518657e685b91ae94793ee68e8d316209bdd9b

MSM2-WS-SteeringProbe-TurnSpeedFast10.stage
SHA256 838bfac3948768ed438e08f9864c546f6cc740948129ecdf3f86bd52eca392d9

modified Standard config SHA256
1c7d85c6012e15275df7f813464bc683a7daa95facf35e13012dfda0be4e9106
```

Repository-owned reproducible generator and focused test:

```text
source/tools/build_wallrun_steering_probe.py
source/tools/test_build_wallrun_steering_probe.py
```

The builder fails closed unless the exact current-build vanilla size/SHA/original byte match, changes exactly one byte, packages one Standard steering asset plus `info.json`, reopens the archives, and verifies the packaged delta.

Focused local validation passed:

```text
PASS: authenticated one-byte patch changes exactly the requested offset
PASS: probe builder fails closed on size, SHA256, and original-byte mismatch
PASS: stage package contains only the intended Standard steering asset plus info.json
PASS: outer package contains exactly one stage archive
PASS: production contract pins current-build vanilla identity and TurnSpeedFast 100->10
PASS: owner-supplied current-build vanilla asset reproduces the validated probe package
```

## Runtime test objective

Establish only whether current-build `Standard.TurnSpeedFast` causally changes observable WallRun steering response.

If the probe clearly changes outdoor/Standard WallRun turning, record that narrow causal fact and continue config-first with one unknown at a time. Do not infer units or complete WallSprint feasibility.

If no effect is visible, first distinguish Standard-vs-Indoor selection, fast-state activation, runtime override, and conflicting asset replacement before abandoning the config route.

After the test, disable/remove the probe and run `Install mods` again so vanilla asset selection is restored before unrelated testing.

## Safety

Runtime script remains `v0.1c38`; c53-c72 work remains OFFLINE/STATIC. This new probe is a separate asset-only owner test and does not open native/gameplay-write gates.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch gates remain closed. Do not modify SocialFeed or other MOD repositories.

## Current bounded action

Follow `docs/CODEX_NEXT.md`: no further implementation or c73 continuation is authorized before the owner reports the single-field WallRun steering probe result.
