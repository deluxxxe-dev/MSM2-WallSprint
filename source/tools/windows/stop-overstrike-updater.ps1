param([Parameter(Mandatory=$true)][string]$GameRoot)
$ErrorActionPreference='Stop'
foreach($name in @('Spider-Man2','Overstrike','Check for updates')){
 foreach($p in @(Get-Process -Name $name -ErrorAction SilentlyContinue)){
  throw ('Close process manually before update: '+$name+' PID='+$p.Id+' Path='+$p.Path)
 }
}
