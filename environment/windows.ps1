param([Parameter(Mandatory=$true)][string]$Request)
$ErrorActionPreference = 'Stop'
[Console]::OutputEncoding = [Text.UTF8Encoding]::new($false)
function Set-ModShortcuts($Desktop, $GameRoot, $Shell) {
    $links = @()
    foreach ($item in @(@('MSM2 - Overstrike','Start-Overstrike.cmd'), @('MSM2 - Play Modded','Play-Modded.cmd'), @('MSM2 - Backup Mods','Backup-Mods.cmd'))) {
        $path = Join-Path $Desktop ($item[0] + '.lnk')
        $target = Join-Path $GameRoot ('Mod-Tools\' + $item[1])
        $link = $Shell.CreateShortcut($path)
        if ((Test-Path -LiteralPath $path) -and -not [String]::Equals($link.TargetPath, $target, [StringComparison]::OrdinalIgnoreCase)) {
            throw "Existing unrelated shortcut preserved: $path"
        }
        $link.TargetPath = $target
        $link.WorkingDirectory = $GameRoot
        $icon = if ($item[1] -eq 'Play-Modded.cmd') { 'Spider-Man2.exe' } else { 'Overstrike\Overstrike.exe' }
        $link.IconLocation = (Join-Path $GameRoot $icon) + ',0'
        $links += $link
    }
    foreach ($link in $links) { $link.Save() }
}
$r = [IO.File]::ReadAllText($Request, [Text.Encoding]::UTF8) | ConvertFrom-Json
$game = [IO.Path]::GetFullPath([string]$r.game)
$exe = Join-Path $game 'Spider-Man2.exe'
$manager = Join-Path $game 'Overstrike\Overstrike.exe'
$targets = @{ 'Spider-Man2'=$exe; 'Overstrike'=$manager; 'Check for updates'=(Join-Path $game 'Check for updates.exe') }
$running = @{}
foreach ($name in $targets.Keys) {
    $running[$name] = @()
    foreach ($p in @(Get-Process -Name $name -ErrorAction SilentlyContinue)) {
        if (-not $p.Path) { throw "Cannot identify running $name. Close it manually." }
        if (-not [String]::Equals([IO.Path]::GetFullPath($p.Path), $targets[$name], [StringComparison]::OrdinalIgnoreCase)) {
            throw "A different $name is running; close it manually. No process was stopped."
        }
        $running[$name] += $p
    }
}
if ($r.action -eq 'check') {
    if ($running['Spider-Man2'].Count -or $running['Overstrike'].Count -or $running['Check for updates'].Count) {
        $details = foreach ($name in $targets.Keys) { foreach ($p in $running[$name]) { "$name PID=$($p.Id) Path=$($p.Path)" } }
        throw ('Close MSM2 and Overstrike before install/backup/restore. No process was stopped. ' + ($details -join '; '))
    }
} elseif ($r.action -eq 'shortcuts') {
    $desktop = [Environment]::GetFolderPath('Desktop')
    if (-not $desktop) { throw 'Desktop folder is unavailable.' }
    $shell = New-Object -ComObject WScript.Shell
    Set-ModShortcuts -Desktop $desktop -GameRoot $game -Shell $shell
} elseif ($r.action -eq 'play' -or $r.action -eq 'manager') {
    $name = if ($r.action -eq 'play') { 'Spider-Man2' } else { 'Overstrike' }
    if ($running[$name].Count) {
        $shell = New-Object -ComObject WScript.Shell
        if (-not $shell.AppActivate($running[$name][0].Id)) { throw "Existing $name has no focusable window; close it manually." }
        Write-Output 'Existing instance reused; no duplicate process launched.'
    } elseif ($name -eq 'Overstrike') {
        Start-Process -FilePath $manager -WorkingDirectory $game | Out-Null
    } else {
        $arguments = @()
        $commandline = Join-Path $game 'commandline.txt'
        if (-not ((Test-Path -LiteralPath $commandline) -and ([IO.File]::ReadAllText($commandline) -split '\s+' -contains '-scripts'))) {
            $arguments = @('-scripts')
        }
        if ($arguments.Count) { Start-Process -FilePath $exe -WorkingDirectory $game -ArgumentList $arguments | Out-Null }
        else { Start-Process -FilePath $exe -WorkingDirectory $game | Out-Null }
    }
} else { throw 'Unknown Mod-Tools Windows action.' }
