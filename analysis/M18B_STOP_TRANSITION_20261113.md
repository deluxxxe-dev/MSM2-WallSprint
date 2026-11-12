# M18b P1 stop correction, 2026-11-13 JST
Baseline6c771b7. Read M18 owner result fully and M17 section1; no raw log/video
attached to this cloud turn. The supplied derived evidence describes full log/
video inspection; no claim of independently viewing those attachments. Unchanged
repository/CLAUDE rules apply. P2-P6 remain deferred. Seamless motion and retained
changed gravity remain mandatory. M18 release guard/deceleration/normal hop arc
retained; owner standing acceptance preserved as evidence, new outcome UNKNOWN.

Step1: explicit0.15s pending-stop debounce after WS-active expiry (the existing
active predicate already has150ms callback freshness). Poll-independent callback
identity cancels a resume even if the next100ms telemetry poll misses it. Capture
Transform, projected velocity and forward at the pending end; debounce must not
age the carry out. <=60 kept/cancelled lines. Jump-release holds retain their
existing immediate path. No hold_start/hop on cancelled pending stop.
A new actual WS callback cancels an active stop hop/rotation immediately, releases
rows without blend and disables that stop force before another hold is admitted.
A confirmed stop bypasses the old WR-freshness pause (only stop): continuing WR
callbacks must not stretch the stop arc. Existing non-stop WR/jump routes remain.
Owned tests cover cancellation, missed resumes, exact boundary and single keep.
Test helper extraction initially duplicated a nested marker; fixed extraction,
no assertion relaxation. Production Werror compile passes.
