@echo off
setlocal DisableDelayedExpansion
set "WS_PACKAGE_NAME=MSM2-WS-v0.1c20.zip"
if not defined MSM2_WS_OVERSTRIKE_EXE goto missing_overstrike
if not "%MSM2_WS_OVERSTRIKE_EXE:~1,2%"==":\" goto missing_overstrike
for %%I in ("%MSM2_WS_OVERSTRIKE_EXE%") do set "WS_OVERSTRIKE=%%~fI"
for %%I in ("%WS_OVERSTRIKE%") do if /I not "%%~nxI"=="Overstrike.exe" goto missing_overstrike
if not exist "%WS_OVERSTRIKE%" goto missing_overstrike
for %%I in ("%WS_OVERSTRIKE%") do set "WS_EXE_ATTR=%%~aI"
if /I "%WS_EXE_ATTR:~0,1%"=="d" goto missing_overstrike
set "WS_GAME_ROOT=C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2"
if defined MSM2_WS_GAME_ROOT set "WS_GAME_ROOT=%MSM2_WS_GAME_ROOT%"
if not "%WS_GAME_ROOT:~1,2%"==":\" goto missing_game
for %%I in ("%WS_GAME_ROOT%") do set "WS_GAME_ROOT=%%~fI"
if not exist "%WS_GAME_ROOT%\Spider-Man2.exe" goto missing_game
for %%I in ("%WS_GAME_ROOT%\Spider-Man2.exe") do set "WS_GAME_ATTR=%%~aI"
if /I "%WS_GAME_ATTR:~0,1%"=="d" goto missing_game
set "WS_PACKAGE=%~dp0%WS_PACKAGE_NAME%"
if exist "%WS_PACKAGE%" goto package_found
set "WS_PACKAGE=%~dp0..\%WS_PACKAGE_NAME%"
if exist "%WS_PACKAGE%" goto package_found
set "WS_PACKAGE=%~dp0..\..\%WS_PACKAGE_NAME%"
:package_found
if defined MSM2_WS_PACKAGE set "WS_PACKAGE=%MSM2_WS_PACKAGE%"
for %%I in ("%WS_PACKAGE%") do if /I not "%%~nxI"=="%WS_PACKAGE_NAME%" goto missing_package
for %%I in ("%WS_PACKAGE%") do set "WS_PACKAGE=%%~fI"
if not exist "%WS_PACKAGE%" goto missing_package
set "WS_LIBRARY=%WS_GAME_ROOT%\Mods Library\Mods-Developing"
if exist "%WS_LIBRARY%\." goto library_ready
mkdir "%WS_LIBRARY%" >nul 2>&1
if errorlevel 1 goto copy_failed
:library_ready
set "WS_DEST=%WS_LIBRARY%\%WS_PACKAGE_NAME%"
if /I "%WS_PACKAGE%"=="%WS_DEST%" goto launch
copy /b /y "%WS_PACKAGE%" "%WS_DEST%" >nul
if errorlevel 1 goto copy_failed
:launch
pushd "%WS_GAME_ROOT%" >nul 2>&1
if errorlevel 1 goto launch_failed
start "" /D "%WS_GAME_ROOT%" "%WS_OVERSTRIKE%"
if errorlevel 1 goto launch_failed_pop
popd
echo [WS] Updated "%WS_DEST%"
echo [WS] Overstrike launched with game root as WorkingDirectory.
exit /b 0
:missing_overstrike
echo [WS ERROR] Set MSM2_WS_OVERSTRIKE_EXE to the existing separated Overstrike.exe absolute path. No guessed AppData path or WorkingDirectory fallback is used.
goto failed
:missing_game
echo [WS ERROR] Game root is unavailable. Set MSM2_WS_GAME_ROOT to its absolute directory containing Spider-Man2.exe.
goto failed
:missing_package
echo [WS ERROR] Put %WS_PACKAGE_NAME% beside this helper, within two parent folders, or set MSM2_WS_PACKAGE to that exact ZIP. Other versions and SF packages are refused.
goto failed
:copy_failed
echo [WS ERROR] Cannot create or update "%WS_LIBRARY%". No other mod is cleaned or removed.
goto failed
:launch_failed_pop
popd
:launch_failed
echo [WS ERROR] Cannot launch the configured Overstrike with game-root WorkingDirectory. No fallback was attempted.
:failed
if not defined MSM2_WS_NO_PAUSE pause
exit /b 1
