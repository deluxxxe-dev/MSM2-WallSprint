// Owned, nonexecuted CPU fixtures. No captured game function bodies.
#include "NativeHooks/rdx_source_contract.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <initializer_list>
constexpr unsigned wr = 0x1000, cr = 0x2000;
struct Fixture {
  std::array<unsigned char,183> writer{};
  std::array<unsigned char,137> body{};
  ws::rdx::CallEffect call{};
  ws::rdx::CalleeAlias alias{};
  template <std::size_t N> static void put(std::array<unsigned char,N> &b,
      unsigned at, std::initializer_list<unsigned char> v) {
    assert(at + v.size() <= N); std::copy(v.begin(),v.end(),b.begin()+at);
  }
  Fixture() {
    writer.fill(0x90); body.fill(0x90);
    put(writer,0,{0x48,0x8b,0xf9,0x48,0x8b,0xf2,0x48,0x83,0xec,0x28});
    put(writer,0x1b,{0x75,1}); put(writer,0x1f,{0x75,2});
    put(writer,0x27,{0xe8,0,0,0,0});
    std::int32_t rel=cr-(wr+0x2c); std::memcpy(writer.data()+0x28,&rel,4);
    put(writer,0x2c,{0x8b,6,0x8b,0x5e,4,0x8b,0x56,8});
    put(writer,0x36,{0x89,0x47,0x30}); put(writer,0x3c,{0x89,0x5f,0x34});
    put(writer,0x42,{0x89,0x57,0x38}); put(writer,0x45,{0xc7,0x47,0x3c,0,0,0x80,0x3f});
    put(writer,0x4c,{0x48,0x83,0xc4,0x28,0xc3});
    put(body,0,{0x48,0x89,0x7c,0x24,8});
    put(body,0x20,{0xbf,1,0,0,0});
    put(body,0x2e,{0x89,0xb9,0x0c,1,0,0,0x48,0x8b,0x7c,0x24,8,0xc3});
  }
  ws::rdx::Writer run(unsigned read=5,bool auth=true,bool enabled=true,bool edi=true,unsigned test=0) {
    call=ws::rdx::exactCall(writer.data(),writer.size(),wr,0x5000,0x27,0x2c);
    ws::rdx::classifyCallee(call,body.data(),body.size(),cr,cr+body.size());
    alias={}; alias.code=body.data(); alias.size=body.size(); alias.imageSize=0x5000;
    alias.authenticated=auth; alias.exactWrite.site=cr+0x2e; alias.edi.enabled=edi;
    alias.readDependency.site=enabled?cr+read:0;
    alias.readDependency.test.site=test?cr+test:0;
    return ws::rdx::branchWriter(writer.data(),writer.size(),{0x36,0x3c,0x42,0x45},&call,&alias);
  }
  void denied() const {
    assert(!alias.resolved && !alias.relevantEffectsResolved && !alias.sourceNoWrite && !call.noWrite);
  }
};
int main() {
  Fixture dead;
  Fixture::put(dead.body,5,{0x8b,0x81,0x0c,1,0,0,0x31,0xc0});
  assert(!dead.run().xyz); dead.denied();
  auto r=dead.alias.readDependency;
  assert(r.decoded && r.addressExact && r.root==1 && r.offset==0x10c && r.width==4 && r.overlap==1);
  assert(r.orderComplete && r.orderMask==1 && r.flowComplete && !r.returnedGpr && !r.returnedFlags && !r.returnedCallerStack);
  assert(dead.alias.edi.complete && dead.alias.edi.exact);
  std::puts("PASS: c21 exact same-field read-before-write killed before returns remains a diagnostic, not a source/effect permission");

  Fixture survive;
  Fixture::put(survive.body,5,{0x8b,0x81,0x0c,1,0,0});
  // Owned downstream writer retains EAX and writes it to protected Position X.
  std::fill(survive.writer.begin()+0x2c,survive.writer.begin()+0x2e,0x90);
  assert(!survive.run().xyz); survive.denied(); r=survive.alias.readDependency;
  assert(r.flowComplete && (r.returnedGpr&1) && r.orderMask==1);
  Fixture home;
  Fixture::put(home.body,5,{0x8b,0x81,0x0c,1,0,0,0x89,0x44,0x24,0x10,0x31,0xc0});
  assert(!home.run().xyz && home.alias.readDependency.returnedCallerStack);
  Fixture partialHome;
  Fixture::put(partialHome.body,5,{0x48,0x8b,0x81,0x0c,1,0,0,
      0x48,0x89,0x44,0x24,0x10,0xc7,0x44,0x24,0x14,0,0,0,0,0x31,0xc0});
  assert(!partialHome.run().xyz && partialHome.alias.readDependency.returnedCallerStack);
  std::puts("PASS: c21 pre-write value surviving into a protected writer carrier or caller stack is reported without claiming downstream causality");

  Fixture after;
  Fixture::put(after.body,0x34,{0x8b,0x81,0x0c,1,0,0,0x48,0x8b,0x7c,0x24,8,0xc3});
  assert(!after.run(0x34).xyz); after.denied(); r=after.alias.readDependency;
  assert(r.orderComplete && r.orderMask==2 && r.fieldOrderMask==2 && r.flowComplete && (r.returnedGpr&1) && (r.newValueGpr&1));
  Fixture reload;
  Fixture::put(reload.body,5,{0x8b,0x81,0x0c,1,0,0,0x31,0xc0});
  Fixture::put(reload.body,0x34,{0x8b,0x91,0x0c,1,0,0,0x48,0x8b,0x7c,0x24,8,0xc3});
  assert(!reload.run().xyz); r=reload.alias.readDependency;
  assert(r.flowComplete && r.fieldOrderMask==3 && !r.returnedGpr && (r.newValueGpr&4));
  std::puts("PASS: c21 read-after-write and later reload distinguish the selected old value from the newly persisted field value");

  Fixture branch;
  Fixture::put(branch.body,5,{0x75,8,0x8b,0x81,0x0c,1,0,0,0xeb,1,0x90});
  Fixture::put(branch.body,0x34,{0x8b,0x91,0x0c,1,0,0,0x48,0x8b,0x7c,0x24,8,0xc3});
  assert(!branch.run(7).xyz); r=branch.alias.readDependency;
  assert(r.orderComplete && r.orderMask==(1|8) && r.fieldOrderMask==3 && r.flowComplete);
  std::puts("PASS: c21 both branch legs expose branch-dependent read presence and mixed before/after same-field reads, never a guessed uniform order");

  for (unsigned kind=0;kind<3;++kind) {
    Fixture f;
    if (kind==0) Fixture::put(f.body,5,{0x8b,0x05,0xf5,0x0f,0,0});
    if (kind==1) Fixture::put(f.body,5,{0x8b,0x44,0x24,8});
    if (kind==2) Fixture::put(f.body,5,{0x8b,6});
    f.run(); r=f.alias.readDependency;
    assert(r.decoded && r.addressExact && r.overlap==0 && r.flowComplete);
    assert(r.root==(kind==0?16:kind==1?4:2));
  }
  std::puts("PASS: c21 exact image, stack and source reads retain conditional region provenance and disjoint spans");

  Fixture indexed;
  Fixture::put(indexed.body,5,{0x8b,0x84,0x91,0x0c,1,0,0});
  assert(!indexed.run().xyz); r=indexed.alias.readDependency;
  assert(r.decoded && !r.addressExact && r.overlap==-1 && !r.flowComplete);
  Fixture unknown;
  Fixture::put(unknown.body,5,{0x41,0x8b,1});
  assert(!unknown.run().xyz && !unknown.alias.readDependency.addressExact);
  Fixture partialReg;
  Fixture::put(partialReg.body,5,{0x8b,0x81,0x0c,1,0,0,0x84,0xc0});
  assert(!partialReg.run().xyz && !partialReg.alias.readDependency.flowComplete &&
         partialReg.alias.readDependency.barrier==cr+11);
  Fixture conflict;
  Fixture::put(conflict.body,5,{0x75,7,0x48,0x8d,0x91,0x0c,1,0,0,0x8b,2});
  assert(!conflict.run(14).xyz); r=conflict.alias.readDependency;
  assert(r.decoded && !r.sameAddress && !r.addressExact && !r.flowComplete);
  std::puts("PASS: c21 unknown/indexed/conflicting read addresses refuse exactness and never clear the existing Transform/Position gate");

  Fixture flags;
  Fixture::put(flags.body,5,{0x39,0xb9,0x0c,1,0,0,0x75,1,0x90});
  assert(!flags.run().xyz); r=flags.alias.readDependency;
  assert(r.flowComplete && r.controlUse && r.firstControl==cr+11 && r.returnedFlags);
  assert(!std::strcmp(r.carrierKind,"flags"));
  Fixture nested=flags;
  Fixture::put(nested.body,0x39,{0xe8,0,0,0,0,0xc3});
  assert(!nested.run().xyz); r=nested.alias.readDependency;
  assert(r.orderComplete && !r.flowComplete && r.barrier==cr+0x39);
  std::puts("PASS: c21 flag/control flow and surviving flags are distinguished from complete value flow; nested CALL is an exact no-expansion blocker");

  assert(!dead.run(5,false).xyz && !dead.alias.readDependency.decoded);
  assert(!dead.run(6).xyz && !dead.alias.readDependency.orderComplete);
  assert(!dead.run(5,true,false).xyz && !dead.alias.readDependency.decoded && dead.alias.edi.complete);
  Fixture loop;
  Fixture::put(loop.body,5,{0xeb,0xfe});
  assert(!loop.run().xyz && !loop.alias.readDependency.decoded);
  Fixture cap;
  for(unsigned at=5;at<21;at+=2) Fixture::put(cap.body,at,{0x75,0});
  Fixture::put(cap.body,21,{0x8b,0x81,0x0c,1,0,0});
  assert(!cap.run(21,true,true,false).xyz && !cap.alias.readDependency.orderComplete &&
         cap.alias.readDependency.orderVisits==4097);
  std::puts("PASS: c21 authenticated boundary/backedge/cap and disabled-diagnostic controls preserve c20 local EDI and c19 fail-closed behavior");

  Fixture testLive;
  Fixture::put(testLive.body,5,{0x8b,0x81,0x0c,1,0,0,0x85,0xc0,0x75,1,0x90});
  assert(!testLive.run(5,true,true,true,11).xyz); testLive.denied();
  r=testLive.alias.readDependency; auto t=r.test;
  assert(r.flowComplete && r.orderMask==1 && r.fieldOrderMask==1 && r.controlUse);
  assert(t.decoded && t.formSupported && t.flagsOnly && !t.lowByteRead &&
         t.samples==1 && t.modeledStates==1 && t.flagsTags==1);
  assert(!std::strcmp(t.operands[0].name,"eax") && t.operands[0].width==4 &&
         t.operands[0].parentTags==1 && t.operands[1].parentTags==1);
  assert(t.firstConsumer==cr+13 && t.firstDependentConsumer==cr+13 &&
         t.consumerTags==1 && t.returnedFlagsTags==1 && r.returnedFlags && (r.returnedGpr&1));
  assert(testLive.alias.edi.complete && testLive.alias.edi.exact);
  // A different register operand must propagate its own dependence too.
  Fixture rhs=testLive;
  Fixture::put(rhs.body,11,{0x85,0xc3}); // TEST EBX,EAX; EBX initially untainted.
  assert(!rhs.run(5,true,true,true,11).xyz);
  t=rhs.alias.readDependency.test;
  assert(t.operands[0].parentTags==0 && t.operands[1].parentTags==1 && t.flagsTags==1);
  std::puts("PASS: c22 TEST reg32,reg32 reads both operands without killing self-taint or registers; selected tags reach ZF branch and returned flags");

  Fixture killed;
  Fixture::put(killed.body,5,{0x8b,0x81,0x0c,1,0,0,0x31,0xc0,0x85,0xc0,0x75,1,0x90});
  assert(!killed.run(5,true,true,true,13).xyz); r=killed.alias.readDependency;
  assert(r.flowComplete && !r.controlUse && !r.returnedFlags &&
         !r.test.flagsTags && r.test.firstConsumer==cr+15 && !r.test.firstDependentConsumer);
  Fixture unrelated=testLive;
  Fixture::put(unrelated.body,11,{0x85,0xdb});
  assert(!unrelated.run(5,true,true,true,11).xyz); r=unrelated.alias.readDependency;
  assert(r.flowComplete && !r.controlUse && !r.test.flagsTags && !r.returnedFlags && (r.returnedGpr&1));
  Fixture overwritten=testLive;
  Fixture::put(overwritten.body,13,{0x31,0xd2,0x75,1,0x90});
  assert(!overwritten.run(5,true,true,true,11).xyz); r=overwritten.alias.readDependency;
  assert(r.flowComplete && r.test.flagsTags==1 && !r.test.firstConsumer &&
         !r.test.returnedFlagsTags && !r.controlUse && !r.returnedFlags);
  std::puts("PASS: c22 dead/killed/unrelated TEST operands and overwritten flags do not create selected-read branch dependence");

  Fixture byte=testLive;
  Fixture::put(byte.body,11,{0x84,0xc0});
  assert(!byte.run(5,true,true,true,11).xyz); r=byte.alias.readDependency;
  assert(r.flowComplete && r.test.lowByteRead && r.test.flagsTags==1 && r.controlUse &&
         r.test.operands[0].width==1 && r.test.operands[0].conservativeParent);
  // Opt-in is only for the named TEST: the old unmodeled-byte refusal remains.
  assert(!byte.run().xyz && !byte.alias.readDependency.flowComplete &&
         byte.alias.readDependency.barrier==cr+11);
  Fixture dil;
  Fixture::put(dil.body,5,{0x8b,0x81,0x0c,1,0,0,0x89,0xc7,0x40,0x84,0xff,0x75,1,0x90});
  assert(!dil.run(5,true,true,true,13).xyz); r=dil.alias.readDependency;
  assert(r.flowComplete && r.test.formSupported && r.test.operands[0].parent==7 &&
         !std::strcmp(r.test.operands[0].name,"dil") && r.test.flagsTags==1);
  Fixture wide;
  Fixture::put(wide.body,5,{0x48,0x8b,0x81,0x0c,1,0,0,0x48,0x85,0xc0,0x75,1,0x90});
  assert(!wide.run(5,true,true,true,12).xyz); r=wide.alias.readDependency;
  assert(r.flowComplete && r.width==8 && r.test.operands[0].width==8 &&
         !r.test.lowByteRead && r.test.flagsTags==1 && r.controlUse);
  std::puts("PASS: c22 pinned read-only low-byte TEST conservatively retains parent tags; full64 reads work; general partial-register model stays closed");

  for (unsigned branchId : {0x72u,0x73u,0x70u,0x71u}) {
    Fixture f=testLive;
    Fixture::put(f.body,13,{static_cast<unsigned char>(branchId),1});
    assert(!f.run(5,true,true,true,11).xyz); r=f.alias.readDependency;
    assert(r.flowComplete && r.test.flagsTags==1 && r.test.firstConsumer==cr+13 &&
           !r.test.consumerTags && !r.test.firstDependentConsumer && !r.controlUse);
  }
  for (unsigned branchId : {0x74u,0x75u,0x76u,0x77u,0x78u,0x79u,0x7au,0x7bu,0x7cu,0x7du,0x7eu,0x7fu}) {
    Fixture f=testLive;
    Fixture::put(f.body,13,{static_cast<unsigned char>(branchId),1});
    assert(!f.run(5,true,true,true,11).xyz); r=f.alias.readDependency;
    assert(r.flowComplete && r.test.consumerTags==1 && r.controlUse && r.paths==2 && r.orderPaths==2);
  }
  std::puts("PASS: c22 TEST clears CF/OF while ZF/SF/PF branches may depend; all sixteen regular Jcc retain both local CFG legs");

  Fixture newField;
  Fixture::put(newField.body,5,{0x8b,0x81,0x0c,1,0,0,0x31,0xc0});
  Fixture::put(newField.body,0x34,{0x8b,0x91,0x0c,1,0,0,0x85,0xd2,0x75,1,0x90,0x48,0x8b,0x7c,0x24,8,0xc3});
  assert(!newField.run(5,true,true,true,0x3a).xyz); r=newField.alias.readDependency;
  assert(r.flowComplete && r.fieldOrderMask==3 && r.test.flagsTags==2 &&
         r.test.consumerTags==2 && r.test.returnedFlagsTags==2 && !r.controlUse && !r.returnedFlags);
  Fixture otherField;
  Fixture::put(otherField.body,5,{0x8b,0x81,0x0c,1,0,0,0x31,0xc0,
      0x8b,0x91,0x0c,1,0,0,0x85,0xd2,0x75,1,0x90});
  assert(!otherField.run(5,true,true,true,19).xyz); r=otherField.alias.readDependency;
  assert(r.flowComplete && r.test.flagsTags==4 && r.test.consumerTags==4 && !r.controlUse);
  std::puts("PASS: c22 TEST keeps selected-read, post-write reload and other pre-write field tags distinct through flags and branch consumption");

  Fixture mixed;
  Fixture::put(mixed.body,5,{0x8b,0x81,0x0c,1,0,0,0x75,2,0x31,0xc0,0x85,0xc0,0x75,1,0x90});
  assert(!mixed.run(5,true,true,true,15).xyz); r=mixed.alias.readDependency;
  assert(r.flowComplete && r.orderMask==1 && r.paths==4 && r.test.samples==2 &&
         r.test.modeledStates==2 && r.test.flagsTags==1 && r.test.consumerTags==1);
  Fixture partialBefore;
  Fixture::put(partialBefore.body,5,{0x8b,0x81,0x0c,1,0,0});
  std::fill(partialBefore.body.begin()+0x34,partialBefore.body.end(),0x90);
  Fixture::put(partialBefore.body,0x34,{0xb0,0,0x84,0xc0});
  Fixture::put(partialBefore.body,0x50,{0x48,0x8b,0x7c,0x24,8,0xc3});
  assert(!partialBefore.run(5,true,true,true,0x36).xyz); r=partialBefore.alias.readDependency;
  assert(!r.flowComplete && r.barrier==cr+0x34 && r.test.decoded &&
         r.test.formSupported && !r.test.samples && !r.test.modeledStates);
  std::puts("PASS: c22 all reaching path tags are retained; static TEST decode is distinct from flow coverage after an earlier partial-write refusal");

  for (unsigned form=0;form<5;++form) {
    Fixture f=testLive;
    // After the write so the old EDI reaching-definition gate remains first
    // and unchanged, rather than bypassing it to exercise new diagnostics.
    std::fill(f.body.begin()+0x34,f.body.end(),0x90);
    Fixture::put(f.body,0x50,{0x48,0x8b,0x7c,0x24,8,0xc3});
    if(form==0) Fixture::put(f.body,0x34,{0x84,0xe4}); // high byte AH not modeled.
    if(form==1) Fixture::put(f.body,0x34,{0x66,0x85,0xc0}); // AX not modeled.
    if(form==2) Fixture::put(f.body,0x34,{0xf7,0xc0,1,0,0,0}); // immediate not modeled.
    if(form==3) Fixture::put(f.body,0x34,{0x85,0x04,0x91}); // indexed memory not modeled.
    if(form==4) Fixture::put(f.body,0x34,{0xb0,0}); // partial register write, not TEST.
    assert(!f.run(5,true,true,true,0x34).xyz); f.denied(); r=f.alias.readDependency;
    assert(r.test.decoded && !r.test.formSupported && !r.flowComplete && r.barrier==cr+0x34);
    if(form==0) assert(!std::strcmp(r.test.operands[0].name,"ah") && r.test.operands[0].width==1);
    if(form==1) assert(!std::strcmp(r.test.operands[0].name,"ax") && r.test.operands[0].width==2);
    if(form==3) assert(!std::strcmp(r.test.operands[0].kind,"memory") &&
                      !std::strcmp(r.test.operands[0].index,"rdx"));
  }
  std::puts("PASS: c22 high-byte/16bit/immediate/indexed-memory/non-TEST partial-write forms report exact operands and retain fail-closed refusal");

  Fixture flagUse=testLive;
  std::fill(flagUse.body.begin()+0x34,flagUse.body.end(),0x90);
  Fixture::put(flagUse.body,0x50,{0x48,0x8b,0x7c,0x24,8,0xc3});
  Fixture::put(flagUse.body,0x34,{0x85,0xc0,0x83,0xd2,0}); // ADC EDX,0 reads CF; unmodeled.
  assert(!flagUse.run(5,true,true,true,0x34).xyz); r=flagUse.alias.readDependency;
  assert(!r.flowComplete && r.barrier==cr+0x36 && r.test.firstConsumer==cr+0x36 &&
         !std::strcmp(r.reason,"test_flags_nonbranch_consumer_unmodeled"));
  Fixture callAfter=flagUse;
  Fixture::put(callAfter.body,0x36,{0xe8,0,0,0,0,0xc3});
  assert(!callAfter.run(5,true,true,true,0x34).xyz); r=callAfter.alias.readDependency;
  assert(r.orderComplete && !r.flowComplete && r.barrier==cr+0x36 && r.test.modeledStates==2);
  assert(!testLive.run(5,false,true,true,11).xyz && !testLive.alias.readDependency.test.decoded);
  assert(!testLive.run(5,true,false,true,11).xyz && !testLive.alias.readDependency.test.decoded);
  assert(!testLive.run(5,true,true,true,12).xyz && !testLive.alias.readDependency.test.decoded);
  std::puts("PASS: c22 unsupported flag use/CALL stop precisely; authenticated, disabled and instruction-boundary controls preserve c19/c20/c21 gates");
}
