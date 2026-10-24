# WallSprint logging convention

WallSprint diagnostic logs are isolated inside the final WallSprint development workspace.

## Directory

Authoritative runtime log directory:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs\
```

Example current install:

```text
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\Mods Library\Mods-Developing\MSM2-WS\logs\
```

Do not hard-code the absolute example path in reusable code. Derive the game root from the running game executable/module path and append the exact relative workspace path above.

If the directory is missing, create it safely. Do not silently fall back to game-root `log_wallsprint`, a shared `logs` folder, `logs_WS`, or any other legacy destination.

## Filename

One file per WallSprint process/run:

```text
MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

Example:

```text
MSM2-WS-v0.1c20-20261024-120500.log
```

`<version>` comes from the authoritative WallSprint build/version metadata. Do not put commit hashes, GUIDs, or random IDs in the user-facing filename.

Use local PC wall-clock time, zero-padded. Keep A→E sessions from the same game run in the same file. If a same-second filename collision occurs, append a bounded numeric suffix such as `-01`, `-02`, etc. without replacing an existing file.

## Human test convention

Normal diagnostic test remains:

```text
A ~4 s -> fully end WallRun -> move/fast-travel -> E ~4 s
```

Send the single newest timestamped file from:

```text
Mods Library\Mods-Developing\MSM2-WS\logs
```

Video is only needed for a visible anomaly or when specifically requested.

## Implementation / failure behavior

Preserve the existing safe logger properties while changing only the destination/name contract:

- derive paths from the game executable/module rather than a personal absolute install path;
- atomically choose a new filename and never truncate an existing log;
- bounded same-second suffix search;
- re-enabling the same loaded module appends to its selected run file;
- a full module reload starts a new run file;
- directory/open/collision failure reports stage, attempted path, and Win32 error through the existing diagnostic channel;
- no silent fallback to any old/shared log path;
- filename uses local time while telemetry line timestamps retain their existing UTC format.

The logger/path tests must cover spaces and Unicode in the containing game/workspace path and assert that forbidden legacy directory names are not created.