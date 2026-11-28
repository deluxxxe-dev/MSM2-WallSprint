# Update-WS only (owner 2026-11-28): close the shared Overstrike of this game root before the update starts and keep
# going. Only the exact <game>\Overstrike\Overstrike.exe process is touched; anything else is left to the updater checks.
param([Parameter(Mandatory=$true)][string]$GameRoot)
$ErrorActionPreference = 'Stop'
$game = [IO.Path]::GetFullPath($GameRoot)
$exe = Join-Path $game 'Overstrike\Overstrike.exe'
$procs = @(Get-Process -Name 'Overstrike' -ErrorAction SilentlyContinue | Where-Object { $_.Path -and [String]::Equals([IO.Path]::GetFullPath($_.Path), $exe, [StringComparison]::OrdinalIgnoreCase) })
if (-not $procs.Count) { Write-Output 'WS: Overstrike is not running.'; exit 0 }
$ids = @($procs | ForEach-Object { $_.Id })
foreach ($p in $procs) { Write-Output ('WS: closing the shared Overstrike PID=' + $p.Id); try { [void]$p.CloseMainWindow() } catch { } }
Wait-Process -Id $ids -Timeout 8 -ErrorAction SilentlyContinue
foreach ($p in @(Get-Process -Id $ids -ErrorAction SilentlyContinue)) { Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue }
Wait-Process -Id $ids -Timeout 10 -ErrorAction SilentlyContinue
if (@(Get-Process -Id $ids -ErrorAction SilentlyContinue).Count) { Write-Output 'WS: Overstrike did not close; close it manually.'; exit 1 }
Write-Output 'WS: Overstrike closed; the update continues.'
exit 0
