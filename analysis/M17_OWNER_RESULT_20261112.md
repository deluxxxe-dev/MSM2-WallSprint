# M17 owner result and problem split (2026-11-12, build 7f3a39e/c8c3e27, log MSM2-WS-v0.1c48-20261112-183118)

Inspected in full: the log (17979 lines). No video was attached this time. Terms: WS = Wall Sprint, WR = vanilla Wall Run.

## Owner report
- Stop from WS: stands now; direction and position jerk at the stop.
- Jumps: the first two per direction worked; later jumps sometimes went opposite to the stick.
- Web zip: still wrong.
- Fall from a face: camera still does not follow; moving to other buildings in the changed gravity sometimes passes
  through walls.
- Owner decision: split the problems and fix/test them one at a time, starting with the stop jerk.

## Problem list (each with its measured cause; fix order chosen by the owner)
P1 Stop jerk (next task, M18) - section 1.
P2 Jump direction reversal - section 2.
P3 Web zip direction (no valid live camera source) - section 3.
P4 Camera does not follow during falls (no mirrored position found) - section 4.
P5 Passing through walls when falling onto other buildings - section 5.
P6 Left/right landing lag, camera jerk at jump takeoff (recorded only, not measured yet).

## 1. P1 stop jerk (VERIFIED-RUNTIME, 3 stops at 09:34:00.9 / 09:34:09.2 / 09:34:17.2)
Four separate motions:
a. Sudden in-plane stop: at the hop takeoff the hero still moves along the face (WS_JUMP_TRACE vel_h -4.3 to -5.0 m/s,
   -10.8 m/s at the WS end); the hop has zero in-plane velocity, so the motion stops in one frame (disp_h constant from
   takeoff on).
b. The hop: start 0.21-0.38 m off the plane, apex 0.8 m, landing at 0.355-0.359 s (needed for standing; crawl_suspect=0
   after the clean stop).
c. Rotation at landing: 0.08 s after the landing the hero up axis is (0.760, 0.644, 0.084) / (0.474, 0.863, 0.177), then
   world up (-0.002, 1.000, -0.004) at 0.19 s, back to the wall normal at 0.29 s. The forward changes from the WS travel
   direction (y 0.32 / -0.27) to the face-horizontal axis (y 0.02 / -0.05). The game's stand-up after the landing swings
   the body toward world up for ~0.2 s and turns it sideways. `WS_FACE_AIR_FACING` writes rows only until the landing.
d. A second jerk ~0.7 s later in 2 of 3 stops: the game moved the hero 0.31 m off the plane, then kept its fall state;
   the walk-off rule released (09:34:02.009, 09:34:18.228; 0.70 s after the landings, after the 0.5 s guard), probed 0.1 m
   into the face and landed by `contact` (original_plane_d -0.45 / -0.26).

## 2. P2 jump direction reversal (VERIFIED-RUNTIME)
- Walking takeoffs use the actual walking velocity at the press. When the stick was just changed, the hero is still moving
  the old way: 09:35:37.042 stick right -> takeoff dir side -0.98 (left); 09:35:51.870 stick down -> e_up -1.
- Standing jumps use a walk pair; the pair from 09:35:27 had theta 4.03 deg (correct ~+-177): stick up -> e_up +1 four
  times in a row (09:35:27-32), stick down -> -0.99.
- Two jumps travelled 16.6 m and 15.9 m in 0.64 s (09:35:38.7, 09:35:47.9): an in-plane speed of ~25 m/s at takeoff,
  not walking speed (not yet explained).

## 3. P3 web zip (VERIFIED-RUNTIME)
- `WS_CAMERA_MAP_CHECK`: no candidate of `WS_CAMERA_SETTER m0` reached validity; medians 64-115 deg for all 8 candidates
  (n up to 121). That matrix is not the live camera basis. Zips keep using takeoff/pair mappings (same faults as P2).

## 4. P4 camera follow (VERIFIED-RUNTIME)
- `WS_POSITION_MIRROR`: the only 12-byte copy of the Transform position found in hero bytes and 64 pointed objects
  (depth 1) is the Transform itself (+0x30, distance 0 in falls). The position the camera follows is not within that
  search; the camera source remains unknown.

## 5. P5 pass-through (VERIFIED-RUNTIME)
- Many walk-off releases with `probe_clean` then `land reason=contact` 0.3-9 s later; the last fall (09:37:25.0) ran 20.8 s
  to plane_d -611 m until B. Contact detection relies on a +u push-back from the game; a surface of another building
  that is not perpendicular to u does not push back along u, so the fall passes through it.
