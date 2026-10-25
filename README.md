# WallSprint

v0.1c32 / package0.1.50 adds only low128/high128 dependency banks for vector parents0-15 and models exact authenticated VMOVUPS0x2CF7FF0: VEX2.256 YMM0 <- m256[RDX], current same Hero source/tag0. Both halves become unknown may-top7 without reading vector bytes; above256 zero is recorded, every future ZMM/EVEX use still refuses. Legacy XMM aliases touch low128 and preserve high128; unmodeled VEX128/other YMM forms remain refused. Both-path return tags union conservatively and zero knowledge intersects. C30 RAX top7/no qword reads and all caps/runtime/ownership/game-write gates remain. Actual next barrier awaits minimum new vector-state capture.

Use `MSM2-WS-v0.1c32.zip` and the included `Migrate-WS.cmd` outside Mods Library. The authoritative clone is `<game>/Mods Library/Mods-Developing/MSM2-WS/_source`; normal updates use `MSM2-WS/Update-WS.cmd`. Exactly one active versioned `.script` remains at workspace root. Overstrike also scans ZIP contents: the independent source clone excludes archives from its working tree using managed Git sparse checkout, and build-cache ZIPs are retained only as `.zip.disabled`. Code and release archive history/downloads remain in GitHub. Shared Overstrike is `<game>/Overstrike/Overstrike.exe` with game-root WorkingDirectory. SF/ordinary mods and the legacy environment are never cleaned or automatically removed.

See [README_TEST.md](README_TEST.md), [docs/LOCAL_WORKSPACE.md](docs/LOCAL_WORKSPACE.md) and [source/BUILD.md](source/BUILD.md) for Python/Git/WSL prerequisites, first migration command and real-PC acceptance checks. Runtime logs use `MSM2-WS/logs/MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log` without fallback. Cloud fixture success is not a claim that the user's PC was migrated.


## Shared launch / private environment backup

The separate [MSM2 Mod-Tools](environment/README.md) provide checked desktop
Overstrike/modded-game launchers, complete shared MOD-environment backup and verified same-path recovery
onto the matching Vanilla game build. They do not change WS/SF runtime code.
[Validation and Windows handoff](environment/TEST_REPORT.md).

For future changes, see the [Mod-Tools manual and specifications](docs/mod-tools/README.md)
or the [complete single-file manual](docs/mod-tools/MANUAL.md).
