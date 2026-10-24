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
    $global:ModTestProcesses['Overstrike'] = [pscustomobject]@{Path=(Join-Path $game 'Overstrike/Overstrike.exe');Id=789}
    $managerBefore = $global:ModTestLaunches.Count
    Run 'manager'
    Check ($global:ModTestLaunches.Count -eq $managerBefore -and $global:ModTestActivations[-1] -eq 789) 'Existing manager was launched twice.'
    $global:ModTestProcesses = @{}
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
    # Load the production shortcut function; its host boundaries remain fake.
    . $helper -Request $request
    $desktop = Join-Path $temp 'owned desktop'
    New-Item -ItemType Directory -Path $desktop | Out-Null
    $global:ModTestShortcuts = @{}
    $global:ModTestSaved = @()
    $shortcutShell = [pscustomobject]@{}
    $shortcutShell | Add-Member -MemberType ScriptMethod -Name CreateShortcut -Value {
        param($path)
        if (-not $global:ModTestShortcuts.ContainsKey($path)) {
            $link = [pscustomobject]@{Path=$path;TargetPath='';WorkingDirectory='';IconLocation=''}
            $link | Add-Member -MemberType ScriptMethod -Name Save -Value { $global:ModTestSaved += $this.Path }
            $global:ModTestShortcuts[$path] = $link
        }
        return $global:ModTestShortcuts[$path]
    }
    Set-ModShortcuts -Desktop $desktop -GameRoot $game -Shell $shortcutShell
    Check ($global:ModTestSaved.Count -eq 3) 'Expected all three desktop shortcuts.'
    $managerLink = $global:ModTestShortcuts[(Join-Path $desktop 'MSM2 - Overstrike.lnk')]
    Check ($managerLink.TargetPath -eq (Join-Path $game 'Mod-Tools/Start-Overstrike.cmd') -and $managerLink.WorkingDirectory -eq $game) 'Manager shortcut must use the installed checked launcher and game CWD.'
    Check ($managerLink.IconLocation -eq ((Join-Path $game 'Overstrike/Overstrike.exe') + ',0')) 'Manager icon path changed.'
    $conflict = Join-Path $desktop 'MSM2 - Backup Mods.lnk'
    [IO.File]::WriteAllText($conflict, 'unrelated shortcut marker')
    $global:ModTestShortcuts[$conflict].TargetPath = 'unrelated application'
    $savedBefore = $global:ModTestSaved.Count
    $blocked = $false
    try { Set-ModShortcuts -Desktop $desktop -GameRoot $game -Shell $shortcutShell } catch { $blocked = $_.Exception.Message -match 'unrelated shortcut preserved' }
    Check ($blocked -and $global:ModTestSaved.Count -eq $savedBefore -and [IO.File]::ReadAllText($conflict) -eq 'unrelated shortcut marker') 'Shortcut collision must preserve existing files before saving any shortcuts.'
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
