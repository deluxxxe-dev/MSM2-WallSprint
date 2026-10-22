#pragma once
#include "msm2/sdk_bridge.hpp"
#include "wallrun_observer.hpp"
namespace msm2::input {
struct Config {bool enabled=true,otherArguments=false,observe=true;unsigned windowMs=2000,maxWindows=8,maxEvents=256;};
void configure(Config,bool exactAndVerified,std::uintptr_t gameBase,std::size_t gameSize);
void tick(const ws::observed::VerifiedWallContext&,const char* exitReason,bool authenticated);
bool authenticated();
void kill() noexcept;
void stop();
void drainLogs();
}
