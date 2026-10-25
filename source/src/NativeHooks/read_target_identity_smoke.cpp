// Sparse, independently authored PE/code fixtures; no game bytes or process reads.
#include "NativeHooks/rdx_source_contract.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
constexpr std::uint32_t target=ws::rdx::readIdentityTarget,imageSize=0x3000000;
struct Image {
  std::array<unsigned char,4096> headers{};
  std::array<unsigned char,24> table{};
  std::array<unsigned char,128> code{};
  unsigned snapshots{},codeEndRead{},sourceReads{},queries{};
  bool unknownMode{},sourceFirstFail{},sourceFinalFail{},sourceChange{},queryFail{},pageChange{},metadataChange{};
  std::array<unsigned char,8> sourceData{{11,22,33,44,55,66,77,88}};
  ws::rdx::ReadImagePage page{true,true,ws::rdx::readImageSource&~4095u,0x1000,0x1000000,0x02,0x02,4096};
  static constexpr std::uint32_t sourceImageSize=0xd000000;
  void enableSource() {
    put16(headers,0x86,3);put32(headers,0x98+56,sourceImageSize);
    constexpr unsigned sec=0x188+80;
    std::memcpy(headers.data()+sec,".rdata",6);put32(headers,sec+8,4096);
    put32(headers,sec+12,ws::rdx::readImageSource&~4095u);put32(headers,sec+36,0x40000040);
  }
  static bool query(void *ctx,std::uint32_t rva,ws::rdx::ReadImagePage &out) {
    auto &f=*static_cast<Image*>(ctx);++f.queries;
    if (f.queryFail || rva!=ws::rdx::readImageSource) return false;
    out=f.page;if (f.pageChange && f.queries>1) out.protect=0x20;return true;
  }
  bool changeFinal{},unstable{},unreadable{},badTableRead{},missingLookup{},coverageChanges{};
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
    if (rva==ws::rdx::readImageSource && n==8) {
      ++f.sourceReads;
      if ((f.sourceFirstFail && f.sourceReads==1) || (f.sourceFinalFail && f.sourceReads==2)) return false;
      std::memcpy(dst,f.sourceData.data(),8);
      if (f.sourceChange && f.sourceReads==2) static_cast<unsigned char*>(dst)[0]^=1;
      return true;
    }
    if (rva<=f.headers.size() && n<=f.headers.size()-rva) {
      std::memcpy(dst,f.headers.data()+rva,n);
      if (f.metadataChange && (f.sourceReads || (f.unknownMode && f.queries)) && rva==0) static_cast<unsigned char*>(dst)[2]^=1;
      return true;
    }
    if (rva>=0x2000 && rva-0x2000<=f.table.size() && n<=f.table.size()-(rva-0x2000)) {
      if (f.badTableRead) return false;
      std::memcpy(dst,f.table.data()+rva-0x2000,n);return true;
    }
    if (rva>=target && rva-target<=f.code.size() && n<=f.code.size()-(rva-target)) {
      ++f.snapshots;f.codeEndRead=std::max(f.codeEndRead,rva+n);
      if (f.unreadable) return false;
      std::memcpy(dst,f.code.data()+rva-target,n);
      if ((f.unstable && f.snapshots==2) || (f.changeFinal && f.snapshots==3)) static_cast<unsigned char*>(dst)[0]^=1;
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
  ws::rdx::ReadTargetIdentity run(bool admitted=true,std::uint32_t t=target,std::uint32_t size=imageSize,const ws::rdx::ReadEntryTags *tags=nullptr) {
    snapshots=codeEndRead=sourceReads=queries=0;
    return ws::rdx::authenticateReadTarget(admitted,t,size,read,lookup,this,tags,query,unknownMode);
  }
};
constexpr std::uint32_t largeFunctionBase=0x1100000;
struct LargeImage : Image {
  std::vector<unsigned char> entries;
  unsigned chunks{},largestChunk{},lookupCalls{},lastMetadataEnd{};
  bool targetEntry{};
  std::uint32_t failReadAt=UINT32_MAX;
  explicit LargeImage(unsigned count) : entries(std::size_t(count)*12) {
    put32(headers,0x188+12,0x1000000);
    put32(headers,0x188+8,(target&~0xfffu)+0x3000-0x1000000);
    put32(headers,0x98+116+3*8,unsigned(entries.size()));
    put32(headers,0x188+40+8,unsigned(entries.size()));
    for (unsigned i=0;i<count;++i) row(i,largeFunctionBase+i*16,largeFunctionBase+8+i*16,0x800);
  }
  void row(unsigned i,std::uint32_t begin,std::uint32_t end,std::uint32_t unwind) {
    put32(entries,i*12,begin);put32(entries,i*12+4,end);put32(entries,i*12+8,unwind);
  }
  static bool readLarge(void *ctx,std::uint32_t rva,void *dst,unsigned n) {
    auto &f=*static_cast<LargeImage*>(ctx);
    if (rva>=0x2000 && std::uint64_t(rva-0x2000)+n<=f.entries.size()) {
      ++f.chunks;f.largestChunk=std::max(f.largestChunk,n);
      if (rva>=f.failReadAt) return false;
      f.lastMetadataEnd=rva+n;std::memcpy(dst,f.entries.data()+rva-0x2000,n);return true;
    }
    return Image::read(static_cast<Image*>(&f),rva,dst,n);
  }
  static ws::rdx::RuntimeRegion lookupLarge(void *ctx,std::uint32_t rva) {
    auto &f=*static_cast<LargeImage*>(ctx);++f.lookupCalls;
    if (f.targetEntry && !f.missingLookup && target<=rva && rva<target+3)
      return {true,true,true,target,target+3,0x800};
    return {};
  }
  ws::rdx::ReadTargetIdentity runLarge() {
    snapshots=codeEndRead=chunks=largestChunk=lookupCalls=lastMetadataEnd=0;
    return ws::rdx::authenticateReadTarget(true,target,imageSize,readLarge,lookupLarge,this);
  }
};
ws::rdx::CalleeAlias flowThroughIdentity(Image &image,bool expectAdmission,bool expectLiveValue,const ws::rdx::ReadEntryTags *entry=nullptr,
    bool disjointR8=false,bool expectLeafFlow=true,bool mismatchedCode=false,bool otherR9=false,bool conflictingAddresses=false,
    bool imageBaseArgument=false,std::uint32_t moduleSize=imageSize) {
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
  if (otherR9) put(parent,0xb,{0x44,0x8b,0x89,0x0c,1,0,0});
  else if (!disjointR8) put(parent,0xb,{0x44,0x8b,0x81,0x0c,1,0,0}); // other-prewrite tag4 in R8.
  if (conflictingAddresses) put(parent,0xb,{0x49,0x89,0xc9,0x75,3,0x49,0x89,0xf1});
  put(parent,0x20,{0xbf,1,0,0,0});put(parent,0x2e,{0x89,0xb9,0x0c,1,0,0});
  put(parent,0x34,{0x8b,0x91,0x0c,1,0,0}); // postwrite tag2 in EDX.
  if (imageBaseArgument) {
    put(parent,0x3c,{0x48,0x8d,0x0d,0,0,0,0});
    Image::put32(parent,0x3f,ws::rdx::readImageBaseOrigin-(0x2000+0x43));
  }
  put(parent,0x44,{0xe8,0,0,0,0,0x48,0x8b,0x7c,0x24,8,0xc3});
  Image::put32(parent,0x45,target-(0x2000+0x49));
  auto main=ws::rdx::exactCall(writer.data(),writer.size(),0x1000,moduleSize,0x27,0x2c);
  ws::rdx::classifyCallee(main,parent.data(),parent.size(),0x2000,0x2089);
  ws::rdx::CalleeAlias alias{};alias.code=parent.data();alias.size=parent.size();
  alias.imageSize=moduleSize;alias.authenticated=true;alias.exactWrite.site=0x202e;
  alias.edi.enabled=true;alias.readDependency.site=0x2005;
  auto &call=alias.readDependency.call;call.site=0x2044;call.targetIdentity=image.run(true,target,moduleSize,entry);
  assert(call.targetIdentity.admitted==expectAdmission);
  call.identity.target=target;
  if (expectAdmission) {
    call.identity.begin=target;call.identity.end=target+call.targetIdentity.size;
    call.identity.boundaryVerified=true;call.size=call.targetIdentity.size;
    call.authenticated=true;call.code=call.targetIdentity.snapshot.data();
  }
  std::array<unsigned char,128> otherCode=call.targetIdentity.snapshot;
  if (mismatchedCode) { otherCode[0]^=1;call.code=otherCode.data(); }
  auto result=ws::rdx::branchWriter(writer.data(),writer.size(),{0x36,0x3c,0x42,0x45},&main,&alias);
  assert(!result.xyz && !alias.resolved && !alias.relevantEffectsResolved && !main.noWrite);
  assert(alias.edi.complete && alias.edi.exact);
  assert(alias.readDependency.flowComplete==(expectAdmission && expectLeafFlow));
  if (expectAdmission && expectLeafFlow) assert(bool(alias.readDependency.returnedGpr&1)==expectLiveValue);
  else if (!expectAdmission) assert(!call.modeledStates && !std::strcmp(alias.readDependency.reason,call.targetIdentity.reason));
  else assert(!call.modeledStates);
  return alias;
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

  for (unsigned count : {262143u,262145u,527161u,ws::rdx::readRuntimeTableEntryCap}) {
    LargeImage large(count);r=large.runLarge();
    assert(r.admitted && r.tableValid && r.tableReadComplete && r.lookupQueried && r.missingRuntime);
    assert(r.tableEntries==count && r.validatedEntries==count && r.metadataBytesRead==count*12);
    assert(!r.entryCapExceeded && !r.byteCapExceeded && !r.validationSkippedByCap);
    assert(large.chunks==(count+63)/64 && large.largestChunk<=768 &&
           large.lastMetadataEnd==0x2000+count*12);
  }
  std::puts("PASS: c25 valid tables below/above old cap, exact527161 count and new entry ceiling validate completely in bounded64-entry reads");

  {
    LargeImage over(ws::rdx::readRuntimeTableEntryCap+1);r=over.runLarge();
    assert(!r.admitted && r.entryCapExceeded && !r.byteCapExceeded && r.validationSkippedByCap);
    assert(r.directoryRangeInImage && r.directorySizeDivisible && !r.lookupQueried && !over.lookupCalls && !over.chunks && !over.snapshots);
    assert(!std::strcmp(r.reason,"target_runtime_entry_cap_exceeded"));
    LargeImage bytes(ws::rdx::readRuntimeTableByteCap/12+1);r=bytes.runLarge();
    assert(!r.admitted && r.byteCapExceeded && r.validationSkippedByCap && !r.lookupQueried && !bytes.chunks);
    assert(!std::strcmp(r.reason,"target_runtime_metadata_byte_cap_exceeded"));
  }
  std::puts("PASS: c25 entry and metadata-byte budget refusals are explicit, skip entry validation and never imply corruption or missing unwind");

  {
    LargeImage odd(16);Image::put32(odd.headers,0x98+116+3*8,16*12+1);r=odd.runLarge();
    assert(!r.directorySizeDivisible && !r.validationSkippedByCap && !odd.chunks && !r.lookupQueried);
    assert(!std::strcmp(r.reason,"target_runtime_directory_size_not_entry_aligned"));
    LargeImage pair(16);Image::put32(pair.headers,0x98+116+3*8,0);r=pair.runLarge();
    assert(!r.directoryPairValid && !pair.chunks && !r.lookupQueried);
    for (auto address : {imageSize-8,UINT32_MAX-15}) {
      LargeImage overflow(16);Image::put32(overflow.headers,0x98+112+3*8,address);r=overflow.runLarge();
      assert(!r.directoryRangeInImage && !r.lookupQueried && !overflow.chunks && !r.validationSkippedByCap);
      assert(!std::strcmp(r.reason,"target_runtime_directory_outside_admitted_image"));
    }
  }
  std::puts("PASS: c25 non-divisible sizes, RVA/size mismatch and in-image/arithmetic overflow refuse as structure errors before any table scan");

  for (unsigned defect=0;defect<7;++defect) {
    constexpr unsigned count=262145;LargeImage broken(count);
    if (defect==0) broken.row(count-1,largeFunctionBase,largeFunctionBase+8,0x800); // unordered tail.
    if (defect==1) broken.row(count-1,largeFunctionBase+(count-2)*16+4,largeFunctionBase+(count-1)*16+8,0x800);
    if (defect==2) broken.row(count-1,largeFunctionBase+(count-1)*16,largeFunctionBase+8+(count-1)*16,imageSize);
    if (defect==3) broken.row(count-1,largeFunctionBase+(count-1)*16,largeFunctionBase+8+(count-1)*16,0);
    if (defect==4) broken.row(count-1,largeFunctionBase+(count-1)*16,imageSize+8,0x800);
    if (defect==5) broken.row(count-1,largeFunctionBase+(count-1)*16,largeFunctionBase+(count-1)*16,0x800);
    if (defect==6) broken.row(count-1,UINT32_MAX-3,4,0x800);
    r=broken.runLarge();assert(!r.tableValid && r.tableBudgetAdmitted && r.validatedEntries==count-1 &&
        !r.lookupQueried && !broken.snapshots && !r.validationSkippedByCap);
    assert(!std::strcmp(r.reason,"target_runtime_metadata_malformed_or_overlapping"));
  }
  std::puts("PASS: c25 unordered/overlapping tails and invalid unwind RVAs are checked beyond the old cap before any target lookup or no-unwind admission");

  {
    LargeImage unreadable(527161);unreadable.failReadAt=0x2000+64*12;r=unreadable.runLarge();
    assert(!r.tableValid && r.tableReadFailed && !r.tableReadComplete && r.validatedEntries==64 &&
        r.metadataBytesRead==64*12 && !r.lookupQueried && !unreadable.lookupCalls && !unreadable.snapshots);
    assert(r.barrier==unreadable.failReadAt && !std::strcmp(r.reason,"target_runtime_table_unreadable"));
  }
  std::puts("PASS: c25 guarded chunk failure reports exact metadata range and validated prefix; incomplete readability never permits target status or code reads");

  {
    LargeImage containing(527161);containing.targetEntry=true;containing.row(527160,target,target+3,0x800);
    r=containing.runLarge();assert(r.admitted && r.tableValid && r.lookupQueried && !r.missingRuntime &&
        r.validatedEntries==527161 && !std::strcmp(r.method,"runtime_function"));
    containing.missingLookup=true;r=containing.runLarge();
    assert(r.tableValid && r.lookupQueried && !r.admitted && !r.missingRuntime && !containing.snapshots &&
        !std::strcmp(r.reason,"target_runtime_lookup_and_PE_table_disagree"));
  }
  std::puts("PASS: c25 fully validated large-table containing entry uses normal identity; lookup disagreement blocks fallback while validated absence keeps strict c24 proof");
  auto code=[](Image &f,std::initializer_list<unsigned char> bytes) {
    f.code.fill(0x90);std::copy(bytes.begin(),bytes.end(),f.code.begin());
  };
  {
    for (bool wide : {false,true}) {
      Image cmp;if (wide) code(cmp,{0x48,0x83,0xf8,0xff,0xc3});
      else code(cmp,{0x83,0xf8,1,0xc3});
      r=cmp.run();assert(r.admitted && r.cmp.decoded && r.cmp.flagsOnly &&
          r.cmp.formSupported && r.cmp.modelApplied && !r.cmp.hasMemory);
      assert(r.cmp.operandCount==2 && r.cmp.operands[0].width==(wide?8u:4u) &&
          r.cmp.operands[0].parent==0 && !std::strcmp(r.cmp.operands[0].parentName,"rax"));
      assert(!std::strcmp(r.cmp.operands[1].kind,"immediate") && r.cmp.operands[1].immediate==(wide?-1:1));
      auto a=flowThroughIdentity(cmp,true,true);const auto &c=a.readDependency.call;
      assert(c.targetIdentity.cmp.flagsTags==1 && c.returnedFlagsTags==1 &&
          !c.clobberGpr && !c.clobberXmm && (c.returnedGpr[0]&1));
      assert(c.targetIdentity.cmp.operands[1].parent==-1 && !c.targetIdentity.cmp.operands[1].parentTags);
    }
  }
  std::puts("PASS: c26 pinned32/64-bit CMP register/immediate completes stable RET identity, models only six flags, preserves operands and treats signed instruction constants as untainted");

  {
    struct Case {std::initializer_list<unsigned char> bytes;unsigned flags;};
    for (const auto &test : {Case{{0x83,0xf8,1,0xc3},1},Case{{0x83,0xfa,1,0xc3},2},
         Case{{0x41,0x83,0xf8,1,0xc3},4},Case{{0x83,0xf9,1,0xc3},0},
         Case{{0x39,0xd0,0xc3},3},Case{{0x48,0x39,0xd0,0xc3},3},Case{{0x44,0x39,0xc0,0xc3},5}}) {
      Image cmp;code(cmp,test.bytes);auto a=flowThroughIdentity(cmp,true,true);
      const auto &c=a.readDependency.call;
      assert(c.targetIdentity.cmp.samples && c.targetIdentity.cmp.flagsTags==test.flags &&
          c.returnedFlagsTags==test.flags && !c.clobberGpr && !c.clobberXmm);
      assert(c.gpr[0].tags==1 && c.gpr[2].tags==2 && c.gpr[8].tags==4);
    }
    Image same;code(same,{0x39,0xc0,0xc3});auto a=flowThroughIdentity(same,true,true);
    assert(a.readDependency.call.targetIdentity.cmp.operands[0].parentTags==1 &&
        a.readDependency.call.targetIdentity.cmp.operands[1].parentTags==1 &&
        !a.readDependency.call.returnedFlagsTags);
  }
  std::puts("PASS: c26 selected1/postwrite2/other-prewrite4/disjoint CMP flags and register pairs retain exact parent tag unions; same-alias subtraction changes flags only and never kills operands");

  {
    struct Partial {std::initializer_list<unsigned char> bytes;unsigned width;int parent;const char *name;};
    for (const auto &test : {Partial{{0x3c,1,0xc3},1,0,"al"},Partial{{0x80,0xfc,1,0xc3},1,0,"ah"},
         Partial{{0x66,0x83,0xf8,1,0xc3},2,0,"ax"}}) {
      Image partial;code(partial,test.bytes);r=partial.run();
      assert(!r.admitted && !r.size && r.cmp.decoded && r.cmp.flagsOnly &&
          !r.cmp.formSupported && !r.cmp.modelApplied && r.barrier==target);
      assert(r.cmp.operands[0].width==test.width && r.cmp.operands[0].parent==test.parent &&
          r.cmp.operands[0].conservativeParent && !std::strcmp(r.cmp.operands[0].name,test.name));
      auto a=flowThroughIdentity(partial,false,false);
      assert(a.readDependency.call.targetIdentity.cmp.operands[0].parentTags==1 &&
          !a.readDependency.call.targetIdentity.cmp.formSupported && !a.readDependency.call.modeledStates);
    }
    Image memory;code(memory,{0x83,0x38,1,0xc3});r=memory.run();
    assert(!r.admitted && r.cmp.hasMemory && r.cmp.flagsOnly && !r.cmp.formSupported &&
        !r.cmp.operands[0].valueTagsKnown && !std::strcmp(r.cmp.operands[0].base,"rax") &&
        !std::strcmp(r.reason,"pinned_cmp_memory_operand_refused"));
    flowThroughIdentity(memory,false,false);
  }
  std::puts("PASS: c26 low/high8 and16-bit CMP aliases are described but refused, memory value tags stay unknown and no partial/memory semantics or target identity are granted");

  {
    for (unsigned jcc : {0x74u,0x75u,0x72u,0x7fu}) {
      Image branch;code(branch,{0x83,0xf8,1,(unsigned char)jcc,2,0xc3,0x90,0xc3});r=branch.run();
      assert(!r.admitted && !r.size && r.cmp.modelApplied && r.barrier==target+3 &&
          r.branch.decoded && r.branch.conditional && r.branch.flagsDependencyKnown &&
          r.branch.flagsProducer==target && r.branch.entryCarrierMask==1 &&
          std::strcmp(r.branch.condition,"unknown") && std::strcmp(r.branch.flagsRead,"unknown"));
      auto a=flowThroughIdentity(branch,false,false);const auto &c=a.readDependency.call;
      assert(!c.authenticated && !c.identity.boundaryVerified && !c.modeledStates &&
          c.targetIdentity.branch.samples && c.targetIdentity.branch.flagsTags==1 &&
          !std::strcmp(c.reason,"target_leaf_control_or_nested_call_refused"));
    }
    Image disjoint;code(disjoint,{0x83,0xf9,1,0x74,0,0xc3});auto a=flowThroughIdentity(disjoint,false,false);
    assert(a.readDependency.call.targetIdentity.branch.flagsDependencyKnown &&
        a.readDependency.call.targetIdentity.branch.samples && !a.readDependency.call.targetIdentity.branch.flagsTags);
  }
  std::puts("PASS: c26 next Jcc reports precise RVA/condition/flag-producer and actual selected/disjoint tags but refuses both paths; no target hash/effect is issued even for disjoint conditions");

  {
    Image overwritten;code(overwritten,{0x83,0xf8,1,0x31,0xc0,0x74,0,0xc3});
    auto a=flowThroughIdentity(overwritten,false,false);const auto &b=a.readDependency.call.targetIdentity.branch;
    assert(b.flagsProducer==target+3 && b.flagsDependencyKnown && b.samples && !b.flagsTags);
    Image vector;code(vector,{0x83,0xf8,1,0x66,0x0f,0x6e,0xc0,0x66,0x0f,0x7e,0xc2,0x39,0xd1,0x74,0,0xc3});
    a=flowThroughIdentity(vector,false,false);
    assert(a.readDependency.call.targetIdentity.branch.flagsProducer==target+11 &&
        a.readDependency.call.targetIdentity.branch.flagsTags==1);
    Image unsupported;code(unsupported,{0x83,0xf8,1,0x0f,0x95,0xc0,0xc3});r=unsupported.run();
    assert(!r.admitted && r.cmp.modelApplied && r.barrier==target+3);
    Image before;code(before,{0x74,0,0xc3});r=before.run();
    assert(!r.admitted && r.branch.decoded && !r.branch.flagsDependencyKnown);
  }
  std::puts("PASS: c26 branch flag provenance follows modeled overwrites and GPR/XMM copies; incoming unknown flags and unsupported flag/partial consumers never acquire false dependency proof");

  {
    Image unstableCmp;code(unstableCmp,{0x83,0xf8,1,0xc3});unstableCmp.unstable=true;r=unstableCmp.run();
    assert(!r.admitted && !r.cmp.decoded && !r.cmp.samples);
    Image crossingCmp;code(crossingCmp,{0x48,0x83,0xf8,1,0xc3});crossingCmp.entry(1,target+3,target+16);r=crossingCmp.run();
    assert(!r.admitted && !r.cmp.decoded && crossingCmp.codeEndRead==target+3);
    Image shifted;code(shifted,{0x90,0x83,0xf8,1,0xc3});r=shifted.run();
    assert(!r.admitted && !r.cmp.decoded && r.barrier==target+1);
    LargeImage acceptedTable(527161);code(acceptedTable,{0x83,0xf8,1,0xc3});r=acceptedTable.runLarge();
    assert(r.admitted && r.cmp.modelApplied && r.tableReadComplete && r.lookupQueried &&
        r.missingRuntime && r.validatedEntries==527161 && r.metadataBytesRead==6325932);
    LargeImage badTail(527161);code(badTail,{0x83,0xf8,1,0xc3});badTail.row(527160,largeFunctionBase,largeFunctionBase+8,0x800);r=badTail.runLarge();
    assert(!r.admitted && !r.cmp.decoded && !r.lookupQueried && !badTail.snapshots);
  }
  std::puts("PASS: c26 CMP extension remains pinned to exact target and stable snapshot; crossing, full large-table validation and late structural refusal preserve c24/c25 gates");

  // Independently authored instruction fixtures from the accepted opcode form,
  // not a captured target body. Gaps deliberately contain undecodable/unmodeled bytes.
  auto fanout=[](Image &f,bool near=false) {
    f.code.fill(0xcc);
    const unsigned char cmp[]={0x41,0x81,0xf8,0xff,0x3f,0,0};
    std::copy(std::begin(cmp),std::end(cmp),f.code.begin());
    if (near) {
      f.code[7]=0x0f;f.code[8]=0x87;Image::put32(f.code,9,11);
      f.code[13]=0x89;f.code[14]=0xc0;f.code[15]=0xc3;
      f.code[24]=0x89;f.code[25]=0xc0;f.code[26]=0xc3;
    } else {
      f.code[7]=0x77;f.code[8]=7;
      f.code[9]=0x89;f.code[10]=0xc0;f.code[11]=0xc3;
      f.code[16]=0x89;f.code[17]=0xc0;f.code[18]=0xc3;
    }
  };
  ws::rdx::ReadEntryTags clean{};clean.samples=1;clean.gpr[0]=1;clean.gpr[2]=2;
  {
    for (bool near : {false,true}) {
      Image f;fanout(f,near);r=f.run(true,target,imageSize,&clean);
      assert(r.admitted && r.cfgComplete && r.finalCodeRechecked && r.paths==2 &&
          r.branch.expanded && r.branch.edgesExact && r.instructions==6 && r.visits==6);
      assert(r.branch.site==target+7 && r.branch.displacementBytes==(near?4u:1u) &&
          r.branch.fallthrough==target+(near?13u:9u) && r.branch.taken==target+(near?24u:16u));
      assert(r.size==(near?27u:19u) && f.snapshots==3 && !r.pathEffectsDiffer);
      auto a=flowThroughIdentity(f,true,true,&clean,true);const auto &c=a.readDependency.call;
      assert(c.returnPaths==2 && c.branchExpansions && !c.pathEffectsDiffer && (c.returnedGpr[0]&1));
      assert(c.outcomes[0].returnedGpr[0]==c.outcomes[1].returnedGpr[0]);
    }
  }
  std::puts("PASS: c27 exact pinned short/near JA edges and current CMP prove both safe RET paths, ignore unreachable gaps, recheck snapshot and preserve identical returned effects");

  {
    Image f;fanout(f);auto first=f.run();
    assert(!first.admitted && first.branch.edgesExact && !first.branch.expanded &&
        !std::strcmp(first.reason,"target_JA_current_call_tags_required"));
    for (unsigned tag : {1u,2u,4u}) {
      auto tainted=clean;tainted.gpr[8]=tag;r=f.run(true,target,imageSize,&tainted);
      assert(!r.admitted && !r.branch.expanded && r.branch.flagsTags==tag &&
          !std::strcmp(r.reason,"target_JA_tracked_condition_dependence_refused"));
    }
    const auto pre=flowThroughIdentity(f,false,false,nullptr,true);
    ws::rdx::ReadEntryTags observed{};observed.samples=pre.readDependency.call.samples;
    for (unsigned k=0;k<16;++k) { observed.gpr[k]=pre.readDependency.call.gpr[k].tags;observed.xmm[k]=pre.readDependency.call.xmmTags[k]; }
    assert(observed.samples && !observed.gpr[8]);
    const auto reproved=flowThroughIdentity(f,true,true,&observed,true);
    assert(reproved.readDependency.flowComplete && reproved.readDependency.call.returnPaths==2);
    auto a=flowThroughIdentity(f,true,false,&clean,false,false);
    assert(!a.readDependency.call.modeledStates && !a.readDependency.flowComplete &&
        !std::strcmp(a.readDependency.call.reason,"target_JA_tracked_condition_dependence_refused"));
  }
  std::puts("PASS: c27 current call tags are required before fan-out, all selected/postwrite/other condition dependence refuses and per-state effects recheck rather than trusting a disjoint identity input");

  {
    Image different;fanout(different);different.code[16]=0x31;different.code[17]=0xc0;
    r=different.run(true,target,imageSize,&clean);assert(r.admitted && r.cfgComplete && r.pathEffectsDiffer);
    auto a=flowThroughIdentity(different,true,true,&clean,true);const auto &c=a.readDependency.call;
    assert(c.returnPaths==2 && c.pathEffectsDiffer && (c.returnedGpr[0]&1) &&
        (c.outcomes[0].returnedGpr[0]&1) && !(c.outcomes[1].returnedGpr[0]&1));
    assert(c.outcomes[0].ret==target+11 && c.outcomes[1].ret==target+18);
    // Reverse which outcome preserves the selected read: never prefer taken or fallthrough.
    Image reverse;fanout(reverse);reverse.code[9]=0x31;reverse.code[10]=0xc0;
    a=flowThroughIdentity(reverse,true,true,&clean,true);
    assert(!(a.readDependency.call.outcomes[0].returnedGpr[0]&1) &&
        (a.readDependency.call.outcomes[1].returnedGpr[0]&1));
    Image vector;fanout(vector);Image::put32(vector.code,9,0xc06e0f66);vector.code[13]=0xc3;
    Image::put32(vector.code,16,0xc0ef0f66);vector.code[20]=0xc3;
    a=flowThroughIdentity(vector,true,true,&clean,true);
    assert(a.readDependency.call.pathEffectsDiffer && (a.readDependency.call.returnedXmm[0]&1) &&
        (a.readDependency.call.outcomes[0].returnedXmm[0]&1) && !(a.readDependency.call.outcomes[1].returnedXmm[0]&1));
  }
  std::puts("PASS: c27 cloned states and both return effects merge conservatively; differing live/killed selected tags remain path-dependent in either outcome ordering");

  {
    for (auto delta : {0xfeu,0xf7u}) {
      Image f;fanout(f);f.code[8]=(unsigned char)delta;r=f.run(true,target,imageSize,&clean);
      assert(!r.admitted && !r.branch.expanded && !r.branch.takenForward &&
          !std::strcmp(r.reason,"target_JA_backedge_or_loop_refused"));
    }
    Image wrap;fanout(wrap,true);Image::put32(wrap.code,9,0x80000000);r=wrap.run(true,target,imageSize,&clean);
    assert(!r.admitted && !r.branch.edgesExact && !std::strcmp(r.reason,"target_JA_encoding_or_RVA_overflow_refused"));
    Image far;fanout(far,true);Image::put32(far.code,9,0x7fffffff);r=far.run(true,target,imageSize,&clean);
    assert(!r.admitted && r.branch.edgesExact && !r.branch.takenInCode);
    Image lastByte;fanout(lastByte,true);Image::put32(lastByte.code,9,127-13);lastByte.code[127]=0xc3;
    r=lastByte.run(true,target,imageSize,&clean);assert(r.admitted && r.paths==2 && r.size==128);
    auto lastFlow=flowThroughIdentity(lastByte,true,true,&clean,true);assert(lastFlow.readDependency.call.returnPaths==2);
    Image capEdge;fanout(capEdge,true);Image::put32(capEdge.code,9,128-13);r=capEdge.run(true,target,imageSize,&clean);
    assert(!r.admitted && !r.branch.takenInWindow && !std::strcmp(r.reason,"target_JA_edge_outside_byte_allowance"));
  }
  std::puts("PASS: c27 signed edge decoding rejects self/back edges, negative RVA underflow, distant code exits and target byte-cap edges without guessing an outcome");

  {
    Image takenCross;fanout(takenCross);takenCross.entry(1,target+16,target+32);r=takenCross.run(true,target,imageSize,&clean);
    assert(!r.admitted && r.branch.takenRuntimeCross && takenCross.codeEndRead==target+16);
    Image bothCross;fanout(bothCross);bothCross.entry(1,target+9,target+32);r=bothCross.run(true,target,imageSize,&clean);
    assert(!r.admitted && r.branch.fallthroughRuntimeCross && bothCross.codeEndRead==target+9);
    Image dynamicCross;fanout(dynamicCross);dynamicCross.dynamic={true,true,true,target+16,target+32,0x800};
    r=dynamicCross.run(true,target,imageSize,&clean);assert(!r.admitted && r.branch.takenRuntimeCross);
    Image middle;fanout(middle);middle.code[8]=1;r=middle.run(true,target,imageSize,&clean);
    assert(!r.admitted && !r.cfgComplete && !std::strcmp(r.reason,"target_cfg_edge_into_instruction_body"));
  }
  std::puts("PASS: c27 either-edge static/dynamic runtime-region crossings stop before code reads there; overlapping instruction boundaries cannot authenticate a shared tail");

  {
    for (unsigned effect=0;effect<6;++effect) {
      Image f;fanout(f);
      if (effect==0) {f.code[16]=0x8b;f.code[17]=1;}
      if (effect==1) {f.code[16]=0x50;f.code[17]=0xc3;}
      if (effect==2) {f.code[16]=0xe8;Image::put32(f.code,17,0);f.code[21]=0xc3;}
      if (effect==3) {f.code[16]=0xff;f.code[17]=0xe0;}
      if (effect==4) {f.code[16]=0xb0;f.code[17]=0;}
      if (effect==5) {f.code[16]=0x75;f.code[17]=0;}
      r=f.run(true,target,imageSize,&clean);assert(!r.admitted && !r.size && !r.cfgComplete &&
          r.paths==1 && r.barrier==target+16);
      auto a=flowThroughIdentity(f,false,false,&clean,true);
      assert(!a.readDependency.call.modeledStates && !a.readDependency.call.authenticated);
    }
    Image instructionCap;fanout(instructionCap);instructionCap.code[8]=71;
    std::fill(instructionCap.code.begin()+9,instructionCap.code.end(),0x90);
    r=instructionCap.run(true,target,imageSize,&clean);assert(!r.admitted && !std::strcmp(r.reason,"target_cfg_instruction_cap"));
    Image missingRet;fanout(missingRet);std::fill(missingRet.code.begin()+16,missingRet.code.end(),0x90);
    r=missingRet.run(true,target,imageSize,&clean);assert(!r.admitted && !r.cfgComplete);
  }
  std::puts("PASS: c27 one safe RET never authenticates a memory/stack/call/indirect/partial/second-branch leg; instruction/path/normal-return bounds remain fail-closed");

  {
    Image mismatch;fanout(mismatch);
    auto rejected=flowThroughIdentity(mismatch,true,false,&clean,true,false,true);
    assert(!rejected.readDependency.call.modeledStates && !std::strcmp(rejected.readDependency.call.reason,"read_call_CFG_identity_pin_or_snapshot_mismatch"));
    Image change;fanout(change);change.changeFinal=true;r=change.run(true,target,imageSize,&clean);
    assert(!r.admitted && !r.stable && !r.finalCodeRechecked && !std::strcmp(r.reason,"target_cfg_final_snapshot_changed"));
    Image unstable;fanout(unstable);unstable.unstable=true;r=unstable.run(true,target,imageSize,&clean);
    assert(!r.admitted && !r.branch.decoded);
    Image regionChanges;fanout(regionChanges);regionChanges.coverageChanges=true;r=regionChanges.run(true,target,imageSize,&clean);
    assert(!r.admitted && !std::strcmp(r.reason,"target_runtime_coverage_changed_before_admission"));
    Image wrongCMP;fanout(wrongCMP);wrongCMP.code[3]=0xfe;r=wrongCMP.run(true,target,imageSize,&clean);
    assert(!r.admitted && !r.branch.expanded && !std::strcmp(r.reason,"target_JA_exact_CMP_flags_producer_required"));
    LargeImage large(527161);fanout(large);r=ws::rdx::authenticateReadTarget(true,target,imageSize,LargeImage::readLarge,LargeImage::lookupLarge,&large,&clean);
    assert(r.admitted && r.cfgComplete && r.paths==2 && r.validatedEntries==527161 && r.metadataBytesRead==6325932);
  }
  std::puts("PASS: c27 final snapshot/current producer/runtime coverage and complete527161-entry metadata gates remain mandatory; no private target bytes or gameplay-write certificates are needed");

  auto memoryAtFallthrough=[&](Image &f,std::initializer_list<unsigned char> bytes) {
    fanout(f);f.code[8]=31;std::fill(f.code.begin()+9,f.code.end(),0xcc);
    std::copy(bytes.begin(),bytes.end(),f.code.begin()+9);f.code[9+bytes.size()]=0xc3;f.code[40]=0xc3;
  };
  {
    Image load;memoryAtFallthrough(load,{0x8b,0x41,0x10});r=load.run(true,target,imageSize,&clean);
    const auto &m=r.mov;
    assert(!r.admitted && r.branch.expanded && !r.paths && m.decoded && m.isMov && m.site==target+9 &&
        m.operandCount==2 && m.operands[0].width==4 && m.operands[0].access==2 &&
        !std::strcmp(m.operands[0].kind,"GPR") && !std::strcmp(m.operands[0].name,"eax") &&
        m.operands[1].access==1 && m.operands[1].width==4 && m.memoryRead && !m.memoryWrite && !m.memoryRmw &&
        m.pureLoad && m.flagsPreserved && m.effectsKnown && m.fullGprWrites==1 && !m.xmmWrites);
    assert(!m.parentState && !m.addressExact && m.aliasMayMask==ws::rdx::movAnyAlias && !m.samples && m.loadedMayTags==7);
    assert(!std::strcmp(m.reason,"target_MOV_loaded_value_provenance_unproven"));
  }
  std::puts("PASS: c28 pinned MOV exact operand/access/width/register and flags-only preservation metadata never turns a pure read form or symbolic entry token into load or target admission");

  {
    for (auto operand : {0x01u,0x02u,0x03u}) {
      Image f;memoryAtFallthrough(f,{0x8b,(unsigned char)operand});auto a=flowThroughIdentity(f,false,false,&clean,true);
      const auto &m=a.readDependency.call.targetIdentity.mov;
      assert(m.parentState && m.samples && m.memoryRead && m.loadedMayTags==7 && m.operands[0].registerTags==1);
      if (operand==1) assert(m.addressExact && m.addressRoot==1 && (m.aliasMayMask&ws::rdx::movHero));
      else assert(!m.addressExact && (m.aliasMayMask&ws::rdx::movUnknown));
      if (operand==2) assert(m.operands[1].baseKind==2 && m.operands[1].baseTags==2);
    }
    Image selected;memoryAtFallthrough(selected,{0x8b,0x00});auto a=flowThroughIdentity(selected,false,false,&clean,true);
    assert(a.readDependency.call.targetIdentity.mov.addressTags==1 && a.readDependency.call.targetIdentity.mov.loadedMayTags==7);
    Image otherParent;memoryAtFallthrough(otherParent,{0x41,0x8b,0x01});
    a=flowThroughIdentity(otherParent,false,false,&clean,true,true,false,true);
    assert(a.readDependency.call.targetIdentity.mov.addressTags==4 && a.readDependency.call.targetIdentity.mov.loadedMayTags==7);
    Image conflict;memoryAtFallthrough(conflict,{0x41,0x8b,0x01});
    a=flowThroughIdentity(conflict,false,false,&clean,true,true,false,false,true);
    const auto &both=a.readDependency.call.targetIdentity.mov;
    assert(both.samples==2 && !both.addressSame && !both.addressExact &&
        (both.aliasMayMask&ws::rdx::movHero) && (both.aliasMayMask&ws::rdx::movSource) && (both.aliasMayMask&ws::rdx::movUnknown));
    Image other;memoryAtFallthrough(other,{0x41,0x8b,0x00});auto tagged=clean;tagged.gpr[8]=4;
    // R8 as address can be described at the pinned site with disjoint branch state
    // only if its tag wasn't used in the condition; current c27 correctly refuses earlier.
    r=other.run(true,target,imageSize,&tagged);assert(!r.mov.decoded && !r.branch.expanded);
  }
  std::puts("PASS: c28 parent-state affine address provenance and incoming selected/postwrite tags stay separate from unknown loaded-value tags; raw loaded bits and other entry tokens remain unknown addresses");

  {
    Image indexed;memoryAtFallthrough(indexed,{0x8b,0x44,0x81,0xf0});auto a=flowThroughIdentity(indexed,false,false,&clean,true);
    const auto &m=a.readDependency.call.targetIdentity.mov;const auto &op=m.operands[1];
    assert(!m.addressExact && m.addressTags==1 && op.scale==4 && op.displacement==-16 &&
        !std::strcmp(op.base,"rcx") && !std::strcmp(op.index,"rax") && op.indexTags==1 && op.baseTags==0 && m.addressWidth==8);
    Image segment;memoryAtFallthrough(segment,{0x64,0x8b,0x01});a=flowThroughIdentity(segment,false,false,&clean,true);
    assert(!a.readDependency.call.targetIdentity.mov.addressExact && !std::strcmp(a.readDependency.call.targetIdentity.mov.operands[1].segment,"fs"));
    Image narrow;memoryAtFallthrough(narrow,{0x67,0x8b,0x01});a=flowThroughIdentity(narrow,false,false,&clean,true);
    assert(a.readDependency.call.targetIdentity.mov.addressWidth==4 && !a.readDependency.call.targetIdentity.mov.addressExact);
  }
  std::puts("PASS: c28 exact base/index/scale/signed displacement/segment/address width is reported without guessing indexed, segmented or32-bit effective pointer provenance");

  {
    Image field;memoryAtFallthrough(field,{0x8b,0x81,0x0c,1,0,0});auto a=flowThroughIdentity(field,false,false,&clean,true);
    assert(a.readDependency.call.targetIdentity.mov.aliasMayMask==(ws::rdx::movHero|ws::rdx::movField10c));
    Image source;memoryAtFallthrough(source,{0x8b,0x06});a=flowThroughIdentity(source,false,false,&clean,true);
    assert(a.readDependency.call.targetIdentity.mov.aliasMayMask&ws::rdx::movSource);
    Image home;memoryAtFallthrough(home,{0x8b,0x44,0x24,8});a=flowThroughIdentity(home,false,false,&clean,true);
    const auto &h=a.readDependency.call.targetIdentity.mov;assert(h.addressExact && h.addressRoot==4 &&
        (h.aliasMayMask&ws::rdx::movCallerHome) && h.addressOffset==-0x30);
    Image below;memoryAtFallthrough(below,{0x8b,0x44,0x24,0xf8});a=flowThroughIdentity(below,false,false,&clean,true);
    assert((a.readDependency.call.targetIdentity.mov.aliasMayMask&(ws::rdx::movPrivateStack|ws::rdx::movUnknown))==
        (ws::rdx::movPrivateStack|ws::rdx::movUnknown));
    assert(!a.readDependency.call.modeledStates);
  }
  std::puts("PASS: c28 Hero/field10C/source/caller-home/below-SP candidate aliases are refused; nested CALL delta is reflected without inventing private-stack allocation or reading stack contents");

  {
    for (bool rmw : {false,true}) {
      Image store;memoryAtFallthrough(store,rmw?std::initializer_list<unsigned char>{0x87,0x01}:
          std::initializer_list<unsigned char>{0x89,0x01});
      auto a=flowThroughIdentity(store,false,false,&clean,true);const auto &m=a.readDependency.call.targetIdentity.mov;
      assert(m.memoryWrite && m.memoryRmw==rmw && !m.pureLoad && !a.readDependency.call.modeledStates &&
          !std::strcmp(m.reason,"target_MOV_memory_write_or_RMW_refused"));
      assert(m.operands[1].registerTags==1);
    }
    Image immediate;memoryAtFallthrough(immediate,{0xc7,0x01,1,0,0,0});r=immediate.run(true,target,imageSize,&clean);
    assert(r.mov.memoryWrite && !std::strcmp(r.mov.operands[1].kind,"immediate") && r.mov.operands[1].immediate==1);
  }
  std::puts("PASS: c28 memory store and RMW decoded directions are distinct and always refuse, including instruction-immediate stores; no GPR/Transform/Position disjointness can grant a memory write");

  {
    for (unsigned width : {1u,2u,4u,8u}) {
      Image f;
      if (width==1) memoryAtFallthrough(f,{0x8a,1});
      if (width==2) memoryAtFallthrough(f,{0x66,0x8b,1});
      if (width==4) memoryAtFallthrough(f,{0x8b,1});
      if (width==8) memoryAtFallthrough(f,{0x48,0x8b,1});
      auto a=flowThroughIdentity(f,false,false,&clean,true);const auto &m=a.readDependency.call.targetIdentity.mov;
      assert(m.operands[0].width==width && m.operands[1].width==width && m.flagsPreserved &&
          (width<4?m.partialGprWrites==1:m.fullGprWrites==1) && !a.readDependency.call.modeledStates);
    }
    Image vector;memoryAtFallthrough(vector,{0x0f,0x10,0x01});auto a=flowThroughIdentity(vector,false,false,&clean,true);
    const auto &m=a.readDependency.call.targetIdentity.mov;assert(m.operands[0].parentXmm==0 &&
        !std::strcmp(m.operands[0].kind,"XMM") && m.xmmWrites==1 && m.flagsPreserved && !m.pureLoad);
  }
  std::puts("PASS: c28 partial/full GPR and XMM destination effects and preserved flags are decoded without adding zero-extension, upper-lane, partial-write or memory-load semantics");

  {
    Image rip;memoryAtFallthrough(rip,{0x8b,0x05,0,0,0,0});
    Image::put32(rip.code,11,target+48-(target+9+6));auto a=flowThroughIdentity(rip,false,false,&clean,true);
    const auto &m=a.readDependency.call.targetIdentity.mov;
    assert(m.addressExact && m.addressRoot==16 && m.addressOffset==target+48 && m.imageRangeKnown &&
        m.codeRangeKnown && m.aliasMayMask==ws::rdx::movImage && m.loadedMayTags==7 && !a.readDependency.call.modeledStates);
    Image dynamicImage;memoryAtFallthrough(dynamicImage,{0x8b,0x05,0,0,0,0});
    Image::put32(dynamicImage.code,11,0x2000-(target+9+6));a=flowThroughIdentity(dynamicImage,false,false,&clean,true);
    assert(a.readDependency.call.targetIdentity.mov.imageRangeKnown && !a.readDependency.call.targetIdentity.mov.codeRangeKnown);
  }
  std::puts("PASS: c28 RIP-relative image/code addresses do not prove immutable loaded-value provenance; even an in-snapshot code address remains a refused read without any extra value observation");

  {
    Image unstableMov;memoryAtFallthrough(unstableMov,{0x8b,1});unstableMov.unstable=true;r=unstableMov.run(true,target,imageSize,&clean);
    assert(!r.mov.decoded && !r.stable);
    Image untagged;memoryAtFallthrough(untagged,{0x8b,1});r=untagged.run();
    assert(!r.mov.decoded && !r.branch.expanded);
    Image pinned;fanout(pinned);pinned.code[16]=0x8b;pinned.code[17]=1;r=pinned.run(true,target,imageSize,&clean);
    assert(!r.admitted && r.mov.site==target+9 && !r.mov.memoryOperands && r.paths==1 && r.barrier==target+16);
    Image f;memoryAtFallthrough(f,{0x8b,1});r=f.run(true,target,imageSize,&clean);
    assert(!r.admitted && !r.cfgComplete && !r.size && !r.paths && r.branch.expanded &&
        r.barrier==target+9 && !r.finalCodeRechecked);
  }
  std::puts("PASS: c28 diagnostics require exact pinned reachable stable bytes; missing current tags, instability, another-site memory or one unsafe leg never grant target identity/hash/RET/effects");

  auto beginSource=[](Image &f) {return ws::rdx::beginReadImageLoad(true,ws::rdx::readImageSource,Image::sourceImageSize,Image::read,Image::query,&f);};
  auto finishSource=[](Image &f,ws::rdx::ReadImageLoad &proof) {return ws::rdx::finishReadImageLoad(proof,Image::sourceImageSize,Image::read,Image::query,&f);};
  auto exactLoad=[&](Image &f,bool bothKill=false) {
    f.enableSource();memoryAtFallthrough(f,{0x48,0x8b,0x81,0,1,0,0});
    // The other JA outcome remains mandatory and has an independently owned RET.
    if (bothKill) {f.code[40]=0x31;f.code[41]=0xc0;f.code[42]=0xc3;}
  };
  ws::rdx::ReadEntryTags evidence=clean;evidence.imageLoadAddressVerified=true;evidence.imageLoadSource=ws::rdx::readImageSource;
  {
    Image f;f.enableSource();auto proof=beginSource(f);
    assert(proof.policyPassed && proof.firstRead && !proof.admitted && proof.sectionUnique &&
        proof.sectionReadable && !proof.sectionWritable && proof.pageQueried && f.sourceReads==1);
    assert(finishSource(f,proof) && proof.admitted && proof.stable && proof.metadataSame && proof.pageSame &&
        proof.valueReadAttempts==2 && f.sourceReads==2 && f.queries==2);
    assert(!finishSource(f,proof) && !proof.admitted && f.sourceReads==2);
  }
  std::puts("PASS: c29 exact qword source needs non-writable PE and committed same-image readonly page plus two equal guarded8byte reads; no early certificate or unbounded recheck");

  {
    for (unsigned flags : {0xc0000040u,0x80000040u,0x20000020u}) {
      Image f;f.enableSource();Image::put32(f.headers,0x188+80+36,flags);auto proof=beginSource(f);
      assert(!proof.policyPassed && !proof.firstRead && !proof.admitted && !f.sourceReads);
    }
    for (unsigned protection : {0x04u,0x08u,0x40u,0x80u,0x01u,0x102u,0x220u,0x10u}) {
      Image f;f.enableSource();f.page.protect=protection;auto proof=beginSource(f);
      assert(!proof.policyPassed && !proof.firstRead && !f.sourceReads);
    }
    Image rx;rx.enableSource();rx.page.protect=0x20;Image::put32(rx.headers,0x188+80+36,0x60000020);
    auto proof=beginSource(rx);assert(proof.firstRead && finishSource(rx,proof));
  }
  std::puts("PASS: c29 writable/unreadable PE and current writable/COW/guard/noaccess/unsupported pages refuse before value reads; readonly executable source is separately admitted");

  {
    for (unsigned defect=0;defect<6;++defect) {
      Image f;f.enableSource();
      if (defect==0) f.page.sameAllocation=false;
      if (defect==1) f.page.type=0x20000;
      if (defect==2) f.page.state=0x2000;
      if (defect==3) f.page.bytes=ws::rdx::readImageSource-f.page.begin+4;
      if (defect==4) f.page.bytes=UINT64_MAX;
      if (defect==5) f.queryFail=true;
      auto proof=beginSource(f);assert(!proof.policyPassed && !f.sourceReads);
    }
    Image cross;cross.enableSource();Image::put32(cross.headers,0x188+80+8,ws::rdx::readImageSource-cross.page.begin+4);
    auto proof=beginSource(cross);assert(!proof.sectionUnique && !proof.firstRead && !cross.sourceReads);
    Image outside;outside.enableSource();proof=ws::rdx::beginReadImageLoad(true,ws::rdx::readImageSource,ws::rdx::readImageSource+4,Image::read,Image::query,&outside);
    assert(!proof.firstRead && !outside.sourceReads);
    proof=ws::rdx::beginReadImageLoad(true,UINT32_MAX-3,Image::sourceImageSize,Image::read,Image::query,&outside);
    assert(!proof.firstRead && !outside.sourceReads);
  }
  std::puts("PASS: c29 wrong allocation/type/state, cross mapped/PE ranges, query failure and overflow/out-of-image pins refuse without source reads");

  {
    for (unsigned directory : {1u,9u,10u,11u,12u,13u}) {
      Image f;f.enableSource();Image::put32(f.headers,0x98+112+directory*8,ws::rdx::readImageSource-4);
      Image::put32(f.headers,0x98+116+directory*8,16);auto proof=beginSource(f);
      assert(proof.loaderRangesChecked && proof.loaderOverlap && proof.loaderDirectory==directory && !f.sourceReads);
    }
    Image malformed;malformed.enableSource();Image::put32(malformed.headers,0x98+112+12*8,UINT32_MAX-3);
    Image::put32(malformed.headers,0x98+116+12*8,16);assert(!beginSource(malformed).firstRead && !malformed.sourceReads);
  }
  std::puts("PASS: c29 direct authenticated import/TLS/loadconfig/bound/IAT/delay directory overlap and malformed ranges refuse without descriptor chasing or memory-value disclosure");

  {
    for (unsigned defect=0;defect<5;++defect) {
      Image f;f.enableSource();f.sourceFirstFail=defect==0;f.sourceFinalFail=defect==1;f.sourceChange=defect==2;
      f.pageChange=defect==3;f.metadataChange=defect==4;auto proof=beginSource(f);
      assert(!finishSource(f,proof) && !proof.admitted && !proof.stable && f.sourceReads<=2);
    }
  }
  std::puts("PASS: c29 initial/final guarded read failure, qword mismatch, PE metadata change and readonly-protection change never produce tag0 or a stale value certificate");

  {
    Image f;exactLoad(f);auto pre=flowThroughIdentity(f,false,false,nullptr,true,true,false,false,false,true,Image::sourceImageSize);
    const auto actual=ws::rdx::currentReadEntryEvidence(pre.readDependency.call);
    assert(actual.samples && actual.imageLoadAddressVerified && actual.imageLoadSource==ws::rdx::readImageSource);
    auto a=flowThroughIdentity(f,true,true,&actual,true,true,false,false,false,true,Image::sourceImageSize);
    const auto &call=a.readDependency.call;
    assert(call.targetIdentity.load.admitted && call.targetIdentity.load.stable && call.returnPaths==2 &&
        call.pathEffectsDiffer && !(call.outcomes[0].returnedGpr[0]&1) && (call.outcomes[1].returnedGpr[0]&1));
    assert(call.targetIdentity.mov.loadedMayTags==0 && call.targetIdentity.mov.flagsPreserved && f.sourceReads==2);
    Image killed;exactLoad(killed,true);a=flowThroughIdentity(killed,true,false,&actual,true,true,false,false,false,true,Image::sourceImageSize);
    assert(!(a.readDependency.call.returnedGpr[0]&1) && a.readDependency.call.targetIdentity.load.admitted);
    for (unsigned tag : {1u,2u,4u}) {
      auto bad=actual;bad.gpr[1]=tag;r=f.run(true,target,Image::sourceImageSize,&bad);
      assert(!r.admitted && !r.load.attempted && !f.sourceReads);
    }
  }
  std::puts("PASS: c29 current same-origin image address and tag0 source proof alone permit exact full64 RAX replacement; both JA outcomes and path-dependent preserved/killed selected tags remain explicit");

  {
    Image f;exactLoad(f);auto a=flowThroughIdentity(f,true,true,&evidence,true,true,false,false,false,true,Image::sourceImageSize);
    assert((a.readDependency.call.returnedGpr[1]&(1u<<2)) && !a.readDependency.call.returnedFlagsTags);
    // An address/evidence claim cannot be reused for a current caller with Hero RCX.
    a=flowThroughIdentity(f,true,false,&evidence,true,false,false,false,false,false,Image::sourceImageSize);
    assert(!a.readDependency.call.modeledStates && !a.readDependency.flowComplete);
    Image width;exactLoad(width);width.code[9]=0x40;r=width.run(true,target,Image::sourceImageSize,&evidence);
    assert(!r.admitted && !r.load.attempted && !width.sourceReads);
    Image noEvidence;exactLoad(noEvidence);r=noEvidence.run(true,target,Image::sourceImageSize,&clean);
    assert(!r.admitted && !r.load.attempted && !noEvidence.sourceReads);
  }
  std::puts("PASS: c29 flags/other GPR tags remain unchanged and full64 load permission cannot generalize to another width, unknown image address, or mismatched actual caller provenance");

  {
    Image next;exactLoad(next);next.code[16]=0x8b;next.code[17]=1;next.code[18]=0xc3;
    r=next.run(true,target,Image::sourceImageSize,&evidence);
    assert(!r.admitted && r.load.admitted && r.mov.loadedMayTags==0 && r.barrier==target+16 && !r.size);
    Image unsafe;exactLoad(unsafe);unsafe.code[40]=0x50;unsafe.code[41]=0xc3;r=unsafe.run(true,target,Image::sourceImageSize,&evidence);
    assert(!r.admitted && r.load.admitted && r.paths==1 && !r.cfgComplete && r.barrier==target+40);
    Image codeChanging;exactLoad(codeChanging);codeChanging.changeFinal=true;r=codeChanging.run(true,target,Image::sourceImageSize,&evidence);
    assert(!r.admitted && !r.load.admitted && r.barrier==target+9 && r.mov.loadedMayTags==7 && !r.size);
    Image changing;exactLoad(changing);changing.sourceChange=true;r=changing.run(true,target,Image::sourceImageSize,&evidence);
    assert(!r.admitted && !r.load.admitted && r.barrier==target+9 && r.mov.loadedMayTags==7 && !r.size);
  }
  std::puts("PASS: c29 admitted exact load advances only to the next precise barrier; one unsafe leg or final source instability refuses all target identity/effects without widening c27 limits");

  // C30 authored writable-source metadata; the private qword read callback is
  // deliberately set to fail so any accidental value observation is detectable.
  auto writableUnknown=[&](Image &f) {
    exactLoad(f);f.unknownMode=true;f.sourceFirstFail=f.sourceFinalFail=true;
    constexpr unsigned sec=0x188+80;
    std::fill(f.headers.begin()+sec,f.headers.begin()+sec+8,0);
    std::memcpy(f.headers.data()+sec,".data",5);
    Image::put32(f.headers,sec+8,0xc565a28-0x9b24000);
    Image::put32(f.headers,sec+12,0x9b24000);Image::put32(f.headers,sec+36,0xc0000040);
    f.page={true,true,0xc2d2000,0x1000,0x1000000,0x04,0x04,2703360};
  };
  {
    Image f;writableUnknown(f);r=f.run(true,target,Image::sourceImageSize,&evidence);
    assert(r.admitted && r.cfgComplete && r.paths==2 && r.finalCodeRechecked && r.unknownLoad.admitted &&
        r.unknownLoad.metadataPassed && r.unknownLoad.metadataRechecked && r.unknownLoad.metadataSame &&
        r.unknownLoad.pageSame && r.unknownLoad.targetCodeRechecked && r.mov.loadedMayTags==7 && !f.sourceReads && f.queries==2);
    assert(!r.load.admitted && !r.load.policyPassed && !r.load.valueReadAttempts &&
        !std::strcmp(r.load.reason,"image_load_section_not_readonly"));
    // Explicit old readonly policy still refuses the same writable source.
    f.unknownMode=false;r=f.run(true,target,Image::sourceImageSize,&evidence);
    assert(!r.admitted && !r.load.admitted && !f.sourceReads && r.barrier==target+9);
  }
  std::puts("PASS: c30 exact pinned writable image qword admits only unknown-top effect after current metadata/code recheck with zero source-value reads; c29 tag0 refusal remains unchanged");

  {
    for (unsigned incoming=0;incoming<=7;++incoming) {
      Image f;writableUnknown(f);auto tags=evidence;tags.gpr.fill(0);tags.xmm.fill(0);tags.gpr[0]=incoming;
      // Move the freshly loaded top to another register, clear RAX, then stop.
      const unsigned char tail[]={0x48,0x89,0xc2,0x31,0xc0,0x8b,0x01};
      std::copy(std::begin(tail),std::end(tail),f.code.begin()+16);
      r=f.run(true,target,Image::sourceImageSize,&tags);
      assert(!r.admitted && r.unknownLoad.admitted && r.barrier==target+21 && r.boundary.tagsKnown &&
          r.boundary.gpr[0]==0 && r.boundary.gpr[2]==7 && r.mov.loadedMayTags==7 && !f.sourceReads);
    }
    // Top7 is intrinsic, not entry-carrier mask bits0/1/2: all entry tags0 still yields top7.
    for (unsigned subset=0;subset<=7;++subset) assert((subset&ws::rdx::readMayTagTop)==subset);
  }
  std::puts("PASS: c30 every possible three-category dependency is contained in top7; incoming RAX0..7 is structurally replaced, top survives copies even with all entry tags0 and only an admitted later zero clears it");

  {
    Image f;writableUnknown(f);auto tags=evidence;tags.gpr[3]=2;tags.xmm[5]=4;
    f.code[16]=0x8b;f.code[17]=1;r=f.run(true,target,Image::sourceImageSize,&tags);
    assert(!r.admitted && r.boundary.tagsKnown && r.boundary.gpr[0]==7 && r.boundary.gpr[3]==2 &&
        r.boundary.xmm[5]==4 && !r.boundary.gpr[4] && !r.boundary.flagsTags && !f.sourceReads);
    Image vector;writableUnknown(vector);const unsigned char tail[]={0x66,0x0f,0x6e,0xc0,0x48,0x85,0xc0,0x8b,1};
    std::copy(std::begin(tail),std::end(tail),vector.code.begin()+16);
    r=vector.run(true,target,Image::sourceImageSize,&tags);
    assert(r.barrier==target+23 && r.boundary.xmm[0]==7 && r.boundary.flagsTags==7 && r.boundary.xmm[5]==4);
  }
  std::puts("PASS: c30 exact load preserves flags/other GPR/XMM/SP and no memory is touched; existing register moves and TEST conservatively propagate new intrinsic top7 into XMM and flags");

  {
    const std::vector<std::vector<unsigned char>> wrong={
      {0x48,0x8b,0x81,8,1,0,0}, {0x48,0x8b,0x99,0,1,0,0}, {0x8b,0x81,0,1,0,0},
      {0x48,0x89,0x81,0,1,0,0}, {0x48,0x87,0x81,0,1,0,0}, {0x8a,0x81,0,1,0,0},
      {0xf3,0x0f,0x7e,0x81,0,1,0,0}, {0x48,0x8b,0x84,0x01,0,1,0,0},
      {0x64,0x48,0x8b,0x81,0,1,0,0}, {0x67,0x48,0x8b,0x81,0,1,0,0}};
    for (const auto &bytes:wrong) {
      Image f;writableUnknown(f);std::copy(bytes.begin(),bytes.end(),f.code.begin()+9);
      r=f.run(true,target,Image::sourceImageSize,&evidence);
      assert(!r.admitted && !r.unknownLoad.attempted && !f.sourceReads && r.barrier==target+9);
    }
    Image chase;writableUnknown(chase);chase.code[16]=0x48;chase.code[17]=0x8b;chase.code[18]=0;
    r=chase.run(true,target,Image::sourceImageSize,&evidence);
    assert(!r.admitted && r.unknownLoad.admitted && r.barrier==target+16 && r.boundary.gpr[0]==7 && !chase.sourceReads);
  }
  std::puts("PASS: c30 wrong source/form/destination/width, store/RMW/partial/XMM/index/segment/address32 loads cannot borrow the rule; a second dereference of unknown RAX stops without chasing or value reads");

  {
    for (unsigned defect=0;defect<6;++defect) {
      Image f;writableUnknown(f);auto bad=evidence;
      if (defect==0) bad.imageLoadAddressVerified=false;
      if (defect==1) ++bad.imageLoadSource;
      if (defect>=2 && defect<=4) bad.gpr[1]=1u<<(defect-2);
      if (defect==5) bad.samples=0;
      r=f.run(true,target,Image::sourceImageSize,&bad);assert(!r.admitted && !r.unknownLoad.attempted && !f.sourceReads);
    }
    Image f;writableUnknown(f);
    auto pre=flowThroughIdentity(f,false,false,nullptr,true,true,false,false,false,true,Image::sourceImageSize);
    auto observed=ws::rdx::currentReadEntryEvidence(pre.readDependency.call);assert(observed.imageLoadAddressVerified);
    auto conflict=pre.readDependency.call;conflict.gpr[1].same=false;
    assert(!ws::rdx::currentReadEntryEvidence(conflict).imageLoadAddressVerified);
    auto a=flowThroughIdentity(f,true,false,&observed,true,false,false,false,false,false,Image::sourceImageSize);
    assert(!a.readDependency.call.modeledStates && !a.readDependency.flowComplete && !f.sourceReads);
  }
  std::puts("PASS: c30 unknown-top eligibility requires current exact same-origin image source, sampled tag0 address and nonconflicting parent proof; actual caller disagreement refuses even after identity passed");

  {
    for (unsigned defect=0;defect<9;++defect) {
      Image f;writableUnknown(f);
      if (defect==0) Image::put32(f.headers,0x188+80+36,0x40000040);
      if (defect==1) ++f.page.begin,f.page.bytes=1;
      if (defect==2) f.page.sameAllocation=false;
      if (defect==3) f.page.protect=0x104;
      if (defect==4) f.queryFail=true;
      if (defect==5) f.pageChange=true;
      if (defect==6) f.metadataChange=true; // mutate metadata on second query below.
      if (defect==7) f.changeFinal=true;
      if (defect==8) f.coverageChanges=true;
      r=f.run(true,target,Image::sourceImageSize,&evidence);
      assert(!r.admitted && !r.unknownLoad.admitted);
      assert(!f.sourceReads && r.mov.loadedMayTags==7);
    }
    Image noValues;writableUnknown(noValues);noValues.sourceChange=true;
    r=noValues.run(true,target,Image::sourceImageSize,&evidence);assert(r.admitted && !noValues.sourceReads);
    LargeImage large(527161);writableUnknown(large);
    r=ws::rdx::authenticateReadTarget(true,target,Image::sourceImageSize,LargeImage::readLarge,LargeImage::lookupLarge,&large,&evidence,Image::query,true);
    assert(r.admitted && r.unknownLoad.admitted && r.validatedEntries==527161 && r.metadataBytesRead==6325932 && !large.sourceReads);
  }
  std::puts("PASS: c30 current metadata/mapping/protection/code/runtime mismatch refuses; numerical qword content never matters and full527161-entry validation/current code gates remain mandatory");

  {
    Image next;writableUnknown(next);next.code[16]=0x8b;next.code[17]=1;
    r=next.run(true,target,Image::sourceImageSize,&evidence);
    assert(!r.admitted && !r.size && r.unknownLoad.admitted && r.barrier==target+16 &&
        !std::strcmp(r.mnemonic,"mov") && r.boundary.path==0 && r.boundary.edgeFrom==target+7 &&
        r.boundary.edgeTarget==target+9 && r.boundary.gpr[0]==7);
    for (unsigned kind=0;kind<5;++kind) {
      Image unsafe;writableUnknown(unsafe);
      if (kind==0) unsafe.code[40]=0x50;
      if (kind==1) {unsafe.code[40]=0xe8;Image::put32(unsafe.code,41,0);}
      if (kind==2) {unsafe.code[40]=0xff;unsafe.code[41]=0xe0;}
      if (kind==3) {unsafe.code[40]=0x75;unsafe.code[41]=0;}
      if (kind==4) {unsafe.code[40]=0xb0;unsafe.code[41]=0;}
      r=unsafe.run(true,target,Image::sourceImageSize,&evidence);
      assert(!r.admitted && !r.cfgComplete && !r.size && r.unknownLoad.admitted && r.paths==1 &&
          r.barrier==target+40 && r.boundary.path==1 && r.boundary.edgeTarget==target+40 &&
          r.boundary.gpr[0]==1 && !unsafe.sourceReads);
    }
  }
  std::puts("PASS: c30 next exact refusal retains path/JA edge and complete actual may-tag state before the unsupported instruction; one unsafe other leg still denies whole target span/hash/effects");

  {
    Image f;writableUnknown(f);f.code[40]=0x31;f.code[41]=0xc0;f.code[42]=0xc3;
    auto a=flowThroughIdentity(f,true,true,&evidence,true,true,false,false,false,true,Image::sourceImageSize);
    const auto &call=a.readDependency.call;assert(call.returnPaths==2 && call.pathEffectsDiffer && !f.sourceReads);
    for (unsigned bit=0;bit<3;++bit) assert((call.outcomes[0].returnedGpr[bit]&1) &&
        !(call.outcomes[1].returnedGpr[bit]&1) && (call.returnedGpr[bit]&1));
    Image reverse;writableUnknown(reverse);reverse.code[16]=0x31;reverse.code[17]=0xc0;reverse.code[18]=0xc3;
    a=flowThroughIdentity(reverse,true,true,&evidence,true,true,false,false,false,true,Image::sourceImageSize);
    assert(!(a.readDependency.call.outcomes[0].returnedGpr[0]&1) && (a.readDependency.call.outcomes[1].returnedGpr[0]&1) &&
        !(a.readDependency.call.returnedGpr[1]&1) && !(a.readDependency.call.returnedGpr[2]&1));
  }
  std::puts("PASS: c30 both normal RET effects remain explicit; conservative union retains all possible loaded categories and selected dependence, with narrowing only through already-admitted later register effects on every relevant path");

}
