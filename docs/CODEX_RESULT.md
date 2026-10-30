# Codex result — r5 post-build WSL verification / diagnostics

2026-10-31 JST. STATUS: tooling fix validated; owner cause/deployment unresolved.
Branch `codex/v01c20-edi-value-provenance`, baseline
`5741e05bf5d613ddccc47805ae8a47f56136547f`; no main merge.
R5 native code, INI, binary/script/source/install ZIP and BUILD_MANIFEST bytes
remain unchanged. No runtime rebuild, native suite or package regeneration.

## What happened / what is known

Owner terminal proves WSL build/test/package completion, then wsl_quiet failure
inside WindowsSystem.build BEFORE payload validation/deployment/checked launch.
The old catch combined UID/environ/cmdline/cwd reads into one PermissionError
message. Stage/errno/PID339 identity/liveness were not captured: actual cause is
UNKNOWN, not assumed orphan, access-denied, self-probe or installed r5 success.
Synthetic v0.1c20 updater successes are fixture output only. This failure path
has not reached the code that archives/replaces active script; actual owner
installed state is not independently inspected and is not claimed lost/updated.

## Changes / exact limits

- Embedded Linux process probe returns bounded authored JSON1: expected/observed
  distro/UID, PID, stage/category/errno, counts; caller adds exit status and rejects
  malformed/incomplete output or identity/exit mismatch. No raw unrelated argv,
  environment values or stderr. New build records bind numeric UID before launch.
- Exact token checks no longer read irrelevant cmdline/cwd; nonmatching readable
  environment is sufficient for that token. Legacy matching still reads both.
- Failed reads revalidate pinned pidfd exit/kernel absence with start/state
  identity; unreadable live process/PID reuse remains UNKNOWN. Even a live reused
  PID with equal start ticks cannot be excused by old signaled pidfd. Matching
  live build is ACTIVE; exact self PID excluded. No broad same-user exemption,
  arbitrary delay, process termination or lock deletion.
- OS exclusion/inherited child handles, unresolved build marker, clean/dirty Git,
  previous-artifact protections and deploy/commit/unlock/checked-launch ordering
  unchanged. Historical records lack original UID; uid_expected=null exposes that
  limitation rather than manufacturing original identity/all-UID proof.
- Fresh bootstrap `Repair-Mod-Tools.ps1 -DiagnoseWSL` reads authenticated pending
  metadata with freshly staged checker before old failing check. It does NOT
  claim/release ownership, merge/update worktree/KIT, build/deploy/launch. Fetch
  updates Git refs/objects; only authored script staging uses private temp files.
  No pending authenticated record yields explicit uncertainty, not a certificate.

Files: environment/operation_lock.py, repair_tools.py, Repair-Mod-Tools.ps1;
workspace.py UID binding; focused/lock/repair tests; analysis and matching
individual/combined operational manuals, PROJECT_STATE/result. No SF/ordinary
MOD/profile/loader mutations. Original five untracked ZIP/bundle files preserved.
Detailed chain/evidence: analysis/WSL_POST_BUILD_VERIFICATION_REVIEW.md.

## Required checks / commands / expected / actual

All commands from repository root. PASS counts denote cases/groups/checks as
specified, not interchangeable unique assertions.

| Exact command | Expected | Actual |
|---|---|---|
| `python3 environment/test_wsl_process_probe.py` | disappearing PID and PermissionError-on-exit revalidated; live permission/build blocks; PID reuse including same tick, self/unrelated/distro/UID, malformed output/exit and post-caller UID binding; real owned Linux token/pidfd | 21 PASS, 0 SKIP, 0 UNAVAILABLE |
| `python3 environment/test_operation_lock.py` | lifetime/concurrency/crash/child/legacy/identity/replace and WSL token regressions | 14 PASS, 1 SKIP, 0 UNAVAILABLE |
| `python3 source/tools/test-workspace.py` | prior artifact survives build/test/hash failures; committed artifact survives launch failure after unlock; actual second updater cannot deploy | 20 PASS groups, 0 SKIP, 0 UNAVAILABLE |
| `python3 environment/test_mod_environment.py` | shared environment/backup/restore/refresh/metadata safety | 28 PASS, 0 SKIP, 0 UNAVAILABLE |
| `python3 environment/test_tooling_repair.py` | clean/dirty/divergent/old checker repair, live gate refusal, read-only diagnostic does not mutate record/KIT/source HEAD/runtime | 4 PASS, 0 SKIP, 0 UNAVAILABLE |
| `python3 environment/test_start_overstrike_preflight.py` | installer source permitted, misplaced loader refused without mutation | 1 PASS group, 0 SKIP, 0 UNAVAILABLE |
| PowerShell Windows contract below | existing mock launch contract preserved | 16 PASS checks, 0 SKIP, 0 UNAVAILABLE |
| PowerShell Parser below | DiagnoseWSL bootstrap syntax valid | 1 PASS, 0 SKIP, 0 UNAVAILABLE |

