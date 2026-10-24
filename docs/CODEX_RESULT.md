# Codex result — one-click shared Overstrike startup (2026-10-24)

Branch: **codex/v01c20-edi-value-provenance**.
Implementation: **845e33cafecae352e5194d790358500bb0066081**, based on the
owner-confirmed Organization baseline 5192f05 and recorded branch policy e884fed.
Output: **MSM2-Mod-Tools-v1.1.0.zip**, 41244 bytes,
SHA256 `24262407787c3c757e4d1240435e6d842c288c3d23e3ce12796e789838b0d219`.

The user requested double-click preparation through Overstrike startup and chose
ordinary environment/layout checks, rather than updating/building WS at launch.
The existing actual-game installation/startup/log acceptance remains accepted.

## Change / use

Run `_source/environment/Start-Overstrike.cmd` once after pulling this branch
with game and Overstrike closed. It derives the game root from the authoritative
source layout (or asks for it outside that layout), checks shared EXE/loaders,
the MSM2 profile and WS layout, installs/refreshes only its own Mod-Tools files
and three owned shortcuts, then opens shared Overstrike with cwd=game.
Existing backup destination/profile and post-restore confirmation are preserved;
first-time backup defaults to home/MSM2-Mod-Backups without making a snapshot.

Thereafter double-click **MSM2 - Overstrike** on the desktop. It validates the
same paths, requires one root versioned WS script, refuses visible WS archives,
links and update/Git locks, and launches/reuses only the matching shared manager.
It does not pull Git, build/test/deploy WS, edit SF/ordinary mods, change profiles,
stop processes or start the game. Use Install mods / Run modded game in Overstrike.
Wrong layouts preserve all files and display an error instead of cleaning them.
Matching running Overstrike is activated; a foreign process path is refused.

Files: new environment/Start-Overstrike.cmd; environment engine/Windows adapter,
Python/PowerShell tests, README/TEST_REPORT; root README and shared recovery docs.
This delivery updates CODEX_RESULT, toolkit ZIP and MOD_TOOLS_RELEASE metadata.

## Validation / limits

- **27 Python tests PASS** (owned real filesystem/Git): existing backup/restore
  regressions plus new first-time setup/upgrade, preserved settings/dirty work,
  repeated startup without reinstallation/full scan/Git, path inference, and
  duplicate/archive/lock/loader/profile/unrelated-toolkit refusals.
- **16 PowerShell checks PASS** with fake launch/process/COM boundaries: exact
  EXE/CWD, same-manager reuse, three shortcut targets/icon/CWD, collision refusal
  before saving any shortcut, existing game protections and syntax parsing.
- Seven CRLF CMD entrypoint contracts and Python syntax pass. Distribution CRC,
  payload hashes/allowlist pass; standalone extraction rebuilds identical ZIP.
- Actual Windows desktop COM and this new launcher remain user acceptance checks.
  Linux/Core fake-host tests do not prove the user's shortcut was installed.

No main merge, SF code change or native runtime/version/binary package change.
WS stays v0.1c20/package0.1.38; EDI remains disabled, ownership/exact-build gates
and zero Position/source/input/movement writes are preserved. During publication,
same-branch upstream commits 013cbfa/2e7ac57 updated CODEX_NEXT/PROJECT_STATE to
reauthorize the separate c20 diagnostic task. Those files were read and retained
verbatim by merging only this development branch, without merging main or
overwriting the upstream task. This user's current launcher request does not
execute that separate native implementation. No native139-suite rerun was needed for shared
launcher-only changes. Next human step: one-time launcher setup, then confirm
the desktop shortcut opens the existing shared ordinary+SF+WS Overstrike UI.

---

# Codex result — Organization development baseline accepted (2026-10-24)

Repository: **deluxxxe-dev/MSM2-WallSprint**.
Branch: **codex/v01c20-edi-value-provenance**.
Owner-confirmed implementation baseline:
**5192f05f32313919fdf4049bd734b971655d163f**.

## Result and files

Fetched only the designated development branch from the Organization remote and
fast-forwarded the cloud checkout from 13c76f0 to the exact supplied baseline.
Updated this checkout's origin to the canonical Organization URL. No main merge,
history rewrite, reset or removal of existing untracked artifacts occurred.
The baseline's only change since 13c76f0 is workspace.py's canonical remote.

AGENTS.md now persists the owner's branch/source-of-truth, baseline and no-main-
merge instructions for later isolated tasks. This CODEX_RESULT entry records the
review and verification. No source, runtime, version or package changes were
needed in this handoff; existing output names remain MSM2-WS-v0.1c20.zip,
MSM2-WS-v0.1c20-source.zip and MSM2-Mod-Tools-v1.0.0.zip. Existing archives were
not regenerated and should not be represented as new baseline builds.

## Reviewed state and validation

- Read AGENTS, DEV_RULES, LOCAL_WORKSPACE, PROJECT_STATE and CODEX_NEXT in order,
  plus LOGGING_CONVENTION, BUILD, current result, c19 runtime evidence, c20
  prepared research/gate and the shared-runtime acceptance analysis.
- GitHub's designated branch resolves to the exact supplied full baseline SHA;
  source/tools/workspace.py uses the Organization remote.
- `python3 source/tools/test-workspace.py`: **13 PASS records, exit 0**. Owned
  filesystem/Git fixtures cover migration, fast-forward updates, rollback,
  independent archive-excluding clones, one active script, protected sibling
  preservation and fail-closed checks. They do not launch the real game.
- Native runtime files are unchanged. No new native build/gameplay regression or
  actual Windows run is claimed for this documentation handoff.

## Next work and unchanged gates

The latest derived actual-game evidence in
analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md accepts shared startup,
loading and versioned WS logs. PROJECT_STATE/CODEX_NEXT still describe the older
acceptance wait; this handoff preserves them as instructed by AGENTS rather than
silently rewriting the gameplay task. Both c20 research documents require an
explicit CODEX_NEXT reauthorization before research resumes. Production
writerAlias.edi.enabled remains false; the unresolved edge is EDI at 0x2B2358E,
not an authorized movement source. Preserve exact-build and all ownership gates,
zero Position/source/input/movement writes, and deferred producer 0x351EB00.

