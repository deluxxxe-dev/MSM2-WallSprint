# Downward locomotion and independent directional intent — implementation design

2026-10-31 JST. Baseline `02db844c4b472f9fc2250ad6eb85df1b45e9b548`,
branch `codex/v01c20-edi-value-provenance`. **Design complete; behavior implementation
BLOCKED on provenance. No runtime change or claim that descent is fixed.**

## Accepted evidence and causal limits

The accepted [r7 owner comparison](V01C38R7_OWNER_COMPARISON_20261031.md) supersedes
older comparison-pending statements. ON retained89 packets/3 sessions show source
admission/publication; OFF121 packets/4 sessions show zero active publications,
A/B commits and source admission. Owner reports ordinary WR in OFF and failed
descent in BOTH. Thus disabling WS orientation-source substitution does not cure
this failure. It does not exclude an additional WS effect on facing/oscillation;
OFF is not an all-other-mods-isolated vanilla control.

Retained tangent-world slopes repeatedly include -12degrees and none below -20.
These are capped retained samples, not exhaustive trajectory extrema or precisely
input-timestamp-aligned observations. `TurnMaxDown=12` in vanilla Standard is an
**OBSERVED-CORRELATION**, not a native consumer/units/current-resource proof.
R7's OFF observer projects displacement without the heading turn deadband; its
-12 readings are not manufactured by the separate WS12degree heading deadband.
No further ON/OFF/video, Fast/Slow parameter or stopped scalar capture is needed.

## Current runtime path, not unused controller abstractions

| Actual source touchpoint | Input and action | What it cannot establish |
| --- | --- | --- |
| `source/src/msm2/sdk_bridge.cpp::sampleCallback` and `source/include/msm2/sdk_bridge.hpp::Snapshot` | Existing HeroSystem/GetHero/GetTransform; guarded snapshot of position, raw12float transform prefix and yaw, hero/transform identities | No mapped intent, physical velocity or final rendered body pose |
| `source/src/NativeHooks/native_source_override.cpp::tick` | Validates native lifecycle/context, samples Hero, reads existing owner+0x398 normal under current bounds/normal checks | Normal is surface geometry, not desired input; native token is directionless |
| Same file, `state.transition` entry | Actor-derived axes projected against surface normal; existing entry interpolation | Actor Forward/Up are observations potentially influenced by prior WS output, not independent intent |
| Same file, `state.heading.sample` (currently line523) -> `source/include/Math/reversal_heading.hpp::sample` | Distinct native-update position differences -> tangent projection -> speed -> unit direction -> candidate/turn | Finite difference is observed travel, not actual native velocity or a request for movement |
| Same file, publication and `writeStackOrientation`/`writeStackCompact` | Existing A48byte/B36byte orientation source substitution under final gates | No movement submission, Position/gravity/animation modification or guarantee of visible skeletal pose |
| Same file, `comparisonSample` | Requested/published heading and Actor axes, ON/OFF provenance | “requested_heading” is displacement-derived target, not user input; coherence is worker snapshot, not atomic native frame |

The existing heading observer waits for travel, minimum tangent speed2, distinct
samples and stable direction. If native movement stays horizontal, projecting
that movement onto the wall cannot create a negative-world-Up request. Earlier
facing is also limited by waiting for movement to change. Valid velocity alone
has the same causal limitation for pre-movement reversal.

`planar_mvp.hpp` inputForward/inputRight and the abstract controller in
`wallsprint.hpp` are fixture/helper inputs, not game producers. `msm2::input`
orientation hook arguments are not user input. Telemetry's foreground
GetAsyncKeyState markers/hotkeys are not remapped movement, controller input or
menu validity. The actor-writer adapter remains disabled. Existing authenticated
scalar callsites do not prove an input/velocity API or steering-config consumer;
the scalar route remains stopped.

## Native downward restriction: what accessible sources prove

See [vanilla schema proof](VANILLA_WALLRUN_STEERING_CONFIG_20261029.md) and
[resource graph review](REFERENCE_MOD_WALLRUN_REVIEW_20261029.md).

* **VERIFIED-STATIC:** build2.810 vanilla TOC SHA1
  `D18940B7DED9CF83B6E7AB57BBD0FA43F5FD5290`; authenticated Standard asset
  `948FA063808F7E18`,544bytes, has UINT8 TurnMaxDown12, Fast100 and Slow75.
  Indoor asset `83DF4507531061B1`,428bytes, lacks TurnMaxDown. Absence is not proof
  of zero/unlimited behavior, inheritance or current state selection.
