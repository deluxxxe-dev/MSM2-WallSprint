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
  ws::rdx::Writer run(bool authenticated = true) {
    call = ws::rdx::exactCall(writer.data(), writer.size(), writerRva, 0x5000,
                              0x27, 0x2c);
    ws::rdx::classifyCallee(call, body.data(), body.size(), calleeRva,
                            calleeRva + body.size());
    alias = {};
    alias.code = body.data();
    alias.size = body.size();
    alias.imageSize = 0x5000;
    alias.authenticated = authenticated;
    return ws::rdx::branchWriter(writer.data(), writer.size(), sites, &call,
                                 &alias);
  }
};
int main() {
  Fixture priv{0x48, 0x83, 0xec, 0x20, 0x48, 0x89, 0x1c,
               0x24, 0x48, 0x83, 0xc4, 0x20, 0xc3};
  auto q = priv.run();
  assert(q.xyz && q.w && priv.alias.resolved && priv.alias.sourceNoWrite &&
         priv.alias.transformNoWrite);
  assert(priv.alias.counts[ws::rdx::privateStack] == 1 &&
         q.callStackLow == -80 && priv.alias.analyses == 4);
  Fixture home{0x48, 0x89, 0x5c, 0x24, 8, 0xc3};
  q = home.run();
  assert(q.xyz && q.w && home.alias.counts[ws::rdx::callerArea] == 1 &&
         home.alias.writeSites[0].offset == -40);
  Fixture ret{0x48, 0x89, 0x1c, 0x24, 0xc3};
  q = ret.run();
  assert(!q.xyz && ret.alias.counts[ws::rdx::callerArea] == 1 &&
         ret.alias.barrier == calleeRva);
  puts("PASS: c17 private-stack and allocated caller-home writes prove "
       "source/Transform disjoint under bounded TEB footprint; actual callee "
       "return write refuses");
  Fixture source{0xc7, 0x42, 4, 1, 0, 0, 0, 0xc3};
  q = source.run();
  assert(!q.xyz && source.alias.counts[ws::rdx::sourceExact] == 1 &&
         source.alias.barrier == calleeRva);
  for (unsigned off : {0u, 4u, 8u, 12u}) {
    source.body[2] = static_cast<unsigned char>(off);
    q = source.run();
    assert(!q.xyz && source.alias.counts[ws::rdx::sourceExact] == 1);
  }
  Fixture transform{0xc7, 0x41, 0x30, 1, 0, 0, 0, 0xc3};
  q = transform.run();
  assert(!q.xyz && transform.alias.counts[ws::rdx::transformExact] == 1);
  Fixture chain{0x48, 0x8b, 0xc2, 0x48, 0x8d, 0x40, 4,
                0xc7, 0,    1,    0,    0,    0,    0xc3};
  q = chain.run();
  assert(!q.xyz && chain.alias.barrier == calleeRva + 7 &&
         chain.alias.counts[ws::rdx::sourceExact] == 1);
  puts("PASS: c17 exact source X/Y/Z span and Hero Transform writes refuse "
       "with exact callee address/effect, including MOV/LEA aliases");
  Fixture args{0x48, 0x89, 0x1a, 0xc3};
  put(args.writer, 0x10, {0x48, 0x8d, 0x54, 0x24, 0x10});
  q = args.run();
  assert(q.xyz && args.alias.argumentOffsets[1] == -24 &&
         !std::strcmp(args.alias.argumentOrigins[1],
                      "writer_entry_RSP_relative") &&
         !(args.alias.sourceArgMask & 2));
  assert(home.alias.sourceArgMask & 2);
  assert(home.alias.transformArgMask & 1);
  puts("PASS: c17 writer pre-call argument map distinguishes repurposed "
       "call-time RDX from entry-writer RDX source and resumes XYZ/W through "
       "preserved aliases");
  Fixture global{0x89, 0x05, 0, 0, 0, 0, 0xc3};
  std::int32_t disp = 0x4000 - (calleeRva + 6);
  std::memcpy(global.body.data() + 2, &disp, 4);
  q = global.run();
  assert(q.xyz && global.alias.usesGlobals &&
         global.alias.counts[ws::rdx::imageGlobal] == 1);
  Fixture globalArg{0x89, 1, 0xc3};
  put(globalArg.writer, 0x10, {0x48, 0x8d, 0x0d, 0, 0, 0, 0});
  disp = 0x4000 - (writerRva + 0x17);
  std::memcpy(globalArg.writer.data() + 0x13, &disp, 4);
  q = globalArg.run();
  assert(q.xyz && globalArg.alias.counts[ws::rdx::imageGlobal] == 1 &&
         globalArg.alias.argumentOffsets[0] == 0x4000);
  Fixture outside = global;
  disp = 0x6000 - (calleeRva + 6);
  std::memcpy(outside.body.data() + 2, &disp, 4);
  q = outside.run();
  assert(!q.xyz && !outside.alias.resolved);
  puts("PASS: c17 direct RIP-global and writer LEA-global argument provenance "
       "is exact/in-image and conditional on runtime region exclusion; foreign "
       "global refuses");
  Fixture branch{0x48, 0x8b, 0xc4, 0x75, 3, 0x48, 0x8b,
                 0xc4, 0x48, 0x89, 0x58, 8, 0xc3};
  q = branch.run();
  assert(q.xyz && branch.alias.writes == 1 && branch.alias.paths == 8);
  Fixture conflict{0x48, 0x8b, 0xc4, 0x75, 4,    0x48, 0x83,
                   0xc0, 8,    0x48, 0x89, 0x58, 8,    0xc3};
  q = conflict.run();
  assert(!q.xyz && conflict.alias.barrier == calleeRva + 9 &&
         conflict.alias.counts[ws::rdx::unknownPointer] == 1 &&
         !std::strcmp(conflict.alias.barrierEffect,
                      "conflicting_address_provenance"));
  puts("PASS: c17 both local branch successors reconverge with identical exact "
       "write origin; conflicting reaching addresses refuse at the common "
       "store");
  Fixture unknown{0x41, 0x89, 0, 0xc3};
  q = unknown.run();
  assert(!q.xyz && unknown.alias.counts[ws::rdx::unknownPointer] == 1 &&
         unknown.alias.barrier == calleeRva);
  Fixture nested{0x90, 0xe8, 0, 0, 0, 0, 0xc3};
  q = nested.run();
  assert(!q.xyz && nested.alias.barrier == calleeRva + 1 &&
         !std::strcmp(nested.alias.mnemonic, "call"));
  Fixture indirect{0xff, 0xe0, 0xc3};
  q = indirect.run();
  assert(!q.xyz && indirect.alias.barrier == calleeRva);
  Fixture loop{0xeb, 0xfe, 0xc3};
  q = loop.run();
  assert(!q.xyz);
  Fixture addr32{0x48, 0x8b, 0xc4, 0x67, 0x89, 0x58, 8, 0xc3};
  q = addr32.run();
  assert(!q.xyz);
  Fixture maximal{0x90};
  for (unsigned at : {0u, 3u, 6u, 9u})
    put(maximal.body, at, {0x75, 1});
  put(maximal.body, 16, {0xc3});
  q = maximal.run();
  assert(q.xyz && maximal.alias.paths == 64 && maximal.alias.visits <= 4096);
  put(maximal.body, 12, {0x75, 1});
  q = maximal.run();
  assert(!q.xyz && maximal.alias.barrier == calleeRva + 12);
  assert(!home.run(false).xyz);
  puts("PASS: c17 unknown pointer, nested call, indirect/backedge and 32-bit "
       "address truncation stop at exact callee blocker; unauthenticated "
       "summary never resumes");
  Fixture spill{0x48, 0x89, 0x5c, 0x24, 8, 0xc3};
  put(spill.writer, 0xa, {0x48, 0x89, 0x34, 0x24});
  q = spill.run();
  assert(!q.xyz &&
         !std::strcmp(spill.alias.barrierEffect, "writer_saved_slot_alias"));
  Fixture later{0x48, 0x89, 0x5c, 0x24, 8, 0xc3};
  put(later.writer, 0x34, {0x0f, 0xc8});
  q = later.run();
  assert(!q.xyz && later.alias.resolved && q.barrier == 0x34);
  Fixture clobber{0x48, 0x31, 0xf6, 0xc3};
  q = clobber.run();
  assert(!q.xyz && clobber.alias.resolved &&
         (clobber.call.gprClobber & (1 << 6)) && q.barrier == sites[0]);
  Fixture writerReturn{0x48, 0x89, 0x18, 0xc3};
  put(writerReturn.writer, 0x10, {0x48, 0x8d, 0x44, 0x24, 0x28});
  q = writerReturn.run();
  assert(!q.xyz && writerReturn.alias.counts[ws::rdx::callerArea] == 1);
  puts("PASS: c17 caller saved-carrier overlap refuses; alias-safe call "
       "resumes unchanged all-path exact XYZ/W proof or reports strictly later "
       "writer blocker");
}
