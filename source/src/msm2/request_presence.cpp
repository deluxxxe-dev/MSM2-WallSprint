#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <MinHook.h>
#include <hde64.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include "msm2/request_presence.hpp"
#include "msm2/sdk_bridge.hpp"

namespace {

constexpr std::uintptr_t kRequestRva = 0x11A1BD0;
constexpr std::uintptr_t kRequestEndRva = 0x11A1CA2;
constexpr std::uintptr_t kRequestUnwindRva = 0x94697E0;
constexpr std::size_t kRequestSize = kRequestEndRva - kRequestRva;
constexpr std::size_t kNativeSnapshot = 32;
constexpr LONG kBatchRecordCap = 16;
constexpr ULONGLONG kDrainIntervalMs = 500;
constexpr ULONGLONG kQuiesceWaitMs = 250;
constexpr std::uint64_t kSessionRecordCap = 65536;
constexpr char kRequestSha256[] = "2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c";
constexpr unsigned char kRequestPrefix[] = {0x4c,0x8b,0xdc,0x49,0x89,0x5b,0x08};

struct PresenceRecord {
    std::uint64_t rcx;
    std::uint64_t rdx;
    std::uint64_t r8;
    std::uint64_t r9;
    std::uint64_t returnPc;
    std::uint64_t threadToken;
    std::uint64_t tsc;
    std::uint64_t slot;
};
static_assert(sizeof(PresenceRecord) == 64, "presence record layout must stay assembly-compatible");

extern "C" {
volatile LONG ws_request_presence_enabled = 0;
volatile LONG ws_request_presence_active_callbacks = 0;
volatile LONG ws_request_presence_count = 0;
volatile LONG ws_request_presence_overflow = 0;
volatile LONG ws_request_presence_contention = 0;
PresenceRecord ws_request_presence_records[kBatchRecordCap]{};
std::uintptr_t ws_request_presence_original = 0;
std::uint64_t ws_request_presence_read_tsc();
void ws_request_presence_observer();
void ws_request_presence_restore0();
void ws_request_presence_restore1();
void ws_request_presence_restore2();
void ws_request_presence_restore3();
void ws_request_presence_restore4();
void ws_request_presence_restore5();
void ws_request_presence_restore6();
void ws_request_presence_restore7();
void ws_request_presence_restore8();
}

enum class PhaseState { Idle, Capturing, Stopped };
struct Runtime {
    bool requested{};
    bool started{};
    bool admitted{};
    bool created{};
    bool enabled{};
    bool sessionLogged{};
    void* target{};
    void* trampoline{};
    std::array<unsigned char,kNativeSnapshot> native{};
    std::array<unsigned char,kNativeSnapshot> patch{};
    PhaseState phase{PhaseState::Idle};
    ULONGLONG sessionStartTick{};
    ULONGLONG batchStartTick{};
    ULONGLONG nextDrainTick{};
    std::uint64_t sessionStartTsc{};
    std::uint64_t batchStartTsc{};
    unsigned batchId{};
    std::uint64_t sessionCaptured{};
    std::uint64_t sessionOverflow{};
    std::uint64_t sessionContention{};
};
Runtime runtime;
volatile LONG captureCloseRequested=0;

LONG atomicRead(volatile LONG* value) {
    return InterlockedCompareExchange(value,0,0);
}

bool regionReadable(const void* address,std::size_t size) {
    if(!address || !size) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if(!VirtualQuery(address,&mbi,sizeof(mbi))) return false;
    if(mbi.State!=MEM_COMMIT || (mbi.Protect&PAGE_GUARD) || (mbi.Protect&PAGE_NOACCESS)) return false;
    auto begin=reinterpret_cast<std::uintptr_t>(address);
    auto regionBegin=reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
    auto regionEnd=regionBegin+mbi.RegionSize;
    return begin>=regionBegin && begin<regionEnd && size<=regionEnd-begin;
}

bool copyCode(const void* address,void* output,std::size_t size) {
    if(!regionReadable(address,size)) return false;
    std::memcpy(output,address,size);
    return true;
}

bool sha256(const void* data,std::size_t size,char (&text)[65]) {
    BCRYPT_ALG_HANDLE algorithm=nullptr;
    BCRYPT_HASH_HANDLE hash=nullptr;
    DWORD objectSize=0,returned=0;
    std::vector<UCHAR> object;
    bool ok=BCryptOpenAlgorithmProvider(&algorithm,BCRYPT_SHA256_ALGORITHM,nullptr,0)>=0;
    if(ok) ok=BCryptGetProperty(algorithm,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&objectSize),sizeof(objectSize),&returned,0)>=0;
    if(ok){object.resize(objectSize);ok=BCryptCreateHash(algorithm,&hash,object.data(),objectSize,nullptr,0,0)>=0;}
    if(ok) ok=BCryptHashData(hash,reinterpret_cast<PUCHAR>(const_cast<void*>(data)),static_cast<ULONG>(size),0)>=0;
    UCHAR digest[32]{};
    if(ok) ok=BCryptFinishHash(hash,digest,sizeof(digest),0)>=0;
    if(hash) BCryptDestroyHash(hash);
    if(algorithm) BCryptCloseAlgorithmProvider(algorithm,0);
    if(!ok) return false;
    for(unsigned i=0;i<32;++i) std::snprintf(text+i*2,3,"%02x",digest[i]);
    text[64]='\0';
    return true;
}

