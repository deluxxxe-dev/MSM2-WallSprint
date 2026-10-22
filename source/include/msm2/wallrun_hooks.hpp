#pragma once
#include <array>
#include "msm2/actor_writer.hpp"
#include "wallrun_observer.hpp"
#include "msm2/sdk_bridge.hpp"
#include "diagnostics.hpp"
namespace msm2::hooks {
struct Options {bool enabled=true;bool objectDiagnostic=true;bool surfaceTelemetry=true;std::array<bool,4> individual{true,true,true,true};};
void start(bool targetBuild,const Options& options);
void drain(const Snapshot* snapshot,const ws::diag::Motion& motion,std::uint64_t snapshotMs);
ws::observed::VerifiedWallContext context(std::uint64_t now);
bool observationPlanAuthenticated();
std::array<msm2::writer::Patch,4> patchSnapshot();
unsigned nativeThreadId();
const char* exitReason();
void logState(std::uint64_t now);
void stop() noexcept;
}
