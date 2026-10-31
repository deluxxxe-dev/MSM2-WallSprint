#pragma once
#include <cstddef>
#include <cstdint>

namespace msm2::request_presence {

struct Config {
    bool enabled{};
};

bool start(bool targetBuildConfirmed, Config config, std::uintptr_t moduleBase, std::size_t moduleSize);
void tick();
void armNextGround();
void armNextWallRun();
void closeCapture(const char* reason);
void stop(const char* reason);
bool available();

} // namespace msm2::request_presence
