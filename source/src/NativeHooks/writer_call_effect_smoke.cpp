// Independently authored, nonexecuted CPU fixtures. No captured game body.
#include "NativeHooks/rdx_source_contract.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <vector>
using Code = std::array<unsigned char, 183>;
constexpr std::array<unsigned, 4> sites{0x36, 0x3c, 0x42, 0x45};
constexpr unsigned writer = 0x1000, callee = 0x2000;
static void put(Code &c, unsigned at, std::initializer_list<unsigned char> b) {
  assert(at + b.size() <= c.size());
  std::copy(b.begin(), b.end(), c.begin() + at);
}
static Code fixture() {
  Code c{};
  c.fill(0x90);
  put(c, 0,
      {0x48, 0x8b, 0xf9,   // RDI = entry RCX
       0x48, 0x8b, 0xf2}); // RSI = entry RDX
  put(c, 0x1b, {0x75, 0x01});
  put(c, 0x1f, {0x75, 0x02});
  put(c, 0x27, {0xe8, 0, 0, 0, 0});
  std::int32_t rel = callee - (writer + 0x2c);
  std::memcpy(c.data() + 0x28, &rel, 4);
  put(c, 0x2c, {0x8b, 0x06, 0x8b, 0x5e, 0x04, 0x8b, 0x56, 0x08});
  put(c, 0x36, {0x89, 0x47, 0x30});
  put(c, 0x3c, {0x89, 0x5f, 0x34});
  put(c, 0x42, {0x89, 0x57, 0x38});
  put(c, 0x45, {0xc7, 0x47, 0x3c, 0, 0, 0x80, 0x3f});
  c[0x4c] = 0xc3;
  return c;
}
static ws::rdx::CallEffect effect(const Code &c,
                                  const std::vector<unsigned char> &body) {
  auto e = ws::rdx::exactCall(c.data(), c.size(), writer, 0x5000, 0x27, 0x2c);
  ws::rdx::classifyCallee(e, body.data(), body.size(), callee,
                          callee + body.size());
  return e;
}
static ws::rdx::Writer inspect(const Code &c, ws::rdx::CallEffect &e) {
  return ws::rdx::branchWriter(c.data(), c.size(), sites, &e);
}
int main() {
  auto c = fixture();
  auto e = effect(c, {0x31, 0xc0, 0xc3});
  auto q = inspect(c, e);
  assert(e.instructionVerified && e.boundaryVerified && e.decoded &&
         e.noWrite && e.relevant);
  assert(e.target == callee && e.ret == writer + 0x2c && q.xyz && q.w &&
         q.wConstant);
  assert(e.gprClobber == 0xf07 && e.xmmClobber == 0x3f &&
         q.reachingStates[0] == 4);
  assert(!ws::rdx::branchWriter(c.data(), c.size(), sites).xyz);
  puts("PASS: c16 exact direct leaf call/no memory writes resumes both nested "
       "local branch paths at exact return and four stores; default c15 call "
       "refusal retained");
  auto volatileCarrier = c;
  put(volatileCarrier, 6,
      {0x8b, 0x02}); // EAX origin before call, reload removed
  volatileCarrier[0x2c] = volatileCarrier[0x2d] = 0x90;
  e = effect(volatileCarrier, {0xc3});
  q = inspect(volatileCarrier, e);
  assert(!q.xyz && q.componentProofMask == 6);
  auto xmm = c;
  put(xmm, 6, {0xf3, 0x0f, 0x10, 0x02});
  put(xmm, 0x36, {0xf3, 0x0f, 0x11, 0x47, 0x30});
  e = effect(xmm, {0xc3});
  q = inspect(xmm, e);
  assert(!q.xyz && q.componentProofMask == 6);
  auto nonvolatile = c;
  e = effect(c, {0x48, 0x31, 0xf6, 0xc3});
  q = inspect(nonvolatile, e);
  assert(e.noWrite && (e.gprClobber & (1 << 6)) && !q.xyz);
  e = effect(c, {0x48, 0x31, 0xff, 0xc3});
  q = inspect(c, e);
  assert(e.noWrite && !q.xyz && q.barrier == sites[0]);
  puts("PASS: c16 volatile GPR/XMM carriers never survive by equality; reload "
       "from preserved source alias succeeds, explicit nonvolatile "
       "source-alias clobber refuses");
  for (unsigned component = 0; component < 3; ++component) {
    e = effect(c, {0xc7, 0x42, static_cast<unsigned char>(component * 4), 1, 0,
                   0, 0, 0xc3});
    q = inspect(c, e);
    assert(!q.xyz && !e.noWrite && e.barrier == callee && q.barrier == 0x27);
  }
  e = effect(c, {0x41, 0x89, 0x00, 0xc3});
  q = inspect(c, e);
  assert(!q.xyz && !e.noWrite && e.barrier == callee &&
         !std::strcmp(e.mnemonic, "mov"));
  for (auto body : {std::vector<unsigned char>{0x50, 0xc3},
                    {0x48, 0x89, 0x44, 0x24, 8, 0xc3},
                    {0xf3, 0xab, 0xc3},
                    {0x48, 0x83, 0xec, 0x20, 0xc3}}) {
    e = effect(c, body);
    q = inspect(c, e);
    assert(!q.xyz && !e.noWrite && e.barrier >= callee);
  }
  puts("PASS: c16 source XYZ/unknown pointer/stack/implicit writes and RSP "
       "effects fail closed at exact callee RVA; no ABI-based memory-safety "
       "assumption");
  e = effect(c, {0x56, 0x48, 0x83, 0xec, 0x20, 0x48, 0x89, 0x74,
                 0x24, 0x10, 0x48, 0x31, 0xf6, 0x48, 0x8b, 0x74,
                 0x24, 0x10, 0x48, 0x83, 0xc4, 0x20, 0x5e, 0xc3});
  q = inspect(c, e);
  assert(e.noWrite && e.privateStackLow == -24 && q.callStackLow == -32 &&
         !(e.gprClobber & (1 << 6)) && q.xyz && q.w);
  e = effect(c, {0x48, 0x83, 0xec, 0x18, 0x0f, 0x11, 0x34, 0x24, 0x0f, 0x57,
                 0xf6, 0x0f, 0x10, 0x34, 0x24, 0x48, 0x83, 0xc4, 0x18, 0xc3});
  q = inspect(c, e);
  assert(e.noWrite && !(e.xmmClobber & (1 << 6)) && q.xyz);
  auto spill = c;
  put(spill, 6, {0x48, 0x89, 0x74, 0x24, 0xf0});
  e = effect(spill, {0x56, 0x5e, 0xc3});
  q = inspect(spill, e);
  assert(e.noWrite && !q.xyz && q.barrier == 0x27);
  for (auto body :
       {std::vector<unsigned char>{0x48, 0x89, 0x44, 0x24, 0xf8, 0xc3},
        {0x48, 0x81, 0xec, 0x10, 0x10, 0, 0, 0xc3},
        {0x48, 0x89, 0x04, 0x24, 0xc3},
        {0x5e, 0xc3}}) {
    e = effect(c, body);
    q = inspect(c, e);
    assert(!q.xyz && !e.noWrite);
  }
  puts("PASS: c16 proven private stack PUSH/POP/GPR/XMM saves and decoded "
       "restoration "
       "preserve carriers; caller spill overlap, unallocated/return/home "
       "writes, "
       "unbalanced/oversized stack refuse without ABI memory assumptions");
  auto indirect = c;
  put(indirect, 0x27, {0xff, 0xd0});
  e = ws::rdx::exactCall(indirect.data(), indirect.size(), writer, 0x5000, 0x27,
                         0x29);
  assert(!e.instructionVerified && !std::strcmp(e.kind, "indirect_call"));
  for (auto pair : {std::array<unsigned, 2>{0x28, 0x2c}, {0x27, 0x2d}}) {
    e = ws::rdx::exactCall(c.data(), c.size(), writer, 0x5000, pair[0],
                           pair[1]);
    assert(!e.instructionVerified);
  }
  e = ws::rdx::exactCall(c.data(), c.size(), writer, callee, 0x27, 0x2c);
  assert(!e.instructionVerified);
  e = effect(c, {0xc3});
  e.ret++;
  q = inspect(c, e);
  assert(!q.xyz);
  e = effect(c, {0xc3});
  e.target++;
  q = inspect(c, e);
  assert(!q.xyz);
  e = effect(c, {0xc3});
  ws::rdx::classifyCallee(e, reinterpret_cast<const unsigned char *>("\xc3"), 1,
                          callee + 1, callee + 2);
  assert(!e.noWrite);
  auto big = std::vector<unsigned char>(1025, 0x90);
  big.back() = 0xc3;
  e = effect(c, big);
  assert(!e.noWrite);
  puts("PASS: c16 indirect/out-of-image/site/return/target/boundary/cap "
       "mismatch refuses without pointer chasing or target expansion");
  e = effect(c, {0xe9, 0, 0, 0, 0, 0xc3});
  q = inspect(c, e);
  assert(!q.xyz && !std::strcmp(e.kind, "thunk/wrapper") &&
         e.barrier == callee);
  e = effect(c, {0x90, 0x75, 0, 0xc3});
  q = inspect(c, e);
  assert(!q.xyz && e.barrier == callee + 1);
  e = effect(c, {0xe8, 0, 0, 0, 0, 0xc3});
  q = inspect(c, e);
  assert(!q.xyz && e.barrier == callee);
  e = effect(c, {0xc3, 0x0f});
  q = inspect(c, e);
  assert(!q.xyz && !e.noWrite && e.barrier == callee + 1 &&
         !std::strcmp(e.mnemonic, "decode_failure"));
  e = effect(c, {0x90});
  q = inspect(c, e);
  assert(!q.xyz && !e.noWrite);
  auto later = c;
  put(later, 0x34, {0x0f, 0xc8});
  e = effect(later, {0xc3});
  q = inspect(later, e);
  assert(!q.xyz && q.barrier == 0x34 &&
         !std::strcmp(q.barrierMnemonic, "bswap"));
  auto stack = c;
  put(stack, 6, {0x48, 0x89, 0x74, 0x24, 8});
  e = effect(stack, {0xc3});
  q = inspect(stack, e);
  assert(
      !q.xyz &&
      !std::strcmp(q.reason, "call_stack_return_or_shadow_alias_unresolved"));
  puts("PASS: c16 callee thunk/branch/nested call stops at exact effect; safe "
       "call exposes strictly later writer blocker and saved-stack "
       "return/shadow alias refuses");
  auto w = c;
  put(w, 0x45, {0x89, 0x5f, 0x3c});
  std::fill(w.begin() + 0x48, w.begin() + 0x4c, 0x90);
  e = effect(w, {0xc3});
  q = inspect(w, e);
  assert(q.xyz && q.w && !q.wConstant &&
         !std::strcmp(q.wKind, "independent_memory"));
  w = c;
  put(w, 0x45, {0x89, 0x6f, 0x3c});
  std::fill(w.begin() + 0x48, w.begin() + 0x4c, 0x90);
  e = effect(w, {0xc3});
  q = inspect(w, e);
  assert(q.xyz && q.w && !std::strcmp(q.wKind, "independent_register"));
  w = c;
  put(w, 0x45, {0xf3, 0x0f, 0x11, 0x47, 0x3c});
  w[0x4a] = w[0x4b] = 0x90;
  e = effect(w, {0xc3});
  q = inspect(w, e);
  assert(q.xyz && !q.w);
  put(w, 0x2c, {0xf3, 0x0f, 0x10, 0x06});
  put(w, 0x30, {0xf3, 0x0f, 0x58, 0xc0});
  // Restore XYZ reload at a separate earlier slot after the call: use
  // nonvolatile carriers.
  put(w, 6, {0x44, 0x8b, 0x26, 0x44, 0x8b, 0x6e, 4, 0x44, 0x8b, 0x76, 8});
  put(w, 0x36, {0x44, 0x89, 0x67, 0x30});
  put(w, 0x3c, {0x44, 0x89, 0x6f, 0x34});
  put(w, 0x42, {0x44, 0x89, 0x77, 0x38});
  // Third store now ends at +0x46; shift W site for this independent layout.
  std::fill(w.begin() + 0x46, w.begin() + 0x4c, 0x90);
  put(w, 0x46, {0xf3, 0x0f, 0x11, 0x47, 0x3c});
  e = effect(w, {0xc3});
  q = ws::rdx::branchWriter(w.data(), w.size(), {0x36, 0x3c, 0x42, 0x46}, &e);
  assert(q.xyz && !q.w && !std::strcmp(q.wKind, "computed"));
  puts("PASS: c16 W constant/register/memory remains independent; volatile "
       "unknown and post-call computed W never borrow XYZ proof");
}
