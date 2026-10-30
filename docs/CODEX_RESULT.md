# Codex result — r4 session-bound source lifetime

2026-10-31 JST. STATUS: partial overall — lifetime patch validated/published; INPUT inventory complete, trigger lacks a statically established runtime candidate.
Branch codex/v01c20-edi-value-provenance; baseline18c563a. No main merge.
Runtime v0.1c38r4/package0.1.60. Follow-on INPUT task starts AFTER this push.

## Scope / evidence / safety

See analysis/V01C38R4_SESSION_LIFETIME.md and supplied r3 repeated-reversal
runtime analysis. Session2 three/session4 two completed reversals, then5s expiry;
no stale in those sessions, session1 separately10 stale pairs/max1385ms. Missing
video means no visual-release clock claim. This patch addresses total duration,
not earlier START or animation speed. Private raw evidence is not redistributed.

Changed native_source_override.cpp: remove normal now-entryMs>=5000 end only;
cap emitted actor observations256 and counter snapshots64 per session. Retention
validation continues past log caps. Existing heading64/movement40/plane64/stale16,
fixed ring1024 unchanged. No admission/reversal math/source target/store changes.
State reset only on genuine new session; no reversal entry-time/generation renewal.
Transition/helper/ownership/source-lifecycle code unchanged. Existing100ms native/
worker freshness, stale suspend/recovery, invalid owner/normal/code/Deinit/disable/
F8/HardClosed guards and A/B stack/caller/target checks remain. Reviewed per-update
and per-store checks have no5s certificate dependency. C52 remains FAIL, no new
watch/hook/input/animation/Position/movement writes. Shared tooling/SF untouched.

Independent4096-per-path write/attempt safety budgets are PRESERVED. Removing
total TIME expiry does not authorize unlimited stores or quota reset. These can
remain a practical long-session limit; changing them needs separate review.
Do not silently call that quota diagnostic-only. No in-game acceptance claimed.

Also changed native smoke fixture, version.hpp/info.json/README_TEST, package
manifest metadata, PROJECT_STATE/result and new analysis. Artifacts:
MSM2-WS-v0.1c38r4.zip, MSM2-WS-v0.1c38r4-source.zip,
MSM2-WS-v0.1c38r4.script.disabled, BUILD_MANIFEST.json. Prior r3 artifacts retained.

## Exact validation / expected / actual

Repository root with `export PATH=/workspace/toolchains/analysis-python/bin:$PATH`.
PASS labels/groups are not summed unique assertions; focused/native suites repeat
within portable/canonical commands.

| Command | Expected | Actual |
| --- | --- | --- |
| `bash source/tools/test_reversal_heading.sh` | same reversal timing/duration and12 focused groups | exit0; 12 PASS / 0 SKIP / 0 UNAVAILABLE; 0.76s |
| `bash source/tools/build_windows.sh` | Windows build/version checks | exit0; 0 PASS / 0 SKIP / 0 UNAVAILABLE; 38.74s |
| `bash source/tools/test.sh` | portable regressions including unchanged entry/reversal | exit0; 25 PASS / 0 SKIP / 0 UNAVAILABLE; 7.83s |
| `bash source/tools/smoke_windows.sh` | Windows/Wine smoke invariants | exit0; 7 PASS / 0 SKIP / 0 UNAVAILABLE; 11.79s |
| `bash source/tools/test_native_source.sh` | actual31.1s same-session owned A/B run with straight>5s, >=20 completed alternating turns, active>30s; caps/Deinit/closed-generation/new-session/disable plus existing stale/identity/code cases | exit0; 11 PASS / 0 SKIP / 0 UNAVAILABLE; 98.45s |
| `bash source/tools/test_hooks_windows.sh` | full canonical chain/counterexamples, no skip/bypass of prior native gates | exit0; 901 PASS / 5 SKIP / 17 UNAVAILABLE; 381.68s |
| `python source/tools/release.py` | version/script/export/import/package validation and both CRCs | exit0; 1 PASS / 0 SKIP / 0 UNAVAILABLE; 1.61s |

