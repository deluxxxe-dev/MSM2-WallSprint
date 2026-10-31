# Codex result — r7 diagnostic-only ON/OFF source comparison

2026-10-31 JST; branch codex/v01c20-edi-value-provenance, baseline64056d4.
Task: equivalent safe observations with source override OFF, NOT a down fix.
Scalar successor20 bytes accepted; no more captures/callee traversal requested.

## CHANGED / mode semantics

See analysis/V01C38R7_SOURCE_COMPARISON.md and source copy. Existing INI startup
key is the treatment. F8 stops input/lifecycle/other observations, so not a valid
OFF control. Readiness separated from source admission; OFF arms only existing
lifecycle observation while source enabled0/killed=true/PathB admission false.
One existing Window receives same authenticated token/snapshot/N398/Actor values
in both modes, even before ON source entry. Same100ms/16 samples/2s/16-window caps;
missing/stale/identity/normal/Deinit/emergency reasons bounded and explicit.
OFF never constructs a source basis; target/published headings are unavailable,
Actor Up is independently observed, not set to wall normal. Actual active basis
publication count/A-B commits/admission logged with session/gen/native+monotonic
clocks. Active publication counter excludes empty permission revocations.
ON epoch guard/store adjacency preserved; comparison runs after publication stores.
No new game pointers/hooks/fields/API/write types. R6 heading math/all constants,
entry/native Deinit/stale/A-B/scales/metadata/Position-W/4096/C52 stay unchanged.

New narrow selector reuses workspace layout/process quiet/OS ownership, validates
r7 package+installed DLL and sole config key, changes installed WS INI only,
backs up/restores exact bytes; refuses other edits/foreign files. No archive,
scripts.txt/profile/source/shared loader/environment code edits. Selector is used
after Install mods, game/manager closed, then fresh restart + Run only; Install
mods after selection would reset ON. Actual loaded INI path/mode/admissions logged.

Version v0.1c38r7/package0.1.63, logs MSM2-WS-v0.1c38r7-YYYYMMDD-HHMMSS.log.
Chat compact spec: same wall/character/camera/start, identical reported down/lower-
diagonal holds, separate ON/OFF fresh attempts. Owner report is not raw mapped
intent. Startup/mode/counters/packets must validate before comparison is accepted.
ON source-dependent ancillary traces remain existing behavior; common comparison
packets are not final animation/physics or zero-overhead causal proof.

Exact selector commands/spec and RESTORE in analysis. Actual Windows config path/
selector/loader/ON-OFF directional result UNAVAILABLE pending owner comparison.
No improvement claim. Do not repeat scalar extraction or unchanged r6 game test.

## Files / validation in progress

Native source worker comparison/config readiness/log/counters, shared value-only
Frame and existing native fixture, telemetry config-path line, parser, narrow
selector/tests, portable wiring, version/info/package/release/README/BUILD;
analysis/state/result. Final targeted/full validation and publication follow.
Initial warning-as-error cross-build found misplaced comparisonMissing declaration
in anonymous namespace versus definition in native-source namespace. Moved its
worker-owned implementation to the existing anonymous scope; retained Werror and
assertions, subsequent build/targeted native passed. No native assertion weakened.

## Targeted validation / expected / actual

Counts below are cases/groups, not unique assertions across repeated suites.
`bash source/tools/test_wall_motion_window.sh`: expected old5 contracts plus equal
ON/OFF cadence/caps/suppression; actual6 PASS/0 SKIP/0 UNAVAILABLE, exit0. The new
case initially reused local names from an earlier case and failed compilation;
scoped its locals without weakening asserts, rerun6 PASS.
`python3 source/tools/test_select_wallrun_comparison.py`: expected exact ON/OFF/
RESTORE, CRLF/other keys/source/foreign-file preservation, running process/DLL/
config/backup/extra script refusals; actual4 unittest PASS/0 SKIP/0 UNAVAILABLE.
`bash source/tools/test_native_source.sh`: expected original ABI/lifecycle/native
writes plus OFF real owned snapshots0active-publications/0WS-A-B-commits, vanilla
Up independent of wall normal, stale baseline reset, identity close, Deinit/F8;
actual15 PASS/0 SKIP/0 UNAVAILABLE, exit0. Same command repeats inside final hooks
with final idle-budget case. No real game/Windows acceptance inferred.
`python3 source/tools/test_wall_motion_log.py source/logs/windows_native_source_synthetic.log`:
expected actual owned vectors/projection/late windows/caps/target-unavailable and
zero OFF counters; actual1 PASS/0 SKIP/0 UNAVAILABLE.

