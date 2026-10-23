// TEST ONLY. Independently generated code and owned memory; no game bytes.
#include "reset_copy_source.cpp"
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <immintrin.h>
#include <cfenv>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
static unsigned reads{},callbacks{};
static std::string proofLog;
namespace msm2 {
bool readMemory(std::uintptr_t p,void* out,std::size_t n){++reads;SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&got)&&got==n;}
void writeLog(const char* fmt,...){char buffer[2048]{};va_list args;va_start(args,fmt);vsnprintf(buffer,sizeof(buffer),fmt,args);va_end(args);proofLog+=buffer;proofLog+='\n';}
}
namespace msm2::trace {void positionCopyCommit(std::uintptr_t) noexcept {}}
namespace msm2::native_source {
bool entryOverridden() noexcept {return false;}
void captureCopy(std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uintptr_t) noexcept {++callbacks;__asm__ volatile("vzeroall" ::: "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","xmm8","xmm9","xmm10","xmm11","xmm12","xmm13","xmm14","xmm15");std::fesetround(FE_DOWNWARD);}
}
extern "C" {
std::atomic<unsigned> ws_native_source_enabled{1};
alignas(32) float ws_test_vector_input[128],ws_test_vectors[128];
std::uint64_t ws_test_gprs[11],ws_test_result;float ws_test_returns[128];std::uint64_t ws_test_return_gprs[11],ws_test_nonvolatile[8];
void ws_test_probe_target();void ws_test_invoke_probe(void*);
}
int main(int argc,char** argv){
 msm2::reset_copy::configure(false,1,1);assert(!reads&&!msm2::reset_copy::authenticated());
 char hash[65]{};assert(digest(reinterpret_cast<const unsigned char*>("abc"),3,hash));assert(!std::strcmp(hash,"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
 // Independently generated direct-copy sequence exercises the production Capstone
 // proof. This validates the decoder, never the accepted game's private bytes.
 std::array<unsigned char,128> map{};map.fill(0x90);unsigned cursor=0;
 constexpr unsigned destinations[]={0,4,8,16,20,24,32,36,40};
 for(unsigned i=0;i<9;++i){unsigned source=i*4;
  map[cursor++]=0x8b;map[cursor++]=source?0x42:0x02;if(source)map[cursor++]=static_cast<unsigned char>(source);
  map[cursor++]=0x89;map[cursor++]=destinations[i]?0x41:0x01;if(destinations[i])map[cursor++]=static_cast<unsigned char>(destinations[i]);
 }
 auto proof=ws::compact_proof::writer(map.data(),map.size());assert(proof.admitted&&proof.pairs==9);
 auto broken=map;broken[5]=5;assert(!ws::compact_proof::writer(broken.data(),broken.size()).admitted);
 std::array<unsigned char,128> caller{};caller.fill(0x90);constexpr std::uintptr_t callerBegin=0x1000,callerReturn=0x107b,writerTarget=0x2000;
 unsigned cp=0;for(auto b:{0x48,0x83,0xec,0x58,0xc5,0xfa,0x59,0xc0})caller[cp++]=static_cast<unsigned char>(b);
 for(unsigned i=0;i<9;++i)for(auto b:{0xc5,0xfa,0x11,0x44,0x24,int(0x20+i*4)})caller[cp++]=static_cast<unsigned char>(b);
 for(auto b:{0x48,0x8d,0x54,0x24,0x20})caller[cp++]=static_cast<unsigned char>(b);
 auto callOffset=unsigned(callerReturn-callerBegin-5);caller[callOffset]=0xe8;auto displacement=std::int32_t(writerTarget-callerReturn);std::memcpy(caller.data()+callOffset+1,&displacement,4);
 assert(ws::compact_proof::caller(caller.data(),caller.size(),callerBegin,writerTarget,callerReturn));caller[callOffset]=0x90;assert(!ws::compact_proof::caller(caller.data(),caller.size(),callerBegin,writerTarget,callerReturn));
 // Correct unwind shape with an independent body still fails the exact hash.
 constexpr unsigned bytes=0x2cfd000;auto* memory=static_cast<unsigned char*>(VirtualAlloc(nullptr,bytes,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(memory);
 base=reinterpret_cast<std::uintptr_t>(memory);target=base+writerRva;std::memset(memory+writerRva,0x90,writerSize);memory[0x800]=1;
 RUNTIME_FUNCTION rf{};rf.BeginAddress=writerRva;rf.EndAddress=writerRva+writerSize;rf.UnwindData=0x800;assert(RtlAddFunctionTable(&rf,1,base));
 assert(!authenticateOriginal(bytes));assert(!installed&&!admitted);assert(RtlDeleteFunctionTable(&rf));assert(VirtualFree(memory,0,MEM_RELEASE));
 if(argc==3){
  std::ifstream wf(argv[1],std::ios::binary),cf(argv[2],std::ios::binary);std::vector<unsigned char>w(std::istreambuf_iterator<char>(wf),{}),c(std::istreambuf_iterator<char>(cf),{});assert(w.size()==444&&c.size()==128);
  assert(digest(w.data(),unsigned(w.size()),hash)&&!std::strcmp(hash,expectedHash));assert(digest(c.data(),unsigned(c.size()),hash)&&!std::strcmp(hash,expectedCallerHash));
  auto mutated=w;mutated[0]^=1;assert(digest(mutated.data(),unsigned(mutated.size()),hash)&&std::strcmp(hash,expectedHash));
  auto* actual=static_cast<unsigned char*>(VirtualAlloc(nullptr,bytes,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE));assert(actual);auto actualBase=reinterpret_cast<std::uintptr_t>(actual);
  std::memcpy(actual+writerRva,w.data(),w.size());std::memcpy(actual+0x2cfc7d0,c.data(),c.size());actual[0x800]=1;
  RUNTIME_FUNCTION functions[2]{};functions[0]={unsigned(writerRva),unsigned(writerRva+writerSize),0x800};functions[1]={0x2cfc7d0,0x2cfc850,0x800};assert(RtlAddFunctionTable(functions,2,actualBase));
  base=actualBase;target=base+writerRva;actual[writerRva]^=1;assert(!authenticateOriginal(bytes));actual[writerRva]^=1;
  actual[0x2cfc7d0+7]^=1;assert(!authenticateOriginal(bytes));actual[0x2cfc7d0+7]^=1;
  attempted=false;msm2::reset_copy::configure(true,actualBase,bytes);assert(msm2::reset_copy::authenticated()&&msm2::reset_copy::compactProofAuthenticated());
  auto status=msm2::reset_copy::proofStatus();assert(status.writer&&status.caller&&status.map);
  assert(proofLog.find("PATH_B_COMPACT3X3_PROOF admitted=1 caller_verified=1 writer_verified=1 rdx_to_rcx_map_verified=1")!=std::string::npos&&proofLog.find("decoded_prefix_bytes=444 caller_decoded_bytes=128 pairs=9")!=std::string::npos&&proofLog.find("PATH_B_CODE")==std::string::npos);
  msm2::reset_copy::stop();assert(!std::memcmp(actual+writerRva,w.data(),w.size()));assert(MH_Uninitialize()==MH_OK);assert(RtlDeleteFunctionTable(functions));assert(VirtualFree(actual,0,MEM_RELEASE));
  mutated=w;mutated.back()=0x0f;auto failure=ws::compact_proof::writer(mutated.data(),mutated.size());logFailure("writer",writerRva,mutated.data(),unsigned(mutated.size()),failure);
  assert(proofLog.find("PATH_B_PROOF_DECODE_FAILURE role=writer function_rva=0x2cfc3c0 byte_offset=0x1bb absolute_rva=0x2cfc57b remaining_bytes=1 first_bytes_hex=0f decoder_backend=Capstone_5.0.6_x86_64_static")!=std::string::npos);
  std::puts("PASS: actual hash-authenticated writer444/caller128 production startup proof and MinHook installation/owned restoration in a nonexecuted registered image; writer/caller byte mutation refused by production authentication; bounded first-failure diagnostics; no full game capture logged");
 }
 assert(__builtin_cpu_supports("avx"));for(unsigned i=0;i<128;++i)ws_test_vector_input[i]=i+.5f;
 ws_test_invoke_probe(reinterpret_cast<void*>(&ws_test_probe_target));std::array<std::uint64_t,11> gp{},ret{};std::array<float,128> vec{},returned{};std::array<std::uint64_t,8> ng{};
 std::memcpy(gp.data(),ws_test_gprs,sizeof(ws_test_gprs));std::memcpy(ret.data(),ws_test_return_gprs,sizeof(ws_test_return_gprs));std::memcpy(vec.data(),ws_test_vectors,sizeof(ws_test_vectors));std::memcpy(returned.data(),ws_test_returns,sizeof(ws_test_returns));std::memcpy(ng.data(),ws_test_nonvolatile,sizeof(ws_test_nonvolatile));auto rounding=std::fegetround();
 assert(MH_Initialize()==MH_OK);assert(MH_CreateHook(reinterpret_cast<void*>(&ws_test_probe_target),reinterpret_cast<void*>(&ws_reset_copy_thunk),&ws_reset_copy_original)==MH_OK);assert(MH_EnableHook(reinterpret_cast<void*>(&ws_test_probe_target))==MH_OK);
 ws_test_invoke_probe(reinterpret_cast<void*>(&ws_test_probe_target));assert(callbacks==1&&std::fegetround()==rounding);
 for(unsigned i=0;i<11;++i){auto mask=i==7?0x8c5ull:~0ull;assert((ws_test_gprs[i]&mask)==(gp[i]&mask));}
 assert(ws_test_result==0x77aabbcc11223344ull&&!std::memcmp(vec.data(),ws_test_vectors,sizeof(ws_test_vectors))&&!std::memcmp(ret.data(),ws_test_return_gprs,sizeof(ws_test_return_gprs))&&!std::memcmp(returned.data(),ws_test_returns,sizeof(ws_test_returns))&&!std::memcmp(ng.data(),ws_test_nonvolatile,sizeof(ws_test_nonvolatile)));
 assert(MH_DisableHook(reinterpret_cast<void*>(&ws_test_probe_target))==MH_OK);assert(MH_Uninitialize()==MH_OK);
 puts("PASS: reset-copy exact-hash/wrong-build refusal, SHA256 known vector; actual second raw thunk/MinHook callback clobber restores YMM0..15/GPR/flags/stack/nonvolatiles/returns/x87-MXCSR; synthetic only");
}
