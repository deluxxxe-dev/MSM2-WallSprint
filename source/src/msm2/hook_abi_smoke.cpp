#include <windows.h>
#include <MinHook.h>
#include <immintrin.h>
#include <thread>
#include <atomic>
#include <vector>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdarg>
#include "msm2/hook_capture.hpp"
#include "msm2/update_bridge.hpp"
#include "NativeHooks/source_lifecycle.hpp"
extern "C" {
alignas(32) float ws_test_vector_input[128],ws_test_vectors[128];
std::uint64_t ws_test_gprs[11],ws_test_result;
float ws_test_returns[128];std::uint64_t ws_test_return_gprs[11],ws_test_nonvolatile[8];
void ws_test_probe_target();void ws_test_invoke_probe(void*);
}
extern "C" {std::atomic<unsigned> ws_write_dispatch_enabled{0};}
static std::atomic<unsigned> dispatchCalls{0},nativeSourceGateCalls{0};
static void sourceGateProbe(unsigned,std::uintptr_t) noexcept {++nativeSourceGateCalls;_mm_setcsr(0x3f80);__asm__ volatile("fninit; vzeroall":::"xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","xmm8","xmm9","xmm10","xmm11","xmm12","xmm13","xmm14","xmm15");}
extern "C" void ws_write_dispatch(unsigned,std::uintptr_t) noexcept {
 ++dispatchCalls;
 _mm_setcsr(0x3f80); // changed rounding mode must not escape the optional bridge
 __asm__ volatile("fninit; vzeroall":::"xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","xmm8","xmm9","xmm10","xmm11","xmm12","xmm13","xmm14","xmm15");
}
extern "C" std::uint64_t ws_write_update_pre(std::uintptr_t owner) noexcept {ws_write_dispatch(2,owner);return 1;}
extern "C" void ws_write_update_post(std::uintptr_t owner,std::uint64_t token) noexcept {assert(token==1);ws_write_dispatch(2,owner);}
extern "C" void ws_write_bridge_reject(const char* reason) noexcept {std::fprintf(stderr,"BRIDGE REJECT: %s\n",reason);std::abort();}
extern "C" void ws_update_after_native();
#define LEAF __attribute__((noinline,noclone))
LEAF std::uint64_t integerTarget(void* self,std::uint64_t b,std::uint64_t c,std::uint64_t d,std::uint64_t e,std::uint64_t f){return (reinterpret_cast<std::uintptr_t>(self)^b)+3*c+5*d+7*e+11*f;}
LEAF double floatTarget(double a,double b,double c,double d,double e,double f){return a+2*b+3*c+4*d+5*e+6*f;}
LEAF __m128 vectorTarget(__m128 a,__m128 b){return _mm_add_ps(_mm_mul_ps(a,_mm_set1_ps(2)),b);}
LEAF std::uint64_t manyStackTarget(std::uint64_t a0,std::uint64_t a1,std::uint64_t a2,std::uint64_t a3,std::uint64_t a4,std::uint64_t a5,std::uint64_t a6,std::uint64_t a7,std::uint64_t a8,std::uint64_t a9,std::uint64_t a10,std::uint64_t a11,std::uint64_t a12,std::uint64_t a13,std::uint64_t a14,std::uint64_t a15,std::uint64_t a16,std::uint64_t a17,std::uint64_t a18,std::uint64_t a19,std::uint64_t a20,std::uint64_t a21,std::uint64_t a22,std::uint64_t a23){return 1*a0+2*a1+3*a2+4*a3+5*a4+6*a5+7*a6+8*a7+9*a8+10*a9+11*a10+12*a11+13*a12+14*a13+15*a14+16*a15+17*a16+18*a17+19*a18+20*a19+21*a20+22*a21+23*a22+24*a23;}
LEAF double variadicTarget(unsigned n,...){va_list args;va_start(args,n);double sum=0;for(unsigned i=0;i<n;++i)sum+=(i+1)*va_arg(args,double);va_end(args);return sum;}
struct LargeReturn {std::uint64_t a,b,c;};
LEAF LargeReturn sretTarget(std::uint64_t a,std::uint64_t b,std::uint64_t c,std::uint64_t d,std::uint64_t e){return {a+2*b,c+3*d,e*7};}
LEAF void extraPostAbi(){
 auto exercise=[](void* target,auto call){
  assert(MH_CreateHook(target,reinterpret_cast<void*>(ws_hook_update),&ws_hook_originals[2])==MH_OK);assert(MH_EnableHook(target)==MH_OK);
  ws_write_dispatch_enabled=1;ws_hook_recording=1;call();ws_hook_recording=0;ws_write_dispatch_enabled=0;
  assert(MH_DisableHook(target)==MH_OK);assert(MH_RemoveHook(target)==MH_OK);ws::capture::Event event;assert(ws::capture::pop(event)&&event.kind==2);assert(!ws::capture::pop(event));
 };
 volatile auto many=&manyStackTarget;auto expected=many(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24);
 exercise(reinterpret_cast<void*>(&manyStackTarget),[&]{assert(many(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24)==expected);});
 volatile auto variadic=&variadicTarget;auto expectFloat=variadic(8,1.25,2.5,3.75,4.,5.25,6.5,7.75,8.);
 exercise(reinterpret_cast<void*>(&variadicTarget),[&]{assert(variadic(8,1.25,2.5,3.75,4.,5.25,6.5,7.75,8.)==expectFloat);});
 volatile auto sret=&sretTarget;auto expectLarge=sret(11,22,33,44,55);
 exercise(reinterpret_cast<void*>(&sretTarget),[&]{auto value=sret(11,22,33,44,55);assert(value.a==expectLarge.a&&value.b==expectLarge.b&&value.c==expectLarge.c);});
}
int main(){
 ws::capture::initializeQueue();
 volatile auto integer=&integerTarget;volatile auto floating=&floatTarget;volatile auto vector=&vectorTarget;
 const auto expectedInt=integer(reinterpret_cast<void*>(123),11,22,33,44,55);const auto expectedFloat=floating(1.25,2.5,3.75,4,5.25,6.5);
 auto x=_mm_setr_ps(1,2,3,4),y=_mm_setr_ps(5,6,7,8);float expectedVector[4];_mm_storeu_ps(expectedVector,vector(x,y));
 bool avx=__builtin_cpu_supports("avx");std::array<std::uint64_t,11> gp{};std::array<float,128> vectors{},returns{};std::array<std::uint64_t,11> rg{};std::array<std::uint64_t,8> ng{};
 if(avx){for(int i=0;i<128;++i)ws_test_vector_input[i]=float(i)+.5f;ws_test_invoke_probe(reinterpret_cast<void*>(&ws_test_probe_target));std::memcpy(gp.data(),ws_test_gprs,sizeof(ws_test_gprs));std::memcpy(vectors.data(),ws_test_vectors,sizeof(ws_test_vectors));std::memcpy(returns.data(),ws_test_returns,sizeof(ws_test_returns));std::memcpy(rg.data(),ws_test_return_gprs,sizeof(ws_test_return_gprs));std::memcpy(ng.data(),ws_test_nonvolatile,sizeof(ws_test_nonvolatile));assert(ws_test_result==0x77aabbcc11223344ull);}
 ws_source_lifecycle_callback=sourceGateProbe;ws_source_lifecycle_enabled=avx?1:0;
 assert(MH_Initialize()==MH_OK);
 void* targets[]={reinterpret_cast<void*>(&integerTarget),reinterpret_cast<void*>(&floatTarget),reinterpret_cast<void*>(&vectorTarget),reinterpret_cast<void*>(&ws_test_probe_target)};
 void (*thunks[])()={ws_hook_init,ws_hook_entry,ws_hook_update,ws_hook_deinit};
 for(int i=0;i<4;++i){assert(MH_CreateHook(targets[i],reinterpret_cast<void*>(thunks[i]),&ws_hook_originals[i])==MH_OK);assert(MH_EnableHook(targets[i])==MH_OK);}
 ws_hook_recording.store(1);
 if(avx)ws_write_dispatch_enabled=1;
 unsigned oldMxcsr=_mm_getcsr();unsigned short originalControl=0,control=0;__asm__ volatile("fnstcw %0":"=m"(originalControl));control=(originalControl&~0x300u)|0x200u;__asm__ volatile("fldcw %0; fld1"::"m"(control));
 assert(integer(reinterpret_cast<void*>(123),11,22,33,44,55)==expectedInt);assert(floating(1.25,2.5,3.75,4,5.25,6.5)==expectedFloat);
 double x87=0;__asm__ volatile("fstpl %0":"=m"(x87));assert(x87==1.);unsigned short afterControl=0;__asm__ volatile("fnstcw %0":"=m"(afterControl));assert(afterControl==control&&_mm_getcsr()==oldMxcsr);__asm__ volatile("fldcw %0"::"m"(originalControl));
 float result[4];_mm_storeu_ps(result,vector(x,y));assert(std::memcmp(result,expectedVector,sizeof(result))==0);
 if(avx){ws_test_invoke_probe(reinterpret_cast<void*>(&ws_test_probe_target));for(unsigned i=0;i<11;++i){auto mask=i==7?0x8c5ull:~0ull;assert((ws_test_gprs[i]&mask)==(gp[i]&mask));}assert(std::memcmp(ws_test_vectors,vectors.data(),sizeof(ws_test_vectors))==0);assert(ws_test_result==0x77aabbcc11223344ull);}
 ws::capture::Event event;unsigned seen[4]{};while(ws::capture::pop(event)){assert(event.kind<4&&event.caller&&event.tsc&&event.thread==GetCurrentThreadId());++seen[event.kind];}
 assert(seen[0]==1&&seen[1]==1&&seen[2]==1&&seen[3]==unsigned(avx));
 const unsigned iterations=5000,threadCount=4;std::atomic<unsigned> completed{0};std::vector<std::thread> threads;
 auto droppedBefore=ws::capture::dropped();std::uint64_t captured=0;
 for(unsigned t=1;t<=threadCount;++t)threads.emplace_back([&,t]{for(unsigned i=0;i<iterations;++i){auto value=integer(reinterpret_cast<void*>(std::uintptr_t(t)),11,22,33,44,55);assert(value==((t^11u)+3*22+5*33+7*44+11*55));}++completed;});
 while(completed.load()<threadCount){while(ws::capture::pop(event)){assert(event.kind==0&&event.self>=1&&event.self<=threadCount&&event.tsc&&event.caller&&event.thread);++captured;}SwitchToThread();}
 for(auto& t:threads)t.join();
 while(ws::capture::pop(event))++captured;
 assert(captured+ws::capture::dropped()-droppedBefore==iterations*threadCount);
 ws_hook_recording.store(0);ws_write_dispatch_enabled=0;if(avx)assert(dispatchCalls>=iterations*threadCount+4);assert(MH_DisableHook(MH_ALL_HOOKS)==MH_OK);assert(integer(reinterpret_cast<void*>(123),11,22,33,44,55)==expectedInt);assert(!ws::capture::pop(event));
 for(void* target:targets)assert(MH_RemoveHook(target)==MH_OK);
 // Exercise the post-original wrapper on EVERY ABI fixture, not only vector return.
 if(avx){
  for(unsigned target=0;target<4;++target){
   assert(MH_CreateHook(targets[target],reinterpret_cast<void*>(ws_hook_update),&ws_hook_originals[2])==MH_OK);assert(MH_EnableHook(targets[target])==MH_OK);
   ws_write_dispatch_enabled=1;ws_hook_recording=1;
   __asm__ volatile("fldcw %0; fld1"::"m"(control));
   if(target==0)assert(integer(reinterpret_cast<void*>(123),11,22,33,44,55)==expectedInt);
   if(target==1)assert(floating(1.25,2.5,3.75,4,5.25,6.5)==expectedFloat);
   if(target==2){_mm_storeu_ps(result,vector(x,y));assert(!std::memcmp(result,expectedVector,sizeof(result)));}
   if(target==3){
    ws_test_invoke_probe(targets[target]);assert(ws_test_result==0x77aabbcc11223344ull);
    for(unsigned i=0;i<11;++i){if(i==8){assert(ws_test_gprs[i]==reinterpret_cast<std::uintptr_t>(&ws_update_after_native));continue;}auto mask=i==7?0x8c5ull:~0ull;assert((ws_test_gprs[i]&mask)==(gp[i]&mask));}
    assert(!std::memcmp(ws_test_vectors,vectors.data(),sizeof(ws_test_vectors)));assert(!std::memcmp(ws_test_returns,returns.data(),sizeof(ws_test_returns)));
    assert(!std::memcmp(ws_test_return_gprs,rg.data(),sizeof(ws_test_return_gprs)));assert(!std::memcmp(ws_test_nonvolatile,ng.data(),sizeof(ws_test_nonvolatile)));
   }
   double postX87=0;__asm__ volatile("fstpl %0":"=m"(postX87));assert(postX87==1.);unsigned short postCW=0;__asm__ volatile("fnstcw %0":"=m"(postCW));assert(postCW==control&&_mm_getcsr()==oldMxcsr);__asm__ volatile("fldcw %0"::"m"(originalControl));
   ws_write_dispatch_enabled=0;ws_hook_recording=0;
   assert(MH_DisableHook(targets[target])==MH_OK);assert(MH_RemoveHook(targets[target])==MH_OK);
   assert(ws::capture::pop(event)&&event.kind==2&&event.caller);assert(!ws::capture::pop(event));
  }
 }
 if(avx)extraPostAbi();
 if(avx)assert(nativeSourceGateCalls>=iterations*threadCount+4);
 ws_source_lifecycle_enabled=0;
 assert(MH_Uninitialize()==MH_OK);
 std::printf("PASS: 4 real MinHook detours plus post-original wrapper on integer/FP/vector/probe returns; integer/FP/vector/24-arg/variadic/sret forwarding preserved; external caller, stack args, nonvolatiles, output RAX/RDX/flags and full vector state preserved; %s; concurrent capture accounting=%u calls\n",avx?"optional local callback enabled; x87/MXCSR and YMM0-15 preserved":"AVX probe SKIPPED",iterations*threadCount);
}
