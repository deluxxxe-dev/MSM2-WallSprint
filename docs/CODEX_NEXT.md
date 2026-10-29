# Next task — owner A/B runtime discrimination of WallRun steering probe and WS facing mismatch; no c73 yet

## Status

Corrected c72 owner OFFLINE result is accepted as **VERIFIED-STATIC** and the direct-CFG route is paused.

Current-build vanilla steering configs are independently authenticated and decoded. The first asset-only probe changes only:

```text
Standard.TurnSpeedFast: 100 -> 10
```

The owner has now reported a first runtime observation while the probe was active, but the observation was made in the MOD WS state rather than an ordinary vanilla WallRun state:

```text
- no visible movement-speed change in the tested WS state;
- when reversing movement direction in WS, displacement reverses while visible body facing remains in the prior direction, producing a moonwalk-like appearance.
```

Canonical detail:

- `analysis/V01C72_OWNER_OFFLINE_RESULT.md`
- `analysis/VANILLA_WALLRUN_STEERING_CONFIG_20261029.md`
- `analysis/VANILLA_WALLRUN_STEERING_PROBE_PREPARED_20261029.md`
- `analysis/WALLRUN_STEERING_PROBE_RUNTIME_RESULT_20261029.md`
- `docs/PROJECT_STATE.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Evidence boundary

The visible WS mismatch is a runtime gameplay fact, but it does **not** yet prove:

- actor transform orientation numerically stayed unchanged;
- animation/root facing is the cause;
- `TurnSpeedFast` caused the mismatch;
- `TurnSpeedFast` is unused;
- vanilla WallRun ignores the Standard steering asset.

The original probe question remains unresolved because ordinary outdoor/Standard WallRun was not isolated.

## Current task ownership

### Codex

No implementation task is authorized now.

Do not:

- create c73;
- add another config field or another probe;
- change the current probe value;
- touch Indoor;
- add native hooks, runtime reads, pointer chasing or gameplay writes;
- infer actor/mover/animation ownership from the visible moonwalk symptom;
- modify SocialFeed or another MOD repository.

### ChatGPT / owner runtime test

Perform one A/B comparison using the existing probe package. No new package or build is required.

#### A — probe enabled

1. Keep `WS Steering Probe - TurnSpeedFast 10` enabled and run `Install mods`.
2. Launch the game.
3. Test an **ordinary outdoor vanilla WallRun**, not WS, with strong left/right steering input.
4. Record whether ordinary WallRun heading response is clearly slower / subtly different / unchanged.
5. Enter the MOD WS state and reverse movement direction.
6. Record whether the moonwalk-like visible body-facing mismatch occurs.

#### B — probe disabled

1. Exit the game.
2. Disable/remove only `WS Steering Probe - TurnSpeedFast 10`.
3. Run `Install mods` again.
4. Launch the game.
5. Repeat the same ordinary outdoor vanilla WallRun steering test.
6. Repeat the same WS direction-reversal maneuver.

Return four compact observations:

```text
A ordinary WallRun: slower / subtle / unchanged / other
A WS reversal: moonwalk yes / no
B ordinary WallRun: baseline behavior description
B WS reversal: moonwalk yes / no
```

No log is required unless a crash/load/install failure occurs.

## Decision after A/B

### Ordinary WallRun differs A vs B

Record a narrow **VERIFIED-RUNTIME** causal fact that `Standard.TurnSpeedFast` influences ordinary Standard WallRun steering response. Do not infer units.

Then choose the next single config quantity only if it answers a concrete WallSprint direction/facing question.

### WS moonwalk occurs in both A and B

Treat the moonwalk as baseline WS behavior rather than a probe artifact. This materially strengthens the case for investigating the WS movement/facing/orientation split. The next investigation should distinguish actor/mover facing from animation presentation and compare the higher-level motion/gravity/orientation route already suggested by Scripted Flight evidence. Do not reopen c73 merely for this symptom.

### WS moonwalk occurs only in A

The asset probe participates in the visible mismatch. Investigate config interaction/state selection before moving to a higher-level native path.

### Ordinary WallRun unchanged A vs B

Do not declare `TurnSpeedFast` unused. First distinguish Standard-vs-Indoor selection, whether the tested situation reaches the Fast steering branch, runtime override/thresholds, and conflicting asset replacement. Pick the smallest discriminating follow-up; do not stack fields.

## Binding safety

Runtime script remains `v0.1c38`. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All native publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch gates remain closed. This A/B asset test does not authorize native gameplay writes.
