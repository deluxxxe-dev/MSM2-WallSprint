# Distribution risk register

Owner decision 2026-11-04: shipping modified vanilla config assets is **permitted** for WallSprint.
This file is the single place to record anything that could make a release risky, so a pre-release check
is one pass. Add a row when a concern is found; do not delete rows, mark them `RESOLVED` with a date.
Rows are concerns, not accusations: say what is known and what is unknown.

| ID | Date | Area | Concern | Status |
| --- | --- | --- | --- | --- |
| D1 | 2026-11-04 | Modified vanilla configs | Packages containing edited `WallRun*Setup*.config` include derived Insomniac/Sony game data. Owner permits it, but Nexus Mods content rules and the game's EULA were not reviewed here. | OPEN: check Nexus rules before upload |
| D2 | 2026-11-04 | Whole-file replacement | A config asset replaces the whole vanilla file. Other mods that edit the same asset (e.g. WallRun speed/steering mods, Unnerfed II) will conflict or override each other. Release notes must list the asset paths. | OPEN |
| D3 | 2026-11-04 | Game updates | The edited configs are byte patches of build v2.810.0.0 (pinned SHA256 in `source/tools/build_wallrun_m1_experiment.py`). After a game update the replacement can silently revert or break WallRun data. Builder refuses non-matching inputs. | OPEN |
| D4 | 2026-11-04 | Provenance of inputs | Release builds must be generated from files the owner extracted, never from files copied from other mods. Third-party mod config bytes must not be redistributed (`DEV_RULES.md` reference-MOD rule still applies). | OPEN |
| D5 | 2026-11-04 | Experiment packages | `MSM2-WS-M1-DownLimit` is an experiment, not a release artifact; keep it out of Git and out of `Mods Library` roots scanned as WS content (see `analysis/STEERING_PROBE_RETIREMENT.md`). | OPEN |
| D6 | 2026-11-04 | Field-name mapping | Names for speed-config fields are inferred from alphabetical order, not decoded. Documentation of field meaning must be labelled HYPOTHESIS until runtime-confirmed. | OPEN |
| D7 | 2026-11-04 | Third-party notices | Existing `THIRD_PARTY_NOTICES.md` covers MinHook/Capstone. Re-check licenses (Capstone/MinHook/GPL files under `analysis/native-probes`) before any public ZIP; probe EXEs and GPL text are in Git history. | OPEN |
| D8 | 2026-11-04 | Repository size | ~97 ZIP files are tracked in Git (`.git` ~200 MB). Public release assets belong in Releases, not the repo. | OPEN |
| D9 | 2026-11-04 | Auto-deployed stage | `Update-WS` writes `MSM2-WS-Assets\MSM2-WS-WallRunSteering.stage` (edited vanilla steering config) from a file the owner extracted into `MSM2-WS\_inputs\`. A public release needs a different, rights-reviewed path to supply that file (see D1/D4). Other mods editing the same steering asset conflict (D2). | OPEN |
| D10 | 2026-11-04 | Vanilla animation clips | M2a stage ships copies of vanilla sprint animclips under WallRun clip paths (game data). Same rules as D1/D4; conflicts with other mods replacing these clips. | OPEN |
| D11 | 2026-11-05 | Native gameplay write | M3c writes WallRun.this+0x240/+0x24C (heap, not code). Experimental; offsets are build-specific (2.810.0.0). Must not ship enabled without stick-driven logic and a user toggle. | OPEN |
| D12 | 2026-11-05 | Reference mod | ActorRelativeCamera (ButtersTM) used only as a research hint/test companion. WS must not bundle, require or copy it; own implementation must be independently derived. | OPEN |
| D13 | 2026-11-05 | New code hook | WS now hooks the camera reference update entry (0x1065680). Conflicts with other camera mods (e.g. ActorRelativeCamera hooking the same function) are likely; release notes must say not to combine. | OPEN |
| D14 | 2026-11-05 | Write cap | Orientation override per-session write cap raised 4096 -> 262144 (~58 min). Still bounded; if a report mentions problems after very long wall runs, check this cap first. | OPEN |
| D15 | 2026-11-05 | Modified vanilla config | WS stage ships a modified camera_wallrun_aimcontext.config (offsets/pivot from vanilla standard traversal). Same policy/risk as the steering config. | OPEN |
| D16 | 2026-11-06 | Camera write | WS writes the camera manager rows +0xD24..+0xD54 (yaw follow toward travel) during WS only. Interaction with other camera mods likely; release notes must say not to combine. | OPEN |
| D17 | 2026-11-06 | Modified vanilla config (global) | M6p experiment herofollowcamconfig VelocityFollowData.Enabled=false (would change ground camera). Rejected and disabled (FOLLOW_ENABLED=False); nothing shipped. | CLOSED |
| D18 | 2026-11-06 | New code hook | WS hooks LookCameraMover reference setter (RVA 0x3350220, pinned entry SHA) and rewrites one argument vector during WS (auto-centering off). Conflicts with other camera mods are likely. | OPEN |
