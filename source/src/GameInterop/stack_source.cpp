#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>
#include "GameInterop/stack_source.hpp"
#include "msm2/sdk_bridge.hpp"
extern "C" bool ws_guarded_call(void (*)(void*),void*);
namespace {
bool writable(std::uintptr_t source){
 auto end=source+64;for(auto p=source;p<end;){MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(p),&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
  auto protection=m.Protect&0xff;if(protection!=PAGE_READWRITE)return false;auto b=reinterpret_cast<std::uintptr_t>(m.BaseAddress);if(m.RegionSize>UINTPTR_MAX-b||b+m.RegionSize<=p)return false;p=std::min(end,b+m.RegionSize);
 }return true;
}
struct Store {std::uintptr_t pointer;const float* values;std::uint64_t qpc{};unsigned bytes{48};};
void store(void* raw){auto* s=static_cast<Store*>(raw);std::memcpy(reinterpret_cast<void*>(s->pointer),s->values,s->bytes);LARGE_INTEGER q{};QueryPerformanceCounter(&q);s->qpc=q.QuadPart;}
}
namespace ws::interop {
StackRange currentStack() noexcept{StackRange r;__asm__ volatile("movq %%gs:0x10,%0":"=r"(r.limit));__asm__ volatile("movq %%gs:0x08,%0":"=r"(r.base));return r;}
bool stackLocation(std::uintptr_t p,std::uintptr_t rsp,StackRange range) noexcept {
 return p&&p%16==0&&p<=UINTPTR_MAX-64&&rsp<=UINTPTR_MAX-0x28&&p==rsp+0x28&&range.limit<range.base&&rsp>=range.limit&&p>=range.limit&&p+64<=range.base&&writable(p);
}
bool stackMatrix(std::uintptr_t p,std::uintptr_t rsp,StackRange range,source_math::Matrix& out) noexcept {
 return stackLocation(p,rsp,range)&&msm2::readMemory(p,&out,64)&&source_math::valid(out);
}
bool callerStackMatrix(std::uintptr_t p,std::uintptr_t rsp,StackRange range,source_math::Matrix& out) noexcept {
 // Authenticated exact direct caller + above entry return/shadow space, within
 // a narrow caller-local stack band. No guessed post-prologue RSP+0x60 rule.
 if(!p||p%16||p>UINTPTR_MAX-64||rsp>UINTPTR_MAX-0x400||p<rsp+0x28||p+64>rsp+0x400||range.limit>=range.base||rsp<range.limit||p<range.limit||p+64>range.base||!writable(p))return false;
 return msm2::readMemory(p,&out,64)&&source_math::valid(out);
}
bool stackCompact(std::uintptr_t p,std::uintptr_t rsp,StackRange range,source_math::CompactBlock& out) noexcept {
 return stackLocation(p,rsp,range)&&msm2::readMemory(p,&out,64);
}
WriteResult writeStackCompact(std::uintptr_t p,const source_math::CompactBlock& expected,const source_math::CompactBlock& replacement) noexcept {
 WriteResult r{};auto range=currentStack();source_math::CompactBlock before{},after{};ws::diag::ActorAxes axes{};ws::Vec3 lengths{};
 if(!p||p%16||p>UINTPTR_MAX-64||p<range.limit||p+64>range.base||expected.metadata!=replacement.metadata||!source_math::compactAxes(replacement,axes,lengths)||!writable(p))return r;
 if(!msm2::readMemory(p,&before,64)||std::memcmp(&before,&expected,64))return r;
 Store s{p,replacement.rows,0,36};bool ok=ws_guarded_call(store,&s);
 if(ok&&msm2::readMemory(p,&after,64)&&!std::memcmp(&after,&replacement,64)){r.success=r.positionUnchanged=true;r.commitQpc=s.qpc;return r;}
 s.values=expected.rows;bool back=ws_guarded_call(store,&s);r.rolledBack=back&&msm2::readMemory(p,&after,64)&&!std::memcmp(&after,&expected,64);r.rollbackFailed=!r.rolledBack;return r;
}
WriteResult writeStackOrientation(std::uintptr_t p,const source_math::Matrix& expected,const source_math::Matrix& replacement) noexcept {
 WriteResult r{};auto range=currentStack();if(!p||p%16||p>UINTPTR_MAX-64||p<range.limit||p+64>range.base)return r;
 if(std::memcmp(expected.v+12,replacement.v+12,16)||!source_math::valid(replacement))return r;
 source_math::Matrix before{};if(!msm2::readMemory(p,&before,64)||std::memcmp(&before,&expected,64)||!writable(p))return r;
 Store s{p,replacement.v,0,48};bool ok=ws_guarded_call(store,&s);source_math::Matrix after{};
 if(ok&&msm2::readMemory(p,&after,64)&&!std::memcmp(&after,&replacement,64)){r.success=r.positionUnchanged=true;r.commitQpc=s.qpc;return r;}
 s.values=expected.v;bool back=ws_guarded_call(store,&s);r.rolledBack=back&&msm2::readMemory(p,&after,64)&&!std::memcmp(&after,&expected,64);r.rollbackFailed=!r.rolledBack;return r;
}
}
