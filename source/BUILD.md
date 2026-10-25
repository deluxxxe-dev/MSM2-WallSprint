# Supported build and final local workspace

Windows-local development uses **Windows Python 3.11+, Git for Windows, built-in Windows PowerShell and WSL2 x86-64 Debian/Ubuntu**. The supported compiler/test chain remains the existing Linux cross-build: MinGW GCC14 POSIX, Clang19 SEH, Capstone5.0.6, Wine and the analysis Python environment. Native MSVC/CMake is not an equivalent replacement; CMakeLists directs to build_windows.sh. Missing tools fail clearly and never skip validation.

Prepare WSL2 and the pinned toolchain outside Mods Library before migration. From a temporary checkout/source extraction in WSL:

```bash
# /workspace must be writable by the WSL user; create/chown it deliberately
# with the distro's normal administrative procedure if needed.
cd "/path/to/temporary/WallSprint/source"
WALLSPRINT_WITH_WINE=1 bash tools/setup_cloud.sh
bash tools/setup_analysis.sh
```

The existing scripts expect the configured `/workspace/toolchains/windows`, `analysis-python`, `wine` and `wine-prefix`. Required host tools include Bash, Python (`python`), g++, make, objdump, tar, curl and sha256sum. A configured default WSL2 distribution is used; `MSM2_WS_WSL_DISTRO` explicitly selects another configured distribution. The wrapper verifies WSL2 and every required executable. Tool setup may require normal Linux package/network prerequisites; the updater does not auto-install a distribution or fall back to a weaker build.

The unchanged required serial validation flow below assumes the full Git checkout root. For the standalone source ZIP, run from its `source` folder and omit the leading `source/` in these commands:

The required flow is:

```bash
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python source/tools/release.py
```

`source/tools/windows/workspace-build.sh` stages source/docs to an owned temporary WSL tree outside Mods Library, executes all commands above and returns validated disabled script/manifest/ZIP outputs. It never builds/unpacks a live `.script` into the authoritative `_source`. Source checkout is an independent Git clone with a managed non-cone sparse checkout excluding ZIP/RAR/7z working-tree paths; all source files and historical archive blobs remain in Git. The update requires clean non-detached intended branch/upstream and `git pull --ff-only`. Branch is supplied explicitly during migration and stored in local Git config `wallsprint.expectedBranch`; an intentional future branch switch requires `Update-WS.cmd --expected-branch <new-development-branch>` and a matching upstream.

On a fresh toolchain, `smoke_windows.sh` also prepares Wine's relocated server, built-in DLLs and the NLS link `usr/share/wine/wine -> .`. The link is created directly under `usr/share/wine`; its destination directory must not be repeated. A pre-existing cloud cache can hide this first-run error, so the bootstrap follow-up also checks an absent link and a fresh isolated Wine prefix. Cloud bootstrap checks do not replace real Windows/WSL acceptance.

Build/test source must be on a case-sensitive Linux filesystem. On the default Windows `/mnt/c` filesystem, `tools/compat/Windows.h` can also match `<windows.h>` and suppress the real system header; errors such as unknown `VOID`/`LPVOID` follow. The normal workspace pipeline already stages to Linux `/tmp` before compiling.

For a legacy reset-watch fixture failure, run `bash tools/windows/reset-diagnostic.sh` from the preparation checkout's `source` directory. It may be invoked from `/mnt/c`: the helper derives that source directory, copies it without build/log/output caches into its own Linux `/tmp/msm2-ws-reset-diagnostic.*` tree, and runs the unchanged build and focused reset suite there. The printed diagnostic directory and its logs are retained on success/failure. It never deploys to the game. A rejected arm prints `RESET_SMOKE_ARM_FAILURE` with session, elapsed commit time, admission/identity state and allocated-window counts, followed by the production arm/skip records. A failed first capture prints `RESET_SMOKE_FAILURE` with the actual arm/store times, hit/drop counts, deadline and stop reason before retaining the original fatal assertion. This is diagnostic evidence only: the real 30ms deadline and all required migration tests remain enforced. The helper also repeats the unchanged short-window executable five times after the focused suite to investigate intermittent admission failures. Combined stdout/stderr are retained in `logs/reset-diagnostic-console.log`; the diagnostic folder and exit code are printed even on failure. The legacy positive fixture prepares a real OS thread snapshot before the synthetic commit QPC; enumeration/filtering and real hardware capture still use production code and the real30ms deadline. Required50ms controls exercise both slow preparation before commit and delayed snapshot delivery after commit (which must refuse). Prearmed/process-expansion tests retain real snapshot acquisition. See analysis/RESET_SMOKE_SNAPSHOT_TIMING.md. A focused PASS does not complete migration or real-PC acceptance.

