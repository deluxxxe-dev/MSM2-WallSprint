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
  Fixture five{0x90};
  for (unsigned at : {0u, 3u, 6u, 9u, 12u})
    put(five.body, at, {0x75, 1});
  put(five.body, 16, {0x48, 0x89, 0x5c, 0x24, 8, 0xc3});
  auto q = five.run();
  assert(q.xyz && q.w && five.alias.cfgComplete && five.alias.resolved);
  assert(five.alias.localBranches == 5 && five.alias.conditionalBranches == 5 &&
         five.alias.branchBudget == 5 && five.alias.pathBudget == 32 &&
         five.alias.stateBudget == 63 && five.alias.paths == 128 &&
         five.alias.writes == 1 && five.alias.blocksVisited == 11 &&
         five.alias.edgesVisited == 15 && five.alias.blocks == 12 &&
         five.alias.edges == 15);
  puts("PASS: c18 exact five-Jcc decode derives branch5/path32/state63; both "
       "successors/reconvergence cover all128 writer/callee paths and one "
       "write");
  Fixture bounded{0x90};
  for (unsigned at = 0; at < 24; at += 3)
    put(bounded.body, at, {0x75, 1});
  put(bounded.body, 24, {0xc3});
  // A single writer path permits all256 callee paths inside total visit cap.
  put(bounded.writer, 0x1b, {0x90, 0x90});
  put(bounded.writer, 0x1f, {0x90, 0x90});
  q = bounded.run();
  assert(q.xyz && bounded.alias.branchBudget == 8 &&
         bounded.alias.pathBudget == 256 && bounded.alias.stateBudget == 511 &&
         bounded.alias.paths == 256);
  put(bounded.body, 24, {0x75, 1, 0x90, 0xc3});
  q = bounded.run();
  assert(!q.xyz && bounded.alias.localBranches == 9 &&
         bounded.alias.branchBudget == 9 && !bounded.alias.cfgComplete &&
         !std::strcmp(bounded.alias.barrierEffect, "branch_resource_ceiling"));
  Fixture visits{0x90};
  for (unsigned at = 0; at < 24; at += 3)
    put(visits.body, at, {0x75, 1});
  put(visits.body, 136, {0xc3});
  put(visits.writer, 0x1b, {0x90, 0x90});
  put(visits.writer, 0x1f, {0x90, 0x90});
  q = visits.run();
  assert(!q.xyz && !visits.alias.resolved && visits.alias.visits == 4097 &&
         !std::strcmp(visits.alias.barrierEffect, "visit_cap"));
  puts("PASS: c18 eight decoded branches fit fixed storage; ninth refuses "
       "after full count, without silently truncating CFG or guessing a path");
  Fixture conflict{0x48, 0x8b, 0xc4, 0x75, 3, 0x48, 0x8b,
                   0xc1, 0x48, 0x89, 0x58, 8, 0xc3};
  // First path reaches a Transform write: cannot be promoted by a safe sibling.
  q = conflict.run();
  assert(!q.xyz && conflict.alias.barrier == calleeRva + 8 &&
         !conflict.alias.resolved);
  Fixture offsets{0x48, 0x8b, 0xc4, 0x75, 4,    0x48, 0x83,
                  0xc0, 8,    0x48, 0x89, 0x58, 8,    0xc3};
  q = offsets.run();
  assert(!q.xyz && offsets.alias.counts[ws::rdx::unknownPointer] == 1 &&
         offsets.alias.writeSites[0].root == -1 &&
         !std::strcmp(offsets.alias.barrierEffect,
                      "conflicting_address_provenance"));
  Fixture roots{0x48, 0x8b, 0xc4, 0x75, 7,    0x48, 0x8d, 5,   0,
                0,    0,    0,    0x48, 0x89, 0x58, 8,    0xc3};
  std::int32_t rootDisp = 0x4000 - (calleeRva + 12);
  std::memcpy(roots.body.data() + 8, &rootDisp, 4);
  q = roots.run();
  assert(!q.xyz && roots.alias.barrier == calleeRva + 12 &&
         roots.alias.writeSites[0].kind == ws::rdx::unknownPointer &&
         roots.alias.writeSites[0].root == -1 &&
         !std::strcmp(roots.alias.barrierEffect,
                      "conflicting_address_provenance"));
  puts("PASS: c18 path-sensitive reconvergence never chooses a safe sibling; "
       "conflicting reaching write addresses join to unknown/refuse");
  Fixture transform{0xc7, 0x42, 0x30, 1, 0, 0, 0, 0xc3};
  put(transform.writer, 0x10, {0x48, 0x8b, 0xd1});
  q = transform.run();
  assert(!q.xyz && transform.alias.transformArgMask == 3 &&
         transform.alias.sourceArgMask == 0 &&
         transform.alias.counts[ws::rdx::transformExact] == 1);
  Fixture rcx{0xc7, 0x41, 0x34, 1, 0, 0, 0, 0xc3};
  put(rcx.writer, 0x10, {0x48, 0x8b, 0xd1});
  assert(!rcx.run().xyz && rcx.alias.counts[ws::rdx::transformExact] == 1);
  Fixture source{0xc7, 0x46, 0x0c, 1, 0, 0, 0, 0xc3};
  put(source.writer, 0x10, {0x48, 0x8b, 0xd1});
  q = source.run();
  assert(!q.xyz && source.alias.sourceArgMask == 0 &&
         source.alias.counts[ws::rdx::sourceExact] == 1);
  Fixture r9{0x41, 0x89, 1, 0xc3};
  assert(!r9.run().xyz && r9.alias.counts[ws::rdx::unknownPointer] == 1);
  puts("PASS: c18 Transform-derived RCX/RDX both refuse exact writes while "
       "entry-source RSI remains distinct; unknown R9 never becomes disjoint");
  Fixture late{0x75, 1, 0x90, 0x48, 0x89, 0x5c, 0x24, 8,
               0x89, 5, 0,    0,    0,    0,    0xc3};
  std::int32_t disp = 0x4000 - (calleeRva + 14);
  std::memcpy(late.body.data() + 10, &disp, 4);
  q = late.run();
  assert(q.xyz && q.w && late.alias.sourceNoWrite &&
         late.alias.transformNoWrite &&
         late.alias.counts[ws::rdx::callerArea] == 1 &&
         late.alias.counts[ws::rdx::imageGlobal] == 1);
  put(late.writer, 0x34, {0x0f, 0xc8});
  q = late.run();
  assert(!q.xyz && late.alias.resolved && q.barrier == 0x34);
  puts("PASS: c18 branch-safe stack/global writes resume all exact XYZ/W "
       "stores or stop at strictly later writer instruction; W independent");
  Fixture loop{0x90, 0x75, 0xfd, 0xc3};
  q = loop.run();
  assert(!q.xyz && loop.alias.backedges == 1 &&
         loop.alias.barrier == calleeRva + 1);
  Fixture indirect{0x90, 0xff, 0xe0};
  assert(!indirect.run().xyz && indirect.alias.indirectTransfers == 1 &&
         indirect.alias.barrier == calleeRva + 1);
  Fixture call{0x90, 0xe8, 0, 0, 0, 0, 0xc3};
  assert(!call.run().xyz && call.alias.nestedCalls == 1 &&
         call.alias.barrier == calleeRva + 1);
  Fixture mid{0x75, 1, 0x48, 0x89, 0xc0, 0xc3};
  assert(!mid.run().xyz && mid.alias.barrier == calleeRva);
  Fixture foreign{0xe9, 0, 0x10, 0, 0, 0xc3};
  assert(!foreign.run().xyz && foreign.alias.barrier == calleeRva);
  Fixture countJump{0xe3, 1, 0x90, 0xc3};
  q = countJump.run();
  assert(
      !q.xyz && countJump.alias.indirectTransfers == 0 &&
      !std::strcmp(countJump.alias.barrierEffect, "unsupported_branch_form"));
  assert(!five.run(false).xyz);
  puts("PASS: c18 exact backedge/indirect/nested-call/mid-instruction/foreign "
       "refusals and unauthenticated code never expand control or resume "
       "writer");
}
