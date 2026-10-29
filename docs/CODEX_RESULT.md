# Codex result — Fast steering A/B negative; Slow probe prepared; Overstrike launcher hardened

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## STATUS

success

## CHANGED

- `analysis/WALLRUN_STEERING_PROBE_AB_RESULT_20261029.md`
- `source/tools/build_wallrun_steering_slow_probe.py`
- `source/tools/test_build_wallrun_steering_slow_probe.py`
- `environment/Start-Overstrike.cmd`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_NEXT.md`
- `docs/CODEX_RESULT.md`

## RESULT

Completed owner A/B for:

```text
Standard.TurnSpeedFast: 100 -> 10
```

Observed:

```text
ordinary WallRun probe ON: unchanged
ordinary WallRun probe OFF: unchanged
WS reversal probe ON: moonwalk YES
WS reversal probe OFF: moonwalk YES
WS disabled + probe ON ordinary WallRun: unchanged
```

Therefore the visible WS moonwalk mismatch is not caused by the Fast probe. The Fast probe produced no visible ordinary-WallRun heading-response change in the tested maneuvers; this is not proof the field is unused.

One further one-field discrimination is prepared:

```text
Standard.TurnSpeedSlow: 75 -> 5
offset 0x219: 0x4B -> 0x05
```

The owner-facing package is not committed because it contains a modified proprietary game asset. The repository contains a fail-closed reproducible builder and focused contract test.

Stopping rule: if the Slow probe also produces no visible ordinary-WallRun heading change, stop serial Fast/Slow steering probes and move to the higher-level WS movement/facing/orientation investigation.

## OVERSTRIKE RECURRENCE

The shared environment repeatedly observed a misplaced:

```text
<game>\Overstrike\winmm.dll
```

The installed `MSM2 - Overstrike` launcher source now runs a bounded preflight before the Python environment checker:

- if that misplaced file exists, preserve it under `Mod-Tools\_quarantine` keyed by SHA256;
- if the same quarantined bytes already exist, remove only the duplicate misplaced copy;
- leave the game-root `winmm.dll` and `scripts_proxy.dll` untouched;
- remove `.MSM2-Mod-Tools.lock` only when its recorded PID is demonstrably not running;
- fail rather than widening cleanup if the repair itself fails.

The installed launcher must be refreshed once from the updated repository after the current occurrence is repaired.

## SAFETY

Runtime script remains `v0.1c38`. C52 remains binding. Direct-CFG c73 remains paused. No SocialFeed or other MOD repository was modified.