Continue future work from this branch and its subsequent commits. The next
gameplay task needs an explicit updated CODEX_NEXT using the accepted workspace
evidence. Separately, Mod-Tools' real Windows shortcut/backup/restore acceptance
remains pending; preserve the healthy environment. SF development stays with
SF Codex. No new runtime test or local migration is requested by this handoff.

---

# Codex result — shared launch and private environment recovery

Branch: **codex/v01c20-edi-value-provenance**. Toolkit implementation commit:
**5d089e628b5eb6ef60c22e1133720abcd1e4b68e**.
Output: **MSM2-Mod-Tools-v1.0.0.zip**, independent of unchanged WSv0.1c20/package0.1.38.

The user delegated SF development back to SF Codex and explicitly requested
simpler future startup plus a complete private shared-environment backup/restore
for the Vanilla game at the original path. The user selected full scope including
ordinary mods, Overstrike/settings, SF/WS source/Git histories/logs and loaders,
excluding game data. No gameplay/EDI/SF implementation advancement is authorized.

## Result

`environment/` adds six CMD entry points, a standard-library Python engine and a
small Windows host adapter. Install copies only its own toolkit/local config to
<game>/Mod-Tools and creates owned desktop Play/Backup shortcuts. Fast launch
validates the selected MSM2 profile, loader and installed DLL list, starts the
game with Overstrike-equivalent CWD/-scripts behavior and reuses a matching game
instance. Open-Overstrike uses only the existing shared manager with cwd=game.
No normal launch pulls Git, rebuilds, updates either project or kills processes.

Explicit shared backup preserves documented whole mod roots, hidden files,
empty directories, independent Git storage, dirty/untracked work, ordinary mods
and logs. It writes a new external timestamped folder, inventories/hash-checks
all files and verifies the archive before publishing it. The folder includes a
standalone recovery engine. Shared operation locking, process checks, source
stat/list checks and external-location enforcement protect against known changing
captures and recursively scanned backup archives. Private snapshots/rollback
folders are also ignored by Git; the utility performs no uploads.

Restore validates the original absolute game path and EXE hash, verifies/stages
before replacing selected roots, preserves current data in a sibling rollback
folder and reverses replacement failures. Incomplete rollback preserves both
locations and reports failure. There is no game-EXE/asset/save/WSL/toolchain copy,
relocation guessing or third-party content in the public toolkit ZIP. Complete
Install mods in restored Overstrike to reapply asset modifications; one explicit
installation confirmation then releases the fast-launch gate. No current user
backup has been created by the cloud and no live PC restore is claimed.

## Files / scope

New environment/{mod_environment.py,windows.ps1, six CMD entry points,README,
TEST_REPORT,LICENSE,build_distribution.py, two tests}; docs/MOD_ENVIRONMENT_BACKUP.
AGENTS/DEV_RULES distinguish this explicitly user-authorized shared utility from
WS-only updater cleanup; LOCAL_WORKSPACE/README link its optional sibling layout.
This delivery adds private-backup Git ignores, this result and release metadata/ZIP.
The existing source/ subtree, WS runtime/version/public WS packages,
CODEX_NEXT/PROJECT_STATE and the SF repository are unchanged by this task.

## Validation / remaining human check

- **21 Python filesystem/Git tests** pass: complete dirty/history/data round trip,
  recovery with the original game removed/recreated as Vanilla at the same path,
  source/target/archive symlink and malformed/CRC-valid-but-hash-invalid ZIP refusals,
  wrong build/path/process/lock refusals, concurrent change detection, synchronous
  rollback and incomplete-rollback preservation, readonly/mtime/empty-directory
  recovery, preserved newer work and explicit post-restore installation gate.
- **11 PowerShell launch-contract checks** pass with owned mocked OS boundaries,
  plus parsing; correct EXE/CWD/-scripts, commandline handling, same-instance reuse,
  active/foreign-process protection. Actual Windows COM/shortcuts/process launch
  and long-path restoration remain pending, not proven by Core/Linux checks.
- Six CMD wrappers are CRLF, use absolute engine paths, forward arguments and move
  cwd outside replaced toolkit directories. Python syntax checks pass.
- Public ZIP CRC/full-file hashes/allowlist pass; a fresh standalone extraction
  rebuilds the exact same37782-byte ZIP without the repository. The manifest records
  source commit and SHA256. Only authored source/text/license/tests are included.
- Existing WS native source and binary assets are byte-identical to the accepted
  runtime delivery. No native139-suite rerun is justified by these separate tools.

Next user step: close game/Overstrike, fast-forward the existing preparation clone
and run environment/Install-Mod-Tools.cmd for the known game root. Choose an
external backup folder, verify shortcut creation/ordinary launch, then create and
verify the first real snapshot. Keep the healthy environment; do not delete it
merely to test restoration. User-facing procedure and limitations are in
[environment/README.md](../environment/README.md); tests are in
[environment/TEST_REPORT.md](../environment/TEST_REPORT.md). SF work stays in SF Codex.

---

# Codex result — final local WS workspace migration

Branch: **codex/v01c20-edi-value-provenance**.
Implementation commit: **a4ec9c0f2635e9f2594bf5972d1765332de73766**.
Latest instruction base: **4402729a53eceeede6476c24d814bfd8469978fa**, merged without history rewrite. Prepared c20 research is preserved in **9057d24** and remains production-disabled; accepted c19 implementationb3185cd/deliveryba0b1d7/runtime evidence retained. No main changes/force push. CODEX_NEXT is preserved from the latest infrastructure task; AGENTS/DEV_RULES/LOCAL_WORKSPACE/LOGGING_CONVENTION/PROJECT_STATE updated as explicitly authorized.

## Outcome

