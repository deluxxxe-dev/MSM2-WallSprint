// Independent owned/offline instruction fixtures; no captured game bytes.
#include "NativeHooks/rdx_source_contract.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
extern "C" {
extern unsigned char ws_rdx_slice[], ws_rdx_slice_end[], ws_rdx_slice_x[],
    ws_rdx_slice_y[], ws_rdx_slice_z[], ws_rdx_slice_w[];
extern unsigned char ws_rdx_scalar[], ws_rdx_scalar_end[], ws_rdx_half[],
    ws_rdx_half_end[];
extern unsigned char ws_rdx_local[], ws_rdx_local_call[], ws_rdx_local_end[],
    ws_rdx_inherited[], ws_rdx_inherited_call[], ws_rdx_inherited_end[];
}
static std::vector<unsigned char> body(unsigned char *a, unsigned char *b) {
  return {a, b};
}
static bool read(std::uintptr_t p, void *out, std::size_t n) {
  std::memcpy(out, reinterpret_cast<void *>(p), n);
  return true;
}
static void call(unsigned char *p, std::uintptr_t target) {
  p[0] = 0xe8;
  auto d = std::int32_t(target - reinterpret_cast<std::uintptr_t>(p + 5));
  std::memcpy(p + 1, &d, 4);
}
static void jump(unsigned char *p, std::uintptr_t target) {
  p[0] = 0xe9;
  auto d = std::int32_t(target - reinterpret_cast<std::uintptr_t>(p + 5));
  std::memcpy(p + 1, &d, 4);
}
int main() {
  auto bytes = body(ws_rdx_slice, ws_rdx_slice_end);
  auto p = ws::rdx::writer(bytes.data(), bytes.size());
  if (!p.xyz)
    std::printf("debug decoded=%u prefix=%u mask=%x barrier=%x reason=%s\n",
                p.decoded, p.prefix, p.mask, p.barrier, p.reason);
  assert(p.decoded && p.prefix && p.xyz && p.w && p.wConstant &&
         p.wBits == 0x3f800000 && !std::strcmp(p.wKind, "constant_one"));
  unsigned char *labels[]{ws_rdx_slice_x, ws_rdx_slice_y, ws_rdx_slice_z,
                          ws_rdx_slice_w};
  for (unsigned k = 0; k < 4; ++k)
    assert(p.stores[k] == std::size_t(labels[k] - ws_rdx_slice));
  for (auto pair : {std::pair<unsigned char *, unsigned char *>{
                        ws_rdx_scalar, ws_rdx_scalar_end},
                    {ws_rdx_half, ws_rdx_half_end}}) {
    auto row = body(pair.first, pair.second);
    auto q = ws::rdx::writer(row.data(), row.size());
    assert(q.xyz && q.w);
  }
  puts("PASS: c14 owned scalar, AVX packed/shuffle at +0xA and double-lane "
       "loads prove RDX XYZ copies independently from constant-one W");
  auto wrong = bytes;
  wrong[13] = 1;
  assert(!ws::rdx::writer(wrong.data(), wrong.size())
              .xyz); // change [RDX] to [RCX]
  wrong = bytes;
  wrong[0] = 0xeb;
  wrong[1] = 0;
  assert(!ws::rdx::writer(wrong.data(), wrong.size()).xyz);
  wrong = bytes;
  wrong.insert(wrong.begin(), {0xc7, 0x02, 0, 0, 0, 0});
  assert(!ws::rdx::writer(wrong.data(), wrong.size()).xyz); // writes source
  wrong = bytes;
  wrong.insert(wrong.begin(), {
                                  0x41,
                                  0x89,
                                  0,
                              });
  assert(!ws::rdx::writer(wrong.data(), wrong.size())
              .xyz); // unknown external store
  wrong = bytes;
  wrong.insert(wrong.begin(), {0xc7, 0x44, 0x24, 0x28, 0, 0, 0, 0});
  assert(!ws::rdx::writer(wrong.data(), wrong.size()).xyz);
  wrong = bytes;
  wrong.insert(wrong.begin() + 10 + 4, {0x0f, 0x58, 0xc0});
  assert(!ws::rdx::writer(wrong.data(), wrong.size())
              .xyz); // arithmetic equality is not bitcopy
  assert(!ws::rdx::writer(bytes.data(), bytes.size() - 3).xyz);
  wrong = bytes;
  wrong[std::size_t(ws_rdx_slice_w - ws_rdx_slice) + 3] =
      0; // constant still valid; make 2.0
  wrong[std::size_t(ws_rdx_slice_w - ws_rdx_slice) + 5] = 0;
  wrong[std::size_t(ws_rdx_slice_w - ws_rdx_slice) + 6] = 0x40;
  auto differentW = ws::rdx::writer(wrong.data(), wrong.size());
  assert(differentW.xyz && differentW.w &&
         std::strcmp(differentW.wKind, "constant_one"));
  puts("PASS: c14 wrong source, branch, source/unknown-memory/input-stack "
       "mutation, arithmetic resemblance, truncated decode and independent W "
       "negatives fail closed");
  auto c = ws::rdx::caller(ws_rdx_local,
                           std::size_t(ws_rdx_local_end - ws_rdx_local),
                           unsigned(ws_rdx_local_call - ws_rdx_local));
  assert(c.decoded && c.prefix && c.rdxLocal && c.owned && c.constructed);
  auto storage = ws::rdx::storage(0x1000, 0x1028, 0x800, 0x2000, true, true, c);
  assert(storage.sameBytes && storage.callerOwned);
  assert(
      !ws::rdx::storage(0x1000, 0x1030, 0x800, 0x2000, true, true, c).bounded);
  assert(
      !ws::rdx::storage(0x1000, 0x1028, 0x800, 0x1030, true, true, c).bounded);
  assert(
      !ws::rdx::storage(UINTPTR_MAX - 0x10, 0x18, 0, UINTPTR_MAX, true, true, c)
           .bounded);
  assert(!ws::rdx::storage(0x1000, 0x1028, 0x800, 0x2000, false, true, c)
              .sameBytes);
  assert(!ws::rdx::storage(0x1000, 0x1028, 0x800, 0x2000, true, false, c)
              .sameBytes);
  auto inherited = ws::rdx::caller(
      ws_rdx_inherited, std::size_t(ws_rdx_inherited_end - ws_rdx_inherited),
      unsigned(ws_rdx_inherited_call - ws_rdx_inherited));
  assert(inherited.prefix && !inherited.rdxLocal && !inherited.owned &&
         !inherited.constructed);
  auto local = body(ws_rdx_local, ws_rdx_local_end);
  local[3] = 0x28;
  auto shortFrame = ws::rdx::caller(local.data(), local.size(),
                                    unsigned(ws_rdx_local_call - ws_rdx_local));
  assert(shortFrame.rdxLocal && !shortFrame.owned);
  puts(
      "PASS: c14 exact caller stack allocation/construction and current-thread "
      "per-call bounds narrow lifetime; inherited RDX, overlapping return/home "
      "bytes, unreadable/mismatched/overflow objects cannot obtain ownership");
  unsigned char chain[64]{};
  auto base = reinterpret_cast<std::uintptr_t>(chain), consumer = base + 48;
  call(chain, consumer);
  chain[5] = 0xc3;
  auto rel = ws::rdx::relation(chain, 6, base, base + 5, consumer, read);
  assert(rel.reaches && !std::strcmp(rel.kind, "direct"));
  call(chain, base + 16);
  jump(chain + 16, consumer);
  rel = ws::rdx::relation(chain, 6, base, base + 5, consumer, read);
  assert(rel.reaches && rel.hops == 1 &&
         !std::strcmp(rel.kind, "wrapper_tail_jump"));
  jump(chain + 16, base + 32);
  chain[32] = 0xeb;
  chain[33] = 14;
  rel = ws::rdx::relation(chain, 6, base, base + 5, consumer, read);
  assert(rel.reaches && rel.hops == 2);
  chain[32] = 0x90;
  rel = ws::rdx::relation(chain, 6, base, base + 5, consumer, read);
  assert(!rel.reaches && rel.next == base + 32);
  unsigned char ind[]{0xff, 0xd0, 0xc3};
  rel = ws::rdx::relation(ind, 3, base, base + 2, consumer, nullptr);
  assert(!rel.reaches && !std::strcmp(rel.kind, "indirect"));
  assert(!ws::rdx::relation(chain, 6, base, base + 4, consumer, read).reaches);
  puts("PASS: c14 anchored direct/wrapper/two-hop tail/indirect producer "
       "classification reports exact unresolved target without guessing "
       "provenance");
}
