#include "actor_writer.cpp"
#include <cassert>
#include <cstdio>
// Test-only link stubs: actor_writer.cpp is included directly without sdk_bridge.cpp.
// readMemory mirrors the production guarded ReadProcessMemory contract; this smoke never admits the c48 probe.
namespace msm2 {void writeLog(const char*,...){}
bool readMemory(std::uintptr_t p,void* out,std::size_t n){if(!p||!n||p>UINTPTR_MAX-n)return false;SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&got)&&got==n;}
}
int main(){
 ws::planar::Config cfg;std::array<msm2::writer::Patch,4> patches{};
 for(unsigned i=0;i<4;++i){cfg.orientation=true;cfg.adhesion=i==2;cfg.movement=i==3;msm2::writer::configure(cfg,true);msm2::writer::admit(true,patches);assert(!ws_write_dispatch_enabled&&!msm2::writer::active());ws_write_dispatch(0,123);assert(!ws_write_update_pre(123));ws_write_update_post(123,1);assert(!ws_write_dispatch_enabled);}
 puts("PASS: Direct Actor writes hard-disabled for old/current INI, all admission arguments and callback paths");
}