* **VERIFIED-STATIC:** reference traversal WallRunConfig names Standard/Indoor
  steering resources. A reference resource edge does not establish the currently
  selected game resource or the instruction consuming a field.
* **VERIFIED-STATIC:** accessible Overstrike source, upstream
  `https://github.com/Tkachov/Overstrike.git` commit
  `443ec468b5a7a922771dce6bfa96396f56896dcb`,
  `DAT1/Files/Config.cs`, `Sections/Config/ConfigBuiltSection.cs`,
  `ConfigReferencesSection.cs`, `Sections/Generic/SerializedSection_I30.cs`
  describe Config_I30/type35F7AFA5, BuiltE501186F, references and typed serialized
  node decoding (ReadByte/ReadSByte/ReadSingle). They prove file interpretation,
  not runtime struct layout, units, selection, clamps or native ABI. Searches
  for TurnMaxDown/SteeringSetupStandard/SteeringSetupIndoor found no consumer in
  those accessible C#/JSON/XML sources or current production src/include/vendor.
* **UNKNOWN:** actual selected Standard/Indoor/default resource in each observed
  WR state, loaded value, conversion to angle, sign/frame, consumer and clamps.
  Prior Slow5 installed-payload verification only proves installed bytes; it
  does not prove load, selection or use. File offsets are not native offsets.

A world-space straight-down wall tangent geometrically has slope -90degrees;
that observation does not establish the serialized field's unit or authorize
writing90. Reusing WSU88 or arbitrary tuning would not close the consumer proof.
No config experiment or live inspection was performed in this task.

## WSU lead and actual availability

[Accepted supplied WSU analysis](WSU_REFERENCE_REVIEW_20261031.md) records
SwingCore SHA256 `c8657d0fdbec3afb49fe6873ba736696cc9035eb9dd8aa1737867fc272b5a1e9`
and wallslide `74846e789ee0480de67da20a3d760c63394a73d06448a4848a478994c10907e1`.
SwingCore DLL-relative0x36770 prefers context+0x98/9C/A0 velocityPure; only its
invalid/small-vector fallback at0x36878 uses context+0x8C/90/94 inputDirection.
This is not evidence that valid input overrides old velocity, or that the named
input is remapped, fresh or world-space. Its producer/caller and ABI are missing.
DLL-relative addresses are not game RVAs.

Wallslide is a runtime state controller plus assets: worker/state caches and
unproven +F8/+114/+1E4/+258/+25C fields, protection changes and save/restore.
Neither these fields nor its clips/code belong in WS. Its Standard12->88 and
Fast/Slow changes are not a config-only descent proof.

This task searched available /workspace and /tmp names and read ZIP directories
of102 bounded archives (<=512MiB, <=16384 members). No SwingCore.dll,
wallslide.dll or sc_core.script member was found. This limited inventory cannot
exclude opaque renamed/deeper nested archives or attachments in another chat;
no such reference binary is presently identified here. **No new WSU disassembly,
producer trace, DLL execution or game binary analysis was performed.** Existing
report conclusions remain supplied static evidence; independent source/design
work above is complete regardless of reference availability.

## Original goal and three-route architecture comparison

The task expanded during publication at remote53655ba. Read
[recovered initial-goal review](INITIAL_WALL_AS_GROUND_REVIEW_20261031.md) and
`docs/FEATURE_ROADMAP.md` (Final intent, movement foundation and contact sections).
The Oct22 goal was already wall-as-ground normal Sprint. Historical posture
writes being overwritten in10–20ms do NOT test transplantation of ground Sprint
locomotion, arbitrary gravity or wall-relative ground admission. This route is
**untested/UNKNOWN**, not rejected. Recovered chat summaries are historical
statements, not new binary/controller proof.

| Route | Available positive evidence | Exact missing proof / cost | Decision |
| --- | --- | --- | --- |
| A. Native ground Sprint with wall-relative support/up/gravity | Closest to original goal; existing validated wall normal/SurfaceFrame and orientation lifetime are reusable. Ground-style posture is a desired presentation, not proof of controller reuse | No authenticated current ground-controller admission/contact/step/slope/gravity consumers or ABI; no evidence they accept arbitrary Up rather than world-Y. Need consistent support query and acceleration frame plus native state-entry/exit ownership, not merely rotate Actor Up | Preferred architecture to investigate FIRST, conditional on discriminator below; feasibility UNKNOWN |
| B. WS-owned tangent movement request through native collision/contact | Existing surface frame supplies geometry; shared reference Scripted Flight strings name RequestSimpleMotion, GravityOrientationOwner/SetGravityVector and mover gravity chain | Strings are search leads only; current-build request ABI, collision sweep/support handling, allowed state, threading, gravity ownership/restore and contact persistence unproven. Flight is not ground Sprint | Reserve alternative if A proves world-up-bound and B independently proves native collision/contact semantics; not currently implementable |
| C. Modify WR steering constraints | Only route with current WR lifecycle and authenticated serialized Standard field; r7 confirms existing WR lateral/upper behavior | Downward consumer/units/resource selection UNKNOWN; WR state/animation may still forbid target despite angle change; no proof parameter remedy delivers wall-as-ground Sprint | Not selected just because r7 uses WR; retain as possible bounded remedy only if A/B evidence later favors it |

