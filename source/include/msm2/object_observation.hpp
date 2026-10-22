#pragma once
#include <windows.h>
#include "msm2/sdk_bridge.hpp"
#include "object_candidates.hpp"
namespace msm2 {
inline bool readBoundedObject(std::uintptr_t owner,unsigned char* out){
 // ReadProcessMemory copies once after page checks; handles an unmapping race.
 // This does not establish object allocation size or semantic lifetime.
 constexpr auto size=ws::discovery::bytes;
 if(!owner||owner>UINTPTR_MAX-size)return false;
 auto cursor=owner,end=owner+size;
 while(cursor<end){MEMORY_BASIC_INFORMATION m{};
  if(!VirtualQuery(reinterpret_cast<void*>(cursor),&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
  auto protection=m.Protect&0xff;
  if(protection!=PAGE_READONLY&&protection!=PAGE_READWRITE&&protection!=PAGE_WRITECOPY&&protection!=PAGE_EXECUTE_READ&&protection!=PAGE_EXECUTE_READWRITE&&protection!=PAGE_EXECUTE_WRITECOPY)return false;
  auto base=reinterpret_cast<std::uintptr_t>(m.BaseAddress);if(m.RegionSize>UINTPTR_MAX-base)return false;
  auto next=base+m.RegionSize;if(next<=cursor)return false;cursor=std::min(end,next);
 }
 return readMemory(owner,out,size);
}
}
