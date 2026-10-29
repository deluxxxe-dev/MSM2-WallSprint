# Current-build vanilla WallRun steering probe prepared — 2026-10-29

## Status

Prepared and locally validated. No gameplay test has been run yet.

Evidence basis:

- current-build vanilla Standard steering asset is already accepted as **VERIFIED-STATIC** in `analysis/VANILLA_WALLRUN_STEERING_CONFIG_20261029.md`;
- owner-supplied vanilla asset SHA256 is pinned before any modification;
- the generated probe changes exactly one byte in that authenticated asset;
- no native hook, runtime pointer, qword read, CFG continuation or gameplay write code is involved.

## Authenticated source asset

```text
configs/hero/hero_WallRunSteeringSetup_Standard.config
asset id 948FA063808F7E18
size 544 bytes
SHA256 32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b
Type = WallRunSteeringSetupConfig
```

The decoded vanilla field selected for the first runtime causality test is:

```text
TurnSpeedFast = 100
```

## Probe change

The test changes only:

```text
Standard.TurnSpeedFast: 100 -> 10
```

In the authenticated 544-byte file this is a one-byte file-level change:

```text
file offset 0x218
0x64 -> 0x0A
```

`0x218` is an asset-file byte offset, not a game-memory/runtime offset.

Rationale for using a low value first:

- it is deliberately large enough to make a causal turning-response change observable if the field is active;
- lowering the value is less likely to create violent/unstable turning than first testing an extreme high value;
- only one field/one asset changes, preserving a clean attribution test.

Field units and exact semantics remain unknown until runtime observation.

## Generated package identity

The owner-facing package was generated from the supplied authenticated vanilla asset and was not committed to Git because it contains a modified proprietary game asset.

```text
MSM2-WS-SteeringProbe-TurnSpeedFast10.zip
SHA256 a61362d305f2d9fff90f941e1e518657e685b91ae94793ee68e8d316209bdd9b
```

It contains exactly one stage archive:

```text
MSM2-WS-SteeringProbe-TurnSpeedFast10.stage
SHA256 838bfac3948768ed438e08f9864c546f6cc740948129ecdf3f86bd52eca392d9
```

The stage contains only:

```text
0/configs/hero/hero_WallRunSteeringSetup_Standard.config
info.json
```

Modified config SHA256:

```text
1c7d85c6012e15275df7f813464bc683a7daa95facf35e13012dfda0be4e9106
```

## Reproducible builder

Repository-owned builder:

```text
source/tools/build_wallrun_steering_probe.py
```

It fails closed unless the input matches all of:

```text
size = 544
SHA256 = 32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b
byte[0x218] = 100
```

It then changes only byte `0x218` to `10`, packages the Standard config into a format-2 MSM2 `.stage`, wraps it in one `.zip`, reopens both archives, and rechecks that the packaged asset differs from vanilla at exactly that one offset.

Focused test:

```text
source/tools/test_build_wallrun_steering_probe.py
```

Local validation result:

```text
PASS: authenticated one-byte patch changes exactly the requested offset
PASS: probe builder fails closed on size, SHA256, and original-byte mismatch
PASS: stage package contains only the intended Standard steering asset plus info.json
PASS: outer package contains exactly one stage archive
PASS: production contract pins current-build vanilla identity and TurnSpeedFast 100->10
PASS: owner-supplied current-build vanilla asset reproduces the validated probe package
```

## Runtime test objective

Establish only one question:

> Does current-build `Standard.TurnSpeedFast` causally change observable WallRun steering response?

Do not use this test to infer field units, final tuning values, arbitrary-surface locomotion support, gravity/orientation ownership, or complete WallSprint feasibility.

## Human test

1. Add/install only the generated `WS Steering Probe - TurnSpeedFast 10` asset probe through shared Overstrike.
2. Ensure no other active mod overrides `hero_WallRunSteeringSetup_Standard.config`; if uncertain, temporarily disable traversal mods that replace the same asset.
3. Run the game and use a repeatable ordinary outdoor/Standard WallRun situation.
4. Apply the same strong steering input used in normal WallRun and observe whether turning/heading response becomes substantially slower or otherwise changes.
5. Exit the game.
6. Disable/remove the probe and run `Install mods` again to restore vanilla asset selection before unrelated testing.

Report:

- whether Standard WallRun turning changed clearly / subtly / not at all;
- whether the effect was slower, faster, unstable, or qualitatively different;
- whether ordinary movement returned to normal after removing the probe;
- any crash/load failure or inability to enter WallRun.

## Decision rule

- Clear turning-response change: promote `Standard.TurnSpeedFast` to **VERIFIED-RUNTIME causal influence on observed WallRun steering response** (without assigning units); next test should remain config-first and change one further quantity only if needed.
- No observable change: do not declare the field unused. First distinguish Standard-vs-Indoor selection, runtime override, threshold/fast-state activation, and conflicting asset override before abandoning the config route.
- Crash/load failure: stop; treat package/asset compatibility as the immediate issue and restore vanilla selection. Do not stack additional config changes.

Direct-CFG c73 remains paused while this materially different route is tested.