Long run uses actual wall clock with synthetic owned lifecycle events and real
MinHook fixture A/B source commits. No backdated source entry/per-reversal reset;
GetTickCount64 advances normally on16ms cadence. Original source entry/session/
generation asserted each iteration; path budgets unexhausted. This is NOT native
owner Windows or actual game proof. Existing real-position-watch regression is
retained, not a newly enabled game watch. No safety assertions removed.

`git diff --exit-code 18c563a -- source/include/Math/reversal_heading.hpp
source/include/Math/source_orientation.hpp source/include/NativeHooks/ownership_guards.hpp
source/src/NativeHooks/source_lifecycle.cpp source/tools/workspace.py environment`
returned0/empty: unchanged independent guards/heading/entry/shared code.
`git diff --check`, staged equivalent and final ZIP CRC/source member identity
checks pass. Packaging repeated only after docs update; binary tests not repeated
for report-only package changes. Nonfatal Wine winedevice warnings do not replace
exit-code checks. All required commands executed; no previous run counted as new.

## SKIP / UNAVAILABLE / owner / publication

Canonical5 optional private replay datasets SKIP (not redistributed); independent
synthetic wrong-hash/decoder/native regressions still execute.17 UNAVAILABLE:
c40 VCH2, c49 bridge6, c50 native identity7, c51 post-return2. Registration never
stands in for native dispatch/order evidence; counterexamples/refusals preserved.
BLOCKED0 for bounded lifetime change. Owner r4 game acceptance additionally
UNAVAILABLE (no owner PC/game run here). No human update/game procedure written.
No full-log request or new capture. No active install/deploy.

Read required AGENTS/manuals/runtime analyses and shared Wisdoms references;
local policy change is not an engine ownership fact. No other repository changed.
Publication commit and independent GitHub head/blob equality recorded after push;
only then continue docs/CODEX_INPUT_START.md separately. Five old untracked legacy
artifacts remain unstaged. Final exact runtime log name if owner later runs:
MSM2-WS-v0.1c38r4-YYYYMMDD-HHMMSS[-NN].log under MSM2-WS/logs (existing naming).

### Verified lifetime publication (before INPUT work)

Implementation e63afec169132aaeae36e3b5ec6b68c55dd6ba75, normal push18c563a->e63afec.
`python3 /tmp/ws-r4-publication.py` exit0: independent GitHub fetch into
/tmp/ws-steering-docs-publication, FETCH_HEAD/ls-remote/local head equal; all13
changed file blobs from18c563a equal local bytes. `python3 /tmp/ws-r4-final-audit.py`
PASS: protected helpers/guards/lifecycle/shared tooling unchanged, None time cap,
4096 write/100ms freshness metadata, script/DLL hash, both ZIP CRCs and runtime/
native-fixture source equality. Exact existing script log naming unchanged.
This record-only package refresh retains script/DLL bytes; no binary test rerun
needed for documentation. Final lifetime record head is preserved in subsequent
INPUT inventory result; separately verify final pushed head before starting it.

## Separately executed follow-on — INPUT START inventory

Lifetime final record head6af7b784505f06961dc8691341c080a10ca2179b independently
verified before inventory began (`python3 /tmp/ws-r4-publication.py`:13 blobs/head
match). New docs-only follow-on commit: analysis/INPUT_START_ADAPTER_INVENTORY.md,
PROJECT_STATE and this appended result; no r4 runtime/artifact change.

RESULT: INPUT_INVENTORY_COMPLETE_NO_RUNTIME_REQUESTED_DIRECTION_CANDIDATE.
The concrete closest definition is planar::Input.inputForward/inputRight; its
producer/callers are test fixtures only. Runtime Actor adapter is hard-disabled.
Actual OS key polling is debug hotkeys, SDK Snapshot is position/yaw/axes, Token
is lifecycle. Namespace msm2::input is orientation-source capture, not move intent.
No XInput/gamepad/action-map or game directional type/reader exists in searched
production/SDK paths. Keyboard remapping/controller/camera-relative/focus/menu
semantics therefore UNKNOWN, not silently mapped to WASD or overwritten axes.

