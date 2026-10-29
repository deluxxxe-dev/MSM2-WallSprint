# Codex result — first steering-probe runtime observation recorded; A/B discrimination next

2026-10-29 JST. Branch **codex/v01c20-edi-value-provenance**.

## STATUS

partial

The first current-build WallRun steering probe was installed and a runtime observation was obtained, but the owner tested behavior in the MOD WS state rather than isolating an ordinary vanilla WallRun state. Therefore the original `Standard.TurnSpeedFast` causal question is not yet complete.

## CHANGED

- `analysis/WALLRUN_STEERING_PROBE_RUNTIME_RESULT_20261029.md`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_NEXT.md`
- `docs/CODEX_RESULT.md`

No source/runtime implementation change was made in this result step.

## PROBE IDENTITY

Existing authenticated one-field probe:

```text
Standard.TurnSpeedFast: 100 -> 10
file offset 0x218: 0x64 -> 0x0A
```

Repository-owned generator/test remain:

```text
source/tools/build_wallrun_steering_probe.py
source/tools/test_build_wallrun_steering_probe.py
```

## OWNER RUNTIME OBSERVATION

Narrow visible behavior:

```text
- no visible movement-speed change in the tested WS state;
- reversing movement direction in WS reverses displacement while visible body facing remains in the prior direction, producing a moonwalk-like appearance.
```

This is recorded as an owner-observed runtime gameplay fact only.

Do not infer from it that actor transform orientation numerically remained unchanged. Animation/root-facing, mover-facing and actor transform remain distinct possible layers.

The observation also does not establish whether `TurnSpeedFast` caused the mismatch because the probe has not yet been compared against probe-disabled WS behavior.

## ORIGINAL PROBE QUESTION

Still **UNKNOWN**:

> Does current-build `Standard.TurnSpeedFast` causally change ordinary outdoor/Standard WallRun steering response?

The test must isolate ordinary vanilla WallRun rather than WS before that question can be closed.

## NEXT

No new package or code is required. Perform one A/B runtime comparison with the existing probe:

```text
A probe ON:
- ordinary outdoor vanilla WallRun steering
- WS direction reversal / moonwalk yes-no

B probe OFF after Install mods:
- ordinary outdoor vanilla WallRun baseline
- WS direction reversal / moonwalk yes-no
```

Interpretation:

```text
ordinary WallRun differs A vs B
  -> narrow runtime causality for TurnSpeedFast established.

WS moonwalk in both A and B
  -> baseline WS movement/facing mismatch, not probe artifact; prioritize higher-level motion/facing/orientation investigation.

WS moonwalk only in A
  -> investigate config interaction/state selection first.
```

Do not create c73 or add a second config field before this A/B result.

## SAFETY

Runtime script remains `v0.1c38`. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

No new native hook, runtime memory write, qword read, pointer chase, direct-CFG continuation or other MOD modification was performed.
