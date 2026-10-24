@echo off
setlocal DisableDelayedExpansion
for %%I in ("%~dp0..\..\..") do set "GAME_ROOT=%%~fI"
set "WS_ENGINE=%~dp0_source\source\tools\workspace.py"
set "WS_SAFE_LAUNCH=%~dp0_source\source\tools\windows\launch-overstrike.ps1"
set "WS_STOP_UPDATER=%~dp0_source\source\tools\windows\stop-overstrike-updater.ps1"
if exist "%WS_ENGINE%" goto engine
echo WS ERROR: independent _source checkout missing. Run Migrate-WS.cmd first.
goto failed
:engine
if not exist "%GAME_ROOT%\winmm.dll" goto recover_script_support
where py >nul 2>&1
if errorlevel 1 goto python
py -3 "%WS_ENGINE%" update --workspace "%~dp0." %*
if errorlevel 1 goto failed
goto post_launch
:python
where python >nul 2>&1
if errorlevel 1 goto prerequisites
python "%WS_ENGINE%" update --workspace "%~dp0." %*
if errorlevel 1 goto failed
goto post_launch
:post_launch
call :stop_exact_updater
if errorlevel 1 goto failed
exit /b 0
:recover_script_support
where powershell.exe >nul 2>&1
if errorlevel 1 goto prerequisites
if not exist "%WS_SAFE_LAUNCH%" (
  echo WS ERROR: recovery helper missing. Pull the current _source branch first.
  goto failed
)
echo WS NOTICE: Game-root winmm.dll is missing. Starting shared Overstrike for .script support recovery.
powershell.exe -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File "%WS_SAFE_LAUNCH%" -GameRoot "%GAME_ROOT%"
if errorlevel 1 goto failed
echo WS RECOVERY: In Overstrike, select .script support and the desired mods, then Install mods.
echo WS RECOVERY: After winmm.dll is restored, close Overstrike and run Update-WS.cmd again.
if not defined MSM2_WS_NO_PAUSE pause
exit /b 2
:stop_exact_updater
where powershell.exe >nul 2>&1
if errorlevel 1 exit /b 1
if not exist "%WS_STOP_UPDATER%" exit /b 1
powershell.exe -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File "%WS_STOP_UPDATER%" -GameRoot "%GAME_ROOT%"
exit /b %errorlevel%
:prerequisites
echo WS ERROR: Windows Python 3.11+, Git for Windows, Windows PowerShell and configured WSL2 toolchain are required. No build/test step is skipped.
:failed
if not defined MSM2_WS_NO_PAUSE pause
exit /b 1