First complete external pipeline attempted via `python3 /tmp/ws-r7-full.py`
invoking `bash source/tools/windows/workspace-build.sh /workspace/WallSprint /tmp/ws-r7-full-result`
with fresh generated token, synthetic WSL_DISTRO_NAME=OwnedCloud and pinned-decoder
PATH. Actual exit3 in191.93s,84 PASS/4 SKIP/0 UNAVAILABLE, hooks:line34 receipt.
Unchanged immediate_reset_smoke.cpp line161 expected capture inactive after a
fixed120ms Sleep; assertion failed. That fixture and production reset/restore
engine are byte-unchanged from task baseline and do not link r7 comparison code.
No exact cause established; scheduling/timing is a hypothesis, not proof.
`bash source/tools/test_reset_trace.sh` rerun unchanged:12 PASS/1 optional-private-
replay SKIP/0 UNAVAILABLE, exit0. No Sleep/assert/deadline/production code modified.
Full pipeline then retried; final result below, not hidden by completion check.

Final diagnostic-only refinement prevents OFF's pre-WallRun idle loop from using
all64 rejection log slots. Native fixture adds80 idle polls with0budget consumed.
No source/permission/math changes in that refinement. Final build/native/hooks
repeated after it; earlier pipeline artifacts are not used as final release.

## Final standard validation / results

`python3 /tmp/ws-r7-retry.py` invoked full external
`bash source/tools/windows/workspace-build.sh /workspace/WallSprint /tmp/ws-r7-retry-result`
with a fresh private operation token, synthetic OwnedCloud distro label and
PATH=/workspace/toolchains/analysis-python/bin:$PATH. Exit0 in475.14s;
952 emitted PASS/32 SKIP/17 UNAVAILABLE; completed0 stage_payload receipt.
This validates the entire pipeline but precedes the final idle-rejection-budget
refinement; these staged artifacts are NOT published as final runtime bytes.

Final source subsequently executed with unchanged original assertions:
| Exact command | Expected | Actual |
|---|---|---|
| `bash source/tools/build_windows.sh` | warnings-as-errors final DLL/PE build | exit0; native object/link newer than final source |
| `bash source/tools/test.sh` | original portable + shared ON/OFF cadence/caps + selector | 36 emitted PASS +4 unittest cases PASS /0 SKIP /0 UNAVAILABLE; exit0 |
| `bash source/tools/smoke_windows.sh` | Windows/Wine enable/disable/log and exports | 7 PASS /0 SKIP /0 UNAVAILABLE; exit0 in12.48s |
| `PATH=/workspace/toolchains/analysis-python/bin:$PATH bash source/tools/test_hooks_windows.sh` | all required old native/regressions + OFF zero-permission/lifetime/idle case + payload checker | 907 PASS /5 SKIP /17 UNAVAILABLE; exit0 in392.14s |

Final smoke/hooks controller `python3 /tmp/ws-r7-final-tests.py` serially invokes
those exact commands with pinned-decoder PATH. Final DLL includes idle refinement;
source/native guards/gameplay math did not change during subsequent test execution.
Final hooks includes actual `bash tools/test_native_source.sh`:80 pre-session idle
polls preserve rejection budget, OFF zero positive pubs/WS commits, vanilla Up,
late snapshots, stale baseline, identity block/no further reads, Deinit and F8.
The original deadline assert that failed initially also passes in both full retry
and final hooks; no Sleep/deadline/reset/restore/ownership code changed.

SKIP5 final hooks = preexisting optional private captures/replays not distributed.
External pipeline32 SKIP additionally includes27 optional historical maintainer
logs absent in clean staging. UNAVAILABLE17 = preexisting native VCH/bridge evidence
boundaries; remain closed, not PASS. Additional owner evidence groups UNAVAILABLE:
actual Windows selector/loader/config path and ON/OFF directional comparison.
BLOCKED0. No further scalar capture, unchanged r6 test or movement patch requested.

