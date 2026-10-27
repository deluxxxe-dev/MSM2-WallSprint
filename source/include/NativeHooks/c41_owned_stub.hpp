#pragma once
// TEST ONLY. Reviewed c41 helper bodies copied without changing their guards.
// Origin: continuation_witness_smoke.cpp relative/encode/decode/current/prepare.
// No production include or linkage. The caller owns identity/publication/lifetime.
#include <windows.h>
#include <capstone/capstone.h>
#include <array>
#include <cstdint>
#include <cstring>
#include <climits>
namespace ws::c41_owned_stub {
struct Ticket {
 DWORD thread{};unsigned sequence{},watchLease{};std::uint64_t gen{};std::uintptr_t original{};
 HANDLE ready{},prepared{},held{},release{};unsigned char* storage{};volatile LONG* marker{};
 LONG token{};std::array<unsigned char,15> bytes{};std::array<unsigned char,16> target{};
};
inline LONG load(volatile LONG* p){return InterlockedCompareExchange(p,0,0);}
inline bool relative(std::uintptr_t from,std::uintptr_t to,std::int32_t& d){
 if(to>=from){if(to-from>INT32_MAX)return false;d=std::int32_t(to-from);}
 else {if(from-to>std::uint64_t(INT32_MAX)+1)return false;d=from-to==std::uint64_t(INT32_MAX)+1?INT32_MIN:-std::int32_t(from-to);}return true;
}
inline bool encode(std::uintptr_t code,std::uintptr_t marker,std::uintptr_t target,LONG token,std::array<unsigned char,15>& b){
 if(!code||!marker||!target||!token||code>UINTPTR_MAX-15||marker%4)return false;
 std::int32_t m{},j{};if(!relative(code+10,marker,m)||!relative(code+15,target,j))return false;
 b={0xc7,0x05,0,0,0,0,0,0,0,0,0xe9,0,0,0,0};std::memcpy(b.data()+2,&m,4);std::memcpy(b.data()+6,&token,4);std::memcpy(b.data()+11,&j,4);return true;
}
inline bool decode(const Ticket& t){
 csh h{};if(cs_open(CS_ARCH_X86,CS_MODE_64,&h)!=CS_ERR_OK)return false;
 cs_option(h,CS_OPT_DETAIL,CS_OPT_ON);cs_insn* ins{};auto n=cs_disasm(h,t.bytes.data(),t.bytes.size(),reinterpret_cast<std::uintptr_t>(t.storage),0,&ins);
 bool ok=n==2&&ins[0].id==X86_INS_MOV&&ins[0].size==10&&ins[1].id==X86_INS_JMP&&ins[1].size==5&&ins[0].detail&&ins[1].detail;
 if(ok){auto& x=ins[0].detail->x86;auto& y=ins[1].detail->x86;ok=x.op_count==2&&x.operands[0].type==X86_OP_MEM&&x.operands[0].size==4&&x.operands[0].mem.base==X86_REG_RIP&&x.operands[0].mem.index==X86_REG_INVALID&&x.operands[0].mem.segment==X86_REG_INVALID&&x.operands[1].type==X86_OP_IMM&&std::uint32_t(x.operands[1].imm)==std::uint32_t(t.token)&&ins[0].address+10+x.operands[0].mem.disp==reinterpret_cast<std::uintptr_t>(t.marker)&&y.op_count==1&&y.operands[0].type==X86_OP_IMM&&std::uint64_t(y.operands[0].imm)==t.original&&!x.eflags&&!y.eflags;}
 for(std::size_t i=0;ok&&i<n;++i){cs_regs reads{},writes{};std::uint8_t nr{},nw{};ok=cs_regs_access(h,&ins[i],reads,&nr,writes,&nw)==CS_ERR_OK;for(unsigned j=0;j<nw;++j)if(writes[j]!=X86_REG_RIP)ok=false;}
 if(ins)cs_free(ins,n);
 cs_close(&h);return ok;
}
inline bool current(const Ticket& t){
 if(!t.storage||!t.marker)return false;
 MEMORY_BASIC_INFORMATION code{},data{};
 if(!VirtualQuery(t.storage,&code,sizeof(code))||!VirtualQuery(const_cast<LONG*>(t.marker),&data,sizeof(data))||code.State!=MEM_COMMIT||code.Protect!=PAGE_EXECUTE_READ||data.State!=MEM_COMMIT||data.Protect!=PAGE_READWRITE||code.AllocationBase!=t.storage||data.AllocationBase!=t.storage)return false;
 std::array<unsigned char,15> b{};std::array<unsigned char,16> target{};SIZE_T got{};
 return ReadProcessMemory(GetCurrentProcess(),t.storage,b.data(),b.size(),&got)&&got==b.size()&&b==t.bytes&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(t.original),target.data(),target.size(),&got)&&got==target.size()&&target==t.target;
}
inline bool prepare(Ticket& t){
 MEMORY_BASIC_INFORMATION code{};if(!VirtualQuery(reinterpret_cast<void*>(t.original),&code,sizeof(code))||code.State!=MEM_COMMIT||(code.Protect!=PAGE_EXECUTE_READ&&code.Protect!=PAGE_EXECUTE))return false;
 SIZE_T got{};if(!ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(t.original),t.target.data(),t.target.size(),&got)||got!=t.target.size())return false;
 const auto origin=t.original&~std::uintptr_t(65535);
 for(unsigned i=1;i<=256&&!t.storage;++i){const auto distance=std::uintptr_t((i+1)/2)*65536;std::uintptr_t at{};
  if(i%2){if(origin>UINTPTR_MAX-distance-8192)continue;at=origin+distance;}else{if(origin<distance)continue;at=origin-distance;}
  t.storage=static_cast<unsigned char*>(VirtualAlloc(reinterpret_cast<void*>(at),8192,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));
 }
 if(!t.storage)return false;
 t.marker=reinterpret_cast<volatile LONG*>(t.storage+4096);t.token=LONG(0x41000000u+t.sequence);
 if(!encode(reinterpret_cast<std::uintptr_t>(t.storage),reinterpret_cast<std::uintptr_t>(t.marker),t.original,t.token,t.bytes))return false;
 std::memcpy(t.storage,t.bytes.data(),t.bytes.size());DWORD previous{};
 return VirtualProtect(t.storage,4096,PAGE_EXECUTE_READ,&previous)&&previous==PAGE_READWRITE&&FlushInstructionCache(GetCurrentProcess(),t.storage,15)&&decode(t)&&current(t)&&load(t.marker)==0;
}
} // namespace ws::c41_owned_stub
