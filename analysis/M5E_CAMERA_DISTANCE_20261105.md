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
