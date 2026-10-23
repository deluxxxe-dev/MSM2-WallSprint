# v0.1c3 result carried into c4

Authority: `V01C3_RUNTIME_RESULT.md`, imported unchanged from main. Published code base: b05278093b6e0ad92db7dad8c51a986ba16ca872 (implementation fd105be35a12c4a4ba8b8a72c3f75a4bcf49ff82).

VERIFIED REAL GAME: accepted executable, compact proof/admission and both A/B surface-relative overrides remained alive. Path-A state-lock contention was zero. The wider control wall measured N398 approximately (-0.113317,0,+0.993559), not exact cardinal A. Visible flicker remained; orientation retention did not pass.

VERIFIED REAL GAME: primary-thread hardware capture worked, but the final process watch had seen=0, eligible=0, installed=0, rescan_count=0. No unknown writer/event/identity was captured. This establishes absent expanded coverage, not absent third writer.

VERIFIED CODE / OWNED REGRESSION: c3's common-record overflow unconditionally clears capture. Its next coordinator poll releases the window before checking the >=850ms expansion condition. Replaying 1100 matching entries against the exact published c3 source reproduces capture=false and seen/eligible/installed=0 without calling Toolhelp. Event-budget/ring exhaustion also closes capture. Earlier c3 fixtures began at elapsed900/1200 and expanded immediately, so they did not exercise pressure during the .60-.85 pre-expansion interval.

UNKNOWN: which precise early-stop condition occurred in the user's c3 run. The supplied summary has neither stop reason nor attempt/error telemetry; it cannot distinguish overflow, budget, lifecycle-read/freshness loss, identity/guard loss or deadline/scheduling. Do not label the demonstrated overflow mechanism as proven real-game causality, or assert a Toolhelp/OpenThread error that was never recorded.

c4 repairs the demonstrated silent pressure-stop paths and instruments coordinator exits and actual enumeration. Real-game enumeration, writer identity and retained ownership remain separate pending gates. No third-path patch or later milestone is authorized.
