# c11 read-only Position writer attribution

## Scope and admission

C10 real A/E orientation passed (see V01C10_RUNTIME_RESULT); its native source admission/token/context and both A/B hot policy functions are unchanged. C11 never writes Actor/Position or changes caller Position. It extends the existing bounded hardware-watch engine with a distinct Position window; no new hook, game ABI, PAGE_GUARD, page protection change or native game call.

The existing helper is the sole coordinator for arming/polling/restoring Up and Position. A worker request requires full t1 Surface Up, retention PASS, source elapsed>=1.55 and<=3.0, same active ownership and fresh100ms publication. The helper rechecks live coherent native token phase/thread/owner/generation/init/accepted/collision, source permission epoch/session/Hero/Transform, worker verified identity/current patches, and a fresh SDK Hero/Transform before arming and every poll. Any failure closes diagnostics without killing otherwise valid orientation. Native Deinit/F8 cancel capture synchronously. Diagnostics do not publish Update or migrate expected native ownership.

Position cannot arm while any earlier watch lease/suspension remains or the previous window is current. Old Up timing, expansion threshold and byte range remain unchanged. Position uses two8-byte write slots at Transform+0x30/+0x38 (all16 bytes including unknown W),600ms maximum, once/session,8 Position windows/process separate from8 original Up windows. One existing process snapshot, at most128 threads, no rescan; coordinator excluded. Occupied DR0/DR1/general-detect is refused. Foreign DR2/DR3 and all saved unrelated bits are preserved. Setup/readback/rollback/restoration reuse the existing guarded Suspend/Get/Set/Resume machinery; no allocations/logs/SDK calls while suspended. Cleanup never suspends its own coordinator; unresolved leases remain reported pending rather than hanging or claiming restoration.

## Records and bounds

Every captured Position write, including unchanged values, contributes to raw bounded evidence (cap256/window). Contention/ring pressure or deadline closes the Position window. The worker aggregates at most16 PC+caller identities/window, prints at most4 events per identity and hit summaries; overflow/drops and incomplete thread coverage are explicit. This is bounded sampled attribution, not proof that no other writer exists.

POSITION_WRITE_EVENT records post-instruction RIP/module RVA/unwind function/caller1/caller2, slot mask, expected Transform, previous serialized Position4 snapshot/current guarded Position4 snapshot, raw fourth-component bits, world delta and SurfaceFrame components, timestamps relative to successful A/B source commits. Before is NOT atomic instruction pre-state; after is NOT function-return state. Source commit timestamps are diagnostic only and never acquire native ownership. The B timestamp update is gated by successful existing B admission and matching target; primary A request identity remains pinned. LastError is preserved for Position VEH and timestamp callbacks.

Per unique identity, at most1024 bytes inside a runtime unwind function boundary are read/fully decoded by the existing pinned Capstone decoder, hashed and inspected for the exact instruction ending at RIP-after. Derived store width/base/displacement/effective address and control counts are logged, never full function bytes or disassembly. Unknown runtime bodies have original_body_verified=0: dynamic hash/boundary is identification, not accepted original or upstream-write permission. Owned Path A patched bytes are not decoded as originals; its authenticated pre-hook originals are inspected at startup. Missing boundary/oversize/decode/unsupported address gives a named blocker.

Final summaries drain on the worker only after queued events/inflight handlers are clear; cleanup pending is reported truthfully. Retired VEH lease tombstones remain resident until process exit exactly as the original engine requires. No persistent process debugging or expansion beyond the one bounded snapshot.

## Evidence status

VERIFIED SYNTHETIC: two independent owned Position writers, exact post-store Vec4 copy and later computed/overwriting/W-mismatch writer, anchored effective store decode, per-writer aggregation/cap, raw256 limit, foreign DR0 refusal/DR2 preservation, full debug-state restoration under timeout/cancel/F8, unchanged orientation/page protection. Production c10 policy and actual MinHook A/B run with real Position watch on cardinal/diagonal targets through normal lifecycle; no source Position write.

ACTUAL GAME: c11 Position writers/caller chain are NOT YET CAPTURED here. The human A/E log is required. No game identity is inferred from an owned fixture. If watch capacity/identity/freshness prevents capture, ship/report the exact rejection rather than widening instrumentation. Never infer exclusive effects or absence of downstream writers from a short, incomplete-coverage window.
