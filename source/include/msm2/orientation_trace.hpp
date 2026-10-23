#pragma once
#include "msm2/sdk_bridge.hpp"
#include "wallrun_observer.hpp"
#include "NativeHooks/path_b_layout.hpp"
#include "NativeHooks/immediate_reset_trace.hpp"
namespace msm2::trace {
struct Config {bool enabled=true;unsigned windowMs=350,maxWindows=8,maxEvents=256,maxThreads=32;};
void configure(Config,bool exactAndVerified,std::uintptr_t gameBase,std::size_t gameSize);
void tick(const ws::observed::VerifiedWallContext&,unsigned nativeThread,const char* exitReason);
void kill() noexcept;
void stop();
void drainLogs();
// External mode: one dedicated helper owns all arming/cleanup; worker only drains.
bool configureReset(bool verified,std::uintptr_t base,std::size_t size,bool prearmed=false,bool multithread=false);
void cancelReset() noexcept;
bool resetCommitted(const reset_trace::Request&) noexcept;
bool beginReset(const reset_trace::Request&,const Snapshot&);
bool pollReset(bool valid);
void beginResetCopyEntry() noexcept;
void resetCopyEntry(const path_b::Entry&) noexcept;
void resetEntry(std::uintptr_t rcx,std::uintptr_t rdx,std::uintptr_t r8,std::uintptr_t r9,std::uintptr_t caller,bool overridden,std::uint64_t nativeUpdate=0,std::uint64_t generation=0,std::uintptr_t owner=0) noexcept;
}
