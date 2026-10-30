# Codex result — scripts_proxy installer dependency validation

2026-10-31 JST. Branch `codex/v01c20-edi-value-provenance`.
Baseline 1a1bce2415f7349092623822553970d80f3d2e11.

## STATUS / changed

success — scoped checker correction; real-PC Update-WS success unverified.

Changed: source/tools/workspace.py and test-workspace.py;
environment/mod_environment.py and test_mod_environment.py;
docs/DEV_RULES.md, LOCAL_WORKSPACE.md, mod-tools README/OVERSTRIKE/PLAY_MODDED and
combined MANUAL.md, environment/README.md, PROJECT_STATE and this result;
analysis/SCRIPTS_PROXY_INSTALLER_DEPENDENCY_20261031.md; generated distribution/
release metadata and ZIPs. AGENTS contains no equivalent blanket ban to change.

## Confirmed behavior and limits

Public Overstrike OS/v1.8.1 tag (5829f3b045397ed19a1c3f282de504ba57197b7b),
project Assembly/FileVersion1.8.1.0, MetaInstallerBase.SetupScripts: source
File.Copy("scripts_proxy.dll", Path.Combine(_gamePath,"winmm.dll")) is process-CWD
relative, not a universal absolute exe-adjacent contract. Owner exception located
that source beside Overstrike.exe. Public source was read with
`git -C /tmp/ws-steering-overstrike-review show OS/v1.8.1:Overstrike/MetaInstallers/MetaInstallerBase.cs`
and project version inspected. No owner EXE binary hash authenticated here.
Owner later log loaded v0.1c38, not proof of r1 or a separate Install-success log.

Both WS updater and shared environment checker had the same blanket scripts_proxy
ban. They now allow an OPTIONAL existing installer source beside shared manager,
requiring regular file/non-reparse. Missing optional source remains compatible
with pre-existing modes. Existing required root proxy/winmm inputs and all unrelated
checks remain. Misplaced manager-side winmm is still refused, including dangling
links; existing Start CMD quarantine/ReadOnly policy is unchanged. No new DLL
copy/download/delete, profile/settings write or source/root hash equality gate.
Source vs game-loader distinction supersedes older duplicate-DLL guidance.
Detailed durable evidence is in the new WS analysis. Wisdoms shared-environment
rules read only; synchronization remains pending to keep this patch bounded.

## Validation: exact commands, expected and actual

| Command | Expected | Actual |
| --- | --- | --- |
| `python3 source/tools/test-workspace.py` | owned Git/Unicode/space fixtures; valid installer proxy accepted; misplaced winmm, missing/wrong-type required files, proxy directory/live/dangling symlink refuse; old WS/SF/unrelated data preserved | exit0; 13 PASS groups |
| `python3 environment/test_mod_environment.py` | shared Start/Play accept distinct regular proxy without mutation; unsafe proxy/missing/type/root-loader/winmm negatives; existing backup/restore/SF isolation | exit0; 28 unittest tests, OK |
| `python3 environment/test_start_overstrike_preflight.py` | existing winmm-only quarantine/root ReadOnly/lock boundaries unchanged | exit0; PASS |
| `XDG_CONFIG_HOME=/tmp/ws-proxy-pwsh/settings XDG_CACHE_HOME=/tmp/ws-proxy-pwsh/cache XDG_DATA_HOME=/tmp/ws-proxy-pwsh/data DOTNET_CLI_HOME=/tmp/ws-proxy-pwsh /tmp/ws-proxy-pwsh/pwsh -NoProfile -File environment/test_windows_contract.ps1` | existing Windows helper syntax and owned fake process/shortcut behavior | exit0; 16 checks, PowerShell7.4.6/Linux; not actual Windows COM/processes |
| `python3 environment/build_distribution.py` | toolkit-only ZIP/hash/allowlist valid | exit0 |
| `python source/tools/release.py` | existing r1 package/CRC/version consistency; no DLL change | exit0; existing r1 package/CRC checks pass |
| Bootstrap block parser: PowerShell Language.Parser.ParseFile on `/tmp/ws-proxy-bootstrap.ps1` (same XDG environment) | valid syntax; do not execute owner Git commands | exit0; PASS |
| `git diff --check`; `git diff --cached --check` | clean diff | exit0 |

