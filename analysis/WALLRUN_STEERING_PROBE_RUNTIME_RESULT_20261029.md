# WallRun steering probe runtime result — 2026-10-29

## Scope

Owner tested the prepared current-build asset-only probe:

```text
Standard.TurnSpeedFast: 100 -> 10
```

The active WallSprint runtime script remained the existing runtime build; the probe itself only replaced the authenticated Standard WallRun steering config asset.

The owner also supplied a gameplay video. Raw video is not committed to the repository. The evidence recorded here is the owner's direct runtime observation and the already-authenticated probe/package identity.

## Owner-observed result

**VERIFIED-RUNTIME (owner-observed visible behavior), narrowly scoped:**

```text
- No visible movement-speed change was observed in the tested WS state.
- In the MOD WallSprint (WS) state, reversing movement direction caused displacement to reverse while the visible character/body facing remained in the prior direction, producing a moonwalk-like appearance.
```

This is a visible gameplay fact only.

Do **not** promote it to any of the following without further evidence:

- actor transform orientation remained numerically unchanged;
- animation root/facing is the sole cause;
- `TurnSpeedFast` caused the mismatch;
- `TurnSpeedFast` is unused;
- vanilla WallRun steering ignores the Standard asset.

The visual body direction may come from actor transform, animation/root motion, a mover-facing layer, or another presentation/control layer. The current result does not distinguish them.

## Important test-scope correction

The original probe question was:

> Does current-build `Standard.TurnSpeedFast` causally change observable ordinary outdoor/Standard WallRun steering response?

The reported observation was made in the MOD WS state rather than an ordinary vanilla WallRun state. Therefore the original causal question is still **UNKNOWN**.

The lack of speed change in WS does not answer the steering-field question because `TurnSpeedFast` was not selected as a speed-field probe and WS may not use the same steering path/state.

## New high-value symptom

The WS state currently permits a visible separation between:

```text
movement/displacement direction
```

and

```text
visible character/body facing direction
```

when the owner reverses direction.

This is directly relevant to WallSprint because a finished wall-sprint should not visually travel backward while the body remains pointed the opposite way unless that behavior is explicitly intended.

At present the responsible layer is **UNKNOWN**.

Plausible classes to discriminate later, not established facts:

- movement vector changes while actor/mover facing is not updated;
- actor orientation changes but animation presentation does not follow;
- WS custom motion bypasses vanilla WallRun steering/facing logic;
- the steering probe affects one layer while WS motion overrides another.

## Next minimal discriminating runtime test

No new asset or code change is required yet.

Perform an A/B comparison with exactly the same existing probe package:

### A — probe enabled

1. Keep `WS Steering Probe - TurnSpeedFast 10` enabled and install mods.
2. Test an **ordinary outdoor vanilla WallRun**, not WS, with strong left/right steering input.
3. Record only whether ordinary WallRun heading response is clearly slower / subtly different / unchanged.
4. Reproduce the WS direction-reversal maneuver and record whether the moonwalk-like visual mismatch occurs.

### B — probe disabled

1. Exit the game.
2. Disable/remove only the steering probe.
3. Run `Install mods` again to restore vanilla Standard steering asset selection.
4. Repeat the same ordinary outdoor WallRun steering test.
5. Repeat the same WS direction-reversal maneuver.

Interpretation:

```text
ordinary WallRun differs A vs B
  -> TurnSpeedFast has runtime causal influence on ordinary Standard WallRun steering.

WS moonwalk occurs in both A and B
  -> moonwalk is baseline WS behavior, not caused by this probe.

WS moonwalk occurs only in A
  -> the probe participates in the visible direction/facing mismatch; investigate config interaction before moving higher-level.

ordinary WallRun unchanged A vs B
  -> do not call the field unused yet; next discriminate Standard selection / Fast branch / override/conflict.
```

## Strategy implication

The new WS symptom increases the value of the previously documented higher-level motion/facing/orientation investigation (e.g. movement request + gravity/up + facing/orientation) **if** A/B confirms that the mismatch is baseline WS behavior rather than a probe artifact.

Do not reopen c73 direct-CFG continuation for this symptom. First finish the A/B discrimination above.

## Safety

No new native hook, runtime memory write, pointer chase, qword read, config modification, or second probe is authorized by this result. Existing C52 safety gates remain binding. No other MOD repository is modified.
