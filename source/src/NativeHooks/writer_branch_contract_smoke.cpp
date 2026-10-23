// Independently authored, nonexecuted 183-byte decoder fixtures. No game body.
#include "NativeHooks/rdx_source_contract.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
using Code = std::array<unsigned char, 183>;
constexpr std::array<unsigned, 4> sites{0x36, 0x3c, 0x42, 0x45};
static void put(Code &c, unsigned at, std::initializer_list<unsigned char> b) {
  assert(at + b.size() <= c.size());
  std::copy(b.begin(), b.end(), c.begin() + at);
}
static Code fixture() {
  Code c{};
  c.fill(0x90);
  put(c, 0,
      {0x8b, 0x02, 0x8b, 0x5a, 0x04, 0x8b, 0x52,
       0x08});                // EAX/EBX/EDX <- entry RDX XYZ
  put(c, 0x1f, {0x75, 0x09}); // JNE -> 0x2a; both local successors reconverge
  put(c, 0x36, {0x89, 0x41, 0x30});
  put(c, 0x3c, {0x89, 0x59, 0x34});
  put(c, 0x42, {0x89, 0x51, 0x38});
  put(c, 0x45, {0xc7, 0x41, 0x3c, 0, 0, 0x80, 0x3f});
  c[0x4c] = 0xc3;
  return c;
}
static ws::rdx::Writer inspect(const Code &c) {
  return ws::rdx::branchWriter(c.data(), c.size(), sites);
}
int main() {
  auto code = fixture();
  auto p = inspect(code);
  if (!p.xyz)
    std::printf("debug reason=%s barrier=%x %s blocks=%u edges=%u visits=%u\n",
                p.reason, p.barrier, p.barrierMnemonic, p.blocks, p.edges,
                p.visits);
  assert(p.decoded && p.branchAware && p.bounded && p.exactSites && p.prefix &&
         p.allRelevant && p.xyz && p.w && p.wConstant &&
         p.wBits == 0x3f800000 && p.stores == sites);
  assert(p.firstBranch == 0x1f && p.branches == 1 && p.reachingStates[0] == 2 &&
         p.pathsExamined == 2 && p.componentProofMask == 7);
  assert(p.blocksVisited && p.edgesVisited && p.blocks <= ws::rdx::blockCap &&
         p.edges <= ws::rdx::edgeCap);
  assert(
      !ws::rdx::writer(code.data(), code.size()).xyz); // preserved c14 refusal
  puts("PASS: c15 exact owned 183-byte JNE at +0x1f explores both successors "
       "and authenticates identical entry RDX provenance at four exact store "
       "RVAs; W constant-one is separate and c14 analyzer unchanged");
  for (unsigned component = 0; component < 3; ++component) {
    auto c = code;
    put(c, 0x24,
        {0x31, static_cast<unsigned char>(component == 0   ? 0xc0
                                          : component == 1 ? 0xdb
                                                           : 0xd2)});
    auto q = inspect(c);
    assert(!q.xyz && !q.allRelevant && q.w &&
           q.componentProofMask == (7u & ~(1u << component)));
    assert(q.barrierValid && q.barrier == sites[component]);
    assert(!std::strcmp(q.paths[component], "conflicting_reaching_provenance"));
  }
  auto bypass = code;
  bypass[0x20] = 0x3c - 0x21;
  auto q = inspect(bypass);
  assert(q.xyz && q.allRelevant && q.reachingStates[0] == 1 &&
         q.reachingStates[1] == 2); // X only proved on paths reaching X
  auto unreachable = code;
  put(unreachable, 0x1f, {0xeb, 0x3c - 0x21});
  q = inspect(unreachable);
  assert(!q.xyz && q.componentProofMask == 6 &&
         !std::strcmp(q.paths[0], "no_entry_path_reaches_store"));
  puts("PASS: c15 one-leg X/Y/Z mutations reject only that component; bypass "
       "paths do not fabricate store evidence or poison paths that actually "
       "reach a store, and unreachable exact stores cannot be proven");
  auto nested = code;
  for (unsigned at : {0x10u, 0x14u, 0x18u, 0x1fu})
    put(nested, at, {0x75, static_cast<unsigned char>(0x30 - at - 2)});
  q = inspect(nested);
  assert(q.xyz && q.branches == 4 && q.pathsExamined == 5 &&
         q.statesScheduled <= 31);
  auto maximal = code;
  maximal[0x1f] = 0x90;
  maximal[0x20] = 0x90;
  for (unsigned at : {0x10u, 0x14u, 0x18u, 0x1cu})
    put(maximal, at, {0x75, 0x01});
  q = inspect(maximal);
  assert(q.xyz && q.branches == 4 && q.pathsExamined == 16 &&
         q.statesScheduled == 31 && q.visits <= 4096);
  auto over = nested;
  put(over, 0xa, {0x75, 0x02});
  q = inspect(over);
  assert(!q.xyz && q.barrierValid &&
         !std::strcmp(q.reason, "local_branch_cap_exceeded"));
  assert(!ws::rdx::branchWriter(code.data(), 184, sites).decoded);
  puts("PASS: c15 nested reconvergence and all16 paths/31 states remain within "
       "explicit block16/edge24/branch4/visit4096/byte183 caps; fifth branch "
       "and oversized writer refuse");
  for (unsigned char delta : {0xfe, 0xf0}) {
    auto c = code;
    c[0x20] = delta;
    q = inspect(c);
    assert(!q.xyz && q.barrier == 0x1f &&
           !std::strcmp(q.barrierMnemonic, "jne") &&
           !std::strcmp(q.reason, "loop_or_backedge_refused"));
  }
  auto middle = code;
  middle[0x20] = 0x37 - 0x21;
  q = inspect(middle);
  assert(!q.xyz && q.barrierHasEdge && q.barrierEdge == 0x37);
  auto outside = code;
  put(outside, 0x1f, {0xe9, 0xff, 0, 0, 0});
  q = inspect(outside);
  assert(!q.xyz && !std::strcmp(q.reason, "branch_leaves_exact_writer"));
  puts("PASS: c15 self/backedges, mid-instruction and out-of-writer transfers "
       "refuse with the exact branch site/target; no recursion, loop fixpoint "
       "or interprocedural traversal");
  for (auto op :
       {std::array<unsigned char, 2>{0xff, 0xe0}, {0xff, 0xd0}, {0x0f, 0xc8}}) {
    auto c = code;
    put(c, 0x24, {op[0], op[1]});
    q = inspect(c);
    assert(!q.xyz && q.barrier == 0x24 && q.barrierValid);
    if (op[1] != 0xe0)
      assert(q.blocksVisited > 0);
  }
  auto memory = code;
  put(memory, 0x24, {0x41, 0x89, 0});
  q = inspect(memory);
  assert(!q.xyz && q.barrier == 0x24 && !std::strcmp(q.barrierMnemonic, "mov"));
  auto exact = sites;
  exact[0] = 0x37;
  q = ws::rdx::branchWriter(code.data(), code.size(), exact);
  assert(!q.xyz && !q.exactSites && q.barrier == 0x37);
  auto wrongSite = code;
  wrongSite[0x36] = wrongSite[0x37] = wrongSite[0x38] = 0x90;
  q = inspect(wrongSite);
  assert(!q.xyz && q.barrier == 0x36);
  auto wrongDestination = code;
  wrongDestination[0x38] = 0x34;
  q = inspect(wrongDestination);
  assert(!q.xyz && q.barrier == 0x36);
  puts("PASS: c15 unsupported/indirect/call/memory effects report exact next "
       "local RVA/mnemonic, and wrong store RVA/target/component cannot borrow "
       "a nearby store");
  auto independent = code;
  put(independent, 0x45, {0x89, 0x71, 0x3c});
  std::fill(independent.begin() + 0x48, independent.begin() + 0x4c, 0x90);
  q = inspect(independent);
  assert(q.xyz && q.w && !q.wConstant &&
         !std::strcmp(q.wKind, "independent_register"));
  auto computed = code;
  put(computed, 0x30, {0xf3, 0x0f, 0x58, 0xc1});
  put(computed, 0x45, {0xf3, 0x0f, 0x11, 0x41, 0x3c});
  computed[0x4a] = computed[0x4b] = 0x90;
  q = inspect(computed);
  assert(q.xyz && !q.w && !std::strcmp(q.wKind, "computed"));
  auto conflict = independent;
  put(conflict, 0x24, {0x31, 0xf6});
  q = inspect(conflict);
  assert(q.xyz && q.allRelevant && !q.w && !std::strcmp(q.wKind, "unknown"));
  puts("PASS: c15 W constant-one/independent/computed/conflicting provenance "
       "is classified separately from XYZ; destination equality never proves W "
       "origin or homogeneous semantics");
}
