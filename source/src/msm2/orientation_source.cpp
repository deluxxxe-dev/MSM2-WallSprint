#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <MinHook.h>
#include <array>
#include <atomic>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include "msm2/orientation_source.hpp"
#include "diagnostics.hpp"
#include "NativeHooks/native_source_override.hpp"
#include "NativeHooks/immediate_reset_trace.hpp"
extern "C" {
std::atomic<unsigned> ws_source_recording{0};
void* ws_source_original{};
void ws_source_thunk();
}
namespace {
using msm2::writeLog;
constexpr unsigned ringCap=512,windowsCap=8;
constexpr std::uintptr_t writerRva=0x2cfc580;
constexpr unsigned writerSize=584;
constexpr char writerHash[]="3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6";
constexpr unsigned char prefix[32]={0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x41,0x56,0x48,0x83,0xec,0x60,0x80,0x79,0x5f,0x00,0x49,0x8b,0xf8,0xc5,0xf8,0x29,0x70};
struct Frame {std::uintptr_t r11,r10,r9,r8,rdx,rcx,rax,flags,ret;};
static_assert(offsetof(Frame,rcx)==40&&offsetof(Frame,ret)==64);
struct Window {unsigned id{},state{};std::uint64_t session{},start{},end{};std::uintptr_t hero{},transform{};ws::Vec3 normal{};std::atomic<unsigned> count{0},other{0},drops{0};};
struct Event {unsigned window{},thread{};std::uint64_t ms{};std::uintptr_t ret{},args[4]{};bool valid[4]{};unsigned char bytes[4][128]{};};
msm2::input::Config cfg;std::uintptr_t base{},target{};std::size_t imageSize{};std::atomic<bool> admitted{false},installed{false};bool created{},resident{};std::atomic<bool> killed{true};
unsigned char owned[32]{},originalWriterBody[584]{},ownedWriterBody[584]{};unsigned used{};std::atomic<unsigned> current{windowsCap};
std::array<Window,windowsCap> windows;std::array<Event,ringCap> ring;std::atomic<unsigned> head{0},tail{0};std::atomic_flag lock=ATOMIC_FLAG_INIT;
msm2::Snapshot last{};ws::Vec3 normal{};bool normalValid{},wasActive{},deinit{};std::uint64_t deinitMs{},runDone{},crawlDone{},lastSession{},lastSample{};unsigned stable{};
bool readable(std::uintptr_t p,void* out,std::size_t n){
 // Canonical user pointer, at most0x80, exactly one level; never touch guarded/uncommitted pages.
 if(p<0x10000||p>0x00007fffffffffffULL||!n||n>128||p>0x00007fffffffffffULL-n)return false;
 auto end=p+n;for(auto pos=p;pos<end;){MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(pos),&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
  auto prot=m.Protect&0xff;if(prot!=PAGE_READONLY&&prot!=PAGE_READWRITE&&prot!=PAGE_WRITECOPY&&prot!=PAGE_EXECUTE_READ&&prot!=PAGE_EXECUTE_READWRITE&&prot!=PAGE_EXECUTE_WRITECOPY)return false;
  auto begin=reinterpret_cast<std::uintptr_t>(m.BaseAddress);if(m.RegionSize>UINTPTR_MAX-begin||begin+m.RegionSize<=pos)return false;pos=std::min(end,begin+m.RegionSize);
 }
 return msm2::readMemory(p,out,n);
}
void hex(const unsigned char* p,unsigned n,char* text){constexpr char digits[]="0123456789abcdef";for(unsigned i=0;i<n;++i){text[i*2]=digits[p[i]>>4];text[i*2+1]=digits[p[i]&15];}text[n*2]=0;}
std::string sha256(const std::vector<unsigned char>& b){
 BCRYPT_ALG_HANDLE alg=nullptr;BCRYPT_HASH_HANDLE h=nullptr;DWORD size=0,done=0;std::vector<unsigned char> obj;unsigned char digest[32]{};
 bool ok=BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)>=0;
 if(ok)ok=BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&size),sizeof(size),&done,0)>=0;
 if(ok){obj.resize(size);ok=BCryptCreateHash(alg,&h,obj.data(),size,nullptr,0,0)>=0;}
 if(ok)ok=BCryptHashData(h,const_cast<PUCHAR>(b.data()),static_cast<ULONG>(b.size()),0)>=0;
 if(ok)ok=BCryptFinishHash(h,digest,32,0)>=0;
 if(h)BCryptDestroyHash(h);
 if(alg)BCryptCloseAlgorithmProvider(alg,0);
 char text[65]{};hex(digest,32,text);return ok?text:"unavailable";
}
bool captureFunction(const char* label,std::uintptr_t pc,unsigned expected=0){
 DWORD64 image=0;auto* f=RtlLookupFunctionEntry(pc,&image,nullptr);
 if(!f||image!=base||f->EndAddress<=f->BeginAddress||f->EndAddress>imageSize||f->EndAddress-f->BeginAddress>0x10000||(expected&&(base+f->BeginAddress!=pc||f->EndAddress-f->BeginAddress!=expected))){writeLog("orientation_source_function label=%s available=0 reason=runtime_boundary_or_64KiB_cap",label);return false;}
 auto begin=base+f->BeginAddress;unsigned size=f->EndAddress-f->BeginAddress;std::vector<unsigned char> b(size);
 if(!msm2::readMemory(begin,b.data(),size))return false;
 auto hash=sha256(b);
 if(expected&&size==584&&hash==writerHash)std::memcpy(originalWriterBody,b.data(),584);
 writeLog("orientation_source_function label=%s available=1 begin_rva=0x%lx end_rva=0x%lx size=%u sha256=%s",label,f->BeginAddress,f->EndAddress,size,hash.c_str());
 // Full body is used privately for authentication; not dumped in this revision.
 return !expected||hash==writerHash;
}
unsigned uniquePrefix(){
 IMAGE_DOS_HEADER dos{};IMAGE_NT_HEADERS64 nt{};if(!msm2::readMemory(base,&dos,sizeof(dos))||dos.e_magic!=IMAGE_DOS_SIGNATURE||dos.e_lfanew<=0||dos.e_lfanew>0x100000||!msm2::readMemory(base+dos.e_lfanew,&nt,sizeof(nt))||nt.Signature!=IMAGE_NT_SIGNATURE||nt.FileHeader.NumberOfSections>96)return 0;
 auto table=base+dos.e_lfanew+24+nt.FileHeader.SizeOfOptionalHeader;unsigned found=0;std::array<unsigned char,65536+31> block{};
 for(unsigned n=0;n<nt.FileHeader.NumberOfSections;++n){IMAGE_SECTION_HEADER s{};if(!msm2::readMemory(table+n*sizeof(s),&s,sizeof(s)))return 0;if(!(s.Characteristics&IMAGE_SCN_MEM_EXECUTE))continue;
  unsigned size=s.Misc.VirtualSize;if(s.VirtualAddress>imageSize||size>imageSize-s.VirtualAddress)return 0;
  for(unsigned off=0;off<size;off+=65536){auto count=std::min(65536u,size-off);auto bytes=std::min(count+31,size-off);if(!msm2::readMemory(base+s.VirtualAddress+off,block.data(),bytes))return 0;
   for(unsigned i=0;i<count&&i+32<=bytes;++i)if(!std::memcmp(block.data()+i,prefix,32)){if(++found>1)return found;}
  }
 }
 return found;
}
bool ownership(){unsigned char b[32]{};return installed&&msm2::readMemory(target,b,32)&&!std::memcmp(b,owned,32);}
const char* callerClass(std::uintptr_t rva){return rva==0x2cfc8f3?"WALLRUN_CALLER":rva==0x1021c9e||rva==0x1022c15?"CRAWL_SURFACE_CALLER":"OTHER_CALLER";}
void finish(const char* reason){auto id=current.exchange(windowsCap);ws_source_recording=0;if(id>=used)return;auto& w=windows[id];writeLog("orientation_source_window_end window=%u session=%llu reason=%s captured=%u other_target_captured=%u dropped=%u",w.id,static_cast<unsigned long long>(w.session),reason,std::min(w.count.load(),cfg.maxEvents),w.other.load(),w.drops.load());}
void begin(const msm2::Snapshot& s,unsigned state,std::uint64_t session,ws::Vec3 n){
 if(used>=cfg.maxWindows||used>=windowsCap)return;
 auto index=used++;auto& w=windows[index];w.id=index+1;w.state=state;w.session=session;w.start=GetTickCount64();w.end=w.start+cfg.windowMs;w.hero=s.hero;w.transform=s.transform;w.normal=n;
 current.store(index,std::memory_order_release);ws_source_recording=1;
 writeLog("orientation_source_window window=%u state=%s session=%llu hero=0x%llx target_transform=0x%llx N398=(%.9g,%.9g,%.9g) window_ms=%u source_normal=%s verified_WallCrawl_state=0",w.id,state==1?"WallRun":"CrawlCandidate",static_cast<unsigned long long>(session),static_cast<unsigned long long>(s.hero),static_cast<unsigned long long>(s.transform),n.x,n.y,n.z,cfg.windowMs,state==1?"active_WallRun":"cached_pre_Deinit_NOT_fresh_contact");
}
}
extern "C" void ws_source_capture(const Frame* f) noexcept {
 // Source override has its own exact-build/native lifecycle/stack-only gates.
 msm2::native_source::capture(f->rcx,f->rdx,f->r8,f->r9,f->ret,reinterpret_cast<std::uintptr_t>(&f->ret));
 msm2::reset_trace::writerEntry(f->rcx,f->rdx,f->r8,f->r9,f->ret,msm2::native_source::entryOverridden());
 // Optional prior read-only snapshots remain independent.
 auto index=current.load(std::memory_order_acquire);if(killed.load()||index>=windowsCap)return;
 struct PreserveLastError {DWORD value=GetLastError();~PreserveLastError(){SetLastError(value);}} preserveLastError;
 auto& w=windows[index];auto time=GetTickCount64();if(time>w.end)return;
 if(lock.test_and_set(std::memory_order_acquire)){++w.drops;return;}
 auto h=head.load(std::memory_order_relaxed);auto t=tail.load(std::memory_order_acquire);
 bool other=f->rcx!=w.transform;
 if(killed.load()||current.load()!=index||w.count.load()>=cfg.maxEvents||(other&&w.other.load()>=16)){lock.clear(std::memory_order_release);return;}
 if(h-t>=ringCap){++w.drops;lock.clear(std::memory_order_release);return;}
 Event e{};e.window=index;e.thread=GetCurrentThreadId();e.ms=time;e.ret=f->ret;e.args[0]=f->rcx;e.args[1]=f->rdx;e.args[2]=f->r8;e.args[3]=f->r9;
 e.valid[1]=readable(f->rdx,e.bytes[1],128);
 if(cfg.otherArguments)for(unsigned i: {0u,2u,3u})e.valid[i]=readable(e.args[i],e.bytes[i],128);
 ring[h%ringCap]=e;++w.count;if(other)++w.other;head.store(h+1,std::memory_order_release);lock.clear(std::memory_order_release);
}
namespace msm2::input {
bool authenticated(){unsigned char body[584]{};return admitted&&!killed.load()&&ownership()&&readMemory(target,body,584)&&!std::memcmp(body,ownedWriterBody,584);}
void configure(Config c,bool verified,std::uintptr_t image,std::size_t size){
 kill();if(created||used){writeLog("orientation_source_config admitted=0 reason=full_process_restart_required");return;}
 cfg=c;base=image;imageSize=size;target=base+writerRva;
 admitted=verified&&c.enabled&&size>writerRva+writerSize&&c.windowMs>=100&&c.windowMs<=2500&&c.maxWindows>0&&c.maxWindows<=windowsCap&&c.maxEvents>0&&c.maxEvents<=256;
 writeLog("orientation_source_config requested=%u admitted_build=%u actor_writes=0 unknown_game_calls=0 RDX_snapshot_bytes=128 one_level_only=1 other_arg_snapshots=%u",unsigned(c.enabled),unsigned(admitted),unsigned(c.otherArguments));
 if(!admitted)return;
 unsigned char b[32]{};
 if(!readMemory(target,b,32)||std::memcmp(b,prefix,32)||uniquePrefix()!=1||!captureFunction("writer",target,writerSize)){admitted=false;writeLog("orientation_source_hook installed=0 reason=prefix_unique_AOB_boundary_or_original_hash_failed");return;}
 for(auto caller:{std::uintptr_t(0x2cfc8f3),std::uintptr_t(0x1021c9e),std::uintptr_t(0x1022c15)}){char label[40];std::snprintf(label,sizeof(label),"caller_%llx",static_cast<unsigned long long>(caller));captureFunction(label,base+caller-1);}
 if(!resident){HMODULE pin=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&configure),&pin)){admitted=false;return;}resident=true;}
 auto init=MH_Initialize();if(init!=MH_OK&&init!=MH_ERROR_ALREADY_INITIALIZED){admitted=false;return;}
 auto code=MH_CreateHook(reinterpret_cast<void*>(target),reinterpret_cast<void*>(&ws_source_thunk),&ws_source_original);created=code==MH_OK;
 if(!created||!readMemory(target,b,32)||std::memcmp(b,prefix,32)){admitted=false;return;}
 auto enabled=MH_EnableHook(reinterpret_cast<void*>(target));installed=enabled==MH_OK;
 if(installed&&(!readMemory(target,owned,32)||!readMemory(target,ownedWriterBody,584)||std::memcmp(ownedWriterBody+32,originalWriterBody+32,552))){MH_DisableHook(reinterpret_cast<void*>(target));installed=false;}
 admitted=installed.load();killed=!admitted.load();
 writeLog("orientation_source_hook installed=%u writer_rva=0x2cfc580 original_size=584 exact_hash=%s unique_AOB=1 known_prefix=1 runtime_boundary=1 passthrough=tailjump no_typed_game_signature=1",unsigned(installed),writerHash);
}
void kill() noexcept{killed=true;ws_source_recording=0;}
void tick(const ws::observed::VerifiedWallContext& ctx,const char* reason,bool authenticated){
 auto time=GetTickCount64();auto index=current.load();
 if(!authenticated||!ctx.lifecycleVerified){kill();finish("lifecycle_or_patch_gate_lost");return;}
 if(killed.load()||!admitted){finish("disabled");return;}
 if(!ownership()){kill();finish("writer_patch_ownership_lost");return;}
 if(index<windowsCap){auto& w=windows[index];if(time>w.end||w.count>=cfg.maxEvents||bool(w.state==1)!=ctx.wallRunActive)finish("timeout_eventcap_or_native_state_change");}
 if(!cfg.observe)return;
 if(wasActive&&!ctx.wallRunActive){deinit=std::strcmp(reason,"verified_deinit")==0;deinitMs=time;stable=0;}wasActive=ctx.wallRunActive;
 if(!ctx.wallRunActive&&(!deinit||time-deinitMs>4000)){finish("outside_post_Deinit_window");return;}
 if(time-lastSample<20)return;
 lastSample=time;
 Snapshot s{};if(!sample(s)){finish("Hero_unavailable");deinit=false;normalValid=false;return;}
 auto axes=ws::diag::actorAxes(s.prefix);if(!axes.valid){finish("invalid_orientation");stable=0;return;}
 if(last.hero&&(last.hero!=s.hero||last.transform!=s.transform)){finish("Hero_or_Transform_changed");deinit=false;normalValid=false;last=s;runDone=ctx.wallRunSessionId;return;}
 if(ctx.wallRunActive){
  last=s;lastSession=ctx.wallRunSessionId;deinit=false;
  ws::Vec3 n{};normalValid=ctx.wallRunOwner<=UINTPTR_MAX-0x3a4&&readMemory(ctx.wallRunOwner+0x398,&n,12)&&ws::unit(n,normal);
  if(!normalValid){finish("active_normal_invalid");return;}
  if(current.load()>=windowsCap&&ctx.elapsed>=.7&&ctx.wallRunUpdateCalls&&normalValid&&runDone!=lastSession){begin(s,1,lastSession,normal);runDone=lastSession;}
 }else if(deinit&&s.hero==last.hero&&s.transform==last.transform){
  if(std::abs(axes.up.y)<.35f)++stable;else {stable=0;finish("CrawlCandidate_orientation_lost");}
  if(current.load()>=windowsCap&&stable>=3&&normalValid&&crawlDone!=lastSession){begin(s,2,lastSession,normal);crawlDone=lastSession;}
 }
}
void drainLogs(){
 for(unsigned n=0;n<ringCap;++n){auto t=tail.load(std::memory_order_relaxed);if(t==head.load(std::memory_order_acquire))break;auto e=ring[t%ringCap];tail.store(t+1,std::memory_order_release);auto& w=windows[e.window];auto rva=e.ret>=base&&e.ret-base<imageSize?e.ret-base:0;
  writeLog("ORIENTATION_WRITER_ENTRY event=%u window=%u state=%s session=%llu thread_id=%u writer_rva=0x2cfc580 return_address=0x%llx caller_rva=0x%llx caller_class=%s RCX=0x%llx RDX=0x%llx R8=0x%llx R9=0x%llx target_transform=0x%llx RCX_matches_target=%u N398=(%.9g,%.9g,%.9g) native_ms=%llu state_source=worker_window_NOT_engine_WallCrawl_enum",t+1,w.id,w.state==1?"WallRun":"CrawlCandidate",static_cast<unsigned long long>(w.session),e.thread,static_cast<unsigned long long>(e.ret),static_cast<unsigned long long>(rva),callerClass(rva),static_cast<unsigned long long>(e.args[0]),static_cast<unsigned long long>(e.args[1]),static_cast<unsigned long long>(e.args[2]),static_cast<unsigned long long>(e.args[3]),static_cast<unsigned long long>(w.transform),unsigned(e.args[0]==w.transform),w.normal.x,w.normal.y,w.normal.z,static_cast<unsigned long long>(e.ms));
  const char* names[]={"RCX","RDX","R8","R9"};for(unsigned i=0;i<4;++i)if(i==1||cfg.otherArguments){char text[257]{};if(e.valid[i])hex(e.bytes[i],128,text);writeLog("ORIENTATION_SOURCE_SNAPSHOT event=%u argument=%s pointer=0x%llx readable=%u size=%u bytes=%s one_level=1",t+1,names[i],static_cast<unsigned long long>(e.args[i]),unsigned(e.valid[i]),e.valid[i]?128:0,e.valid[i]?text:"unavailable");}
 }
}
void stop(){kill();finish("stop");drainLogs();if(installed){if(ownership()){auto code=MH_DisableHook(reinterpret_cast<void*>(target));if(code==MH_OK)installed=false;writeLog("orientation_source_shutdown restored=%u status=%d trampoline_retained=1",unsigned(!installed),int(code));}else writeLog("orientation_source_shutdown restored=0 ownership_lost=1 foreign_patch_untouched=1");}}
}
