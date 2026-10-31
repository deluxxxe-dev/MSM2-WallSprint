# GravityOrientationOwner c45 zero-hit and c46 WallRun Update scalar plan — 2026-10-31

Branch: `codex/v01c20-edi-value-provenance`.

## c45 runtime-verified facts

Actual-game `v0.1c45 / package 0.1.70` ran on the pinned Spider-Man2.exe 2.810.0.0 build with SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.

The exact `GravityOrientationOwner` entry observer fully admitted:

- range `0x134EF00..0x134EFBB`;
- whole-function SHA256 `bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53`;
- 5-byte / one-instruction prefix;
- `.pdata` UnwindData `0x94A1CB8`;
- split observer unwind and MinHook trampoline validation passed.

The automatic worker-side capture remained live for 418 persisted 500 ms batches. All 418 reported retained=0, overflow=0, contention=0 and session_retained=0. No Gravity presence record was emitted.

A verified WallRun occurred entirely inside that capture interval: Init=1 / Entry=1 / Update=1470 / Deinit=1, duration about 16.280694 s. Gravity capture remained zero-hit through that session.

Accepted interpretation: the exact `0x134EF00` entry was not observed executing during the bounded supplied ordinary locomotion / verified WallRun session. This does not prove non-use in every possible game state.

Decision: combined with c44 RequestSimpleMotion zero-hit, stop the ScriptedFlight-derived Request/Gravity presence pair as the ordinary vanilla locomotion observation route unless a materially different state supplies a new reason.

## c46 static evidence

The verified native WallRun Update function is already authenticated independently:

- RVA `0x1424530..0x14246D6`;
- SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`.

Minimal disassembly of the exact c45-captured body establishes:

- Update+`0x2A`: `mov rsi, rcx`, preserving the WallRun `this` pointer in RSI;
- Update+`0x55`: 4-byte scalar compare from `[rsi+0x250]`;
- Update+`0x98`: 4-byte scalar compare from `[rsi+0x208]`;
- Update+`0xDE`: second 4-byte scalar compare from `[rsi+0x250]`;
- Update+`0x136`: 4-byte scalar compare from `[rsi+0x220]`;
- Update+`0x17A`: 4-byte scalar compare from `[rsi+0x238]`;
- Update+`0x199`: 4-byte scalar compare from `[rsi+0x40C]`.

Thus the five unique direct `this` scalar-read candidates are exactly:

`0x208, 0x220, 0x238, 0x250, 0x40C`.

This is static direct-read evidence. It does not establish which branch executes on each Update, which serializer field produced a value, or what any value means.

## c46 runtime design

Observe only those five 4-byte locations from the already authenticated active WallRun owner, using the existing worker-side ~100 ms object observation gate. No additional native hook is introduced.

Per field:

- read raw 32-bit value and float interpretation;
- log first sample and later raw-bit changes only, with a bounded total change-log count;
- summarize attempts, successful reads, change count, first/last/min/max and stability;
- count exact numeric equality with the previously recovered serializer numbers `0.4`, `12`, `75`, `100`, `50` only as numeric comparison evidence.

Do not label a field `TurnMaxDown`, `TurnSpeed*`, or any other semantic name based on numeric coincidence alone.

## Safety

C46 keeps all gameplay writes OFF. The c45 Gravity presence hook is disabled in the active INI. Scalar observation performs direct bounded reads only and no pointer dereference, game call, native patch, scalar write or transform/gravity/movement modification.

## Next runtime discriminator

One verified WallRun is enough after c46 full updater pipeline PASS:

- left ~3 seconds;
- right ~3 seconds;
- down ~5 seconds;
- naturally end the WallRun.

The result can distinguish stable configuration-like direct-read values from fields that change with steering. Only after a candidate has stronger runtime linkage should any controlled write/A-B test be considered.
