# WallSprint project state

## Current task — r5 wall-plane facing implementation

Owner baseline confirms up/down/diagonal translation already exists; do not treat lateral motion as absent. Sideways body/animation persists, descent slower/alternating facing, stick release returns vanilla cling. See analysis/R4_DIRECTIONAL_BASELINE_20261031.md. Compressed follow-up シーケンス 01.mp4 (36.416667s) successfully inspected: visible wall-travel/sideways-facing mismatch and later cling posture. Camera roll prevents screen-space direction inference; exact input/log alignment and quantitative descent speed remain unverified. Current heading targets only+/-entry Forward; next CODEX_NEXT authorizes minimum full-wall-plane displacement-facing orientation proof only. Speed/animation/idle and earlier INPUT START deferred, not solved. Owner bootstrap succeeded; later game run confirms basic launch recovery, not every tooling case.


## r5 implementation / acceptance pending

v0.1c38r5/package0.1.61 replaces binary +/-entry targets with authenticated
world-space wall-tangent displacement facing. Experimental12deg heading deadband,
8deg first-candidate stability; existing33ms/two samples/speed>=2/gap250ms,
entry0.25s and turn120ms retained. Proper A/B basis, stale/native/identity/4096
budgets unchanged. WALL_PLANE_HEADING schema2 vectors/angles replaces binary
sign logs;64 total events and at most8 hold events/session. See
analysis/V01C38R5_WALL_PLANE_HEADING.md; exact validation/publication CODEX_RESULT.
No translation/input/animation/idle/corner change; visual body acceptance pending.
Owner test procedure supplied by Chat, not invented from camera screen axes.

## Previous handoff — update ownership / Mod-Tools 1.2.0

Implemented tooling-only OS-held shared ownership, commit-before-checked-launch,
and a fresh Git-authenticated bootstrap that repairs installed KIT before old
checkers can reject it. Audit records persist intentionally; do not delete them.
Unknown/active/malformed/reparse locks refuse; empty legacy recovery requires
Windows process and all running WSL distributions to be proved inactive.
See analysis/UPDATE_LOCK_TOOLKIT_RECOVERY.md and docs/CODEX_RESULT.md.
Cloud regressions passed; supplied owner repair marker and subsequent game run
now confirm basic launch usability, not all Windows/CIM/WSL/concurrency cases.
Tooling-only handoff kept r4 bytes unchanged; current r5 task above supersedes
its movement pause/repair-next-action wording. Publication was independently verified.

## Active handoff — lateral movement route investigation

Owner r4 test accepted for fixed5s-expiry removal: session3 eleven completed reversals,27.031s override until verified native Deinit, no hard kill. One40ms stale episode recovered. See analysis/V01C38R4_OWNER_RUNTIME_20261031.md; this is bounded acceptance, not unlimited lifetime proof.
Owner explicitly allows earlier facing START to be deferred. It remains unresolved, not accepted as fixed. Current CODEX_NEXT supersedes old lifetime/input queue and authorizes only one-flat-wall lateral movement route investigation per AFTER_TURN_PLAN. No movement writes yet. Existing4096 budgets and all native/stale/identity gates remain.


## Lateral-route review completed — one missing original continuation

See analysis/FLAT_WALL_LATERAL_ROUTE_REVIEW.md. Existing authenticated Update
capture (0x1424530..0x14246d6,422 bytes, SHA matches static plan) was privately
verified and decoded89/89 instructions. It is NOT a complete logical function:
four conditional edges go to uncaptured0x14248f9 and terminal fallthrough is absent.
ONE selected original route is Update -> shared0x14248f9 continuation; helper CALLs
are untyped, not certified input/motion APIs. Next bounded acquisition is only
up to128 original bytes at0x14248f9 with current-build/section/runtime metadata,
using existing offline PE range primitives; no new runtime capture command exists.
No motion-write experiment admitted, no repeated input inventory/duration tuning.
Runtime/artifacts remain r4/package0.1.60. Earlier START is deferred backlog.

## Current handoff — repeated reversal lifetime

See analysis/V01C38R3_REPEATED_REVERSAL_RUNTIME.md. Supplied023446 r3 log session2 completes3 reversals and session4 completes2, then both hit5second override expiry. Video unavailable; precise visible-release alignment UNKNOWN. START remains displacement-based and unmodified.
Current CODEX_NEXT now authorizes replacing only mandatory normal5second expiry with authenticated session-bound continuation. This supersedes older preserve-five-second task constraints for that expiry alone; all freshness/emergency/build/identity/native safety gates remain. Earlier START timing work remains blocked separately. r4 lifetime change implemented; validation/publication status below.


## r4 lifetime implementation — validation complete