**v0.1c20 / package0.1.38: the latest real-PC diagnostic identifies44.69ms pre-installation thread-list setup against a30ms deadline. The test-only correction prepares a real OS snapshot before the synthetic commit, retains real hardware capture/deadline checks, and adds slow-setup success plus delayed-snapshot refusal controls. The full external pipeline passes139 public tests /5 optional private-input SKIP. The corrected real-PC focused run now passes22 records /1 optional SKIP with exit0. The full corrected real-PC pipeline now passes139 tests /5 optional private-input SKIP and deploys successfully; shared Overstrike shows exactly1 WallSprint alongside ordinary mods. The subsequent successful SF update and shared screenshots confirm exactly1 checked SocialFeedProbe and1 checked WallSprint alongside ordinary mods. The user now confirms game launch/WS operation and supplies both new versioned logs: installation, shared loading and workspace logging are accepted. WS short sessions1594/2234ms pass retention/Deinit, zero hard kills and Position writes; SF bootstrap works but handle candidates remain0. EDI/gameplay research remains paused.**

The authoritative shape is game-root Mods Library/Mods-Developing/MSM2-WS with one root MSM2-WS-v<version>.script, Update-WS.cmd, logs, independent _source clone and disabled _archive. A one-time Migrate-WS.cmd is included. Shared Overstrike is only game-root/Overstrike/Overstrike.exe with game-root WorkingDirectory. No AppData/project-specific copy/profile/library is created. No old D-drive environment, SF or ordinary mod is cleaned, backed up, moved, renamed or overwritten by WS tooling.

## Changes / files

- New root and source/tools/windows Migrate-WS.cmd / Update-WS.cmd call repository-owned source/tools/workspace.py via Windows Python3.11+. Migration accepts an explicit validated game-root/branch, creates only WS folders and clones the intended WallSprint remote with no local/shared-object optimization. Existing correct clones are validated, never replaced by a legacy .git copy.
- Independent clone checks reject root/nested symlink/junction/reparse points, worktree/gitdir files, external common-dir/alternates/core.worktree, wrong remote, detached/unexpected branch, dirty tree and active source scripts. Expected branch is explicit/stored in local Git config; matching upstream and git pull --ff-only are mandatory. No hard-coded future branch or personal drive dependency. Git long paths enabled for the clone.
- Update validates the exact workspace/game/shared paths, stops only the validated game/shared Overstrike using reviewed PowerShell, and refuses a different running Overstrike instead of killing it. Shared Overstrike must exist; Overstrike/winmm.dll and scripts_proxy.dll duplicates refuse; game-root scripts_proxy.dll and winmm.dll are mandatory. Shared binaries are never repaired/copied by WS tooling.
- WindowsSystem invokes configured WSL2 and source/tools/windows/workspace-build.sh. The same pinned Linux cross-build, portable tests, actual DLL/logger smoke, full hooks/native plus new workspace suites, and package/release validation run in a temporary WSL source copy outside Mods Library. Missing tools fail; no MSVC/CMake/prebuilt/skip-tests shortcut. Toolchain uses the existing WSL /workspace/toolchains prefix, not a legacy checkout. Inherited private capture/toolchain overrides do not become old-drive dependencies.
- Transactional deployment stages validated .script.disabled under _archive/_build, verifies authoritative header/manifest/ZIP/DLL hashes, source cleanliness/HEAD and layout, then archives only positively WS-owned old root scripts as .script.disabled and atomically deploys one versioned root script. Postcondition asserts exactly one active script recursively, zero in _source/logs/_archive. Failed deployment/launch rolls back prior active bytes/helper where possible; failed preflight/build/test/package never starts replacement. Update lock rejects concurrent/stale update state. Source Git changes are not discarded.
- Tracked root WallSprint.script removed without history rewrite; .gitignore/output rules prevent live script materialization. Existing generated cloud cache scripts were renamed to disabled names preserving bytes. package.py writes only versioned .script.disabled; release.py virtualizes the install ZIP's one active versioned member, never unpacking it into source. Historical ZIPs remain. Source ZIP contains no active script.
- Runtime GameInterop/log_file.hpp now appends exact Mods Library/Mods-Developing/MSM2-WS/logs to its executable root, creates/checks each directory and rejects blocked/reparse components. Filename MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log comes from authoritative version/local time, numeric -01..bounded9999 collisions use CREATE_NEW, no overwrite/fallback. Same loaded-module A/E/re-enable append retained. Errors keep stage/path/Win32 code via debug output; telemetry payload/UTC line clock/native policy unchanged.
- log_file_smoke.cpp/smoke_windows.sh test new actual Windows Unicode/space nested directories, version/timestamp/collision/reopen/failure behavior and real DLL integration. test-workspace.py adds8groups of actual owned Git/filesystem fixtures including ff updates/divergence, independent clone, active/disabled invariants, rollback, protected siblings and alias/lock/reparse/dirty/preflight failures. Existing suites retained. New Windows wrappers, LF/CRLF attributes, source/build docs, README_TEST and audit synchronize the final workflow.
- Prepared EDI code/offline tests remain preserved but writer_entry_snapshot.cpp explicitly leaves edi.enabled=false. No EDI/gameplay/producer advancement is claimed. Superseded UpdateForTest/AppData tooling was removed from the current tree and remains in Git history. Mod description now identifies infrastructure-only behavior.

## Validation

Final Windows x64 warnings-as-errors build PASS. Serial public results:

```text
Portable ASan/UBSan                        13 PASS
DLL smoke + actual Windows logger          7 PASS
Full hooks/native/prepared/workspace      111 PASS /5 private-input SKIP
Total public PASS records                 131
Entry/ABI/hardware/finalization/ranges      11 PASS (included)
C14 contract                               4 PASS (included)
C15 writer branch                          6 PASS (included)
C16 call/effect                            7 PASS (included)
C17 alias                                  7 PASS (included)
C18 local callee CFG                        6 PASS (included)
C19 write contract                         7 PASS (included)
Prepared EDI offline                        6 PASS (included; production disabled)
Workspace Git/filesystem                    8 PASS (included)
Logger filesystem                          3 PASS (included in DLL smoke)
Separate historical private replay          4 PASS records
```

Historical replay contains1duplicate baseline and separately resolves1optional public SKIP; other4 absent historical private inputs stay skipped. Those444/128byte reset-copy functions are not the current137/183byte callee/writer. No raw capture is executed/redistributed.

