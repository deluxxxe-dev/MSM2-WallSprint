#define WIN32_LEAN_AND_MEAN
#include "NativeHooks/writer_entry_snapshot.hpp"
#include "Math/movement_delta.hpp"
#include "NativeHooks/rdx_source_contract.hpp"
#include "msm2/sdk_bridge.hpp"
#include <MinHook.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <bcrypt.h>
#include <capstone/capstone.h>
#include <cmath>
#include <cstring>
#include <windows.h>
namespace msm2 {
void writeLog(const char *, ...) __attribute__((format(printf, 1, 2)));
}
extern "C" {
void ws_movement_entry_thunk();
void *ws_movement_entry_original{};
std::atomic<unsigned> ws_movement_entry_enabled{0};
bool ws_guarded_call(void (*)(void *), void *);
}
namespace {
using namespace msm2;
constexpr unsigned windows = 8, perSession = 8, capacity = windows * perSession;
struct Scope {
  reset_trace::Request request{};
  std::atomic<unsigned> reserved{0};
  std::atomic<bool> active{false};
  unsigned index{};
  std::atomic<bool> activated{false}, finalized{false};
};
struct Snapshot {
  std::uint64_t id{}, session{}, generation{};
  DWORD tid{};
  std::uintptr_t hero{}, target{}, owner{}, rsp{}, ret{}, caller2{}, stackLow{},
      stackHigh{};
  double elapsed{};
  std::array<std::uint64_t, 16> gpr{};
  std::array<std::uint32_t, 64> xmm{};
  std::array<unsigned char, 64> stack{};
  std::array<std::array<unsigned char, 16>, 3> pointed{};
  float before[4]{};
  unsigned pointerMask{};
  bool stackRead{}, beforeRead{}, chainVerified{};
  ws::diag::ActorAxes basis{};
};
bool callStackFits(const Snapshot &v) noexcept;
struct Record {
  std::atomic<unsigned> state{0};
  Snapshot snapshot{};
};
struct Aggregate {
  unsigned mask{}, events{};
  float values[4]{}, after[4]{};
  bool printed{}, rejected{}, ordered{true};
  std::array<unsigned, 4> stores{};
};
std::array<Scope, windows> scopes;
std::array<Record, capacity> records;
std::array<Aggregate, capacity> aggregates;
struct Evidence {
  std::array<unsigned, 27> xyz{}, full{};
  unsigned complete{};
  std::array<std::uint32_t, 27> sourceW{};
  std::array<bool, 27> sourceWSeen{}, sourceWMixed{};
  std::uint32_t destinationW{};
  bool destinationWSeen{}, destinationWMixed{}, summarized{};
};
std::array<Evidence, windows> evidence;
ws::rdx::Writer contractWriter{};
ws::rdx::CallEffect writerCall{};
ws::rdx::CalleeAlias writerAlias{};
constexpr const char *calleeAliasHash =
    "2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d";
bool calleeAliasIdentity(std::uintptr_t t, std::uintptr_t b, std::uintptr_t e,
                         const char *hash) noexcept {
  return t == 0x2b23560 && b == 0x2b23560 && e == 0x2b235e9 && hash &&
         !std::strcmp(hash, calleeAliasHash);
}
std::array<unsigned char, ws::rdx::calleeByteCap> writerCallee{};
unsigned writerCalleeSize{};
char writerCalleeHash[65]{};
std::array<unsigned char, ws::rdx::readCallByteCap> readCallCode{};
bool writerCallCurrent() noexcept;
ws::rdx::Caller contractCaller{}, contractParent{};
ws::rdx::Relation contractProducer{};
std::array<unsigned char, 8192> producerCode{};
unsigned producerSize{}, producerBegin{}, producerEnd{};
bool contractConfigured{};
bool contractRead(std::uintptr_t, void *, std::size_t) noexcept;
std::atomic<Scope *> activeScope{nullptr};
std::atomic<unsigned> callbacks{0};
std::atomic<unsigned> usedScopes{0};
struct Local {
  std::uintptr_t rsp{};
  std::uint64_t id{};
};
thread_local std::array<Local, 8> local;
thread_local unsigned depth{};
std::uintptr_t imageBase{}, target{}, callReturn{}, callerStart{};
unsigned functionSize{};
std::size_t contractImageSize{};
std::array<unsigned char, 1024> baseline{}, patch{};
std::array<unsigned char, 5> directCall{};
std::array<unsigned char, 30> callerCode{};
unsigned callerCodeSize{};
char baselineHash[65]{};
writer_entry::Gate gate{};
bool configured{}, created{}, failed{};
std::atomic<bool> installed{false};
const char *chainKind = "unresolved";
bool chainDirect{};
bool digest(const unsigned char *data, unsigned count, char *text) {
  BCRYPT_ALG_HANDLE alg{};
  BCRYPT_HASH_HANDLE hash{};
  unsigned char object[512]{}, result[32]{};
  DWORD size{}, done{};
  bool ok = BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr,
                                        0) >= 0;
  if (ok)
    ok = BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH,
                           reinterpret_cast<PUCHAR>(&size), sizeof(size), &done,
                           0) >= 0 &&
         size <= sizeof(object);
  if (ok)
    ok = BCryptCreateHash(alg, &hash, object, size, nullptr, 0, 0) >= 0;
  if (ok)
    ok = BCryptHashData(hash, const_cast<PUCHAR>(data), count, 0) >= 0;
  if (ok)
    ok = BCryptFinishHash(hash, result, 32, 0) >= 0;
  if (hash)
    BCryptDestroyHash(hash);
  if (alg)
    BCryptCloseAlgorithmProvider(alg, 0);
  constexpr char h[] = "0123456789abcdef";
  for (unsigned i = 0; i < 32; ++i) {
    text[2 * i] = h[result[i] >> 4];
    text[2 * i + 1] = h[result[i] & 15];
  }
  text[64] = 0;
  return ok;
}
bool bytesEqual(const std::array<unsigned char, 1024> &expected) {
  unsigned char current[1024]{};
  return functionSize &&
         writer_entry::guardedRead(target, current, functionSize) &&
         !std::memcmp(current, expected.data(), functionSize);
}
bool writerCallCurrent() noexcept {
  if (!writerCall.instructionVerified || !writerCall.boundaryVerified ||
      !writerCall.noWrite || !writerCalleeSize)
    return false;
  DWORD64 module{};
  auto f =
      RtlLookupFunctionEntry(imageBase + writerCall.target, &module, nullptr);
  unsigned char code[ws::rdx::calleeByteCap]{};
  char hash[65]{};
  return f && module == imageBase && f->BeginAddress == writerCall.begin &&
         f->EndAddress == writerCall.end &&
         writer_entry::guardedRead(imageBase + writerCall.begin, code,
                                   writerCalleeSize) &&
         !std::memcmp(code, writerCallee.data(), writerCalleeSize) &&
         digest(code, writerCalleeSize, hash) &&
         !std::strcmp(hash, writerCalleeHash);
}
bool callStackFits(const Snapshot &v) noexcept {
  return !writerCall.relevant ||
         (contractWriter.callStackLow < 0 && v.rsp >= v.stackLow &&
          v.rsp <= v.stackHigh &&
          v.rsp - v.stackLow >= std::uint64_t(-contractWriter.callStackLow) &&
          contractWriter.callStackHigh >= 0 &&
          v.stackHigh - v.rsp >= std::uint64_t(contractWriter.callStackHigh));
}
bool transformWriteRegionsFit(const Snapshot &v) noexcept {
  return !writerAlias.transformWriteSeen ||
         (v.target <= UINTPTR_MAX - 0x110 && v.stackLow <= v.stackHigh &&
          (v.target + 0x110 <= v.stackLow || v.target >= v.stackHigh));
}
bool aliasRegionsFit(const Snapshot &v) noexcept {
  const unsigned span = writerAlias.transformWriteSeen ? 0x110 : 64;
  return transformWriteRegionsFit(v) &&
         (!writerAlias.usesGlobals ||
          (imageBase <= UINTPTR_MAX - contractImageSize &&
           v.target <= UINTPTR_MAX - span && v.stackLow <= v.stackHigh &&
           (v.stackHigh <= imageBase ||
            v.stackLow >= imageBase + contractImageSize) &&
           (v.target + span <= imageBase ||
            v.target >= imageBase + contractImageSize)));
}
bool callIntact() {
  unsigned char current[5]{};
  unsigned char body[30]{};
  return writer_entry::guardedRead(callReturn - 5, current, 5) &&
         !std::memcmp(current, directCall.data(), 5) &&
         (!callerCodeSize ||
          (writer_entry::guardedRead(callerStart, body, callerCodeSize) &&
           !std::memcmp(body, callerCode.data(), callerCodeSize)));
}
bool entryBranchOwned() {
  unsigned char code[5]{}, relay[14]{};
  std::int32_t delta{};
  if (!writer_entry::guardedRead(target, code, 5) || code[0] != 0xe9)
    return false;
  std::memcpy(&delta, code + 1, 4);
  auto destination = std::uintptr_t(std::int64_t(target + 5) + delta);
  if (!writer_entry::guardedRead(destination, relay, 14) || relay[0] != 0xff ||
      relay[1] != 0x25)
    return false;
  std::uint32_t displacement{};
  std::memcpy(&displacement, relay + 2, 4);
  std::uintptr_t to{};
  std::memcpy(&to, relay + 6, 8);
  return !displacement &&
         to == reinterpret_cast<std::uintptr_t>(ws_movement_entry_thunk);
}
bool owned() { return installed && bytesEqual(patch) && callIntact(); }
struct Chain {
  CONTEXT context{};
  std::uintptr_t ret{}, rsp{}, caller2{};
  bool ok{};
};
void unwindCaller(void *raw) {
  auto &c = *static_cast<Chain *>(raw);
  std::uintptr_t limit{}, top{};
  __asm__ volatile("movq %%gs:0x10,%0" : "=r"(limit));
  __asm__ volatile("movq %%gs:0x08,%0" : "=r"(top));
  if (c.rsp < limit || c.rsp > top || top - c.rsp < 8)
    return;
  std::uintptr_t ret{};
  if (!writer_entry::guardedRead(c.rsp, &ret, 8) || ret != c.ret)
    return;
  c.context.Rip = c.ret;
  c.context.Rsp = c.rsp + 8;
  DWORD64 module{};
  auto f = RtlLookupFunctionEntry(c.ret - 1, &module, nullptr);
  if (!f)
    return;
  void *handler{};
  DWORD64 frame{};
  RtlVirtualUnwind(UNW_FLAG_NHANDLER, module, c.ret, f, &c.context, &handler,
                   &frame, nullptr);
  MEMORY_BASIC_INFORMATION info{};
  if (c.context.Rsp <= c.rsp + 8 || c.context.Rsp > top ||
      c.context.Rsp - c.rsp > 0x10000 ||
      !VirtualQuery(reinterpret_cast<void *>(c.context.Rip), &info,
                    sizeof(info)) ||
      info.State != MEM_COMMIT ||
      (info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) ||
      !(info.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ |
                        PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))
    return;
  c.caller2 = c.context.Rip;
  c.ok = true;
}
void rawCapture(const writer_entry::Frame &f, const unsigned char *fx) {
  const auto rsp = reinterpret_cast<std::uintptr_t>(&f.ret);
  while (depth && local[depth - 1].rsp <= rsp)
    --depth;
  if (depth == local.size()) {
    depth = 0;
    return;
  }
  auto &token = local[depth++];
  token = {rsp, 0};
  auto *scope = activeScope.load(std::memory_order_acquire);
  if (!scope || !scope->active.load() || !gate)
    return;
  const auto &q = scope->request;
  if (f.rcx != q.transform || f.ret != callReturn ||
      scope->reserved.load() >= perSession || !gate(q) || !owned())
    return;
  Snapshot v{};
  v.session = q.session;
  v.generation = q.generation;
  v.owner = q.owner;
  v.hero = q.hero;
  v.target = f.rcx;
  v.rsp = rsp;
  v.ret = f.ret;
  v.tid = GetCurrentThreadId();
  v.basis = q.basis;
  auto tick = GetTickCount64();
  if (tick < q.initMs)
    return;
  v.elapsed = double(tick - q.initMs) / 1000.;
  std::uint64_t g[] = {f.rax, f.rcx, f.rdx, f.rbx, rsp,   f.rbp, f.rsi, f.rdi,
                       f.r8,  f.r9,  f.r10, f.r11, f.r12, f.r13, f.r14, f.r15};
  std::memcpy(v.gpr.data(), g, sizeof(g));
  std::memcpy(v.xmm.data(), fx + 160, 256);
  std::uintptr_t lo{}, hi{};
  __asm__ volatile("movq %%gs:0x10,%0" : "=r"(lo));
  __asm__ volatile("movq %%gs:0x08,%0" : "=r"(hi));
  v.stackLow = lo;
  v.stackHigh = hi;
  if (rsp >= lo && rsp <= hi && hi - rsp >= 0x28 + 64)
    v.stackRead = writer_entry::guardedRead(rsp + 0x28, v.stack.data(), 64);
  const std::uintptr_t pointers[] = {f.rdx, f.r8, f.r9};
  for (unsigned i = 0; i < 3; ++i)
    if (writer_entry::guardedRead(pointers[i], v.pointed[i].data(), 16))
      v.pointerMask |= 1u << i;
  v.beforeRead = writer_entry::guardedRead(f.rcx + 0x30, v.before, 16);
  Chain c{};
  c.rsp = rsp;
  c.ret = f.ret;
  c.context.ContextFlags = CONTEXT_FULL;
  c.context.Rax = f.rax;
  c.context.Rcx = f.rcx;
  c.context.Rdx = f.rdx;
  c.context.Rbx = f.rbx;
  c.context.Rbp = f.rbp;
  c.context.Rsi = f.rsi;
  c.context.Rdi = f.rdi;
  c.context.R8 = f.r8;
  c.context.R9 = f.r9;
  c.context.R10 = f.r10;
  c.context.R11 = f.r11;
  c.context.R12 = f.r12;
  c.context.R13 = f.r13;
  c.context.R14 = f.r14;
  c.context.R15 = f.r15;
  if (ws_guarded_call(unwindCaller, &c) && c.ok) {
    v.caller2 = c.caller2;
    v.chainVerified = true;
  }
  if (!scope->active.load() ||
      scope != activeScope.load(std::memory_order_acquire) || !gate(q) ||
      !owned())
    return;
  auto count = scope->reserved.load();
  do {
    if (count >= perSession)
      return;
  } while (!scope->reserved.compare_exchange_weak(count, count + 1));
  auto slot = scope->index * perSession + count;
  v.id = slot + 1;
  auto &record = records[slot];
  record.snapshot = v;
  record.state.store(1, std::memory_order_release);
  token.id = v.id;
}
unsigned matchMask(const std::uint32_t *candidate, const float *actual,
                   unsigned observed) {
  unsigned mask = 0;
  for (unsigned i = 0; i < 4; ++i) {
    std::uint32_t bits{};
    std::memcpy(&bits, actual + i, 4);
    if ((observed & (1u << i)) && candidate[i] == bits)
      mask |= 1u << i;
  }
  return mask;
}
unsigned laneMatches(const std::uint32_t *values, const float *stores,
                     unsigned observed) {
  unsigned result = 0;
  for (unsigned lane = 0; lane < 4; ++lane)
    for (unsigned component = 0; component < 4; ++component) {
      std::uint32_t bits{};
      std::memcpy(&bits, stores + component, 4);
      if ((observed & (1u << component)) && values[lane] == bits)
        result |= 1u << (lane * 4 + component);
    }
  return result;
}
bool contractRead(std::uintptr_t p, void *out, std::size_t n) noexcept {
  MEMORY_BASIC_INFORMATION m{};
  if (!VirtualQuery(reinterpret_cast<void *>(p), &m, sizeof(m)) ||
      !(m.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE |
                     PAGE_EXECUTE_WRITECOPY)))
    return false;
  // Only direct jump bytes in the admitted image; no pointer chase.
  return p >= imageBase && p - imageBase < contractImageSize && n <= 5 &&
         n <= contractImageSize - (p - imageBase) &&
         writer_entry::guardedRead(p, out, n);
}
void finish(unsigned slot) {
  auto &record = records[slot];
  if (record.state.load(std::memory_order_acquire) != 1)
    return;
  auto &a = aggregates[slot];
  if (a.printed)
    return;
  a.printed = true;
  const auto &v = record.snapshot;
  writeLog(
      "MOVEMENT_WRITER_ENTRY_SNAPSHOT session=%llu invocation_id=%llu "
      "thread_id=%lu elapsed=%.6f writer_rva=0x2cfc900 target_transform=0x%llx "
      "return_rva=0x%llx caller2_observed_rva=0x%llx rcx=0x%llx rdx=0x%llx "
      "r8=0x%llx r9=0x%llx rsp=0x%llx selected_gpr_state=16_raw_registers "
      "selected_xmm_state=16_raw_XMM "
      "stack_arg_snapshot=64_bytes_at_entry_rsp_plus_0x28 stack_read=%u "
      "pointer_read_mask=0x%x entry_target_verified=1 "
      "code_integrity_verified=1 true_entry_snapshot=1 position_before_read=%u "
      "writable_source_proven=0",
      (unsigned long long)v.session, (unsigned long long)v.id, v.tid, v.elapsed,
      (unsigned long long)v.target, (unsigned long long)(v.ret - imageBase),
      (unsigned long long)(v.caller2 >= imageBase ? v.caller2 - imageBase : 0),
      (unsigned long long)v.gpr[1], (unsigned long long)v.gpr[2],
      (unsigned long long)v.gpr[8], (unsigned long long)v.gpr[9],
      (unsigned long long)v.rsp, unsigned(v.stackRead), v.pointerMask,
      unsigned(v.beforeRead));
  for (unsigned i = 0; i < 4; ++i)
    writeLog("MOVEMENT_ENTRY_GPR_RAW invocation_id=%llu group=%u "
             "bits=(0x%llx,0x%llx,0x%llx,0x%llx)",
             (unsigned long long)v.id, i, (unsigned long long)v.gpr[i * 4],
             (unsigned long long)v.gpr[i * 4 + 1],
             (unsigned long long)v.gpr[i * 4 + 2],
             (unsigned long long)v.gpr[i * 4 + 3]);
  auto &e = evidence[slot / perSession];
  const bool complete = a.mask == 15 && !a.rejected;
  if (complete) {
    ++e.complete;
    std::uint32_t w{};
    std::memcpy(&w, a.values + 3, 4);
    if (e.destinationWSeen && e.destinationW != w)
      e.destinationWMixed = true;
    e.destinationWSeen = true;
    e.destinationW = w;
  }
  unsigned sourceMask = 0, pointerMask = 0, stableMask = 0;
  for (unsigned id = 0; id < 27; ++id) {
    std::uint32_t bits[4]{};
    bool read = true;
    const char *origin = "XMM";
    unsigned offset = id;
    if (id < 16)
      std::memcpy(bits, v.xmm.data() + id * 4, 16);
    else if (id < 20) {
      origin = "GPR";
      offset = (id - 16) * 4;
      for (unsigned n = 0; n < 4; ++n)
        bits[n] = std::uint32_t(v.gpr[offset + n]);
    } else if (id < 23) {
      origin = "pointer";
      offset = id - 20;
      read = v.pointerMask & (1u << offset);
      std::memcpy(bits, v.pointed[offset].data(), 16);
    } else {
      origin = "stack";
      offset = 0x28 + (id - 23) * 16;
      read = v.stackRead;
      std::memcpy(bits, v.stack.data() + (id - 23) * 16, 16);
    }
    auto mask = read ? matchMask(bits, a.values, a.mask) : 0;
    auto before = read && v.beforeRead ? matchMask(bits, v.before, 15) : 0;
    float values[4]{};
    std::memcpy(values, bits, 16);
    bool finite = true;
    for (float f : values)
      finite &= std::isfinite(f);
    unsigned deltaMask = 0;
    bool moved = false;
    if (read && v.beforeRead && a.mask == 15 && finite)
      for (unsigned k = 0; k < 3; ++k) {
        float delta = a.values[k] - v.before[k];
        moved |= std::isfinite(delta) && std::fabs(delta) > 1e-5f;
        if (std::isfinite(delta) && std::fabs(values[k] - delta) <= 1e-5f)
          deltaMask |= 1u << k;
      }
    const char *relation = before == 15                 ? "current_position"
                           : a.mask == 15 && mask == 15 ? "final_position"
                           : deltaMask == 7 && moved    ? "delta_like"
                           : mask                       ? "partial"
                                                        : "none";
    bool matched = mask == 15 && a.mask == 15;
    if (matched) {
      ++e.full[id];
      sourceMask |= 1u << (id % 32);
      if (id >= 20 && id < 23)
        pointerMask |= 1u << (id - 20);
    }
    const bool xyzMatched = complete && read && (mask & 7) == 7;
    if (xyzMatched) {
      ++e.xyz[id];
      sourceMask |= 1u << id;
      if (id >= 20 && id < 23)
        pointerMask |= 1u << (id - 20);
      if (e.sourceWSeen[id] && e.sourceW[id] != bits[3])
        e.sourceWMixed[id] = true;
      e.sourceWSeen[id] = true;
      e.sourceW[id] = bits[3];
    }
    const bool xyzStable = e.xyz[id] >= 2 && e.xyz[id] == e.complete;
    if (xyzStable)
      stableMask |= 1u << id;
    writeLog("MOVEMENT_ENTRY_SOURCE_CANDIDATE session=%llu invocation_id=%llu "
             "candidate_id=%u origin=%s origin_register_or_offset=0x%x "
             "read_valid=%u value4=(%.9g,%.9g,%.9g,%.9g) "
             "raw_bits=(0x%x,0x%x,0x%x,0x%x) relationship=%s "
             "exact_store_match_mask=0x%x observed_store_mask=0x%x "
             "lane_to_store_match_matrix=0x%x "
             "same_origin_full_matches=%u same_origin_xyz_matches=%u "
             "stable_across_invocations=%u xyz_stable_across_invocations=%u "
             "stability_scope=observed_session_prefix_XYZ_bit_matches "
             "storage_provenance_stable=0 "
             "stable_across_surfaces=0 "
             "typed_semantics=unknown writable_source_proven=0 "
             "reason=entry_value_correlation_NOT_causal_or_exclusive_proof",
             (unsigned long long)v.session, (unsigned long long)v.id, id,
             origin, offset, unsigned(read), values[0], values[1], values[2],
             values[3], bits[0], bits[1], bits[2], bits[3], relation, mask,
             a.mask, read ? laneMatches(bits, a.values, a.mask) : 0, e.full[id],
             e.xyz[id], unsigned(xyzStable), unsigned(xyzStable));
  }
  auto d = !v.beforeRead || !a.events
               ? ws::movement::Delta{}
               : ws::movement::decompose(
                     {v.before[0], v.before[1], v.before[2]},
                     {a.after[0], a.after[1], a.after[2]}, v.basis, 1);
  writeLog(
      "MOVEMENT_WRITER_INVOCATION_MAP session=%llu invocation_id=%llu "
      "thread_id=%lu target_transform=0x%llx entry_return_rva=0x%llx "
      "store_30_seen=%u store_34_seen=%u store_38_seen=%u store_3c_seen=%u "
      "position_before=(%.9g,%.9g,%.9g,%.9g) "
      "position_after=(%.9g,%.9g,%.9g,%.9g) "
      "exact_component_values=(%.9g,%.9g,%.9g,%.9g) "
      "entry_source_match_mask=0x%x entry_pointer_candidate_mask=0x%x "
      "source_stable=%u "
      "stability_scope=observed_XYZ_match_pattern_NOT_storage_lifetime "
      "position_before_read=%u position_after_read=%u "
      "delta_valid=%u delta_normal=%.9g delta_forward=%.9g delta_right=%.9g "
      "events=%u reason=%s writable_source_proven=0 Position_writes_by_mod=0",
      (unsigned long long)v.session, (unsigned long long)v.id, v.tid,
      (unsigned long long)v.target, (unsigned long long)(v.ret - imageBase),
      unsigned(a.mask & 1 ? 1 : 0), unsigned(a.mask & 2 ? 1 : 0),
      unsigned(a.mask & 4 ? 1 : 0), unsigned(a.mask & 8 ? 1 : 0), v.before[0],
      v.before[1], v.before[2], v.before[3], a.after[0], a.after[1], a.after[2],
      a.after[3], a.values[0], a.values[1], a.values[2], a.values[3],
      sourceMask, pointerMask, unsigned(stableMask != 0),
      unsigned(v.beforeRead), unsigned(a.events > 0), unsigned(d.valid),
      d.normal, d.forward, d.right, a.events,
      a.mask == 15 ? "four_same_invocation_post_store_components_NOT_atomic_"
                     "return_or_causal_proof"
                   : "partial_or_no_store_capture");
  bool samePointer = v.stackRead && (v.pointerMask & 1) &&
                     !std::memcmp(v.stack.data(), v.pointed[0].data(), 16);
  auto storage = ws::rdx::storage(v.rsp, v.gpr[2], v.stackLow, v.stackHigh,
                                  v.stackRead && (v.pointerMask & 1),
                                  samePointer, contractCaller);
  bool distinctTarget =
      v.target <= UINTPTR_MAX - 64 &&
      (v.target + 64 <= v.stackLow || v.target >= v.stackHigh);
  bool sites = true;
  for (unsigned k = 0; k < 4; ++k)
    sites &= a.stores[k] == contractWriter.stores[k];
  unsigned xyz = 0;
  if ((v.pointerMask & 1) && complete) {
    std::uint32_t b[4]{};
    std::memcpy(b, v.pointed[0].data(), 16);
    xyz = matchMask(b, a.values, 15) & 7;
  }
  const bool calleeCodeVerified =
      writerAlias.authenticated && writerAlias.resolved &&
      writerAlias.sourceNoWrite && writerAlias.positionProtected &&
      writerAlias.relevantEffectsResolved &&
      calleeAliasIdentity(writerCall.target, writerCall.begin, writerCall.end,
                          writerCalleeHash) &&
      writerCall.noWrite && writerCallCurrent();
  const bool callCodeVerified = !writerCall.relevant || calleeCodeVerified;
  const bool callStackVerified = callStackFits(v);
  const bool globalRegionsVerified = aliasRegionsFit(v);
  bool causal = callCodeVerified && callStackVerified &&
                globalRegionsVerified && contractConfigured &&
                contractWriter.branchAware && contractWriter.bounded &&
                contractWriter.exactSites && contractWriter.allRelevant &&
                contractWriter.xyz && complete && a.ordered && sites &&
                storage.sameBytes && distinctTarget && xyz == 7;
  std::uint32_t w{};
  std::memcpy(&w, a.values + 3, 4);
  writeLog(
      "MOVEMENT_WRITER_CALL_INTEGRITY session=%llu invocation_id=%llu "
      "relevant_call=%u current_code_verified=%u stack_footprint_verified=%u "
      "writer_entry_stack_low_offset=%lld global_regions_verified=%u "
      "transform_write_regions_verified=%u "
      "writable_source_proven=0",
      (unsigned long long)v.session, (unsigned long long)v.id,
      unsigned(writerCall.relevant), unsigned(calleeCodeVerified),
      unsigned(callStackVerified), (long long)contractWriter.callStackLow,
      unsigned(globalRegionsVerified), unsigned(transformWriteRegionsFit(v)));
  bool wCausal =
      (!writerCall.relevant || (storage.sameBytes && distinctTarget)) &&
      callCodeVerified && callStackVerified && globalRegionsVerified &&
      contractConfigured && contractWriter.branchAware &&
      contractWriter.bounded && contractWriter.exactSites && contractWriter.w &&
      complete && a.ordered && sites &&
      (!contractWriter.wConstant || w == contractWriter.wBits);
  writeLog(
      "MOVEMENT_RDX_XYZ_PROOF session=%llu invocation_id=%llu "
      "writer_rva=0x2cfc900 source_register=RDX source_offsets=0,4,8 "
      "position_offsets=0x30,0x34,0x38 "
      "component_paths=%s "
      "static_prefix_proven=%u prefix_end_rva=0x%llx "
      "xyz_causality_proven=%u entry_store_xyz_match_mask=0x%x "
      "scope=entry_through_sampled_stores_NOT_whole_CFG "
      "writable_source_proven=0 reason=%s",
      (unsigned long long)v.session, (unsigned long long)v.id,
      contractWriter.xyz
          ? "all_relevant_local_branch_paths_RDX_0_4_8_to_Position_30_34_38"
          : "unresolved",
      unsigned(contractWriter.xyz),
      (unsigned long long)(target - imageBase + contractWriter.prefixEnd),
      unsigned(causal), xyz,
      !contractConfigured   ? "contract_not_admitted"
      : !contractWriter.xyz ? contractWriter.reason
      : !callCodeVerified   ? "callee_code_identity_changed_or_unavailable"
      : !callStackVerified  ? "call_stack_footprint_outside_entry_TEB_bounds"
      : !globalRegionsVerified
          ? "RIP_global_not_excluded_from_protected_regions"
      : !complete          ? "partial_or_rejected_invocation"
      : !a.ordered         ? "store_order_or_missing_first_event"
      : !sites             ? "store_site_mismatch"
      : !storage.sameBytes ? "bounded_equal_RDX_stack_snapshot_missing"
      : !distinctTarget    ? "stack_target_alias_not_excluded"
      : xyz != 7           ? "entry_XYZ_store_bits_mismatch"
                 : "decoded_causal_path_and_same_invocation_bits_verified");
  writeLog(
      "MOVEMENT_POSITION_W_PROOF session=%llu invocation_id=%llu "
      "store_rva=0x2cfc945 decoded_store_offset=0x%x decoded_site_verified=%u "
      "source_kind=%s source_value_bits=0x%x "
      "source_value_is_constant=%u source_value_valid=%u "
      "destination_value_valid=%u source_register_id=%d source_offset=%lld "
      "destination_value_bits=0x%x causality_proven=%u "
      "homogeneous_coordinate_semantics=unknown reason=%s",
      (unsigned long long)v.session, (unsigned long long)v.id,
      contractWriter.stores[3], unsigned(contractWriter.prefix && sites),
      contractWriter.wKind, contractWriter.wConstant ? contractWriter.wBits : w,
      unsigned(contractWriter.wConstant),
      unsigned(contractWriter.wConstant || (a.mask & 8)),
      unsigned((a.mask & 8) != 0), contractWriter.wSourceReg,
      (long long)contractWriter.wSourceOffset, w, unsigned(wCausal),
      !contractConfigured  ? "contract_not_admitted"
      : !contractWriter.w  ? contractWriter.reason
      : !callCodeVerified  ? "callee_code_identity_changed_or_unavailable"
      : !callStackVerified ? "call_stack_footprint_outside_entry_TEB_bounds"
      : !globalRegionsVerified
          ? "RIP_global_not_excluded_from_protected_regions"
      : !complete ? "partial_or_rejected_invocation"
      : !sites    ? "store_site_mismatch"
      : !wCausal  ? "executed_W_bits_do_not_match_decoded_constant"
                  : "decoded_store_operand_and_invocation_verified_NOT_"
                    "coordinate_type");
  writeLog(
      "MOVEMENT_RDX_STORAGE_PROOF session=%llu invocation_id=%llu "
      "call_rva=0x351eeab rdx_relation=%s stack_object_begin=0x%llx "
      "stack_object_size=16 current_thread_TEB_bounded=%u "
      "pointer_stack_bytes_equal=%u static_RDX_local_assignment=%u "
      "lifetime_scope=%s caller_owned=%u ownership_exclusive=0 reason=%s",
      (unsigned long long)v.session, (unsigned long long)v.id,
      storage.bounded ? "entry_rsp_plus_0x28_equals_precall_rsp_plus_0x20"
                      : "unproven",
      (unsigned long long)(storage.bounded ? v.gpr[2] : 0),
      unsigned(storage.bounded), unsigned(storage.sameBytes),
      unsigned(contractCaller.rdxLocal),
      storage.callerOwned
          ? "allocated_caller_frame_until_return_NOT_exclusive_effect"
          : "current_invocation_snapshot_only_owner_unproven",
      unsigned(contractConfigured && storage.callerOwned && distinctTarget),
      contractCaller.reason);
  unsigned char liveProducer[8192]{};
  bool producerIntact =
      producerSize &&
      writer_entry::guardedRead(imageBase + producerBegin, liveProducer,
                                producerSize) &&
      !std::memcmp(liveProducer, producerCode.data(), producerSize);
  auto relation = producerIntact ? ws::rdx::relation(liveProducer, producerSize,
                                                     imageBase + producerBegin,
                                                     imageBase + 0x351a020,
                                                     callerStart, contractRead)
                                 : ws::rdx::Relation{};
  writeLog(
      "MOVEMENT_RDX_PRODUCER_PROOF session=%llu invocation_id=%llu "
      "storage=entry_rsp_plus_0x28 producer_rva=0x%llx "
      "producer_function_begin=0x%x producer_function_end=0x%x "
      "producer_relation_to_0x351A020=%s static_anchored_relation=%s "
      "relation_reaches_tiny_caller=%u "
      "anchored_call_rva=0x%llx direct_target_rva=0x%llx next_edge_rva=0x%llx "
      "exact_jump_hops=%u producer_code_intact_at_worker=%u "
      "code_scope=startup_and_worker_NOT_atomic_entry "
      "runtime_stack_verified=%u construction_kind=unknown "
      "producer_x_rva=0x%llx producer_y_rva=0x%llx producer_z_rva=0x%llx "
      "caller_first_barrier_rva=0x%llx parent_candidate_x_rva=0x%llx "
      "parent_candidate_y_rva=0x%llx parent_candidate_z_rva=0x%llx "
      "parent_first_barrier_rva=0x%llx parent_slice_reason=%s "
      "parent_candidate_is_causal=0 "
      "uses_current_position=unknown uses_dt=unknown "
      "uses_direction_like_vector=unknown uses_speed_like_scalar=unknown "
      "source_lifetime_exclusive=0 downstream_conflicts_proven_absent=0 "
      "writable_source_proven=0 reason=%s",
      (unsigned long long)v.session, (unsigned long long)v.id,
      (unsigned long long)(contractCaller.constructed
                               ? callerStart - imageBase +
                                     contractCaller.producers[0] - 1
                               : 0),
      producerBegin, producerEnd,
      v.chainVerified && v.caller2 == imageBase + 0x351a020 ? relation.kind
                                                            : "unresolved",
      relation.kind,
      unsigned(relation.reaches && v.chainVerified &&
               v.caller2 == imageBase + 0x351a020),
      (unsigned long long)(relation.call >= imageBase
                               ? relation.call - imageBase
                               : 0),
      (unsigned long long)(relation.target >= imageBase
                               ? relation.target - imageBase
                               : 0),
      (unsigned long long)(relation.next >= imageBase
                               ? relation.next - imageBase
                               : 0),
      relation.hops, unsigned(producerIntact), unsigned(v.chainVerified),
      (unsigned long long)(contractCaller.producers[0]
                               ? callerStart - imageBase +
                                     contractCaller.producers[0] - 1
                               : 0),
      (unsigned long long)(contractCaller.producers[1]
                               ? callerStart - imageBase +
                                     contractCaller.producers[1] - 1
                               : 0),
      (unsigned long long)(contractCaller.producers[2]
                               ? callerStart - imageBase +
                                     contractCaller.producers[2] - 1
                               : 0),
      (unsigned long long)(contractCaller.prefix ? 0
                                                 : callerStart - imageBase +
                                                       contractCaller.barrier),
      (unsigned long long)(contractParent.producers[0]
                               ? producerBegin + contractParent.producers[0] - 1
                               : 0),
      (unsigned long long)(contractParent.producers[1]
                               ? producerBegin + contractParent.producers[1] - 1
                               : 0),
      (unsigned long long)(contractParent.producers[2]
                               ? producerBegin + contractParent.producers[2] - 1
                               : 0),
      (unsigned long long)(contractParent.prefix
                               ? 0
                               : producerBegin + contractParent.barrier),
      contractParent.reason,
      producerIntact ? relation.reason
                     : "producer_span_not_captured_or_changed");
  writeLog(
      "MOVEMENT_CALLER_CHAIN_RUNTIME session=%llu invocation_id=%llu "
      "return_rva=0x%llx caller2_observed_rva=0x%llx relation=%s "
      "static_direct_verified=%u runtime_stack_verified=%u reason=%s",
      (unsigned long long)v.session, (unsigned long long)v.id,
      (unsigned long long)(v.ret - imageBase),
      (unsigned long long)(v.caller2 >= imageBase ? v.caller2 - imageBase : 0),
      (v.chainVerified && v.caller2 == imageBase + 0x351a020) ? chainKind
                                                              : "unresolved",
      unsigned(chainDirect && v.caller2 == imageBase + 0x351a020),
      unsigned(v.chainVerified),
      v.chainVerified ? "entry_return_and_bounded_caller_unwind_observed_"
                        "static_relation_separate"
                      : "caller_unwind_unavailable");
}
} // namespace
extern "C" void ws_movement_entry_capture(const msm2::writer_entry::Frame *f,
                                          const unsigned char *fx) noexcept {
  auto error = GetLastError();
  ++callbacks;
  rawCapture(*f, fx);
  --callbacks;
  SetLastError(error);
}
namespace msm2::writer_entry {
bool guardedRead(std::uintptr_t p, void *out, std::size_t n) noexcept {
  MEMORY_BASIC_INFORMATION m{};
  if (!p || !n || p > UINTPTR_MAX - n ||
      !VirtualQuery(reinterpret_cast<void *>(p), &m, sizeof(m)) ||
      m.State != MEM_COMMIT || (m.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
    return false;
  auto base = reinterpret_cast<std::uintptr_t>(m.BaseAddress);
  return p >= base && p - base <= m.RegionSize &&
         n <= m.RegionSize - (p - base) && readMemory(p, out, n);
}
const char *
callerRelation(const unsigned char *bytes, std::size_t size,
               std::uintptr_t begin, std::uintptr_t ret, std::uintptr_t caller,
               bool (*read)(std::uintptr_t, void *, std::size_t)) noexcept {
  if (!bytes || !size || size > 8192 || ret <= begin || ret - begin > size)
    return "unresolved";
  csh h{};
  if (cs_open(CS_ARCH_X86, CS_MODE_64, &h) != CS_ERR_OK)
    return "unresolved";
  cs_option(h, CS_OPT_DETAIL, CS_OPT_ON);
  cs_insn *ins{};
  auto count = cs_disasm(h, bytes, size, begin, 0, &ins);
  std::uintptr_t end = begin;
  bool complete = true;
  const char *result = "unresolved";
  for (std::size_t i = 0; i < count; ++i) {
    auto &p = ins[i];
    if (!p.id || !p.detail || p.address != end) {
      complete = false;
      break;
    }
    end += p.size;
    if (end != ret || p.id != X86_INS_CALL)
      continue;
    auto &x = p.detail->x86;
    if (x.op_count != 1)
      continue;
    if (x.operands[0].type == X86_OP_REG || x.operands[0].type == X86_OP_MEM)
      result = "indirect";
    else if (x.operands[0].type == X86_OP_IMM) {
      auto dest = std::uintptr_t(x.operands[0].imm);
      if (dest == caller)
        result = "direct";
      else {
        unsigned char jump[5]{};
        std::int32_t delta{};
        if (read && read(dest, jump, 5) && jump[0] == 0xe9) {
          std::memcpy(&delta, jump + 1, 4);
          if (std::int64_t(dest + 5) + delta == std::int64_t(caller))
            result = "wrapper";
        }
      }
    }
  }
  if (ins)
    cs_free(ins, count);
  cs_close(&h);
  return complete && end == begin + size ? result : "unresolved";
}
void configure(const Spec &s, Gate validate) noexcept {
  if (configured || created)
    return;
  const char *reason = "exact_build_boundary_hash_or_direct_call_failed";
  unsigned char code[1024]{};
  char hash[65]{};
  DWORD64 image{};
  auto f = s.verified && s.base && s.imageSize > 0x351eeb0
               ? RtlLookupFunctionEntry(s.base + 0x2cfc900, &image, nullptr)
               : nullptr;
  bool ok = s.verified && s.decoded && s.directCall && validate &&
            s.begin == 0x2cfc900 && s.end == 0x2cfc9b7 &&
            s.callerBegin == 0x351eea7 && s.callerEnd == 0x351eec5 &&
            s.original && s.hash && f && image == s.base &&
            f->BeginAddress == s.begin && f->EndAddress == s.end &&
            guardedRead(s.base + s.begin, code, s.end - s.begin) &&
            !std::memcmp(code, s.original, s.end - s.begin) &&
            digest(code, s.end - s.begin, hash) && !std::strcmp(hash, s.hash);
  unsigned char call[5]{};
  std::int32_t delta{};
  ok = ok && guardedRead(s.base + 0x351eeab, call, 5) && call[0] == 0xe8;
  if (ok) {
    std::memcpy(&delta, call + 1, 4);
    ok = std::int64_t(s.base + 0x351eeb0) + delta ==
         std::int64_t(s.base + s.begin);
  }
  unsigned char callerBytes[30]{};
  char callerHash[65]{};
  DWORD64 callerImage{};
  auto callerFunction =
      ok ? RtlLookupFunctionEntry(s.base + 0x351eeaf, &callerImage, nullptr)
         : nullptr;
  ok = ok && s.callerOriginal && s.callerHash && callerFunction &&
       callerImage == s.base && callerFunction->BeginAddress == s.callerBegin &&
       callerFunction->EndAddress == s.callerEnd &&
       guardedRead(s.base + s.callerBegin, callerBytes, sizeof(callerBytes)) &&
       !std::memcmp(callerBytes, s.callerOriginal, sizeof(callerBytes)) &&
       digest(callerBytes, sizeof(callerBytes), callerHash) &&
       !std::strcmp(callerHash, s.callerHash);
  if (ok) {
    imageBase = s.base;
    target = s.base + s.begin;
    functionSize = s.end - s.begin;
    callReturn = s.base + 0x351eeb0;
    callerStart = s.base + s.callerBegin;
    std::memcpy(baseline.data(), code, functionSize);
    std::memcpy(directCall.data(), call, 5);
    std::memcpy(baselineHash, hash, 65);
    gate = validate;
    chainKind = s.caller2Relation;
    chainDirect = s.caller2Direct;
    callerCodeSize = 30;
    std::memcpy(callerCode.data(), callerBytes, 30);
    contractImageSize = s.imageSize;
    const std::array<unsigned, 4> sites{0x36, 0x3c, 0x42, 0x45};
    writerCall = ws::rdx::exactCall(code, functionSize, s.begin, s.imageSize,
                                    0x27, 0x2c);
    bool calleeCurrent = false;
    if (writerCall.instructionVerified && writerCall.target == 0x2b23560) {
      DWORD64 calleeImage{};
      auto cf = RtlLookupFunctionEntry(s.base + writerCall.target, &calleeImage,
                                       nullptr);
      if (cf && calleeImage == s.base) {
        writerCall.begin = cf->BeginAddress;
        writerCall.end = cf->EndAddress;
        const auto cb = writerCall.begin, ce = writerCall.end;
        // This task does not authorize inspecting the deferred producer or
        // recursive/overlapping writer code, even if a call unexpectedly lands
        // there.
        if (cb == 0x2b23560 && ce == 0x2b235e9 && ce <= s.imageSize &&
            ce - cb <= writerCallee.size() &&
            !(cb <= 0x351eb00 && 0x351eb00 < ce) &&
            !(cb < s.end && s.begin < ce)) {
          writerCalleeSize = unsigned(ce - cb);
          unsigned char recheck[ws::rdx::calleeByteCap]{};
          calleeCurrent =
              guardedRead(s.base + cb, writerCallee.data(), writerCalleeSize) &&
              digest(writerCallee.data(), writerCalleeSize, writerCalleeHash) &&
              guardedRead(s.base + cb, recheck, writerCalleeSize) &&
              !std::memcmp(recheck, writerCallee.data(), writerCalleeSize);
          calleeCurrent =
              calleeCurrent &&
              calleeAliasIdentity(writerCall.target, cb, ce, writerCalleeHash);
          if (calleeCurrent)
            ws::rdx::classifyCallee(writerCall, writerCallee.data(),
                                    writerCalleeSize, cb, ce);
          else
            writerCall.reason =
                "callee_guarded_current_code_hash_or_recheck_failed";
        } else
          writerCall.reason =
              "callee_exact_boundary_cap_overlap_or_deferred_producer_refused";
      } else
        writerCall.reason = "callee_in_image_runtime_function_boundary_missing";
    }
    writerAlias.code = writerCallee.data();
    writerAlias.size = writerCalleeSize;
    writerAlias.imageSize = s.imageSize;
    writerAlias.authenticated = calleeCurrent;
    writerAlias.exactWrite.site = 0x2b2358e;
    writerAlias.edi.enabled =
        calleeCurrent; // c20 diagnostic-only opt-in after exact current identity.
    writerAlias.readDependency.site = calleeCurrent ? 0x2b23578 : 0;
    writerAlias.readDependency.test.site = calleeCurrent ? 0x2b235be : 0;
    auto &nestedReadCall = writerAlias.readDependency.call;
    nestedReadCall.site = calleeCurrent ? 0x2b235d4 : 0;
    if (calleeCurrent) {
      nestedReadCall.identity = ws::rdx::exactCall(writerCallee.data(),writerCalleeSize,
          writerCall.begin,s.imageSize,0x74,0x79);
      auto &nc=nestedReadCall.identity;
      struct ImageReader { std::uintptr_t base; } owner{s.base};
      const auto readImage=[](void *ctx,std::uint32_t rva,void *dst,unsigned n) {
        return guardedRead(static_cast<ImageReader*>(ctx)->base+rva,dst,n);
      };
      const auto lookupImage=[](void *ctx,std::uint32_t rva) {
        const auto base=static_cast<ImageReader*>(ctx)->base;
        DWORD64 module{};auto entry=RtlLookupFunctionEntry(base+rva,&module,nullptr);
        ws::rdx::RuntimeRegion region{};
        if (entry) {
          region.found=true;region.sameImage=module==base;
          RUNTIME_FUNCTION copy{};
          region.readable=guardedRead(reinterpret_cast<std::uintptr_t>(entry),&copy,sizeof(copy));
          region.begin=copy.BeginAddress;region.end=copy.EndAddress;region.unwind=copy.UnwindData;
        }
        return region;
      };
      auto &proof=nestedReadCall.targetIdentity;
      proof=ws::rdx::authenticateReadTarget(nc.instructionVerified && s.imageSize<=UINT32_MAX,
          std::uint32_t(nc.target),std::uint32_t(s.imageSize),readImage,lookupImage,&owner);
      if (proof.admitted) {
        nc.begin=nc.target;nc.end=nc.target+proof.size;nc.boundaryVerified=true;
        nestedReadCall.size=proof.size;
        std::copy_n(proof.snapshot.data(),proof.size,readCallCode.data());
        nestedReadCall.authenticated=digest(readCallCode.data(),proof.size,nestedReadCall.hash);
        if (nestedReadCall.authenticated) nestedReadCall.code=readCallCode.data();
        else { proof.admitted=false;proof.reason="target_snapshot_SHA256_failed";proof.barrier=proof.target; }
      }
    }
    writerAlias.reason =
        calleeCurrent ? "writer_call_not_reached_in_bounded_prefix"
                      : "exact_c17_target_boundary_hash_not_authenticated";
    contractWriter = ws::rdx::branchWriter(code, functionSize, sites,
                                           &writerCall, &writerAlias);
    writeLog(
        "MOVEMENT_WRITER_CALL_ARGUMENT_CONTRACT call_rva=0x2cfc927 "
        "arg_rcx_origin=%s arg_rdx_origin=%s arg_r8_origin=%s arg_r9_origin=%s "
        "argument_offsets=%lld,%lld,%lld,%lld source_span_alias_mask=0x%x "
        "transform_alias_mask=0x%x proof_complete=%u call_states=%u "
        "writable_source_proven=0 reason=%s",
        writerAlias.argumentOrigins[0], writerAlias.argumentOrigins[1],
        writerAlias.argumentOrigins[2], writerAlias.argumentOrigins[3],
        (long long)writerAlias.argumentOffsets[0],
        (long long)writerAlias.argumentOffsets[1],
        (long long)writerAlias.argumentOffsets[2],
        (long long)writerAlias.argumentOffsets[3], writerAlias.sourceArgMask,
        writerAlias.transformArgMask, unsigned(writerAlias.argumentsComplete),
        writerAlias.analyses, writerAlias.reason);
    writeLog(
        "MOVEMENT_CALLEE_CFG_CONTRACT target_rva=0x2b23560 "
        "target_end=0x2b235e9 "
        "local_branch_sites=%u conditional_branch_sites=%u branch_cap=%u "
        "branch_resource_ceiling=8 path_cap_per_call=%u state_cap_per_call=%u "
        "blocks_total=%u edges_total=%u blocks_visited=%u edges_visited=%u "
        "paths_examined=%u backedges_seen=%u indirect_transfers_seen=%u "
        "nested_calls_seen=%u complete_local_cfg=%u "
        "first_unresolved_rva=0x%llx "
        "first_unresolved_instruction=%s first_unresolved_effect=%s "
        "counts_scope=static_analysis_not_CPU_execution full_bytes_logged=0 "
        "reason=%s",
        writerAlias.localBranches, writerAlias.conditionalBranches,
        writerAlias.branchBudget, writerAlias.pathBudget,
        writerAlias.stateBudget, writerAlias.blocks, writerAlias.edges,
        writerAlias.blocksVisited, writerAlias.edgesVisited, writerAlias.paths,
        writerAlias.backedges, writerAlias.indirectTransfers,
        writerAlias.nestedCalls, unsigned(writerAlias.cfgComplete),
        (unsigned long long)writerAlias.barrier, writerAlias.mnemonic,
        writerAlias.barrierEffect, writerAlias.reason);
    for (unsigned i = 0; i < writerAlias.writes; ++i) {
      const auto &w = writerAlias.writeSites[i];
      const bool unknown = w.kind == ws::rdx::unknownPointer;
      writeLog("MOVEMENT_CALLEE_WRITE_ALIAS site_rva=0x%llx width=%u "
               "address_class=%s address_root=%d address_offset=%lld "
               "may_alias_source=%u may_alias_transform=%u "
               "global_disjoint_conditional_on_runtime_regions=%u "
               "coverage=%s full_bytes_logged=0",
               (unsigned long long)w.at, w.width,
               ws::rdx::aliasKindName(w.kind), w.root, (long long)w.offset,
               unsigned(unknown || w.kind == ws::rdx::sourceExact ||
                        w.kind == ws::rdx::sourcePossible),
               unsigned(unknown || w.kind == ws::rdx::transformExact ||
                        w.kind == ws::rdx::transformPossible),
               unsigned(w.kind == ws::rdx::imageGlobal),
               writerAlias.resolved ? "all_reachable_paths"
                                    : "analyzed_prefix_only");
    }
    const auto &tw = writerAlias.exactWrite;
    writeLog(
        "MOVEMENT_CALLEE_WRITE_CONTRACT site_rva=0x%llx instruction_decoded=%u "
        "mnemonic=%s width=%u dst_base=%s dst_index=%s dst_scale=%u "
        "dst_disp=%lld "
        "dst_provenance=%s dst_root=%d dst_root_offset=%lld "
        "value_source_kind=%s value_source_register_or_memory=%s "
        "value_source_tag=%d value_source_root=%d value_source_offset=%lld "
        "value_source_constant_bits=0x%x value_source_provenance_exact=%u "
        "reaching_state_count=%u analyzed_states_same_destination=%u "
        "analyzed_states_same_value_origin=%u "
        "all_reaching_states_same_destination=%u "
        "all_reaching_states_same_value_origin=%u "
        "all_reaching_states_examined=%u "
        "write_overlaps_position_range=%s "
        "write_overlaps_transform_identity_or_pointer_carrier=%s "
        "write_affects_writer_reload_provenance=%s "
        "write_effect_relevant_to_xyz=%s "
        "write_effect_relevant_to_w=%s dependency_site_rva=0x%llx "
        "modeled_irrelevant_transform_write=%u transform_no_write_proven=%u "
        "transform_position_protected_range_unmodified=%u "
        "relevant_transform_effects_resolved=%u read_site_cap=64 "
        "counts_scope=static_analysis_not_CPU_execution runtime_observation=0 "
        "full_bytes_logged=0 writable_source_proven=0 reason=%s",
        (unsigned long long)tw.site, unsigned(tw.decoded), tw.mnemonic,
        tw.width, tw.base, tw.index, tw.scale, (long long)tw.displacement,
        tw.destinationOrigin, tw.root, (long long)tw.offset, tw.valueKind,
        tw.sourceOperand, tw.valueTag, tw.valueRoot, (long long)tw.valueOffset,
        tw.valueBits, unsigned(tw.valueExact), tw.reachingStates,
        unsigned(tw.destinationSame && tw.destinationExact),
        unsigned(tw.valueSame && tw.valueExact),
        unsigned(tw.complete && tw.destinationSame && tw.destinationExact),
        unsigned(tw.complete && tw.valueSame && tw.valueExact),
        unsigned(tw.complete),
        tw.reachingStates && tw.destinationExact
            ? (tw.positionOverlap ? "1" : "0")
            : "unknown",
        tw.reachingStates && tw.destinationExact
            ? (tw.carrierOverlap ? "1" : "0")
            : "unknown",
        tw.reloadDependency         ? "1"
        : tw.complete && tw.modeled ? "0"
                                    : "unknown",
        tw.positionOverlap || tw.carrierOverlap || tw.reloadDependency ? "1"
        : tw.complete && tw.modeled                                    ? "0"
                                    : "unknown",
        tw.positionOverlap || tw.carrierOverlap || tw.reloadDependency ? "1"
        : tw.complete && tw.modeled                                    ? "0"
                                    : "unknown",
        (unsigned long long)tw.dependencySite, unsigned(tw.modeled),
        unsigned(writerAlias.transformNoWrite),
        unsigned(writerAlias.positionProtected),
        unsigned(writerAlias.relevantEffectsResolved), writerAlias.reason);
    const auto &ev = writerAlias.edi;
    unsigned laterReads = 0;
    for (unsigned i = 0; i < writerAlias.reads; ++i)
      laterReads += writerAlias.readSites[i].at > tw.site &&
                    writerAlias.readSites[i].at < 0x2b235e9;
    writeLog(
        "MOVEMENT_CALLEE_VALUE_CONTRACT site_rva=0x2b2358e value_register=edi "
        "enabled=%u "
        "reaching_definition_count=%u producer_rva=0x%llx producer_kind=%s "
        "origin_kind=%s producer_root=%d producer_offset=%lld "
        "constant_bits=0x%x "
        "addend_mod_2_32=0x%x all_reaching_states_examined=%u "
        "all_reaching_states_same_value_origin=%u value_provenance_exact=%u "
        "writer_call_states_examined=%u all_writer_call_states_examined=%u "
        "coverage_scope=all_local_paths_for_analyzed_call_inputs "
        "entry_register_roots=callee_entry_raw_bits_NOT_pointer_types "
        "reaching_states=%u local_paths=%u instruction_visits=%u "
        "later_relevant_read_count=%u later_reads_scope=analyzed_prefix "
        "first_dependency_rva=0x%llx write_effect_relevant_to_xyz=%s "
        "write_effect_relevant_to_w=%s first_unresolved_rva=0x%llx "
        "first_unresolved_instruction=%s conflict_producer_rva=0x%llx "
        "conflict_root=%d conflict_offset=%lld "
        "definition_cap=16 copy_chain_cap=8 value_visit_cap=4096 "
        "full_bytes_logged=0 writable_source_proven=0 reason=%s",
        unsigned(ev.enabled), ev.definitions, (unsigned long long)ev.origin.producer,
        ev.origin.producerKind, ws::rdx::bitKindName(ev.origin.kind),
        ev.origin.root, (long long)ev.origin.offset, ev.origin.bits,
        ev.origin.addend, unsigned(ev.complete),
        unsigned(ev.complete && ev.same), unsigned(ev.exact),
        writerAlias.analyses, unsigned(ev.writerStatesComplete),
        ev.reachingStates, ev.paths, ev.visits, laterReads,
        (unsigned long long)tw.dependencySite,
        tw.reloadDependency || tw.positionOverlap || tw.carrierOverlap ? "1"
        : writerAlias.relevantEffectsResolved && tw.complete           ? "0"
                                                             : "unknown",
        tw.reloadDependency || tw.positionOverlap || tw.carrierOverlap ? "1"
        : writerAlias.relevantEffectsResolved && tw.complete           ? "0"
                                                             : "unknown",
        (unsigned long long)ev.barrier, ev.mnemonic,
        (unsigned long long)ev.conflict.producer, ev.conflict.root,
        (long long)ev.conflict.offset, ev.reason);
    for (unsigned j = 0; j < ev.definitions; ++j) {
      const auto &def = ev.producers[j];
      writeLog("MOVEMENT_CALLEE_VALUE_DEFINITION index=%u producer_rva=0x%llx "
               "producer_kind=%s origin_kind=%s root=%d offset=%lld "
               "constant_bits=0x%x addend_mod_2_32=0x%x chain_length=%u "
               "chain_scope=local_only full_bytes_logged=0",
               j, (unsigned long long)def.producer, def.producerKind,
               ws::rdx::bitKindName(def.kind), def.root, (long long)def.offset,
               def.bits, def.addend, def.chainSize);
      for (unsigned k = 0; k < def.chainSize; ++k)
        writeLog("MOVEMENT_CALLEE_VALUE_CHAIN definition=%u index=%u "
                 "producer_rva=0x%llx full_bytes_logged=0",
                 j, k, (unsigned long long)def.chain[k]);
    }
    const auto &rd = writerAlias.readDependency;
    const char *order = !rd.orderComplete ? "unresolved"
        : (rd.orderMask & 3) == 3 ? "branch_dependent_order"
        : (rd.orderMask & 12) && (rd.orderMask & 3) ? "branch_dependent_presence"
        : rd.orderMask & 1 ? "read_before_write"
        : rd.orderMask & 2 ? "read_after_write" : "unpaired_read_or_write";
    writeLog("MOVEMENT_CALLEE_READ_DEPENDENCY site_rva=0x2b23578 enabled=%u "
             "instruction_decoded=%u mnemonic=%s operand_class=memory_read "
             "read_width=%u address_class=%s address_root=%d address_offset=%lld "
             "address_exact=%u same_address_all_analyzed_states=%u "
             "address_scope=last_analyzed_state operand_index=%u base=%s index=%s scale=%u displacement=%lld "
             "overlap_transform_10c=%d overlap_unknown_minus1=1 "
             "value_carrier_kind=%s value_carrier=%s samples=%u "
             "order=%s order_mask=%u all_local_order_paths_examined=%u "
             "order_paths=%u order_visits=%u "
             "order_scope=local_CFG_CALL_normal_return_only full_bytes_logged=0",
             unsigned(rd.site != 0), unsigned(rd.decoded), rd.mnemonic,
             rd.width, rd.addressClass, rd.root, (long long)rd.offset,
             unsigned(rd.addressExact), unsigned(rd.sameAddress), rd.operandIndex, rd.base,
             rd.index, rd.scale, (long long)rd.displacement, rd.overlap, rd.carrierKind,
             rd.carrier, rd.samples, order, rd.orderMask,
             unsigned(rd.orderComplete), rd.orderPaths, rd.orderVisits);
    writeLog("MOVEMENT_CALLEE_READ_FLOW site_rva=0x2b23578 "
             "complete_local_flow=%u return_paths=%u instruction_visits=%u "
             "returned_gpr_may_depend_mask=0x%x returned_xmm_may_depend_mask=0x%x "
             "returned_flags_may_depend=%u returned_caller_stack_may_depend=%u "
             "new_field_value_gpr_mask=0x%x same_field_order_mask=%u "
             "field_order_coverage=analyzed_flow_paths "
             "first_possible_use_rva=0x%llx control_may_depend=%u "
             "first_control_rva=0x%llx memory_escape=%u first_escape_rva=0x%llx "
             "first_unresolved_rva=0x%llx first_unresolved_instruction=%s "
             "downstream_XYZ_W_relevance=unknown cross_invocation_feedback=unknown "
             "flow_kind=conservative_possible_dependence effect_certificate=0 "
             "full_bytes_logged=0 writable_source_proven=0 reason=%s",
             unsigned(rd.flowComplete), rd.paths, rd.visits, rd.returnedGpr,
             rd.returnedXmm, unsigned(rd.returnedFlags), unsigned(rd.returnedCallerStack),
             rd.newValueGpr, rd.fieldOrderMask, (unsigned long long)rd.firstUse,
             unsigned(rd.controlUse), (unsigned long long)rd.firstControl,
             unsigned(rd.memoryEscape), (unsigned long long)rd.firstEscape,
             (unsigned long long)rd.barrier, rd.unresolvedMnemonic, rd.reason);
    const auto &rt = rd.test;
    writeLog("MOVEMENT_CALLEE_READ_TEST site_rva=0x2b235be enabled=%u "
             "instruction_decoded=%u mnemonic=%s operand_count=%u "
             "form_supported=%u flags_only=%u low_byte_read=%u "
             "analyzed_states=%u modeled_states=%u flags_may_depend_tags=0x%x "
             "supported_flags_model=AND_ZF_SF_PF_CF0_OF0_AF_undefined modeled_operand_writes=0 "
             "dependence_kind=conservative_parent_tags_no_numeric_value "
             "effect_certificate=0 full_bytes_logged=0 reason=%s",
             unsigned(rt.site != 0), unsigned(rt.decoded), rt.mnemonic, rt.operandCount,
             unsigned(rt.formSupported), unsigned(rt.flagsOnly), unsigned(rt.lowByteRead),
             rt.samples, rt.modeledStates, rt.flagsTags, rt.reason);
    for (unsigned j = 0; j < std::min(rt.operandCount, 2u); ++j) {
      const auto &op = rt.operands[j];
      writeLog("MOVEMENT_CALLEE_READ_TEST_OPERAND site_rva=0x2b235be index=%u "
               "kind=%s register=%s width=%u parent_gpr=%d "
               "parent_may_depend_tags=0x%x low_byte_parent_overapproximation=%u "
               "base=%s memory_index=%s segment=%s scale=%u displacement=%lld "
               "immediate=%lld tags_scope=analyzed_states_union "
               "tags_1_selected_2_postwrite_4_other_prewrite=1 full_bytes_logged=0",
               j, op.kind, op.name, op.width, op.parent, op.parentTags,
               unsigned(op.conservativeParent), op.base, op.index, op.segment, op.scale,
               (long long)op.displacement, (long long)op.immediate);
    }
    writeLog("MOVEMENT_CALLEE_READ_TEST_FLAGS site_rva=0x2b235be "
             "first_consumer_rva=0x%llx first_consumer_instruction=%s consumer_kind=%s "
             "branch_samples=%u consumer_may_depend_tags=0x%x "
             "first_possible_dependent_branch_rva=0x%llx returned_flags_tags=0x%x "
             "CF_OF_conditions_independent=1 both_CFG_legs_enumerated=1 "
             "coverage=analyzed_local_flow_paths downstream_XYZ_W_relevance=unknown "
             "cross_invocation_feedback=unknown effect_certificate=0 full_bytes_logged=0",
             (unsigned long long)rt.firstConsumer, rt.consumerMnemonic, rt.consumerKind,
             rt.consumerSamples, rt.consumerTags,
             (unsigned long long)rt.firstDependentConsumer, rt.returnedFlagsTags);
    const auto &ac = writerAlias.counts;
    const auto &rcall=writerAlias.readDependency.call;
    const auto &identity=rcall.targetIdentity;
    writeLog("MOVEMENT_CALLEE_READ_TARGET_IDENTITY target_rva=0x%x enabled=%u admitted=%u "
             "method=%s code_region_begin_rva=0x%x code_region_end_rva=0x%x "
             "runtime_lookup_queried=%u runtime_lookup_found=%u runtime_lookup_same_image=%u runtime_lookup_readable=%u "
             "runtime_begin_rva=0x%x runtime_end_rva=0x%x next_runtime_rva=0x%x "
             "PE_runtime_table_valid=%u PE_runtime_entries=%u missing_runtime_entry=%u "
             "scan_bytes=%u admitted_bytes=%u instructions=%u snapshot_stable=%u "
             "barrier_rva=0x%x barrier_instruction=%s reason=%s "
             "proof_scope=method_specific_identity_only no_full_bytes=1 effect_certificate=0",
             identity.target,unsigned(identity.enabled),unsigned(identity.admitted),identity.method,
             identity.codeBegin,identity.codeEnd,unsigned(identity.lookupQueried),unsigned(identity.lookup.found),unsigned(identity.lookup.sameImage),
             unsigned(identity.lookup.readable),identity.runtimeBegin,identity.runtimeEnd,identity.nextRuntime,
             unsigned(identity.tableValid),identity.tableEntries,unsigned(identity.missingRuntime),
             identity.scanBytes,identity.size,identity.instructions,unsigned(identity.stable),
             identity.barrier,identity.mnemonic,identity.reason);
    writeLog("MOVEMENT_CALLEE_READ_RUNTIME_TABLE target_rva=0x%x "
             "directory_rva=0x%x directory_bytes=%u entry_bytes=12 derived_entries=%u "
             "RVA_size_pair_valid=%u directory_range_in_image=%u entry_size_divisible=%u "
             "metadata_byte_cap=%u entry_cap=%u byte_cap_exceeded=%u entry_cap_exceeded=%u "
             "budget_admitted=%u entry_validation_skipped_by_cap=%u "
             "metadata_bytes_read=%u entries_validated=%u guarded_read_failed=%u "
             "all_entries_read_and_validated=%u table_structurally_valid=%u "
             "runtime_lookup_queried=%u chunk_entries=64 max_chunk_bytes=768 "
             "reason=%s full_bytes_logged=0 effect_certificate=0",
             identity.target,identity.directoryRva,identity.directoryBytes,identity.tableEntries,
             unsigned(identity.directoryPairValid),unsigned(identity.directoryRangeInImage),
             unsigned(identity.directorySizeDivisible),ws::rdx::readRuntimeTableByteCap,
             ws::rdx::readRuntimeTableEntryCap,unsigned(identity.byteCapExceeded),unsigned(identity.entryCapExceeded),
             unsigned(identity.tableBudgetAdmitted),unsigned(identity.validationSkippedByCap),
             identity.metadataBytesRead,identity.validatedEntries,unsigned(identity.tableReadFailed),
             unsigned(identity.tableReadComplete),unsigned(identity.tableValid),unsigned(identity.lookupQueried),identity.reason);
    const auto &cmp=identity.cmp;
    writeLog("MOVEMENT_CALLEE_READ_CMP site_rva=0x%x decoded=%u operand_count=%u "
             "has_memory_operand=%u flags_only=%u form_supported=%u prefix_modeled=%u "
             "decoded_eflags=0x%llx flags_written=%s GPR_XMM_memory_writes=%s "
             "samples=%u flags_tags_known=%u flags_may_depend_tags=0x%x "
             "reason=%s scope=stable_pinned_prefix_not_complete_target_identity "
             "numerical_branch_outcome=unknown effect_certificate=0 full_bytes_logged=0",
             cmp.site,unsigned(cmp.decoded),cmp.operandCount,unsigned(cmp.hasMemory),
             unsigned(cmp.flagsOnly),unsigned(cmp.formSupported),unsigned(cmp.modelApplied),
             (unsigned long long)cmp.decodedEflags,cmp.flagsOnly?"CF_PF_AF_ZF_SF_OF":"unproven",cmp.flagsOnly?"0":"unproven",
             cmp.samples,unsigned(cmp.formSupported && cmp.samples!=0),cmp.flagsTags,cmp.reason);
    for (unsigned j=0;j<std::min(cmp.operandCount,2u);++j) {
      const auto &op=cmp.operands[j];
      writeLog("MOVEMENT_CALLEE_READ_CMP_OPERAND site_rva=0x%x index=%u "
               "kind=%s register=%s width=%u decoded_access=%u parent_gpr=%d parent_register=%s "
               "value_tags_known=%u parent_may_depend_tags=0x%x conservative_parent_tags=%u "
               "immediate=%lld base=%s memory_index=%s segment=%s scale=%u displacement=%lld "
               "memory_value_observed=0 partial_form_semantics=0 "
               "tags_1_selected_2_postwrite_4_other_prewrite=1 full_bytes_logged=0",
               cmp.site,j,op.kind,op.name,op.width,op.access,op.parent,op.parentName,
               unsigned(op.valueTagsKnown && cmp.samples!=0),op.parentTags,unsigned(op.conservativeParent),
               (long long)op.immediate,op.base,op.index,op.segment,op.scale,(long long)op.displacement);
    }
    const auto &branch=identity.branch;
    writeLog("MOVEMENT_CALLEE_READ_LEAF_BRANCH site_rva=0x%x decoded=%u instruction=%s "
             "conditional=%u condition=%s flags_read=%s flags_producer_rva=0x%x "
             "flags_dependency_known=%u samples=%u consumer_may_depend_tags=0x%x "
             "control_expanded=0 numerical_outcome=unknown both_outcomes_unresolved=1 "
             "rule=strict_linear_no_branch effect_certificate=0 full_bytes_logged=0",
             branch.site,unsigned(branch.decoded),branch.mnemonic,unsigned(branch.conditional),
             branch.condition,branch.flagsRead,branch.flagsProducer,
             unsigned(branch.flagsDependencyKnown && branch.samples!=0),branch.samples,branch.flagsTags);
    writeLog("MOVEMENT_CALLEE_READ_CALL site_rva=0x2b235d4 enabled=%u decoded=%u "
             "form=%s operand=%s direct=%u target_rva=0x%llx exact_target=%u target_reason=%s "
             "target_begin_rva=0x%llx target_end_rva=0x%llx boundary_verified=%u "
             "snapshot_authenticated=%u snapshot_sha256=%s bytes=%u "
             "samples=%u modeled_states=%u nested_visits=%u "
             "entry_flags_tags=0x%x caller_stack_tags=0x%x memory_escape=%u "
             "barrier_rva=0x%llx barrier_instruction=%s reason=%s "
             "expansion_depth_cap=1 leaf_byte_cap=128 leaf_instruction_cap=32 "
             "identity_scope=startup_current_code_snapshot full_bytes_logged=0 effect_certificate=0",
             unsigned(rcall.site!=0),unsigned(rcall.decoded),rcall.identity.kind,rcall.operand,
             unsigned(rcall.direct),(unsigned long long)rcall.identity.target,
             unsigned(rcall.identity.instructionVerified),rcall.identity.reason,(unsigned long long)rcall.identity.begin,
             (unsigned long long)rcall.identity.end,unsigned(rcall.identity.boundaryVerified),
             unsigned(rcall.authenticated),rcall.authenticated?rcall.hash:"unavailable",rcall.size,
             rcall.samples,rcall.modeledStates,rcall.visits,rcall.flagsTags,rcall.callerStackTags,
             unsigned(rcall.memoryEscape),(unsigned long long)rcall.barrier,rcall.unresolvedMnemonic,rcall.reason);
    for (unsigned k=0;k<16;++k) {
      const auto &arg=rcall.gpr[k];
      const char *aliasClass=arg.kind!=1 ? "unknown_raw_bits_or_load"
          : arg.root==1 ? "Hero_Transform" : arg.root==2 ? "writer_entry_RDX_source"
          : arg.root==4 ? "caller_stack" : arg.root==16 ? "image_global" : "other_possible_alias";
      writeLog("MOVEMENT_CALLEE_READ_CALL_CARRIER site_rva=0x2b235d4 gpr=%u "
               "gpr_tags=0x%x xmm_same_index_tags=0x%x origin_kind=%d root=%d offset=%lld "
               "alias_class=%s "
               "constant_bits=0x%x same_origin_all_analyzed_states=%u "
               "origin_scope=last_state_tags_union kind_0_unknown_1_pointer_2_mem32_3_mem64_4_constant_5_entry_6_computed=1 "
               "raw_carriers_not_typed_arguments=1 tags_1_selected_2_postwrite_4_other_prewrite=1",
               k,arg.tags,rcall.xmmTags[k],arg.kind,arg.root,(long long)arg.offset,aliasClass,arg.bits,unsigned(arg.same));
    }
    writeLog("MOVEMENT_CALLEE_READ_CALL_RETURN site_rva=0x2b235d4 "
             "selected_gpr_mask=0x%x postwrite_gpr_mask=0x%x other_prewrite_gpr_mask=0x%x "
             "selected_xmm_mask=0x%x postwrite_xmm_mask=0x%x other_prewrite_xmm_mask=0x%x "
             "returned_flags_tags=0x%x modified_gpr_mask_analyzed_prefix=0x%x modified_xmm_mask_analyzed_prefix=0x%x "
             "Win64_volatile_gpr_mask=0xf07 Win64_nonvolatile_gpr_mask=0xf0f8 "
             "Win64_volatile_xmm_mask=0x3f Win64_nonvolatile_xmm_mask=0xffc0 "
             "coverage=modeled_normal_returns_only unknown_calls_never_invalidate_then_continue=1 "
             "downstream_XYZ_W_relevance=unknown cross_invocation_feedback=unknown effect_certificate=0",
             rcall.returnedGpr[0],rcall.returnedGpr[1],rcall.returnedGpr[2],
             rcall.returnedXmm[0],rcall.returnedXmm[1],rcall.returnedXmm[2],
             rcall.returnedFlagsTags,rcall.clobberGpr,rcall.clobberXmm);
    writeLog(
        "MOVEMENT_CALLEE_ALIAS_CONTRACT target_rva=0x2b23560 "
        "target_end=0x2b235e9 "
        "identity_authenticated=%u memory_writes_seen=%u "
        "writes_proven_private_stack=%u "
        "writes_proven_global_disjoint=%u writes_proven_other_disjoint=%u "
        "writes_caller_shadow_or_return=%u writes_source_exact=%u "
        "writes_source_possible_alias=%u "
        "writes_transform_exact=%u writes_transform_possible_alias=%u "
        "writes_unknown=%u "
        "source_span_no_write_proven=%u transform_no_write_proven=%u "
        "transform_position_protected_range_unmodified=%u "
        "relevant_transform_effects_resolved=%u "
        "all_relevant_write_aliases_resolved=%u next_barrier_rva=0x%llx "
        "next_barrier_instruction=%s next_barrier_effect=%s address_root=%d "
        "address_offset=%lld write_width=%u callee_byte_cap=137 branch_cap=%u "
        "path_cap_per_call=%u state_cap_per_call=%u call_state_cap=16 "
        "total_path_cap=4096 "
        "total_visit_cap=4096 unique_write_cap=24 instruction_visits=%u "
        "return_paths=%u "
        "RIP_disjoint_conditional_on_runtime_regions=1 runtime_observation=0 "
        "writable_source_proven=0 full_bytes_logged=0 reason=%s",
        unsigned(writerAlias.authenticated), writerAlias.writes,
        ac[ws::rdx::privateStack], ac[ws::rdx::imageGlobal],
        ac[ws::rdx::callerArea] + ac[ws::rdx::otherDisjoint],
        ac[ws::rdx::callerArea], ac[ws::rdx::sourceExact],
        ac[ws::rdx::sourcePossible], ac[ws::rdx::transformExact],
        ac[ws::rdx::transformPossible], ac[ws::rdx::unknownPointer],
        unsigned(writerAlias.sourceNoWrite),
        unsigned(writerAlias.transformNoWrite),
        unsigned(writerAlias.positionProtected),
        unsigned(writerAlias.relevantEffectsResolved),
        unsigned(writerAlias.resolved), (unsigned long long)writerAlias.barrier,
        writerAlias.mnemonic, writerAlias.barrierEffect,
        writerAlias.barrierRoot, (long long)writerAlias.barrierOffset,
        writerAlias.barrierWidth, writerAlias.branchBudget,
        writerAlias.pathBudget, writerAlias.stateBudget, writerAlias.visits,
        writerAlias.paths, writerAlias.reason);
    writeLog(
        "MOVEMENT_WRITER_CALL_CONTRACT writer_rva=0x2cfc900 call_rva=0x2cfc927 "
        "call_kind=%s target_rva=0x%llx target_boundary_begin=0x%llx "
        "target_boundary_end=0x%llx target_code_hash=%s return_rva=0x2cfc92c "
        "exact_build_verified=1 current_code_verified=%u "
        "target_boundary_verified=%u target_complete_decode=%u "
        "relevant_to_xyz_paths=%u effect=%s source_span_no_write_proven=%u "
        "source_span_may_write=0 source_span_effect_unknown=%u "
        "entry_RDX_identity=volatile_clobbered "
        "source_reload_requires_preserved_alias=1 "
        "RCX_destination_identity=volatile_clobbered "
        "gpr_clobber_mask=0x%x xmm_clobber_mask=0x%x "
        "callee_private_stack_low=%lld private_stack_byte_cap=4096 "
        "stack_alias_policy=return_shadow_disjoint_saved_slots "
        "W_independent=1 next_callee_barrier_rva=0x%llx "
        "next_callee_instruction=%s callee_byte_cap=1024 expansion_depth=0 "
        "runtime_call_observation=0 new_hooks=0 writable_source_proven=0 "
        "full_bytes_logged=0 reason=%s",
        writerCall.kind, (unsigned long long)writerCall.target,
        (unsigned long long)writerCall.begin,
        (unsigned long long)writerCall.end,
        calleeCurrent ? writerCalleeHash : "unavailable",
        unsigned(calleeCurrent), unsigned(writerCall.boundaryVerified),
        unsigned(writerCall.decoded), unsigned(writerCall.relevant),
        writerCall.effect, unsigned(writerCall.noWrite),
        unsigned(!writerCall.noWrite), writerCall.gprClobber,
        writerCall.xmmClobber, (long long)writerCall.privateStackLow,
        (unsigned long long)writerCall.barrier, writerCall.mnemonic,
        writerCall.reason);
    contractCaller = ws::rdx::caller(callerBytes, sizeof(callerBytes),
                                     0x351eeab - s.callerBegin);
    contractConfigured = true;
    if (s.caller2Original && s.caller2End > s.caller2Begin &&
        s.caller2End - s.caller2Begin <= producerCode.size() &&
        s.caller2End <= s.imageSize) {
      producerBegin = s.caller2Begin;
      producerEnd = s.caller2End;
      producerSize = producerEnd - producerBegin;
      DWORD64 producerImage{};
      auto producerFunction = RtlLookupFunctionEntry(imageBase + 0x351a01f,
                                                     &producerImage, nullptr);
      bool verifiedProducer =
          producerFunction && producerImage == imageBase &&
          producerFunction->BeginAddress == producerBegin &&
          producerFunction->EndAddress == producerEnd &&
          guardedRead(imageBase + producerBegin, producerCode.data(),
                      producerSize) &&
          !std::memcmp(producerCode.data(), s.caller2Original, producerSize);
      if (!verifiedProducer)
        producerSize = 0;
      contractProducer = ws::rdx::relation(
          producerCode.data(), producerSize, imageBase + producerBegin,
          imageBase + 0x351a020, callerStart, contractRead);
      if (contractProducer.call >= imageBase + producerBegin &&
          contractProducer.call < imageBase + producerEnd)
        contractParent = ws::rdx::caller(
            producerCode.data(), producerSize,
            unsigned(contractProducer.call - imageBase - producerBegin));
    }
    writeLog("MOVEMENT_RDX_STATIC_CONTRACT admitted_build=1 "
             "writer_complete_decode=%u entry_store_prefix=%u "
             "xyz_RDX_0_4_8_paths=%u W_kind=%s W_classified=%u "
             "first_barrier_rva=0x%x first_barrier_instruction=%s "
             "caller_prefix=%u caller_RDX_local=%u "
             "caller_allocation_owned=%u caller2_relation=%s "
             "exact_next_producer_rva=0x%llx writable_source_proven=0 "
             "full_bytes_logged=0 reason=%s",
             unsigned(contractWriter.decoded), unsigned(contractWriter.prefix),
             unsigned(contractWriter.xyz), contractWriter.wKind,
             unsigned(contractWriter.w),
             contractWriter.barrierValid ? s.begin + contractWriter.barrier : 0,
             contractWriter.barrierMnemonic, unsigned(contractCaller.prefix),
             unsigned(contractCaller.rdxLocal), unsigned(contractCaller.owned),
             contractProducer.kind,
             (unsigned long long)(contractProducer.next >= imageBase
                                      ? contractProducer.next - imageBase
                                      : 0),
             contractWriter.reason);
    writeLog(
        "MOVEMENT_RDX_BRANCH_CONTRACT writer_rva=0x2cfc900 writer_bytes=%u "
        "blocks_visited=%u edges_visited=%u blocks_total=%u edges_total=%u "
        "branches_seen=%u first_branch_rva=0x%x "
        "all_relevant_store_paths_proven=%u component_scope=XYZ x_path=%s "
        "y_path=%s z_path=%s proven_component_mask=0x%x "
        "reaching_states_X_Y_Z_W=%u,%u,%u,%u paths_examined=%u "
        "states_scheduled=%u instruction_visits=%u "
        "exact_store_sites_verified=%u next_barrier_rva=0x%x "
        "next_barrier_instruction=%s next_edge_rva=0x%x max_writer_bytes=183 "
        "block_cap=16 edge_cap=24 branch_cap=4 path_cap=16 state_cap=31 "
        "instruction_visit_cap=4096 W_independent=1 "
        "homogeneous_coordinate_semantics=unknown writable_source_proven=0 "
        "full_bytes_logged=0 reason=%s",
        functionSize, contractWriter.blocksVisited, contractWriter.edgesVisited,
        contractWriter.blocks, contractWriter.edges, contractWriter.branches,
        contractWriter.branches ? s.begin + contractWriter.firstBranch : 0,
        unsigned(contractWriter.allRelevant), contractWriter.paths[0],
        contractWriter.paths[1], contractWriter.paths[2],
        contractWriter.componentProofMask, contractWriter.reachingStates[0],
        contractWriter.reachingStates[1], contractWriter.reachingStates[2],
        contractWriter.reachingStates[3], contractWriter.pathsExamined,
        contractWriter.statesScheduled, contractWriter.visits,
        unsigned(contractWriter.exactSites),
        contractWriter.barrierValid ? s.begin + contractWriter.barrier : 0,
        contractWriter.barrierMnemonic,
        contractWriter.barrierHasEdge ? s.begin + contractWriter.barrierEdge
                                      : 0,
        contractWriter.reason);
    configured = true;
    reason = "exact_build_runtime_boundary_and_c12_baseline_hash_direct_call_"
             "verified";
  }
  writeLog("MOVEMENT_ENTRY_DIAGNOSTIC_GATE admitted=%u writer_rva=0x2cfc900 "
           "end_rva=0x2cfc9b7 original_sha256=%s "
           "hash_scope=accepted_build_c12_startup_baseline "
           "NOT_whole_function_semantics=1 reason=%s new_Position_writes=0",
           unsigned(ok), ok ? hash : "unavailable", reason);
}
bool start(const reset_trace::Request &q) noexcept {
  if (!configured || failed || installed || usedScopes == windows ||
      !q.position || !q.session || !q.generation || !q.hero || !q.transform ||
      !q.owner || !gate || !gate(q) || !bytesEqual(baseline) || !callIntact())
    return false;
  for (unsigned i = 0; i < usedScopes.load(std::memory_order_acquire); ++i)
    if (scopes[i].request.session == q.session)
      return false; // hard cap is per session, not per install
  char currentHash[65]{};
  if (!digest(baseline.data(), functionSize, currentHash) ||
      std::strcmp(currentHash, baselineHash))
    return false;
  if (!created) {
    HMODULE pin{};
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_PIN,
                            reinterpret_cast<LPCWSTR>(&start), &pin))
      return false;
    auto init = MH_Initialize();
    if (init != MH_OK && init != MH_ERROR_ALREADY_INITIALIZED)
      return false;
    if (MH_CreateHook(reinterpret_cast<void *>(target),
                      reinterpret_cast<void *>(ws_movement_entry_thunk),
                      &ws_movement_entry_original) != MH_OK)
      return false;
    created = true;
  }
  if (!bytesEqual(baseline) || !gate(q) ||
      MH_EnableHook(reinterpret_cast<void *>(target)) != MH_OK)
    return false;
  installed = true;
  if (!guardedRead(target, patch.data(), functionSize) ||
      std::memcmp(patch.data() + 32, baseline.data() + 32, functionSize - 32)) {
    stop();
    failed = true;
    return false;
  }
  if (!gate(q)) {
    stop();
    return false;
  }
  auto scopeIndex = usedScopes.load();
  auto &scope = scopes[scopeIndex];
  scope.index = scopeIndex;
  scope.request = q;
  scope.active = false;
  usedScopes.store(scopeIndex + 1, std::memory_order_release);
  activeScope.store(&scope, std::memory_order_release);
  ws_movement_entry_enabled = 0;
  writeLog(
      "MOVEMENT_ENTRY_DIAGNOSTIC_START session=%llu installed=1 "
      "diagnostic_only=1 eligible_cap=8 stack_bytes=64 pointer_candidates=3 "
      "return_interception=0 ABI=raw_entry_tailjump no_source_mutation=1 "
      "capture_ready=0",
      (unsigned long long)q.session);
  return true;
}
bool activate() noexcept {
  auto *scope = activeScope.load(std::memory_order_acquire);
  if (!scope || !installed || failed || !gate || !gate(scope->request) ||
      !owned())
    return false;
  scope->activated.store(true, std::memory_order_release);
  scope->active.store(true, std::memory_order_release);
  ws_movement_entry_enabled = 1;
  writeLog("MOVEMENT_ENTRY_DIAGNOSTIC_ACTIVE session=%llu capture_ready=1 "
           "hardware_watch_published=1 eligible_cap=8",
           (unsigned long long)scope->request.session);
  return true;
}
void cancel() noexcept {
  ws_movement_entry_enabled = 0;
  auto *s = activeScope.exchange(nullptr);
  if (s) {
    s->active = false;
    s->finalized.store(
        true, std::memory_order_release); // reporting only; install/cleanup
                                          // policy unchanged
  }
}
void stop() noexcept {
  cancel();
  if (!installed)
    return;
  if (!bytesEqual(patch) && !entryBranchOwned()) {
    failed = true;
    writeLog("MOVEMENT_ENTRY_DIAGNOSTIC_STOP restored=0 "
             "foreign_or_code_changed=1 trampoline_retained=1");
    return;
  }
  auto status = MH_DisableHook(reinterpret_cast<void *>(target));
  if (status == MH_OK) {
    installed = false;
    if (!bytesEqual(baseline))
      failed = true;
  } else
    failed = true;
  writeLog(
      "MOVEMENT_ENTRY_DIAGNOSTIC_STOP restored=%u code_restored=%u "
      "inflight_callbacks=%u trampoline_retained=1 reason=%s",
      unsigned(!installed), unsigned(!installed && !failed), callbacks.load(),
      failed ? "cleanup_failure_capture_disabled" : "owned_patch_restored");
}
bool originalIfOwned(std::uintptr_t p, void *out, std::size_t n) noexcept {
  if (p != target || n != functionSize || !owned())
    return false;
  std::memcpy(out, baseline.data(), n);
  return true;
}
std::uint64_t pair(std::uint64_t session, std::uint64_t generation,
                   std::uintptr_t transform, std::uintptr_t rsp,
                   std::uintptr_t ret) noexcept {
  auto *scope = activeScope.load(std::memory_order_acquire);
  auto invalidate = [&]() noexcept {
    for (unsigned i = 0; i < depth; ++i)
      if (local[i].rsp == rsp)
        local[i].id = 0;
    return std::uint64_t{0};
  };
  if (!scope || !scope->active || scope->request.session != session ||
      scope->request.generation != generation ||
      scope->request.transform != transform || !gate || !gate(scope->request) ||
      !owned())
    return invalidate();
  for (unsigned i = depth; i > 0; --i) {
    auto t = local[i - 1];
    if (t.rsp != rsp)
      continue;
    if (!t.id || t.id > capacity)
      return 0;
    auto &rec = records[t.id - 1];
    if (rec.state.load(std::memory_order_acquire) != 1)
      return 0;
    auto &v = rec.snapshot;
    return v.session == session && v.generation == generation &&
                   v.target == transform && v.ret == ret &&
                   v.tid == GetCurrentThreadId()
               ? t.id
               : invalidate();
  }
  return 0;
}
void storeComponent(std::uint64_t id, unsigned component,
                    std::uint32_t writtenBits, const float p[4],
                    std::uintptr_t ret, std::uintptr_t caller2,
                    unsigned storeOffset) noexcept {
  if (!id || id > capacity || component > 3 || !p)
    return;
  auto slot = unsigned(id - 1);
  auto &rec = records[slot];
  if (rec.state.load(std::memory_order_acquire) != 1)
    return;
  auto &v = rec.snapshot;
  auto &a = aggregates[slot];
  if (a.printed || a.rejected || ret != v.ret ||
      (v.chainVerified && caller2 != v.caller2))
    return;
  if (a.mask & (1u << component)) {
    a.printed = true;
    a.rejected = true;
    writeLog("MOVEMENT_WRITER_INVOCATION_REJECT invocation_id=%llu "
             "reason=duplicate_component_or_reentry_unproven "
             "writable_source_proven=0",
             (unsigned long long)id);
    return;
  }
  if (component != a.events)
    a.ordered = false;
  a.mask |= 1u << component;
  a.stores[component] = storeOffset;
  std::memcpy(a.values + component, &writtenBits, 4);
  std::memcpy(a.after, p, 16);
  ++a.events;
  if (a.mask == 15)
    finish(slot);
}

int registerIndex(unsigned r) {
  switch (r) {
#define R(A, B, N)                                                             \
  case X86_REG_##A:                                                            \
  case X86_REG_##B:                                                            \
    return N
    R(RAX, EAX, 0);
    R(RCX, ECX, 1);
    R(RDX, EDX, 2);
    R(RBX, EBX, 3);
    R(RSP, ESP, 4);
    R(RBP, EBP, 5);
    R(RSI, ESI, 6);
    R(RDI, EDI, 7);
    R(R8, R8D, 8);
    R(R9, R9D, 9);
    R(R10, R10D, 10);
    R(R11, R11D, 11);
    R(R12, R12D, 12);
    R(R13, R13D, 13);
    R(R14, R14D, 14);
    R(R15, R15D, 15);
#undef R
  default:
    return -1;
  }
}
void observe(std::uint64_t id, std::uintptr_t after, std::uintptr_t transform,
             const std::uint64_t *gpr, const unsigned char *xmm, bool floating,
             const float p[4], std::uintptr_t ret, std::uintptr_t caller2,
             bool positionReadable) noexcept {
  if (id && id <= capacity && !positionReadable) {
    aggregates[id - 1].rejected = true;
    return;
  }
  if (!id || id > capacity || !gpr || !p || !functionSize || after <= target ||
      after > target + functionSize)
    return;
  csh h{};
  if (cs_open(CS_ARCH_X86, CS_MODE_64, &h) != CS_ERR_OK)
    return;
  cs_option(h, CS_OPT_DETAIL, CS_OPT_ON);
  cs_insn *ins{};
  auto count = cs_disasm(h, baseline.data(), functionSize, target, 0, &ins);
  std::uintptr_t pc = target;
  bool complete = true, found = false;
  unsigned component{}, storeOffset{};
  std::uint32_t bits{};
  for (std::size_t i = 0; i < count; ++i) {
    auto &v = ins[i];
    if (!v.id || !v.detail || v.address != pc) {
      complete = false;
      break;
    }
    pc += v.size;
    if (pc != after || (v.id != X86_INS_MOV && v.id != X86_INS_MOVSS &&
                        v.id != X86_INS_VMOVSS))
      continue;
    auto &x = v.detail->x86;
    if (x.op_count != 2 || x.operands[0].type != X86_OP_MEM ||
        x.operands[0].size != 4 || x.operands[0].mem.index != X86_REG_INVALID ||
        x.operands[0].mem.segment != X86_REG_INVALID)
      continue;
    auto &mem = x.operands[0].mem;
    int base = registerIndex(mem.base);
    if (base < 0)
      continue;
    std::uintptr_t address{};
    if (mem.disp >= 0) {
      if (gpr[base] > UINTPTR_MAX - std::uint64_t(mem.disp))
        continue;
      address = gpr[base] + mem.disp;
    } else {
      if (gpr[base] < std::uint64_t(-mem.disp))
        continue;
      address = gpr[base] - std::uint64_t(-mem.disp);
    }
    if (transform > UINTPTR_MAX - 64 || address < transform + 48 ||
        address > transform + 60 || (address - transform - 48) % 4)
      continue;
    component = unsigned(address - transform - 48) / 4;
    storeOffset = unsigned(v.address - target);
    auto &source = x.operands[1];
    if (source.type == X86_OP_IMM) {
      bits = std::uint32_t(source.imm);
      found = true;
    } else if (source.type == X86_OP_REG) {
      int g = registerIndex(source.reg);
      if (g >= 0) {
        bits = std::uint32_t(gpr[g]);
        found = true;
      } else if (floating && xmm && source.reg >= X86_REG_XMM0 &&
                 source.reg <= X86_REG_XMM15) {
        std::memcpy(&bits, xmm + 16 * (source.reg - X86_REG_XMM0), 4);
        found = true;
      }
    }
  }
  if (ins)
    cs_free(ins, count);
  cs_close(&h);
  if (complete && pc == target + functionSize && found &&
      records[id - 1].state.load(std::memory_order_acquire) == 1 &&
      records[id - 1].snapshot.target == transform)
    storeComponent(id, component, bits, p, ret, caller2, storeOffset);
  else if (id && id <= capacity)
    aggregates[id - 1].rejected = true;
}
CaptureSummary summary(std::uint64_t session) noexcept {
  CaptureSummary result{};
  for (unsigned i = 0; i < usedScopes.load(std::memory_order_acquire); ++i) {
    auto &scope = scopes[i];
    if (scope.request.session != session)
      continue;
    result.activated = scope.activated.load(std::memory_order_acquire);
    for (unsigned j = 0; j < scope.reserved.load(std::memory_order_acquire);
         ++j)
      if (records[i * perSession + j].state.load(std::memory_order_acquire) ==
          1)
        ++result.snapshots;
    auto &e = evidence[i];
    result.complete = e.complete;
    for (unsigned id :
         {20u, 23u, 0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,  10u, 11u,
          12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 21u, 22u, 24u, 25u, 26u})
      if (e.xyz[id] > result.xyzMatches) {
        result.xyzMatches = e.xyz[id];
        result.candidate = id;
      }
  }
  return result;
}
void drain() noexcept {
  for (unsigned i = 0; i < usedScopes; ++i) {
    auto &scope = scopes[i];
    if (!scope.finalized.load(std::memory_order_acquire) || callbacks.load())
      continue;
    for (unsigned j = 0; j < scope.reserved.load(); ++j)
      finish(i * perSession + j);
    auto &e = evidence[i];
    if (!e.summarized) {
      e.summarized = true;
      auto result = summary(scope.request.session);
      auto id = result.candidate;
      writeLog(
          "MOVEMENT_ENTRY_CAPTURE_SUMMARY session=%llu capture_activated=%u "
          "true_entry_snapshots=%u complete_invocation_maps=%u "
          "xyz_source_candidate=%u xyz_match_count=%u xyz_match_mask=0x%x "
          "xyz_stable_across_invocations=%u source_w_pattern=%s "
          "source_w_bits=0x%x destination_w_pattern=%s destination_w_bits=0x%x "
          "stability_scope=session_XYZ_match_pattern storage_lifetime_proven=0 "
          "stable_across_surfaces=0 writable_source_proven=0 "
          "Position_writes_by_mod=0 phase=finalized summary_final=1",
          (unsigned long long)scope.request.session, unsigned(result.activated),
          result.snapshots, result.complete, id, result.xyzMatches,
          result.xyzMatches ? 7 : 0,
          unsigned(result.xyzMatches >= 2 &&
                   result.xyzMatches == result.complete),
          id == 27 || !e.sourceWSeen[id] ? "unobserved"
          : e.sourceWMixed[id]           ? "mixed"
                                         : "constant_observed",
          id == 27 ? 0 : e.sourceW[id],
          !e.destinationWSeen   ? "unobserved"
          : e.destinationWMixed ? "mixed"
                                : "constant_observed",
          e.destinationW);
    }
  }
}
} // namespace msm2::writer_entry
