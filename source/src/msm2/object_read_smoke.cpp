#include "msm2/object_observation.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
namespace msm2 {
bool readMemory(std::uintptr_t address,void* output,std::size_t size){SIZE_T count=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),output,size,&count)&&count==size;}
}
int main(){
 auto p=static_cast<unsigned char*>(VirtualAlloc(nullptr,8192,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);std::memset(p,0x42,8192);
 auto owner=reinterpret_cast<std::uintptr_t>(p);unsigned char out[512]{};
 assert(msm2::readBoundedObject(owner,out));assert(out[0]==0x42&&out[511]==0x42);
 DWORD old=0;assert(VirtualProtect(p,8192,PAGE_READONLY,&old));assert(msm2::readBoundedObject(owner,out));
 assert(VirtualProtect(p+4096,4096,PAGE_NOACCESS,&old));assert(!msm2::readBoundedObject(owner+4096-256,out));
 assert(VirtualProtect(p,4096,PAGE_READONLY|PAGE_GUARD,&old));assert(!msm2::readBoundedObject(owner,out));
 assert(!msm2::readBoundedObject(0,out));assert(!msm2::readBoundedObject(UINTPTR_MAX-10,out));
 assert(VirtualFree(p,0,MEM_RELEASE));assert(!msm2::readBoundedObject(owner,out));
 std::puts("PASS: production bounded object reader: readable/read-only pages, guard/noaccess/cross-page/unmapped/null/overflow refusal; no game function calls");
}
