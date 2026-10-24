param([Parameter(Mandatory=$true)][string]$GameRoot)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($GameRoot).TrimEnd('\')
$overstrike=[IO.Path]::GetFullPath((Join-Path $root 'Overstrike\Overstrike.exe'))
$updater=[IO.Path]::GetFullPath((Join-Path $root 'Check for updates.exe'))

if(-not (Test-Path -LiteralPath $overstrike -PathType Leaf)){
    throw "Shared Overstrike.exe is missing: $overstrike"
}

Start-Process -FilePath $overstrike -WorkingDirectory $root | Out-Null
$stopped=@()

# Overstrike 1.8.1.0 may spawn its updater from the game root. On this layout
# that updater can load the game-root WINMM.dll and hold it open while
# Overstrike's .script support installer tries to replace the same file.
# Kill only the exact game-root updater; never kill an unrelated same-name process.
for($i=0;$i -lt 50;$i++){
    foreach($proc in Get-Process -ErrorAction SilentlyContinue){
        try{$path=$proc.Path}catch{continue}
        if([string]::IsNullOrEmpty($path)){continue}
        try{$full=[IO.Path]::GetFullPath($path)}catch{continue}
        if($full -ieq $updater){
            Stop-Process -Id $proc.Id -Force -ErrorAction Stop
            $stopped += "$($proc.ProcessName):$($proc.Id)"
        }
    }
    Start-Sleep -Milliseconds 100
}

foreach($proc in Get-Process -ErrorAction SilentlyContinue){
    try{$path=$proc.Path}catch{continue}
    if([string]::IsNullOrEmpty($path)){continue}
    try{$full=[IO.Path]::GetFullPath($path)}catch{continue}
    if($full -ieq $updater){
        throw "Exact game-root Overstrike updater remained active and may lock WINMM.dll: $updater"
    }
}

Write-Output ('overstrike_started=1 stopped_game_root_updater='+($stopped -join ','))
