// Sparse, independently authored PE/code fixtures; no game bytes or process reads.
#include "NativeHooks/rdx_source_contract.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
constexpr std::uint32_t target=ws::rdx::readIdentityTarget,imageSize=0x3000000;
struct Image {
  std::array<unsigned char,4096> headers{};
  std::array<unsigned char,24> table{};
  std::array<unsigned char,128> code{};
  unsigned snapshots{},codeEndRead{};
  bool unstable{},unreadable{},badTableRead{},missingLookup{},coverageChanges{};
  ws::rdx::RuntimeRegion dynamic{};
  template<class B> static void put32(B &b,unsigned off,std::uint32_t v) { std::memcpy(b.data()+off,&v,4); }
  template<class B> static void put16(B &b,unsigned off,std::uint16_t v) { std::memcpy(b.data()+off,&v,2); }
  void entry(unsigned n,std::uint32_t begin,std::uint32_t end) {
    put32(table,n*12,begin);put32(table,n*12+4,end);put32(table,n*12+8,0x800);
  }
  Image() {
    put16(headers,0,0x5a4d);put32(headers,60,0x80);put32(headers,0x80,0x4550);
    put16(headers,0x84,0x8664);put16(headers,0x86,2);put16(headers,0x94,240);
    constexpr unsigned optional=0x98,section=0x188;
    put16(headers,optional,0x20b);put32(headers,optional+56,imageSize);put32(headers,optional+108,16);
    put32(headers,optional+112+3*8,0x2000);put32(headers,optional+116+3*8,24);
    put32(headers,section+8,0x3000);put32(headers,section+12,target&~0xfffu);
    put32(headers,section+36,0x60000020);put32(headers,section+40+8,24);
    put32(headers,section+40+12,0x2000);put32(headers,section+40+36,0x40000000);
    entry(0,target-0x100,target-0x80);entry(1,target+0x200,target+0x220);
    code.fill(0x90);code[0]=0x89;code[1]=0xc0;code[2]=0xc3;
  }
  static bool read(void *ctx,std::uint32_t rva,void *dst,unsigned n) {
    auto &f=*static_cast<Image*>(ctx);
    if (rva<=f.headers.size() && n<=f.headers.size()-rva) {
      std::memcpy(dst,f.headers.data()+rva,n);return true;
    }
    if (rva>=0x2000 && rva-0x2000<=f.table.size() && n<=f.table.size()-(rva-0x2000)) {
      if (f.badTableRead) return false;
      std::memcpy(dst,f.table.data()+rva-0x2000,n);return true;
    }
    if (rva>=target && rva-target<=f.code.size() && n<=f.code.size()-(rva-target)) {
      ++f.snapshots;f.codeEndRead=std::max(f.codeEndRead,rva+n);
      if (f.unreadable) return false;
      std::memcpy(dst,f.code.data()+rva-target,n);
      if (f.unstable && f.snapshots==2) static_cast<unsigned char*>(dst)[0]^=1;
      return true;
    }
    return false;
  }
  static ws::rdx::RuntimeRegion lookup(void *ctx,std::uint32_t rva) {
    auto &f=*static_cast<Image*>(ctx);
    if (f.coverageChanges && f.snapshots>=2 && rva==target)
      return {true,true,true,target,target+3,0x800};
    if (f.dynamic.found && f.dynamic.begin<=rva && rva<f.dynamic.end) return f.dynamic;
    if (f.missingLookup) return {};
    for (unsigned i=0;i<2;++i) {
      std::uint32_t b{},e{},u{};std::memcpy(&b,f.table.data()+12*i,4);
      std::memcpy(&e,f.table.data()+12*i+4,4);std::memcpy(&u,f.table.data()+12*i+8,4);
      if (b<=rva && rva<e) return {true,true,true,b,e,u};
    }
    return {};
  }
  ws::rdx::ReadTargetIdentity run(bool admitted=true,std::uint32_t t=target,std::uint32_t size=imageSize) {
    snapshots=codeEndRead=0;
    return ws::rdx::authenticateReadTarget(admitted,t,size,read,lookup,this);
  }
};
void flowThroughIdentity(Image &image,bool expectAdmission,bool expectLiveValue) {
  std::array<unsigned char,183> writer{};std::array<unsigned char,137> parent{};
  writer.fill(0x90);parent.fill(0x90);
  auto put=[](auto &b,unsigned at,std::initializer_list<unsigned char> v) {
    std::copy(v.begin(),v.end(),b.begin()+at);
  };
  put(writer,0,{0x48,0x8b,0xf9,0x48,0x8b,0xf2,0x48,0x83,0xec,0x28});
  put(writer,0x27,{0xe8,0,0,0,0});Image::put32(writer,0x28,0x2000-(0x1000+0x2c));
  put(writer,0x2c,{0x8b,6,0x8b,0x5e,4,0x8b,0x56,8});
  put(writer,0x36,{0x89,0x47,0x30});put(writer,0x3c,{0x89,0x5f,0x34});
  put(writer,0x42,{0x89,0x57,0x38});put(writer,0x45,{0xc7,0x47,0x3c,0,0,0x80,0x3f});
  put(writer,0x4c,{0x48,0x83,0xc4,0x28,0xc3});
  put(parent,0,{0x48,0x89,0x7c,0x24,8,0x8b,0x81,0x0c,1,0,0});
  put(parent,0x20,{0xbf,1,0,0,0});put(parent,0x2e,{0x89,0xb9,0x0c,1,0,0});
  put(parent,0x34,{0xe8,0,0,0,0,0x48,0x8b,0x7c,0x24,8,0xc3});
  Image::put32(parent,0x35,target-(0x2000+0x39));
  auto main=ws::rdx::exactCall(writer.data(),writer.size(),0x1000,imageSize,0x27,0x2c);
  ws::rdx::classifyCallee(main,parent.data(),parent.size(),0x2000,0x2089);
  ws::rdx::CalleeAlias alias{};alias.code=parent.data();alias.size=parent.size();
  alias.imageSize=imageSize;alias.authenticated=true;alias.exactWrite.site=0x202e;
  alias.edi.enabled=true;alias.readDependency.site=0x2005;
  auto &call=alias.readDependency.call;call.site=0x2034;call.targetIdentity=image.run();
  assert(call.targetIdentity.admitted==expectAdmission);
  call.identity.target=target;
  if (expectAdmission) {
    call.identity.begin=target;call.identity.end=target+call.targetIdentity.size;
    call.identity.boundaryVerified=true;call.size=call.targetIdentity.size;
    call.authenticated=true;call.code=call.targetIdentity.snapshot.data();
  }
  auto result=ws::rdx::branchWriter(writer.data(),writer.size(),{0x36,0x3c,0x42,0x45},&main,&alias);
  assert(!result.xyz && !alias.resolved && !alias.relevantEffectsResolved && !main.noWrite);
  assert(alias.edi.complete && alias.edi.exact);
  assert(alias.readDependency.flowComplete==expectAdmission);
  if (expectAdmission) assert(bool(alias.readDependency.returnedGpr&1)==expectLiveValue);
  else assert(!call.modeledStates && !std::strcmp(alias.readDependency.reason,call.targetIdentity.reason));
}
int main() {
  Image safe;auto r=safe.run();
  assert(r.admitted && r.missingRuntime && r.tableValid && r.stable && r.size==3 && r.instructions==2);
  assert(r.target==target && safe.snapshots==2 && !std::strcmp(r.method,"strict_single_target_no_unwind_linear"));
  assert(!safe.run(false).admitted && !safe.run(true,target+1).admitted);
  std::puts("PASS: c24 exact pinned no-entry target admits only stable linear modeled entry-through-RET span under build/call/code-region gates");

  Image trap;trap.code.fill(0x90);trap.code[0]=0x75;trap.code[1]=4;
  trap.code[2]=0xc3;trap.code[6]=0x31;trap.code[7]=0xc0;trap.code[8]=0xc3;
  r=trap.run();assert(!r.admitted && !r.size && r.barrier==target &&
      !std::strcmp(r.reason,"target_leaf_control_or_nested_call_refused"));
  Image call;call.code[0]=0xe8;call.code[1]=call.code[2]=call.code[3]=call.code[4]=0;call.code[5]=0xc3;
  assert(!call.run().admitted);
  std::puts("PASS: c24 first-RET trap with another reachable exit, every branch and nested CALL refuse at the actual control instruction");

  Image memory;memory.code[0]=0x8b;memory.code[1]=1;
  r=memory.run();assert(!r.admitted && !std::strcmp(r.reason,"target_leaf_memory_operand_refused"));
  Image stack;stack.code[0]=0x50;stack.code[1]=0xc3;
  r=stack.run();assert(!r.admitted && r.barrier==target);
  Image partial;partial.code[0]=0xb0;partial.code[1]=0;partial.code[2]=0xc3;
  assert(!partial.run().admitted);
  std::puts("PASS: c24 memory, stack construction and partial-register effects never gain no-unwind identity or effect-model admission");

  Image crossing;crossing.entry(1,target+2,target+16);
  r=crossing.run();assert(!r.admitted && crossing.codeEndRead==target+2 &&
      !std::strcmp(r.reason,"target_leaf_no_RET_before_runtime_region"));
  Image dynamic;dynamic.dynamic={true,true,true,target+2,target+16,0x800};
  r=dynamic.run();assert(!r.admitted && dynamic.codeEndRead==target+2);
  Image overlap;overlap.entry(0,target-4,target+4);overlap.entry(1,target-1,target+8);
  r=overlap.run();assert(!r.admitted && !overlap.snapshots &&
      !std::strcmp(r.reason,"target_runtime_metadata_malformed_or_overlapping"));
  std::puts("PASS: c24 static/dynamic runtime-region crossings stop before code reads there; overlapping/ambiguous unwind metadata refuses");

  Image unstable;unstable.unstable=true;r=unstable.run();
  assert(!r.admitted && !r.stable && !std::strcmp(r.reason,"target_guarded_snapshot_unstable"));
  Image unavailable;unavailable.unreadable=true;
  assert(!unavailable.run().admitted);
  Image changed;changed.coverageChanges=true;r=changed.run();
  assert(!r.admitted && !std::strcmp(r.reason,"target_runtime_coverage_changed_before_admission"));
  std::puts("PASS: c24 unavailable/unstable double-read and changing runtime coverage never admit a current snapshot hash");

  Image inside;inside.entry(0,target-1,target+4);r=inside.run();
  assert(!r.admitted && r.lookup.found && !inside.snapshots &&
      !std::strcmp(r.reason,"target_inside_runtime_entry_not_begin"));
  Image normal;normal.entry(0,target,target+3);r=normal.run();
  assert(r.admitted && !r.missingRuntime && r.size==3 && !std::strcmp(r.method,"runtime_function"));
  Image mismatch;mismatch.entry(0,target,target+3);mismatch.missingLookup=true;r=mismatch.run();
  assert(!r.admitted && !mismatch.snapshots && !std::strcmp(r.reason,"target_runtime_lookup_and_PE_table_disagree"));
  Image tooBig;tooBig.entry(0,target,target+129);assert(!tooBig.run().admitted);
  std::puts("PASS: c24 runtime entry start/interior, exact normal identity, lookup-table disagreement and oversized spans are distinct fail-closed classifications");

  Image flags;Image::put32(flags.headers,0x188+36,0xe0000020);r=flags.run();
  assert(!r.admitted && !flags.snapshots);
  Image malformed;Image::put16(malformed.headers,0x84,0x14c);assert(!malformed.run().admitted);
  Image badTable;badTable.badTableRead=true;assert(!badTable.run().admitted);
  Image outside;r=outside.run(true,target,target);assert(!r.admitted && !outside.snapshots);
  std::puts("PASS: c24 outside-image/data-or-writable code, malformed PE and unreadable metadata refuse before target code inspection");

  Image noRet;for(unsigned i=0;i<128;i+=4) {
    noRet.code[i]=0xf3;noRet.code[i+1]=0x0f;noRet.code[i+2]=0x1e;noRet.code[i+3]=0xfa;
  }
  r=noRet.run();assert(!r.admitted && r.instructions==32 && r.barrier==target+128);
  Image cap;cap.code.fill(0x90);r=cap.run();
  assert(!r.admitted && r.instructions==33 && !std::strcmp(r.reason,"target_leaf_instruction_cap"));
  std::puts("PASS: c24 byte/instruction/normal-return bounds remain128/32; no convenient RET or unbounded scan substitutes for a complete admitted exit set");

  Image live;flowThroughIdentity(live,true,true);
  Image dead;dead.code[0]=0x31;dead.code[1]=0xc0;flowThroughIdentity(dead,true,false);
  Image refused;refused.code[0]=0x8b;refused.code[1]=1;flowThroughIdentity(refused,false,false);
  Image badSnapshot;badSnapshot.unstable=true;flowThroughIdentity(badSnapshot,false,false);
  std::puts("PASS: c24 admitted identity alone feeds c23 live/killed return flow; failed identity propagates precise blocker and preserves c19/c20/c21 safety certificates");
}
