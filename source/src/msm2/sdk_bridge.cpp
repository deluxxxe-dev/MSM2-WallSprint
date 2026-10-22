// Uses the unmodified public MSM2 template game headers; see analysis/sdk_sources.json.
#include <windows.h>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <limits>
#include <vector>
#include "msm2/sdk_bridge.hpp"
#include "pattern.hpp"
#include "game/HeroSystem.h"
#include "game/Transform.h"
#include "game/Component/Component.h"
namespace {
HeroSystem* heroSystem{};
Actor* (*actorFunction)(ActorHandle*){};
bool initialized{};
std::array<msm2::HookCandidate,4> candidates{};
std::uintptr_t base{};std::size_t imageSize{};
constexpr char heroPattern[]="48 89 05 ** ** ** ** 89 35 ?? ?? ?? ?? 48 89 35";
constexpr char actorPattern[]="44 8B 01 41 8B D0 C1 EA 14 81 E2 FF 07 00 00 74 ?? 41 81 E0 FF FF 0F 00 4B 8D 0C ?? 48 C1 E1 06 48 03 0D ?? ?? ?? ?? 44 3B 05 ?? ?? ?? ?? 73 ?? 0F B7 41 ?? 3B D0 74 ?? 33 C9 48 8B C1";
bool sectionReadable(std::uintptr_t address,std::size_t size){
 MEMORY_BASIC_INFORMATION info{};
 if(!VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))||info.State!=MEM_COMMIT||(info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;
 auto end=reinterpret_cast<std::uintptr_t>(info.BaseAddress)+info.RegionSize;
 return address<=end&&size<=end-address;
}
extern "C" bool ws_guarded_call(void (*callback)(void*), void* context);
struct SampleCall {msm2::Snapshot* out;bool ok;};
void sampleCallback(void* context){
 auto& call=*static_cast<SampleCall*>(context);auto* out=call.out;
 {
  auto* sys=GetHeroSystem();if(!sys)return;
  auto* actor=sys->GetHero();if(!actor)return;
  auto* transform=actor->GetTransform();if(!transform)return;
  out->hero=reinterpret_cast<std::uintptr_t>(actor);out->transform=reinterpret_cast<std::uintptr_t>(transform);
  // Read the public structure as a bounded snapshot before inspecting its accessor.
  Transform copy{};
  if(!msm2::readMemory(out->transform,&copy,sizeof(copy)))return;
  auto p=copy.GetPosition();out->position[0]=p.x;out->position[1]=p.y;out->position[2]=p.z;
  std::memcpy(out->prefix,&copy,sizeof(out->prefix));
  // Identical scalar convention to template Transform::GetRotation; not a quaternion.
  out->yaw=copy.GetRotation();
  call.ok=true;
 }
}
bool guardedSample(msm2::Snapshot* out){SampleCall call{out,false};return ws_guarded_call(sampleCallback,&call)&&call.ok;}

}
// Exact public API implementations use MSM2 source patterns; no loader ABI replacement.
HeroSystem* GetHeroSystem(){return initialized?heroSystem:nullptr;}
Actor* GetActor(ActorHandle* handle){return actorFunction?actorFunction(handle):nullptr;}
namespace msm2 {
bool readMemory(std::uintptr_t address,void* output,std::size_t size){
 if(!address||!size||address>std::numeric_limits<std::uintptr_t>::max()-size)return false;
 SIZE_T done=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),output,size,&done)&&done==size;
}
std::array<HookCandidate,4> hookCandidates(){return candidates;}
bool initialize(bool allowGameCalls,bool auditReferencePatterns,bool resolveWallRun){
 candidates={};
 initialized=false;heroSystem=nullptr;actorFunction=nullptr;
 auto module=GetModuleHandleW(nullptr);base=reinterpret_cast<std::uintptr_t>(module);
 IMAGE_DOS_HEADER dos{};if(!readMemory(base,&dos,sizeof(dos))||dos.e_magic!=IMAGE_DOS_SIGNATURE||dos.e_lfanew<0||dos.e_lfanew>1024*1024){writeLog("resolver: invalid DOS header");return false;}
 IMAGE_NT_HEADERS64 nt{};if(!readMemory(base+dos.e_lfanew,&nt,sizeof(nt))||nt.Signature!=IMAGE_NT_SIGNATURE||nt.FileHeader.Machine!=IMAGE_FILE_MACHINE_AMD64||nt.OptionalHeader.Magic!=IMAGE_NT_OPTIONAL_HDR64_MAGIC||nt.FileHeader.NumberOfSections>96){writeLog("resolver: invalid x64 PE");return false;}
 imageSize=nt.OptionalHeader.SizeOfImage;
 if(imageSize<4096||imageSize>2ULL*1024*1024*1024)return false;
 auto table=base+dos.e_lfanew+sizeof(DWORD)+sizeof(IMAGE_FILE_HEADER)+nt.FileHeader.SizeOfOptionalHeader;
 if(table<base||table-base>imageSize||nt.FileHeader.NumberOfSections*sizeof(IMAGE_SECTION_HEADER)>imageSize-(table-base))return false;
 ws::Pattern h(heroPattern),a(actorPattern);ws::Matches hm,am;
 struct Audit {const char* label;ws::Pattern pattern;ws::Matches matches;};
 std::vector<Audit> audit;
 if(auditReferencePatterns||resolveWallRun){
  audit.push_back({"ActorComponentSmall",ws::Pattern("48 89 5C 24 08 57 48 83 EC 20 48 8B DA 48 8B F9 48 85 D2 0F 84 ?? ?? ?? ?? 66 83 79 18 00"),{}});
  audit.push_back({"ActorComponentLarge",ws::Pattern("44 0F B7 49 08 4C 8B C2 4C 8B 11 41 FF C9 49 C1 E8 04 45 23 C1 49 63 C0"),{}});
  audit.push_back({"WallRunEntry",ws::Pattern("48 8B C4 48 89 58 18 48 89 70 20 57 41 54 41 55 41 56 41 57 48 81 EC A0 01 00 00"),{}});
  audit.push_back({"WallRunUpdate",ws::Pattern("48 8B C4 55 53 56 57 41 56 41 57 48 8D A8 F8 FE FF FF 48 81 EC D8 01 00 00 C5 F8 29 70 A8"),{}});
  audit.push_back({"WallRunInit",ws::Pattern("48 89 5C 24 10 57 48 83 EC 20 48 8B D9 E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ??"),{}});
  audit.push_back({"WallRunDeinit",ws::Pattern("40 53 48 83 EC 20 48 8B D9 E8 ?? ?? ?? ?? 48 8B 83 28 01 00 00 48 8B 48 60 48 85 C9 74 ?? 48 8B 01 BA 01 00 00 00 44 8D 42 02"),{}});
 }
 for(unsigned i=0;i<nt.FileHeader.NumberOfSections;++i){
  IMAGE_SECTION_HEADER s{};if(!readMemory(table+i*sizeof(s),&s,sizeof(s)))return false;
  if(!(s.Characteristics&IMAGE_SCN_MEM_EXECUTE))continue;
  auto length=static_cast<std::size_t>(s.Misc.VirtualSize);
  if(s.VirtualAddress>imageSize||length>imageSize-s.VirtualAddress||!sectionReadable(base+s.VirtualAddress,length))return false;
  std::vector<std::uint8_t> buffer(length);if(!readMemory(base+s.VirtualAddress,buffer.data(),length))return false;
  ws::scan(buffer.data(),length,base+s.VirtualAddress,h,hm);ws::scan(buffer.data(),length,base+s.VirtualAddress,a,am);
  for(auto& item:audit)ws::scan(buffer.data(),length,base+s.VirtualAddress,item.pattern,item.matches);
 }
 writeLog("resolver HeroSystem matches=%zu GetActor matches=%zu (required exactly 1 each)",hm.count,am.count);
 for(const auto& item:audit){
  const char* labels[]={"WallRunInit","WallRunEntry","WallRunUpdate","WallRunDeinit"};
  for(unsigned i=0;i<4;++i)if(std::strcmp(item.label,labels[i])==0)candidates[i]={item.matches.count,item.matches.only};
  writeLog("reference_AOB_audit label=%s source=SwingCore_embedded_pattern matches=%zu unique_candidate_RVA=0x%llx semantic_identity=UNVERIFIED hook_installed=0 game_call=0",item.label,item.matches.count,static_cast<unsigned long long>(item.matches.count==1?item.matches.only-base:0));
  if(item.matches.count==1){unsigned char prefix[16]{};if(readMemory(item.matches.only,prefix,sizeof(prefix))){char hex[49]{};for(unsigned i=0;i<16;++i)snprintf(hex+i*3,4,"%02x ",prefix[i]);writeLog("reference_AOB_prefix label=%s bytes=%s (observation only)",item.label,hex);}}
 }
 if(!allowGameCalls){writeLog("target_build_gate=REJECTED; AOB audit only, no game API calls/hooks");return false;}
 if(hm.count!=1||am.count!=1){writeLog("SDK unavailable: unique public signatures not established; no game calls/patches/hooks");return false;}
 std::int32_t disp{};if(!readMemory(hm.only+3,&disp,sizeof(disp)))return false;
 auto target=static_cast<std::uintptr_t>(static_cast<std::intptr_t>(hm.only+7)+disp);
 if(target<base||target-base>imageSize||sizeof(HeroSystem)>imageSize-(target-base)||!sectionReadable(target,sizeof(HeroSystem))){writeLog("resolver HeroSystem RIP target invalid");return false;}
 heroSystem=reinterpret_cast<HeroSystem*>(target);actorFunction=reinterpret_cast<decltype(actorFunction)>(am.only);initialized=true;
 writeLog("SDK initialized backend=SM2ScriptTemplate commit=%s HeroSystem_RVA=0x%llx GetActor_RVA=0x%llx read_only=true",templateCommit,static_cast<unsigned long long>(target-base),static_cast<unsigned long long>(am.only-base));
 return true;
}
bool sample(Snapshot& out){
 out={};if(!initialized||!guardedSample(&out))return false;
 for(float x:out.position)if(!std::isfinite(x)||std::abs(x)>1e7f)return false;
 return true; // Keep nonfinite raw orientation visible to diagnostic validation.
}
}