**Preferred route: A, evidence-gated native ground Sprint reuse**, rather than
selecting C prematurely. This is a research priority based on the recovered goal
and absence of a ground-Sprint test, not a feasibility PASS or permission to
switch states. B requires more WS controller/contact policy; C has the smallest
known serialized lever but does not establish the desired controller semantics.
No available evidence demonstrates arbitrary-up ground Sprint or rules it out.

Exact current-source boundary: `sdk_bridge.cpp::sampleCallback` and vendor
`game/Transform.h` expose transform observation/access, not controller gravity.
Vendor `game/Actor.h` supplies generic component lookup/addition and Transform,
not a typed ground-controller/support ABI; casting a component would not prove
it. Existing `native_source_override.cpp::tick` validates a WR owner and normal,
not ground-state owner. `Math/reversal_heading.hpp` constructs a basis, not
contact queries. `planar_mvp.hpp`, core test fixtures and surface dryrun diagnostics
are not native ground/collision implementations. Shared
`MSM2-Wisdoms/docs/WALLSPRINT_REFERENCE_MODS.md`, “Higher-level movement architecture
hint from Scripted Flight”, has reference labels only, not source/control flow
for a current ground-state consumer. Overstrike's decoder supplies no such
controller semantics. Consequently support query direction, grounded acceptance,
gravity vector, slope tests and native contact retention are all UNKNOWN.

Animation reuse and controller reuse are independent: a Sprint clip could change
presentation while WR still controls motion; real ground-controller reuse could
still need separate animation-state compatibility. Neither is solved by Actor
Up. Do not import WSU clips, gravity writes or controller offsets.

Preserve existing surface geometry, guarded Hero identity and orientation
restore/publication work without resetting working code. WR token lifetime cannot
be stretched into a ground-state certificate: if a future A transition exits WR,
current Deinit/lease must close normally, with separately proven ground owner,
state entry, support lifetime and bounded restoration. Restore actual native
settings/state only if ownership, initial values and exit behavior are proved;
no automatic reuse of dead WR leases. B likewise requires its own collision and
request ownership. These are design requirements, not new production operations.

## Independent intent work retained across all three routes

A, B and C each require authenticated mapped intent to face before travel. The
completed adapter/lifecycle design below is preserved. For A it would feed the
proven native ground controller and facing; for B a proven native motion request;
for C the native WR steering path. None can be bound without actual ABI proof.
Do not implement a down-facing target before the selected route can sustain down
motion. The earlier conditional WR-capability policy below is C-specific, not
the selected architecture. No numerical tuning or motion injection is proposed.

### Future source touchpoints and contracts (proposals, not implementations)

| Touchpoint | Minimal future responsibility | Admission / failure behavior |
| --- | --- | --- |
| New WS-owned GameInterop intent adapter, called from existing native-source `tick` after validated Hero/normal/lifecycle snapshot and before heading selection | Return optional typed sample with mapped movement vector, coordinate frame, producer sequence/time, hero/owner/transform/generation and neutral/valid/unavailable state | Exact build/function/field identity, type/width/alignment, calling convention if an API, producer-thread/lifetime and bounded readable ownership must be proven. No guessed getter/offset. Recheck lifecycle before publication. Native update number alone cannot prove producer freshness |
| Explicit intent entry point beside `reversal_heading.hpp::sample`, used only by native-source worker | Convert proven input into wall-tangent target; give valid intent priority over old motion for facing | Verify whether input is world/camera/actor-local first; authenticate any frame transform. Project `intentWorld - normal*dot(intentWorld,normal)` and normalize only above proven producer deadzone. Invalid/near-normal projection has no target. Never apply speed2 to normalized intent |
| Existing heading lifecycle and publication | Preserve entry behavior and stable repeated reversal, use admitted target only after native motion capability is proven for requested direction | Keep0.25s entry,33ms cadence, two distinct observations,120ms turn,12degree deadband/8degree stability,250ms observation-gap checks, no mid-turn retarget and candidate rebase after completion. Distinguish fresh held input from stale sample; neutral/release, menu/focus, producer loss, owner/session change invalidate intent candidate. No queued stale turn after reinput |
| Future native steering policy, only after separately reviewed consumer proof | Smallest independently authored change to the proven active downward envelope, if that is the actual limiting consumer | Require current EXE identity, selected asset/state/hash, decoded field mapping, units/sign/range and actual clamp use. File UINT8/offset213 is not a memory ABI. No value selected now; no Standard/Indoor inheritance assumptions. If evidence disproves config route, do not improvise a motion API |
| Existing sdk bridge / telemetry | Keep existing snapshot semantics; future admitted intent provenance separate from Actor axes and displacement | No raw WASD/XInput workaround; no independent menu/remapping guesses. Label requested target vs measured travel vs Actor basis vs final pose explicitly |
| Existing publication and A/B hooks | Consume admitted orientation as today | Preserve native token/epoch/thread/identity, stale100ms, ownership, Deinit, final-store adjacency,4096 and C52. No new writes, relaxed restoration/continuation or slot watch |

