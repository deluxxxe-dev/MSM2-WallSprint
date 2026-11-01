// Direct Actor writes remain disabled. The separate b5 module may change only
// a verified caller-local source; it does not use this legacy Actor adapter.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include "msm2/actor_writer.hpp"
#include "msm2/update_bridge.hpp"
#include "msm2/sdk_bridge.hpp"

extern "C" {
std::atomic<unsigned> ws_write_dispatch_enabled{0};
std::atomic<unsigned> ws_update_probe_enabled{0};
}

namespace {
constexpr std::uintptr_t kWallRun250Offset=0x250;
constexpr std::uintptr_t kConsumerThreshold0Rva=0x5303DE8;
constexpr std::uintptr_t kConsumerThreshold1Rva=0x5337098;
constexpr std::uint32_t kV8Threshold0Bits=0x3F000000u; // 0.5f
constexpr std::uint32_t kV8Threshold1Bits=0x3F35C28Fu; // 0.7099999785f

std::atomic<unsigned> thresholdProbeEnabled{0},finalSnapshotPending{0};
std::atomic<std::uint32_t> threshold0Bits{kV8Threshold0Bits},threshold1Bits{kV8Threshold1Bits};
std::atomic<std::uint64_t> samples{0},readOk{0},readFail{0},fieldNonNanSamples{0},fieldNanSamples{0};
std::atomic<std::uint64_t> consumer0True{0},consumer1Fallthrough{0};
std::atomic<std::uint32_t> firstBits{0},lastBits{0};
std::atomic<unsigned> firstCaptured{0};
std::uint64_t lastLoggedSamples{},lastThresholdCheck{};

float asFloat(std::uint32_t bits){float value{};std::memcpy(&value,&bits,sizeof(value));return value;}

void resetProbe(){
 ws_update_probe_enabled.store(0,std::memory_order_release);
 thresholdProbeEnabled.store(0,std::memory_order_release);finalSnapshotPending.store(0,std::memory_order_release);
 threshold0Bits.store(kV8Threshold0Bits,std::memory_order_relaxed);
 threshold1Bits.store(kV8Threshold1Bits,std::memory_order_relaxed);
 samples.store(0,std::memory_order_relaxed);readOk.store(0,std::memory_order_relaxed);readFail.store(0,std::memory_order_relaxed);
 fieldNonNanSamples.store(0,std::memory_order_relaxed);fieldNanSamples.store(0,std::memory_order_relaxed);
 consumer0True.store(0,std::memory_order_relaxed);consumer1Fallthrough.store(0,std::memory_order_relaxed);
 firstBits.store(0,std::memory_order_relaxed);lastBits.store(0,std::memory_order_relaxed);firstCaptured.store(0,std::memory_order_relaxed);
 lastLoggedSamples=lastThresholdCheck=0;
}

bool readLiveThresholds(std::uint32_t& a,std::uint32_t& b){
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
 if(!base||base>UINTPTR_MAX-kConsumerThreshold1Rva)return false;
 return msm2::readMemory(base+kConsumerThreshold0Rva,&a,sizeof(a))&&msm2::readMemory(base+kConsumerThreshold1Rva,&b,sizeof(b));
}

void logProbeSnapshot(const char* phase){
 auto n=samples.load(std::memory_order_acquire);
 auto ok=readOk.load(std::memory_order_acquire),bad=readFail.load(std::memory_order_acquire);
 auto nonNan=fieldNonNanSamples.load(std::memory_order_acquire),nan=fieldNanSamples.load(std::memory_order_acquire);
 auto aTrue=consumer0True.load(std::memory_order_acquire),bFallthrough=consumer1Fallthrough.load(std::memory_order_acquire);
 auto t0=threshold0Bits.load(std::memory_order_acquire),t1=threshold1Bits.load(std::memory_order_acquire);
 std::uint32_t live0=0,live1=0;bool liveOk=readLiveThresholds(live0,live1);
 bool liveUnchanged=liveOk&&live0==t0&&live1==t1;
 msm2::writeLog("wallrun_0x250_threshold_probe phase=%s enabled=%u update_probe_gate=%u write_dispatch_gate=%u exact_update_pre_original_samples=%llu read_ok=%llu read_fail=%llu field_non_nan=%llu field_nan=%llu consumer_gt_threshold0_or_unordered=%llu consumer_gt_threshold1_or_unordered=%llu threshold0_bits=0x%08x threshold0=%.9g threshold1_bits=0x%08x threshold1=%.9g live_check_ok=%u live_threshold0_bits=0x%08x live_threshold1_bits=0x%08x live_thresholds_unchanged_from_admission=%u v8_expected_threshold_bits_match=%u first_bits=0x%08x last_bits=0x%08x source=preserved_Update_thunk_pre_original_callback field=WallRun.this+0x250 pointer_dereference=0 game_call=0 game_write=0",
  phase,unsigned(thresholdProbeEnabled.load(std::memory_order_acquire)),unsigned(ws_update_probe_enabled.load(std::memory_order_acquire)),unsigned(ws_write_dispatch_enabled.load(std::memory_order_acquire)),static_cast<unsigned long long>(n),static_cast<unsigned long long>(ok),static_cast<unsigned long long>(bad),static_cast<unsigned long long>(nonNan),static_cast<unsigned long long>(nan),static_cast<unsigned long long>(aTrue),static_cast<unsigned long long>(bFallthrough),
  t0,asFloat(t0),t1,asFloat(t1),unsigned(liveOk),live0,live1,unsigned(liveUnchanged),unsigned(t0==kV8Threshold0Bits&&t1==kV8Threshold1Bits),firstBits.load(std::memory_order_acquire),lastBits.load(std::memory_order_acquire));
}
}

