param([Parameter(Mandatory=$true)][string]$GameRoot)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($GameRoot).TrimEnd('\')
$updater=[IO.Path]::GetFullPath((Join-Path $root 'Check for updates.exe'))
$stopped=@()

# Poll briefly because Overstrike may spawn the updater just after its UI process starts.
# Stop only the updater whose executable path is exactly the game-root copy.
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
    if($full -ieq $updater){throw "Exact game-root Overstrike updater remained active: $updater"}
}
Write-Output ('stopped_game_root_updater='+($stopped -join ','))
