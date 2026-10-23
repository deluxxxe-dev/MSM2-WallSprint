// TEST ONLY: independent owned functions and actual MinHook/hardware
// exceptions. Internal fixture setup cannot admit a synthetic function through
// production's exact game-RVA/boundary/hash API. No game body or runtime
// evidence is copied.
#include "writer_entry_snapshot.cpp"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <immintrin.h>
#include <string>
#include <thread>
#include <vector>
static std::string output;
namespace msm2 {
bool readMemory(std::uintptr_t p, void *out, std::size_t n) {
  SIZE_T got{};
  return ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(p),
                           out, n, &got) &&
         got == n;
}
void writeLog(const char *fmt, ...) {
  char b[4096];
  va_list args;
  va_start(args, fmt);
  vsnprintf(b, sizeof(b), fmt, args);
  va_end(args);
  output += b;
  output += '\n';
}
} // namespace msm2
extern "C" {
alignas(32) float ws_test_vector_input[128], ws_test_vectors[128],
    ws_test_returns[128];
std::uint64_t ws_test_gprs[11], ws_test_result, ws_test_return_gprs[11],
    ws_test_nonvolatile[8];
void ws_test_probe_target();
void ws_test_invoke_probe(void *);
std::uint64_t ws_owned_entry_writer(float *, const float *, std::uint64_t);
std::uint64_t ws_owned_entry_caller(float *, const float *, std::uint64_t);
void ws_owned_entry_return();
}
static msm2::reset_trace::Request expected;
static std::atomic<bool> valid{true};
static std::atomic<ULONGLONG> nativeMs{0};
static bool clobber{};
static bool validate(const msm2::reset_trace::Request &q) noexcept {
  if (clobber) {
    _mm_setcsr(0x3f80);
    __asm__ volatile("fninit; vzeroall" ::
                         : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5",
                           "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11",
                           "xmm12", "xmm13", "xmm14", "xmm15");
  }
  auto tick = GetTickCount64();
  return valid && q.session == expected.session &&
         q.generation == expected.generation && q.hero == expected.hero &&
         q.transform == expected.transform && q.owner == expected.owner &&
         q.thread == expected.thread &&
         q.permissionEpoch == expected.permissionEpoch &&
         q.acceptedEpoch == expected.acceptedEpoch && tick >= nativeMs &&
         tick - nativeMs <= 100;
}
static void prepare(void *function, void *ret) {
  imageBase = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
  target = reinterpret_cast<std::uintptr_t>(function);
  callReturn = reinterpret_cast<std::uintptr_t>(ret);
  DWORD64 image{};
  auto f = RtlLookupFunctionEntry(target, &image, nullptr);
  assert(f && image + f->BeginAddress == target);
  functionSize = f->EndAddress - f->BeginAddress;
  assert(functionSize > 32 && functionSize < 1024);
  assert(
      msm2::writer_entry::guardedRead(target, baseline.data(), functionSize));
  assert(digest(baseline.data(), functionSize, baselineHash));
  assert(msm2::writer_entry::guardedRead(callReturn - 5, directCall.data(), 5));
  gate = validate;
  configured = true;
  chainKind = "unresolved";
  expected = {};
  expected.position = true;
  expected.session = 1;
  expected.generation = 1;
  expected.hero = 0x123;
  expected.transform = 0x1122334455667788ull;
  expected.owner = 0x456;
  expected.thread = 123;
  expected.permissionEpoch = 7;
  expected.acceptedEpoch = 8;
  expected.initMs = GetTickCount64() - 1700;
  expected.basis = {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}, true};
  nativeMs = GetTickCount64();
}
static void abi() {
  for (unsigned i = 0; i < 128; ++i)
    ws_test_vector_input[i] = float(i) * .125f + 1;
  ws_test_invoke_probe(reinterpret_cast<void *>(ws_test_probe_target));
  auto ret = ws_test_gprs[8];
  std::array<std::uint64_t, 11> before{};
  std::memcpy(before.data(), ws_test_gprs, sizeof(ws_test_gprs));
  std::array<float, 128> vectorBefore{}, returnBefore{};
  std::memcpy(vectorBefore.data(), ws_test_vectors, sizeof(ws_test_vectors));
  std::memcpy(returnBefore.data(), ws_test_returns, sizeof(ws_test_returns));
  prepare(reinterpret_cast<void *>(ws_test_probe_target),
          reinterpret_cast<void *>(ret));
  assert(msm2::writer_entry::start(expected));
  assert(!ws_movement_entry_enabled);
  assert(msm2::writer_entry::activate());
  clobber = true;
  auto csr = _mm_getcsr();
  unsigned short cw{}, sw{};
  __asm__ volatile("fld1; fnstcw %0; fnstsw %1" : "=m"(cw), "=m"(sw));
  SetLastError(0x991122);
  ws_test_invoke_probe(reinterpret_cast<void *>(ws_test_probe_target));
  assert(GetLastError() == 0x991122);
  unsigned short cwAfter{}, swAfter{};
  __asm__ volatile("fnstcw %0; fnstsw %1" : "=m"(cwAfter), "=m"(swAfter));
  assert(cw == cwAfter && sw == swAfter && csr == _mm_getcsr());
  double x87{};
  __asm__ volatile("fstpl %0" : "=m"(x87));
  assert(x87 == 1);
  assert(!std::memcmp(before.data(), ws_test_gprs, sizeof(ws_test_gprs)) &&
         !std::memcmp(vectorBefore.data(), ws_test_vectors,
                      sizeof(ws_test_vectors)) &&
         !std::memcmp(returnBefore.data(), ws_test_returns,
                      sizeof(ws_test_returns)));
  assert(ws_test_result == 0x77aabbcc11223344ull);
  for (unsigned i = 0; i < 8; ++i)
    assert(ws_test_nonvolatile[i] == 0x2222000011110000ull + i);
  auto &entry = records[0].snapshot;
  assert(records[0].state == 1 && entry.gpr[1] == before[1] &&
         entry.gpr[2] == before[2] && entry.ret == ret);
  assert(!std::memcmp(entry.xmm.data(), vectorBefore.data(), 16));
  assert(activeScope.load()->reserved == 1);
  msm2::writer_entry::stop();
  assert(!installed && bytesEqual(baseline));
  msm2::writer_entry::drain();
  assert(output.find("true_entry_snapshot=1") != std::string::npos);
  puts("PASS: c13 actual raw MinHook entry preserves integer/stack arguments, "
       "GPR/flags/nonvolatiles, all YMM0..15, XMM, x87/MXCSR, LastError and "
       "return value even when capture gate clobbers floating state");
}
struct Hit {
  std::atomic<bool> ready{false};
  std::uint64_t id{};
  std::uintptr_t rip{}, ret{}, caller2{};
  std::uint64_t g[16]{};
  unsigned char xmm[256]{};
  float p[4]{};
};
static std::array<Hit, 128> hits;
static std::atomic<unsigned> hitCount{0};
static float *hero;
static float source[4]{10, 20, 30, 0};
static float other[16]{};
static LONG CALLBACK watched(EXCEPTION_POINTERS *e) {
  if (e->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP)
    return EXCEPTION_CONTINUE_SEARCH;
  auto &c = *e->ContextRecord;
  auto mask = c.Dr6 & 3;
  if (!mask)
    return EXCEPTION_CONTINUE_SEARCH;
  auto error = GetLastError();
  if (c.Rip > target && c.Rip < target + functionSize) {
    CONTEXT u = c;
    DWORD64 image{};
    auto f = RtlLookupFunctionEntry(c.Rip, &image, nullptr);
    void *h{};
    DWORD64 frame{};
    assert(f);
    RtlVirtualUnwind(UNW_FLAG_NHANDLER, image, c.Rip, f, &u, &h, &frame,
                     nullptr);
    auto index = hitCount.fetch_add(1);
    assert(index < hits.size());
    auto &hit = hits[index];
    hit.id = msm2::writer_entry::pair(expected.session, expected.generation,
                                      expected.transform, u.Rsp - 8, u.Rip);
    hit.ret = u.Rip;
    auto caller = RtlLookupFunctionEntry(u.Rip - 1, &image, nullptr);
    if (caller) {
      RtlVirtualUnwind(UNW_FLAG_NHANDLER, image, u.Rip, caller, &u, &h, &frame,
                       nullptr);
      hit.caller2 = u.Rip;
    }
    hit.rip = c.Rip;
    std::uint64_t g[] = {c.Rax, c.Rcx, c.Rdx, c.Rbx, c.Rsp, c.Rbp,
                         c.Rsi, c.Rdi, c.R8,  c.R9,  c.R10, c.R11,
                         c.R12, c.R13, c.R14, c.R15};
    std::memcpy(hit.g, g, sizeof(g));
    std::memcpy(hit.xmm, &c.Xmm0, 256);
    assert(msm2::writer_entry::guardedRead(expected.transform + 48, hit.p, 16));
    hit.ready.store(true, std::memory_order_release);
  }
  c.Dr6 &= ~DWORD64(mask);
  SetLastError(error);
  return EXCEPTION_CONTINUE_EXECUTION;
}
struct Worker {
  HANDLE ready{}, go{}, done{}, thread{};
  DWORD tid{};
  unsigned role{};
  CONTEXT saved{};
};
static DWORD WINAPI work(void *ptr) {
  auto &w = *static_cast<Worker *>(ptr);
  SetEvent(w.ready);
  assert(WaitForSingleObject(w.go, 2000) == WAIT_OBJECT_0);
  for (unsigned i = 0; i < 3; ++i) {
    assert(ws_owned_entry_caller(hero, source, w.role == 0 && i == 0 ? 1 : 0) ==
           0x123455667788abcdull);
  }
  ws_owned_entry_caller(other, source, 0);
  SetEvent(w.done);
  return 0;
}
static CONTEXT get(Worker &w) {
  assert(SuspendThread(w.thread) != DWORD(-1));
  CONTEXT c{};
  c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
  assert(GetThreadContext(w.thread, &c));
  assert(ResumeThread(w.thread) != DWORD(-1));
  return c;
}
static void set(Worker &w, const CONTEXT &c) {
  assert(SuspendThread(w.thread) != DWORD(-1));
  assert(SetThreadContext(w.thread, &c));
  assert(ResumeThread(w.thread) != DWORD(-1));
}
static void pairing() {
  hero = static_cast<float *>(
      VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
  assert(hero);
  for (unsigned i = 0; i < 12; ++i)
    hero[i] = float(i + 1);
  prepare(reinterpret_cast<void *>(ws_owned_entry_writer),
          reinterpret_cast<void *>(ws_owned_entry_return));
  expected.transform = reinterpret_cast<std::uintptr_t>(hero);
  expected.hero = expected.transform + 128;
  expected.owner = expected.transform + 256;
  auto wrong = expected;
  for (unsigned key = 0; key < 8; ++key) {
    wrong = expected;
    switch (key) {
    case 0:
      ++wrong.session;
      break;
    case 1:
      ++wrong.generation;
      break;
    case 2:
      ++wrong.hero;
      break;
    case 3:
      ++wrong.transform;
      break;
    case 4:
      ++wrong.owner;
      break;
    case 5:
      ++wrong.thread;
      break;
    case 6:
      ++wrong.permissionEpoch;
      break;
    case 7:
      ++wrong.acceptedEpoch;
      break;
    }
    assert(!msm2::writer_entry::start(wrong) && !installed);
  }
  baselineHash[0] ^= 1;
  assert(!msm2::writer_entry::start(expected) && !installed);
  baselineHash[0] ^= 1;
  baseline[0] ^= 1;
  assert(!msm2::writer_entry::start(expected) && !installed);
  baseline[0] ^= 1;
  directCall[0] ^= 1;
  assert(!msm2::writer_entry::start(expected) && !installed);
  directCall[0] ^= 1;
  nativeMs = GetTickCount64() - 101;
  assert(!msm2::writer_entry::start(expected));
  nativeMs = GetTickCount64();
  assert(msm2::writer_entry::start(expected));
  assert(!ws_movement_entry_enabled);
  assert(msm2::writer_entry::activate());
  auto veh = AddVectoredExceptionHandler(1, watched);
  assert(veh);
  std::array<Worker, 2> workers{};
  for (unsigned i = 0; i < 2; ++i) {
    auto &w = workers[i];
    w.role = i;
    w.ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    w.go = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    w.done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    w.thread = CreateThread(nullptr, 0, work, &w, 0, &w.tid);
    assert(w.thread && WaitForSingleObject(w.ready, 2000) == WAIT_OBJECT_0);
    w.saved = get(w);
    auto c = w.saved;
    c.Dr0 = expected.transform + 48;
    c.Dr1 = expected.transform + 56;
    c.Dr7 = (c.Dr7 & ~0xff000full) | 0x990005ull;
    set(w, c);
  }
  nativeMs = GetTickCount64();
  for (auto &w : workers)
    SetEvent(w.go);
  for (auto &w : workers)
    assert(WaitForSingleObject(w.done, 4000) == WAIT_OBJECT_0);
  assert(hitCount == 28 && activeScope.load()->reserved == 7);
  for (unsigned i = 0; i < hitCount; ++i) {
    auto &hit = hits[i];
    assert(hit.ready.load(std::memory_order_acquire) && hit.id);
    msm2::writer_entry::observe(hit.id, hit.rip, expected.transform, hit.g,
                                hit.xmm, true, hit.p, hit.ret, hit.caller2);
  }
  unsigned full = 0;
  for (unsigned i = 0; i < 7; ++i) {
    auto &a = aggregates[i];
    assert(a.mask == 15 && a.printed && !std::memcmp(a.values, source, 12) &&
           a.values[3] == 1);
    ++full;
  }
  assert(full == 7);
  for (auto &w : workers) {
    assert(WaitForSingleObject(w.thread, 2000) == WAIT_OBJECT_0);
    for (auto h : {w.thread, w.ready, w.go, w.done})
      CloseHandle(h);
  }
  RemoveVectoredExceptionHandler(veh);
  auto before = activeScope.load()->reserved.load();
  nativeMs = GetTickCount64();
  ws_owned_entry_writer(hero, source, 0);
  assert(activeScope.load()->reserved ==
         before); // wrong caller cannot reuse a token
  valid = false;
  ws_owned_entry_caller(hero, source, 0);
  assert(activeScope.load()->reserved == before);
  valid = true;
  nativeMs = GetTickCount64() - 101;
  ws_owned_entry_caller(hero, source, 0);
  assert(activeScope.load()->reserved == before);
  nativeMs = GetTickCount64();
  ++expected.generation;
  ws_owned_entry_caller(hero, source, 0);
  assert(activeScope.load()->reserved == before);
  --expected.generation;
  for (unsigned i = 0; i < 64; ++i) {
    nativeMs = GetTickCount64();
    ws_owned_entry_caller(hero, source, 0);
  }
  assert(activeScope.load()->reserved == 8);
  for (unsigned i = 0; i < 12; ++i)
    assert(hero[i] == float(i + 1));
  assert(!std::memcmp(hero + 12, source, 12) && hero[15] == 1 &&
         source[3] == 0);
  msm2::writer_entry::stop();
  assert(!installed && bytesEqual(baseline));
  msm2::writer_entry::drain();
  assert(output.find("entry_pointer_candidate_mask=0x1") != std::string::npos &&
         output.find("true_entry_snapshot=1") != std::string::npos &&
         output.find("origin=pointer origin_register_or_offset=0x0") !=
             std::string::npos);
  auto stats = msm2::writer_entry::summary(expected.session);
  assert(stats.activated && stats.snapshots == 8 && stats.complete == 7 &&
         stats.xyzMatches == 7 && stats.candidate == 20);
  assert(msm2::writer_entry::summary(expected.session + 1).snapshots == 0);
  assert(evidence[0].full[20] == 0 && evidence[0].xyz[20] == 7 &&
         evidence[0].xyz[23] == 0);
  assert(output.find("same_origin_xyz_matches=7 stable_across_invocations=1") !=
         std::string::npos);
  assert(output.find("source_w_pattern=constant_observed source_w_bits=0x0 "
                     "destination_w_pattern=constant_observed "
                     "destination_w_bits=0x3f800000") != std::string::npos);
  puts("PASS: c14 actual concurrent/nested hardware maps retain seven XYZ "
       "mask=0x7 matches with independent constant W=1 and untouched source "
       "W=0; session summary counts eight entries/seven complete maps without "
       "c12 or cross-session contamination");
  VirtualFree(hero, 0, MEM_RELEASE);
  puts("PASS: c13 real hardware-store to true-entry invocation pairing on "
       "concurrent and nested owned calls; seven complete XYZ/W maps use "
       "captured register/immediate bits, generic target/wrong caller cannot "
       "mix tokens");
  puts("PASS: c13 active identity/generation/stale/close gates,8-snapshot hard "
       "cap, guarded pointer/stack evidence, exact candidate bits/metadata "
       "preservation and owned unhook restoration");
}
static bool noRead(std::uintptr_t, void *, std::size_t) { return false; }
static void guards() {
  msm2::writer_entry::Spec bad{};
  msm2::writer_entry::configure(bad, validate);
  assert(!configured);
  bad.verified = true;
  bad.base = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
  bad.imageSize = 0x4000000;
  bad.begin = 0x2cfc901;
  bad.end = 0x2cfc9b7;
  msm2::writer_entry::configure(bad, validate);
  assert(!configured);
  auto *memory = static_cast<unsigned char *>(
      VirtualAlloc(nullptr, 8192, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
  assert(memory);
  DWORD old{};
  assert(
      VirtualProtect(memory + 4096, 4096, PAGE_READWRITE | PAGE_GUARD, &old));
  unsigned char data[16]{};
  assert(!msm2::writer_entry::guardedRead(
      reinterpret_cast<std::uintptr_t>(memory + 4096), data, 16));
  MEMORY_BASIC_INFORMATION m{};
  assert(VirtualQuery(memory + 4096, &m, sizeof(m)) &&
         (m.Protect & PAGE_GUARD));
  assert(!msm2::writer_entry::guardedRead(
      reinterpret_cast<std::uintptr_t>(memory + 4090), data, 16));
  assert(VirtualProtect(memory, 4096, PAGE_NOACCESS, &old));
  assert(!msm2::writer_entry::guardedRead(
      reinterpret_cast<std::uintptr_t>(memory), data, 16));
  assert(!msm2::writer_entry::guardedRead(UINTPTR_MAX - 2, data, 16));
  VirtualFree(memory, 0, MEM_RELEASE);
  unsigned char direct[6]{0xe8, 0xfb, 0x0f, 0, 0, 0xc3};
  assert(std::string(msm2::writer_entry::callerRelation(
             direct, 6, 0x1000, 0x1005, 0x2000, noRead)) == "direct");
  unsigned char indirect[3]{0xff, 0xd0, 0xc3};
  assert(std::string(msm2::writer_entry::callerRelation(
             indirect, 3, 0x1000, 0x1002, 0x2000, noRead)) == "indirect");
  assert(std::string(msm2::writer_entry::callerRelation(
             direct, 6, 0x1000, 0x1005, 0x3000, noRead)) == "unresolved");
  unsigned char wrapper[16]{};
  auto base = reinterpret_cast<std::uintptr_t>(wrapper);
  wrapper[0] = 0xe8;
  std::int32_t callDelta = 3;
  std::memcpy(wrapper + 1, &callDelta, 4);
  wrapper[5] = 0xc3;
  wrapper[8] = 0xe9;
  std::int32_t jumpDelta = 32;
  std::memcpy(wrapper + 9, &jumpDelta, 4);
  assert(std::string(msm2::writer_entry::callerRelation(
             wrapper, 6, base, base + 5, base + 13 + 32,
             msm2::writer_entry::guardedRead)) == "wrapper");
  wrapper[8] = 0x90;
  assert(std::string(msm2::writer_entry::callerRelation(
             wrapper, 6, base, base + 5, base + 13 + 32,
             msm2::writer_entry::guardedRead)) == "unresolved");
  std::uint32_t sourceBits[4]{0, 0, 0, 0};
  float actual[4]{0, 1, 0, 1};
  assert(matchMask(sourceBits, actual, 15) == 5);
  assert(matchMask(sourceBits, actual, 3) == 1);
  assert(matchMask(sourceBits, actual, 0) == 0);
  std::uint32_t scalarBits[4]{};
  float laneStores[4]{10, 20, 30, 1};
  std::memcpy(scalarBits, laneStores + 1, 4);
  assert(laneMatches(scalarBits, laneStores, 15) == 2 &&
         laneMatches(scalarBits, laneStores, 0) == 0);
  puts("PASS: c13 exact build/boundary refusal, "
       "guard/noaccess/cross-page/null/overflow reads leave protection "
       "unchanged, exact/partial/no-observation candidate matching and "
       "conservative direct/indirect/unresolved chain classification");
}

static void cleanup() {
  float storage[32]{}, input[4]{1, 2, 3, 1};
  prepare(reinterpret_cast<void *>(ws_owned_entry_writer),
          reinterpret_cast<void *>(ws_owned_entry_return));
  expected.transform = reinterpret_cast<std::uintptr_t>(storage);
  assert(msm2::writer_entry::start(expected));
  assert(!ws_movement_entry_enabled);
  assert(msm2::writer_entry::activate());
  directCall[0] ^= 1;
  assert(!owned());
  msm2::writer_entry::stop();
  assert(!installed && bytesEqual(baseline));
  directCall[0] ^= 1;
  assert(!msm2::writer_entry::start(expected) && !installed);
  ++expected.session;
  nativeMs = GetTickCount64();
  assert(msm2::writer_entry::start(expected));
  assert(!ws_movement_entry_enabled);
  assert(msm2::writer_entry::activate());
  auto *active = activeScope.load();
  msm2::writer_entry::cancel();
  assert(!activeScope.load() && !ws_movement_entry_enabled && installed);
  assert(ws_owned_entry_caller(storage, input, 0) == 0x123455667788abcdull &&
         active->reserved == 0);
  msm2::writer_entry::stop();
  assert(!installed && bytesEqual(baseline));
  ++expected.session;
  nativeMs = GetTickCount64();
  assert(msm2::writer_entry::start(expected));
  assert(!ws_movement_entry_enabled);
  assert(msm2::writer_entry::activate());
  DWORD old{};
  assert(VirtualProtect(reinterpret_cast<void *>(target), 5,
                        PAGE_EXECUTE_READWRITE, &old));
  *reinterpret_cast<unsigned char *>(target) = 0x90;
  DWORD ignored{};
  assert(VirtualProtect(reinterpret_cast<void *>(target), 5, old, &ignored));
  FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void *>(target),
                        5);
  msm2::writer_entry::stop();
  assert(installed && failed &&
         *reinterpret_cast<unsigned char *>(target) == 0x90 &&
         !activeScope.load()); // foreign patch is not overwritten
  assert(VirtualProtect(reinterpret_cast<void *>(target), 5,
                        PAGE_EXECUTE_READWRITE, &old));
  std::memcpy(reinterpret_cast<void *>(target), patch.data(), 5);
  assert(VirtualProtect(reinterpret_cast<void *>(target), 5, old, &ignored));
  FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void *>(target),
                        5);
  msm2::writer_entry::stop();
  assert(!installed && bytesEqual(baseline));
  assert(output.find("foreign_or_code_changed=1") != std::string::npos);
  puts("PASS: c13 diagnostic cancellation is synchronous, owned unhook works "
       "even when caller integrity changes, foreign patches remain untouched, "
       "cleanup failure stays closed and trampoline remains safe for inflight "
       "tails");
}
static void accounting() {
  // Worker-only owned record fixture; cannot pass the public game gate.
  usedScopes = 4;
  for (unsigned scope = 0; scope < 4; ++scope) {
    auto &q = scopes[scope];
    q.index = scope;
    q.request.session = scope + 1;
    q.activated = scope != 3;
    q.reserved = scope < 2 ? 2 : scope == 2 ? 2 : 0;
  }
  float input[4]{10, 20, 30, 0};
  for (unsigned scope = 0; scope < 2; ++scope)
    for (unsigned j = 0; j < 2; ++j) {
      unsigned id = scope * perSession + j;
      auto &v = records[id].snapshot;
      v.id = id + 1;
      v.session = scope + 1;
      v.pointerMask = 1;
      v.stackRead = true;
      v.rsp = 0x1000;
      v.gpr[2] = 0x1028;
      v.stackLow = 0x800;
      v.stackHigh = 0x2000;
      v.target = 0x4000;
      std::memcpy(v.pointed[0].data(), input, 16);
      std::memcpy(v.stack.data(), input, 16);
      records[id].state = 1;
      auto &a = aggregates[id];
      a.mask = 15;
      a.events = 4;
      std::memcpy(a.values, input, 12);
      a.values[3] = 1;
      if (scope == 1 && j == 1)
        a.values[0] = 99;
      finish(id);
    }
  for (unsigned scope = 0; scope < 2; ++scope) {
    auto q = msm2::writer_entry::summary(scope + 1);
    assert(q.snapshots == 2 && q.complete == 2 && q.candidate == 20 &&
           q.xyzMatches == (scope ? 1u : 2u));
  }
  assert(evidence[0].xyz[23] == 2 && evidence[0].full[20] == 0 &&
         evidence[1].xyz[20] == 1);
  assert(output.find("same_origin_xyz_matches=2 stable_across_invocations=1") !=
         std::string::npos);
  auto &v = records[16].snapshot;
  v.id = 17;
  v.session = 3;
  records[16].state = 1;
  float targetPosition[4]{};
  msm2::writer_entry::storeComponent(17, 0, 0, targetPosition, 0, 0, 0);
  msm2::writer_entry::storeComponent(17, 0, 0, targetPosition, 0, 0, 0);
  auto &unreadable = records[17].snapshot;
  unreadable.id = 18;
  unreadable.session = 3;
  records[17].state = 1;
  msm2::writer_entry::observe(18, 1, 0, nullptr, nullptr, false, targetPosition,
                              0, 0, false);
  msm2::writer_entry::storeComponent(18, 0, 0, targetPosition, 0, 0, 0);
  assert(aggregates[17].rejected && aggregates[17].events == 0);
  msm2::writer_entry::drain();
  auto q = msm2::writer_entry::summary(3);
  assert(q.snapshots == 2 && q.complete == 0 && q.xyzMatches == 0 &&
         aggregates[16].rejected);
  q = msm2::writer_entry::summary(4);
  assert(!q.activated && q.snapshots == 0 && q.complete == 0);
  assert(output.find("session=1 capture_activated=1 true_entry_snapshots=2 "
                     "complete_invocation_maps=2 xyz_source_candidate=20 "
                     "xyz_match_count=2") != std::string::npos);
  assert(output.find("session=2 capture_activated=1 true_entry_snapshots=2 "
                     "complete_invocation_maps=2 xyz_source_candidate=20 "
                     "xyz_match_count=1") != std::string::npos);
  assert(output.find("session=3 capture_activated=1 true_entry_snapshots=2 "
                     "complete_invocation_maps=0") != std::string::npos);
  auto before = output.size();
  msm2::writer_entry::drain();
  assert(before == output.size());
  puts("PASS: c14 worker summaries isolate sessions, retain pointer/stack XYZ "
       "matches with W mismatch, reject duplicate/partial maps and invalidate "
       "stability after a later XYZ mismatch; drain is idempotent and "
       "zero-capture remains zero");
}
int main(int argc, char **argv) {
  if (argc > 1 && std::string(argv[1]) == "--accounting")
    accounting();
  else if (argc > 1 && std::string(argv[1]) == "--abi")
    abi();
  else if (argc > 1 && std::string(argv[1]) == "--pair")
    pairing();
  else if (argc > 1 && std::string(argv[1]) == "--cleanup")
    cleanup();
  else
    guards();
  auto f = std::fopen(argc > 1 && std::string(argv[1]) == "--pair"
                          ? "logs/windows_writer_entry_pair_synthetic.log"
                          : "logs/windows_writer_entry_other_synthetic.log",
                      "wb");
  assert(f);
  std::fwrite(output.data(), 1, output.size(), f);
  std::fclose(f);
}
