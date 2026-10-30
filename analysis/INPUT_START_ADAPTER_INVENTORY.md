# INPUT-triggered START — existing adapter inventory

2026-10-31 JST; branch codex/v01c20-edi-value-provenance.
Executed ONLY AFTER lifetime code e63afec169132aaeae36e3b5ec6b68c55dd6ba75 and
record head6af7b784505f06961dc8691341c080a10ca2179b were pushed and independently
verified (13 changed blobs, fetched head/local/ls-remote equal). Separate task/
commit; lifetime tests and input inventory evidence are not combined certificates.

RESULT: INPUT_INVENTORY_COMPLETE_NO_RUNTIME_REQUESTED_DIRECTION_CANDIDATE.
No trigger adapter or diagnostic added; runtime stays r4/package0.1.60.
This is a concrete adapter/definition/caller inventory, not another review of
the old displacement detector. Desired~1s improvement is not measured latency.

## One best existing definition — NOT a proven runtime read candidate

`source/include/planar_mvp.hpp`: ws::planar::Input::inputForward/inputRight,
float members consumed by Controller::update. Intended coordinates are the
controller's saved wall-tangent heading/right: desired=right*inputRight+
heading*inputForward, individually clamped[-1,1], normalized if magnitude>1.
Thus inputForward is the closest existing abstraction for requested longitudinal
sign. But it has NO authenticated game address/offset/API, device mapping, reader,
producer, or proof of game-request acceptance/order. It is an abstract value
injected into unit fixtures, not a field discovered in a game object.

Actual callers: source/src/planar_mvp_tests.cpp constructs Input via input() and
passes literals to Controller::update (e.g. inputForward=1 for movement-only test).
`source/src/msm2/actor_writer.cpp` includes the planar types through its header,
but hard-disables the legacy adapter: configure/admit/dispatch do not instantiate
or feed this controller; active() returns false. Header inclusion is not a runtime
caller. Source-wide caller search finds no production Input population/Controller
instance. No permission to reactivate that disabled direct Actor path.

Freshness/session/focus/menu: abstract Input carries session/Hero/Transform/owner,
now/dt/lifecycle, but their runtime producer is absent. They cannot establish
input freshness. Focus/menu/deadzone/remapping/camera-relative/controller semantics
are not defined by those members; they must not be assumed to inherit unrelated
source freshness or game acceptance. There is no bounded safe runtime read to
instrument here, so no candidate diagnostic is prepared from this abstraction.

## Exact alternatives searched / why they are not substitutes

| Existing path | Actual definition/callers | Evidence and missing boundary |
| --- | --- | --- |
| source/include/wallsprint.hpp; source/src/core_tests.cpp | Sample.inputRight/inputForward -> ws::Controller::tick, mocked Host in core tests | Another pure abstract controller; no game adapter. Flags combat/cutscene/etc are fixture-supplied, not authenticated game/menu input. |
| source/src/msm2/actor_writer.cpp; source/include/msm2/actor_writer.hpp | Direct-Actor runtime adapter stubs; dispatch disabled | No runtime input producer; enabling it would bypass the current architecture/gates. |
| source/src/msm2/telemetry.cpp:64–65,102–104,130–145 | foreground PID + GetAsyncKeyState for F8/F9/F10 and digit markers | OS hotkeys only. No movement keys, action bindings, menu acceptance or camera/controller mapping. Focus filter alone does not establish menu acceptance. No WASD hardcoding introduced. |
| source/include/msm2/sdk_bridge.hpp; source/src/msm2/sdk_bridge.cpp | bounded Snapshot through existing HeroSystem/GetHero/Transform read | Position/yaw/axes only; no game directional input. GetActor/GetHeroSystem definitions are not requested-move APIs. |
| source/vendor/SM2ScriptTemplate/game/{Actor,HeroSystem,Transform,Native}.h and Component/ | Existing actor/transform/generic component definitions | No current-build directional-input type/field/accessor. Generic GetComponent declaration does not establish a safe component name/layout or authorize traversal/API calls. |
| source/include/NativeHooks/source_lifecycle.hpp; source/src/NativeHooks/source_lifecycle.cpp | Token -> observer: owner/session generation/update/thread/timestamps | Typed lifecycle carries no direction. Fresh source ownership is not fresh input. |
| source/src/msm2/{wallrun_hooks,hook_capture,update_bridge}.cpp and hook_thunks.S | Existing verified lifecycle/capture/update forwarding, context identity | Opaque captured arguments are not typed directional fields; no requested-move structure or safe input buffer definition. No added hooks or inferred ABI. |
| source/include/msm2/orientation_source.hpp; source/src/msm2/orientation_source.cpp | misleading namespace msm2::input, actual authenticated orientation-source hook/capture | “input” here means writer source arguments, not keyboard/game-accepted directional intent. Matrices can contain prior WS output; not independent intent. |
| source/include/Math/reversal_heading.hpp and native_source_override.cpp | displacement observer using position updates | Existing effect-following data, not a new earlier requested-direction channel. |