Workspace fixtures use temporary owned Unicode/space game/remote/legacy/SF/ordinary-mod trees. Names, hashes, sizes and mtimes of sibling SF/ordinary/legacy data remain unchanged. Fresh no-reference clone and fast-forward next-version deployment, archive collision/idempotence and exact one-active postcondition pass. Detached/dirty/unexpected/diverged/reparse/worktree/alternates/active-source/foreign-script/duplicate shared DLL/lock failures pass. Stop/build/test/package/script hash/DLL hash/launch failure leaves or restores prior active script bytes; no real game process or user C:/D: folder is touched. Real Windows OS process stopping/WSL/GUI remains a local acceptance item.

Owned entry ABI still preserves GPR/YMM/XMM/x87/MXCSR/flags/stack/nonvolatiles/LastError/returns;8captures/7complete concurrent+nested maps/28paired stores/two threads, XYZ7/sourceW0/destW1 and finalized1/0 summaries. Owned A/E integration retains >=4s verified Deinit/retention/Up1/hard0/Position0/12events and clean DR restoration; measured counters in analysis/V01C20_RELEASE.json. Fixtures cannot pass actual-game admission.

Staging wrapper: **PASS: full real staging wrapper ran unchanged required build/portable/DLL/hooks/workspace/package chain from an owned Unicode/space source path and returned validated disabled payload/ZIP/manifests outside the checkout.**

Standalone source ZIP: **PASS: fresh source ZIP rebuilt DLL and passed54 focused +7 DLL/logger +8 workspace records, preserved finalized1/0 logs; no builds/private inputs or active scripts in source.** Archive CRC/current complete code/docs/helpers/licenses, nested script/DLL hashes, no-live-source and private-body exclusion are recorded in V01C20_RELEASE.json. Published ZIP/metadata are verified through fresh GitHub clone, not an anonymous HTTP/browser test.

## Safety / research preserved

Accepted c19 actual A3844ms/E4094ms, normal Deinit/retention/Up1/stale0/hard0/Position0,8entries/maps each and XYZ7/sourceW0/destW1 remain authoritative. Exact blocker is still MOV0x2B2358E [RCX+0x10C],EDI with unresolved EDI provenance. Prepared research9057d24 is inactive; c20 EDI is the next gameplay-research task only after migration acceptance and explicit CODEX_NEXT reauthorization.

Exact exe2.810.0.0/SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9, callee0x2B23560..0x2B235E9/hash2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d/current full code/boundary and all lifecycle/orientation/stale/DR/direct-call/source/alias gates retained. Native source/lifecycle/c12 analyzer/raw thunk/ABI/pairing/stop/cleanup/old State/Slice/caller/producer are unchanged. No gate/assertion/timing weakening. Existing snapshot non-atomicity/incomplete hardware/close-overlap/bounded refresh limits remain.

writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/downstream_conflicts_proven_absent0; Position/source/input/movement writes0. No producer0x351EB00 traversal, new game call/observation/PAGE_GUARD/data-page mutation, movement/adhesion/camera/animation/physics work. Runtime changes are limited to log destination/name.

## Outputs / first local action

- MSM2-WS-v0.1c20.zip (virtual versioned .script + complete migration bootstrap)
- MSM2-WS-v0.1c20-source.zip (complete source/docs/tests; no active script)
- Migrate-WS.cmd / Update-WS.cmd; non-active MSM2-WS-v0.1c20.script.disabled; BUILD_MANIFEST.json; analysis/V01C20_RELEASE.json

Extract the install ZIP **outside Mods Library**, keep its tools directory together with Migrate-WS.cmd, prepare Windows Python/Git/PowerShell/configured WSL2 and shared game-root Overstrike/DLL prerequisites described in README_TEST/source/BUILD, then replace the placeholder with the real game root and run:

```bat
Migrate-WS.cmd --game-root "<actual game root>" --branch codex/v01c20-edi-value-provenance
```

Thereafter double-click installed MSM2-WS/Update-WS.cmd. Confirm one shared UI sees ordinary mods + SF + WS, choose Install mods -> Run modded game, and verify a new versioned log in MSM2-WS/logs. Keep the old environment until all real-PC checks pass; it is never automatically deleted. Normal human A~4s -> fully end -> move/fast-travel -> E~4s can then verify regression/new logging; privately send newest complete runtime log, video only on visible anomaly. Real migration is not claimed complete by cloud fixture success.


## First-run WSL bootstrap follow-up

Follow-up implementation commit: **556e67e9404dc75fa78230ef8e19ba5710ce2e2a**. Latest instruction base: **d50bd29afae075791717e5491b82d98bf0744588** (real-PC acceptance gate), fast-forwarded without rewriting history. PROJECT_STATE and CODEX_NEXT are preserved from that instruction base. The original cloud regression and standalone69 counts above are the b191708 baseline; the additional checks below specifically exercise this bootstrap repair.

User-confirmed local prerequisites: Windows Git2.55/Python3.13; Ubuntu24.04 WSL2 x86_64, Python3.12/glibc2.39; basic/support packages; pinned GCC14/Clang19 setup and Capstone5.0.6 analysis environment completed. Game-root winmm.dll was restored by the user and is absent from shared Overstrike. This is prerequisite evidence, not completed migration/UI/runtime acceptance.

A first-use defect was found before asking the user to migrate: smoke_windows.sh attempted to create the NLS symlink inside its own nonexistent destination (usr/share/wine/wine/wine). Cached cloud toolchains already had usr/share/wine/wine -> ., masking that branch. The corrected command creates the link at usr/share/wine/wine. source/BUILD.md documents the first-run requirement.

Validation: bash syntax PASS; the exact checked-in Wine setup prelude with the NLS link absent creates the correct link; a fresh owned isolated Wine prefix runs real DLL load/SEH/repeated enable-disable/cleanup and diagnostic formatter (2 PASS), followed by the full existing DLL/logger smoke (7 PASS). The original cloud link was preserved/restored and only the owned fresh-prefix server was stopped. Wine emitted missing auxiliary winedevice/rundll32 messages, recorded in the test log; the checked programs returned0. No Wine GUI, actual game or real-PC success is inferred. Test logs are source/logs/workspace_fresh_wine.log and workspace_bootstrap_smoke.log.

Runtime DLL/version remain v0.1c20/package0.1.38; this change repairs test-environment setup only. EDI remains production-disabled and no native safety/admission/timing gate changes. Current ZIP source/docs are refreshed; prior pinned b191708 download links still identify the original snapshots. Migration uses the current GitHub branch for its independent _source clone and full validation. Real Windows migration/shared Overstrike/UI/new runtime log acceptance remains PENDING.

