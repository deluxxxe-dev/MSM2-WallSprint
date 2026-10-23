#define WIN32_LEAN_AND_MEAN
#include "NativeHooks/writer_entry_snapshot.hpp"
#include "Math/movement_delta.hpp"
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
};
struct Snapshot {
  std::uint64_t id{}, session{}, generation{};
  DWORD tid{};
  std::uintptr_t hero{}, target{}, owner{}, rsp{}, ret{}, caller2{};
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
struct Record {
  std::atomic<unsigned> state{0};
  Snapshot snapshot{};
};
struct Aggregate {
  unsigned mask{}, events{};
  float values[4]{}, after[4]{};
  bool printed{};
};
std::array<Scope, windows> scopes;
std::array<Record, capacity> records;
std::array<Aggregate, capacity> aggregates;
std::array<unsigned, 28> candidateCounts{};
std::array<std::uint64_t, 28> candidateSessions{};
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
  unsigned sourceMask = 0, pointerMask = 0;
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
      ++candidateCounts[id];
      candidateSessions[id] = v.session;
      sourceMask |= 1u << (id % 32);
      if (id >= 20 && id < 23)
        pointerMask |= 1u << (id - 20);
    }
    writeLog("MOVEMENT_ENTRY_SOURCE_CANDIDATE session=%llu invocation_id=%llu "
             "candidate_id=%u origin=%s origin_register_or_offset=0x%x "
             "read_valid=%u value4=(%.9g,%.9g,%.9g,%.9g) "
             "raw_bits=(0x%x,0x%x,0x%x,0x%x) relationship=%s "
             "exact_store_match_mask=0x%x observed_store_mask=0x%x "
             "lane_to_store_match_matrix=0x%x "
             "same_origin_full_matches=%u stable_across_invocations=0 "
             "stable_across_surfaces=0 "
             "typed_semantics=unknown writable_source_proven=0 "
             "reason=entry_value_correlation_NOT_causal_or_exclusive_proof",
             (unsigned long long)v.session, (unsigned long long)v.id, id,
             origin, offset, unsigned(read), values[0], values[1], values[2],
             values[3], bits[0], bits[1], bits[2], bits[3], relation, mask,
             a.mask, read ? laneMatches(bits, a.values, a.mask) : 0,
             candidateCounts[id]);
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
      "source_stable=0 position_before_read=%u position_after_read=%u "
      "delta_valid=%u delta_normal=%.9g delta_forward=%.9g delta_right=%.9g "
      "events=%u reason=%s writable_source_proven=0 Position_writes_by_mod=0",
      (unsigned long long)v.session, (unsigned long long)v.id, v.tid,
      (unsigned long long)v.target, (unsigned long long)(v.ret - imageBase),
      unsigned(a.mask & 1 ? 1 : 0), unsigned(a.mask & 2 ? 1 : 0),
      unsigned(a.mask & 4 ? 1 : 0), unsigned(a.mask & 8 ? 1 : 0), v.before[0],
      v.before[1], v.before[2], v.before[3], a.after[0], a.after[1], a.after[2],
      a.after[3], a.values[0], a.values[1], a.values[2], a.values[3],
      sourceMask, pointerMask, unsigned(v.beforeRead), unsigned(a.events > 0),
      unsigned(d.valid), d.normal, d.forward, d.right, a.events,
      a.mask == 15 ? "four_same_invocation_post_store_components_NOT_atomic_"
                     "return_or_causal_proof"
                   : "partial_or_no_store_capture");
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
  if (s)
    s->active = false;
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
  if (!scope || !scope->active || scope->request.session != session ||
      scope->request.generation != generation ||
      scope->request.transform != transform || !gate || !gate(scope->request) ||
      !owned())
    return 0;
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
               : 0;
  }
  return 0;
}
void storeComponent(std::uint64_t id, unsigned component,
                    std::uint32_t writtenBits, const float p[4],
                    std::uintptr_t ret, std::uintptr_t caller2) noexcept {
  if (!id || id > capacity || component > 3 || !p)
    return;
  auto slot = unsigned(id - 1);
  auto &rec = records[slot];
  if (rec.state.load(std::memory_order_acquire) != 1)
    return;
  auto &v = rec.snapshot;
  auto &a = aggregates[slot];
  if (a.printed || ret != v.ret || (v.chainVerified && caller2 != v.caller2))
    return;
  if (a.mask & (1u << component)) {
    a.printed = true;
    writeLog("MOVEMENT_WRITER_INVOCATION_REJECT invocation_id=%llu "
             "reason=duplicate_component_or_reentry_unproven "
             "writable_source_proven=0",
             (unsigned long long)id);
    return;
  }
  a.mask |= 1u << component;
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
             const float p[4], std::uintptr_t ret,
             std::uintptr_t caller2) noexcept {
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
  unsigned component{};
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
    storeComponent(id, component, bits, p, ret, caller2);
}
void drain() noexcept {
  for (unsigned i = 0; i < usedScopes; ++i) {
    auto &scope = scopes[i];
    if (scope.active.load() || callbacks.load())
      continue;
    for (unsigned j = 0; j < scope.reserved.load(); ++j)
      finish(i * perSession + j);
  }
}
} // namespace msm2::writer_entry
