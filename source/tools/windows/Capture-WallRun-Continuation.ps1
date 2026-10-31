param([string]$SourceRoot,[string]$GameRoot)
$ErrorActionPreference = 'Stop'
if (-not $SourceRoot) { $SourceRoot = Join-Path $PSScriptRoot '..\..\..' }
if (-not $GameRoot) { $GameRoot = Join-Path $SourceRoot '..\..\..\..' }
$branch = 'codex/v01c20-edi-value-provenance'
$SourceRoot = (Resolve-Path -LiteralPath $SourceRoot).Path
$GameRoot = (Resolve-Path -LiteralPath $GameRoot).Path
if ((& git -C $SourceRoot remote get-url origin) -ne 'https://github.com/deluxxxe-dev/MSM2-WallSprint.git') { throw 'Wrong WS origin' }
if ((& git -C $SourceRoot branch --show-current) -ne $branch) { throw 'Wrong WS branch' }
& git -C $SourceRoot fetch origin $branch
if ($LASTEXITCODE -ne 0) { throw 'Fetch failed; source unchanged' }
$commit = & git -C $SourceRoot rev-parse "origin/$branch"
if ($LASTEXITCODE -ne 0) { throw 'Remote head unavailable' }
$exe = Join-Path $GameRoot 'Spider-Man2.exe'
if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) { throw 'Game EXE missing' }
$logs = Join-Path $GameRoot 'Mods Library\Mods-Developing\MSM2-WS\logs'
if (-not (Test-Path -LiteralPath $logs -PathType Container)) { throw 'Existing WS logs directory missing' }
$temp = Join-Path ([IO.Path]::GetTempPath()) ('WS-offline-' + [guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($temp) | Out-Null
try {
    foreach ($name in @('capture_wallrun_continuation.py','slot_publication.py')) {
        $body = @(& git -C $SourceRoot show ($commit + ':source/tools/' + $name))
        if ($LASTEXITCODE -ne 0) { throw "Pinned helper unavailable: $name" }
        [IO.File]::WriteAllText((Join-Path $temp $name), ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    }
    $stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
    $output = Join-Path $logs ("MSM2-WS-wallrun-fragment77-$stamp.json")
    for ($i=1; (Test-Path -LiteralPath $output) -and $i -le 99; $i++) { $output = Join-Path $logs ("MSM2-WS-wallrun-fragment77-$stamp-{0:D2}.json" -f $i) }
    if (Test-Path -LiteralPath $output) { throw 'Output collision cap reached' }
    $converted = @()
    foreach ($path in @((Join-Path $temp 'capture_wallrun_continuation.py'),$exe,$output)) {
        $value = & wsl.exe --exec wslpath -a $path
        if ($LASTEXITCODE -ne 0) { throw 'WSL path conversion unavailable' }
        $converted += $value
    }
    & wsl.exe --exec /workspace/toolchains/analysis-python/bin/python3 $converted[0] $converted[1] --output $converted[2] --complete-fragment
    if ($LASTEXITCODE -ne 0) { throw "Offline capture refused/unavailable; return small JSON if generated: $output" }
    Write-Host "Return this small JSON only: $output"
    Write-Host 'Source worktree, runtime, installed KIT and game unchanged; no build/deploy/game launch.'
}
finally { Remove-Item -LiteralPath $temp -Recurse -Force }
