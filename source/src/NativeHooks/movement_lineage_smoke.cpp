// Independent owned x64 fixtures. No captured game instruction bodies.
#include "Math/movement_delta.hpp"
#include "NativeHooks/movement_lineage.hpp"
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <windows.h>
using namespace ws::lineage;
using Bytes = std::vector<unsigned char>;
static Bytes copy(bool stack = false) {
  Bytes b;
  for (unsigned k = 0; k < 4; ++k) {
    if (stack)
      b.insert(b.end(),
               {0xf3, 0x0f, 0x10, 0x44, 0x24, (unsigned char)(40 + k * 4)});
    else
      b.insert(b.end(), {0xf3, 0x0f, 0x10, 0x42, (unsigned char)(k * 4)});
    b.insert(b.end(), {0xf3, 0x0f, 0x11, 0x41, (unsigned char)(48 + k * 4)});
  }
  b.push_back(0xc3);
  return b;
}
static Proof proof(const Bytes &b) { return writer(b.data(), b.size()); }
static bool read(std::uintptr_t a, void *o, std::size_t z) {
  SIZE_T n{};
  return ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(a), o,
                           z, &n) &&
         n == z;
}
static Context context(float *target, float *source) {
  Context c{};
  c.gpr[1] = reinterpret_cast<std::uintptr_t>(target);
  c.gpr[2] = reinterpret_cast<std::uintptr_t>(source);
  c.floating = true;
  c.xmm[0] = source[3];
  return c;
}
static void call(Bytes &b, unsigned offset, std::uint64_t begin,
                 std::uint64_t ret, std::uint64_t target) {
  b[offset] = 0xe8;
  auto d = std::int32_t(target - ret);
  std::memcpy(b.data() + offset + 1, &d, 4);
  (void)begin;
}
int main() {
  auto b = copy();
  auto p = proof(b);
  assert(p.complete && p.mask == 15 && p.storeCount == 4 && p.decoded);
  float target[16]{}, source[4]{10, 20, 30, 1};
  std::memcpy(target + 12, source, 16);
  auto c = context(target, source);
  auto s = sample(p, p.stores[3].after, c,
                  reinterpret_cast<std::uintptr_t>(target), target + 12, read);
  assert(s.target && s.fullRow && s.match && s.mask == 15);
  assert(target[0] == 0 && source[0] == 10);
  target[15] = 7;
  s = sample(p, p.stores[3].after, c, reinterpret_cast<std::uintptr_t>(target),
             target + 12, read);
  assert(!s.match && s.wObserved);
  target[15] = 1;
  target[12] += 2;
  s = sample(p, p.stores[3].after, c, reinterpret_cast<std::uintptr_t>(target),
             target + 12, read);
  assert(!s.match);
  target[12] -= 2;
  auto wrong = c;
  wrong.gpr[1] += 16;
  assert(!sample(p, p.stores[3].after, wrong,
                 reinterpret_cast<std::uintptr_t>(target), target + 12, read)
              .target);
  wrong = c;
  wrong.gpr[2] = 1;
  assert(!sample(p, p.stores[3].after, wrong,
                 reinterpret_cast<std::uintptr_t>(target), target + 12, read)
              .readable);
  assert(!sample(p, 3, c, reinterpret_cast<std::uintptr_t>(target), target + 12,
                 read)
              .found);
  puts("PASS: c12 complete scalar XYZ/W pointer lineage, guarded trap-time "
       "source reconstruction, exact target, invalid pointer, W "
       "mismatch/downstream overwrite negatives and metadata preservation");
  auto st = proof(copy(true));
  assert(st.complete);
  std::array<unsigned char, 128> stack{};
  std::memcpy(stack.data() + 40, source, 16);
  c.gpr[4] = reinterpret_cast<std::uintptr_t>(stack.data());
  assert(sample(st, st.stores[3].after, c,
                reinterpret_cast<std::uintptr_t>(target), target + 12, read)
             .match);
  Bytes registers;
  for (unsigned k = 0; k < 4; ++k)
    registers.insert(registers.end(),
                     {0xf3, 0x0f, 0x11, (unsigned char)(0x49 + k * 8),
                      (unsigned char)(48 + k * 4)});
  registers.push_back(0xc3);
  auto rp = proof(registers);
  assert(rp.complete);
  for (unsigned k = 0; k < 4; ++k)
    c.xmm[(k + 1) * 4] = source[k];
  assert(sample(rp, rp.stores[3].after, c,
                reinterpret_cast<std::uintptr_t>(target), target + 12, read)
             .match);
  wrong = c;
  wrong.floating = false;
  assert(!sample(rp, rp.stores[3].after, wrong,
                 reinterpret_cast<std::uintptr_t>(target), target + 12, read)
              .readable);
  Bytes prologue{0x57, 0x48, 0x83, 0xec, 0x20};
  auto cb = copy();
  cb.pop_back();
  prologue.insert(prologue.end(), cb.begin(), cb.end());
  prologue.insert(prologue.end(), {0x48, 0x83, 0xc4, 0x20, 0x5f, 0xc3});
  assert(proof(prologue).complete);
  puts("PASS: c12 entry register/stack/pointer variants, supported "
       "prologue/epilogue and unavailable floating context refuse");
  Bytes callerBytes(64, 0x90);
  callerBytes.back() = 0xc3;
  call(callerBytes, 32, 0x1000, 0x1025, 0x2000);
  auto dc =
      callsite(callerBytes.data(), callerBytes.size(), 0x1000, 0x1025, 0x2000);
  assert(dc.decoded && dc.verified && dc.offset == 32);
  assert(
      !callsite(callerBytes.data(), callerBytes.size(), 0x1000, 0x1024, 0x2000)
           .verified);
  callerBytes[33] ^= 1;
  assert(
      !callsite(callerBytes.data(), callerBytes.size(), 0x1000, 0x1025, 0x2000)
           .verified);
  callerBytes = {0x48, 0xb8, 0xe8, 0, 0, 0, 0, 0, 0, 0, 0xc3};
  assert(
      !callsite(callerBytes.data(), callerBytes.size(), 0x1000, 0x1007, 0x2000)
           .verified);
  assert(!callsite(nullptr, 0, 0, 0, 0).verified);
  puts("PASS: c12 complete caller decode/anchored direct E8 target/return, "
       "wrong target/return and embedded noninstruction E8 rejection");
  auto partial = b;
  partial.erase(partial.end() - 11, partial.end() - 1);
  assert(!proof(partial).complete);
  auto multi = b;
  multi.insert(multi.end() - 1, b.begin(), b.begin() + 10);
  assert(!proof(multi).complete);
  auto conditional = b;
  conditional.insert(conditional.begin(), {0x75, 0x00});
  auto neg = proof(conditional);
  assert(!neg.complete && neg.unsupported && neg.firstUnsupported == 0);
  auto indirect = b;
  indirect.insert(indirect.begin(), {0xff, 0xd0});
  assert(!proof(indirect).complete);
  auto truncated = b;
  truncated.back() = 0x0f;
  assert(!proof(truncated).decoded);
  auto tail = b;
  tail.insert(tail.end() - 1, {0x85, 0xc0, 0x75, 0x00});
  auto tp = proof(tail);
  assert(!tp.complete && tp.sourceMap);
  assert(sample(tp, tp.stores[3].after, c,
                reinterpret_cast<std::uintptr_t>(target), target + 12, read)
             .match);
  auto computed = b;
  computed.insert(computed.begin() + 5, {0xf3, 0x0f, 0x58, 0xc1});
  auto cp = proof(computed);
  assert(cp.complete);
  char expression[384]{};
  describe(cp, cp.stores[0].value, expression, sizeof(expression));
  assert(std::string(expression).find('+') != std::string::npos);
  Bytes avx{0x0f, 0x10, 0x02, 0xc5, 0xf2, 0x58,
            0xc2, 0x0f, 0x11, 0x41, 0x30, 0xc3};
  auto ap = proof(avx);
  assert(ap.complete);
  describe(ap, ap.stores[1].value, expression, sizeof(expression));
  assert(std::string(expression) == "entry_xmm1.lane1");
  // Repeated DAG edges and bounded pool exhaustion must not expand
  // exponentially.
  Bytes dense{0x0f, 0x10, 0x02};
  for (unsigned i = 0; i < 180; ++i)
    dense.insert(dense.end(), {0x0f, 0x59, 0xc0});
  dense.insert(dense.end(), {0x0f, 0x11, 0x41, 0x30, 0xc3});
  assert(!proof(dense).sourceMap);
  puts("PASS: c12 partial/multiple writes, branch/call/invalid decode fail "
       "closed; supported computed scalar retains expression without semantic "
       "upgrade");
  // Register payload constructed with untyped add and multiply. Arithmetic
  // shape is observable; calling a scalar dt or a vector velocity would be a
  // guess.
  for (unsigned pattern = 0; pattern < 3; ++pattern) {
    Bytes body{0x0f, 0x10, 0x0a};
    if (pattern != 1)
      body.insert(body.end(), {0x0f, 0x58, 0xcb});
    if (pattern != 0)
      body.insert(body.end(), {0x0f, 0x59, 0xcc});
    auto at = unsigned(body.size());
    body.insert(body.end(), {0xe8, 0, 0, 0, 0, 0xc3});
    auto result = caller(body.data(), body.size(), at, rp);
    assert(result.decoded && std::string(result.construction) == "unknown");
    assert(std::string(result.shape) == (pattern == 0 ? "add_untyped"
                                         : pattern == 1
                                             ? "multiply_untyped"
                                             : "add_and_multiply_untyped"));
  }
  // Full vector payload lets caller substitution walk pointer loads as well.
  auto at = unsigned(0);
  Bytes pointerCaller{0x48, 0x8d, 0x54, 0x24, 0x20, 0xe8, 0, 0, 0, 0, 0xc3};
  at = 5;
  auto cr = caller(pointerCaller.data(), pointerCaller.size(), at, p);
  assert(cr.sliceComplete);
  describe(cr.slice, cr.payload[0], expression, sizeof(expression));
  assert(std::string(expression).find("entry_gpr4") != std::string::npos);
  puts("PASS: c12 caller payload substitution/stack construction and "
       "target+delta/displacement/velocity-times-dt-shaped arithmetic stay "
       "untyped until provenance is independently established");
  for (auto frame : std::array<ws::diag::ActorAxes, 2>{
           {{{0, 0, 1}, {1, 0, 0}, {0, 1, 0}, true},
            {{-.70710677f, 0, -.70710677f},
             {-.70710677f, 0, .70710677f},
             {0, 1, 0},
             true}}}) {
    ws::Vec3 start{3, 4, 5};
    auto end = start + frame.forward * 2 + frame.right * .5f;
    auto d = ws::movement::decompose(start, end, frame, 100);
    assert(d.valid && std::abs(d.normal) < 1e-5 &&
           std::abs(d.forward - 2) < 1e-5 && std::abs(d.right - .5f) < 1e-5);
  }
  puts("PASS: c12 A/cardinal and E/diagonal predicted-to-observed SurfaceFrame "
       "delta correlation is diagnostic only");
}
