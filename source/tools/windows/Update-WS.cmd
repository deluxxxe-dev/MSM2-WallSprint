@echo off
setlocal DisableDelayedExpansion
set "WS_ENGINE=%~dp0_source\source\tools\workspace.py"
set "WS_CLOSE=%~dp0_source\source\tools\windows\close-overstrike-for-update.ps1"
if exist "%WS_CLOSE%" powershell.exe -NoLogo -NoProfile -NonInteractive -ExecutionPolicy RemoteSigned -File "%WS_CLOSE%" -GameRoot "%~dp0..\..\.."
where py >nul 2>&1
if errorlevel 1 goto python
py -3 "%WS_ENGINE%" update --workspace "%~dp0." %*
goto finish
:python
python "%WS_ENGINE%" update --workspace "%~dp0." %*
:finish
set "WS_RESULT=%ERRORLEVEL%"
if not "%WS_RESULT%"=="0" if not defined MSM2_WS_NO_PAUSE pause
exit /b %WS_RESULT%
