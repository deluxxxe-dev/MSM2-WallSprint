#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <MinHook.h>
#include <array>
#include <algorithm>
#include <atomic>
#include <cstring>
#include "NativeHooks/reset_copy_source.hpp"
#include "NativeHooks/compact_map_proof.hpp"
#include "NativeHooks/native_source_override.hpp"
#include "msm2/sdk_bridge.hpp"
#include "msm2/orientation_trace.hpp"
extern "C" {
void ws_reset_copy_thunk();void* ws_reset_copy_original{};std::atomic<unsigned> ws_reset_copy_recording{0};
}
namespace {
constexpr std::uintptr_t writerRva=0x2cfc3c0,returnRva=0x2cfc84b;
constexpr unsigned writerSize=444;
constexpr char expectedHash[]="ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196";
constexpr char expectedCallerHash[]="1665a7a329a983101673f05ccd95bbf3d738c39099469b4acfd14344ec98a79d";
std::uintptr_t base{},target{},callerBegin{};unsigned callerSize{};
std::array<unsigned char,writerSize> original{},owned{};std::array<unsigned char,0x4000> callerOriginal{};
std::atomic<bool> installed{false},admitted{false};bool attempted{};bool compactProof{};
msm2::reset_copy::ProofStatus proofStatus;
bool digest(const unsigned char* data,unsigned count,char (&text)[65]){
 BCRYPT_ALG_HANDLE alg=nullptr;BCRYPT_HASH_HANDLE hash=nullptr;unsigned char object[512]{},result[32]{};DWORD size=0,done=0;
 bool ok=BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)>=0;
 if(ok)ok=BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&size),sizeof(size),&done,0)>=0&&size<=sizeof(object);
 if(ok)ok=BCryptCreateHash(alg,&hash,object,size,nullptr,0,0)>=0;
 if(ok)ok=BCryptHashData(hash,const_cast<PUCHAR>(data),count,0)>=0;
 if(ok)ok=BCryptFinishHash(hash,result,32,0)>=0;
 if(hash)BCryptDestroyHash(hash);
 if(alg)BCryptCloseAlgorithmProvider(alg,0);
 constexpr char hex[]="0123456789abcdef";for(unsigned i=0;i<32;++i){text[2*i]=hex[result[i]>>4];text[2*i+1]=hex[result[i]&15];}text[64]=0;return ok;
}
bool own(){std::array<unsigned char,writerSize> bytes{};return installed.load()&&msm2::readMemory(target,bytes.data(),bytes.size())&&bytes==owned;}
bool authenticateOriginal(std::size_t imageSize){
 if(!base||imageSize<=returnRva||imageSize<=writerRva+writerSize)return false;
 DWORD64 image=0;auto* f=RtlLookupFunctionEntry(target,&image,nullptr);
 if(!f||image!=base||f->BeginAddress!=writerRva||f->EndAddress!=writerRva+writerSize||!msm2::readMemory(target,original.data(),original.size()))return false;
 char hash[65]{};if(!digest(original.data(),original.size(),hash)||std::strcmp(hash,expectedHash))return false;
 proofStatus.writer=true;
 // Caller bounds are obtained from accepted-build unwind metadata. Authenticate
 // an actual direct CALL to this writer, never infer an entry-RSP offset from a
 // post-prologue hardware snapshot. Unknown call encodings fail closed.
 f=RtlLookupFunctionEntry(base+returnRva-1,&image,nullptr);
 if(!f||image!=base||f->BeginAddress!=0x2cfc7d0||f->EndAddress!=0x2cfc850||f->EndAddress>imageSize)return false;
 callerBegin=base+f->BeginAddress;callerSize=f->EndAddress-f->BeginAddress;
 unsigned char call[5]{};std::int32_t displacement{};
 if(!msm2::readMemory(base+returnRva-5,call,5)||call[0]!=0xe8)return false;
 std::memcpy(&displacement,call+1,4);
 if(std::int64_t(base+returnRva)+displacement!=std::int64_t(target))return false;
 return msm2::readMemory(callerBegin,callerOriginal.data(),callerSize)&&digest(callerOriginal.data(),callerSize,hash)&&!std::strcmp(hash,expectedCallerHash);
}
void logFailure(const char* role,std::uintptr_t functionRva,const unsigned char* bytes,unsigned size,const ws::compact_proof::Result& result){
 if(result.admitted)return;
 auto offset=std::min(size,result.failureOffset),count=std::min(16u,size-offset);char hex[33]{};constexpr char digits[]="0123456789abcdef";
 for(unsigned i=0;i<count;++i){hex[i*2]=digits[bytes[offset+i]>>4];hex[i*2+1]=digits[bytes[offset+i]&15];}
 msm2::writeLog("PATH_B_PROOF_DECODE_FAILURE role=%s function_rva=0x%llx byte_offset=0x%x absolute_rva=0x%llx remaining_bytes=%u first_bytes_hex=%s decoder_backend=%s decoder_status=%u reason=%s",role,(unsigned long long)functionRva,offset,(unsigned long long)(functionRva+offset),size-offset,count?hex:"none",ws::compact_proof::backend,result.decoderStatus,result.reason);
}
struct Frame {std::uintptr_t r11,r10,r9,r8,rdx,rcx,rax,flags,ret;};
static_assert(offsetof(Frame,rcx)==40&&offsetof(Frame,ret)==64);
}
extern "C" void ws_reset_copy_capture(const Frame* f) noexcept {
 msm2::native_source::captureCopy(f->rcx,f->rdx,f->r8,f->r9,f->ret,reinterpret_cast<std::uintptr_t>(&f->ret));
 if(msm2::native_source::entryOverridden())msm2::trace::positionCopyCommit(f->rcx);
}
namespace msm2::reset_copy {
ProofStatus proofStatus() noexcept {return ::proofStatus;}
bool authenticated() noexcept {
 if(!admitted.load()||!own())return false;
 std::array<unsigned char,0x4000> caller{};
 return callerSize&&readMemory(callerBegin,caller.data(),callerSize)&&!std::memcmp(caller.data(),callerOriginal.data(),callerSize);
}
bool compactProofAuthenticated() noexcept {return compactProof&&authenticated();}
void configure(bool verified,std::uintptr_t image,std::size_t size){
 if(attempted)return;
 attempted=true;base=image;target=base+writerRva;
 if(!verified||!authenticateOriginal(size)){writeLog("PATH_B_COMPACT3X3_PROOF admitted=0 caller_verified=0 writer_verified=%u rdx_to_rcx_map_verified=0 source_bytes=0x24 reason=prerequisite_hash_range_or_caller_failed",unsigned(::proofStatus.writer));writeLog("RESET_COPY_HOOK admitted=0 installed=0 reason=prerequisite_boundary_original_hash_or_direct_caller_failed");return;}
 auto proof=ws::compact_proof::writer(original.data(),original.size());auto callerProof=ws::compact_proof::callerProof(callerOriginal.data(),callerSize,callerBegin-base,writerRva,returnRva);compactProof=proof.admitted&&callerProof.admitted;
 ::proofStatus.caller=callerProof.admitted;::proofStatus.map=proof.admitted;
 writeLog("PATH_B_COMPACT3X3_PROOF admitted=%u caller_verified=%u writer_verified=1 rdx_to_rcx_map_verified=%u source_bytes=0x24 decoded_prefix_bytes=%u caller_decoded_bytes=%u pairs=%u decoder_backend=%s reason=%s",unsigned(compactProof),unsigned(callerProof.admitted),unsigned(proof.admitted),proof.decodedBytes,callerProof.decodedBytes,proof.pairs,ws::compact_proof::backend,proof.reason);
 logFailure("writer",writerRva,original.data(),writerSize,proof);logFailure("caller",callerBegin-base,callerOriginal.data(),callerSize,callerProof);
 if(!compactProof)return;
 for(const auto& pair:proof.map)writeLog("PATH_B_COMPACT3X3_MAP source_offset=0x%x destination_offset=0x%x store_rva=0x%llx bytes=4",pair.source,pair.destination,(unsigned long long)(writerRva+pair.instruction));
 HMODULE pin=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&configure),&pin))return;
 auto init=MH_Initialize();if(init!=MH_OK&&init!=MH_ERROR_ALREADY_INITIALIZED)return;
 if(MH_CreateHook(reinterpret_cast<void*>(target),reinterpret_cast<void*>(&ws_reset_copy_thunk),&ws_reset_copy_original)!=MH_OK)return;
 std::array<unsigned char,writerSize> before{};
 if(!readMemory(target,before.data(),before.size())||before!=original)return;
 installed=MH_EnableHook(reinterpret_cast<void*>(target))==MH_OK;
 if(installed&&(!readMemory(target,owned.data(),owned.size())||std::memcmp(owned.data()+32,original.data()+32,writerSize-32))){MH_DisableHook(reinterpret_cast<void*>(target));installed=false;}
 admitted=installed.load();
 writeLog("RESET_COPY_HOOK admitted=%u installed=%u writer_rva=0x2cfc3c0 end_rva=0x2cfc57c size=444 original_sha256=%s caller_return_rva=0x2cfc84b caller_begin_rva=0x%llx caller_size=%u direct_call_verified=1 ABI=raw_tailjump source_layout_requires_entry_validation=1",unsigned(admitted.load()),unsigned(installed.load()),expectedHash,(unsigned long long)(callerBegin-base),callerSize);
}
void stop(){admitted=false;if(installed&&own()){auto status=MH_DisableHook(reinterpret_cast<void*>(target));if(status==MH_OK)installed=false;writeLog("RESET_COPY_STOP restored=%u trampoline_retained=1",unsigned(!installed.load()));}else if(installed)writeLog("RESET_COPY_STOP restored=0 foreign_patch_untouched=1");}
}