The final path and update/migration protocol are in docs/LOCAL_WORKSPACE.md. First run the complete release's `Migrate-WS.cmd --game-root "<actual game root>" --branch codex/v01c20-edi-value-provenance` outside Mods Library after preparing the shared `<game>/Overstrike/Overstrike.exe`, root scripts_proxy.dll and root winmm.dll. Afterwards use only the installed `MSM2-WS/Update-WS.cmd`. No personal C-/D-drive path, legacy clone link or AppData Overstrike copy is embedded.

Capstone5.0.6 is fetched from its official source archive and SHA256-checked against `240ebc834c51aae41ca9215d3190cc372fd132b9c5c8aa2d5f19ca0c325e28f9`. Full x86 decode, no diet/reduced ISA; no external Capstone DLL. Third-party notices remain in vendor/capstone and THIRD_PARTY_NOTICES. Do not copy reference mod source/binaries.

The full hooks suite retains c7-c19 lifecycle/orientation/stale/publication/ABI/DR/entry/call/alias/finalization and cleanup tests. C20 local EDI provenance is accepted from the authenticated actual-game log. C21 adds a bounded diagnostic-only read-dependency sidecar and eight owned fixture groups; current read/order/flow classification awaits the new A/E log, and the existing effect refusal remains intact. `test-workspace.py` adds real owned Git/filesystem migration/update/rollback/isolation fixtures. Logger smoke uses actual Windows filesystem APIs under Wine and new versioned workspace log names; no raw user logs are packaged.

Optional private historical replay remains optional rather than a fake PASS:

```bash
WALLSPRINT_PATH_B_CAPTURE=/private/accepted-capture.log bash source/tools/test_compact_proof.sh
```

Never place private game bodies/logs under Git or release inputs. This historical444/128byte replay is not the current137byte callee or183byte writer.

Public outputs outside Mods Library are `MSM2-WS-v<version>.zip`, optional `MSM2-WS-v<version>-source.zip`, and non-active `MSM2-WS-v<version>.script.disabled`/BUILD_MANIFEST. Overstrike recursively scans archive contents, so the pipeline returns those ZIPs to WS `_archive/_build` only with a `.zip.disabled` suffix. The workspace validates that zero visible ZIP/RAR/7z remain recursively before deployment/launch. Direct `release.py` under Mods Library refuses before work. The install ZIP contains one virtual versioned active script member; source ZIP contains none. Test source ZIP extraction only outside Mods Library; never unpack the install ZIP into `_source`. Disabled intermediates and old archived scripts are not Overstrike-active. Normal ZIPs are Overstrike-visible even without extraction; plain public archives must stay outside Mods Library. The updater preserves clean tracked source archive blobs/history through Git sparse checkout and disables only positively validated old WS build-cache ZIPs. See analysis/OVERSTRIKE_ARCHIVE_DISCOVERY.md for the real24-entry failure and independent regression model. Full source/tests/licenses/docs are included for isolated reproduction.

Cloud validation does not complete the user's real migration. Windows process stopping, WSL installation, real shared Overstrike recognition of ordinary mods + SF + WS, actual runtime logging and gameplay A/E remain local acceptance checks. The old environment is never automatically deleted.

## Current c22 bounded diagnostic

