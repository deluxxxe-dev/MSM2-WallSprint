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

## Development mod library location

WallSprint and SocialFeed development packages must be kept separate from ordinary installed/mod-library packages under this local directory on the user's PC:

```text
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\Mods Library\Mods-Developing
```

This folder is the shared development-package library for both:

```text
MSM2-WS-*
MSM2-SF-*
```

Do not place active WS/SF development ZIPs directly in `Mods Library` root when a local packaging/copy helper is being used. A local helper may create `Mods-Developing` if it does not exist and should place the current user-facing install ZIP there.

This is only the development package storage location. It does not replace the game's actual runtime/script installation paths and it does not change WallSprint logging (`log_wallsprint`). Cloud Codex jobs cannot write to the user's `C:` drive; cloud outputs must still be packaged/published normally, while local copy/install instructions and local helper scripts must use the path above.

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