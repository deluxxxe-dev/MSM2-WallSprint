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
    alias.edi.enabled = true;
    return ws::rdx::branchWriter(writer.data(), writer.size(), sites, &call,
                                 &alias);
  }
};
static void init(Fixture &f, std::initializer_list<unsigned char> prefix) {
  f.body.fill(0x90);
  put(f.body, 0, {0x48, 0x89, 0x7c, 0x24, 8}); // preserve RDI in owned home
  put(f.body, 5, prefix);
  put(f.body, 0x2e,
      {0x89, 0xb9, 0x0c, 1, 0, 0, 0x48, 0x8b, 0x7c, 0x24, 8, 0xc3});
}
int main() {
  Fixture copy{0x90};
  init(copy, {0x8b, 0xfa}); // EDI=low32(callee-entry RDX)
  auto q = copy.run();
  assert(q.xyz && q.w && copy.alias.edi.complete && copy.alias.edi.same &&
         copy.alias.edi.writerStatesComplete &&
         copy.alias.edi.origin.root == 2 &&
         copy.alias.edi.origin.kind == ws::rdx::BitKind::entryRegister &&
         copy.alias.edi.origin.producer == calleeRva + 5 &&
         copy.alias.edi.definitions == 1 &&
         copy.alias.edi.reachingStates == 4 && !copy.alias.transformNoWrite &&
         copy.alias.positionProtected && copy.alias.relevantEffectsResolved);
  Fixture chain{0x90};
  init(chain, {0x8b, 0xc2, 0x8b, 0xf8});
  assert(chain.run().xyz && chain.alias.edi.origin.root == 2 &&
         chain.alias.edi.origin.chainSize == 2 &&
         chain.alias.edi.origin.chain[0] == calleeRva + 5);
  Fixture r9{0x90};
  init(r9, {0x41, 0x8b, 0xf9});
  assert(r9.run().xyz && r9.alias.edi.origin.root == 9);
  puts("PASS: c20 local EDI copies preserve low32 callee-entry RDX/R9 bit "
       "identity "
       "through 32-bit chains without upgrading integer bits into pointer "
       "types");
  Fixture constant{0x90};
  init(constant, {0xbf, 1, 0, 0, 0});
  assert(constant.run().xyz &&
         constant.alias.edi.origin.kind == ws::rdx::BitKind::constant &&
         constant.alias.edi.origin.bits == 1);
  Fixture memory{0x90};
  init(memory, {0x8b, 0x3e});
  assert(memory.run().xyz &&
         memory.alias.edi.origin.kind == ws::rdx::BitKind::memoryLoad &&
         memory.alias.edi.origin.root == 2);
  Fixture spill{0x90};
  init(spill,
       {0x41, 0x8b, 0xf9, 0x89, 0x7c, 0x24, 0x10, 0x8b, 0x7c, 0x24, 0x10});
  assert(spill.run().xyz && spill.alias.edi.origin.root == 9 &&
         spill.alias.edi.origin.chainSize == 2);
  Fixture computed{0x90};
  init(computed, {0x48, 0x8b, 0xfa, 0x48, 0x83, 0xc7, 4});
  assert(computed.run().xyz && computed.alias.edi.origin.root == 2 &&
         computed.alias.edi.origin.addend == 4);
  Fixture wrap{0x90};
  init(wrap, {0xbf, 0xff, 0xff, 0xff, 0xff, 0x48, 0x83, 0xc7, 1});
  assert(wrap.run().xyz && wrap.alias.edi.origin.bits == 0);
  Fixture sp{0x90};
  init(sp, {0x53, 0x8b, 0xfc, 0x5b});
  assert(sp.run().xyz && sp.alias.edi.origin.root == 4 &&
         sp.alias.edi.origin.addend == std::uint32_t(-8) &&
         sp.alias.edi.origin.chainSize == 2 &&
         sp.alias.edi.origin.chain[0] == calleeRva + 5);
  puts("PASS: c20 constant, memory, stack-spill reload and exact affine "
       "computed "
       "EDI origins are bounded; low32 addition wraps modulo2^32");
  Fixture equal{0x90};
  init(equal, {0x75, 7, 0xbf, 1, 0, 0, 0, 0xeb, 5, 0xbf, 1, 0, 0, 0});
  q = equal.run();
  assert(q.xyz && equal.alias.edi.definitions == 2 &&
         equal.alias.edi.reachingStates == 8 && equal.alias.edi.same);
  Fixture conflict = equal;
  conflict.body[15] = 2;
  assert(!conflict.run().xyz && !conflict.alias.edi.same &&
         !conflict.alias.edi.complete &&
         !conflict.alias.relevantEffectsResolved);
  Fixture roots{0x90};
  init(roots,
       {0x75, 7, 0x8b, 0xfa, 0xeb, 8, 0x90, 0x90, 0x90, 0x41, 0x8b, 0xf9});
  assert(!roots.run().xyz && !roots.alias.edi.same);
  puts(
      "PASS: c20 both local branch legs reach EDI; same constant via two local "
      "definitions joins exactly, differing constants/entry roots refuse");
  Fixture partial{0x90};
  init(partial, {0x40, 0xb7, 1});
  assert(!partial.run().xyz && partial.alias.edi.barrier == calleeRva + 5 &&
         !partial.alias.edi.complete);
  Fixture cap{0x90};
  init(cap, {0x90});
  for (unsigned i = 5; i < 23; i += 2)
    put(cap.body, i, {0x8b, 0xff});
  assert(!cap.run().xyz && !cap.alias.edi.exact &&
         cap.alias.edi.origin.chainSize == 8);
  Fixture tombstone{0x90};
  init(tombstone, {0x48, 0x89, 0x7c, 0x24, 0x10, 0xc7, 0x44, 0x24, 0x14, 1, 0,
                   0, 0, 0x8b, 0x7c, 0x24, 0x10});
  assert(!tombstone.run().xyz && !tombstone.alias.edi.exact);
  assert(!copy.run(false).xyz);
  puts(
      "PASS: c20 unsupported partial registers, copy-chain cap and partially "
      "overwritten spills refuse without invented input-memory or bit origins");
  Fixture later{0x90};
  init(later, {0x8b, 0xfa});
  put(later.body, 0x39, {0xe8, 0, 0, 0, 0, 0xc3});
  assert(!later.run().xyz && later.alias.edi.complete &&
         later.alias.edi.exact && !later.alias.edi.writerStatesComplete &&
         !later.alias.relevantEffectsResolved &&
         later.alias.barrier == calleeRva + 0x39);
  Fixture dependency{0x90};
  init(dependency, {0x8b, 0xfa});
  std::fill(dependency.writer.begin() + 0x2c, dependency.writer.begin() + 0x36,
            0x90);
  put(dependency.writer, 0x2c, {0x8b, 0x87, 0x0c, 1, 0, 0});
  q = dependency.run();
  assert(!q.xyz && dependency.alias.edi.complete &&
         dependency.alias.exactWrite.reloadDependency && q.barrier == 0x2c);
  puts("PASS: c20 exact all-local EDI definitions can precede a strictly later "
       "CALL/reload blocker; value coverage never substitutes for full "
       "protected effects");
  Fixture foreign{0x90};
  init(foreign, {0x90});
  put(foreign.body, 5, {0xe8, 0, 0, 0, 0});
  assert(!foreign.run().xyz && foreign.alias.edi.barrier == calleeRva + 5);
  Fixture loop{0x90};
  init(loop, {0xeb, 0xfe});
  assert(!loop.run().xyz && !loop.alias.edi.complete);
  puts("PASS: c20 prior call/backedge and unauthenticated code remain exact "
       "refusals; no traversal outside the authenticated local callee");
}
