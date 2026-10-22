# v0.1b7 gate

| Gate | Status |
|---|---|
| Existing source path >=.70 s, same exact build/caller/stack/Hero/native lifecycle | Preserved |
| Primary thread requested >=.60 s, deadline native Init+1.10 s | Implemented; scheduling/actual game prearm pending |
| Startup enabled/admitted/common hook/primary support self-check | Required; failure explicitly logs V01B7_DIAGNOSTIC_NOT_ADMITTED |
| First source override already watched | Measured per session by PREARM_STATUS; never assumed |
| All current-Hero common writer callers | Bounded read-only records; invalid source flagged |
| Surface->WorldUp reset / same A/E writer+caller | **UNKNOWN until actual game A/E capture** |
| RESET_WRITER_VERIFIED | **0** |
| Retained surface orientation | Not established |
| Reset writer NOP/skip/global force | Prohibited, absent |
| Direct Actor/Position/adhesion/movement/animation/camera/physics writes | Disabled, no additions |
| New native targets or guessed ABI / reference MOD payload | None |
| Next milestone | Not authorized |

See docs/CODEX_RESULT.md for current cloud validation; Wine independent fixtures cannot establish game success. Optional private game-capture replay is explicitly SKIP when absent. Preserve test failures separately from passed checks.
