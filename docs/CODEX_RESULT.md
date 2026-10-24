# Codex result — final local WS workspace migration

Branch: **codex/v01c20-edi-value-provenance**.
Implementation commit: **a4ec9c0f2635e9f2594bf5972d1765332de73766**.
Latest instruction base: **4402729a53eceeede6476c24d814bfd8469978fa**, merged without history rewrite. Prepared c20 research is preserved in **9057d24** and remains production-disabled; accepted c19 implementationb3185cd/deliveryba0b1d7/runtime evidence retained. No main changes/force push. CODEX_NEXT is preserved from the latest infrastructure task; AGENTS/DEV_RULES/LOCAL_WORKSPACE/LOGGING_CONVENTION/PROJECT_STATE updated as explicitly authorized.

## Outcome

**v0.1c20 / package0.1.38 implements final local migration/update/logging infrastructure. Cloud build/regressions/owned workspace fixtures pass. Real Windows migration, WSL prerequisites, process stopping, shared Overstrike UI recognition and actual runtime logs remain PENDING user execution.**

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