v0.1c22/package0.1.40 extends only the authenticated pinned TEST read/flag sidecar.
`bash tools/test_read_dependency.sh` includes the8 c21 groups and8 c22 groups;
`test_hooks_windows.sh` retains this suite and all existing c19/c20/native/workspace
regressions. Low-byte TEST is read-only/conservative parent taint, never a partial
register write model or a writer/effect certificate. See analysis/V01C22_READ_TEST_FLOW.md.
Actual TEST operands/next blocker still require the shortest normal A/E game log.

## Current c23 call diagnostic

v0.1c23/package0.1.41 retains the full chain. test_read_dependency.sh now
includes16 c21/c22 groups plus5 call groups. Only a single authenticated
register-only leaf can continue (128bytes/32instructions/depth1); unknown
calls and any memory/branch/further-call effect stop. Actual CALL metadata
requires the new compact READ_CALL log, not the old TEST capture. See
analysis/V01C23_READ_CALL_FLOW.md; all prior game-write gates remain.

## Current c24 identity diagnostic

v0.1c24/package0.1.42 adds test_read_target_identity.sh (9 owned sparse PE/code
groups), called by the full hooks suite after the21 retained read/call groups.
Only target0x2CF7FE0 can attempt the strict no-entry linear proof; original
128byte/32instruction caps and old effect gates remain. Any branch/memory/stack/
nested CALL, code/runtime crossing or unstable snapshot refuses. Actual reason/
span/hash still needs the new compact TARGET_IDENTITY record.

## Current c25 metadata validation

v0.1c25/package0.1.43 adds6 large-table groups to test_read_target_identity.sh
(15 total), retaining21 read/call groups and the full pipeline. Explicit
600000-entry/8MiB/64-entry-chunk bounds cover observed527161 without unbounded
allocation. Complete table checks precede actual lookup and unchanged c24
no-unwind proof. Cap and default lookup fields never imply corruption/absence.
See analysis/V01C25_RUNTIME_TABLE_BUDGET.md.

## Current c26 pinned CMP classification

v0.1c26/package0.1.44 adds6 CMP/next-boundary groups to test_read_target_identity.sh
(21 total), retaining21 read/call groups and full native pipeline. Complete c25
metadata validation gates remain. Only pinned decoded32/64-bit GPR CMP can add
reg/immediate flags-only effects; partial/memory/branch/stack/call forms still
refuse. Compact prefix metadata is not full target identity or an effect waiver.
See analysis/V01C26_CMP_FLAGS.md. Actual operands/next boundary pending; no raw
private game byte fixture required or distributed.

## Current c27 bounded JA proof

v0.1c27/package0.1.45 adds7 groups to test_read_target_identity.sh (28 total),
retaining21 read/call groups and the full pipeline. Exact pinned short/near JA
only, current call tag-disjoint gate,1 split/2 paths/128bytes/32 unique decoded
instructions/64 visits; all RET outcomes required. Final code/runtime checks,
per-state tag recheck and conservative effects preserve fail-closed boundaries.
See analysis/V01C27_JA_FANOUT.md. Actual edges/paths need minimal live metadata,
never a private game body fixture. Existing Overstrike recovery helpers retained.

## Current c28 diagnostic-only MOV classification

v0.1c28/package0.1.46 adds8 groups to test_read_target_identity.sh (36 total),
retaining21 call/read and full pipeline. No memory-load semantics: exact pinned
MOV operand/access/address/alias/tag metadata only, write/RMW/unproven-value
refusals and unchanged c27 all-path/cap/metadata/current-code rules. Address
provenance and loaded-value provenance remain separate; unknown never means0.
See analysis/V01C28_MOV_CLASSIFICATION.md; no private body/data fixture required.

## c29 exact image-load proof

v0.1c29/package0.1.47 adds8 owned groups to test_read_target_identity.sh
(44 total), retaining21 read/call groups and all c19-c28/native regressions.
Covers PE section/range/flags, direct loader-directory overlap, current page
allocation/type/state/protection, source and metadata/code equality, guarded
first/second read failures, current caller address/tag proof, exact full64 RAX
replacement, preserved flags/other registers, and next-barrier/unsafe-leg refusal.
No private game bytes/value fixture needed; details analysis/V01C29_IMAGE_LOAD_PROOF.md.
