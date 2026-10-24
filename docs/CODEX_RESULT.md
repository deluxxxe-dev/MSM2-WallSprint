# Codex result — final local WS workspace migration

Branch: **codex/v01c20-edi-value-provenance**.
Implementation commit: **a4ec9c0f2635e9f2594bf5972d1765332de73766**.
Latest instruction base: **4402729a53eceeede6476c24d814bfd8469978fa**, merged without history rewrite. Prepared c20 research is preserved in **9057d24** and remains production-disabled; accepted c19 implementationb3185cd/deliveryba0b1d7/runtime evidence retained. No main changes/force push. CODEX_NEXT is preserved from the latest infrastructure task; AGENTS/DEV_RULES/LOCAL_WORKSPACE/LOGGING_CONVENTION/PROJECT_STATE updated as explicitly authorized.

## Outcome

**v0.1c20 / package0.1.38: the old real-PC helper pipeline passed131 tests, but shared Overstrike showed24 WallSprint entries, blocking UI acceptance. The archive-aware correction passes136 cloud public tests /5 optional private-input SKIP and now requires zero scanner-visible archives. Corrected real-PC migration/UI, SF visibility, game installation and a new runtime log remain PENDING.**

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
