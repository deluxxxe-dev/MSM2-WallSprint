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
$temp = Join-Path ([IO.Path]::GetTempPath()) ('WS-ground-metadata-' + [guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($temp) | Out-Null
try {
    $helper = Join-Path $temp 'discover_ground_support.py'
    $body = @(& git -C $SourceRoot show ($commit + ':source/tools/discover_ground_support.py'))
    if ($LASTEXITCODE -ne 0) { throw 'Pinned offline helper unavailable' }
    [IO.File]::WriteAllText($helper, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    $stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
    $output = Join-Path $logs ("MSM2-WS-ground-support-anchors-$stamp.json")
    for ($i=1; (Test-Path -LiteralPath $output) -and $i -le 99; $i++) {
        $output = Join-Path $logs ("MSM2-WS-ground-support-anchors-$stamp-{0:D2}.json" -f $i)
    }
    if (Test-Path -LiteralPath $output) { throw 'Output collision cap reached' }
    & py -3 $helper $exe --output $output
    $pythonExit = $LASTEXITCODE
    if ($pythonExit -ne 0) { throw "Offline discovery refused/no match/unavailable; return JSON if generated: $output" }
    Write-Host "Return this small JSON only: $output"
    Write-Host "Helper source commit: $commit"
    Write-Host 'Candidates only. No consumer/ownership proof. No game/build/deploy/runtime update.'
}
finally { Remove-Item -LiteralPath $temp -Recurse -Force }