Next local step: leave Ubuntu with exit, fast-forward the Windows preparation checkout, then run its Migrate-WS.cmd with the actual game root and codex/v01c20-edi-value-provenance. The helper runs the preserved full build/test/package pipeline before deployment and never deletes the old environment or changes SF/ordinary mods.


## Real-PC first migration failure / focused reset diagnostics

Diagnostic implementation commit: **569b0e0e37b00022a7e1c38047728f1d3c533682**. The user's first real-PC attempt at9815793 completed the Windows/independent-clone/branch/WSL preflights and Windows x64 build, then portable13 and DLL/logger7. The supplied terminal excerpt contains47 PASS records and3 optional private-input SKIP records before failing in immediate_reset_smoke.cpp at assert(windows[0].hits==1). The full native suite/package/deploy/Overstrike-launch stages did not complete. Migration is NOT accepted; this is not a gameplay failure or actual-game test. No full terminal dump or private game body is committed.

The root cause is UNCONFIRMED: the old output lacks hit/drop/capture/timing state. Wine auxiliary missing-process messages alone do not identify it. The failing legacy fixture has a real30ms commit-relative capture deadline; host delay is one hypothesis, not an established fact.

The owned fixture now records QPC just before its store and, only before the original first-hit fatal assertion fails, prints RESET_SMOKE_FAILURE with hits/drops/capture/primary-installed, arm/store timing from commit, unchanged deadline and stop reason plus its synthetic diagnostic output. No production source, deadline, assertion, budget, retry, skip or admission gate was changed. source/BUILD.md documents focused diagnosis. Runtime DLL/artifact/version and the latest CODEX_NEXT/PROJECT_STATE remain unchanged.

Validation on cloud: focused reset suite9 PASS /1 optional private SKIP. A separate owned negative variant introduces40ms delay and is required to fail the original hits==1 assertion: observed store41.3498ms, deadline30.0000ms, hits0, stop_reason=deadline. That confirms the added failure output and preserved deadline, not the cause on the user's PC. Logs: source/logs/workspace_reset_diagnostic_tests.log and workspace_reset_diagnostic_negative.log. Original131/5SKIP and standalone69 above remain baseline records, not a successful real-PC migration.

Next human action: dismiss the migration failure pause, fast-forward the Windows preparation checkout, and invoke source/tools/windows/reset-diagnostic.sh through WSL so source is staged to Linux before build/reset tests. This collects the precise failure without deploying to the game. Share RESET_SMOKE_FAILURE and following assertion, or the final focused PASS output. Full migration/build/tests/package and real shared Overstrike/UI/new runtime log acceptance are still required afterward. Keep EDI paused and all Position/source/input/movement writes0.


## Corrected focused diagnostic filesystem / runner

Staging helper implementation commit: **bc050b27d9cf1cdd01fda6208b4e735a2a549c25**. The user's focused diagnostic attempt at8bf261c failed before running the reset fixture: Windows-mounted /mnt/c source compilation reported missing VOID/LPVOID/BOOL in MinHook. The prior direct-build guidance was incorrect. tools/compat/Windows.h contains an include of lowercase windows.h; a case-insensitive source filesystem can resolve that to the same shim, hiding the real MinGW header. An owned header-alias fixture reproduces the missing-type error. No vendor/compat/production source is changed.

New source/tools/windows/reset-diagnostic.sh derives its preparation source folder, copies it without build/log/output caches into a uniquely owned Linux /tmp/msm2-ws-reset-diagnostic.* tree, creates the required log directory, checks header-case separation, then runs the unchanged build_windows.sh and test_reset_trace.sh. It retains that diagnostic tree/logs for inspection and never deploys to the game. Source/BUILD.md now requires a case-sensitive Linux build tree and routes focused diagnosis through this helper. The normal migration pipeline already stages to Linux and is unchanged.

Validation: shell syntax/default source resolution PASS; actual default helper in a fresh Linux staging tree completed Windows DLL build and9 focused PASS /1 optional private-input SKIP (source/logs/workspace_case_sensitive_reset.log). Owned Windows.h/windows.h alias reproduces the compiler error. The original capture assertion, real30ms deadline, existing suites, production DLL/manifest and latest CODEX_NEXT/PROJECT_STATE remain unchanged. The first real-PC capture failure is still UNCONFIRMED because this second attempt did not reach the fixture. No claim of successful migration, shared Overstrike/UI or new runtime log acceptance is made.

Next user step: fast-forward the Windows preparation checkout, then invoke its source/tools/windows/reset-diagnostic.sh using wsl -d Ubuntu-24.04 --exec bash. Send RESET_SMOKE_FAILURE plus the following assertion, or final focused PASS output. Full migration acceptance remains pending afterward. No old environment/SF/ordinary mods are deleted or changed by focused diagnosis.


## User-confirmed focused and full migration pipeline PASS

The user supplied terminal output from the corrected Linux staging helper: the Windows DLL build and focused reset suite completed9 PASS /1 optional private-input SKIP. The original first-capture assertion did not recur. Its original cause remains unconfirmed; no real30ms deadline, assertion or gate was relaxed.

The subsequent real Windows Migrate-WS.cmd attempt completed the full preserved WSL pipeline and returned to PowerShell. The test portion contains exactly131 PASS /5 optional private-input SKIP records; package validation and final staging each add a separate PASS line. The18 optional maintainer-log SKIP messages are absent cloud report files, not omitted build/test stages. Final actual-helper messages after the fixture tests are: required full WSL build/test/package completed; WS update complete: MSM2-WS-v0.1c20.script; Local migration checks passed. Fixture-only v0.1c21 messages are not the deployed user version.

This supports real helper preflight/independent-checkout/update/build/tests/package, versioned deployment and its one-active-script postcondition. Shared Overstrike launch was requested successfully, but no GUI observation has yet been supplied. The user has not yet confirmed ordinary mods + SF + WS together, Install mods / Run modded game, or the actual new versioned runtime log. Overall acceptance remains PENDING, EDI stays paused, and the old environment must remain until the remaining checks pass.

