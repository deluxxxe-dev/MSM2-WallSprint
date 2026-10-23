// Owned nonexecuted 137/183-byte CPU fixtures, never a private game body.
#include "NativeHooks/rdx_source_contract.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <initializer_list>
using WriterCode = std::array<unsigned char, 183>;
using CalleeCode = std::array<unsigned char, 137>;
constexpr unsigned writerRva = 0x1000, calleeRva = 0x2000;
constexpr std::array<unsigned, 4> sites{0x36, 0x3c, 0x42, 0x45};
template <std::size_t N>
static void put(std::array<unsigned char, N> &b, unsigned at,
                std::initializer_list<unsigned char> bytes) {
  assert(at + bytes.size() <= N);
  std::copy(bytes.begin(), bytes.end(), b.begin() + at);
}
struct Fixture {
  WriterCode writer{};
  CalleeCode body{};
  ws::rdx::CallEffect call{};
  ws::rdx::CalleeAlias alias{};
  Fixture(std::initializer_list<unsigned char> bytes) {
    writer.fill(0x90);
    body.fill(0x90);
    put(body, 0, bytes);
    put(writer, 0,
        {0x48, 0x8b, 0xf9, 0x48, 0x8b, 0xf2, 0x48, 0x83, 0xec, 0x28});
    put(writer, 0x1b, {0x75, 0x01});
    put(writer, 0x1f, {0x75, 0x02});
    put(writer, 0x27, {0xe8, 0, 0, 0, 0});
    std::int32_t rel = calleeRva - (writerRva + 0x2c);
    std::memcpy(writer.data() + 0x28, &rel, 4);
    put(writer, 0x2c, {0x8b, 0x06, 0x8b, 0x5e, 4, 0x8b, 0x56, 8});
    put(writer, 0x36, {0x89, 0x47, 0x30});
    put(writer, 0x3c, {0x89, 0x5f, 0x34});
    put(writer, 0x42, {0x89, 0x57, 0x38});
    put(writer, 0x45, {0xc7, 0x47, 0x3c, 0, 0, 0x80, 0x3f});
    put(writer, 0x4c, {0x48, 0x83, 0xc4, 0x28, 0xc3});
  }
  ws::rdx::Writer run(bool authenticated = true,
                      std::uintptr_t site = calleeRva + 0x2e) {
    call = ws::rdx::exactCall(writer.data(), writer.size(), writerRva, 0x5000,
                              0x27, 0x2c);
    ws::rdx::classifyCallee(call, body.data(), body.size(), calleeRva,
                            calleeRva + body.size());
    alias = {};
    alias.code = body.data();
    alias.size = body.size();
    alias.imageSize = 0x5000;
    alias.authenticated = authenticated;
    alias.exactWrite.site = site;
    return ws::rdx::branchWriter(writer.data(), writer.size(), sites, &call,
                                 &alias);
  }
};
static void exact(Fixture &f,
                  std::initializer_list<unsigned char> code = {
                      0xc7, 0x81, 0x0c, 1, 0, 0, 1, 0, 0, 0, 0xc3}) {
  put(f.body, 0x2e, code);
}
int main() {
  Fixture safe{0x90};
  exact(safe);
  auto q = safe.run();
  const auto &t = safe.alias.exactWrite;
  assert(q.xyz && q.w && safe.alias.resolved && safe.alias.sourceNoWrite &&
         safe.alias.positionProtected && safe.alias.relevantEffectsResolved &&
         !safe.alias.transformNoWrite && safe.alias.transformWriteSeen &&
         t.modeled && t.complete && t.destinationExact && t.root == 1 &&
         t.offset == 0x10c && !t.positionOverlap && !t.carrierOverlap &&
         !t.reloadDependency && t.valueExact && t.reachingStates == 4 &&
         !std::strcmp(t.base, "rcx") && !std::strcmp(t.index, "none") &&
         t.displacement == 0x10c && !std::strcmp(t.valueKind, "constant") &&
         t.valueBits == 1);
  puts("PASS: c19 exact opted-in Transform+0x10C dword MOV has exact "
       "destination/value "
       "on all4 reaching writer states; Position/provenance protected while "
       "literal Transform-no-write stays0");
  for (unsigned off : {0u, 0x28u, 0x30u, 0x34u, 0x38u, 0x3cu}) {
    Fixture overlap{0x90};
    exact(overlap);
    std::memcpy(overlap.body.data() + 0x30, &off, 4);
    q = overlap.run();
    assert(!q.xyz && !overlap.alias.resolved &&
           overlap.alias.exactWrite.carrierOverlap);
    assert(overlap.alias.exactWrite.positionOverlap == (off >= 0x30));
  }
  Fixture other{0x90};
  exact(other);
  other.body[0x30] = 0x10;
  assert(
      !other.run().xyz); // +0x110 is not a blanket outside-Position whitelist.
  assert(!safe.run(false).xyz);
  assert(!safe.run(true, 0).xyz);
  assert(!safe.run(true, calleeRva + 137).xyz);
  assert(!safe.run(true, calleeRva + 0x30).xyz);
  puts("PASS: c19 all XYZ/W rows and other protected identity bytes reject; "
       "neighboring Transform offset, unauthenticated instruction and absent "
       "opt-in never gain permission");
  Fixture roots{0x48, 0x8b, 0xc1, 0x75, 2,    0xeb, 8,   0x48,
                0x8d, 0x84, 0x24, 0xfc, 0xfe, 0xff, 0xff};
  exact(roots, {0xc7, 0x80, 0x0c, 1, 0, 0, 1, 0, 0, 0, 0xc3});
  q = roots.run();
  assert(!q.xyz && !roots.alias.exactWrite.destinationSame &&
         roots.alias.barrier == calleeRva + 0x2e &&
         !std::strcmp(roots.alias.barrierEffect,
                      "conflicting_address_provenance"));
  Fixture offsets{0x48, 0x8b, 0xc1, 0x75, 2, 0xeb, 4, 0x48, 0x83, 0xc0, 4};
  exact(offsets, {0xc7, 0x80, 0x0c, 1, 0, 0, 1, 0, 0, 0, 0xc3});
  assert(!offsets.run().xyz && !offsets.alias.exactWrite.destinationSame);
  Fixture values{0xb8, 1, 0, 0, 0, 0x75, 5, 0xb8, 2, 0, 0, 0};
  exact(values, {0x89, 0x81, 0x0c, 1, 0, 0, 0xc3});
  assert(
      !values.run().xyz && !values.alias.exactWrite.valueSame &&
      !std::strcmp(values.alias.barrierEffect, "value_provenance_unresolved"));
  puts("PASS: c19 reconvergent conflicting root, displacement or constant "
       "origin "
       "refuses the exact instruction instead of selecting a safe path");
  Fixture reg{0x66, 0x0f, 0x7e, 0xc0}; // MOVD EAX,XMM0, exact entry bits.
  exact(reg, {0x89, 0x81, 0x0c, 1, 0, 0, 0xc3});
  assert(reg.run().xyz &&
         !std::strcmp(reg.alias.exactWrite.valueKind, "register"));
  Fixture mem{0x8b, 0x06}; // entry-source[0] through preserved RSI.
  exact(mem, {0x89, 0x81, 0x0c, 1, 0, 0, 0xc3});
  assert(mem.run().xyz &&
         !std::strcmp(mem.alias.exactWrite.valueKind, "memory") &&
         mem.alias.exactWrite.valueRoot == 2);
  Fixture computed{0xf3, 0x0f, 0x58, 0xc1, 0x66, 0x0f, 0x7e, 0xc0};
  exact(computed, {0x89, 0x81, 0x0c, 1, 0, 0, 0xc3});
  assert(!computed.run().xyz &&
         !std::strcmp(computed.alias.exactWrite.valueKind, "computed"));
  Fixture unknown{0x90};
  exact(unknown, {0x44, 0x89, 0x89, 0x0c, 1, 0, 0, 0xc3});
  assert(!unknown.run().xyz &&
         !std::strcmp(unknown.alias.exactWrite.valueKind, "unknown"));
  puts("PASS: c19 constant/entry-register/memory value origins are exact; "
       "computed "
       "and unknown R9 values remain diagnostic blockers without invented "
       "semantics");
  Fixture reload{0x90};
  exact(reload);
  std::fill(reload.writer.begin() + 0x2c, reload.writer.begin() + 0x36, 0x90);
  put(reload.writer, 0x2c, {0x8b, 0x87, 0x0c, 1, 0, 0});
  q = reload.run();
  assert(!q.xyz && reload.alias.exactWrite.reloadDependency &&
         reload.alias.exactWrite.dependencySite == writerRva + 0x2c &&
         q.barrier == 0x2c);
  Fixture early{0x90};
  exact(early);
  put(early.writer, 0x10, {0x8b, 0x81, 0x0c, 1, 0, 0});
  q = early.run();
  assert(!q.xyz && early.alias.exactWrite.reloadDependency &&
         early.alias.exactWrite.dependencySite == writerRva + 0x10);
  Fixture ambiguous{0x90};
  exact(ambiguous);
  std::fill(ambiguous.writer.begin() + 0x2c, ambiguous.writer.begin() + 0x36,
            0x90);
  put(ambiguous.writer, 0x2c, {0x41, 0x8b, 0x01});
  assert(!ambiguous.run().xyz && ambiguous.alias.exactWrite.reloadDependency);
  puts("PASS: c19 earlier/later overlapping writer reload carriers and unknown "
       "pointer reads reject even though the modeled write misses Position "
       "rows");
  Fixture continuation{0x90};
  exact(continuation, {0xc7, 0x81, 0x0c, 1, 0, 0, 1, 0, 0, 0, 0x48, 0x89, 0x5c,
                       0x24, 8, 0xc3});
  put(continuation.writer, 0x10, {0x48, 0x8b, 0xd1, 0x41, 0xb8, 1, 0, 0, 0});
  assert(continuation.run().xyz && continuation.alias.transformArgMask == 3 &&
         continuation.alias.sourceArgMask == 0 &&
         continuation.alias.counts[ws::rdx::callerArea] == 1);
  put(continuation.writer, 0x34, {0x0f, 0xc8});
  q = continuation.run();
  assert(!q.xyz && continuation.alias.resolved && q.barrier == 0x34);
  Fixture nested{0x90};
  exact(nested,
        {0xc7, 0x81, 0x0c, 1, 0, 0, 1, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xc3});
  assert(!nested.run().xyz && nested.alias.barrier == calleeRva + 0x38);
  puts("PASS: c19 modeled irrelevant Transform write plus later home store "
       "resumes "
       "XYZ/W; strictly later writer BSWAP or callee CALL remains an exact "
       "blocker");
  Fixture read{0x90};
  exact(read, {0xc7, 0x81, 0x0c, 1, 0, 0, 1, 0, 0, 0, 0x8b, 0x81, 0x0c, 1, 0, 0,
               0xc3});
  assert(!read.run().xyz && read.alias.barrier == calleeRva + 0x38 &&
         read.alias.exactWrite.reloadDependency);
  Fixture stackRead{0x8b, 0x84, 0x24, 0, 0x20, 0, 0};
  exact(stackRead);
  assert(!stackRead.run().xyz && stackRead.alias.exactWrite.reloadDependency);
  Fixture cap{0x90};
  for (unsigned at = 0; at < 46; at += 2)
    put(cap.body, at, {0x8b, 0x06});
  exact(cap);
  for (unsigned at = 56; at < 134; at += 2)
    put(cap.body, at, {0x8b, 0x06});
  put(cap.body, 134, {0xc3});
  q = cap.run();
  assert(!q.xyz && cap.alias.reads == ws::rdx::aliasReadCap &&
         !cap.alias.resolved);
  Fixture pop{0x48, 0x81, 0xec, 0,    1, 0, 0, 0x58,
              0x48, 0x81, 0xc4, 0xf8, 0, 0, 0};
  exact(pop);
  q = pop.run();
  assert(q.xyz && pop.alias.stackLow == -304 && q.callStackLow <= -304);
  Fixture popOutside{0x90};
  exact(popOutside);
  std::fill(popOutside.writer.begin() + 0x2c, popOutside.writer.begin() + 0x36,
            0x90);
  put(popOutside.writer, 0x2c, {0x48, 0x81, 0xc4, 0, 0x20, 0, 0, 0x58});
  q = popOutside.run();
  assert(!q.xyz && popOutside.alias.exactWrite.reloadDependency &&
         q.barrier == 0x33);
  puts("PASS: c19 callee read-after-write and out-of-bounded-stack reads never "
       "reuse stale memory provenance or escape runtime TEB exclusion");
}
