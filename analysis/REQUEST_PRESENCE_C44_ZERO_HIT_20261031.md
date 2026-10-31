# RequestSimpleMotion c44 actual-game zero-hit result — 2026-10-31

Branch: `codex/v01c20-edi-value-provenance`.

## Result

`v0.1c44 / package 0.1.69` successfully admitted the exact read-only RequestSimpleMotion entry hook on the pinned game build and continuously persisted 500 ms worker batches during actual gameplay. Across the complete supplied log there were **335 periodic batches and zero captured Request entry records**.

This supersedes c43's inconclusive missing-record result. C44 persistence was active and repeatedly wrote empty batches before, during, and after verified gameplay, so this is valid bounded zero-hit evidence for the exact hooked entry.

## Runtime-verified facts

Target build:

- Spider-Man2.exe `2.810.0.0`;
- on-disk SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- target-build gate accepted.

RequestSimpleMotion admission:

- entry RVA `0x11A1BD0`;
- function range `0x11A1BD0..0x11A1CA2`;
- whole-function SHA256 `2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c` matched;
- 7-byte / 2-instruction stolen span passed;
- `.pdata` UnwindData `0x94697E0` matched;
- all observer/restore unwind checks passed;
- MinHook trampoline validation passed;
- final `REQUEST_PRESENCE_ADMISSION admitted=1`;
- automatic script-session capture started with 500 ms drain / 16-record hot batch / 65536 session cap.

Persistence result:

- first periodic batch: `batch_id=1`, retained=0, overflow=0, contention=0;
- last periodic batch: `batch_id=335`, retained=0, overflow=0, contention=0;
- every observed batch retained=0;
- every observed batch overflow=0 and contention=0;
- no `REQUEST_PRESENCE_RECORD` line exists in the complete log;
- session retained count remained 0.

The periodic interval spans approximately `13:48:13.449Z` through `13:51:01.611Z` (~168.2 s after the first drain; capture started at `13:48:12.956Z`). This excludes c43's former shutdown/finalization ambiguity because empty batches were persisted continuously while the game was still running.

Verified WallRun activity occurred inside the same capture interval. Eight completed WallRun lifecycle sessions were logged. The main long session was session #8:

- Init=1 / Entry=1 / Update=1377 / Deinit=1;
- duration `17.240649 s`;
- verified lifecycle identity;
- heuristic worker classification predominantly `WallRunSide` (152 Side, 6 Mixed samples).

The Request periodic batches immediately around and after this session still retained zero records.

## Interpretation boundary

The exact entry at `0x11A1BD0` was not observed executing during this bounded actual-game session containing ordinary movement and verified WallRun activity.

This supports rejecting **this exact RequestSimpleMotion entry as a useful observation source for the tested vanilla locomotion sequence**.

Do not overstate this as:

- proof that no engine path can ever call the function;
- proof that every possible caller must enter through the exact function entry;
- proof of the function's semantic meaning beyond the accepted prior static/reference identity;
- proof about untested gameplay states.

No argument semantics were observed because there were no entry hits.

## Decision

Stop spending runtime cycles on RequestSimpleMotion presence for ordinary ground Sprint / WallRun. Do not enlarge its buffer, capture duration, shutdown logic, or add more argument decoding.

The existing motion/gravity discriminator design leaves `GravityOrientationOwner` as the next smallest presence-only question. Its accepted exact identity is already available from prior owner audit/static resolution:

- range `0x134EF00..0x134EFBB`;
- SHA256 `bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53`;
- `.pdata` UnwindData RVA `0x94A1CB8`;
- accepted 5-byte single-instruction prefix audit.

Next work should only establish whether that exact owner entry executes during the same ground Sprint / WallRun sequence. Preserve the c44 read-only streaming model and all gameplay writes OFF. Do not yet dereference owner arguments, call the setter, infer gravity-vector semantics, or modify movement/gravity state.
