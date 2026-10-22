#pragma once
#include "msm2/sdk_bridge.hpp"
#include "diagnostics.hpp"
namespace msm2 {
void dumpDiagnostic(const Snapshot* sample,const ws::diag::Motion& motion,std::uint64_t tick,std::uint64_t id,const char* trigger);
}
