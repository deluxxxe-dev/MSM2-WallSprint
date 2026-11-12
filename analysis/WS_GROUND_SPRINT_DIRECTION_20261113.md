# Direction check: WS as the game's own ground sprint on the face (2026-11-13)

Owner (2026-11-13): making the hero stand with a small hop cannot be smooth; the stop should flow like stopping a ground
RT run.

## Why the hop exists (summary of evidence)
- WS is driven through the vanilla WR (wall run) state. When it ends, the game goes to its wall-contact (crawl) state, not
  to standing. The game switches to standing on the face only after an air phase it detects and a landing (M13-M18:
  crawl with no/low hop, stands with a hop apex of ~0.8-0.95 m).
- In the face-as-ground state (after such a landing) the game's own ground locomotion runs on the face: standing and
  walking in all directions are smooth and accepted (M11-M18).

## Proposed structure (HYPOTHESIS, to be checked by one owner test)
Enter the face-as-ground state once at the WS start (the hero is already in the air when it reaches the wall, so the
landing is natural), and let the game's own ground sprint (RT) do the running on the face. The stop is then the game's
own ground sprint stop, with no hop and no state change.

## Unknown that decides it
Does the game's ground sprint (RT) work in the face-as-ground state, or does RT on the face start the vanilla WR / the
mod's WS again?

## Owner test (current build, no code change)
1. WS, stop, and if the hero crawls, one A jump so it stands on the face (face-as-ground).
2. Walk 2 s with the stick, then hold RT with the stick pushed for 3 s, release RT and the stick, wait 3 s, B.
3. Repeat in two stick directions (along the face sideways, and up the face).
Return the log and one video.

## Decision table
- The hero runs on the face like on the ground and stops with the ground stop animation -> next task: WS start enters
  face-as-ground and RT on the face stays the game's ground sprint (the mod stops starting WR/WS while the hero stands on
  the face); remove the stop hop.
- RT on the face starts WR/WS (WS_SET_GRAVITY_HERO ws=1 / session_timeline) -> next task: suppress the WR start while the
  hero stands on the face and re-test the sprint.
- RT on the face does nothing different from walking -> look for the game's sprint condition (speed/state) before deciding.
