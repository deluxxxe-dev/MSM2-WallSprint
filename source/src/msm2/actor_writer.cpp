// Direct Actor writes remain disabled. The separate b5 module may change only
// a verified caller-local source; it does not use this legacy Actor adapter.
#include "msm2/actor_writer.hpp"
#include "msm2/update_bridge.hpp"
#include "msm2/sdk_bridge.hpp"
extern "C" {std::atomic<unsigned> ws_write_dispatch_enabled{0};}
namespace msm2::writer {
void configure(ws::planar::Config c,bool exactBuild){ws_write_dispatch_enabled=0;writeLog("writer_config requested_orientation=%u exact_build=%u Actor_writes=0 reason=direct_Actor_adapter_hard_disabled old_configuration_cannot_enable_writes=1",unsigned(c.orientation),unsigned(exactBuild));}
void admit(bool,const std::array<Patch,4>&){ws_write_dispatch_enabled=0;writeLog("direct_actor_admission=DISABLED reason=legacy_direct_Actor_adapter_unavailable orientation_pulse=0");}
void kill(const char*) noexcept{ws_write_dispatch_enabled=0;}
void drainLogs(){}
bool active(){return false;}
}
extern "C" void ws_write_dispatch(unsigned,std::uintptr_t) noexcept{}
extern "C" std::uint64_t ws_write_update_pre(std::uintptr_t) noexcept{return 0;}
extern "C" void ws_write_update_post(std::uintptr_t,std::uint64_t) noexcept{}
extern "C" void ws_write_bridge_reject(const char*) noexcept{ws_write_dispatch_enabled=0;}
