# M16 owner result, part 1 (2026-11-12, build fe01adf, log MSM2-WS-v0.1c48-20261112-084028, up to test 4)

Inspected in full: the log (17084 lines) and the video clip.mp4 (9.6 s, 4 frames/s over the whole duration).
The owner will send the log for tests 5 onward separately; the M17 task is written after that.
Terms: WS = Wall Sprint, WR = vanilla Wall Run.

## Owner report
- Test 4: a right stick jump went through the face it should have landed on.
- The stop from WS ends in the crawl again (stands only after a jump).

## 1. Falling through the face (VERIFIED-RUNTIME)
- 23:45:30.584 right jump (walking takeoff), normal arc, landed on the face at 31.230 (travel 3.3 m, plane_d 0).
- From 31.34 the game moved the hero down in world y every frame with rising speed (fall guard: 9.4 -> 16.6 m/s,
  `speed_over_9`) although the hero stood on the face: after a WallSprint face-jump landing the game can stay in its own
  air/fall state. The video shows the hero on the glass face at 1.0 s.
- 23:45:33.449 the M16 walk-off rule fired (`world_down_accelerating_four`, plane_d -0.008, v_plane 4.89 = walking). The
  released fall moved the hero along -u, i.e. INTO the face: plane_d -0.17 / -0.57 / -1.21 m at 0.08 / 0.16 / 0.25 s.
  The game pushed back along +u (game_step_u 0.05 -> 0.34 -> 0.92 m/frame): its collision answered, but the release
  ignores game steps.
- 33.763 `land reason=game_fall_ended` re-placed the plane through the current position (inside the face); 34.247 a new
  release from plane_d -0.219 continued into the building; the fall then ran 26.8 s to plane_d -366 m (video: camera
  passes the glass at 1.25 s, then the building interior and the city from below).
- Earlier releases (21.848, 27.213) were also right after face-jump landings and ended after 0.02-0.06 s.
- Conclusion: the game's world-down fall signature does not mean "no ground under the hero" on a face; it also appears
  when the game keeps an air state after our face-jump landing. A release toward -u without a contact check passes
  through the face. The game's +u push-back is a usable "surface here" signal; re-placing the plane at a penetrated
  position must never happen.
- Safety for the current build: until fixed, a fall through a face can occur after face jumps.

## 2. Stop ends in the crawl again (VERIFIED-RUNTIME + owner)
- 4 stop hops with M16 parameters: d0 = 0.14 / 0.40 / 0.27 / 0.37 m, apex d0 + 0.15 m, airtime 0.355-0.366 s.
- Comparison: M14 (peak ~0.25 m, 0.2 s) crawl; M15 (d0 0.45 -> apex ~0.95 m, 0.42 s) stands; M16 (apex 0.29-0.55 m,
  0.36 s) crawl. With airtime about equal in M15 and M16, the distance from the face (apex) decides: the game leaves its
  wall-contact state only somewhere between 0.55 m and 0.95 m (OBSERVED-CORRELATION, 3 builds).

## 3. Face jumps and facing (VERIFIED-RUNTIME)
- Walking jumps: arc unchanged (1.8 m, 0.645-0.653 s, travel 3.3 m), directions right.
- `WS_FACE_AIR_FACING`: writes 0-36 per jump, overwritten_next_call 0 in face jumps (one stop hop 22 writes / 1
  overwrite / max 112.8 deg). The rotation write holds the rows when used. Owner judgement of the body turn is pending
  (part 2).

## 4. Snapshot (VERIFIED-RUNTIME)
- Full coverage reached (F 60, W 60, J 52, H 12, G 60, A 60, L 60): candidate_count_A = 0 and candidate_count_H = 0. No
  4-byte field in hero bytes 0x000-0xFFF separates ground {F, W, G} from the game's airborne {A} or from {H}. The ground/air
  flag, if any, is not in the first 4 KiB of the hero object. Do not repeat this search there.
