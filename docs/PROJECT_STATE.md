# WallSprint project state

## Current stage — v0.1c38r1 orientation-only reversal implemented; functional tests passed; existing hooks-runner cwd blocker; real-game proof pending

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

## Orientation-only reversal proof — v0.1c38r1 / package0.1.57

Canonical implementation: `analysis/V01C38R1_REVERSAL_HEADING.md`.
Accepted evidence: `analysis/WALLRUN_REVERSAL_RUNTIME_RESULT_20261030.md`.
The source-active reversal numerically retained Transform Forward while
position-difference motion reversed; later source-inactive comparison changed
Forward. No independent animation/input ownership follows.

The unchanged initial orientation transition is followed by a bounded
forward/back-only displacement observer: >=100ms/dt<=250ms, tangent speed>=2,
dot(entry Forward)>=0.8 or <=-0.8, two consecutive new-sign observations >=100ms
apart. The accepted turn rotates positively about the same wall normal through
180 degrees over0.25 active seconds, without mid-turn retarget or cumulative
heading drift. Invalid/gap/stale/session history does not bridge confirmation.
The thresholds are experimental, not engine constants.

Only the existing coherent A/B orientation basis is published differently;
Position/W/metadata/scales, all admission and stale/cleanup/timeout guards and
five-second budget remain. No movement/input/velocity/animation writes, c73,
new hooks/watches, steering assets or shared infrastructure changes.

Focused math (10 groups), portable (23), Windows build/smoke and real A/B
fixture checks passed under Wine. The canonical hooks command reaches830 PASS
then stops at an unchanged c67 runpy relative-path defect (`source/source/tools`).
Its remaining tests pass when invoked from repository root. This is a separate
existing runner blocker, not permission to bypass Update-WS validation.
Automated results/publication are recorded in CODEX_RESULT. Actual r1 game
response remains UNKNOWN. After the separate canonical-runner blocker is resolved, existing Update-WS.cmd
remains the owner deployment path; no cloud deployment or new human test prose here.

## Accepted c72 outcome

Corrected c72 owner OFFLINE result remains **VERIFIED-STATIC**:

```text
repository_result = POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
c71 records = 102/102
all-successor-exact = 95/95
exact edges = 99/99/99
post_c71_uncertified_boundary_refusals = 54
reader/writer/rmw/address-only candidates = 0/0/0/0
live_qword_reads = 0
```

Direct-CFG continuation is paused. Do not automatically open c73.

Canonical detail:

- `analysis/V01C72_OWNER_OFFLINE_RESULT.md`
- `analysis/VANILLA_WALLRUN_STEERING_CONFIG_20261029.md`
- `analysis/WALLRUN_STEERING_PROBE_AB_RESULT_20261029.md`
- `analysis/WALLRUN_MOVEMENT_FACING_SOURCE_REVIEW_20261030.md`

## Current-build vanilla steering evidence

Authenticated MSM2 v2.810.0.0 vanilla assets:

```text
configs/hero/hero_WallRunSteeringSetup_Standard.config
asset id 948FA063808F7E18
size 544
SHA256 32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b

configs/hero/hero_WallRunSteeringSetup_Indoor.config
asset id 83DF4507531061B1
size 428
SHA256 921a3b95b522f7bbebadf53c003d5569a0ae9f6cc740d1935d682fb9e3fe9c05
```

Both decode as `WallRunSteeringSetupConfig`.

Relevant Standard values include:

```text
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

## Completed TurnSpeedFast runtime A/B

Probe:

```text
Standard.TurnSpeedFast: 100 -> 10
file offset 0x218: 0x64 -> 0x0A
```

Owner runtime result:

```text
probe ON ordinary WallRun: no visible heading-response change
probe OFF ordinary WallRun: no visible heading-response change
WS reversal with probe ON: moonwalk-like visible facing mismatch YES
WS reversal with probe OFF: moonwalk-like visible facing mismatch YES
WS disabled + probe ON ordinary WallRun: no visible heading-response change
```

**VERIFIED-RUNTIME, narrowly scoped:** the WS moonwalk symptom is baseline WS behavior, not caused by this probe. No visible ordinary-WallRun steering response was observed from `TurnSpeedFast 100 -> 10` in the tested maneuvers. This does not prove the field is unused.

Current Overstrike source confirms the probe package shape is valid for MSM2 format-version-2 stage installation, so invalid stage layout is not the leading no-effect explanation.

## Completed Slow5 probe and later extraction evidence

Owner observed no visible change from Standard `TurnSpeedSlow: 75 -> 5`.
This is a narrow owner-observed runtime result; actual Standard/Slow selection
and consumption remain **UNKNOWN**, and the isolation steps were not all
independently verified.

ChatGPT directly inspected the later supplied export (2026-10-30 JST):

```text
hero_wallrunsteeringsetup_standard(1).config
544 bytes
SHA256 d83095b5e68cc6fa20c95bc044345f7f46fd6c0a35a682df25374b140dfcec2d
exactly one byte differs from authenticated vanilla:
file offset 0x219: 75 -> 5
TurnSpeedFast at 0x218 remains 100
```

**VERIFIED-STATIC (supplied export):** the intended Slow5 payload is present.
Owner-workflow evidence supports installed replacement at the later extraction
time; source/copy hashes and a full GUI trace were not supplied. It does not
retroactively certify the earlier game session's installed state. The earlier
vanilla export does not establish installation failure. Extraction investigation
is sufficiently resolved; do not repeat it by default.

The Fast/Slow serial-probe stopping rule now applies. No further steering values,
c73, reinstall or repeated game test is requested.

## Historical c38 movement/facing source review — 2026-10-30

Canonical source locations and evidence limits:
`analysis/WALLRUN_MOVEMENT_FACING_SOURCE_REVIEW_20261030.md`.

**VERIFIED-STATIC (c38 source at 28c5fa6, before r1):**

- The reachable orientation path is telemetry `run` -> native-source `tick` ->
  `source_math::Transition`, then existing A/B caller-local source replacements.
- The entry Transform Forward is projected onto the wall to construct a saved
  target. Up becomes the wall normal; Forward/Right remain entry-derived after
  interpolation. Both source paths replace all orientation axes, not just Up.
- This runtime path does not select movement/input direction or rebuild that
  target when displacement reverses. The direction-aware `ws::Controller` and
  `ws::planar::Controller` helpers have test instantiations, not runtime callers;
  the legacy Actor writer is disabled. Movement sampling here is read-only.
- Existing diagnostics can log displacement and sampled Transform Forward with
  bounded/conditional coverage. Source Forward logs are not final Actor readback;
  retention diagnostics check Up. There is no independent visible-animation
  facing measurement or numeric three-way reversal comparison in those paths.

**HYPOTHESIS, not runtime causality:** entry-derived orientation-source retention
may separate from displacement handled elsewhere. It does not prove the final
Transform stayed fixed or explain the visible body/animation.

The subsequent `WALLRUN_REVERSAL_RUNTIME_RESULT_20261030.md` resolves the
numerical Transform-vs-displacement question for the supplied source-active
reversal. It does not certify animation ownership. The fixed r1 task now
supersedes this historical no-change handoff; runtime outcome for r1 remains
pending, with no serial steering changes or c73 authorized.

## Shared Overstrike recurrence mitigation

Two separate loader failure modes are now handled by the owner-operated `MSM2 - Overstrike` launcher preflight:

```text
1. <game>\Overstrike\winmm.dll exists even though loaders belong at game root.
2. <game>\scripts_proxy.dll or <game>\winmm.dll is marked ReadOnly.
```

The second mode explains the observed recurrence pattern if `scripts_proxy.dll` is ReadOnly: Overstrike deletes/recreates game-root `winmm.dll` during Install mods by copying `scripts_proxy.dll`, so a ReadOnly source can recreate a ReadOnly destination and make the next install fail at `File.Delete(winmm.dll)` with `UnauthorizedAccessException`.

`environment/Start-Overstrike.cmd` now performs a bounded installed-launch preflight before the Python environment checker:

- misplaced `Overstrike\winmm.dll` is preserved under `Mod-Tools\_quarantine` (or only the duplicate copy is removed when identical bytes are already preserved);
- game-root `scripts_proxy.dll` and `winmm.dll` have only their ReadOnly attribute cleared when present; no bytes are changed;
- `.MSM2-Mod-Tools.lock` is removed only when its recorded PID is demonstrably not running;
- any unexpected repair failure stops launch rather than widening cleanup.

This repair does not modify SocialFeed, ordinary MODs, WS runtime code, Profiles, or asset data.

## Safety

Runtime script is `v0.1c38r1` (package0.1.57); only the authorized post-entry
orientation-source reversal behavior changes. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All native publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch gates remain closed.
