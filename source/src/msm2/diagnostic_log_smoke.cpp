#include "msm2/orientation_log.hpp"
#include <cstdarg>
#include <cstdio>
#include <string>
#include <cassert>
static std::string output;
namespace msm2::hooks {void logState(std::uint64_t){}}
namespace msm2 {
void writeLog(const char* format,...){char buffer[4096]{};va_list args;va_start(args,format);vsnprintf(buffer,sizeof(buffer),format,args);va_end(args);output+=buffer;output+='\n';}
}
int main(){
 msm2::Snapshot sample{};sample.hero=0x100;sample.transform=0x200;sample.position[0]=3;sample.prefix[1]=-1;sample.prefix[4]=1;sample.prefix[10]=1;
 ws::diag::Motion motion{};motion.valid=true;motion.velocity={1,2,3};motion.epoch=1;
 for(unsigned i=0;i<6;++i)msm2::dumpDiagnostic(&sample,motion,100,1+i,ws::diag::markers[i]);
 msm2::dumpDiagnostic(nullptr,{},200,7,"MARK_AIR");
 for(auto marker:ws::diag::markers)assert(output.find(marker)!=std::string::npos);
 for(auto field:{"row-major","column-major","dot01=","det=1.000000000","orthogonality_error_Frobenius=0","candidate PRIMARY:","Right=R0 Up=R1 Forward=R2","opposite=","ground_truth_source=USER_MARKER","NOT_physics_velocity","NO stale sample substituted","END_DIAGNOSTIC sample_id=7","world_up=(0,1,0)"})assert(output.find(field)!=std::string::npos);
 std::puts("PASS: production Windows diagnostic formatter, six marker records, row/column metrics, signs, unavailable snapshot");
}
