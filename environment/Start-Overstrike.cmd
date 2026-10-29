@echo off
setlocal DisableDelayedExpansion
set "MOD_ENGINE=%~dp0mod_environment.py"
set "MOD_GAME=%~dp0.."

rem Installed steady-state launcher only: safely repair two known stale-environment cases
rem before the Python environment checker runs. Source/bootstrap copies fall through.
if exist "%MOD_GAME%\Spider-Man2.exe" (
  powershell.exe -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -Command ^
    "$ErrorActionPreference='Stop'; $game=[IO.Path]::GetFullPath($env:MOD_GAME);" ^
    "$bad=Join-Path $game 'Overstrike\winmm.dll';" ^
    "if(Test-Path -LiteralPath $bad){" ^
      "$item=Get-Item -LiteralPath $bad -Force; if($item.PSIsContainer){throw 'Overstrike\winmm.dll is not a regular file'};" ^
      "$q=Join-Path $game 'Mod-Tools\_quarantine'; New-Item -ItemType Directory -Force -Path $q | Out-Null;" ^
      "$h=(Get-FileHash -LiteralPath $bad -Algorithm SHA256).Hash.ToLowerInvariant();" ^
      "$dst=Join-Path $q ('Overstrike-winmm-'+$h.Substring(0,16)+'.dll.disabled');" ^
      "if(Test-Path -LiteralPath $dst){" ^
        "$dh=(Get-FileHash -LiteralPath $dst -Algorithm SHA256).Hash.ToLowerInvariant(); if($dh -ne $h){throw 'Quarantine hash collision'}; Remove-Item -LiteralPath $bad -Force;" ^
      "}else{Move-Item -LiteralPath $bad -Destination $dst};" ^
      "Write-Host ('AUTO-REPAIRED misplaced Overstrike\winmm.dll -> '+$dst)" ^
    "};" ^
    "$lock=Join-Path $game '.MSM2-Mod-Tools.lock';" ^
    "if(Test-Path -LiteralPath $lock){" ^
      "$raw=(Get-Content -LiteralPath $lock -Raw).Trim(); $ownerPid=0;" ^
      "if([int]::TryParse($raw,[ref]$ownerPid)){" ^
        "$proc=Get-CimInstance Win32_Process -Filter ('ProcessId='+$ownerPid) -ErrorAction SilentlyContinue;" ^
        "if(-not $proc){Remove-Item -LiteralPath $lock -Force; Write-Host 'AUTO-REPAIRED stale MSM2 Mod-Tools lock'}" ^
      "}" ^
    "}"
  if errorlevel 1 goto preflight_failed
)

cd /d "%TEMP%"
where py >nul 2>&1
if errorlevel 1 goto python
py -3 "%MOD_ENGINE%" start %*
goto finish
:python
python "%MOD_ENGINE%" start %*
goto finish
:preflight_failed
echo MOD ENVIRONMENT ERROR: automatic shared-Overstrike preflight repair failed.
set "MOD_RESULT=1"
pause
exit /b %MOD_RESULT%
:finish
set "MOD_RESULT=%ERRORLEVEL%"
if not "%MOD_RESULT%"=="0" pause
exit /b %MOD_RESULT%
