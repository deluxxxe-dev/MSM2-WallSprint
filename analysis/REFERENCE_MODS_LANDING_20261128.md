# Reference MODs for the face-jump landing / wall locomotion (2026-11-28)

Research hints only (AGENTS.md): no third-party code, binaries, assets, offsets or signatures enter WallSprint. Archives
were extracted into a private scratch directory and read with `unzip -l`, `md5sum` and `strings`; nothing was executed.

| Archive (owner upload) | SHA256 | Content |
|---|---|---|
| Scripted_Flight 1.2 zip | c1f43885f6138143ad54cf0d6d3be86b1e51b0abc27cc51155bb9de6e93794f7 | same as the earlier review (`SCRIPTED_FLIGHT_RESOLVER_MOTION_GRAVITY_REVIEW.md`): .modular -> ScriptedFlight.script -> ScriptedFlight.dll + animation stages; author ButtersTM |
| Walk_On_Walls_Mod-1002-V1 rar | 91dbf5412eb950eea74c2da9c352443f898d4602d9df186b6bab3b785065d579 | WalkOnWallsV1.stage (author vrfloopy) |
| Web_Swinging_Unlimited 3.0 zip | 287df65b5ac36513bddb0e5db876477622f03a654faceba25d6b59cde8cf1b9b | .modular -> sc_core.script (SwingCore.dll + component DLLs) + webrun_drivers.stage; author ButtersTM |

## Walk On Walls (VERIFIED-STATIC, reference artifact)
- Only 109 `.animclip` files under `characters/hero/hero_spiderman/animations/` (wall crawl / crawl clips, 2 under
  `traversal/`). No config, no script, no DLL.
- 77 clips are byte-identical (one replacement clip); others carry names like `Stand_Idle_Spiderman` /
  `Stand_Idle_L090_Spiderman`: the vanilla wall-crawl state is kept and its clips are replaced by standing clips.
- Consequence: wall movement, attachment, corner turns and crawl jumps stay the game's own crawl logic; only the pose
  changes. Limits stated by the author (360-degree turn not possible, conflicts with crawl-speed mods) follow from that.
- For WS: a different architecture (crawl state + upright clips) that gets native wall handling incl. corners, but the
  crawl jump leaves the wall with world gravity (owner must-have 2 not met) and replacing crawl clips changes vanilla
  crawling everywhere. Useful as the reference for the corner wrap and as evidence that the crawl state tolerates
  standing clips.

## Scripted Flight (VERIFIED-STATIC, reference artifact strings)
- Its landing uses the game's own landing transition: strings `Transition.goodGround.call->TransitionLand`,
  `policy=native-TransitionLand`, `worldDownSpeed=... height=...`, dependency `TransitionLand semantic=fall-setting-
  tracker-and-dive-momentum`, `reason=native TransitionLand rejected direct nose-down admission`.
- HYPOTHESIS for WS: the game's air state ends through a "good ground" check that then calls a native landing transition;
  our face-jump lag (game air 0.6-0.74 s after our landing, analysis/M21T_FACE_JUMP_20261128.md) means that check does
  not pass on the face.

## Web-Swinging Unlimited 3.0 / SwingCore (VERIFIED-STATIC, reference artifact strings)
- Has a wall "surface run" mode with native floor queries and crawl/stand handoff: `SURFACE RUN: support=wall
  nativeGround=...`, `SURFACE RUN: stop-to-crawl ...`, `CrawlStandAdmission`, `CrawlStandPosition`, `TransitionStand`,
  `TransitionCrawl`, `floorForceCrawlFlags`, `FloorRequestSync`.
- Names native functions (game symbol names in its diagnostics): `HeroTransitionManager::TransitionWallCrawlToJump`,
  `...TransitionWallCrawlToFall`, `HeroStateWallCrawlBase::InitForWallCrawling/DeinitForWallCrawling`,
  `HeroMoverManager::SetMoveProcsToStandard`, `TransitionWallRun ABI=(this,u32,Vec3*,u32,float)`.
- `MoverStandard good-ground timer unreadable`: the standard mover keeps a good-ground timer. HYPOTHESIS: this timer is
  the condition the air state waits for.

## Next step options (not implemented)
1. Read-only diagnostic: find the hero mover's good-ground timer by comparing mover memory between ground jumps (air)
   and grounded frames, then log it during our face-jump hold. If it stays "not grounded" on the face, a bounded write
   (INI, default off, restore) at our landing is the candidate fix.
2. Identify the native landing transition in the current build (needs the exact game executable; the cloud has none)
   and request it at our landing instead of waiting.
3. Corner wrap: compare with the native crawl corner turn (Walk On Walls shows the crawl state keeps working with
   standing clips).
Wisdoms synchronization: pending (cross-repository edit not authorized in this task).
