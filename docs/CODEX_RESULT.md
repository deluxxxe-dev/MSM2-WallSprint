# Codex result — confirmed WSL environ EACCES / narrow root token recovery

2026-10-31 JST. STATUS: tooling implementation validated; owner recovery pending.
Branch `codex/v01c20-edi-value-provenance`; baseline
`053f6a5c1c7a73a0cccd7d18144c74ab99099040`. No WallSprint main merge.
R5 native/source/INI/binary/package/manifests byte-identical; no native rebuild.

## Supplied evidence / concrete change

Prior fresh23525e8 DiagnoseWSL confirms Ubuntu-24.04 matches, observed UID1000,
legacy expected UIDnull, PIDs346/347 environ EACCES(errno13), UNKNOWN/exit2.
No repair/deploy occurred. Their identity/relevance and owner r5 installed state
remain unknown. Do NOT repeat identical diagnosis, assume orphan or ignore all
unreadable same-user processes. Same UID does not establish environ readability.

Selected minimum allowed route: bounded read-only selected-distro root token
inspection, not a new supervisor/process framework. Existing user check binds
distro/known UID; only QUIET or access-denied-only UNKNOWN can reach root. Active,
reused/malformed identity, wrong distro/UID and other genuine unknowns refuse.
The only privileged command is WSL `--distribution <recorded> --user root --exec
python3 -c <authored readonly exact-token probe>`. Actual inspector UID0 and
scope/exit/schema/identity required. No build/compiler/deployment/shell is run as
root; no security settings/permissions/sysctl changes, WSL shutdown, process kill,
loader/profile/SF/ordinary-mod cleanup or lock/index deletion.

Root checks exact token across ALL UIDs, even after user QUIET, to cover surviving /
reparented/new-session/UID-escaped token children. Final PID/start inventory is
rechecked; changed/new identities rescan immediately at most3 passes then UNKNOWN.
No sleep or top-level-exit-only completion claim. Root unavailable/denied/timeouts/
wrong identity/malformed output remain specific refusal, never silently assumed
available. Null legacy original UID is not manufactured as1000: user observed1000,
inspector0 and build UIDnull are separate fields; all-UID exact-token absence is
the narrower recovery authority. Empty/PID-only records without exact token cannot
use this privileged route. No full environ/argv/token/private stderr is emitted.

WSL_TOKEN_CHECK TOKEN_ABSENT authority=token_retirement_only_not_deployment is a
snapshot, not deployment or universal process-tree proof. Descendants that erase
the token/arbitrary adversarial future processes are outside the existing inherited-
token contract; no broader certificate invented. Existing OS/inherited Windows
handle exclusion, safe interrupted-record preservation, Git clean/ff gates, build
exit/result/hash/version/source-HEAD and commit/unlock/checked-launch protections
remain independent. Supported normal repair holds that gate before reclaim/KIT
refresh. Root proof failure preserves old lock/source HEAD/KIT/runtime in fixtures.

Files: operation_lock.py; root/ordinary probe and repair tests; analysis/
WSL_TOKEN_ROOT_RECOVERY.md; individual+combined operational manuals, environment/
LOCAL_WORKSPACE docs and PROJECT_STATE/result. No version/package regeneration.
Original five untracked historical ZIP/bundle files retained unstaged.

## Exact commands / expected / actual

PASS counts below are cases/groups/checks, not summed unique assertions.
All final commands exited0.

