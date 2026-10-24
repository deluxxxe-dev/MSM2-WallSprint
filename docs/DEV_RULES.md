# WallSprint development rules

## Distribution / provenance

WallSprint should remain independently implemented so it can be prepared for eventual Nexus Mods distribution.

Reference projects such as Walk On Walls, ActorRelativeCamera, SwingCore, and similar mods may be used only as research hints. Do not copy their code, binaries, assets, proprietary data, or extracted package contents into WallSprint production source or release packages.

Game-native behavior should be re-established from Spider-Man2.exe, WallSprint runtime instrumentation, and reproducible user logs wherever practical.

Do not add new vendored copies of SM2ScriptTemplate source files. Existing template dependence should eventually be reduced into WallSprint-owned minimal interop definitions after functional milestones are stable.

Release packages must not contain raw private user logs, full captured native game functions, unnecessary game binary fragments, or files from other mods.

## Release package naming

The user-facing install ZIP must use exactly this pattern:

```text
MSM2-WS-v<version>.zip
```

Examples:

```text
MSM2-WS-v0.1c20.zip
MSM2-WS-v0.2.zip
```

Do not append implementation/task descriptions such as `transform_write_contract`, `callee_alias`, `movement_source_lineage`, or similar explanatory suffixes to the install ZIP filename.

If a separate source archive is required for development/reproducibility, it may use `MSM2-WS-v<version>-source.zip`; the user-facing install ZIP must remain the plain version-only name above.

## Development mod library / Overstrike workflow

WS and SF development packages share this local development-only library:

```text
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\Mods Library\Mods-Developing
```

Overstrike recursively scans `Mods Library`, so development packages under `Mods-Developing` remain visible to Overstrike.

For WallSprint:
- all current WS development install artifacts (`MSM2-WS-*`) must be copied/updated under `Mods Library\Mods-Developing`, not directly under `Mods Library`;
- do not move, rename, overwrite, delete, or otherwise modify any `MSM2-SF-*` file;
- a local helper may create `Mods-Developing` if missing;
- when replacing an older WS development build, only replace/remove matching WS development artifacts and leave unrelated mods untouched.

The separated development copy of `Overstrike.exe` may remain in the AppData-side location used by the shared SF/WS workflow, but its **WorkingDirectory must be the MSM2 game root**:

```text
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2
```

Do not use the AppData folder as Overstrike WorkingDirectory. This keeps Overstrike pointed at the game-root `Mods Library\Mods-Developing` and lets game-root dependencies such as `scripts_proxy.dll` resolve from the intended location.

If WallSprint has or gains a double-click local test/update helper, use `UpdateForTest.cmd` as the preferred entry point. It should update only WS development artifacts in `Mods-Developing` and launch/use the separated Overstrike with the game root as WorkingDirectory. It must never touch SF artifacts.

This development-library rule does not change WallSprint runtime logging (`log_wallsprint`) or the actual runtime/script install destination required by the game. Cloud Codex cannot directly write the user's `C:` drive; it must produce the correctly named package and any local helper/update script so the user's local execution performs the copy/update into the path above.

## Native safety

- Current target build: Spider-Man2.exe 2.810.0.0.
- Expected SHA256: c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9.
- Fail closed on build/signature mismatch.
- Do not guess typed Win64 signatures for unknown game functions.
- Preserve registers, stack alignment, shadow space, and return state when assembly thunks are required.
- Prefer narrow caller/owner/state admission over global hooks or patches.
- Never globally patch the common orientation writer solely because it is involved in WallRun/Crawl.

## Scope discipline

Work in milestones. Do not add later systems merely to mask a failure in the current one.

Current order is:

1. orientation ownership / reset path;
2. planar adhesion;
3. surface-relative movement;
4. animation integration;
5. camera;
6. jumps / wall-to-wall;
7. corners / ceilings / continuous surfaces;
8. parkour;
9. web swing;
10. combat.

A failure in orientation must not be hidden by adding adhesion or movement writes.

## Documentation

Keep analysis files explicit about VERIFIED, CANDIDATE, and UNKNOWN findings. Do not upgrade a hypothesis into a verified field name or engine semantic without evidence.