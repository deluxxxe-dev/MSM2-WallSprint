param([string]$SourceRoot, [string]$GameRoot)
$ErrorActionPreference = 'Stop'
if (-not $SourceRoot) { $SourceRoot = Join-Path $PSScriptRoot '..\..\..' }
$SourceRoot = (Resolve-Path -LiteralPath $SourceRoot).Path
if (-not $GameRoot) { $GameRoot = Join-Path $SourceRoot '..\..\..\..' }
$GameRoot = (Resolve-Path -LiteralPath $GameRoot).Path
$branch = 'codex/v01c20-edi-value-provenance'
$remote = & git -C $SourceRoot remote get-url origin
if ($LASTEXITCODE -ne 0 -or $remote -ne 'https://github.com/deluxxxe-dev/MSM2-WallSprint.git') { throw 'Wrong WS origin' }
$current = & git -C $SourceRoot branch --show-current
if ($LASTEXITCODE -ne 0 -or $current -ne $branch) { throw 'Wrong WS branch' }
& git -C $SourceRoot fetch origin $branch
if ($LASTEXITCODE -ne 0) { throw 'Fetch failed; source unchanged' }
$commit = & git -C $SourceRoot rev-parse "origin/$branch"
if ($LASTEXITCODE -ne 0 -or $commit -notmatch '^[0-9a-f]{40}$') { throw 'Remote head unavailable' }
$exe = Join-Path $GameRoot 'Spider-Man2.exe'
$logs = Join-Path $GameRoot 'Mods Library\Mods-Developing\MSM2-WS\logs'
if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) { throw 'Game EXE missing' }
if (-not (Test-Path -LiteralPath $logs -PathType Container)) { throw 'Existing WS logs directory missing' }
$temp = Join-Path ([IO.Path]::GetTempPath()) ('WS-wallrun250-' + [guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($temp) | Out-Null
try {
    foreach ($name in @('wallrun_scalar_writer_refs_v7.py','wallrun_scalar_writer_refs_v6.py','wallrun_scalar_writer_refs_v5.py','wallrun_scalar_writer_refs.py','discover_ground_support.py')) {
        $body = @(& git -C $SourceRoot show ($commit + ':source/tools/' + $name))
        if ($LASTEXITCODE -ne 0) { throw "Pinned helper unavailable: $name" }
        [IO.File]::WriteAllText((Join-Path $temp $name), ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    }
    $stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
    $output = Join-Path $logs ("MSM2-WS-wallrun250-writers-$stamp.json")
    for ($i=1; (Test-Path -LiteralPath $output) -and $i -le 99; $i++) {
        $output = Join-Path $logs ("MSM2-WS-wallrun250-writers-$stamp-{0:D2}.json" -f $i)
    }
    if (Test-Path -LiteralPath $output) { throw 'Output collision cap reached' }

    $scriptPath = Join-Path $temp 'wallrun_scalar_writer_refs_v7.py'
    $v6Path = Join-Path $temp 'wallrun_scalar_writer_refs_v6.py'
    $v5Path = Join-Path $temp 'wallrun_scalar_writer_refs_v5.py'
    $v4Path = Join-Path $temp 'wallrun_scalar_writer_refs.py'
    $dependencyPath = Join-Path $temp 'discover_ground_support.py'
    $converted = @()
    foreach ($path in @($scriptPath,$v6Path,$v5Path,$v4Path,$dependencyPath,$exe,$output)) {
        $value = & wsl.exe --exec wslpath -a $path
        if ($LASTEXITCODE -ne 0) { throw 'WSL path conversion unavailable; no runtime fallback' }
        $converted += $value
    }

    & wsl.exe --exec /workspace/toolchains/analysis-python/bin/python3 -c 'import capstone; assert tuple(map(int,capstone.__version__.split(chr(46)))) == (5,0,6)'
    if ($LASTEXITCODE -ne 0) { throw 'Pinned WSL analysis-python/Capstone5.0.6 unavailable; no automatic install or game operation' }

    & wsl.exe --exec /workspace/toolchains/analysis-python/bin/python3 -m py_compile $converted[0] $converted[1] $converted[2] $converted[3] $converted[4]
    if ($LASTEXITCODE -ne 0) { throw 'Pinned V7 Python helper syntax preflight failed; EXE scan not started' }

    & wsl.exe --exec /workspace/toolchains/analysis-python/bin/python3 $converted[0] $converted[5] --output $converted[6]
    $scanExit = $LASTEXITCODE
    if (-not (Test-Path -LiteralPath $output -PathType Leaf)) { throw 'Offline owner-provenance report missing' }
    Write-Host "Return this small JSON only: $output"
    Write-Host "Helper source commit: $commit"
    Write-Host 'Read-only on-disk EXE provenance trace. No game launch, hook, build, package, deploy, or memory write.'
    if ($scanExit -ne 0) { throw "Offline owner-provenance trace refused; return generated JSON for exact reason: $output" }
}
finally { Remove-Item -LiteralPath $temp -Recurse -Force }
