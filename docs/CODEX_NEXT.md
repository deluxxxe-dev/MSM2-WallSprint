# Next task — keep WS active across repeated reversals
Branch codex/v01c20-edi-value-provenance; no main merge.
Read AGENTS required docs and analysis/V01C38R3_REPEATED_REVERSAL_RUNTIME.md plus analysis/REVERSAL_START_TIMING_REVIEW.md.
Prior START review is complete; do not repeat it or CODEX_TURN_SPEED.

## Purpose
Owner explicitly requests WS remain active across any number of reversals while valid wallrun continues.
Supplied session2 completes3 reversals; session4 completes2; both close on fixed5second budget without stale episode.
Change ONLY normal WS lifetime, not reversal timing, duration, movement or animation.

## Bounded implementation
Inspect source/include/Math/source_orientation.hpp and source/src/NativeHooks/native_source_override.cpp (especially Transition.enter, tick fixed5000ms expiry, HardClosed reset), their actual consumers and relevant config/tests.
Replace mandatory experimental5second stop with session-bound continuation for normal WS: continuing authenticated valid native WallRun updates may retain the existing orientation override regardless of elapsed total time or reversal count.
Explicitly authorize this specific expiry-policy change; earlier preserve-five-second instructions are superseded ONLY for normal duration expiry.
Do NOT renew/reset lifecycle by reversal, invent new generations, re-enter HardClosed session, or bypass invalidation. Preserve native Deinit/owner/generation loss, missing or stale updates, invalid surface/basis, disable/emergency stop, exact-build/current code and all A/B destination/ownership guards and cleanup.
Separate total experimental runtime cap from freshness/watchdog deadlines: never remove the latter.
If some certification/publication lifetime is actually proven only within5s and cannot be revalidated by existing per-update path, stop with the exact code dependency; do not remove the guard blindly.
Keep bounded diagnostics bounded on long sessions (no unbounded memory/logging); exhausted diagnostic event quota must not be confused with gameplay lifetime. Preserve independent safety budgets and never expand hooks/watch permissions.
No new API/hook, Position/velocity/input/animation writes, C52 relaxation or c73.
Earlier facing START remains unresolved: no rotation acceleration, threshold/count/interval tuning, speculative1.1second offset, or new directional-input work in this task.

## Scope and completion
Allowed: minimum lifetime implementation/config needed, corresponding focused tests and docs, version/package bump per repository conventions, PROJECT_STATE/CODEX_RESULT and affected manual only.
Forbidden: SF/other mods, shared loader/environment/updater/profile changes, unrelated cleanup.
Use existing required build/portable/native/hooks/smoke/release commands from repo.
Meaningful regression cases: same valid session beyond5s/30s and many alternating completed reversals; valid straight session beyond5s; actual Deinit and disable immediately close; stale/gap/invalid owner/surface/code retain existing refusal/recovery semantics; closed generation cannot re-enter; subsequent genuine session starts cleanly; log caps stay bounded.
Record tests as synthetic/static, not in-game acceptance. Commit/push and verify. Report exact runtime version/log name and any failure.
Reply STATUS / CHANGED / RESULT / NEED only; Chat provides human update/test instructions.
