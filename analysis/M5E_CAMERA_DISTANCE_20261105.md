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
