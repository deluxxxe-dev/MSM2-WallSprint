#pragma once
#include "planar_mvp.hpp"
#include <array>
#include <atomic>
namespace msm2::writer {
struct Patch {std::uintptr_t address{};std::array<unsigned char,32> bytes{};};
void configure(ws::planar::Config,bool exactBuild);
void admit(bool completeVerified,const std::array<Patch,4>&);
void kill(const char* reason) noexcept;
void drainLogs();
bool active();
}
extern "C" {
extern std::atomic<unsigned> ws_write_dispatch_enabled;
// Separate read-only Update-thunk gate. It never enables ws_write_dispatch.
extern std::atomic<unsigned> ws_update_probe_enabled;
void ws_update_probe_sample(std::uintptr_t owner) noexcept;
void ws_write_dispatch(unsigned kind,std::uintptr_t owner) noexcept;
}