namespace msm2::writer {
void configure(ws::planar::Config c,bool exactBuild){
 ws_write_dispatch_enabled.store(0,std::memory_order_release);resetProbe();
 writeLog("writer_config requested_orientation=%u exact_build=%u Actor_writes=0 reason=direct_Actor_adapter_hard_disabled old_configuration_cannot_enable_writes=1",unsigned(c.orientation),unsigned(exactBuild));
}

void admit(bool completeVerified,const std::array<Patch,4>&){
 ws_write_dispatch_enabled.store(0,std::memory_order_release);
 ws_update_probe_enabled.store(0,std::memory_order_release);
 thresholdProbeEnabled.store(0,std::memory_order_release);
 std::uint32_t a=0,b=0;bool thresholdsReadable=completeVerified&&readLiveThresholds(a,b);
 if(thresholdsReadable){
  threshold0Bits.store(a,std::memory_order_relaxed);threshold1Bits.store(b,std::memory_order_relaxed);
  thresholdProbeEnabled.store(1,std::memory_order_release);
  ws_update_probe_enabled.store(1,std::memory_order_release);
 }
 writeLog("direct_actor_admission=DISABLED reason=legacy_direct_Actor_adapter_unavailable orientation_pulse=0");
 writeLog("wallrun_0x250_threshold_probe_admission complete_verified=%u admitted=%u update_probe_gate=%u write_dispatch_gate=%u live_thresholds_readable=%u threshold0_bits=0x%08x threshold0=%.9g threshold1_bits=0x%08x threshold1=%.9g threshold0_nan=%u threshold1_nan=%u v8_expected_threshold_bits_match=%u exact_update_pre_original=1 direct_update_thunk_callback=1 bridge_unwind_or_argument_clone=0 field_read_width=4 pointer_dereference=0 game_call=0 game_write=0",
  unsigned(completeVerified),unsigned(thresholdsReadable),unsigned(ws_update_probe_enabled.load(std::memory_order_acquire)),unsigned(ws_write_dispatch_enabled.load(std::memory_order_acquire)),unsigned(thresholdsReadable),a,asFloat(a),b,asFloat(b),unsigned(std::isnan(asFloat(a))),unsigned(std::isnan(asFloat(b))),unsigned(thresholdsReadable&&a==kV8Threshold0Bits&&b==kV8Threshold1Bits));
}

void kill(const char*) noexcept{
 ws_write_dispatch_enabled.store(0,std::memory_order_release);
 ws_update_probe_enabled.store(0,std::memory_order_release);
 if(thresholdProbeEnabled.exchange(0,std::memory_order_acq_rel))finalSnapshotPending.store(1,std::memory_order_release);
}

void drainLogs(){
 auto n=samples.load(std::memory_order_acquire);bool final=finalSnapshotPending.exchange(0,std::memory_order_acq_rel)!=0;
 if(!n)return;
 if(!final&&n==lastLoggedSamples)return;
 auto now=GetTickCount64();if(!final&&lastLoggedSamples&&now-lastThresholdCheck<500)return;
 lastThresholdCheck=now;lastLoggedSamples=n;logProbeSnapshot(final?"final":"cumulative");
}

bool active(){return false;}
}

extern "C" void ws_update_probe_sample(std::uintptr_t owner) noexcept{
 if(!thresholdProbeEnabled.load(std::memory_order_acquire)||!ws_update_probe_enabled.load(std::memory_order_acquire))return;
 samples.fetch_add(1,std::memory_order_relaxed);
 if(!owner||owner>UINTPTR_MAX-kWallRun250Offset){readFail.fetch_add(1,std::memory_order_relaxed);return;}
 std::uint32_t bits=0;if(!msm2::readMemory(owner+kWallRun250Offset,&bits,sizeof(bits))){readFail.fetch_add(1,std::memory_order_relaxed);return;}
 readOk.fetch_add(1,std::memory_order_relaxed);lastBits.store(bits,std::memory_order_relaxed);
 unsigned expected=0;if(firstCaptured.compare_exchange_strong(expected,1,std::memory_order_acq_rel))firstBits.store(bits,std::memory_order_release);
 float value=asFloat(bits);bool fieldNan=std::isnan(value);
 if(fieldNan)fieldNanSamples.fetch_add(1,std::memory_order_relaxed);else fieldNonNanSamples.fetch_add(1,std::memory_order_relaxed);
 float t0=asFloat(threshold0Bits.load(std::memory_order_relaxed)),t1=asFloat(threshold1Bits.load(std::memory_order_relaxed));
 bool unordered0=fieldNan||std::isnan(t0),unordered1=fieldNan||std::isnan(t1);
 if(unordered0||value>t0)consumer0True.fetch_add(1,std::memory_order_relaxed);
 if(unordered1||value>t1)consumer1Fallthrough.fetch_add(1,std::memory_order_relaxed);
}

extern "C" void ws_write_dispatch(unsigned,std::uintptr_t) noexcept{}
extern "C" std::uint64_t ws_write_update_pre(std::uintptr_t) noexcept{return 0;}
extern "C" void ws_write_update_post(std::uintptr_t,std::uint64_t) noexcept{}
extern "C" void ws_write_bridge_reject(const char*) noexcept{ws_write_dispatch_enabled.store(0,std::memory_order_release);}