Exact focused inventory commands executed (repository root):

```bash
rg --files source | rg -i 'input|controller|gamepad|key|sdk'
rg -n 'Controller|GetAsyncKeyState|XInput|GetKeyState|moveX|moveY|requestedDirection|MoveDirection|UpdateInput|InputState|GameInput|SetInput' source/include source/src source/vendor --glob '!**/capstone/**' --glob '!**/minhook/**'
rg -n 'inputRight|inputForward|controller|planar::Input' source/src/msm2/actor_writer.cpp source/include/msm2/actor_writer.hpp
rg -n 'ws::Controller|planar::Controller|\bController\s+[a-zA-Z]|planar::Input' source/src source/include/msm2 --glob '!**/vendor/**'
rg -n -i 'input|controller|gamepad|keyboard|button|key' source/vendor/SM2ScriptTemplate source/include/msm2 source/include/NativeHooks/source_lifecycle.hpp
```

Expected/actual: definitions in pure helpers, fixture callers and hotkeys only;
manual definition/caller review confirms no runtime directional reader. `rg`
no-match exit1 is absence evidence, not a runtime test failure. Read actual
Actor/Native/Component/SDK/lifecycle/capture/bridge and Wisdoms reference sections;
no new binary scan/hook or unknown API call. Analysis inventories exact paths,
coordinates/conditions and statuses; no positive game-input proof from names.

ONE next target: current-build action-mapped requested vector consumed by original
verified WallRun_Update BEFORE steering. Need exact typed consumer access/base/
lifetime/coordinate mapping BEFORE preparing bounded read-only pair logging.
No such address/type is established, so no safe candidate diagnostic/adapter was
implemented. This completes the explicitly allowed no-candidate branch, not just
restating displacement limitations or repeating the prior START review.

Follow-on verification: `git diff --check` and `git diff --exit-code 6af7b78 --
source environment BUILD_MANIFEST.json MSM2-WS-v0.1c38r4.zip
MSM2-WS-v0.1c38r4-source.zip MSM2-WS-v0.1c38r4.script.disabled` both exit0/PASS.
Source/artifact unchanged, so no recompile/repackage or repeat of the seven
lifetime commands for docs-only inventory (7 SKIP in THIS follow-on phase only).
Input adapter tests (old/zero displacement, held one-turn, repeated signs,
release/conflict/focus/menu/identity/stale) NOT RUN because no adapter exists.
UNAVAILABLE: actual candidate-input runtime ordering/semantics. Trigger
implementation BLOCKED by absent static address/type/producer proof, not a safety
policy denial. Lifetime901 PASS/5 SKIP/17 UNAVAILABLE above remain separate;
no synthetic input test promoted to native or owner-game acceptance.

No new output log/version or owner update/game instruction; r4 existing log name
above. Normal docs-only commit/push and independent changed-blob/head readback
recorded below; runtime lifetime patch remains independently reproducible.


### Verified separate INPUT publication

Inventory commit53bf93f0b7a34f04a2e7543f760bbce4169e26a5, normal push after
lifetime6af7b78. `python3 /tmp/ws-input-publication.py` exit0: independent GitHub
fetch/head/ls-remote equality and all3 changed blobs against6af7b78 match local.
Exact final caller query was also rerun into /tmp/ws-input-callers-final.log
(single regex word-boundary escapes), same fixture-only callers. Documentation
query spelling normalized; runtime/artifacts unchanged. This record-only commit
is normally pushed and independently verified again; final full SHA in chat.
No rebuild/repackage for docs-only follow-on, no owner deployment/test request.