No XInput/gamepad adapter, input-action binding map or requested-direction read
exists in the searched production/SDK paths. Device/input method is unknown; no
keyboard-only assumption. For any future adapter, release/conflict/focus/menu/
stale/session-loss behavior and held-vs-new sign must be specified before enabling
trigger. These are requirements, NOT tests passed by the current abstraction.

## Relevant reference evidence (read-only)

Wisdoms docs/WALLSPRINT_REFERENCE_MODS.md higher-level movement labels and
ENGINE_INTERNALS.md input-method uncertainty, REVERSE_ENGINEERING_GUIDE.md ABI/
correlation cautions reviewed atafb1513c80edff9bf11f19b7a36baf3894bb6429.
Local analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md: reference Scripted Flight
labels UpdateInput/UpdateSteering/inputX/inputY exist, but provide NO current-build
address, ABI, coordinate system or runtime consumer proof. docs/AFTER_TURN_PLAN.md
already treats these as hints. No reference binary execution, copying or broad
scan performed, no cross-project/SF mutation.

## ONE tightly scoped next discovery target (not executed)

Identify the game-action-mapped requested movement vector/sign consumed by the
ORIGINAL, already authenticated WallRun_Update route for this exact build,
immediately before steering/motion submission. Anchor to the existing verified
Update owner/lifecycle in source/src/msm2/wallrun_hooks.cpp and sdk_bridge.cpp's
existing lifecycle resolver; do NOT assume reference UpdateInput label is that
function or turn raw registers into typed arguments.

Required static deliverable for this ONE route: an exact current-build consumer
read/accessor plus proven base identity/lifetime, full field extent/type, mapping
from stored coordinates to independently defined wall-relative longitudinal sign,
and why access is safe without unknown calls/hooks. Then a separate authorized
bounded read-only probe may pair candidate value/sign, position delta/sign,
session/generation/native update/monotonic time, read-valid/fresh/focus/menu state;
check keyboard remapping/controller/camera-relative behavior and ordering BEFORE
displacement. Missing fields are unavailable, never default “allowed”.

Currently NO address/type/consumer read is established, so preparing a memory
probe or calling a new game API would invent the prerequisite. No such probe,
trigger or further discovery is executed in this bounded task. This exact next
route, not a repeated raw displacement capture or1s offset, is the handoff.

## Validation / limits

Exact inventory queries are recorded in CODEX_RESULT. Runtime and all artifacts
byte-identical to published lifetime head6af7b78; no new version/log/package.
Input-specific trigger/held/conflict/focus/menu tests are NOT RUN: no adapter was
implemented or enabled. Synthetic planar/controller tests from the lifetime
portable run do not prove game input or a1s gain. No owner game/update instructions.
