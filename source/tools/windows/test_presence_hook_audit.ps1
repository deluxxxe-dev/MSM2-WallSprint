$ErrorActionPreference='Stop'
$toolRoot=Split-Path $PSScriptRoot -Parent
$fixture=Join-Path ([IO.Path]::GetTempPath()) ('WS-ground-test-'+[guid]::NewGuid().ToString('N'))
$game=Join-Path $fixture 'game テ space'
$source=Join-Path $game 'Mods Library/Mods-Developing/MSM2-WS/_source'
$logs=Join-Path $game 'Mods Library/Mods-Developing/MSM2-WS/logs'
$script=Join-Path $PSScriptRoot 'Audit-Presence-Hooks.ps1'
$global:fetches=0;$global:pythonFailed=$false;$global:badDecoder=$false
function global:git {
    $global:LASTEXITCODE=0
    switch($args[2]) {
        'remote' { 'https://github.com/deluxxxe-dev/MSM2-WallSprint.git' }
        'branch' { 'codex/v01c20-edi-value-provenance' }
        'fetch' { $global:fetches++ }
        'rev-parse' { '1111111111111111111111111111111111111111' }
        'show' { Get-Content -LiteralPath (Join-Path $toolRoot (Split-Path ($args[3].Split(':')[1]) -Leaf)) }
        default { throw 'Unexpected Git action' }
    }
}
function global:Get-Date { param([string]$Format) '20300101-000000' }
function global:wsl.exe {
    $global:LASTEXITCODE=0
    if($global:pythonFailed){$global:LASTEXITCODE=7;return}
    if($args[1] -eq 'wslpath'){return $args[3]}
    if($args[1] -ne '/workspace/toolchains/analysis-python/bin/python3'){throw 'Unexpected interpreter'}
    if($args[2] -eq '-c'){if($global:badDecoder){$global:LASTEXITCODE=5};return}
    if($args[4] -ne '--output'){throw 'Unexpected analysis arguments'}
    if(-not (Test-Path -LiteralPath $args[2])){throw 'Fresh helper missing'}
    foreach($dep in @('discover_ground_support.py','verify_motion_gravity.py','resolve_motion_gravity.py')) {if(-not(Test-Path -LiteralPath (Join-Path (Split-Path $args[2]) $dep))){throw 'Dependency not staged'}}
    if(Test-Path -LiteralPath (Join-Path (Split-Path $args[2]) 'motion_gravity_patterns.json')){throw 'Discovery profile must not be staged'}
    [IO.File]::WriteAllText($args[5],'{"schema":1,"result":"MOCK_TRANSPORT_ONLY"}')
}

try {
    [IO.Directory]::CreateDirectory($source)>$null;[IO.Directory]::CreateDirectory($logs)>$null
    [IO.File]::WriteAllText((Join-Path $game 'Spider-Man2.exe'),'owned fake EXE unchanged')
    [IO.File]::WriteAllText((Join-Path $source 'dirty.txt'),'preserve local change')
    $old=Join-Path $logs 'MSM2-WS-presence-hook-audit-20300101-000000.json'
    [IO.File]::WriteAllText($old,'preserve older output')
    & $script -SourceRoot $source
    if($global:fetches -ne 1 -or (Get-Content -LiteralPath $old -Raw) -ne 'preserve older output'){throw 'Fetch/output preservation failed'}
    if(-not (Test-Path -LiteralPath (Join-Path $logs 'MSM2-WS-presence-hook-audit-20300101-000000-01.json'))){throw 'Collision suffix missing'}
    if((Get-Content -LiteralPath (Join-Path $source 'dirty.txt') -Raw) -ne 'preserve local change'){throw 'Source changed'}
    if((Get-Content -LiteralPath (Join-Path $game 'Spider-Man2.exe') -Raw) -ne 'owned fake EXE unchanged'){throw 'EXE changed'}
    'PASS: PowerShell wrapper derives Unicode/space game layout, fresh staged helpers, pinned Git reads and collision-safe output; source/EXE preserved (mock transport)'
    $global:pythonFailed=$true;$refused=$false
    try { & $script -SourceRoot $source -GameRoot $game } catch { $refused=$_.Exception.Message -like '*path conversion unavailable*' }
    if(-not $refused){throw 'Python failure not refused'}
    'PASS: unavailable Python refuses before capture; no build/deploy/game/repair fallback (mock transport)'
    $global:pythonFailed=$false;$global:badDecoder=$true;$refused=$false
    try { & $script -SourceRoot $source -GameRoot $game } catch { $refused=$_.Exception.Message -like '*Capstone5.0.6 unavailable*' }
    if(-not $refused){throw 'Decoder dependency failure not refused'}
    'PASS: missing/wrong decoder refuses before analysis with no automatic install (mock transport)'

}
finally { Remove-Item -LiteralPath $fixture -Recurse -Force; Remove-Item Function:git,Function:wsl.exe,Function:Get-Date }