bool exactRequestUnwind(std::uintptr_t target,std::uintptr_t moduleBase) {
    DWORD64 imageBase=0;
    auto* entry=RtlLookupFunctionEntry(static_cast<DWORD64>(target),&imageBase,nullptr);
    bool ok=entry && imageBase==moduleBase && entry->BeginAddress==kRequestRva && entry->EndAddress==kRequestEndRva && entry->UnwindData==kRequestUnwindRva;
    msm2::writeLog("REQUEST_PRESENCE_SITE_PDATA ok=%u begin_rva=0x%lx end_rva=0x%lx unwind_rva=0x%lx expected_begin=0x%llx expected_end=0x%llx expected_unwind=0x%llx",unsigned(ok),entry?entry->BeginAddress:0ul,entry?entry->EndAddress:0ul,entry?entry->UnwindData:0ul,static_cast<unsigned long long>(kRequestRva),static_cast<unsigned long long>(kRequestEndRva),static_cast<unsigned long long>(kRequestUnwindRva));
    return ok;
}

bool exactObserverUnwind(void* address,std::uintptr_t expectedImageBase,const char* name) {
    DWORD64 imageBase=0;
    auto pc=reinterpret_cast<DWORD64>(address);
    auto* entry=RtlLookupFunctionEntry(pc,&imageBase,nullptr);
    bool ok=entry && imageBase==expectedImageBase && imageBase+entry->BeginAddress==pc && entry->EndAddress>entry->BeginAddress;
    msm2::writeLog("REQUEST_PRESENCE_SELF_UNWIND label=%s ok=%u begin=0x%llx end=0x%llx image=0x%llx",name,unsigned(ok),static_cast<unsigned long long>(entry?imageBase+entry->BeginAddress:0),static_cast<unsigned long long>(entry?imageBase+entry->EndAddress:0),static_cast<unsigned long long>(imageBase));
    return ok;
}

