# M2 animation — first plan (2026-11-04)

## Clues already in the repo
- `analysis/WSU_REFERENCE_REVIEW_20261031.md` (VERIFIED-STATIC, reference mod): WSU's `wallslide_down.stage` ships the steering config with
  TurnMaxDown 12->88 (same mechanism as our M1) together with `wall_slide_dn_spiderman.animclip`; WSU also ships `webrun_drivers.stage`
  replacing `Spiderman_Traversal.animset`. So asset-level clip/animset replacement is a practiced route for WallRun visuals. No WSU bytes are used.
- Owner asset search (2026-11-04): ~380 `wall_run` / 114 `wallrun` assets, mostly `characters/hero/hero_spiderman/animations/*.animclip`
  (archive `animsetclips`). Candidate base loop: `wall_run_spiderman.animclip` (17.5 KB). Others: `wall_run_45_*`, `wall_run_up_*`, `wall_run_angle_*`, `wall_run_idle*`.

## First experiment (M2a, one change)
Replace the bytes at the path `characters/hero/hero_spiderman/animations/wall_run_spiderman.animclip` with the owner-extracted vanilla
ground-sprint loop clip (path to be chosen from a `sprint` search). Observe sideways WallRun only.
| Observation | Meaning | Next |
| --- | --- | --- |
| sprint cycle plays during sideways WR, body still on wall | clip swap works | extend to up/down/45 clips |
| no visible change | WR uses another clip / animset | try `wall_run_idle` or animset route |
| broken pose / T-pose / crash | incompatible clip format | revert; animset route |
HYPOTHESIS: which clip is the sideways loop; whether a ground clip is compatible with the WR state.

## Chosen pairs (owner `sprint` search, 2026-11-04)
- Peter: `characters/hero/hero_spiderman/animations/stand_sprint_fwd_spiderman.animclip` (20.8 KB) -> `.../animations/wall_run_spiderman.animclip` (17.5 KB)
- Miles: `characters/hero/hero_spiderman_milesmorales/animation/base/stand_sprint_fwd_spiderman_miles.animclip` (20.1 KB) -> `.../animation/wall_run_spiderman_miles.animclip` (20.1 KB)
Implemented as optional entries in `source/tools/ws_assets.py` (`CLIP_SWAPS`); the owner places the extracted sprint clips in `MSM2-WS\_inputs\`.
No hash pin (files not yet seen); size sanity only; the deploy log prints each clip SHA256 prefix.

## M2a result (owner, 2026-11-04) — VERIFIED-RUNTIME (owner report + screenshot)
Sideways WallRun plays the sprint cycle for Peter and Miles; no detach/collapse. But the body stays world-upright (feet not on the wall);
upward WallRun keeps the vanilla clip (different clip, not swapped).
Cause found in config: the shipped c46-c48 INI had `WALLSPRINT_ENABLE=0` / `NATIVE_SOURCE_OVERRIDE=0` (log: `native_source_config enabled=0`),
so WS's accepted orientation substitution was OFF. The vanilla WR clip carries its own sideways body pose; the sprint clip does not.
## M2b (one change): default INI ENABLE=1 + NATIVE_SOURCE_OVERRIDE=1
| Observation | Next |
| --- | --- |
| feet on wall, sprinting sideways | M2 core works; then swap up/down/45 clips |
| body tilted wrong way / upside down | adjust orientation source mapping (sign/axis) |
| no change | check log `native_source_config enabled=1 admitted=1` |