v0.1c38r4/package0.1.60 removes ONLY normal5s expiry in tick. Existing per-update/
per-write100ms freshness, native identity/current-code/normal and Deinit/disable
close/recovery gates remain; reversal never resets entry or generations. No
5s-only certificate dependency found in current revalidated A/B consumers.
Independent4096-per-path write/attempt budgets still apply (not unlimited writes).
Heading64/actor256/counter64 diagnostics remain independent of source permission.
Actual31s synthetic/Wine A/B run passes straight beyond5s and >=20 completed
reversals beyond30s with original session/generation/entry, plus close/reset gates.
Build/focused12/portable25/smoke7/native11 pass; canonical 901 PASS/5 SKIP/17 UNAVAILABLE.
Owner r4 persistence acceptance is now bounded VERIFIED-RUNTIME per latest owner evidence; prior synthetic validation remains distinct. INPUT inventory completed separately.
See analysis/V01C38R4_SESSION_LIFETIME.md.

## INPUT START follow-on — inventory completed after lifetime publication

Lifetime implementation e63afec169132aaeae36e3b5ec6b68c55dd6ba75 / record head
6af7b784505f06961dc8691341c080a10ca2179b pushed and independently verified before
this separate inventory. See analysis/INPUT_START_ADAPTER_INVENTORY.md.
Best existing directional definition is planar::Input.inputForward/inputRight,
but only test fixtures supply it; disabled Actor adapter is not a runtime producer.
Runtime hotkeys, SDK snapshots and lifecycle tokens do not supply game-accepted
move intent; msm2::input is orientation-source capture, not directional input.
No statically safe input address/API/type found, so no input probe/trigger added.
ONE next discovery target: action-mapped requested move vector consumed by the
original authenticated WallRun_Update before steering, with identity/type/frame/
focus/menu/freshness and pre-displacement ordering proof. No broad scan or API guess.
Runtime remains r4/package0.1.60; earlier START gain still unproven. Independent
4096 write quotas still limit long sessions; no unlimited-store claim.

## Current owner correction — 2026-10-31 JST

Supersedes duration-only acceptance/task wording below: owner requests earlier reversal START, not faster turn/animation. Approx1.1s is requested adjustment, not measured latency. r3 video/log supplied; visible lag remains owner-reported. No further duration tuning is authorized. Current CODEX_NEXT requests bounded source review and only necessary existing-value diagnostics; no behavior/timer change.
Supplied r3 log session1 exits override at17:10:57.972Z with bounded_test_window_5seconds, native Deinit at17:11:00.241Z. Source override and native WallRun persistence differ; matching video release to those events remains UNKNOWN. Preserve5s bound until separately reviewed.


## Reversal START review completed — documentation only

See analysis/REVERSAL_START_TIMING_REVIEW.md. Current trigger observes displacement,
then waits two eligible33ms samples. Acceptance sets logical turnStart; first
positive angular sample/publication/actual source commit are later distinct
boundaries. No existing authenticated pre-displacement intent is available;
1.1s cannot be subtracted from an existing reversal delay. No behavior patch or
new diagnostic justified without that missing directional-intent/time identity.
Existing bounded events distinguish candidate/acceptance/completion and source
exit/native Deinit; silent rejection/full frame timing remain unknown.

Five-second source budget starts at Transition.enter(), not reversal; reversal
never resets it. Supplied timeout/Deinit excerpts prove source/native lifetime
can differ, not that reversal causes release or straight WS override is unlimited.
Runtime remains v0.1c38r3/package0.1.59, unchanged and not accepted as the START
solution. No new owner update/game run required by this documentation review.

## Roadmap / current handoff — 2026-10-31 JST

Overall scope and order are reconciled in `docs/FEATURE_ROADMAP.md`.
Steering Probe tooling retirement is completed/published; see analysis/STEERING_PROBE_RETIREMENT.md.
Historical r3 duration experiment is implemented/tested but is not the requested START solution. Current START review is complete; do not re-execute CODEX_TURN_SPEED.md.
R2 supplied runtime now confirms reversals with remaining visible lag; six completed logged candidate-to-completion intervals approximately302–322ms. This supersedes earlier r2-runtime-UNKNOWN notes below; it is not input/animation latency.
After facing acceptance: flat-wall sideways movement proof, then four-way/diagonal movement and stop/contact/lifecycle foundation; camera only afterward. See `docs/AFTER_TURN_PLAN.md` for the staged handoff and proof boundaries. This is planning, not movement-write authorization.
Current five-second bound, C52 and every existing native safety gate remain.


## Current stage — START review complete; earlier directional-intent evidence missing

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

## v0.1c38r3 / package0.1.59 — duration-only experiment

See analysis/V01C38R3_TURN_DURATION.md. Supplied r2 six completed turns confirm
reversal with candidate-to-completion302–322ms; logged acceptance41–54ms and
turn248–270ms are not input latency. Qualitative visible lag remains; no exact
animation/video-clock alignment. Session3 had one25ms stale episode. Expected
five-second stops/verified Deinit/Up retention/Position writes0 remain.

