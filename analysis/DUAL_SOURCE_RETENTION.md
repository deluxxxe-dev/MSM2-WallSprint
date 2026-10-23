# v0.1c1 dual-source retention design

The verified lifecycle/update side is the state writer. It publishes session, generation, active state, current Hero/Transform/owner, native thread/update, elapsed/progress, Surface N398, and the interpolated Right/Up/Forward basis through atomic 64-bit words guarded by an even/odd sequence. Readers retry a bounded eight times and never take the worker state lock. A separate atomic lifecycle gate closes admission immediately on Deinit/kill/end.

Path A consumes every exact qualifying `0x2CFC8F3 -> 0x2CFC580` call, writes its 48 orientation bytes, and keeps Position unchanged. Path B consumes exact `0x2CFC84B -> 0x2CFC3C0` calls, preserves each compact row magnitude, and writes only `[RDX,RDX+0x24)`. Both re-read the same publication and integrity guards before committing. Neither hot hook can reject with `state_lock_contention`.

The bounded hardware watch remains observational. Retention is counted only on distinct verified native updates after both sources have committed and transition progress reached 1. A loss below `dot(ActorUp,N398)=0.98` resets the continuous count. `ORIENTATION_RETENTION_PASS=1` requires 20 distinct updates or 300 ms in one still-active session. A synthetic PASS validates this accounting but is not game evidence.

No direct Actor, Position, adhesion, movement, animation, camera, physics, parkour, swing, or combat write is enabled.