User-local generated ZIP reports: install3332536bytes/SHA256236e9088014b244a0c7e044ef5dca0a135d922543de4ccc55b300246a9775374; source764618bytes/SHA25617a0263dd6d604cf15cec5b95a2b9a96e272b4be27cd65e3b709cb34de24b974. These describe the local build. Public ZIP snapshots remain pinned at2735e500e7bee17ce6d3016736439f2bd5edad0c; this follow-up only records acceptance evidence and does not republish packages or change runtime/source.

Next human step: confirm that the shared Overstrike window opened and that ordinary mods, SocialFeed and WallSprint appear together in the same MOD list. Then proceed to installation/game launch and new runtime-log checks one step at a time. No gameplay research is authorized by this partial infrastructure result.


## Archive-aware duplicate correction — implementation a2faf166851270a78bd5ca30c67cd55bcc59cc5c

The user supplied shared-Overstrike screenshots showing24 checked WallSprint entries and ordinary mods; SF is not identifiable in that list. This is a failed UI acceptance gate despite the old131-test pipeline/deployment PASS. Upstream extension dispatch recursively discovers `.script` members in ZIP/RAR/7z without extraction. The repository's44 tracked ZIPs contain22 install scripts; source22 + build-cache1 + root active1 predicts the observed24. Individual entry paths were not captured. analysis/OVERSTRIKE_ARCHIVE_DISCOVERY.md records the evidence and independently implemented model.

Changes: workspace.py adds archive-aware guards, managed non-cone Git sparse checkout (all code retained, archive working-tree paths excluded), clean/tracked-source upgrade checks and positive legacy build-ZIP validation/quarantine. It refuses untracked/foreign/corrupt/misplaced archives and custom sparse configuration. Git archive blobs/commit history remain intact. Old WS build outputs move only from _archive/_build to collision-safe disabled names under _archive with exact bytes/mtime. New build ZIPs return only as .zip.disabled. Strict checks run after upgrade, after build and before launch. release.py refuses direct ZIP output within Mods Library; workspace-build.sh also rejects an unsafe old-bootstrap source tree and output archives. Windows root entry names, independent-clone/upstream/ff-only/no-links checks, old active-script rollback and sibling isolation remain.

Existing8 workspace groups retained plus5 archive groups =13. They cover recursive nested ZIP discovery without extraction, disabled-container exclusion, tracked archive/history/code preservation, root/nested/mixed-case sparsity and clean ff updates, old full-checkout/build-cache upgrade with modeled3 entries ->1, bytes/mtime/collision preservation, unknown archive refusal, custom sparse preservation, direct-release/old-bootstrap guards and fresh build output refusing a raw ZIP. A separate actual independent repository clone reduced44 visible ZIP files to0 while retaining all44 tracked blobs, code bytes, HEAD and clean state in an owned Unicode/space path. Public ZIP filenames/downloads and historical artifacts remain available outside Mods Library; no force/history rewrite or main changes.

Validation: actual full external staging pipeline completed build/portable/DLL/native/prepared/workspace/package with136 public PASS /5 optional private-input SKIP (portable13, DLL/logger7, hooks116 including13workspace). Both returned archives are .zip.disabled with valid CRC; manifest/script/DLL hashes agree and no visible archive/script remains in returned build artifacts. Latest13 workspace groups also passed with the old-bootstrap guard. An initial cloud attempt hung in an unchanged owned Up-watch fixture; only stale/owned synthetic test processes were stopped and the prior cloud Wine prefix was preserved under /workspace/toolchains/ws-archive-wine-isolation-sprs4yir/previous-prefix. The full suite then completed with a fresh cloud-only prefix. The original hang cause is unconfirmed; no test, deadline, assertion or native gate was relaxed. User-PC WSL state was not changed by cloud tools.

Every native source/header and version file is unchanged from d19fa36. The exact tested DLL from the successful full pipeline is retained for release. EDI remains disabled; all ownership/write safety gates and c19 evidence remain. No SF/ordinary-mod/old-D/shared-cache/profile cleanup is performed. The earlier real-PC physical one-script postcondition is insufficient and does not count as whole-workspace acceptance.

Current standalone source ZIP verification: PASS74 records (54 focused native +7 DLL/logger +13 workspace), no private inputs or previous builds/logs at extraction. Verification exposed a test-fixture manifest referring to upstream ZIP bytes rather than the actual active fixture ZIP (creation timestamps can differ). The fixture now hashes the actual payload; a valid-CRC archive with a mismatched manifest is explicitly refused without moving either archive. The61 native checks were retained only after proving all runtime sources/headers/build/native-test scripts byte-identical across this fixture-only correction, then the corrected13 workspace groups ran successfully. Original standalone69 above is historical baseline only. Current artifact validation covers CRC/current complete sources/docs/helpers/licenses/script and DLL hashes/private-body exclusion; details are in V01C20_RELEASE.json.

Next user action: close the current Overstrike without installing, update the Windows preparation checkout, rerun that updated Migrate-WS.cmd for the same game root/branch, then confirm exactly one WallSprint in shared Overstrike. Use the updated preparation helper to upgrade the old installed engine. SF/ordinary visibility, Install mods / Run modded game and a fresh actual versioned runtime log remain required. Keep the old environment and keep gameplay research paused.

Fixture consistency and validation-log refinement commit: **31065f8228f415ef0f464aef6b88b727adafa63a**. No production archive ownership/hash check was relaxed.


## Real-PC archive-upgrade rerun — reset arm rejected

Diagnostic implementation commit: **6fcf33e45e49523a456a75016e267f7717713e79** (branch codex/v01c20-edi-value-provenance). The supplied user output at delivery890d23c reports46 public PASS and3 optional private-input SKIP, then immediate_reset_smoke.cpp:156 asserting beginReset(r,snapshot). The earlier failure was hits==1 after arming; this is a different rejection point. The terminal does not identify which synthetic session failed or whether rejection happened before window allocation or during thread enumeration/debug-register installation. Scheduling/deadline and installation failure are possibilities, not established causes. winedevice messages also appeared in successful preceding tests; they do not establish this failure's cause. No raw user terminal dump is committed.

