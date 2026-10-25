# WallSprint

v0.1c30 / package0.1.48 models only accepted MOV0x2CF7FE9, full64 RAX <- [RCX+0x100] at source0xC2D2580, as unknown may-depend top0x7. C29 actual writable .data/PAGE_READWRITE correctly refused tag0 without reading values. C30 keeps that refusal and replaces only RAX abstract contents with an opaque unknown origin/top7; selected dependence remains possible. No qword read/log, numerical interpretation, pointer chase or generic memory rule. Exact current parent source/tags plus accepted PE/page metadata and unchanged code are rechecked. Both c27 JA outcomes/RET/caps/runtime checks remain; minimum next-barrier/path/tag-state capture is pending. Ownership and gameplay writes remain denied.

Use `MSM2-WS-v0.1c30.zip` and the included `Migrate-WS.cmd` outside Mods Library. The authoritative clone is `<game>/Mods Library/Mods-Developing/MSM2-WS/_source`; normal updates use `MSM2-WS/Update-WS.cmd`. Exactly one active versioned `.script` remains at workspace root. Overstrike also scans ZIP contents: the independent source clone excludes archives from its working tree using managed Git sparse checkout, and build-cache ZIPs are retained only as `.zip.disabled`. Code and release archive history/downloads remain in GitHub. Shared Overstrike is `<game>/Overstrike/Overstrike.exe` with game-root WorkingDirectory. SF/ordinary mods and the legacy environment are never cleaned or automatically removed.

See [README_TEST.md](README_TEST.md), [docs/LOCAL_WORKSPACE.md](docs/LOCAL_WORKSPACE.md) and [source/BUILD.md](source/BUILD.md) for Python/Git/WSL prerequisites, first migration command and real-PC acceptance checks. Runtime logs use `MSM2-WS/logs/MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log` without fallback. Cloud fixture success is not a claim that the user's PC was migrated.


## Shared launch / private environment backup

The separate [MSM2 Mod-Tools](environment/README.md) provide checked desktop
Overstrike/modded-game launchers, complete shared MOD-environment backup and verified same-path recovery
onto the matching Vanilla game build. They do not change WS/SF runtime code.
[Validation and Windows handoff](environment/TEST_REPORT.md).

For future changes, see the [Mod-Tools manual and specifications](docs/mod-tools/README.md)
or the [complete single-file manual](docs/mod-tools/MANUAL.md).
