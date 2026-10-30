param([Parameter(Mandatory=$true)][string]$GameRoot)
$ErrorActionPreference='Stop'
$bundled=Join-Path $PSScriptRoot '..\shared\mod_environment.py'
$source=Join-Path $PSScriptRoot '..\..\..\environment\mod_environment.py'
$engine=if(Test-Path -LiteralPath $bundled -PathType Leaf){$bundled}else{$source}
if(-not(Test-Path -LiteralPath $engine -PathType Leaf)){throw 'Current shared toolkit missing; use Repair-Mod-Tools bootstrap'}
if(Get-Command py -ErrorAction SilentlyContinue){& py -3 $engine start --game-root $GameRoot}else{& python $engine start --game-root $GameRoot}
if($LASTEXITCODE -ne 0){throw 'Checked shared manager launch refused; no process was killed'}
