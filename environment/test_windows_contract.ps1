# Owned fake processes/launches only; runs on Core/Linux and Desktop/Windows.
$ErrorActionPreference = 'Stop'
$temp = Join-Path ([IO.Path]::GetTempPath()) ('MSM2-launch-test-' + [Guid]::NewGuid().ToString('N'))
$checks = 0
$global:ModTestProcesses = @{}
$global:ModTestLaunches = @()
$global:ModTestActivations = @()
function Check($condition, $message) { if (-not $condition) { throw $message }; $script:checks++ }
function Get-Process { param([string]$Name, $ErrorAction) if ($global:ModTestProcesses.ContainsKey($Name)) { $global:ModTestProcesses[$Name] } }
function Stop-Process { throw 'The toolkit must not stop any process.' }
function Start-Process { param($FilePath, $WorkingDirectory, $ArgumentList) $global:ModTestLaunches += [pscustomobject]@{File=$FilePath; Cwd=$WorkingDirectory; Args=$ArgumentList} }
$global:ModTestShell = [pscustomobject]@{}
$global:ModTestShell | Add-Member -MemberType ScriptMethod -Name AppActivate -Value { param($id) $global:ModTestActivations += $id; return $true }
function New-Object { param($ComObject) if ($ComObject -ne 'WScript.Shell') { throw 'Unexpected COM request' }; return $global:ModTestShell }
try {
    $game = Join-Path $temp ("Game's " + [char]0x30c6)
    New-Item -ItemType Directory -Path $game | Out-Null
    $request = Join-Path $temp 'request.json'
    $helper = Join-Path $PSScriptRoot 'windows.ps1'
    function Run([string]$Action) {
        [IO.File]::WriteAllText($request, (@{action=$Action;game=$game} | ConvertTo-Json), [Text.UTF8Encoding]::new($false))
        & $helper -Request $request
    }
    Run 'play'
    Check ($global:ModTestLaunches.Count -eq 1) 'Expected one game launch.'
    Check ($global:ModTestLaunches[-1].File -eq (Join-Path $game 'Spider-Man2.exe') -and $global:ModTestLaunches[-1].Cwd -eq $game) 'Game path/CWD changed.'
    Check ($global:ModTestLaunches[-1].Args -eq '-scripts') 'Modded game requires -scripts.'
    [IO.File]::WriteAllText((Join-Path $game 'commandline.txt'), '-other -ScRiPtS', [Text.Encoding]::UTF8)
    Run 'play'
    Check ($null -eq $global:ModTestLaunches[-1].Args) 'Duplicated existing -scripts argument.'
    Run 'manager'
    Check ($global:ModTestLaunches[-1].File -eq (Join-Path $game 'Overstrike/Overstrike.exe') -and $global:ModTestLaunches[-1].Cwd -eq $game) 'Shared manager path/CWD changed.'
    $global:ModTestProcesses['Spider-Man2'] = [pscustomobject]@{Path=(Join-Path $game 'Spider-Man2.exe');Id=123}
    $before = $global:ModTestLaunches.Count
    Run 'play'
    Check ($global:ModTestLaunches.Count -eq $before -and $global:ModTestActivations[-1] -eq 123) 'Existing game was launched twice.'
    $blocked = $false
    try { Run 'check' } catch { $blocked = $_.Exception.Message -match 'Close MSM2 and Overstrike' }
    Check $blocked 'Backup/restore did not refuse an active game.'
    $global:ModTestProcesses = @{'Overstrike'=[pscustomobject]@{Path=(Join-Path $temp 'foreign/Overstrike.exe');Id=456}}
    $blocked = $false
    try { Run 'manager' } catch { $blocked = $_.Exception.Message -match 'different Overstrike' }
    Check $blocked 'Different manager was not protected.'
    $global:ModTestProcesses = @{}
    Run 'check'
    Check ($global:ModTestLaunches.Count -eq $before) 'Process check launched a program.'
    $parseCount = 0
    foreach ($file in @($helper, $PSCommandPath)) {
        $tokens=$null;$errors=$null
        $null=[Management.Automation.Language.Parser]::ParseFile($file,[ref]$tokens,[ref]$errors)
        Check (-not $errors.Count) 'PowerShell parsing failed.'
        $parseCount++
    }
    Write-Host "PASS: Mod-Tools Windows launch contract ($checks checks, owned processes/launches; actual shortcuts/launch require Windows)."
} finally {
    Remove-Item -LiteralPath $temp -Recurse -Force
}
