# WallSprint

v0.1c26 / package0.1.44 classifies only the pinned CMP at0x2CF7FE0 from stable guarded current code. Full32/64-bit GPR register/register or register/immediate forms may write only CF/PF/AF/ZF/SF/OF; operands remain intact, immediates are untainted and tags never determine a numerical branch outcome. Partial8/16-bit/memory/unknown forms refuse. A next branch reports its condition/flag provenance but remains a strict linear-leaf refusal. C25 actual527161-entry table validation/no-entry status is accepted; actual c26 CMP form/next boundary remains pending. No target hash/effect is admitted without the unchanged128-byte/32-instruction complete linear normal-RET proof; no effect/XYZ/W/ownership/game-write permission.

Use `MSM2-WS-v0.1c26.zip` and the included `Migrate-WS.cmd` outside Mods Library. The authoritative clone is `<game>/Mods Library/Mods-Developing/MSM2-WS/_source`; normal updates use `MSM2-WS/Update-WS.cmd`. Exactly one active versioned `.script` remains at workspace root. Overstrike also scans ZIP contents: the independent source clone excludes archives from its working tree using managed Git sparse checkout, and build-cache ZIPs are retained only as `.zip.disabled`. Code and release archive history/downloads remain in GitHub. Shared Overstrike is `<game>/Overstrike/Overstrike.exe` with game-root WorkingDirectory. SF/ordinary mods and the legacy environment are never cleaned or automatically removed.

See [README_TEST.md](README_TEST.md), [docs/LOCAL_WORKSPACE.md](docs/LOCAL_WORKSPACE.md) and [source/BUILD.md](source/BUILD.md) for Python/Git/WSL prerequisites, first migration command and real-PC acceptance checks. Runtime logs use `MSM2-WS/logs/MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log` without fallback. Cloud fixture success is not a claim that the user's PC was migrated.


## Shared launch / private environment backup

The separate [MSM2 Mod-Tools](environment/README.md) provide checked desktop
Overstrike/modded-game launchers, complete shared MOD-environment backup and verified same-path recovery
onto the matching Vanilla game build. They do not change WS/SF runtime code.
[Validation and Windows handoff](environment/TEST_REPORT.md).

For future changes, see the [Mod-Tools manual and specifications](docs/mod-tools/README.md)
or the [complete single-file manual](docs/mod-tools/MANUAL.md).
