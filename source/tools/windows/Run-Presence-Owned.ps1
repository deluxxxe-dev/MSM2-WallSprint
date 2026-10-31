param([Parameter(Mandatory=$true)][string]$SourceRoot)
$ErrorActionPreference = 'Stop'
$SourceRoot = (Resolve-Path -LiteralPath $SourceRoot).Path
$branch = 'codex/v01c20-edi-value-provenance'
$origin = & git -C $SourceRoot remote get-url origin
if ($LASTEXITCODE -ne 0 -or $origin -ne 'https://github.com/deluxxxe-dev/MSM2-WallSprint.git') { throw 'Wrong WS origin' }
$current = & git -C $SourceRoot branch --show-current
if ($LASTEXITCODE -ne 0 -or $current -ne $branch) { throw 'Wrong WS branch' }
& git -C $SourceRoot fetch origin $branch
if ($LASTEXITCODE -ne 0) { throw 'Fetch failed; no checkout/build/deploy attempted' }
$commit = & git -C $SourceRoot rev-parse "origin/$branch"
if ($LASTEXITCODE -ne 0 -or $commit -notmatch '^[0-9a-f]{40}$') { throw 'Fetched commit unavailable' }
$temp = Join-Path ([IO.Path]::GetTempPath()) ('WS-owned-code-' + [guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($temp) | Out-Null
try {
    $body = @(& git -C $SourceRoot show ($commit + ':source/tools/run_presence_owned.py'))
    if ($LASTEXITCODE -ne 0) { throw 'Pinned owned fixture runner unavailable' }
    $runner = Join-Path $temp 'run_presence_owned.py'
    [IO.File]::WriteAllText($runner, (($body -join "`n") + "`n"), (New-Object Text.UTF8Encoding($false)))
    & py -3 $runner --source $SourceRoot --commit $commit
    if ($LASTEXITCODE -ne 0) { throw 'Owned fixture failed/unavailable. Return result JSON if generated; no game operation.' }
}
finally { Remove-Item -LiteralPath $temp -Recurse -Force }
