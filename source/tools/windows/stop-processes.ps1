param([Parameter(Mandatory=$true)][string]$GameRoot)
$ErrorActionPreference = 'Stop'
$targets = @{
  'Spider-Man2' = [IO.Path]::GetFullPath((Join-Path $GameRoot 'Spider-Man2.exe'))
  'Overstrike' = [IO.Path]::GetFullPath((Join-Path $GameRoot 'Overstrike\Overstrike.exe'))
}
foreach ($name in $targets.Keys) {
  foreach ($process in @(Get-Process -Name $name -ErrorAction SilentlyContinue)) {
    if (-not $process.Path) { throw "Cannot validate process path for $name; no unrelated process will be stopped." }
    if ($name -eq 'Overstrike' -and -not [String]::Equals([IO.Path]::GetFullPath($process.Path), $targets[$name], [StringComparison]::OrdinalIgnoreCase)) { throw "A different Overstrike is running. Close it manually; WS only stops the shared game-root instance." }
    if ([String]::Equals([IO.Path]::GetFullPath($process.Path), $targets[$name], [StringComparison]::OrdinalIgnoreCase)) {
      Stop-Process -Id $process.Id -Force -ErrorAction Stop
      $process.WaitForExit(10000) | Out-Null
      if (-not $process.HasExited) { throw "Process did not stop: $name" }
    }
  }
}
