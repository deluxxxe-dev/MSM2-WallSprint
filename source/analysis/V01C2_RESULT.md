# v0.1c2 real-game result used by c3

VERIFIED from the checked-in runtime handoff: the accepted build passes the complete Capstone compact proof (writer444, caller128, nine pairs). Both Path A and Path B supply the SurfaceFrame basis to the real Hero on A and diagonal E. Visible sideways rotation is confirmed, and Path A state-lock contention is zero.

Retention FAILS: Surface Up repeatedly returns to World Up within a few native updates / a few tens of milliseconds. ORIENTATION_RETENTION_PASS is absent. The primary-only hardware watch sees known Path A Surface writes, but misses the reset visible in worker samples. Late World-Up Path A calls are already converted.

A writer executing on another process thread is the strongest current CANDIDATE explanation. Its function/caller identity is UNKNOWN; no third path is authorized for writes. c3 therefore retains both verified source overrides and expands the diagnostic watch.

See V01C2_RUNTIME_RESULT.md for the supplied measured facts. No private game log or video is copied into this result.
