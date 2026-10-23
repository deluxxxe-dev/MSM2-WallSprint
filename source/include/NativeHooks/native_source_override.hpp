#pragma once
#include "wallrun_observer.hpp"
#include <atomic>
#include "msm2/actor_writer.hpp"
#include "NativeHooks/source_lifecycle.hpp"
namespace msm2::native_source {
struct Config {bool enabled=true,sourceOverride=true,debug=true,dualSource=true;float delay=.70f,rotateTime=.25f;};
void configure(Config,bool verified,std::uintptr_t gameBase,std::size_t gameSize,const std::array<msm2::writer::Patch,4>&);
void tick(const ws::observed::VerifiedWallContext&,const char* exitReason,bool authenticated);
void capture(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t ret,std::uintptr_t originalRsp) noexcept;
void captureCopy(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t ret,std::uintptr_t originalRsp) noexcept;
void prearm(const ws::native_source::Token&) noexcept;
void lifecycleClosed(const ws::native_source::Token&) noexcept;
void kill() noexcept;
void drainLogs();
void stop();
bool active() noexcept;
bool observationGuardsAuthenticated() noexcept;
bool entryOverridden() noexcept;
}
extern "C" {extern std::atomic<unsigned> ws_native_source_enabled;}
