@echo off
setlocal DisableDelayedExpansion
set "WS_ENGINE=%~dp0_source\source\tools\workspace.py"
if exist "%WS_ENGINE%" goto engine
set "WS_ENGINE=%~dp0tools\workspace.py"
if exist "%WS_ENGINE%" goto engine
set "WS_ENGINE=%~dp0source\tools\workspace.py"
if exist "%WS_ENGINE%" goto engine
echo WS ERROR: migration engine missing. Extract the complete workspace release outside Mods Library first.
goto failed
:engine
where py >nul 2>&1
if errorlevel 1 goto python
py -3 "%WS_ENGINE%" migrate %*
if errorlevel 1 goto failed
exit /b 0
:python
where python >nul 2>&1
if errorlevel 1 goto prerequisites
python "%WS_ENGINE%" migrate %*
if errorlevel 1 goto failed
exit /b 0
:prerequisites
echo WS ERROR: Windows Python 3.11+, Git for Windows and configured WSL2 toolchain are required. See source/BUILD.md.
:failed
if not defined MSM2_WS_NO_PAUSE pause
exit /b 1