`python3 -m py_compile source/tools/select_wallrun_comparison.py source/tools/test_select_wallrun_comparison.py source/tools/test_wall_motion_log.py` expected/actual exit0.
`git diff --check` expected/actual exit0.
`git diff --exit-code 64056d4 -- source/include/Math/reversal_heading.hpp source/include/Math/source_orientation.hpp source/src/GameInterop/stack_source.cpp source/mod/WallSprint.ini environment source/src/NativeHooks/immediate_reset_smoke.cpp source/src/msm2/orientation_trace.cpp source/src/NativeHooks/immediate_reset_trace.cpp` expected/actual exit0.
R6 math/default INI/stack writes/reset engine/shared tools unchanged; no test gate
weakened. Native diagnostic code/version differs as explicitly authorized.

Shared Wisdoms note published/readback matched
490538c43805807d366e5d9abc8827a10737e676. Actual game cause/acceptance stays UNKNOWN.
Release/CRC/source audit and publication recorded after generation.

## Final release / source audit

`PATH=/workspace/toolchains/analysis-python/bin:$PATH python3 source/tools/package.py`:
expected DLL/version/INI/source/license/PE/import/CRC contracts; actual1 PASS,
exit0,0 SKIP/UNAVAILABLE.
`PATH=/workspace/toolchains/analysis-python/bin:$PATH python3 source/tools/release.py`:
expected install/source archives and disabled script/manifest; actual1 PASS,
exit0,0 SKIP/UNAVAILABLE. No unvalidated external retry artifact reused.
`python3 /tmp/ws-r7-audit.py`: expected both ZIP CRCs, sole virtual install script,
disabled-script exact equality, source archive matches final CPP/headers/tests/
selector bytes, unchanged r6 heading/stack/INI and r7 diagnostic manifest; actual
1 PASS. Version/byte sizes/SHA256 in analysis/V01C38R7_RELEASE.json.
Outputs: MSM2-WS-v0.1c38r7.zip, MSM2-WS-v0.1c38r7-source.zip,
MSM2-WS-v0.1c38r7.script.disabled, BUILD_MANIFEST.json.
Root result/publication records may be newer than packaged pre-publication docs;
validated runtime/source bytes match final implementation. No owner deployment.

## NEED / exact treatment / log

Diagnostic-only r7. Update/install current WS once using normal updater/manager;
Chat supplies concise numbered comparison. With game AND Overstrike fully closed,
use `py -3 <_source>/source/tools/select_wallrun_comparison.py --game-root <game> --mode ON`
or OFF for separate fresh processes; RESTORE restores exact original bytes.
No F8 control, archive/scripts.txt/profile edit, changed speed/angle or new scalar
capture. Select AFTER Install mods and Run only afterward, because reinstall
rewrites default ON. Actual startup ini_path/mode/observation admission/source
admission/counters must confirm selection before accepting comparison. Detailed
one-method commands/spec in analysis/V01C38R7_SOURCE_COMPARISON.md.
Return separate MSM2-WS-v0.1c38r7-YYYYMMDD-HHMMSS.log files in WS/logs with mode
inside, plus compact owner report as requested by Chat. Still no raw mapped intent
or downward fix/causal acceptance. Publication follows after push/readback.

## Publication verified — r7

Implementation/artifacts commit `0cd1ed7e94e4633d3ba4817e02f548a780c85490`.
Initial push correctly rejected because remote gained documentation-only262e95b
(WSU_REFERENCE_REVIEW_20261031.md) during validation. Read/retained that parallel
reference research, which explicitly leaves current task unchanged. Merged ONLY
same development branch, not main; merge `f82d42a14574f479d60011acf2eb8417572c1a3e`
pushed successfully. No rebase/force/reset or source/artifact change from merge.
`git diff --exit-code 0cd1ed7 HEAD -- source BUILD_MANIFEST.json 'MSM2-WS-v0.1c38r7*'`
exit0. No rebuild needed for this documentation-only merge/record.
Independent GitHub fetch/FETCH_HEAD/local HEAD/ls-remote agree; all26 changed blobs
including three new runtime/source artifacts and parallel note match checkout
bytes: `python3 /tmp/ws-r7-publication.py` PASS. Subsequent record commit changes
only this result/state, not tested bytes. Five old untracked artifacts preserved.
Owner runtime is not deployed by publication; normal Update-WS -> Install mods
is the later owner installation step. Chat supplies numbered ON/OFF comparison;
selection uses installed WS INI through the dedicated tool, full restart, Run
only after selection; RESTORE exact original bytes. No improvement claim.
