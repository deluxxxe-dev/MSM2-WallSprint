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
