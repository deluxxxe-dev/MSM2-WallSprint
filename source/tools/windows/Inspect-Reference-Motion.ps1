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
$downloads = Join-Path ([Environment]::GetFolderPath('UserProfile')) 'Downloads'
$library = Join-Path $GameRoot 'Mods Library'
$logs = Join-Path $GameRoot 'Mods Library\Mods-Developing\MSM2-WS\logs'
if (-not (Test-Path -LiteralPath $logs -PathType Container)) { throw 'Existing WS logs directory missing' }
$temp = Join-Path ([IO.Path]::GetTempPath()) ('WS-reference-motion-' + [guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($temp) | Out-Null
try {
    foreach ($name in @('reference_motion_evidence.py','ground_support_code_refs.py','discover_ground_support.py')) {
        $body = @(& git -C $SourceRoot show ($commit + ':source/tools/' + $name))
        if ($LASTEXITCODE -ne 0) { throw "Pinned helper unavailable: $name" }
        [IO.File]::WriteAllText((Join-Path $temp $name), ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    }
    $stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
    $output = Join-Path $logs ("MSM2-WS-reference-motion-refs-$stamp.json")
    for ($i=1; (Test-Path -LiteralPath $output) -and $i -le 99; $i++) {
        $output = Join-Path $logs ("MSM2-WS-reference-motion-refs-$stamp-{0:D2}.json" -f $i)
    }
    if (Test-Path -LiteralPath $output) { throw 'Output collision cap reached' }
    $converted = @()
    foreach ($path in @((Join-Path $temp 'reference_motion_evidence.py'),$downloads,$library,$output)) {
        $value = & wsl.exe --exec wslpath -a $path
        if ($LASTEXITCODE -ne 0) { throw 'WSL path conversion unavailable; no runtime fallback' }
        $converted += $value
    }
    & wsl.exe --exec /workspace/toolchains/analysis-python/bin/python3 -c 'import capstone; assert tuple(map(int,capstone.__version__.split(chr(46)))) == (5,0,6)'
    if ($LASTEXITCODE -ne 0) { throw 'Pinned WSL analysis-python/Capstone5.0.6 unavailable; no automatic install or game operation' }
    & wsl.exe --exec /workspace/toolchains/analysis-python/bin/python3 $converted[0] --root $converted[1] --root $converted[2] --output $converted[3]
    if ($LASTEXITCODE -ne 0) { throw "Reference motion evidence unavailable/refused/no relation; return JSON if generated: $output" }
    Write-Host "Return this small JSON only: $output"
    Write-Host "Helper source commit: $commit"
    Write-Host 'Reference DLL candidates only, NOT game RVAs. No EXE/live read/build/deploy/game launch.'
}
finally { Remove-Item -LiteralPath $temp -Recurse -Force }
