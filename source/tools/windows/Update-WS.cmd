@echo off
setlocal DisableDelayedExpansion
set "WS_ENGINE=%~dp0_source\source\tools\workspace.py"
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