bool pinObserverAndValidateUnwind() {
    HMODULE observerModule=nullptr;
    auto observer=reinterpret_cast<LPCWSTR>(&ws_request_presence_observer);
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,observer,&observerModule) || !observerModule){
        msm2::writeLog("REQUEST_PRESENCE_SELF_UNWIND admitted=0 reason=module_pin_failed win32=%lu",GetLastError());
        return false;
    }
    auto imageBase=reinterpret_cast<std::uintptr_t>(observerModule);
    struct Label {const char* name;void* address;};
    const Label labels[]={
        {"observer",reinterpret_cast<void*>(&ws_request_presence_observer)},
        {"restore0",reinterpret_cast<void*>(&ws_request_presence_restore0)},
        {"restore1",reinterpret_cast<void*>(&ws_request_presence_restore1)},
        {"restore2",reinterpret_cast<void*>(&ws_request_presence_restore2)},
        {"restore3",reinterpret_cast<void*>(&ws_request_presence_restore3)},
        {"restore4",reinterpret_cast<void*>(&ws_request_presence_restore4)},
        {"restore5",reinterpret_cast<void*>(&ws_request_presence_restore5)},
        {"restore6",reinterpret_cast<void*>(&ws_request_presence_restore6)},
        {"restore7",reinterpret_cast<void*>(&ws_request_presence_restore7)},
        {"restore8",reinterpret_cast<void*>(&ws_request_presence_restore8)}
    };
    bool ok=true;
    for(const auto& label:labels) ok=exactObserverUnwind(label.address,imageBase,label.name)&&ok;
    return ok;
}

bool pinTarget(void* target) {
    HMODULE targetModule=nullptr;
    return GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(target),&targetModule)!=FALSE;
}

bool validateInstructionSpan(const unsigned char* target) {
    unsigned span=0;
    unsigned instructions=0;
    bool relative=false;
    while(span<5 && instructions<4){
        hde64s hs{};
        unsigned length=hde64_disasm(target+span,&hs);
        if(!length || (hs.flags&F_ERROR)) return false;
        relative=relative||((hs.flags&F_RELATIVE)!=0);
        span+=length;
        ++instructions;
    }
    bool ok=span==sizeof(kRequestPrefix) && instructions==2 && !relative;
    msm2::writeLog("REQUEST_PRESENCE_STOLEN_SPAN ok=%u bytes=%u instructions=%u relative=%u expected_bytes=%u expected_instructions=2",unsigned(ok),span,instructions,unsigned(relative),unsigned(sizeof(kRequestPrefix)));
    return ok;
}

bool absoluteJumpAt(const void* address,std::uintptr_t expected) {
    unsigned char bytes[14]{};
    if(!copyCode(address,bytes,sizeof(bytes))) return false;
    if(bytes[0]!=0xff || bytes[1]!=0x25 || bytes[2]||bytes[3]||bytes[4]||bytes[5]) return false;
    std::uint64_t destination=0;
    std::memcpy(&destination,bytes+6,sizeof(destination));
    return destination==expected;
}

bool validateTrampoline(void* trampoline,std::uintptr_t target) {
    unsigned char prefix[sizeof(kRequestPrefix)]{};
    if(!copyCode(trampoline,prefix,sizeof(prefix)) || std::memcmp(prefix,kRequestPrefix,sizeof(prefix))!=0) return false;
    return absoluteJumpAt(reinterpret_cast<const unsigned char*>(trampoline)+sizeof(kRequestPrefix),target+sizeof(kRequestPrefix));
}

bool patchRelay(void* target,std::uintptr_t& relay) {
    unsigned char bytes[5]{};
    if(!copyCode(target,bytes,sizeof(bytes)) || bytes[0]!=0xe9) return false;
    std::int32_t displacement=0;
    std::memcpy(&displacement,bytes+1,sizeof(displacement));
    auto base=reinterpret_cast<std::uintptr_t>(target)+5;
    relay=static_cast<std::uintptr_t>(static_cast<std::intptr_t>(base)+displacement);
    return absoluteJumpAt(reinterpret_cast<void*>(relay),reinterpret_cast<std::uintptr_t>(&ws_request_presence_observer));
}

bool nativeRestored() {
    std::array<unsigned char,kNativeSnapshot> current{};
    return copyCode(runtime.target,current.data(),current.size()) && current==runtime.native;
}

void resetBatch() {
    InterlockedExchange(&ws_request_presence_count,0);
    InterlockedExchange(&ws_request_presence_overflow,0);
    InterlockedExchange(&ws_request_presence_contention,0);
    std::memset(ws_request_presence_records,0,sizeof(ws_request_presence_records));
}

