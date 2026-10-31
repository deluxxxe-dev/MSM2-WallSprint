# Result — WallSprint v0.1c44 Request presence runtime result

2026-10-31. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C44 ACTUAL-GAME STREAMING CAPTURE VERIFIED / EXACT REQUEST ENTRY ZERO-HIT VERIFIED FOR THE BOUNDED TEST / REQUEST OBSERVATION ROUTE STOPPED / NEXT TARGET IS GRAVITY OWNER PRESENCE ONLY.**

## C44 ACTUAL RUNTIME RESULT

The owner ran `v0.1c44 / package 0.1.69` on the pinned target build and supplied `MSM2-WS-v0.1c44-20261031-224811.log`.

Target-build and Request hook admission all passed:

- Spider-Man2.exe `2.810.0.0`;
- on-disk SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- Request range `0x11A1BD0..0x11A1CA2`;
- exact whole-function SHA matched;
- stolen span passed;
- `.pdata` UnwindData `0x94697E0` matched;
- all split observer/restore unwind checks passed;
- MinHook trampoline validation passed;
- `REQUEST_PRESENCE_ADMISSION admitted=1`;
- automatic 500 ms streaming capture started.

C44 persistence also worked as intended. The complete log contains 335 `REQUEST_PRESENCE_BATCH_SUMMARY` records from the live game session. Every batch reports:

- retained=0;
- overflow=0;
- contention=0;
- session_retained=0.

There are no `REQUEST_PRESENCE_RECORD` lines in the complete log.

This is no longer the c43 shutdown/finalization ambiguity. Empty batches were written continuously while the game was alive, including during verified WallRun gameplay.

Eight completed WallRun lifecycle sessions were present in the same capture interval. Main session #8 recorded:

- Init=1 / Entry=1 / Update=1377 / Deinit=1;
- duration `17.240649 s`;
- verified WallRun lifecycle identity;
- predominantly `WallRunSide` by the existing worker heuristic.

Request batches around and after that session still retained zero hits.

Detailed evidence: `analysis/REQUEST_PRESENCE_C44_ZERO_HIT_20261031.md`.

## ACCEPTED INTERPRETATION

Runtime-verified fact:

The exact hooked entry `0x11A1BD0` was not observed executing during this bounded actual-game session containing ordinary movement and verified WallRun activity.

Decision:

Do not spend more runtime cycles enlarging Request buffers, waiting longer, changing shutdown flush, or decoding Request arguments. The exact RequestSimpleMotion entry is rejected as a useful observation source for the tested vanilla ground-Sprint / WallRun sequence.

This does not prove that no engine state can ever call it, nor that indirect/interior entry paths are impossible. No argument semantics were observed because the entry produced zero hits.

## C44 SAFETY RESULT

The diagnostic remained read-only:

- `WALLSPRINT_ENABLE=0`;
- native-source override OFF;
- orientation/adhesion/movement writes OFF;
- no captured-pointer dereference;
- no Request argument modification;
- no request suppression.

No SocialFeed or other MOD repository/file was changed.

## NEXT BOUNDED TARGET

The previously accepted motion/gravity design leaves `GravityOrientationOwner` as the next smallest presence-only discriminator.

Accepted prior identity:

- range `0x134EF00..0x134EFBB`;
- SHA256 `bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53`;
- `.pdata` UnwindData RVA `0x94A1CB8`;
- 5-byte single-instruction prefix audit already available.

Next implementation must reuse the proven c44 bounded streaming observer model and change only the observed site/identity contract required for GravityOrientationOwner presence. It must not dereference owner arguments, call SetGravityVector, infer field semantics, or write gravity/movement/orientation state.
