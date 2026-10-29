# Next task — owner runtime test of single-field WallRun steering probe; no c73 yet

## Status

Corrected c72 owner OFFLINE result is accepted as **VERIFIED-STATIC** and the direct-CFG route is paused.

Current-build vanilla steering configs have now been independently obtained and decoded:

```text
configs/hero/hero_WallRunSteeringSetup_Standard.config
asset id 948FA063808F7E18
size 544
SHA256 32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b
Type WallRunSteeringSetupConfig

configs/hero/hero_WallRunSteeringSetup_Indoor.config
asset id 83DF4507531061B1
size 428
SHA256 921a3b95b522f7bbebadf53c003d5569a0ae9f6cc740d1935d682fb9e3fe9c05
Type WallRunSteeringSetupConfig
```

Canonical detail:

- `analysis/V01C72_OWNER_OFFLINE_RESULT.md`
- `analysis/VANILLA_WALLRUN_STEERING_CONFIG_20261029.md`
- `analysis/VANILLA_WALLRUN_STEERING_PROBE_PREPARED_20261029.md`
- `docs/PROJECT_STATE.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Current decision

The owner explicitly authorized proceeding directly to the next minimal test without waiting for another approval.

The first asset-only runtime probe is therefore prepared:

```text
Standard.TurnSpeedFast: 100 -> 10
```

Authenticated file-level delta:

```text
file offset 0x218
0x64 -> 0x0A
```

This is an asset-file offset only, not a game-memory/runtime offset.

The generated owner-facing package is intentionally not committed because it contains a modified proprietary game asset:

```text
MSM2-WS-SteeringProbe-TurnSpeedFast10.zip
SHA256 a61362d305f2d9fff90f941e1e518657e685b91ae94793ee68e8d316209bdd9b
```

Repository-owned reproducible builder/test:

```text
source/tools/build_wallrun_steering_probe.py
source/tools/test_build_wallrun_steering_probe.py
```

Focused local validation passed six checks, including reproduction from the authenticated owner-supplied current-build vanilla asset.

## Task ownership

### Codex

No further implementation task is authorized now.

Do not:

- create c73;
- add more config changes;
- add native hooks, runtime reads, pointer chasing or gameplay writes;
- change the probe value or touch Indoor;
- modify SocialFeed or another MOD repository.

If invoked before the owner runtime result exists, only verify the current repository state/tests if specifically requested; otherwise stop and report that the owner runtime probe is the next evidence dependency.

### ChatGPT / owner runtime test

Test only this question:

> Does current-build `Standard.TurnSpeedFast` causally change observable WallRun steering response?

Procedure:

1. Add/install `MSM2-WS-SteeringProbe-TurnSpeedFast10.zip` through the shared Overstrike environment.
2. Ensure no other active mod overrides `hero_WallRunSteeringSetup_Standard.config`; temporarily disable a conflicting traversal mod if necessary.
3. Launch the modded game.
4. Use a repeatable ordinary outdoor/Standard WallRun situation.
5. Apply strong steering input comparable to vanilla behavior and observe turning/heading response.
6. Exit the game.
7. Disable/remove the steering probe and run `Install mods` again to restore vanilla asset selection before unrelated tests.

Return only the relevant behavior result:

- changed clearly / changed subtly / no visible change;
- slower / faster / unstable / other qualitative change;
- any crash/load failure or inability to enter WallRun;
- whether behavior returned to normal after removing the probe.

No runtime log is required unless an unexpected loader/crash problem produces one.

## Decision after runtime result

### Clear steering change

Record a narrow **VERIFIED-RUNTIME** causal fact that `Standard.TurnSpeedFast` influences observed WallRun steering response. Do not assign units from the name/value alone.

Then choose the next single config quantity only if it answers a concrete WallSprint direction-control question. Continue config-first before reopening long native exploration.

### No visible change

Do not declare the field unused. First distinguish:

- Standard vs Indoor asset selection;
- whether the tested situation reaches the "Fast" steering branch;
- runtime overrides/thresholds;
- another active mod overriding the same asset.

Choose the smallest discriminating follow-up; do not stack multiple changed fields.

### Crash/load failure

Restore vanilla selection and stop. Diagnose package/asset compatibility before any second config experiment.

## Binding safety

Runtime script remains `v0.1c38`. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All native publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch gates remain closed. This asset-only probe does not authorize native gameplay writes.
