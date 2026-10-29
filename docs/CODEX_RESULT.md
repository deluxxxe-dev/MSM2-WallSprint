# Codex result — Fast steering A/B negative; Slow probe prepared; Overstrike launcher hardened

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## STATUS

success

## CHANGED

- `analysis/WALLRUN_STEERING_PROBE_AB_RESULT_20261029.md`
- `source/tools/build_wallrun_steering_slow_probe.py`
- `source/tools/test_build_wallrun_steering_slow_probe.py`
- `environment/Start-Overstrike.cmd`
- `environment/test_start_overstrike_preflight.py`
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

Two recurring shared-environment failures have been observed:

```text
<game>\Overstrike\winmm.dll       # misplaced loader
<game>\winmm.dll                  # Install mods cannot delete: UnauthorizedAccessException
```

Current public Overstrike code deletes game-root `winmm.dll` during script setup and recreates it by copying game-root `scripts_proxy.dll`. A ReadOnly source can therefore recreate a ReadOnly destination, matching the observed pattern where one manual repair succeeds and a later Install mods fails again.

The installed `MSM2 - Overstrike` launcher source now runs a bounded preflight before the Python environment checker:

- if misplaced `Overstrike\winmm.dll` exists, preserve it under `Mod-Tools\_quarantine` keyed by SHA256;
- if the same quarantined bytes already exist, remove only the duplicate misplaced copy;
- if game-root `scripts_proxy.dll` or `winmm.dll` is ReadOnly, clear only that attribute and verify it cleared;
- remove `.MSM2-Mod-Tools.lock` only when its recorded PID is demonstrably not running;
- fail rather than widening cleanup if any repair itself fails.

This preflight does not change loader bytes, Profiles, MOD assets, SocialFeed, ordinary MODs, or WS runtime content.

A focused static regression test pins the repair scope to only these loader/stale-environment cases.

If `UnauthorizedAccessException` recurs after the updated launcher has normalized both root loader attributes, the next diagnosis is an active file lock or Windows ACL; do not repeat Install mods unchanged.

## VALIDATION

The Slow package was regenerated from the authenticated current-build Standard asset and verified as exactly one byte different at offset `0x219` (`75 -> 5`). The prepared package identity is:

```text
MSM2-WS-SteeringProbe-TurnSpeedSlow5.zip
SHA256 f9d10c6e3988f464a287c362d9d7312620cfa46f6af01a124671728519d8af79
```

Existing Fast builder/test remain single-purpose. Slow uses its separate repository-owned builder/test.

## SAFETY

Runtime script remains `v0.1c38`. C52 remains binding. Direct-CFG c73 remains paused. No SocialFeed or other MOD repository was modified.