bool waitForCallbacks() {
    const auto start=GetTickCount64();
    while(atomicRead(&ws_request_presence_active_callbacks)!=0 && GetTickCount64()-start<kQuiesceWaitMs) Sleep(1);
    return atomicRead(&ws_request_presence_active_callbacks)==0;
}

bool drainBatch(const char* reason,bool rearm) {
    if(runtime.phase!=PhaseState::Capturing) return true;
    InterlockedExchange(&ws_request_presence_enabled,0);
    if(!waitForCallbacks()){
        msm2::writeLog("REQUEST_PRESENCE_DRAIN deferred=1 reason=%s active_callbacks=%ld wait_cap_ms=%llu records_preserved=1",reason?reason:"unspecified",atomicRead(&ws_request_presence_active_callbacks),static_cast<unsigned long long>(kQuiesceWaitMs));
        if(rearm && runtime.admitted && runtime.enabled && atomicRead(&captureCloseRequested)==0) InterlockedExchange(&ws_request_presence_enabled,1);
        runtime.nextDrainTick=GetTickCount64()+kDrainIntervalMs;
        return false;
    }

    const auto endTsc=ws_request_presence_read_tsc();
    const auto endTick=GetTickCount64();
    LONG count=std::clamp(atomicRead(&ws_request_presence_count),0L,kBatchRecordCap);
    LONG overflow=std::max(0L,atomicRead(&ws_request_presence_overflow));
    LONG contention=std::max(0L,atomicRead(&ws_request_presence_contention));
    const auto batchMs=endTick>=runtime.batchStartTick?endTick-runtime.batchStartTick:0;
    const auto sessionMs=endTick>=runtime.sessionStartTick?endTick-runtime.sessionStartTick:0;
    const auto sessionTscSpan=endTsc>runtime.sessionStartTsc?endTsc-runtime.sessionStartTsc:0;
    ++runtime.batchId;
    runtime.sessionCaptured+=static_cast<std::uint64_t>(count);
    runtime.sessionOverflow+=static_cast<std::uint64_t>(overflow);
    runtime.sessionContention+=static_cast<std::uint64_t>(contention);

    msm2::writeLog("REQUEST_PRESENCE_BATCH_SUMMARY batch_id=%u retained=%ld batch_cap=%ld overflow=%ld contention=%ld lost=%ld reason=%s actual_batch_ms=%llu periodic_worker_drain_ms=%llu session_retained=%llu session_record_cap=%llu",runtime.batchId,count,kBatchRecordCap,overflow,contention,overflow+contention,reason?reason:"unspecified",static_cast<unsigned long long>(batchMs),static_cast<unsigned long long>(kDrainIntervalMs),static_cast<unsigned long long>(runtime.sessionCaptured),static_cast<unsigned long long>(kSessionRecordCap));
    for(LONG i=0;i<count;++i){
        const auto& record=ws_request_presence_records[i];
        const bool offsetValid=sessionTscSpan>0 && record.tsc>=runtime.sessionStartTsc && record.tsc<=endTsc;
        const double offsetMs=offsetValid?(static_cast<double>(record.tsc-runtime.sessionStartTsc)*static_cast<double>(sessionMs)/static_cast<double>(sessionTscSpan)):0.0;
        msm2::writeLog("REQUEST_PRESENCE_RECORD batch_id=%u slot=%llu session_offset_ms_est=%.3f offset_valid=%u offset_clock=tsc_linearized_to_GetTickCount64 rcx_bits=0x%016llx rdx_bits=0x%016llx r8_bits=0x%016llx r9_bits=0x%016llx return_pc=0x%016llx thread_token=0x%016llx tsc=%llu semantics=opaque_bits_no_dereference",runtime.batchId,static_cast<unsigned long long>(record.slot),offsetMs,unsigned(offsetValid),static_cast<unsigned long long>(record.rcx),static_cast<unsigned long long>(record.rdx),static_cast<unsigned long long>(record.r8),static_cast<unsigned long long>(record.r9),static_cast<unsigned long long>(record.returnPc),static_cast<unsigned long long>(record.threadToken),static_cast<unsigned long long>(record.tsc));
    }

    resetBatch();
    if(runtime.sessionCaptured>=kSessionRecordCap){
        runtime.phase=PhaseState::Idle;
        msm2::writeLog("REQUEST_PRESENCE_SESSION_CAP_REACHED retained=%llu cap=%llu capture_gate_closed=1 hook_forwarding_preserved=1",static_cast<unsigned long long>(runtime.sessionCaptured),static_cast<unsigned long long>(kSessionRecordCap));
        return true;
    }
    if(rearm && runtime.admitted && runtime.enabled && atomicRead(&captureCloseRequested)==0){
        runtime.batchStartTick=endTick;
        runtime.batchStartTsc=endTsc;
        runtime.nextDrainTick=endTick+kDrainIntervalMs;
        InterlockedExchange(&ws_request_presence_enabled,1);
    }else{
        runtime.phase=PhaseState::Idle;
    }
    return true;
}

