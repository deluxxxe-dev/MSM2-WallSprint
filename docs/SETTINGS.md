# WallSprint player settings (ModSettings preparation, owner 2026-11-06)

All player-tunable values live in one place: `source/include/msm2/ws_settings.hpp` (native) plus the config patches in
`source/tools/ws_assets.py`. The later ModSettings screen and its presets map onto these names.

## Live native settings (WallSprint.ini, read at game start)
| key | section | default | meaning |
|---|---|---|---|
| WALLSPRINT_CAMERA_AUTOCENTER | WallSprint | 0 | 1 = vanilla WS camera auto-centering; 0 = hold the view (right stick / mouse turn it) |
| TURN_RATE_DEG_PER_SEC | WallSprintSettings | 400 | WS travel/facing turn rate |
| MOUSE_MANUAL_MS | WallSprintSettings | 150 | after a physical mouse move, the camera is player-controlled for this long |
| STICK_MANUAL_THRESHOLD | WallSprintSettings | 0.2 | right stick magnitude counted as camera input |

## Config-baked values (need Update-WS; a live setting needs a native route, see FEATURE_ROADMAP)
| value | where | default | source of the default |
|---|---|---|---|
| WS camera distance behind | camera_wallrun_aimcontext Offset*.Z | 3.5 m | M6n (ground ~4 m measured) |
| WS camera height | Offset*.Y | 1 m (u8 slot, integers only) | M6h |
| WS camera pivot height | PivotHeight | 0 | M6n (world-up pivot made distance direction-dependent) |
| WS camera FOV | FieldOfView | 72 (vanilla) | unchanged; ground default UNKNOWN |
| TurnMaxDown / TurnGain* / MoveSpeedFast | WallRun steering | 90 / 0,0 / 9 | M1b, M1c, M6h (MoveSpeedFast is not the speed) |

## Planned (reserved, not implemented)
- Leave-wall gravity reset time (`gravityResetSeconds`), wall-jump values, real WS travel speed (control field UNKNOWN), camera follow rate for auto-centering ON.
