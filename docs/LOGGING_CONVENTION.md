# WallSprint logging convention

WallSprint diagnostic logs are isolated from other Spider-Man 2 MOD logs.

## Directory
Relative to the game root:
```text
log_wallsprint\
```
Example current install:
```text
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\log_wallsprint\
```
Do not hard-code the absolute install path. Derive the game root and append `log_wallsprint`.

If the directory is missing, create it safely. Do not silently fall back to the old shared `logs` directory.

## Filename
One file per WallSprint process/run:
```text
WallSprint_YYYYMMDD-HHMMSS.log
```
Example:
```text
WallSprint_20261024-034812.log
```
Use local PC wall-clock time, zero-padded. Keep A→E sessions from the same game run in the same file. If a same-second filename collision occurs, append `_01`, `_02`, etc.

## Human test convention
Normal diagnostic test:
```text
A ~4 s -> fully end WallRun -> move/fast-travel -> E ~4 s
```
Send the single newest timestamped file from `log_wallsprint`. Video is only needed for a visible anomaly or when specifically requested.
