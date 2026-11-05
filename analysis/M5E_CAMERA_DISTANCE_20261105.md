# M5e: camera comes too close at WS entry (2026-11-05)

Owner: since M5b, the camera moves very close to the hero for a moment when WS starts (most visible when starting upward). It recovers afterwards.
Known: the WS camera uses the HeroCameraManager actor-relative branch (+0x304 = 1 during WS). Nearby +0x2fc/+0x300 floats and +0x305..+0x308 bytes look like transition/blend state (names UNKNOWN, analysis/M5_CAMERA_20261105.md).
HYPOTHESIS A: the switch to the actor-relative branch starts its blend from a near-zero offset.
HYPOTHESIS B: the actor's orientation rotates over 0.25 s at entry, and the actor-relative camera offset follows the rotating actor axes, passing close to the hero.

## M5e-1 diagnostic (read-only, no behavior change)
`WS_CAMERA_DISTANCE`:
- Once, picks up to 8 position-like slots in camera manager +0xC00..+0x1000 that are 0.5-20 m from the hero.
- Logs their distance to the hero at 10 Hz for 3 s after each WS camera engage (max 30 lines per entry, first 12 entries). Also logs a ground baseline every 3 s (max 20).
- Also logs +0x2fc, +0x300 and +0x305..+0x308.
Decision:
| result | next |
|---|---|
| one slot = camera position (baseline steady 3-6 m) and it dips at entry within 0.25 s | B: slow the entry rotation for the camera or delay the +0x304 engage until rotation completes (one change) |
| dip coincides with +0x2fc/+0x300/+0x305.. changing, independent of rotation | A: set/hold that blend value at engage (one change) |
| no position slot found | widen the range |

## M5e-1 result (log `MSM2-WS-v0.1c48-20261105-214216.log` + 16 s video, not committed)
Owner: 3 upward WS entries; right after entry the camera dives unnaturally to the hero's feet.
VERIFIED-RUNTIME (this build/session):
- Camera manager +0xD54 = camera position (the only position-like slot; ground baseline 4.1-8.7 m from the hero).
- +0x300 = engage blend: starts ~0.45-0.5 at engage and falls linearly to 0 in ~0.5 s. +0x306/+0x307 = 1 while engaged. +0x2fc stayed 0.
- All 3 entries: the distance falls with the blend 4.7 -> ~2.1 m by 0.5 s. At 0.94-0.97 s it drops to 0.5-0.96 m, then recovers over ~1.5-2 s.
- The drop time matches the end of the orientation override rotation (source entry at 0.703 s + 0.25 s rotation = 0.953 s). The video shows the camera ending up at the feet looking out at the sky, then returning behind the hero.
Conclusion: HYPOTHESIS B supported (OBSERVED-CORRELATION). The dive comes from the actor-relative camera following the actor axes as they rotate; the blend itself (A) does not cause it.
Change (M5e-2): the +0x304 engage is delayed to 1000 ms after the WS session start (a new session = >300 ms since the last WS update), so the camera switches after the rotation has finished. The camera is upright (vanilla) for the first second. Restore/F8/300 ms end rule unchanged. `WS_CAMERA_REF restore` now logs `engage_delay_ms` and `delayed_updates`. Distance diagnostic kept.
Decision: no dive and roll still smooth -> keep (maybe tune the delay). Dive remains, later than 1 s -> the camera smooths in actor space; next hold the camera position during the switch.

## M5e-2 result (log `MSM2-WS-v0.1c48-20261105-220802.log` + 12 s video, not committed) — REJECTED
Owner: still wrong.
- With the 1 s delay, the engage happened after the rotation. The distance still fell with the blend: 2.8 -> 0.82 m at 0.53-0.58 s after engage. It then recovered to ~3.4 m over ~1.5 s (2 entries).
- Hypothesis B is REJECTED: the dive does not depend on the actor rotating.
- Video: during the delayed first second the vanilla camera looks steeply up the wall. After the engage, the camera goes under the hero and looks out at the sky. It returns behind the hero while it re-centers.
- HYPOTHESIS C: the camera's pitch angle carries over unchanged into the actor-relative frame. "Looking up the wall" in world space becomes a steep upward look in the wall-as-floor frame. The orbit then places the camera below the floor (inside the wall), and collision pushes it in to ~0.8 m. Auto re-centering of the pitch brings the distance back.
Change (M5e-3): the engage delay is reverted to 0 (the 1 s variant is kept in code as `kEngageDelayMs`). Read-only `WS_CAMERA_PITCH_SCAN`: on the ground (no WS for 1 s), every 50 ms, each float in manager +0..0x1000 is compared with the pitch of +0xD34 (radians/degrees, both signs). Candidates with rms < 0.12 rad are listed, given a pitch range > 0.6 rad and at least 60 samples (max 8 logs).
Decision: a pitch slot found -> M5e-4: set it to level (0) at engage (one change). None -> the pitch is stored in another form (e.g. a quaternion); widen the search.

## M5e-3 run (log `MSM2-WS-v0.1c48-20261105-223600.log`, not committed) — no camera diagnostic output
- 2.5 min session, ground only (no WS), hero available 13:37:05-13:38:29. `WS_CAMERA_REF admitted=1`.
- Zero `WS_CAMERA_PITCH_SCAN` lines and zero `WS_CAMERA_DISTANCE` lines. The ground baseline distance had logged in the two earlier runs. The log does not show the cause (the camera manager pointer is not logged without WS). It also does not show whether the deployed script was this build.
Change (M5e-3b): `WS_CAMERA_DIAG_STATUS` every 5 s (max 40): installed/killed, recorded manager pointer, hero pointer/position, camera forward read, number of distance slots, pitch samples/range/logs, engaged. The new key also proves which build is deployed.

## M5e-3b run (log `MSM2-WS-v0.1c48-20261105-225547.log`, not committed) — wrong camera instance recorded
VERIFIED-RUNTIME: `WS_CAMERA_DIAG_STATUS` showed that the recorded manager (last caller of the camera entry) had +0xD34 = (0,0,0) the whole session, across two manager addresses after a reload. The hero was valid. So the recorded `this` was not the live camera, and every camera diagnostic and use stayed silent.
Consequence: the camera entry is called by more than one manager instance. The single "last caller" pointer only works when the live camera happens to call last (as in earlier runs). The same pointer also drives the WS +0x304 roll and the camera-relative stick mapping. It is a latent bug for those too.
Change (M5e-3c):
- Thunk: still a leaf tail-jump. It records every caller into an 8-slot ring and a raw pointer, using only volatile R10/R11, no stack.
- Worker (100 ms): selects the live manager among the distinct recent callers: unit forward +0xD34 and position +0xD54 within 30 m of the hero, nearest wins. The current choice is kept while it stays valid. `WS_CAMERA_SELECT` is logged on each change. If the engaged manager is replaced, +0x304 is restored first.
- `ws_camref_this` is now this selected manager, so all existing consumers (stick mapping, roll, diagnostics) use the live camera.
Owner test unchanged: ground camera pitch sweep, no WS.
