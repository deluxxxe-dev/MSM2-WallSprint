# Codex result — current-build WallRun steering probe prepared and validated

2026-10-29 JST. Branch **codex/v01c20-edi-value-provenance**.

## STATUS

success

The owner-authorized first asset-only runtime probe is prepared from the authenticated current-build vanilla Standard WallRun steering config.

## CHANGED

- `source/tools/build_wallrun_steering_probe.py`
- `source/tools/test_build_wallrun_steering_probe.py`
- `analysis/VANILLA_WALLRUN_STEERING_PROBE_PREPARED_20261029.md`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_NEXT.md`
- `docs/CODEX_RESULT.md`

The modified proprietary game asset/package is **not** committed to GitHub.

## RESULT

Authenticated input:

```text
configs/hero/hero_WallRunSteeringSetup_Standard.config
size 544
SHA256 32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b
TurnSpeedFast = 100
```

Probe delta:

```text
Standard.TurnSpeedFast: 100 -> 10
file offset 0x218: 0x64 -> 0x0A
```

`0x218` is an asset-file offset, not a game-memory/runtime offset.

Generated owner-facing package:

```text
MSM2-WS-SteeringProbe-TurnSpeedFast10.zip
SHA256 a61362d305f2d9fff90f941e1e518657e685b91ae94793ee68e8d316209bdd9b

MSM2-WS-SteeringProbe-TurnSpeedFast10.stage
SHA256 838bfac3948768ed438e08f9864c546f6cc740948129ecdf3f86bd52eca392d9

modified Standard config SHA256
1c7d85c6012e15275df7f813464bc683a7daa95facf35e13012dfda0be4e9106
```

Stage contents are exactly:

```text
0/configs/hero/hero_WallRunSteeringSetup_Standard.config
info.json
```

The builder fails closed on current-build input size/SHA/original-byte mismatch, changes exactly one byte, packages the asset, reopens the archives and re-verifies the one-byte delta.

Focused local validation:

```text
PASS: authenticated one-byte patch changes exactly the requested offset
PASS: probe builder fails closed on size, SHA256, and original-byte mismatch
PASS: stage package contains only the intended Standard steering asset plus info.json
PASS: outer package contains exactly one stage archive
PASS: production contract pins current-build vanilla identity and TurnSpeedFast 100->10
PASS: owner-supplied current-build vanilla asset reproduces the validated probe package
```

## SAFETY

No native hook, qword read, pointer chase, direct-CFG continuation or runtime memory write was added. Runtime script remains `v0.1c38`. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

No SocialFeed or other MOD repository was modified.

## NEXT

Owner runtime test only:

1. add/install the generated `WS Steering Probe - TurnSpeedFast 10` package through shared Overstrike;
2. ensure another active mod is not replacing the same Standard steering asset;
3. test a repeatable outdoor/Standard WallRun with strong steering input;
4. report whether turning changed clearly/subtly/not at all and in what direction;
5. exit, disable/remove the probe and run `Install mods` again to restore vanilla selection;
6. confirm normal behavior after removal.

No c73 or second config field is authorized until this one-variable result is analyzed.