Only post-entry reversal duration is now120ms through one named constant. Two
33ms observations, speed2/dot0.8/max gap250/equal-time WAIT, entry0.25s and all
native A/B/lifecycle/write gates stay unchanged. No new movement/animation/input
API. Canonical hooks 900 PASS /5 SKIP /17 UNAVAILABLE; build/focused/portable/smoke/native/release pass. Actual r3 response UNKNOWN. This experimental120ms is not animation sync or
total input latency. No owner deployment/game-test prose; future roadmap stays
unauthorized pending the separate acceptance/task handoff.

## Steering Probe retirement completed (tooling only)

Known WS Fast10/Slow5 probe stages/ZIPs are identity-checked and reversibly
archived outside Mods Library. Managed sparse policy excludes stages; source
builders/history retained. Unknown/foreign files refuse, unrelated library and
profile choices/order untouched. See analysis/STEERING_PROBE_RETIREMENT.md and
LOCAL_WORKSPACE. Workspace18 groups/environment28 tests/preflight/distribution
checks passed. r2 runtime bytes unchanged. Owner duplicate paths/UI acceptance
remain unverified. Publication recorded in CODEX_RESULT before queued r3 begins.

## v0.1c38r2 / package0.1.58 — correctness and tuning separated

Canonical evidence: `analysis/V01C38R1_REVERSAL_RUNTIME_RESULT_20261031.md`.
Implementation: `analysis/V01C38R2_REVERSAL_LATENCY.md`.
Owner confirms r1 reverses but perceives delay. Logged candidate-to-completion
474/358/366/353ms is not input latency. Update160->161 at the SAME timestamp
reset pending evidence unnecessarily; no lifecycle stale episode explains it.

A: equal-time different updates WAIT, preserving observation and candidate
baseline; no extra votes, events, turn progress or freshness. Genuine backward
wall/native order and >250ms gaps still reset. Correctness passed separately
with original100ms parameters before tuning, then rechecked on final code.
B: one named interval33ms for observation/confirmation; two samples remain
necessary (synthetic acceptance at66ms after baseline0). Speed2, dot0.8,
max250ms gap and fixed250ms turn, entry/A-B/Position-W/metadata/scales/five-second
contracts and diagnostic sampler100ms are unchanged. Actual r2 response: reversal confirmed with residual lag (supplied evidence above).

No source/input/velocity/Position writes or animation/input ownership claim.
Exact automated outcomes/publication in CODEX_RESULT; no cloud deployment.

## Shared checker correction — 2026-10-31

`Overstrike/scripts_proxy.dll` is permitted as a regular non-reparse installer
source; it is distinct from forbidden misplaced `Overstrike/winmm.dll`. Existing
root files remain intact/required. No automatic DLL relocation, new equality
requirement or profile change. Current source contract and owner-evidence limits:
`analysis/SCRIPTS_PROXY_INSTALLER_DEPENDENCY_20261031.md`.

Owner's later supplied log loaded v0.1c38, not proof of r1/Update-WS success.
The old updater checker executes before fetch, so a guarded one-time Git
fast-forward is required to obtain this correction (exact block in CODEX_RESULT).
That checker-only patch preserved v0.1c38r1/package0.1.57 bytes. Real-PC updater
and game acceptance remain unverified.

## Historical r1 orientation-only proof — v0.1c38r1 / package0.1.57

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

Focused math (10 groups), portable (23), Windows build/smoke and owned A/B
fixture checks passed under Wine for the unchanged r1 runtime. The c67 wrapper
now resolves its child script beside its resolved __file__, preserving all
assertions and run_name. Root/source/unrelated-cwd verification all pass14 groups.
The unchanged canonical hooks command now completes successfully; exact counts
are recorded in CODEX_RESULT. No cwd workaround, skipped test or updater bypass.

Runtime/source/version/DLL bytes are unchanged by this runner repair. Actual
owner Windows Update-WS execution and r1 game response remain unverified; existing
Update-WS.cmd remains the deployment path. No local deployment performed.

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

## Historical shared Overstrike mitigation — superseded

The previous launcher moved manager-side winmm, cleared ReadOnly attributes and
removed PID-only locks. Mod-Tools 1.2.0 removes these automatic mutations. It
permits a regular installer Overstrike/scripts_proxy.dll, refuses misplaced
Overstrike/winmm.dll unchanged, and uses process-lifetime OS ownership. Legacy
artifacts are preserved only after verified inactivity; no automatic process kill.

## Safety

Runtime script is `v0.1c38r2` (package0.1.58); only the authorized post-entry
orientation-source reversal behavior changes. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All native publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch gates remain closed.
