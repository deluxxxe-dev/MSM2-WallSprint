#pragma once
#include <cstdint>
#include "msm2/sdk_bridge.hpp"
#include "diagnostics.hpp"
namespace msm2::camera_ref {
// Exact-build hook on HeroCameraManager::UpdateCameraRefMatrix entry (RVA 0x1065680) that only records `this`.
void configure(bool verified, std::uintptr_t image, std::size_t imageSize);
// Game thread, called from the verified WallRun Update pre-original callback: enable actor-relative camera reference.
void onWallSprintUpdate() noexcept;
// Worker thread (100 ms): restore the flag after WallSprint ends; read-only camera-forward scan on the ground.
void tick(const msm2::Snapshot& snapshot, const ws::diag::Motion& motion, bool available, float stickX, float stickY, bool stick) noexcept;
void kill() noexcept;
void stop() noexcept;
}
