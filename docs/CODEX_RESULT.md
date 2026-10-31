# Result — WallSprint v0.1c46 verified-WallRun Update scalar diagnostic

2026-10-31. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C45 ACTUAL-GAME GRAVITY OWNER ZERO-HIT ACCEPTED / SCRIPTEDFLIGHT MOTION-GRAVITY PRESENCE ROUTE STOPPED FOR TESTED VANILLA LOCOMOTION / C46 BOUNDED WALLRUN UPDATE SCALAR OBSERVER PUBLISHED / OWNER FULL PIPELINE + C46 RUNTIME PENDING.**

## C45 ACTUAL RUNTIME RESULT

The owner ran `v0.1c45 / package 0.1.70` on the pinned target build and supplied the runtime log plus video.

Runtime verified:

- exact target build accepted: Spider-Man2.exe `2.810.0.0`, on-disk SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- exact `GravityOrientationOwner` range `0x134EF00..0x134EFBB` admitted;
- whole-function SHA, 5-byte/1-instruction prefix, `.pdata` UnwindData `0x94A1CB8`, split observer unwind and MinHook trampoline all passed;
- automatic 500 ms streaming capture started;
- 418 persisted batches were produced, IDs 1..418;
- every batch retained=0, overflow=0, contention=0, session_retained=0;
- no `REQUEST_PRESENCE_RECORD target_role=GravityOrientationOwner` exists;
- one verified WallRun occurred while capture was active: Init=1 / Entry=1 / Update=1470 / Deinit=1, duration about `16.280694 s`;
- Gravity batches immediately before, during and after that verified WallRun remained zero-hit.

This is valid bounded zero-hit evidence for the exact Gravity owner entry during the supplied ordinary locomotion / verified WallRun session. It does not prove the function can never execute in other game states.

Combined with c44's exact RequestSimpleMotion zero-hit, the recovered ScriptedFlight Request/Gravity pair is no longer used as the ordinary vanilla locomotion observation route without a new state-specific reason.

## C46 NEXT OBSERVATION

The next evidence source is the already verified native WallRun `Update` body itself, not another reference-mod-derived function.

Exact authenticated Update identity remains:

- RVA `0x1424530..0x14246D6`;
- body SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`;
- verified lifecycle hook already executes during actual WallRun.

Bounded static review of that exact body shows `mov rsi,rcx` preserves the WallRun `this` pointer, followed by reachable 4-byte scalar comparisons directly from five unique `this` offsets:

- `+0x208`;
- `+0x220`;
- `+0x238`;
- `+0x250` (read at two sites);
- `+0x40C`.

These are static direct-read facts only. Their engine semantics are UNKNOWN.

C46 adds worker-side read-only observation of exactly these five offsets while the existing authenticated WallRun session is active and has recent verified native Update activity. It does not add a native hook. It logs the first bit pattern, later changes, and a bounded session summary.

For comparison only, summaries count exact numeric matches against previously recovered serializer values `0.4 / 12 / 75 / 100 / 50`. Exact numeric equality is not accepted as field-name or consumer-semantic proof.

## SAFETY / ACTIVE CONFIG

C46 version `v0.1c46`, package `0.1.71`.

- `WALLSPRINT_ENABLE=0`;
- `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`;
- orientation / adhesion / movement / gravity writes OFF;
- `REQUEST_PRESENCE_DIAGNOSTIC=0` so the c45 Gravity presence hook is not created;
- verified WallRun lifecycle/object diagnostics remain ON;
- scalar reads are direct bounded `owner+offset` 4-byte reads only;
- no new pointer dereference;
- no new game call;
- no scalar write;
- no semantic field names assigned.

The compiled c45 presence observer remains in source but dormant; its exact Gravity identity and assembly safety invariants continue to be checked so stale unsafe code cannot silently ship.

The new scalar observer also has a sanitizer/`-Werror` unit test in the normal full test chain. It checks first/change logging, unchanged-value behavior, exact numeric-hit counting and post-finish inactivity. A separate local warning-clean compile of the new header/test logic passed; this is not a substitute for the owner full Windows/WSL pipeline.

## C46 CHANGED FILES

- `source/include/wallrun_scalar_observation.hpp` (new)
- `source/include/msm2/surface_source.hpp`
- `source/src/wallrun_scalar_observation_tests.cpp` (new)
- `source/include/version.hpp`
- `source/mod/info.json`
- `source/mod/WallSprint.ini`
- `source/mod/presets/request_presence_read_only.ini`
- `source/tools/package.py`
- `source/tools/test.sh`
- `source/tests/request_presence_static.py`
- `README_TEST.md`
- `source/README_TEST.md`
- `analysis/GRAVITY_OWNER_C45_ZERO_HIT_AND_C46_SCALAR_PLAN_20261031.md`
- `docs/CODEX_RESULT.md`
- `docs/CODEX_NEXT.md`

No SocialFeed or other MOD repository/file is in scope.

## VALIDATION STILL REQUIRED

Chat/GitHub publication is not a local Windows/WSL build certificate. Run the installed `MSM2-WS/Update-WS.cmd` with game/shared Overstrike closed and require the full build/test/smoke/hooks/package/deploy PASS for c46 before actual-game testing.

After that PASS, one ordinary WallRun is enough: left ~3 s -> right ~3 s -> down ~5 s, then end the WallRun naturally. Return the c46 log and video. The acceptance question is whether the five authenticated direct-read candidates are readable/stable, whether any exactly equal the known serializer numbers, and whether any bit pattern changes in temporal correlation with steering. No write test is authorized yet.
