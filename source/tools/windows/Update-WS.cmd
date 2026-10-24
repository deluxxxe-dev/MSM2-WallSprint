@echo off
setlocal DisableDelayedExpansion
set "WS_ENGINE=%~dp0_source\source\tools\workspace.py"
if exist "%WS_ENGINE%" goto engine
echo WS ERROR: independent _source checkout missing. Run Migrate-WS.cmd first.
goto failed
:engine
where py >nul 2>&1
if errorlevel 1 goto python
py -3 "%WS_ENGINE%" update --workspace "%~dp0." %*
if errorlevel 1 goto failed
exit /b 0
:python
where python >nul 2>&1
if errorlevel 1 goto prerequisites
python "%WS_ENGINE%" update --workspace "%~dp0." %*
if errorlevel 1 goto failed
exit /b 0
:prerequisites
echo WS ERROR: Windows Python 3.11+, Git for Windows and configured WSL2 toolchain are required. No build/test step is skipped.
:failed
if not defined MSM2_WS_NO_PAUSE pause
exit /b 1
