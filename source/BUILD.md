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

The full hooks suite retains c7-c19 lifecycle/orientation/stale/publication/ABI/DR/entry/call/alias/finalization and cleanup tests. Prepared EDI owned tests remain, with production research disabled pending reauthorization. `test-workspace.py` adds real owned Git/filesystem migration/update/rollback/isolation fixtures. Logger smoke uses actual Windows filesystem APIs under Wine and new versioned workspace log names; no raw user logs are packaged.

Optional private historical replay remains optional rather than a fake PASS:

```bash
WALLSPRINT_PATH_B_CAPTURE=/private/accepted-capture.log bash source/tools/test_compact_proof.sh
```

Never place private game bodies/logs under Git or release inputs. This historical444/128byte replay is not the current137byte callee or183byte writer.

Public outputs outside Mods Library are `MSM2-WS-v<version>.zip`, optional `MSM2-WS-v<version>-source.zip`, and non-active `MSM2-WS-v<version>.script.disabled`/BUILD_MANIFEST. Overstrike recursively scans archive contents, so the pipeline returns those ZIPs to WS `_archive/_build` only with a `.zip.disabled` suffix. The workspace validates that zero visible ZIP/RAR/7z remain recursively before deployment/launch. Direct `release.py` under Mods Library refuses before work. The install ZIP contains one virtual versioned active script member; source ZIP contains none. Test source ZIP extraction only outside Mods Library; never unpack the install ZIP into `_source`. Disabled intermediates and old archived scripts are not Overstrike-active. Normal ZIPs are Overstrike-visible even without extraction; plain public archives must stay outside Mods Library. The updater preserves clean tracked source archive blobs/history through Git sparse checkout and disables only positively validated old WS build-cache ZIPs. See analysis/OVERSTRIKE_ARCHIVE_DISCOVERY.md for the real24-entry failure and independent regression model. Full source/tests/licenses/docs are included for isolated reproduction.

Cloud validation does not complete the user's real migration. Windows process stopping, WSL installation, real shared Overstrike recognition of ordinary mods + SF + WS, actual runtime logging and gameplay A/E remain local acceptance checks. The old environment is never automatically deleted.
