param([Parameter(Mandatory=$true)][string]$GameRoot,[switch]$DiagnoseWSL)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($GameRoot)
$repo=Join-Path $root 'Mods Library\Mods-Developing\MSM2-WS\_source'
$branch='codex/v01c20-edi-value-provenance'
if ((& git -C $repo branch --show-current) -ne $branch -or $LASTEXITCODE -ne 0) { throw 'Wrong/missing WS branch' }
if ((& git -C $repo remote get-url origin) -ne 'https://github.com/deluxxxe-dev/MSM2-WallSprint.git' -or $LASTEXITCODE -ne 0) { throw 'Wrong WS origin' }
# Fetch updates refs only; checked ff/repair occurs while OS ownership is held.
& git -C $repo fetch origin $branch
if ($LASTEXITCODE -ne 0) { throw 'Fetch failed' }
$commit=(& git -C $repo rev-parse ("origin/"+$branch))
if ($LASTEXITCODE -ne 0) { throw 'Fetched head unavailable' }
$temp=Join-Path ([IO.Path]::GetTempPath()) ('MSM2-tools-bootstrap-'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $temp | Out-Null
try {
 foreach ($name in @('operation_lock.py','repair_tools.py')) {
  $lines=@(& git -C $repo show ($commit+':environment/'+$name))
  if ($LASTEXITCODE -ne 0) { throw ('Bootstrap source missing: '+$name) }
  [IO.File]::WriteAllText((Join-Path $temp $name), (($lines -join "`n")+"`n"), (New-Object Text.UTF8Encoding($false)))
 }
 $engine=Join-Path $temp 'repair_tools.py'
 $probeArgs=@();if($DiagnoseWSL){$probeArgs=@('--diagnose-wsl')}
 if (Get-Command py -ErrorAction SilentlyContinue) { & py -3 $engine --source $repo --game-root $root --commit $commit @probeArgs }
 else { & python $engine --source $repo --game-root $root --commit $commit @probeArgs }
 if ($LASTEXITCODE -ne 0) { throw 'Tooling action refused; runtime retained. See cause-specific WSL/ownership evidence above.' }
} finally { Remove-Item -LiteralPath $temp -Recurse -Force }