Until both proof gates pass, leave current successful lateral/upper behavior as
it is; do not publish a speculative face-down target while movement remains
horizontal. Unavailable new intent must not convert the working old path into a
global failure, nor let a stale intent candidate enter that fallback. Held
input must have a proven current producer contract, not synthetic freshness
from polling the same vector. World Up projected into the wall defines vertical;
an actor-local axis potentially changed by WS cannot independently define it.

Actual native velocity, displacement, orientation source, final rendered body
pose and animation are separate quantities. Native movement and animation may
still resist downward travel even with an allowed angle. No pose/animation
repair, reverse playback, camera/gravity write or new movement submission is
part of this preferred design. If consumer/capability proof fails, implementation
stays blocked instead of treating source rotation as locomotion.

### Future validation contract

Before behavior implementation: pure adapter tests for mapped devices/remapping,
coordinate transforms, neutral vs unavailable, deadzone/projection degeneracy,
producer sequence freshness, owner/generation/thread changes and menu loss;
heading repeated-reversal/release/reinput tests; native ABI/consumer negative
identity tests once an actual binding exists. Preserve all current native
admission and C52 regressions. These proposed tests are not executed tests.

A later authorized behavior patch must state its **actual new version and exact
versioned log**; neither exists in this docs-only task. Owner workflow then:
Update-WS -> Install mods -> restore correct ON/default if an old r7 OFF selection
persists -> Run modded game (restoration with game/manager closed, then reopen;
Install mods again may reset selection). Confirm loaded config/version in log.
Then test:

1. Lateral left/right and repeated reversal without releasing WS.
2. Horizontal WS -> down held3seconds: sustained downward travel/facing, no
   periodic horizontal flips.
3. Lower-right/lower-left held3seconds: visible diagonal down travel/stable facing.
4. Upper/upper-diagonal regressions and RT-held stick-neutral/reinput.

Installation content, loaded/selected/consumed native configuration, mapped intent,
actual motion and visible pose each need their own evidence; none substitutes
for the others. No unchanged r7 game run, update or test is requested now.

## ONE first discriminating verification — proposed, not initiated

Perform a bounded **current-build native ground-Sprint support-frame consumer
review** before choosing a WR-constraint remedy: authenticate the ground-Sprint
state/controller entry identity and follow its grounded-admission/support-query
Up direction and gravity/slope use through the first consumer. The output must
identify whether these operations derive from a per-owner mutable support/up
frame (and who owns it), or a world-vertical constant/constraint; an unknown
callee/field is an exact blocker, not an arbitrary-up PASS. Require exact EXE/
function identity and source/consumer relationship, not a posture test or a
reference string. No live writes/state switches, binary execution, repeated
r7/OFF capture or resumed scalar CFG. Available sources here contain no
identifiable authenticated ground-Sprint consumer, so this verification remains
proposed rather than a claim of performed disassembly or a request for broad
byte capture.

This ONE verification discriminates A from a possible B/C investigation. If A
supports a coherent arbitrary-up support/gravity contract, it justifies further
bounded ABI/lifetime work; if world-up-bound, it explains why A needs more than
Actor rotation. It still does not prove writable ownership or authorize a
behavior patch. Independent inputDirection producer provenance remains an
explicit prerequisite in the retained intent design; the unavailable SwingCore
context producer is a possible lead, not a second immediate acquisition request.
No owner operation or acquisition is automatically requested by this task.
