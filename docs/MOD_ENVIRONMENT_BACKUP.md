# Shared environment launch and recovery

This is the user's explicitly requested shared infrastructure task. SF feature/
probe work is left to the SF Codex; WS gameplay/EDI research remains unchanged.

See [user instructions](../environment/README.md) and [tests](../environment/TEST_REPORT.md).
The toolkit version is1.1.0, independent of WSv0.1c20/package0.1.38 and SF0.1.3.

## Delivered behavior

Install-Mod-Tools copies only its own files/config to `<game>/Mod-Tools` and
creates owned Overstrike/Play/Backup desktop shortcuts. Modded launch follows the observed
Overstrike LaunchGame behavior: Spider-Man2.exe, cwd=game, -scripts if not already
in commandline.txt. It validates the chosen profile's script support and installed
DLL list, reuses a matching running game and refuses a foreign matching-name process.
Open-Overstrike uses only game/Overstrike/Overstrike.exe, cwd=game. There is no auto
Git pull, rebuild, new manager copy or forced process kill in normal launch.

Backup captures the whole Overstrike, Profiles, Mods Library, scripts, Mod-Tools,
root scripts_proxy.dll/winmm.dll/scripts.txt/commandline.txt (existing entries).
Hidden Git files, dirty/untracked work, logs, ordinary mods and empty directories
are included. A manifest inventories relative paths, sizes, file modes/mtimes and
SHA256; a new external timestamped folder is published only after full ZIP/CRC/hash
verification and stable file-list/stat checks. The private archive and independent
recovery engine travel together. No user backup/mod binary is shipped with the
public source-only toolkit ZIP or sent anywhere by the tool.

Restore supports the original absolute game path and matching EXE hash. It
validates the archive and current roots, stages and checks all files, then moves
old selected roots to a preserved sibling rollback folder before replacing them.
Synchronous replacement failures restore old roots; incomplete rollback reports
and preserves both trees. Successful rollback copies are retained too. Zip traversal,
reserved/case-alias paths, unlisted/duplicate/link/encrypted entries and external
Git-object dependencies refuse. A shared operation lock coordinates toolkit calls;
source stat changes during staging or capture refuse rather than publishing a
known changing snapshot. It is not a filesystem/VSS atomic snapshot of live editors.

No game EXE/asset archives are archived or replaced. On Vanilla, Overstrike must
reapply the saved selection with Install mods; direct launch requires explicit
installation confirmation once after restore. Python3.11+ and Windows PowerShell
are needed for recovery; external WSL/MSVC/Git/Python installation and credentials,
ACL/ownership/alternate streams/saves are outside the game-folder snapshot scope.
No separate project or manager directory is created; old D sources are untouched.

## Evidence and limits

Owned real-filesystem/Git tests and mocked OS launch tests are listed in the test
report. Actual Windows shortcuts/COM/launch, long-path and full user restore remain
human verification. No current WS/SF binary or safety gate changed. The user should
first install the toolkit, create/verify a backup outside the game and test ordinary
launch. Do not delete the healthy environment merely to test restoration.

Upstream behavior reference only: Tkachov/Overstrike at
ab61de27a4ba76d92780178f600f73f15bcddd81, MainWindow.xaml.cs LaunchGame and
Data/Profile.cs/AppSettings.cs. No upstream implementation or external binary was
copied; toolkit code and tests are independently authored.