void logSession(const char* reason) {
    if(runtime.sessionLogged) return;
    runtime.sessionLogged=true;
    msm2::writeLog("REQUEST_PRESENCE_SESSION_SUMMARY batches=%u batch_record_cap=%ld periodic_worker_drain_ms=%llu session_record_cap=%llu retained=%llu overflow=%llu contention=%llu lost=%llu reason=%s",runtime.batchId,kBatchRecordCap,static_cast<unsigned long long>(kDrainIntervalMs),static_cast<unsigned long long>(kSessionRecordCap),static_cast<unsigned long long>(runtime.sessionCaptured),static_cast<unsigned long long>(runtime.sessionOverflow),static_cast<unsigned long long>(runtime.sessionContention),static_cast<unsigned long long>(runtime.sessionOverflow+runtime.sessionContention),reason?reason:"unspecified");
}

void startCapture() {
    resetBatch();
    InterlockedExchange(&captureCloseRequested,0);
    runtime.sessionStartTick=GetTickCount64();
    runtime.sessionStartTsc=ws_request_presence_read_tsc();
    runtime.batchStartTick=runtime.sessionStartTick;
    runtime.batchStartTsc=runtime.sessionStartTsc;
    runtime.nextDrainTick=runtime.sessionStartTick+kDrainIntervalMs;
    runtime.phase=PhaseState::Capturing;
    InterlockedExchange(&ws_request_presence_enabled,1);
    msm2::writeLog("REQUEST_PRESENCE_CAPTURE_START class=automatic label=auto_admission capture_lifetime=script_session periodic_worker_drain_ms=%llu batch_record_cap=%ld session_record_cap=%llu shutdown_flush=best_effort_not_required_for_prior_batches manual_F11_F12_required=0",static_cast<unsigned long long>(kDrainIntervalMs),kBatchRecordCap,static_cast<unsigned long long>(kSessionRecordCap));
}

} // namespace

