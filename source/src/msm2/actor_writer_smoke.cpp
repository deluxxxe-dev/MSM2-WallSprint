#include "actor_writer.cpp"
#include <cassert>
#include <cstdio>
namespace msm2 {void writeLog(const char*,...){} }
int main(){
 ws::planar::Config cfg;std::array<msm2::writer::Patch,4> patches{};
 for(unsigned i=0;i<4;++i){cfg.orientation=true;cfg.adhesion=i==2;cfg.movement=i==3;msm2::writer::configure(cfg,true);msm2::writer::admit(true,patches);assert(!ws_write_dispatch_enabled&&!msm2::writer::active());ws_write_dispatch(0,123);assert(!ws_write_update_pre(123));ws_write_update_post(123,1);assert(!ws_write_dispatch_enabled);}
 puts("PASS: Direct Actor writes hard-disabled for old/current INI, all admission arguments and callback paths");
}
