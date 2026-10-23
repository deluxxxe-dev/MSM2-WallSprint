# c6 Path A Position proof — NOT ADMITTED

## Known identity and missing evidence

VERIFIED from existing exact-build guards and prior runtime evidence:

- Spider-Man2.exe2.810.0.0 / SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9.
- Path A writer0x2CFC580..0x2CFC7C8,584 original bytes, SHA3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6.
- Caller0x2CFC850..0x2CFC8F8,168 bytes, return0x2CFC8F3, SHA88502996d39173bf38086e8ddafd9eff2875dada30717d1c9f4651324fbe3b3f.
- Existing Path A runtime gate: exact caller, fresh current-Hero RCX, same native thread, current owner/session/generation/epoch, exact RDX=entryRSP+0x28, aligned writable current-thread stack, proper finite64-byte source matrix, R8null, current owned patches and kill state.
- Caller-local layout candidate Position Vec4 at source+0x30/34/38/3C. Existing orientation writes48 bytes and verifies all16 Position bytes unchanged. Source affine validation observes W near1; it does not prove destination W semantics.
- The existing read-only SDK obtains HeroPosition from Transform+0x30..0x38. This establishes its read layout, not the writer's complete data flow or side effects.

UNKNOWN: accepted-build complete Position copy/computation, its destination stores on every executable path, destination W/metadata handling, caller Position construction and propagation through downstream calls, and whether changing this source has only the intended Hero Position effect. The checkout/runtime summaries do not supply original584/168 instruction bodies for independent cloud replay. A hash, layout and orientation success cannot establish these missing facts.

## Implemented startup inspection

The source hook already authenticates/stores the original writer BEFORE its MinHook patch. A new internal snapshot API returns that immutable saved body only while current whole-writer ownership remains authenticated. The c6 startup inspector receives it with the independently authenticated unchanged caller. Hooked entry bytes are never decoded as originals. Failure affects Position diagnostics only; c5 orientation admission is unchanged.

Capstone5.0.6 (already static/pinned) must decode the complete584/168 spans contiguously, without skipdata. The inspector tracks exact move/LEA/stack facts and register clobbers, derives the exact caller E8 destination/return and RDX stack relation when supported, and records Position-range source-read/target-store candidates with width/relative instruction offset/source lineage. Arithmetic loses direct-copy lineage. Scalar/partial/SIMD alias writes invalidate affected facts. Calls clear volatile facts. Branches/calls are explicitly marked; the linear candidate scan is NOT path-complete control-flow analysis.

PATH_A_POSITION_PROOF reports admitted=0, individual original identity flags, decoded sizes, call/stack result, candidate component mask/counts and exact rejection reason. Position source/destination offsets remain unproven unless candidate accesses are actually found. PATH_A_POSITION_ACCESS caps24 rows; PATH_A_POSITION_CONTROL caps16 branch/call offsets/direct target RVAs. Suppressed totals are explicit. No raw instruction bytes or full disassembly/body is dumped or distributed.

Rejections distinguish missing original identity, wrong span, incomplete writer/caller decode, unproven exact call, unproven caller stack, unresolved control flow/downstream effects, incomplete/computed map, and unproven W/exclusive effects. Even a complete apparent16-byte copy cannot admit Position in c6. There is no Position store helper or hidden INI bypass in this revision.

## Runtime evidence and next proof

For the first2 successful Path A commits/session, PATH_A_POSITION_RUNTIME records source Position/W, read-only HeroPosition before native writer, exact return/RCX/native thread/RDX stack relation and source row preservation. This reuses the already authenticated c5 entry. It is before-original evidence, not causal/post-writer proof. Other callers, wrong threads, arbitrary heap sources or unauthenticated targets do not produce this record.

The next handoff needs the actual accepted-game PROOF/ACCESS/CONTROL/RUNTIME records to identify the unresolved Position slice. Then authenticate the caller's Position initialization, whole writer CFG and any downstream call effects, including W and exclusive target scope. A bounded private instruction slice may be needed after the exact failed edge/offset is known; no undirected raw-log prerequisite blocks c6. Do not infer a safe source extension from direct_component_mask=0xF or equal source/Hero coordinates alone.

## Independent tests

Owned584/168 instruction fixtures cover complete decode, exact E8 target/return/stack, direct16-byte copy candidate, computed store losing direct lineage, conditional flow, wrong call/stack, truncated VEX/span, and access/control caps. Every fixture remains admitted=0, including the apparent perfect copy. No game functions execute and no private original584 bytes are shipped. Actual accepted-build Position semantics remain UNKNOWN pending the new startup evidence.
