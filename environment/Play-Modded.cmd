@echo off
setlocal DisableDelayedExpansion
set "MOD_ENGINE=%~dp0mod_environment.py"
cd /d "%TEMP%"
where py >nul 2>&1
if errorlevel 1 goto python
py -3 "%MOD_ENGINE%" play %*
goto finish
:python
python "%MOD_ENGINE%" play %*
:finish
set "MOD_RESULT=%ERRORLEVEL%"
if not "%MOD_RESULT%"=="0" pause
if "%MOD_RESULT%"=="0" if "play"=="backup" pause
if "%MOD_RESULT%"=="0" if "play"=="restore" pause
if "%MOD_RESULT%"=="0" if "play"=="verify" pause
exit /b %MOD_RESULT%