| Command | Expected | Actual |
|---|---|---|
| `python3 environment/test_wsl_root_recovery.py` | protected unrelated absence resolves only via root; protected matching/unknown, escaped UID token, known/null UID, actual root identity/distro, root unavailable/timeout/failed protocol, malformed/reused records, bounded inventory churn; actual reparented live child after top parent exit blocks | 20 PASS /0 SKIP /0 UNAVAILABLE |
| `python3 environment/test_wsl_process_probe.py` | prior PID/permission/identity/scope/UID/post-caller and owned Linux token/pidfd cases | 21 PASS /0 SKIP /0 UNAVAILABLE |
| `python3 environment/test_operation_lock.py` | crash/child/legacy/race/replacement/shared exclusion, all-distro legacy and token regressions | 14 PASS /1 SKIP /0 UNAVAILABLE |
| `python3 source/tools/test-workspace.py` | build/test/hash failure keeps previous runtime; launch failure keeps committed new runtime; actual second updater cannot deploy | 20 PASS groups /0 SKIP /0 UNAVAILABLE |
| `python3 environment/test_mod_environment.py` | shared environment/settings/backup/restore/refresh safety | 28 PASS /0 SKIP /0 UNAVAILABLE |
| `python3 environment/test_tooling_repair.py` | existing clean/dirty/divergent/old-checker/idempotent/readonly tests; real owned Git ff + simulated EACCES/root absence repairs without build; root denial leaves prior bytes/HEAD | 6 PASS /0 SKIP /0 UNAVAILABLE |
| `python3 environment/test_start_overstrike_preflight.py` | installer proxy accepted, misplaced loader refused unchanged | 1 PASS group /0 SKIP /0 UNAVAILABLE |
| PowerShell mock contract below | existing launch contract preserved | 16 PASS checks /0 SKIP /0 UNAVAILABLE |
| PowerShell parser below | existing bootstrap syntax retained | 1 PASS /0 SKIP /0 UNAVAILABLE |

```
XDG_CACHE_HOME=/tmp/ws-lock-xdg-cache XDG_CONFIG_HOME=/tmp/ws-lock-xdg-config XDG_DATA_HOME=/tmp/ws-lock-xdg-data DOTNET_CLI_HOME=/tmp/ws-lock-dotnet /tmp/ws-proxy-pwsh/pwsh -NoLogo -NoProfile -File environment/test_windows_contract.ps1
```

Same private XDG variables with `-File /tmp/ws-proc-parse.ps1` invoke Parser.ParseFile
on environment/Repair-Mod-Tools.ps1, throw on errors, no bootstrap execution.
PowerShell is Core/Linux with owned mocks, NOT actual Windows/WSL acceptance.
`python3 -m py_compile environment/operation_lock.py environment/test_wsl_root_recovery.py`: PASS.
`git diff --check`: PASS.
`git diff --exit-code 053f6a5 -- source/include source/src source/mod BUILD_MANIFEST.json 'MSM2-WS-v0.1c38r5*' 'MSM2-Mod-Tools-v1.2.0.zip'`: PASS; protected bytes identical.

Initial existing isolated Linux fixture incorrectly expected a privileged proof
while running as non-root. It now verifies user token absence and mandatory root
identity refusal on non-root hosts; production checks were not weakened. Retested
full affected focused suites after final guards. Real owned subreaper harness is
process-local, reaps all its children, and proves top-level exit != child exit.
Cross-UID/root-read authorization cases are models, not a claim of actual WSL root
privilege. Reparented live-child/environment/pidfd evidence is actual owned Linux.

SKIP1: full sandbox same-user /proc inventory cannot read protected environment;
probe fails closed. Isolated token tests are separate evidence, not a replacement.
UNAVAILABLE: actual Windows/selected WSL root readability, ptrace restrictions,
owner repair and r5 deployment (one owner acceptance group). Never counted PASS.
NOT RUN: native Windows build/runtime portable/smoke/hooks/package/release suites,
per explicit tooling-only/byte-preservation scope; no runtime changes justify them.
No BLOCKED code subtask; owner privilege/recovery acceptance remains pending.

## ONE owner action / staging / result boundaries

Use the common manual's fresh NORMAL Repair-Mod-Tools.ps1 bootstrap, WITHOUT
DiagnoseWSL, game/manager/updater normally closed. It fetches/stages current code
before old checks; root inspects only; held-gate clean/ff repair refreshes KIT with
original records preserved. Success marker:
`TOOLING_REPAIR_COMPLETE runtime_unchanged=1 no_build=1 no_game_launch=1`.
Return its output including refusal. No full compile merely to classify blocked
processes, no game test requested, no identical diagnosis-only loop.

Existing validated staging may persist but lacks authenticated exact-source/
build-session/inactivity receipt. No cache/manual deployment shortcut added.
Normal subsequent Update-WS rebuild is still unavoidable and is NOT this action.
TOKEN_ABSENT or TOOLING_REPAIR_COMPLETE does not establish r5 deployment. Chat
gives game-test instructions only after deployment success. Publication SHA /
independent readback recorded after commit/push.

Final review additionally binds saved build_token to its recorded operation token when present; mismatched identity refuses before inspection. Authentic owned fixtures now use their actual operation token; focused20/lock/repair suites rerun.
