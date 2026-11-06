#pragma once
#include <atomic>
#include <cstdint>
// M7 prep (owner 2026-11-06): one place for every player-tunable WallSprint value, so the later ModSettings screen and
// presets map onto these fields. Today they are read once at start from WallSprint.ini [WallSprintSettings]
// (missing key = default). Config-baked values (camera distance/height/pivot, steering) are listed in
// docs/SETTINGS.md and still need Update-WS; a live setting for them needs a native route (FEATURE_ROADMAP).
namespace ws::settings {
inline std::atomic<float> turnRateDegPerSec{400.f};   // WS travel/facing turn rate (ground sprint measured max ~470)
inline std::atomic<unsigned> mouseManualMs{150};      // camera counts as player-controlled this long after a mouse move
inline std::atomic<float> stickManualThreshold{0.2f}; // right stick magnitude that counts as player camera input
// Reserved for upcoming features (not used yet): leave-wall gravity reset time, wall-jump strength, etc.
inline std::atomic<float> gravityResetSeconds{0.f};
}
