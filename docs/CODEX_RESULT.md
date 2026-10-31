# Codex result — one bounded offline WallRun continuation acquisition

2026-10-31 JST. Branch codex/v01c20-edi-value-provenance; baseline e0b626b.
STATUS: helper/wrapper prepared and focused checks pass; actual owner acquisition
UNAVAILABLE (no authenticated original EXE/capture in this environment).
No runtime version/build/gameplay change; v0.1c38r6/package0.1.62 bytes unchanged.

## CHANGED / result boundary

New source/tools/capture_wallrun_continuation.py and Windows entrypoint
source/tools/windows/Capture-WallRun-Continuation.ps1; matching Python/PowerShell
focused tests; analysis/WALLRUN_CONTINUATION_OFFLINE_ACQUISITION.md and PROJECT_STATE.
Reuse existing exact-build SHA + PE/range/pinned decoder primitives. ONLY original
file RVA0x14248f9,max128 bytes, section/runtime/file-backed bounds and first control
boundary. Metadata/unwind conflicts refuse; no entry is a fact, not invented span.
No scan/follow/live read, guessed API/ABI or write certificate. JSON contains only
this bounded code window/provenance and bounded covering unwind metadata.

Wrapper fetches/stages fresh pinned helpers before old updater checks, without
changing source worktree/local changes/runtime/KIT. No full WS build/tests/package,
new diagnostics/game test, token recovery, root build, source reset, dependencies
installation or SF/profile/loader mutation. Existing output and EXE preserved.
Output collision suffix + exclusive create; only owned TEMP staging removed.
No owner data/raw EXE/function/log added to Git or distribution.

## Exact validation / expected / actual

| Command | Expected | Actual |
|---|---|---|
| `/workspace/toolchains/analysis-python/bin/python3 source/tools/test_wallrun_continuation.py` | wrong SHA/build; invalid RVA/cap; executable/section edge/raw/truncated PE; covering/absent/future runtime bounds; invalid/chained unwind; bounded first-call/RET/128-byte output; decode gap; CLI refusal/EXE/output preservation | 8 PASS /0 SKIP /0 UNAVAILABLE |
| PowerShell command below | space/Unicode layout derivation, Git-only fresh helper staging, output collision and local source/EXE preservation; unavailable WSL refusal | 2 PASS groups /0 SKIP /0 UNAVAILABLE, MOCK transport only |
| `python3 -m py_compile source/tools/capture_wallrun_continuation.py source/tools/test_wallrun_continuation.py` | valid Python | exit0 |
| `git diff --check` | no whitespace errors | exit0 |

```bash
XDG_CACHE_HOME=/tmp/ws-lock-xdg-cache XDG_CONFIG_HOME=/tmp/ws-lock-xdg-config XDG_DATA_HOME=/tmp/ws-lock-xdg-data DOTNET_CLI_HOME=/tmp/ws-lock-dotnet /tmp/ws-proxy-pwsh/pwsh -NoProfile -File source/tools/windows/test_capture_wallrun_continuation.ps1
```

First PowerShell invocation without XDG settings failed before script parsing:
read-only /home/agent cache. Set only task-specific XDG/DOTNET paths under /tmp;
rerun passed. No user home/security settings changed or executable assertion skipped.
Focused fake PE tests patch expected SHA ONLY in test process; CLI never offers
hash/RVA/cap override. Synthetic accepted fixtures are not original-game evidence.
Actual Windows PowerShell5.1/WSL transport + original EXE extraction UNAVAILABLE,
not PASS. No game EXE/target capture found by bounded filename search under selected
workspace/tmp. BLOCKED0; SKIP0 focused. Native/portable/smoke/hooks/release pipeline
NOT RUN: offline helper/docs only, no runtime bytes changed; task explicitly forbids
new runtime rebuild/unchanged game test. Existing PE/decoder modules unchanged.

## NEED — ONE copy/paste action (Chat owner handoff)

This fetches Git refs/objects and stages helpers in TEMP, NOT source worktree/KIT
update or runtime deploy. Existing WSL analysis Python/Capstone5.0.6 prerequisite
is already established by owner setup. No full build/test/package/game launch.
Return only newly generated small JSON at the printed WS logs path; REFUSED JSON
also useful. Missing WSL/decoder produces explicit error, no repair/install fallback.
Do not upload the EXE or rerun unchanged r6 game comparison.

```powershell
$game = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2"
$repo = Join-Path $game 'Mods Library\Mods-Developing\MSM2-WS\_source'
$branch = 'codex/v01c20-edi-value-provenance'
git -C $repo fetch origin $branch
if ($LASTEXITCODE -ne 0) { throw 'Fetch failed' }
$body = @(git -C $repo show ("origin/$branch" + ':source/tools/windows/Capture-WallRun-Continuation.ps1'))
if ($LASTEXITCODE -ne 0) { throw 'Offline entrypoint unavailable' }
$temp = Join-Path $env:TEMP ('WS-continuation-' + [guid]::NewGuid().ToString('N') + '.ps1')
[IO.File]::WriteAllText($temp, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
try { & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $temp -SourceRoot $repo -GameRoot $game }
finally { Remove-Item -LiteralPath $temp -Force }
```

Output: `Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-wallrun-continuation-YYYYMMDD-HHMMSS.json`
(or -01..-99 suffix). This is the one bounded missing action, not broad investigation.
Commit/publication verification recorded below after push.

## Shared knowledge / unchanged runtime verification

Wisdoms operational note published at
`c6e8dd7d1abf143073a784de31e0d6c93c94f203`; remote main SHA readback matches.
Baseline full SHA: `e0b626b01baa5e6ee89baff67bdda4e60c67487e`.
`git diff --exit-code e0b626b -- source/include source/src source/mod environment BUILD_MANIFEST.json 'MSM2-WS-v0.1c38r6*'`
expected unchanged native/math/INI/shared tools/runtime/artifacts; actual exit0.
No new package/output ZIP/version: only the future private continuation JSON.

## Publication verified — offline helper

Implementation commit `2ae748985cabe49f0ca614662904d7f1a84884e3` pushed to
`codex/v01c20-edi-value-provenance`. Independent GitHub fetch, FETCH_HEAD, local
HEAD and ls-remote agree; all7 changed blobs match checkout bytes using
`python3 /tmp/ws-offline-publication.py`. Runtime/artifacts remain r6 unchanged.
This follow-up records publication only; focused tests are not repeated because
no executable/tested bytes changed. Five original untracked files preserved,
no main merge/reset/force/clean. Owner actual extraction still UNAVAILABLE.
Chat's ONE fresh-bootstrap block in CODEX_RESULT obtains only small private JSON;
no Update-WS/full rebuild or unchanged game test required by this task.