Only the test fixture's failure output changes: every rejected arm now prints RESET_SMOKE_ARM_FAILURE with session, elapsed commit time, admission/identity/alignment/debugger state, before/after window counts, active leases and an explicitly unattributed LastError. It prints existing production skip/arm records and, only if a new window was allocated, that window's timing/hit/stop diagnostic. The original fatal arm assertion remains. No absent-window indexing, retry-to-pass, clock substitution, deadline extension or production admission change is introduced. QueryPerformanceCounter failure is explicitly asserted. Runtime sources/headers/config/version and the validated distributed DLL/script are unchanged from890d23c.

The case-sensitive reset-diagnostic.sh helper captures both stdout and stderr, prints its retained folder and exit status even on failure, runs the existing focused suite once and repeats the unchanged short-window fixture five times without rebuilding. It stops at the first failure through pipefail. It neither runs the full migration nor deploys to the game; SF/ordinary mods/old D/shared cache and user Wine state are not changed. README_TEST (both copies) and source/BUILD.md describe this path. release.py includes only the three named independent diagnostic validation logs.

Validation: focused9 PASS /1 optional private SKIP; the actual helper in a fresh Linux stage rebuilt the DLL and completed19 PASS /1 optional SKIP (focused9 plus five repetitions with2 PASS each). Two separate owned negative variants deterministically backdate the commit by50ms or supply no candidate thread: both produce the new diagnostic, preserve the fatal assertion and return nonzero. The pre-allocation case reports windows0->0 and the failed-installation case0->1. No production/game source was changed. Baseline full136/5 and source74 results above belong to890d23c; they are not rerun or relabeled as successful real-PC acceptance for this diagnostic-only change. Public ZIPs are refreshed with current code/docs and the same validated runtime payload.

Next action: fast-forward the Windows preparation checkout and invoke its source/tools/windows/reset-diagnostic.sh through WSL Ubuntu-24.04. Send RESET_SMOKE_ARM_FAILURE / RESET_SMOKE_FAILURE and subsequent records, or the final exit line if all five repetitions pass. Do not repeat the full migration or install mods yet. Actual cause and the full real-PC acceptance gate remain unresolved; c20 EDI/gameplay research stays paused.


## Confirmed real-PC setup latency / test-only snapshot correction

Implementation commit: **79d2acff84574b7c794abe980560dad1e3e9052a**, branch codex/v01c20-edi-value-provenance. The user's focused result on6547ec4 now identifies session1: all printed admission/identity/alignment gates pass, no debugger, window start44.692600ms after commit and observed return44.706700ms against the real30ms deadline. installed/skipped_busy/failed/hits/drops are all0; no lease remains. Production enumerates the thread list before assigning window.start, then correctly skips installation after expiration. Thus the failure occurs in pre-installation setup, not a failed debug-register write. Individual snapshot/enumeration API costs are not separately measured. LastError18 is unattributed; the cleanup stop label does not establish identity loss. analysis/RESET_SMOKE_SNAPSHOT_TIMING.md records measured facts and limits without publishing the raw terminal dump.

immediate_reset_smoke.cpp now prepares a real OS thread snapshot before the synthetic commit QPC for the direct legacy and async-helper positive tests. A test-translation-unit-only atomic adapter transfers that real handle once to unchanged production enumeration/filtering/close logic. QPC, thread entries, filtering, suspension/context APIs, actual owned-memory store/VEH/MinHook and the30ms capture/deadline assertions remain real. No API adapter enters the runtime DLL. Prearmed and process-expansion tests use actual snapshot acquisition through fallback. No production function or game guard changes.

The required reset suite adds --slow-setup (50ms preparation before commit, original positive hardware assertions must succeed) and --slow-snapshot (50ms delivery delay after commit, arm must fail with no installed watch/hit/lease, unchanged memory and debug registers). This tests both the repaired fixture boundary and preserved fail-closed behavior. Existing failure diagnostics remain fatal. There is no retry-to-pass, clock substitution, new gameplay write or deadline extension.

Validation: focused12 PASS /1 optional private SKIP. The complete fresh-stage diagnostic helper also rebuilt the DLL and passed22 records /1 optional SKIP, including all five repeated short capture executions. The actual full external workspace-build pipeline rebuilds DLL, runs portable13 + DLL/logger7 + hooks119 =139 public PASS /5 optional private-input SKIP, creates validated packages and returns only .script.disabled / .zip.disabled artifacts with correct CRC/hashes. Latest full output is source/logs/workspace_reset_snapshot_full.log; the exact DLL from that successful pipeline is used in the final release. The previously recorded source74 validation is historical (890d23c), not a newly repeated standalone suite; current ZIP integrity and complete current source/docs/helper contents are checked separately. Native source/header/version bytes are unchanged from6547ec4 except the owned smoke-test file; CODEX_NEXT/PROJECT_STATE and all gameplay/ownership/write gates are preserved. Documentation, packaging's required analysis list and selected independent log list are updated. SF/ordinary mods/old D/shared Overstrike caches and profiles remain untouched.

Next human action: update the Windows preparation checkout and run its reset-diagnostic.sh through configured Ubuntu-24.04 WSL, without deploying. If it passes, proceed next to full migration and then shared UI/log acceptance one step at a time; if it fails, retain the diagnostic and subsequent production records. Real-PC acceptance is not claimed from cloud tests.


## Real-PC corrected focused reset run — PASS, full migration next

The user supplied Windows PowerShell/Ubuntu-24.04 WSL output after fast-forwarding to delivery **f2be95b40ea22c8568d7c39489ba0e75461f426e** (snapshot correction implementation **79d2acff84574b7c794abe980560dad1e3e9052a**, branch codex/v01c20-edi-value-provenance). The diagnostic helper rebuilt successfully and completed **22 public PASS records /1 optional private-input SKIP**, all five repeated legacy captures, and **Reset diagnostic exit:0**. The added50ms-before-commit positive and50ms-after-commit arm-refusal cases both passed, together with the coordinator/prearm/multithread cases. No arm/capture assertion failed. This confirms the corrected focused fixture on the actual user machine. Auxiliary winedevice messages remain in that successful run; they do not invalidate the completed checks.

