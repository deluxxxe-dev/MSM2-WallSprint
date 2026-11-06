#pragma once
#include <atomic>
#include <cstdint>
// M6b: the WS stick-driven travel direction, written on the game thread by the WallRun pre-original callback
// (actor_writer) and read by the orientation worker (native_source_override) so the facing follows the
// commanded travel instead of a lagging displacement estimate. C++17 inline variables: one object program-wide.
namespace ws::drive {
inline std::atomic<float> dir[3]{};
inline std::atomic<std::uint64_t> ms{0};
inline std::atomic<std::uintptr_t> owner{0};
// M6g: hero position published by the orientation worker (pivot for the WS camera follow experiment).
inline std::atomic<float> heroPos[3]{};
inline std::atomic<std::uint64_t> heroMs{0};
// M6o: WS camera auto-centering (yaw follow + aim correction). Default OFF like the owner's ground camera setting;
// WallSprint.ini WALLSPRINT_CAMERA_AUTOCENTER=1 turns it on (ModSettings later).
inline std::atomic<bool> cameraAutoCenter{false};
// M8c: WS-only in-memory jump config write (YBoost 0 during WS). WallSprint.ini WALLSPRINT_JUMP_CONFIG_WRITE, default 0.
inline std::atomic<bool> jumpConfigWrite{false};
// M8g: hide the XInput A button from the game while WS is active (input-route test). WALLSPRINT_JUMP_HOLDBACK_TEST, default 0.
inline std::atomic<bool> jumpHoldbackTest{false};
}