During implementation the first draft called existing linked()/lstat on absent
optional files, yielding FileNotFoundError (workspace and environment tests).
Corrected to presence-aware lexists logic; all final suites above re-run and pass.
PowerShell initially failed initializing read-only host-home cache/data paths;
portable official7.4.6 runtime was unpacked under /tmp and XDG config/cache/data
redirected there. Final helper check passed without modifying product code or
host-home permissions. No unsafe workaround or suppressed test failure.

SKIP: unrelated native/reversal research/build chain not rerun for checker-only
changes; runtime/C++/version/DLL and root loader bytes unchanged, C52 binding.
UNAVAILABLE: real Windows Update-WS/Overstrike Install and owner game outcome;
Linux fake-launch PowerShell checks do not establish those. No user DLLs,
profiles, SF or ordinary mods were modified in cloud tests.

## Updater order / exact one-time owner bootstrap

workspace.update -> workspace_paths -> game_preflight occurs BEFORE its git pull.
Thus the old checker can stop without fetching this correction. Following is a
manual Git code refresh, not a validation bypass. It refuses dirty/detached/wrong
branch and uses ff-only (divergence refuses); no reset/clean/main merge/DLL change.
The existing normal updater subsequently retains all checks. ChatGPT presents
owner steps; no local deployment was executed here.

```powershell
& {
    $ErrorActionPreference = 'Stop'
    $wsSource = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\Mods Library\Mods-Developing\MSM2-WS\_source"
    $wsBranch = 'codex/v01c20-edi-value-provenance'
    $current = git -C $wsSource branch --show-current
    if ($LASTEXITCODE -ne 0 -or $current -ne $wsBranch) { throw 'Unexpected or detached branch; preserve current checkout.' }
    $pending = git -C $wsSource status --porcelain
    if ($LASTEXITCODE -ne 0 -or $pending) { throw 'Preserve local changes before updating; nothing was reset.' }
    git -C $wsSource fetch origin $wsBranch
    if ($LASTEXITCODE -ne 0) { throw 'Fetch failed; stop.' }
    git -C $wsSource merge --ff-only FETCH_HEAD
    if ($LASTEXITCODE -ne 0) { throw 'Fast-forward refused; preserve divergence and stop.' }
}
```

The Git arguments and order were reviewed against existing fixture-proven Git
operations; the owner-specific command was not run against their C: drive.
Installed Mod-Tools may also contain the old environment checker: its existing
supported refresh entry is `_source/environment/Start-Overstrike.cmd` (KIT byte
comparison already refreshes changed engine bytes at the same tool version).
That copies only toolkit files, never DLLs. Do not remove manager scripts_proxy
or rename the game loaders to get past validation.

## Publication / runtime

Runtime v0.1c38r1/package0.1.57 unchanged. Runtime DLL and disabled script were
compared byte-for-byte with the baseline Git script payload, with source/include/
mod and native hooks/launcher helper diff checked empty. All three distribution
ZIP CRC checks pass. Implementation commit `68a0e10f89a1fac94f81067a68610a4d4a2f18e2` was normally
pushed. Independent GitHub fetch into `/tmp/ws-steering-docs-publication` verified
the remote head and all18 changed source/docs/metadata/ZIP blobs byte-identical.
Packaged environment checker and workspace checker were additionally compared
with the checkout. Exact publication procedure:

```bash
git -C /tmp/ws-steering-docs-publication fetch origin codex/v01c20-edi-value-provenance
git -C /tmp/ws-steering-docs-publication rev-parse FETCH_HEAD
git diff --name-only 1a1bce2 HEAD
```

For every changed path, read `git -C /tmp/ws-steering-docs-publication show FETCH_HEAD:<path>`
and compare local bytes. Actual all18 matched, exit0. This final record-only
follow-up refreshes packaged documentation and receives the same verification.
Final record SHA is in handoff to avoid a self-referential hash here. No claim of real-PC
updater/game success. Bootstrap above is the only required one-time checker
refresh; no new gameplay design or repeated-install loop.
