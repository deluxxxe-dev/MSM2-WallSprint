@echo off
setlocal DisableDelayedExpansion
set "MOD_ENGINE=%~dp0mod_environment.py"
cd /d "%TEMP%"
where py >nul 2>&1
if errorlevel 1 goto python
py -3 "%MOD_ENGINE%" verify %*
goto finish
:python
python "%MOD_ENGINE%" verify %*
:finish
set "MOD_RESULT=%ERRORLEVEL%"
if not "%MOD_RESULT%"=="0" pause
if "%MOD_RESULT%"=="0" if "verify"=="backup" pause
if "%MOD_RESULT%"=="0" if "verify"=="restore" pause
if "%MOD_RESULT%"=="0" if "verify"=="verify" pause
exit /b %MOD_RESULT%