```
XDG_CACHE_HOME=/tmp/ws-lock-xdg-cache XDG_CONFIG_HOME=/tmp/ws-lock-xdg-config XDG_DATA_HOME=/tmp/ws-lock-xdg-data DOTNET_CLI_HOME=/tmp/ws-lock-dotnet /tmp/ws-proxy-pwsh/pwsh -NoLogo -NoProfile -File environment/test_windows_contract.ps1
```

Parser exact command uses those same private XDG variables and
`/tmp/ws-proxy-pwsh/pwsh -NoLogo -NoProfile -File /tmp/ws-proc-parse.ps1`.
That owned script invokes `[System.Management.Automation.Language.Parser]::ParseFile`
on environment/Repair-Mod-Tools.ps1 with token/error refs and throws on errors;
no bootstrap actions are executed. PowerShell is Core/Linux with owned mocks,
not actual Windows COM/WSL acceptance.

Other verification:
- `python3 -m py_compile environment/operation_lock.py environment/repair_tools.py source/tools/workspace.py`: PASS.
- `git diff --check`: PASS.
- `git diff --exit-code 5741e05 -- source/include source/src source/mod BUILD_MANIFEST.json 'MSM2-WS-v0.1c38r5*' 'MSM2-Mod-Tools-v1.2.0.zip'`: PASS, protected bytes unchanged.

Initial existing lock-test run failed only its old diagnostic-wording regex;
updated expectation to new refusal message, then full focused suites passed.
Final code has been retested. No exception/permission safety assertion removed.

SKIP1: full sandbox /proc inventory cannot read a same-user process environment;
production checker fails closed. Isolated real owned child/env/pidfd integration
passes but is not substituted for full inventory or WSL evidence.
UNAVAILABLE: actual owner Windows/WSL/PID339/default-user identity/bootstrap /
deployment (one owner acceptance group). Owner state is necessary to classify
that original failure; no simulated result is promoted to real-PC PASS.
NOT RUN: native/build/portable runtime/smoke/hooks/package/release suites because
CODEX_NEXT requires tooling-only change and byte-identical r5 artifacts. Existing
runtime validation remains historical, not rerun/claimed for this fix.
No BLOCKED implementation subtask; actual owner diagnosis remains pending.

## Staging reuse assessment / ONE owner next action

Build output can persist under `_archive/_build` while external /tmp checkout is
normally removed. Hash/version/DLL manifest is not an exact source-HEAD/build-
session/inactivity receipt. Owner staging was not inspected. No cache shortcut
or manual unverified deployment added; normal retry necessarily rebuilds.

After publication, close game/Overstrike/updater normally and execute ONE fresh
read-only diagnostic block under docs/mod-tools/README.md (DiagnoseWSL). Return
its exact output, including refusal JSON/nonzero exit. Do not run Update-WS/game
or delete locks as part of this action. Chat gives game steps only after actual
deployment succeeds. Supported normal repair remains documented but is not the
next requested action. Publication SHA/readback recorded after push.

## Publication / shared knowledge

Implementation commit `5ccdd9ecc2b4b10f3e5bff877d033289a50c7f69` pushed to
codex/v01c20-edi-value-provenance. `python3 /tmp/ws-proc-publication.py` PASS:
independent clone fetch/FETCH_HEAD, ls-remote and local HEAD match, all15 changed
tracked blobs byte-equal. Final documentation-only handoff record is pushed and
checked with the same script. No runtime/test rerun for record-only edits.

Reusable process-evidence/deployment distinction synchronized to separate
MSM2-Wisdoms docs/MSM2_DEVELOPMENT_MANUAL.md, commit
`6e35580572b983862f888e35f4cc06ce67c71724`; push, ls-remote/FETCH_HEAD and document
blob readback PASS. That repository's documentation main branch is separate;
WallSprint main has NOT been merged or changed. No engine ownership fact promoted.

Owner ONE next action remains the fresh DiagnoseWSL block; no update/game test
until its private cause-specific result is reviewed. Build validated != deployment
validated. R5 binaries/packages unchanged, owner deployment not certified.