This diagnostic did not deploy to the game. The complete corrected migration, archive duplicate resolution in shared Overstrike, SF visibility, installation/game launch and a new actual runtime log are still unconfirmed. Do not mark the overall workspace gate complete or resume EDI/gameplay research. No raw user terminal output is committed.

This acceptance update changes only docs/CODEX_RESULT.md and analysis/V01C20_RELEASE.json. Source, helpers, tests, runtime payload, version and public release ZIPs remain at f2be95b; the ZIPs retain the documentation captured at that release. Validation for this record checks the supplied22/1 counts, all5 repetition labels, exit0, absence of fatal diagnostics, JSON parsing and existing public ZIP sizes/hashes. No executable changes require a test rerun.

Next human action: close shared Overstrike, update the Windows preparation checkout, then run its Migrate-WS.cmd for the same actual game root and development branch with Ubuntu-24.04 selected. After successful full pipeline/deployment, inspect the reopened shared Overstrike for exactly one WallSprint and ordinary mods; confirm SF visibility separately. Do not Install mods until that UI check is reviewed. Preserve the old environment.


## Real-PC corrected migration and duplicate resolution — PASS, SF visibility next

The user supplied the complete real Windows/Ubuntu-24.04 migration output after updating the preparation checkout to **f1917b1d52175173ccae8c5ef04b2bcd896efa51**. The preserved pipeline completed **139 public test PASS /5 optional private-input SKIP**, package generation/validation, disabled artifact staging, and final MSM2-WS-v0.1c20.script deployment/Overstrike launch. The terminal includes2 additional packaging/staging PASS records (141 total PASS lines), and27 optional maintainer-log absences separate from the5 private-input SKIPs. These missing cloud validation-log copies do not skip local build/test execution. No fatal helper/test error appears.

Two supplied screenshots jointly cover the shared MSM2 (Steam) list1..25 at the known game root. Ordinary mods are visible; WallSprint appears once at24 and is checked; Suits Menu follows at25. The previous24 WallSprint entries are reduced to1 on the real PC, confirming UI duplicate resolution after the archive-aware correction. SocialFeed is not identifiable anywhere in this supplied list; do not infer whether its folder or active script exists without filesystem evidence. Install mods / Run modded game and a fresh WS runtime log are still unconfirmed. The overall ordinary+SF+WS/log acceptance gate remains open; gameplay/EDI work stays paused.

This is a documentation/derived-evidence update only to docs/CODEX_RESULT.md and analysis/V01C20_RELEASE.json. No raw terminal dump/screenshots, source changes or new release binaries are committed. Public release assets remain at f2be95b (their existing sizes/hashes verified), and CODEX_NEXT/PROJECT_STATE are preserved. The local generated install ZIP reported3362929 bytes/SHA2562310334098ebeb9996f054488ac563b244fa5e9331777fd616ae0596ecb38d20; its source ZIP reported782588 bytes/SHA256a35e6852bf2c6db3400e616c65a932fbeb2a3dc8a1a4f58a5d38efc88a58f8d3. These are user-local outputs, not replacements for the public download hashes.

Next action: read-only list Mods-Developing project folders and recursively list script/script.disabled paths to identify the SF placement. WallSprint tooling must not repair/clean/mutate SF, ordinary mods or shared Overstrike profiles. Do not Install mods until the SF visibility check is resolved or the user explicitly chooses a WS-only test. Preserve the old environment.


## Shared SF+WS recognition accepted after successful SF updater

The user completed the SF repository's updater at **3ca46be4260048663ae8afc741678ffd435a8afd**: native54+40, actual CMD15, workflow87 and Core11 passed, its staging package verified and MSM2-SF-v0.1.3.script Prepared. This was SF-owned tooling, not cleanup/mutation by WS tooling. Two screenshots cover the shared MSM2 (Steam) list1..26: ordinary mods, exactly one checked SocialFeedProbe at24 and one checked WallSprint at25, followed by Suits Menu. SF visibility is now confirmed together with the previously accepted139-test WS migration and duplicate resolution.

The shared-list recognition gate passes. Install mods / Run modded game, a fresh versioned WS log and the SF log remain pending. No real game/EDI/player-signal inference or gameplay research reauthorization follows. Preserve old sources. Next: click Install mods once with the shown SF/WS selection and desired ordinary mods; review completion/error before continuing launch/log acceptance.

Only this derived acceptance record and V01C20_RELEASE metadata change. No raw user output/screenshots, native source/helper/runtime/version/ZIP changes. Existing public ZIP hashes remain at f2be95b; CODEX_NEXT and PROJECT_STATE are preserved. No test rerun is needed for documentation-only evidence updates.


## Actual shared game launch / new versioned logs — workspace gate PASS

The user reports successful game startup and visible WS operation and supplies both 20261024-180229 logs. analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md records derived evidence only. WS accepts the expected2.810.0.0 executable hash, installs the existing verified hooks and writes the canonical project log with no shared fallback. Two1594/2234ms WallRun sessions retain orientation (dot1), finish by verified Deinit with source active to exit, stale0/hard-kill0, and both watches report restored cleanup/no pending cleanup and Position writes0. Session2 finalizes8 entries/maps; this does not prove source ownership or replace the prior4s A/E acceptance. One diagnostic-log drop is explicitly writes_unaffected; no whole-process shutdown is claimed from truncated tails. EDI tracing remains disabled.

SF logs prove script_enable/module pin/worker/diagnosis and canonical new SF logging.20 summaries (18 ready) remain valid0/range_pass0 with Transform closed, so loader/logging acceptance does not establish a working player handle or a visual feed. The current probe has no visual UI. The user-confirmed launch plus both real logs completes shared installation/launch/logging acceptance after the previously passed helpers/UI. The infrastructure gate passes; no new gameplay/EDI research is authorized until CODEX_NEXT is explicitly changed. No further setup rerun is needed from this evidence.

Only analysis/docs/structured metadata change; raw logs/game bodies/process data are not committed. Source/helpers/runtime/version/public assets and CODEX_NEXT/PROJECT_STATE are preserved. Derived-summary parsing validates session counts, retention/Deinit/cleanup/Position gates and SF zero-candidate status; existing ZIP hashes remain unchanged. No binary regression rerun is required for this log review. Old sources are not deleted.
