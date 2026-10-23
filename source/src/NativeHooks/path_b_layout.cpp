#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <array>
#include <atomic>
#include <algorithm>
#include <cstring>
#include "NativeHooks/path_b_layout.hpp"
#include "msm2/orientation_trace.hpp"
namespace {
constexpr unsigned capacity=256,perSession=128;
std::array<msm2::path_b::Entry,capacity> records;std::atomic<unsigned> head{0},tail{0},drops{0},skipped{0};std::atomic<bool> disabled{true};std::atomic_flag gate=ATOMIC_FLAG_INIT;
struct Budget {std::uint64_t session{},generation{};unsigned count{};};std::array<Budget,8> budgets;std::uint64_t serial{};
}
namespace msm2::path_b {
bool readBounded(std::uintptr_t p,void* out,unsigned n) noexcept {
 if(p<0x10000||!n||n>128||p>0x00007fffffffffffULL-n)return false;
 auto end=p+n;for(auto pos=p;pos<end;){MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(pos),&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
  auto prot=m.Protect&0xff;if(prot!=PAGE_READONLY&&prot!=PAGE_READWRITE&&prot!=PAGE_WRITECOPY&&prot!=PAGE_EXECUTE_READ&&prot!=PAGE_EXECUTE_READWRITE&&prot!=PAGE_EXECUTE_WRITECOPY)return false;
  auto b=reinterpret_cast<std::uintptr_t>(m.BaseAddress);if(m.RegionSize>UINTPTR_MAX-b||b+m.RegionSize<=pos)return false;pos=std::min(end,b+m.RegionSize);
 }
 return readMemory(p,out,n);
}
void configure(bool enabled){disabled=!enabled;LARGE_INTEGER f{};QueryPerformanceFrequency(&f);writeLog("PATH_B_LAYOUT_CONFIG enabled=%u path_b_override=0 path_b_characterization_active=%u bytes=128 per_session=128 max_sessions=8 window_elapsed=[0.60,1.50] qpc_frequency=%llu pointer_dereferences=0",unsigned(enabled),unsigned(enabled),(unsigned long long)f.QuadPart);}
void capture(Entry e) noexcept {
 if(disabled.load()||e.elapsed<.60f||e.elapsed>1.50f)return;
 if(gate.test_and_set(std::memory_order_acquire)){++drops;return;}
 Budget* budget=nullptr;for(auto& b:budgets)if(b.session==e.session&&b.generation==e.generation){budget=&b;break;}
 if(!budget)for(auto& b:budgets)if(!b.session){b.session=e.session;b.generation=e.generation;budget=&b;break;}
 auto h=head.load(std::memory_order_relaxed);
 if(!budget||budget->count>=perSession){++skipped;gate.clear(std::memory_order_release);return;}
 if(h-tail.load(std::memory_order_acquire)>=capacity){++drops;gate.clear(std::memory_order_release);return;}
 e.inside=e.stackLimit<e.stackBase&&e.rdx>=e.stackLimit&&e.rdx<e.stackBase;
 e.spanInside=e.inside&&e.rdx<=UINTPTR_MAX-128&&e.rdx+128<=e.stackBase;
 e.relation=e.rsp<=UINTPTR_MAX-0x28&&e.rdx==e.rsp+0x28;
 e.readable=readBounded(e.rdx,e.raw,128);LARGE_INTEGER q{};QueryPerformanceCounter(&q);e.qpc=q.QuadPart;e.id=++serial;
 ++budget->count;records[h%capacity]=e;head.store(h+1,std::memory_order_release);gate.clear(std::memory_order_release);
 // Entry is this function's OWN raw arguments, not the common-writer cache.
 trace::resetCopyEntry(e);
}
void drainLogs(){
 for(unsigned n=0;n<capacity;++n){auto t=tail.load(std::memory_order_relaxed);if(t==head.load(std::memory_order_acquire))break;auto e=records[t%capacity];tail.store(t+1,std::memory_order_release);
  char raw[257]{};constexpr char digits[]="0123456789abcdef";if(e.readable)for(unsigned i=0;i<128;++i){raw[2*i]=digits[e.raw[i]>>4];raw[2*i+1]=digits[e.raw[i]&15];}
  writeLog("RESET_COPY_LAYOUT_ENTRY session=%llu entry_id=%llu generation=%llu native_update=%llu elapsed=%.6f qpc=%llu thread_id=%u caller_rva=0x%llx RCX=0x%llx RDX=0x%llx R8=0x%llx R9=0x%llx RSP=0x%llx stack_limit=0x%llx stack_base=0x%llx rdx_minus_rsp=%lld rdx_inside_stack_bounds=%u source_span_inside_stack_bounds=%u expected_rsp_relation_match=%u source_readable=%u raw_source_0x80=%s N398=(%.9g,%.9g,%.9g) hero_up_before=(%.9g,%.9g,%.9g) path_a_interpolated_up=(%.9g,%.9g,%.9g) path_a_reference_valid=%u path_b_override=0 path_b_characterization_active=1 layout_semantics=UNKNOWN",(unsigned long long)e.session,(unsigned long long)e.id,(unsigned long long)e.generation,(unsigned long long)e.update,e.elapsed,(unsigned long long)e.qpc,e.thread,(unsigned long long)e.caller,(unsigned long long)e.rcx,(unsigned long long)e.rdx,(unsigned long long)e.r8,(unsigned long long)e.r9,(unsigned long long)e.rsp,(unsigned long long)e.stackLimit,(unsigned long long)e.stackBase,(long long)(std::int64_t(e.rdx)-std::int64_t(e.rsp)),unsigned(e.inside),unsigned(e.spanInside),unsigned(e.relation),unsigned(e.readable),e.readable?raw:"unavailable",e.normal.x,e.normal.y,e.normal.z,e.actorBefore.x,e.actorBefore.y,e.actorBefore.z,e.pathAUp.x,e.pathAUp.y,e.pathAUp.z,unsigned(e.referenceValid));
 }
 auto lost=drops.exchange(0),bounded=skipped.exchange(0);if(lost||bounded)writeLog("PATH_B_LAYOUT_BUDGET dropped=%u bounded_skipped=%u path_b_override=0 incomplete_capture=1",lost,bounded);
}
void kill() noexcept{disabled=true;}
}