namespace msm2::request_presence {

bool start(bool targetBuildConfirmed,Config config,std::uintptr_t moduleBase,std::size_t moduleSize) {
    msm2::writeLog("REQUEST_PRESENCE_CONFIG requested=%u mode=read_only entry_rva=0x%llx site_size=%llu capture_control=automatic_on_admission capture_lifetime=script_session periodic_worker_drain_ms=%llu batch_record_cap=%ld session_record_cap=%llu legacy_manual_controls=disabled",unsigned(config.enabled),static_cast<unsigned long long>(kRequestRva),static_cast<unsigned long long>(kRequestSize),static_cast<unsigned long long>(kDrainIntervalMs),kBatchRecordCap,static_cast<unsigned long long>(kSessionRecordCap));
    if(runtime.created){
        InterlockedExchange(&ws_request_presence_enabled,0);
        msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=resident_hook_already_created process_restart_required=1 previous_hook_enabled=%u trampoline_retained=1 original_forward_preserved=1",unsigned(runtime.enabled));
        return false;
    }
    runtime=Runtime{};
    runtime.requested=config.enabled;
    InterlockedExchange(&ws_request_presence_enabled,0);
    InterlockedExchange(&ws_request_presence_active_callbacks,0);
    InterlockedExchange(&captureCloseRequested,0);
    resetBatch();
    ws_request_presence_original=0;
    if(!config.enabled){
        msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=config_disabled");
        return false;
    }
    runtime.started=true;
    if(!targetBuildConfirmed){msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=target_build_gate_rejected");return false;}
    if(!moduleBase || moduleSize<kRequestEndRva || kRequestRva>=moduleSize){msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=module_span_invalid base=0x%llx size=0x%llx",static_cast<unsigned long long>(moduleBase),static_cast<unsigned long long>(moduleSize));return false;}
    auto target=moduleBase+kRequestRva;
    runtime.target=reinterpret_cast<void*>(target);
    if(!pinTarget(runtime.target)){msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=target_module_pin_failed win32=%lu",GetLastError());return false;}
    if(!copyCode(runtime.target,runtime.native.data(),runtime.native.size())){msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=native_snapshot_unreadable");return false;}
    char hash[65]{};
    if(!regionReadable(runtime.target,kRequestSize) || !sha256(runtime.target,kRequestSize,hash)){
        msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=whole_site_hash_unavailable");return false;
    }
    bool siteHash=std::strcmp(hash,kRequestSha256)==0;
    bool prefix=std::memcmp(runtime.native.data(),kRequestPrefix,sizeof(kRequestPrefix))==0;
    msm2::writeLog("REQUEST_PRESENCE_SITE_IDENTITY whole_sha256=%s expected=%s hash_ok=%u prefix_ok=%u prefix_bytes=7 site_bytes=%llu",hash,kRequestSha256,unsigned(siteHash),unsigned(prefix),static_cast<unsigned long long>(kRequestSize));
    bool spanOk=validateInstructionSpan(reinterpret_cast<const unsigned char*>(runtime.target));
    bool pdataOk=exactRequestUnwind(target,moduleBase);
    if(!siteHash || !prefix || !spanOk || !pdataOk){
        msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=site_identity_or_span_or_pdata_mismatch hash_ok=%u prefix_ok=%u span_ok=%u pdata_ok=%u",unsigned(siteHash),unsigned(prefix),unsigned(spanOk),unsigned(pdataOk));return false;
    }
    if(!pinObserverAndValidateUnwind()){
        msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=observer_unwind_metadata_rejected");return false;
    }
    MH_STATUS status=MH_Initialize();
    if(status!=MH_OK && status!=MH_ERROR_ALREADY_INITIALIZED){
        msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=minhook_initialize status=%d",int(status));return false;
    }
    void* original=nullptr;
    status=MH_CreateHook(runtime.target,reinterpret_cast<LPVOID>(&ws_request_presence_observer),&original);
    if(status!=MH_OK || !original){
        msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=minhook_create status=%d",int(status));return false;
    }
    runtime.created=true;
    runtime.trampoline=original;
    ws_request_presence_original=reinterpret_cast<std::uintptr_t>(original);
    bool trampolineOk=validateTrampoline(original,target);
    msm2::writeLog("REQUEST_PRESENCE_TRAMPOLINE ok=%u trampoline=0x%llx expected_forward=0x%llx copied_prefix=7",unsigned(trampolineOk),static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(original)),static_cast<unsigned long long>(target+sizeof(kRequestPrefix)));
    if(!trampolineOk){msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=trampoline_identity_rejected retained_until_process_exit=1");return false;}
    status=MH_EnableHook(runtime.target);
    if(status!=MH_OK){msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=minhook_enable status=%d retained_until_process_exit=1",int(status));return false;}
    runtime.enabled=true;
    if(!copyCode(runtime.target,runtime.patch.data(),runtime.patch.size())){
        MH_DisableHook(runtime.target);runtime.enabled=false;msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=patch_snapshot_unreadable disabled_after_enable=1");return false;
    }
    std::uintptr_t relay=0;
    bool relayOk=patchRelay(runtime.target,relay);
    bool suffixOk=std::memcmp(runtime.patch.data()+5,runtime.native.data()+5,runtime.native.size()-5)==0;
    if(!relayOk || !suffixOk){
        MH_STATUS disabled=MH_DisableHook(runtime.target);
        runtime.enabled=false;
        bool restored=nativeRestored();
        msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=0 reason=enabled_patch_identity_rejected relay_ok=%u suffix_ok=%u disable_status=%d native_restored=%u retained_until_process_exit=1",unsigned(relayOk),unsigned(suffixOk),int(disabled),unsigned(restored));
        return false;
    }
    runtime.admitted=true;
    msm2::writeLog("REQUEST_PRESENCE_ADMISSION admitted=1 target=0x%llx relay=0x%llx observer=0x%llx trampoline=0x%llx whole_site_sha=verified pdata=exact observer_unwind=split_verified callback_io=0 callback_locks=0 callback_allocations=0 captured_pointer_dereference=0 argument_writes=0 request_suppression=0 original_forward=tail_trampoline retention=process_exit",static_cast<unsigned long long>(target),static_cast<unsigned long long>(relay),static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(&ws_request_presence_observer)),static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(original)));
    startCapture();
    return true;
}

void tick() {
    if(runtime.phase==PhaseState::Capturing && atomicRead(&captureCloseRequested)==0 && GetTickCount64()>=runtime.nextDrainTick) drainBatch("periodic_worker",true);
}

void armNextGround() {
    msm2::writeLog("REQUEST_PRESENCE_ARM rejected=1 class=legacy_manual_control reason=F11_disabled_capture_auto_starts_on_admission");
}

void armNextWallRun() {
    msm2::writeLog("REQUEST_PRESENCE_ARM rejected=1 class=legacy_manual_control reason=F12_disabled_capture_auto_starts_on_admission");
}

void closeCapture(const char* reason) {
    InterlockedExchange(&captureCloseRequested,1);
    InterlockedExchange(&ws_request_presence_enabled,0);
    msm2::writeLog("REQUEST_PRESENCE_CAPTURE_GATE closed=1 reason=%s prior_periodic_batches_already_persisted=1",reason?reason:"unspecified");
}

void stop(const char* reason) {
    if(runtime.phase==PhaseState::Stopped){logSession(reason);return;}
    InterlockedExchange(&captureCloseRequested,1);
    InterlockedExchange(&ws_request_presence_enabled,0);
    if(runtime.phase==PhaseState::Capturing) drainBatch(reason?reason:"stop",false);
    LONG active=atomicRead(&ws_request_presence_active_callbacks);
    if(runtime.enabled && runtime.target){
        std::array<unsigned char,kNativeSnapshot> current{};
        bool currentKnown=copyCode(runtime.target,current.data(),current.size());
        if(active==0 && currentKnown && current==runtime.patch){
            MH_STATUS disabled=MH_DisableHook(runtime.target);
            bool restored=disabled==MH_OK && nativeRestored();
            msm2::writeLog("REQUEST_PRESENCE_DISABLE owned_patch=1 active_callbacks=0 status=%d native_restored=%u trampoline_retained=1 observer_module_pinned=1",int(disabled),unsigned(restored));
            if(disabled==MH_OK) runtime.enabled=false;
        }else{
            msm2::writeLog("REQUEST_PRESENCE_DISABLE owned_patch=0_or_busy skip_disable=1 active_callbacks=%ld current_readable=%u patch_match=%u capture_gate=closed code_and_trampoline_retained=1",active,unsigned(currentKnown),unsigned(currentKnown&&current==runtime.patch));
        }
    }
    runtime.admitted=false;
    runtime.phase=PhaseState::Stopped;
    logSession(reason);
}

bool available() {return runtime.admitted&&runtime.enabled;}

} // namespace msm2::request_presence