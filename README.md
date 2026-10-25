# WallSprint

v0.1c28 / package0.1.46 classifies only the pinned MOV0x2CF7FE9 after accepted JA edges (fallthrough0x2CF7FE9/taken0x2CF8013). Records operand classes/access/width/aliases, exact decoded writes/flags, address components/tags and conservative caller-state alias provenance. Address provenance is separate from loaded-value provenance: writes/RMW and all unproven loads refuse, including image/code addresses; no memory-load semantics or value observation added. C27 both-path/current-code/runtime/cap gates remain. Actual MOV form/address is pending minimal guarded metadata; no identity/effect/ownership or gameplay-write permission.

Use `MSM2-WS-v0.1c28.zip` and the included `Migrate-WS.cmd` outside Mods Library. The authoritative clone is `<game>/Mods Library/Mods-Developing/MSM2-WS/_source`; normal updates use `MSM2-WS/Update-WS.cmd`. Exactly one active versioned `.script` remains at workspace root. Overstrike also scans ZIP contents: the independent source clone excludes archives from its working tree using managed Git sparse checkout, and build-cache ZIPs are retained only as `.zip.disabled`. Code and release archive history/downloads remain in GitHub. Shared Overstrike is `<game>/Overstrike/Overstrike.exe` with game-root WorkingDirectory. SF/ordinary mods and the legacy environment are never cleaned or automatically removed.

See [README_TEST.md](README_TEST.md), [docs/LOCAL_WORKSPACE.md](docs/LOCAL_WORKSPACE.md) and [source/BUILD.md](source/BUILD.md) for Python/Git/WSL prerequisites, first migration command and real-PC acceptance checks. Runtime logs use `MSM2-WS/logs/MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log` without fallback. Cloud fixture success is not a claim that the user's PC was migrated.


## Shared launch / private environment backup

The separate [MSM2 Mod-Tools](environment/README.md) provide checked desktop
Overstrike/modded-game launchers, complete shared MOD-environment backup and verified same-path recovery
onto the matching Vanilla game build. They do not change WS/SF runtime code.
[Validation and Windows handoff](environment/TEST_REPORT.md).

For future changes, see the [Mod-Tools manual and specifications](docs/mod-tools/README.md)
or the [complete single-file manual](docs/mod-tools/MANUAL.md).
