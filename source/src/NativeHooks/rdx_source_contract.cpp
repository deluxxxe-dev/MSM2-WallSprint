#include "NativeHooks/rdx_source_contract.hpp"
#include <algorithm>
#include <capstone/capstone.h>
#include <cstring>
namespace ws::rdx {
namespace {
struct Decode {
  csh h{};
  cs_insn *p{};
  std::size_t n{};
  ~Decode() {
    if (p)
      cs_free(p, n);
    if (h)
      cs_close(&h);
  }
  bool run(const unsigned char *b, std::size_t size, std::uintptr_t base = 0) {
    if (!b || !size || size > 8192 || base > UINTPTR_MAX - size ||
        cs_open(CS_ARCH_X86, CS_MODE_64, &h) != CS_ERR_OK)
      return false;
    cs_option(h, CS_OPT_DETAIL, CS_OPT_ON);
    cs_option(h, CS_OPT_SKIPDATA, CS_OPT_OFF);
    n = cs_disasm(h, b, size, base, 0, &p);
    auto end = base;
    for (std::size_t i = 0; i < n; ++i) {
      if (!p[i].id || !p[i].detail || p[i].address != end)
        return false;
      end += p[i].size;
    }
    return end == base + size;
  }
};
int reg(unsigned r) {
  switch (r) {
#define R(A, B, I)                                                             \
  case X86_REG_##A:                                                            \
  case X86_REG_##B:                                                            \
    return I
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
int xmm(unsigned r) {
  return r >= X86_REG_XMM0 && r <= X86_REG_XMM15 ? int(r - X86_REG_XMM0) : -1;
}
bool group(const cs_insn &i, unsigned g) {
  for (unsigned k = 0; k < i.detail->groups_count; ++k)
    if (i.detail->groups[k] == g)
      return true;
  return false;
}
enum Kind { unknown, pointer, memory, memoryWide, constant, entry, computed };
struct Value {
  Kind kind{};
  int root{};
  std::int64_t off{};
  std::uint32_t bits{};
};
bool equal(Value a, Value b) {
  return a.kind == b.kind && a.root == b.root && a.off == b.off &&
         a.bits == b.bits;
}
Value plus(Value a, std::int64_t off) {
  if (a.kind != pointer || off < -(1ll << 30) || off > (1ll << 30) ||
      a.off < -(1ll << 30) || a.off > (1ll << 30))
    return {};
  a.off += off;
  return a;
}
struct State {
  std::array<Value, 16> g{};
  std::array<Value, 64> x{};
  struct Slot {
    Value v{};
    std::int64_t off{};
    unsigned width{}, producer{};
    bool used{};
  };
  std::array<Slot, 128> stack{};
  bool overflow{};
  State() {
    for (unsigned i = 0; i < 16; ++i)
      g[i] = {pointer, int(i)};
    for (unsigned i = 0; i < 64; ++i)
      x[i] = {entry, int(16 + i / 4), std::int64_t(i % 4)};
  }
  Value address(const cs_x86_op &a) const {
    if (a.type != X86_OP_MEM || a.mem.index != X86_REG_INVALID ||
        a.mem.segment != X86_REG_INVALID)
      return {};
    auto r = reg(a.mem.base);
    return r >= 0 ? plus(g[r], a.mem.disp) : Value{};
  }
  Value load(Value a, unsigned width) const {
    if (a.kind != pointer)
      return {};
    if (a.root == 4)
      for (auto &s : stack)
        if (s.used && a.off < s.off + s.width && s.off < a.off + width)
          return a.off == s.off && width == s.width ? s.v : Value{};
    return width == 4   ? Value{memory, a.root, a.off}
           : width == 8 ? Value{memoryWide, a.root, a.off}
                        : Value{};
  }
  Value value(const cs_x86_op &a, unsigned lane = 0) const {
    if (a.type == X86_OP_IMM)
      return {constant, 0, 0, std::uint32_t(a.imm)};
    if (a.type == X86_OP_MEM)
      return load(plus(address(a), lane * 4), a.size == 8 ? 8 : 4);
    if (a.type == X86_OP_REG) {
      auto r = reg(a.reg), x = xmm(a.reg);
      if (r >= 0) {
        auto v = g[r];
        if (a.size == 4 && v.kind == memoryWide)
          v.kind = memory;
        return a.size == 4 && v.kind == pointer ? Value{entry, r} : v;
      }
      if (x >= 0)
        return this->x[4 * x + lane];
    }
    return {};
  }
  Value laneValue(const cs_x86_op &a, unsigned lane) const {
    if (a.type == X86_OP_MEM)
      return load(plus(address(a), lane * 4), 4);
    if (a.type == X86_OP_REG && reg(a.reg) >= 0) {
      auto v = g[reg(a.reg)];
      if (v.kind == memoryWide)
        return {memory, v.root, v.off + lane * 4};
      return lane ? Value{} : value(a);
    }
    return value(a, lane);
  }
  bool save(Value a, Value v, unsigned width, unsigned at) {
    if (a.kind != pointer || a.root != 4 || a.off < -4096 || a.off > 4096)
      return false;
    for (auto &s : stack)
      if (s.used && a.off < s.off + s.width && s.off < a.off + width)
        s.used = false;
    for (auto &s : stack)
      if (!s.used) {
        s = {v, a.off, width, at + 1, true};
        return true;
      }
    overflow = true;
    return false;
  }
};
struct Slice {
  State s{};
  unsigned mask{}, barrier{}, end{};
  std::array<Value, 4> out{};
  std::array<unsigned, 4> stores{};
  bool step(const cs_insn &ins, bool writer) {
    auto &x = ins.detail->x86;
    if (x.addr_size != 8)
      for (unsigned j = 0; j < x.op_count; ++j)
        if (x.operands[j].type == X86_OP_MEM)
          return false;
    unsigned at = unsigned(ins.address);
    if (ins.id == X86_INS_NOP || ins.id == X86_INS_ENDBR64 ||
        ins.id == X86_INS_VZEROUPPER)
      return true;
    if (group(ins, CS_GRP_CALL) || group(ins, CS_GRP_JUMP) ||
        group(ins, CS_GRP_RET) || group(ins, CS_GRP_INT) ||
        group(ins, CS_GRP_IRET))
      return false;
    if (ins.id == X86_INS_PUSH && x.op_count == 1 &&
        x.operands[0].type == X86_OP_REG && reg(x.operands[0].reg) >= 0) {
      s.g[4] = plus(s.g[4], -8);
      return s.save(s.g[4], s.value(x.operands[0]), 8, at);
    }
    if (ins.id == X86_INS_POP && x.op_count == 1 &&
        x.operands[0].type == X86_OP_REG && reg(x.operands[0].reg) >= 0 &&
        reg(x.operands[0].reg) != 4) {
      s.g[reg(x.operands[0].reg)] = s.load(s.g[4], 8);
      s.g[4] = plus(s.g[4], 8);
      return true;
    }
    if (x.op_count < 2)
      return false;
    const auto &d = x.operands[0], &src = x.operands[1];
    int g = d.type == X86_OP_REG ? reg(d.reg) : -1,
        v = d.type == X86_OP_REG ? xmm(d.reg) : -1;
    if ((ins.id == X86_INS_CMP || ins.id == X86_INS_TEST))
      return true;
    if ((ins.id == X86_INS_ADD || ins.id == X86_INS_SUB) && g >= 0 &&
        d.size == 8 && src.type == X86_OP_IMM) {
      s.g[g] = plus(s.g[g], (ins.id == X86_INS_SUB ? -1 : 1) * src.imm);
      return true;
    }
    if (ins.id == X86_INS_LEA && g >= 0 && d.size == 8) {
      s.g[g] = s.address(src);
      return s.g[g].kind != unknown;
    }
    bool scalar = ins.id == X86_INS_MOVSS || ins.id == X86_INS_VMOVSS;
    bool half = ins.id == X86_INS_MOVSD || ins.id == X86_INS_VMOVSD ||
                ins.id == X86_INS_MOVQ || ins.id == X86_INS_VMOVQ;
    bool move = half || scalar || ins.id == X86_INS_MOV ||
                ins.id == X86_INS_MOVD || ins.id == X86_INS_VMOVD ||
                ins.id == X86_INS_MOVAPS || ins.id == X86_INS_MOVUPS ||
                ins.id == X86_INS_VMOVAPS || ins.id == X86_INS_VMOVUPS ||
                ins.id == X86_INS_MOVDQA || ins.id == X86_INS_MOVDQU ||
                ins.id == X86_INS_VMOVDQA || ins.id == X86_INS_VMOVDQU;
    if (move && x.op_count == 2) {
      if (d.type == X86_OP_MEM) {
        auto a = s.address(d);
        if (a.kind != pointer)
          return false;
        if (a.root == 4) {
          // Never accept a prefix that changes the runtime-observed input
          // window.
          if (writer && a.off < 0x38 && a.off + d.size > 0x28)
            return false;
          if (d.size == 8)
            return s.save(a, s.value(src), 8, at);
          if (d.size != 4 && d.size != 16)
            return false;
          for (unsigned k = 0; k < d.size / 4; ++k)
            if (!s.save(plus(a, 4 * k), s.value(src, k), 4, at))
              return false;
          return true;
        }
        if (!writer || a.root != 1 || a.off < 0x30 || a.off + d.size > 0x40 ||
            a.off % 4 || (d.size != 4 && d.size != 16))
          return false;
        for (unsigned k = 0; k < d.size / 4; ++k) {
          unsigned c = unsigned(a.off - 0x30) / 4 + k;
          if (mask & (1u << c))
            return false;
          out[c] = s.value(src, k);
          stores[c] = at;
          mask |= 1u << c;
        }
        return true;
      }
      if (g >= 0 && (d.size == 4 || d.size == 8)) {
        s.g[g] = s.value(src);
        if (d.size == 4 && s.g[g].kind == pointer)
          s.g[g] = {};
        return true;
      }
      if (v >= 0 && d.size == 16) {
        unsigned lanes =
            half                                                          ? 2
            : scalar || ins.id == X86_INS_MOVD || ins.id == X86_INS_VMOVD ? 1
                                                                          : 4;
        auto old = s;
        for (unsigned k = 0; k < lanes; ++k)
          s.x[v * 4 + k] = old.laneValue(src, k);
        if (lanes < 4 &&
            (src.type == X86_OP_MEM ||
             (!scalar && ins.id != X86_INS_MOVSD && ins.id != X86_INS_VMOVSD)))
          for (unsigned k = lanes; k < 4; ++k)
            s.x[v * 4 + k] = {constant};
        return true;
      }
      return false;
    }
    if ((ins.id == X86_INS_VMOVSS || ins.id == X86_INS_VMOVSD) &&
        x.op_count == 3 && v >= 0 && xmm(src.reg) >= 0 &&
        xmm(x.operands[2].reg) >= 0) {
      auto old = s;
      for (unsigned k = 0; k < 4; ++k)
        s.x[v * 4 + k] = old.laneValue(
            k < (ins.id == X86_INS_VMOVSD ? 2u : 1u) ? x.operands[2] : src, k);
      return true;
    }
    if ((ins.id == X86_INS_XOR || ins.id == X86_INS_PXOR ||
         ins.id == X86_INS_XORPS || ins.id == X86_INS_VPXOR ||
         ins.id == X86_INS_VXORPS) &&
        d.type == X86_OP_REG) {
      const auto &a = x.op_count == 3 ? src : d,
                 &b = x.operands[x.op_count - 1];
      if (a.type != X86_OP_REG || b.type != X86_OP_REG || a.reg != b.reg)
        return false;
      if (g >= 0) {
        s.g[g] = {constant};
        return d.size == 4 || d.size == 8;
      }
      if (v >= 0) {
        for (unsigned k = 0; k < 4; ++k)
          s.x[v * 4 + k] = {constant};
        return true;
      }
      return false;
    }
    auto old = s;
    bool shuffle = ins.id == X86_INS_SHUFPS || ins.id == X86_INS_VSHUFPS;
    bool pshuffle = ins.id == X86_INS_PSHUFD || ins.id == X86_INS_VPSHUFD;
    if ((shuffle || pshuffle) && v >= 0 &&
        x.operands[x.op_count - 1].type == X86_OP_IMM) {
      unsigned imm = unsigned(x.operands[x.op_count - 1].imm);
      if (pshuffle && x.op_count == 3) {
        for (unsigned k = 0; k < 4; ++k)
          s.x[v * 4 + k] = old.value(src, (imm >> (2 * k)) & 3);
        return true;
      }
      if (shuffle && (x.op_count == 3 || x.op_count == 4)) {
        const auto &a = x.op_count == 4 ? src : d,
                   &b = x.operands[x.op_count - 2];
        for (unsigned k = 0; k < 4; ++k)
          s.x[v * 4 + k] = old.value(k < 2 ? a : b, (imm >> (2 * k)) & 3);
        return true;
      }
    }
    bool unpack = ins.id == X86_INS_UNPCKLPS || ins.id == X86_INS_UNPCKHPS ||
                  ins.id == X86_INS_VUNPCKLPS || ins.id == X86_INS_VUNPCKHPS;
    if (unpack && v >= 0 && (x.op_count == 2 || x.op_count == 3)) {
      const auto &a = x.op_count == 3 ? src : d,
                 &b = x.operands[x.op_count - 1];
      unsigned start =
          ins.id == X86_INS_UNPCKHPS || ins.id == X86_INS_VUNPCKHPS ? 2 : 0;
      for (unsigned k = 0; k < 4; ++k)
        s.x[v * 4 + k] = old.value(k % 2 ? b : a, start + k / 2);
      return true;
    }
    if ((ins.id == X86_INS_BLENDPS || ins.id == X86_INS_VBLENDPS) && v >= 0 &&
        (x.op_count == 3 || x.op_count == 4) &&
        x.operands[x.op_count - 1].type == X86_OP_IMM) {
      const auto &a = x.op_count == 4 ? src : d,
                 &b = x.operands[x.op_count - 2];
      unsigned imm = unsigned(x.operands[x.op_count - 1].imm);
      for (unsigned k = 0; k < 4; ++k)
        s.x[v * 4 + k] = old.value(imm & (1u << k) ? b : a, k);
      return true;
    }
    bool arithmetic = ins.id == X86_INS_ADDPS || ins.id == X86_INS_VADDPS ||
                      ins.id == X86_INS_SUBPS || ins.id == X86_INS_VSUBPS ||
                      ins.id == X86_INS_MULPS || ins.id == X86_INS_VMULPS ||
                      ins.id == X86_INS_ADDSS || ins.id == X86_INS_VADDSS ||
                      ins.id == X86_INS_SUBSS || ins.id == X86_INS_VSUBSS ||
                      ins.id == X86_INS_MULSS || ins.id == X86_INS_VMULSS;
    if (arithmetic && v >= 0 && (x.op_count == 2 || x.op_count == 3)) {
      bool scalarOp = ins.id == X86_INS_ADDSS || ins.id == X86_INS_VADDSS ||
                      ins.id == X86_INS_SUBSS || ins.id == X86_INS_VSUBSS ||
                      ins.id == X86_INS_MULSS || ins.id == X86_INS_VMULSS;
      const auto &a = x.op_count == 3 ? src : d,
                 &b = x.operands[x.op_count - 1];
      for (unsigned k = 0; k < (scalarOp ? 1u : 4u); ++k)
        s.x[v * 4 + k] =
            old.value(a, k).kind != unknown && old.value(b, k).kind != unknown
                ? Value{computed}
                : Value{};
      if (scalarOp && x.op_count == 3)
        for (unsigned k = 1; k < 4; ++k)
          s.x[v * 4 + k] = old.value(src, k);
      return true;
    }
    return false;
  }
};
} // namespace
Writer writer(const unsigned char *b, std::size_t n) noexcept {
  Writer r{};
  Decode d;
  r.decoded = n <= 1024 && d.run(b, n);
  if (!r.decoded) {
    r.reason = "writer_complete_decode_failed_or_span_over_1024";
    return r;
  }
  Slice p;
  for (std::size_t k = 0; k < d.n; ++k) {
    auto &i = d.p[k];
    if (!p.step(i, true)) {
      r.barrier = unsigned(i.address);
      std::memcpy(r.barrierMnemonic, i.mnemonic, sizeof(r.barrierMnemonic) - 1);
      r.reason =
          "unsupported_or_control_or_memory_effect_before_complete_stores";
      return r;
    }
    if (p.mask == 15) {
      r.prefix = true;
      r.prefixEnd = unsigned(i.address + i.size);
      break;
    }
  }
  r.mask = p.mask;
  r.stores = p.stores;
  if (!r.prefix) {
    r.reason = "incomplete_four_store_prefix";
    return r;
  }
  r.xyz = true;
  for (unsigned k = 0; k < 3; ++k)
    r.xyz &= equal(p.out[k], {memory, 2, std::int64_t(4 * k)});
  auto w = p.out[3];
  r.w = w.kind == constant || w.kind == memory || w.kind == entry;
  r.wBits = w.bits;
  r.wConstant = w.kind == constant;
  r.wSourceReg = w.root;
  r.wSourceOffset = w.off;
  r.wKind = w.kind == constant
                ? (w.bits == 0x3f800000 ? "constant_one" : "computed")
            : w.kind == memory   ? "independent_memory"
            : w.kind == entry    ? "independent_register"
            : w.kind == computed ? "computed"
                                 : "unknown";
  r.reason = r.xyz ? "entry_to_exact_stores_RDX_XYZ_bitcopy_prefix_NOT_whole_"
                     "CFG_or_exclusive_effects"
                   : "XYZ_not_exact_RDX_0_4_8_bitcopy";
  return r;
}
namespace {
bool conditional(unsigned id) {
  switch (id) {
  case X86_INS_JA:
  case X86_INS_JAE:
  case X86_INS_JB:
  case X86_INS_JBE:
  case X86_INS_JE:
  case X86_INS_JNE:
  case X86_INS_JG:
  case X86_INS_JGE:
  case X86_INS_JL:
  case X86_INS_JLE:
  case X86_INS_JO:
  case X86_INS_JNO:
  case X86_INS_JP:
  case X86_INS_JNP:
  case X86_INS_JS:
  case X86_INS_JNS:
    return true;
  default:
    return false;
  }
}
void classifyW(Writer &r, Value w, bool consistent) {
  r.w =
      consistent && (w.kind == constant || w.kind == memory || w.kind == entry);
  r.wConstant = r.w && w.kind == constant;
  r.wBits = w.bits;
  r.wSourceReg = w.root;
  r.wSourceOffset = w.off;
  r.wKind = !consistent ? "unknown"
            : w.kind == constant
                ? (w.bits == 0x3f800000 ? "constant_one" : "computed")
            : w.kind == memory   ? "independent_memory"
            : w.kind == entry    ? "independent_register"
            : w.kind == computed ? "computed"
                                 : "unknown";
}
} // namespace
CallEffect exactCall(const unsigned char *b, std::size_t n,
                     std::uintptr_t writerRva, std::size_t imageSize,
                     unsigned offset, unsigned returnOffset) noexcept {
  CallEffect r{};
  r.writer = writerRva;
  r.call = writerRva + offset;
  r.ret = writerRva + returnOffset;
  Decode d;
  if (offset >= n || returnOffset > n || writerRva > imageSize ||
      n > imageSize - writerRva || n > writerByteCap || !d.run(b, n, writerRva))
    return r;
  for (unsigned i = 0; i < d.n; ++i) {
    const auto &ins = d.p[i];
    if (ins.address != r.call)
      continue;
    const auto &x = ins.detail->x86;
    if (ins.id != X86_INS_CALL || x.op_count != 1 ||
        ins.address + ins.size != r.ret) {
      r.reason = "exact_callsite_or_return_mismatch";
      return r;
    }
    if (x.operands[0].type != X86_OP_IMM) {
      r.kind = "indirect_call";
      r.reason = x.operands[0].type == X86_OP_REG
                     ? "indirect_register_operand_unresolved_no_pointer_chase"
                     : "indirect_memory_operand_unresolved_no_pointer_chase";
      return r;
    }
    r.kind = "direct_call";
    r.target = std::uintptr_t(x.operands[0].imm);
    if (!r.target || r.target >= imageSize ||
        (r.target >= writerRva && r.target < writerRva + n)) {
      r.reason = "direct_target_outside_image_or_recursive_writer";
      return r;
    }
    r.instructionVerified = true;
    r.reason = "exact_direct_call_requires_runtime_boundary_and_current_code";
    return r;
  }
  r.reason = "exact_call_not_instruction_boundary";
  return r;
}
void classifyCallee(CallEffect &r, const unsigned char *b, std::size_t n,
                    std::uintptr_t begin, std::uintptr_t end) noexcept {
  r.begin = begin;
  r.end = end;
  r.noWrite = r.decoded = r.boundaryVerified = false;
  r.gprClobber = r.xmmClobber = 0;
  r.privateStackLow = 0;
  r.barrier = 0;
  std::memset(r.mnemonic, 0, sizeof(r.mnemonic));
  std::memcpy(r.mnemonic, "none", 4);
  r.effect = "source_span_effect_unknown";
  if (!r.instructionVerified || begin != r.target || end <= begin ||
      n != end - begin || n > calleeByteCap) {
    r.reason = "callee_exact_entry_boundary_or_1024_byte_cap_refused";
    return;
  }
  r.boundaryVerified = true;
  Decode d;
  if (!d.run(b, n, begin)) {
    r.barrier = d.n ? d.p[d.n - 1].address + d.p[d.n - 1].size : begin;
    std::memcpy(r.mnemonic, "decode_failure", 15);
    r.reason = "callee_complete_decode_failed";
    return;
  }
  r.decoded = true;
  auto stop = [&](const cs_insn &ins, const char *why) {
    r.barrier = ins.address;
    std::memcpy(r.mnemonic, ins.mnemonic, sizeof(r.mnemonic) - 1);
    r.mnemonic[sizeof(r.mnemonic) - 1] = 0;
    r.reason = why;
  };
  Slice state;
  const State initial;
  for (unsigned k = 0; k < d.n; ++k) {
    const auto &ins = d.p[k];
    const auto &x = ins.detail->x86;
    if (ins.id == X86_INS_RET && x.op_count == 0) {
      if (!equal(state.s.g[4], initial.g[4])) {
        stop(ins, "callee_return_stack_pointer_not_restored");
        return;
      }
      // Restoration is decoded, never assumed from a guessed native ABI.
      for (unsigned j = 0; j < 16; ++j) {
        if (!equal(state.s.g[j], initial.g[j]))
          r.gprClobber |= 1u << j;
        for (unsigned lane = 0; lane < 4; ++lane)
          if (!equal(state.s.x[j * 4 + lane], initial.x[j * 4 + lane]))
            r.xmmClobber |= 1u << j;
      }
      r.gprClobber |= 0xf07; // RAX/RCX/RDX/R8..R11
      r.xmmClobber |= 0x3f;  // XMM0..5, all four tracked lanes
      r.noWrite = true;      // SOURCE span; private stack effects are separate.
      r.effect = "source_span_no_write_proven";
      r.reason = "bounded_leaf_source_disjoint_private_stack_and_decoded_"
                 "register_effects";
      return;
    }
    if (group(ins, CS_GRP_CALL) || group(ins, CS_GRP_JUMP) ||
        group(ins, CS_GRP_RET) || group(ins, CS_GRP_INT) ||
        group(ins, CS_GRP_IRET)) {
      if (k == 0 && ins.id == X86_INS_JMP)
        r.kind = "thunk/wrapper";
      stop(ins, "callee_control_effect_unresolved_no_expansion");
      return;
    }
    bool pure = ins.id == X86_INS_NOP || ins.id == X86_INS_ENDBR64 ||
                ins.id == X86_INS_MOV || ins.id == X86_INS_LEA ||
                ins.id == X86_INS_MOVSS || ins.id == X86_INS_MOVSD ||
                ins.id == X86_INS_MOVAPS || ins.id == X86_INS_MOVUPS ||
                ins.id == X86_INS_MOVDQA || ins.id == X86_INS_MOVDQU ||
                ins.id == X86_INS_VMOVSS || ins.id == X86_INS_VMOVSD ||
                ins.id == X86_INS_VMOVAPS || ins.id == X86_INS_VMOVUPS ||
                ins.id == X86_INS_VMOVDQA || ins.id == X86_INS_VMOVDQU ||
                ins.id == X86_INS_XOR || ins.id == X86_INS_XORPS ||
                ins.id == X86_INS_PXOR || ins.id == X86_INS_ADD ||
                ins.id == X86_INS_SUB || ins.id == X86_INS_CMP ||
                ins.id == X86_INS_TEST || ins.id == X86_INS_ADDSS ||
                ins.id == X86_INS_SUBSS || ins.id == X86_INS_MULSS;
    bool push = ins.id == X86_INS_PUSH && x.op_count == 1 &&
                x.operands[0].type == X86_OP_REG && x.operands[0].size == 8;
    bool pop = ins.id == X86_INS_POP && x.op_count == 1 &&
               x.operands[0].type == X86_OP_REG && x.operands[0].size == 8;
    auto sp = state.s.g[4];
    auto privateWrite = [&](Value a, unsigned width) {
      if (a.kind != pointer || a.root != 4 || sp.kind != pointer ||
          sp.root != 4 || a.off < sp.off || a.off < -4096 ||
          a.off + width > 0 || !width)
        return false;
      r.privateStackLow = std::min(r.privateStackLow, a.off);
      return true;
    };
    if (push) {
      // PUSH allocates exactly its own eight bytes below the current SP.
      auto next = plus(sp, -8);
      if (next.kind != pointer || next.root != 4 || next.off < -4096 ||
          next.off + 8 > 0) {
        stop(ins, "callee_push_outside_bounded_private_stack");
        return;
      }
      r.privateStackLow = std::min(r.privateStackLow, next.off);
    }
    bool memoryWrite = false;
    for (unsigned j = 0; j < x.op_count; ++j)
      if (x.operands[j].type == X86_OP_MEM &&
          ((x.operands[j].access & CS_AC_WRITE) ||
           (j == 0 && ins.id != X86_INS_CMP && ins.id != X86_INS_TEST &&
            ins.id != X86_INS_NOP))) {
        memoryWrite = true;
        if (j != 0 ||
            !privateWrite(state.s.address(x.operands[j]), x.operands[j].size)) {
          stop(ins, "callee_memory_write_alias_to_source_stack_or_destination_"
                    "unresolved");
          return;
        }
      }
    if (!pure && !push && !pop) {
      stop(ins, "callee_implicit_or_unsupported_effect_unresolved");
      return;
    }
    if (pop && (sp.kind != pointer || sp.root != 4 || sp.off < -4096 ||
                sp.off + 8 > 0)) {
      stop(ins, "callee_pop_outside_bounded_private_stack");
      return;
    }
    cs_regs reads{}, writes{};
    uint8_t nr{}, nw{};
    if (cs_regs_access(d.h, &ins, reads, &nr, writes, &nw) != CS_ERR_OK) {
      stop(ins, "callee_register_effect_decode_failed");
      return;
    }
    bool modeled = state.step(ins, false);
    for (unsigned j = 0; j < nw; ++j) {
      if (writes[j] == X86_REG_EFLAGS)
        continue;
      auto g = reg(writes[j]), v = xmm(writes[j]);
      if ((g < 0 && v < 0) || (g == 4 && !modeled)) {
        stop(ins, "callee_stack_pointer_or_unmodeled_register_effect");
        return;
      }
      if (!modeled) {
        if (g >= 0)
          state.s.g[g] = {};
        if (v >= 0)
          for (unsigned lane = 0; lane < 4; ++lane)
            state.s.x[v * 4 + lane] = {};
      }
    }
    auto after = state.s.g[4];
    if (after.kind != pointer || after.root != 4 || after.off < -4096 ||
        after.off > 0 || ((memoryWrite || push || pop) && !modeled)) {
      stop(ins, "callee_stack_or_memory_effect_not_bounded_and_modeled");
      return;
    }
  }
  r.reason = "callee_no_bounded_plain_return";
}
const char *aliasKindName(AliasKind k) noexcept {
  constexpr const char *names[] = {
      "callee_private_stack", "caller_shadow_or_return_area",
      "source_span_exact",    "source_span_possible_alias",
      "hero_transform_exact", "hero_transform_possible_alias",
      "rip_relative_global",  "known_disjoint_object",
      "unknown_pointer"};
  return k < aliasKinds ? names[k] : "unknown_pointer";
}
const char *bitKindName(BitKind k) noexcept {
  switch (k) {
  case BitKind::constant:
    return "constant";
  case BitKind::entryRegister:
    return "register_copy";
  case BitKind::memoryLoad:
    return "memory_load";
  case BitKind::computed:
    return "computed";
  default:
    return "unknown";
  }
}
namespace {
constexpr int globalRoot = 16;
Value aliasAddress(const State &s, const cs_insn &ins, const cs_x86_op &op,
                   std::uintptr_t base) {
  if (ins.detail->x86.addr_size != 8)
    return {};
  if (op.type == X86_OP_MEM && op.mem.base == X86_REG_RIP &&
      op.mem.index == X86_REG_INVALID && op.mem.segment == X86_REG_INVALID &&
      ins.detail->x86.addr_size == 8)
    return {pointer, globalRoot,
            std::int64_t(base + ins.address + ins.size) + op.mem.disp};
  return s.address(op);
}
bool globalLea(Slice &s, const cs_insn &ins, std::uintptr_t base,
               std::size_t imageSize) {
  const auto &x = ins.detail->x86;
  if (ins.id != X86_INS_LEA || x.op_count != 2 ||
      x.operands[0].type != X86_OP_REG || x.operands[0].size != 8 ||
      reg(x.operands[0].reg) < 0 || x.operands[1].type != X86_OP_MEM ||
      x.operands[1].mem.base != X86_REG_RIP)
    return false;
  auto a = aliasAddress(s.s, ins, x.operands[1], base);
  if (a.kind != pointer || a.root != globalRoot || a.off <= 0 ||
      std::uint64_t(a.off) >= imageSize)
    return false;
  s.s.g[reg(x.operands[0].reg)] = a;
  return true;
}
bool overlaps(std::int64_t at, unsigned width, std::int64_t lo,
              std::int64_t hi) {
  return width && at < hi && at + std::int64_t(width) > lo;
}
void copyName(char *out, unsigned cap, const char *in) {
  std::memset(out, 0, cap);
  if (!in)
    in = "none";
  if (in)
    for (unsigned j = 0; j + 1 < cap && in[j]; ++j)
      out[j] = in[j];
}
bool readMayDepend(const AliasRead &read) {
  if (read.root == 1)
    return overlaps(read.offset, read.width, 0x10c, 0x110);
  // Source and stack disjointness is conditional on complete runtime TEB
  // exclusion of Transform+[0,0x110); global disjointness uses image exclusion.
  return read.root != 2 && read.root != 4 && read.root != globalRoot;
}
bool recordRead(CalleeAlias &a, const State &state, const cs_insn &ins,
                std::uintptr_t base) {
  if (!a.exactWrite.site || ins.id == X86_INS_LEA)
    return true;
  auto remember = [&](Value v, unsigned width) {
    AliasRead read{base + ins.address, v.kind == pointer ? v.root : -1, v.off,
                   width};
    if (read.root == 2 || read.root == 4) {
      const auto off = read.offset + (read.root == 2 ? 0x28 : 0);
      if (off < -4096 || off + read.width > 4096)
        read.root = -1;
      else {
        a.stackLow = std::min(a.stackLow, off);
        a.stackHigh = std::max(a.stackHigh, off + read.width);
      }
    }
    // A root16 read is only disjoint if its whole span is inside the image.
    if (read.root == globalRoot) {
      if (read.offset <= 0 || std::uint64_t(read.offset) >= a.imageSize ||
          read.width > a.imageSize - std::uint64_t(read.offset))
        read.root = -1;
      else
        a.usesGlobals = true;
    }
    bool found = false;
    for (unsigned k = 0; k < a.reads; ++k) {
      const auto &old = a.readSites[k];
      found |= old.at == read.at && old.root == read.root &&
               old.offset == read.offset && old.width == read.width;
    }
    if (!found) {
      if (a.reads == aliasReadCap) {
        a.exactWrite.dependencySite = read.at;
        return false;
      }
      a.readSites[a.reads++] = read;
    }
    if (a.exactWrite.modeled && readMayDepend(read)) {
      a.exactWrite.reloadDependency = true;
      a.exactWrite.dependencySite = read.at;
      return false;
    }
    return true;
  };
  // POP has an implicit [RSP] read absent from Capstone's explicit operands.
  // RET's natural return slot is already in the bounded call footprint.
  if (ins.id == X86_INS_POP && !remember(state.g[4], 8))
    return false;
  const auto &x = ins.detail->x86;
  for (unsigned j = 0; j < x.op_count; ++j) {
    const auto &op = x.operands[j];
    if (op.type != X86_OP_MEM ||
        (!(op.access & CS_AC_READ) && (op.access & CS_AC_WRITE)))
      continue;
    if (!remember(aliasAddress(state, ins, op, base), op.size))
      return false;
  }
  return true;
}
bool writeValueExact(const Value &v) {
  return v.kind == constant || (v.kind == entry && v.root >= 16) ||
         ((v.kind == memory || v.kind == memoryWide) &&
          (v.root == 1 || v.root == 2 || v.root == 4 || v.root == globalRoot));
}
void describeWrite(CalleeAlias &a, const State &s, const cs_insn &ins, csh h) {
  auto &t = a.exactWrite;
  if (!t.site || ins.address != t.site)
    return;
  const auto &x = ins.detail->x86;
  t.decoded = true;
  copyName(t.mnemonic, sizeof(t.mnemonic), ins.mnemonic);
  if (x.op_count != 2 || x.operands[0].type != X86_OP_MEM)
    return;
  const auto &dst = x.operands[0], &src = x.operands[1];
  copyName(t.base, sizeof(t.base), cs_reg_name(h, dst.mem.base));
  copyName(t.index, sizeof(t.index), cs_reg_name(h, dst.mem.index));
  t.scale = unsigned(dst.mem.scale);
  t.displacement = dst.mem.disp;
  t.width = dst.size;
  const auto addr = aliasAddress(s, ins, dst, 0);
  const auto val = s.value(src);
  if (t.reachingStates) {
    t.destinationSame &= t.root == (addr.kind == pointer ? addr.root : -1) &&
                         t.offset == addr.off;
    t.valueSame &= t.valueTag == int(val.kind) && t.valueRoot == val.root &&
                   t.valueOffset == val.off && t.valueBits == val.bits;
  } else {
    t.root = addr.kind == pointer ? addr.root : -1;
    t.offset = addr.off;
    t.valueTag = int(val.kind);
    t.valueRoot = val.root;
    t.valueOffset = val.off;
    t.valueBits = val.bits;
  }
  ++t.reachingStates;
  t.destinationExact =
      t.destinationSame && addr.kind == pointer && addr.root == 1;
  t.destinationOrigin = t.destinationExact ? "entry_writer_RCX_transform"
                                           : "unknown_or_conflicting";
  t.valueKind = val.kind == constant                           ? "constant"
                : val.kind == computed                         ? "computed"
                : val.kind == memory || val.kind == memoryWide ? "memory"
                : val.kind == entry && val.root >= 16          ? "register"
                                                               : "unknown";
  copyName(t.sourceOperand, sizeof(t.sourceOperand),
           src.type == X86_OP_REG   ? cs_reg_name(h, src.reg)
           : src.type == X86_OP_IMM ? "immediate"
                                    : "memory_operand");
  t.valueExact = t.valueSame && writeValueExact(val);
  t.positionOverlap =
      !t.destinationExact || overlaps(addr.off, dst.size, 0x30, 0x40);
  // Protect the entire original Transform+[0,64), including the Position rows.
  t.carrierOverlap = !t.destinationExact || overlaps(addr.off, dst.size, 0, 64);
}
// Separate low32 bit identities. These never enter legacy address provenance.
bool sameBits(const BitOrigin &a, const BitOrigin &b) {
  return a.kind == b.kind && a.root == b.root && a.offset == b.offset &&
         a.bits == b.bits && a.addend == b.addend;
}
bool exactBits(const BitOrigin &v) {
  return v.kind == BitKind::constant || v.kind == BitKind::entryRegister ||
         v.kind == BitKind::memoryLoad;
}
BitOrigin produced(BitOrigin v, const cs_insn &ins, const char *kind) {
  v.producer = ins.address;
  v.producerKind = kind;
  if (v.chainSize == valueChainCap) {
    v.kind = BitKind::unknown;
    v.producerKind = "copy_chain_cap";
  } else
    v.chain[v.chainSize++] = ins.address;
  return v;
}
struct BitState {
  std::array<BitOrigin, 16> g{};
  struct Slot {
    BitOrigin value{};
    std::int64_t offset{};
    unsigned width{};
    bool used{};
  };
  std::array<Slot, 128> stack{};
  BitState() {
    for (unsigned j = 0; j < g.size(); ++j) {
      g[j].kind = BitKind::entryRegister;
      g[j].root =
          int(j); // callee-entry raw register bits, NEVER a pointer type
      g[j].producerKind = "callee_entry_register";
    }
  }
  BitOrigin load(Value a, unsigned width, std::int64_t entrySp) const {
    if (a.kind != pointer)
      return {};
    if (a.root == 4) {
      for (const auto &s : stack)
        if (s.used && overlaps(a.off, width, s.offset, s.offset + s.width))
          return a.off == s.offset && width <= s.width && width >= 4
                     ? s.value
                     : BitOrigin{};
    }
    BitOrigin v;
    v.kind = BitKind::memoryLoad;
    v.root = a.root;
    v.offset = a.off - (a.root == 4 ? entrySp : 0);
    return v;
  }
  bool save(Value a, unsigned width, BitOrigin v) {
    if (a.kind != pointer || a.root != 4 || a.off < -4096 ||
        a.off + width > 4096)
      return false;
    for (auto &slot : stack)
      if (slot.used &&
          overlaps(a.off, width, slot.offset, slot.offset + slot.width)) {
        if (a.off <= slot.offset && a.off + width >= slot.offset + slot.width)
          slot.used = false;
        else
          slot.value =
              {}; // Keep an unknown tombstone for the unsaved remainder.
      }
    for (auto &slot : stack)
      if (!slot.used) {
        slot = {v, a.off, width, true};
        return true;
      }
    return false;
  }
  BitOrigin operand(const cs_x86_op &op, const State &state, const cs_insn &ins,
                    std::int64_t entrySp) const {
    if (op.type == X86_OP_IMM) {
      BitOrigin v;
      v.kind = BitKind::constant;
      v.bits = std::uint32_t(op.imm);
      return v;
    }
    if (op.type == X86_OP_REG && reg(op.reg) >= 0 && op.size >= 4)
      return g[reg(op.reg)];
    if (op.type == X86_OP_MEM && op.size >= 4)
      return load(aliasAddress(state, ins, op, 0), op.size, entrySp);
    return {};
  }
  bool step(const cs_insn &ins, const State &state, std::int64_t entrySp) {
    const auto &x = ins.detail->x86;
    if (ins.id == X86_INS_PUSH && x.op_count == 1) {
      if (x.operands[0].type == X86_OP_REG && reg(x.operands[0].reg) == 4)
        return false; // Do not reuse the legacy PUSH-RSP address transfer.
      auto value = operand(x.operands[0], state, ins, entrySp);
      g[4].addend -= 8;
      g[4] = produced(g[4], ins, "computed");
      return save(plus(state.g[4], -8), 8, value);
    }
    if (ins.id == X86_INS_POP && x.op_count == 1 &&
        x.operands[0].type == X86_OP_REG && reg(x.operands[0].reg) >= 0) {
      g[reg(x.operands[0].reg)] =
          produced(load(state.g[4], 8, entrySp), ins, "memory_load");
      g[4].addend += 8;
      g[4] = produced(g[4], ins, "computed");
      return true;
    }
    if (x.op_count == 2 && ins.id == X86_INS_MOV) {
      const auto &dst = x.operands[0], &src = x.operands[1];
      auto v = operand(src, state, ins, entrySp);
      if (dst.type == X86_OP_MEM)
        return save(aliasAddress(state, ins, dst, 0), dst.size, v);
      if (dst.type == X86_OP_REG && reg(dst.reg) >= 0) {
        if (dst.size != 4 && dst.size != 8)
          v = {};
        g[reg(dst.reg)] = produced(v, ins,
                                   src.type == X86_OP_IMM   ? "constant"
                                   : src.type == X86_OP_MEM ? "memory_load"
                                                            : "register_copy");
        return true;
      }
    }
    if (x.op_count >= 2 && x.operands[0].type == X86_OP_REG &&
        reg(x.operands[0].reg) >= 0) {
      auto j = reg(x.operands[0].reg);
      if (ins.id == X86_INS_XOR && x.operands[1].type == X86_OP_REG &&
          x.operands[0].reg == x.operands[1].reg) {
        BitOrigin v;
        v.kind = BitKind::constant;
        g[j] = produced(v, ins, "constant");
        return true;
      }
      if ((ins.id == X86_INS_ADD || ins.id == X86_INS_SUB) &&
          x.operands[0].size == 8 && x.operands[1].type == X86_OP_IMM) {
        auto v = g[j];
        const auto delta = std::uint32_t(x.operands[1].imm) *
                           (ins.id == X86_INS_ADD ? 1u : std::uint32_t(-1));
        if (v.kind == BitKind::constant)
          v.bits += delta;
        else
          v.addend += delta;
        g[j] = produced(v, ins, "computed");
        return true;
      }
      if (ins.id == X86_INS_LEA && x.operands[1].type == X86_OP_MEM &&
          x.operands[1].mem.index == X86_REG_INVALID &&
          x.operands[1].mem.segment == X86_REG_INVALID && x.addr_size == 8 &&
          reg(x.operands[1].mem.base) >= 0 && x.operands[0].size == 8) {
        auto v = g[reg(x.operands[1].mem.base)];
        if (v.kind == BitKind::constant)
          v.bits += std::uint32_t(x.operands[1].mem.disp);
        else
          v.addend += std::uint32_t(x.operands[1].mem.disp);
        g[j] = produced(v, ins, "computed");
        return true;
      }
    }
    // Existing Slice decides supported instruction effects; invalidate every
    // otherwise written GPR here rather than invent a low32 transfer function.
    // The handle-free explicit operand fallback is conservative for all GPR
    // destinations of currently supported Slice instructions (e.g. MOVD).
    if (x.op_count && x.operands[0].type == X86_OP_REG &&
        reg(x.operands[0].reg) >= 0 && ins.id != X86_INS_CMP &&
        ins.id != X86_INS_TEST)
      g[reg(x.operands[0].reg)] = produced({}, ins, "unknown");
    // Other supported stores (e.g. vector saves) invalidate stack carriers.
    for (unsigned j = 0; j < x.op_count; ++j)
      if (x.operands[j].type == X86_OP_MEM &&
          (x.operands[j].access & CS_AC_WRITE))
        if (!save(aliasAddress(state, ins, x.operands[j], 0),
                  x.operands[j].size, {}))
          return false;
    return true;
  }
};
template <class Nodes>
bool ediDefinitions(CalleeAlias &a, Decode &d, const Nodes &nodes,
                    const Slice &input, unsigned targetIndex) {
  auto &v = a.edi;
  v.complete = v.exact = false;
  auto fail = [&](const cs_insn &ins, const char *reason) {
    v.barrier = ins.address;
    copyName(v.mnemonic, sizeof(v.mnemonic), ins.mnemonic);
    v.reason = reason;
    return false;
  };
  const auto &store = d.p[targetIndex];
  const auto &sx = store.detail->x86;
  if (store.id != X86_INS_MOV || sx.op_count != 2 ||
      sx.operands[0].type != X86_OP_MEM || sx.operands[0].size != 4 ||
      sx.operands[1].type != X86_OP_REG || sx.operands[1].reg != X86_REG_EDI)
    return fail(store, "exact_value_site_is_not_MOV_dword_from_EDI");
  std::array<bool, aliasByteCap> relevant{};
  relevant[targetIndex] = true;
  for (unsigned i = unsigned(d.n); i > 0; --i)
    for (unsigned j = 0; j < nodes[i - 1].count; ++j)
      relevant[i - 1] = relevant[i - 1] || relevant[nodes[i - 1].next[j]];
  if (!relevant[0])
    return fail(store, "EDI_store_unreachable_from_callee_entry");
  struct Work {
    Slice slice{};
    BitState bits{};
    unsigned pc{};
  };
  std::array<Work, aliasBranchCeiling + 1> todo{};
  todo[0].slice = input;
  unsigned pending = 1;
  const auto entrySp = input.s.g[4].off;
  while (pending) {
    auto work = todo[--pending];
    for (;;) {
      auto &ins = d.p[work.pc];
      auto &node = nodes[work.pc];
      if (++v.visits > 4096)
        return fail(ins, "EDI_definition_visit_cap");
      if (work.pc == targetIndex) {
        auto bits = work.bits.g[7];
        ++v.reachingStates;
        if (v.reachingStates == 1)
          v.origin = bits;
        else if (!sameBits(v.origin, bits)) {
          v.same = false;
          v.conflict = bits;
          return fail(ins, "EDI_reaching_value_origins_conflict");
        }
        bool seen = false;
        for (unsigned j = 0; j < v.definitions; ++j)
          seen |= v.producers[j].producer == bits.producer &&
                  v.producers[j].chain == bits.chain;
        if (!seen) {
          if (v.definitions == valueDefinitionCap)
            return fail(ins, "EDI_reaching_definition_cap");
          v.producers[v.definitions++] = bits;
        }
        if (!exactBits(bits))
          return fail(
              ins, "EDI_value_unknown_or_unsupported_computation_or_chain_cap");
        ++v.paths;
        break;
      }
      if (node.branch) {
        for (unsigned j = 1; j < node.count; ++j)
          if (relevant[node.next[j]]) {
            if (pending == todo.size())
              return fail(ins, "EDI_pending_state_cap");
            todo[pending] = work;
            todo[pending++].pc = unsigned(node.next[j]);
          }
        if (relevant[node.next[0]]) {
          work.pc = unsigned(node.next[0]);
          continue;
        }
        break;
      }
      if (group(ins, CS_GRP_CALL) || group(ins, CS_GRP_RET) ||
          group(ins, CS_GRP_INT) || group(ins, CS_GRP_IRET))
        return fail(ins, "EDI_prior_control_effect_no_expansion");
      if (!work.bits.step(ins, work.slice.s, entrySp) ||
          !work.slice.step(ins, false))
        return fail(ins, "EDI_prior_instruction_or_stack_effect_unresolved");
      if (!node.count)
        return fail(ins, "EDI_path_ends_before_store");
      if (!relevant[node.next[0]])
        break;
      work.pc = unsigned(node.next[0]);
    }
  }
  v.complete = v.exact = v.same && v.reachingStates;
  v.reason =
      "all_local_EDI_definitions_exact_conditional_on_callee_entry_raw_bits";
  return v.complete;
}
// Tracks possible dependence on one selected read (mask1), a later field reload
// (mask2), and other pre-write field reads (mask4). This is NOT a causal/effect
// certificate. Unsupported effects stop the sidecar without relaxing c19/c20.
struct ReadFlow : ReadVectorTags {
  std::array<unsigned, 16> g{};
  unsigned flags{};
  bool logicalTestFlags{}, pinnedTestFlags{};
  struct Slot { std::int64_t off{}; unsigned width{}, tags{}; bool used{}; };
  std::array<Slot, 128> stack{};
  unsigned load(Value at, unsigned width) const {
    unsigned tags = 0;
    if (at.kind == pointer && at.root == 4)
      for (const auto &s : stack)
        if (s.used && overlaps(at.off, width, s.off, s.off + s.width))
          tags |= s.tags;
    return tags;
  }
  bool save(Value at, unsigned width, unsigned tags) {
    if (at.kind != pointer || at.root != 4 || at.off < -4096 ||
        at.off + width > 4096) return false;
    for (auto &s : stack)
      if (s.used && at.off <= s.off && at.off + width >= s.off + s.width)
        s.used = false;
    // Partial overwrite retains the old tags conservatively, never clears a
    // surviving byte. Over-approximation cannot become an effect permission.
    for (auto &s : stack) if (!s.used) {
      s = {at.off, width, tags, true}; return true;
    }
    return false;
  }
  unsigned get(unsigned r) const {
    if (r == X86_REG_EFLAGS) return flags;
    if (reg(r) >= 0) return g[reg(r)];
    if (r>=X86_REG_YMM0 && r<=X86_REG_YMM15) return ymmRead(r-X86_REG_YMM0);
    return xmm(r) >= 0 ? x[xmm(r)] : 0;
  }
  bool put(unsigned r, unsigned tags) {
    if (r == X86_REG_EFLAGS) flags = tags;
    else if (reg(r) >= 0) g[reg(r)] = tags;
    else if (xmm(r) >= 0) x[xmm(r)] = tags;
    else return false;
    return true;
  }
};
// Only the pinned read-only TEST may inspect a low-byte alias. We retain the
// whole parent's possible-dependence tags; never infer an untainted byte or
// change the general partial-register/write/address model.
int testParent(unsigned r) {
  if (reg(r) >= 0) return reg(r);
  switch (r) {
  case X86_REG_AL: return 0;
  case X86_REG_CL: return 1;
  case X86_REG_DL: return 2;
  case X86_REG_BL: return 3;
  case X86_REG_SPL: return 4;
  case X86_REG_BPL: return 5;
  case X86_REG_SIL: return 6;
  case X86_REG_DIL: return 7;
  case X86_REG_R8B: return 8;
  case X86_REG_R9B: return 9;
  case X86_REG_R10B: return 10;
  case X86_REG_R11B: return 11;
  case X86_REG_R12B: return 12;
  case X86_REG_R13B: return 13;
  case X86_REG_R14B: return 14;
  case X86_REG_R15B: return 15;
  default: return -1;
  }
}
void describeTest(ReadTestContract &t, const cs_insn &ins, csh decoder) {
  const auto &x = ins.detail->x86;
  t.decoded = true; t.operandCount = x.op_count;
  copyName(t.mnemonic, sizeof(t.mnemonic), ins.mnemonic);
  t.formSupported = ins.id == X86_INS_TEST && x.op_count == 2 && !x.prefix[0];
  for (unsigned j = 0; j < std::min<unsigned>(x.op_count, 2); ++j) {
    const auto &op = x.operands[j]; auto &out = t.operands[j];
    out.width = op.size;
    if (op.type == X86_OP_REG) {
      copyName(out.kind, sizeof(out.kind), "register");
      copyName(out.name, sizeof(out.name), cs_reg_name(decoder, op.reg));
      out.parent = testParent(op.reg);
      out.conservativeParent = op.size == 1 && out.parent >= 0;
      t.formSupported &= out.parent >= 0 &&
                         (op.size == 1 || op.size == 4 || op.size == 8);
    } else {
      t.formSupported = false;
      if (op.type == X86_OP_MEM) {
        copyName(out.kind, sizeof(out.kind), "memory");
        copyName(out.base, sizeof(out.base), cs_reg_name(decoder, op.mem.base));
        copyName(out.index, sizeof(out.index), cs_reg_name(decoder, op.mem.index));
        copyName(out.segment, sizeof(out.segment), cs_reg_name(decoder, op.mem.segment));
        out.scale = op.mem.scale; out.displacement = op.mem.disp;
      } else if (op.type == X86_OP_IMM) {
        copyName(out.kind, sizeof(out.kind), "immediate"); out.immediate = op.imm;
      } else copyName(out.kind, sizeof(out.kind), "unsupported");
    }
  }
  t.formSupported &= x.op_count == 2 && x.operands[0].size == x.operands[1].size;
  t.lowByteRead = t.formSupported && x.operands[0].size == 1;
  // Verify the decoder's complete register effects, not only its mnemonic.
  cs_regs reads{}, writes{}; std::uint8_t nr{}, nw{};
  t.flagsOnly = ins.id == X86_INS_TEST &&
      cs_regs_access(decoder, &ins, reads, &nr, writes, &nw) == CS_ERR_OK &&
      nw == 1 && writes[0] == X86_REG_EFLAGS;
  bool registerReadsOnly = true;
  for (unsigned k = 0; k < nr; ++k)
    registerReadsOnly &= x.op_count == 2 &&
        ((x.operands[0].type == X86_OP_REG && reads[k] == x.operands[0].reg) ||
         (x.operands[1].type == X86_OP_REG && reads[k] == x.operands[1].reg));
  t.formSupported &= t.flagsOnly && registerReadsOnly;
  t.reason = ins.id != X86_INS_TEST ? "pinned_instruction_is_not_test"
      : t.formSupported ? "decoded_not_reached_by_flow"
      : "test_partial_or_unmodeled_operand_or_effect";
}
unsigned testBranchTags(unsigned id, unsigned tags) {
  // TEST clears CF/OF. ZF/SF/PF may depend on its AND result; AF is undefined.
  // Both CFG legs remain enumerated even for a constant condition.
  return id == X86_INS_JB || id == X86_INS_JAE ||
         id == X86_INS_JO || id == X86_INS_JNO ? 0u : tags;
}
// Full parent identity is metadata for refused partial reads, not permission.
int cmpParent(unsigned r) {
  const int full = testParent(r);
  if (full >= 0) return full;
  switch (r) {
#define P(R, I) case X86_REG_##R: return I
    P(AX,0); P(CX,1); P(DX,2); P(BX,3); P(SP,4); P(BP,5); P(SI,6); P(DI,7);
    P(R8W,8); P(R9W,9); P(R10W,10); P(R11W,11); P(R12W,12); P(R13W,13);
    P(R14W,14); P(R15W,15); P(AH,0); P(CH,1); P(DH,2); P(BH,3);
#undef P
  default: return -1;
  }
}
ReadCmpContract describeCmp(const cs_insn &ins, csh decoder) {
  ReadCmpContract t{};
  const auto &x = ins.detail->x86;
  t.site = std::uint32_t(ins.address); t.decoded = ins.id == X86_INS_CMP;
  t.operandCount = x.op_count; t.decodedEflags = x.eflags;
  constexpr unsigned parents[] = {X86_REG_RAX,X86_REG_RCX,X86_REG_RDX,X86_REG_RBX,
      X86_REG_RSP,X86_REG_RBP,X86_REG_RSI,X86_REG_RDI,X86_REG_R8,X86_REG_R9,
      X86_REG_R10,X86_REG_R11,X86_REG_R12,X86_REG_R13,X86_REG_R14,X86_REG_R15};
  for (unsigned j=0;j<std::min<unsigned>(x.op_count,2);++j) {
    const auto &op=x.operands[j]; auto &o=t.operands[j];
    o.width=op.size; o.access=op.access;
    if (op.type==X86_OP_REG) {
      copyName(o.kind,sizeof(o.kind),"register");
      copyName(o.name,sizeof(o.name),cs_reg_name(decoder,op.reg));
      o.parent=cmpParent(op.reg); o.conservativeParent=op.size<4 && o.parent>=0;
      o.valueTagsKnown=o.parent>=0;
      if (o.parent>=0) copyName(o.parentName,sizeof(o.parentName),cs_reg_name(decoder,parents[o.parent]));
    } else if (op.type==X86_OP_IMM) {
      copyName(o.kind,sizeof(o.kind),"immediate"); o.immediate=op.imm; o.valueTagsKnown=true;
    } else if (op.type==X86_OP_MEM) {
      copyName(o.kind,sizeof(o.kind),"memory"); t.hasMemory=true;
      copyName(o.base,sizeof(o.base),cs_reg_name(decoder,op.mem.base));
      copyName(o.index,sizeof(o.index),cs_reg_name(decoder,op.mem.index));
      copyName(o.segment,sizeof(o.segment),cs_reg_name(decoder,op.mem.segment));
      o.scale=op.mem.scale; o.displacement=op.mem.disp;
    } else copyName(o.kind,sizeof(o.kind),"unsupported");
  }
  cs_regs reads{},writes{};std::uint8_t nr{},nw{};
  t.flagsOnly=t.decoded && cs_regs_access(decoder,&ins,reads,&nr,writes,&nw)==CS_ERR_OK &&
      nw==1 && writes[0]==X86_REG_EFLAGS;
  for (unsigned j=0;j<x.op_count;++j) t.flagsOnly &= !(x.operands[j].access&CS_AC_WRITE);
  constexpr std::uint64_t cmpFlags = X86_EFLAGS_MODIFY_CF | X86_EFLAGS_MODIFY_PF |
      X86_EFLAGS_MODIFY_AF | X86_EFLAGS_MODIFY_ZF | X86_EFLAGS_MODIFY_SF | X86_EFLAGS_MODIFY_OF;
  t.formSupported=t.flagsOnly && t.site==readIdentityTarget && x.op_count==2 &&
      !x.prefix[0] && !x.prefix[1] && !x.prefix[2] && !x.prefix[3] && x.eflags==cmpFlags &&
      x.operands[0].type==X86_OP_REG && reg(x.operands[0].reg)>=0 &&
      (x.operands[0].size==4 || x.operands[0].size==8) &&
      x.operands[0].size==x.operands[1].size && x.operands[0].access==CS_AC_READ &&
      ((x.operands[1].type==X86_OP_REG && reg(x.operands[1].reg)>=0 && x.operands[1].access==CS_AC_READ) ||
       x.operands[1].type==X86_OP_IMM);
  for (unsigned k=0;k<nr;++k)
    t.formSupported &= x.op_count==2 &&
        ((x.operands[0].type==X86_OP_REG && reads[k]==x.operands[0].reg) ||
         (x.operands[1].type==X86_OP_REG && reads[k]==x.operands[1].reg));
  // Decoder must report every explicit register read, with no hidden inputs.
  for (unsigned j=0;j<x.op_count;++j) if (x.operands[j].type==X86_OP_REG) {
    bool found=false;for (unsigned k=0;k<nr;++k) found |= reads[k]==x.operands[j].reg;
    t.formSupported &= found;
  }
  if (t.formSupported) {
    t.flagsEntryGprMask=1u<<t.operands[0].parent;
    if (x.operands[1].type==X86_OP_REG) {
      t.flagsEntryGprMask |= 1u<<t.operands[1].parent;
      // Identical aliases subtract identical bits: all six flags are constant.
      if (x.operands[0].reg==x.operands[1].reg) t.flagsEntryGprMask=0;
    }
  }
  t.reason=!t.decoded ? "pinned_instruction_is_not_cmp"
      : t.hasMemory ? "pinned_cmp_memory_operand_refused"
      : !t.formSupported ? "pinned_cmp_width_form_or_effect_refused"
      : "pinned_cmp_full_GPR_readonly_flags_supported";
  return t;
}
unsigned carrierTags(std::uint32_t mask, const ReadFlow &flow) {
  unsigned tags=0;
  for (unsigned k=0;k<16;++k) {
    if (mask&(1u<<k)) tags |= flow.g[k];
    if (mask&(1u<<(16+k))) tags |= flow.x[k];
  }
  return tags;
}
void describeLeafBranch(ReadLeafBranch &b, const cs_insn &ins, bool flagsKnown,
                        std::uint32_t source, const ReadFlow &dependencies) {
  b.decoded=true;b.site=std::uint32_t(ins.address);b.conditional=conditional(ins.id);
  copyName(b.mnemonic,sizeof(b.mnemonic),ins.mnemonic);
  b.flagsProducer=source;b.flagsDependencyKnown=b.conditional && flagsKnown;
  b.entryCarrierMask=dependencies.logicalTestFlags
      ? testBranchTags(ins.id,dependencies.flags) : dependencies.flags;
  switch (ins.id) {
#define C(ID, COND, FLAGS) case X86_INS_##ID: b.condition=COND;b.flagsRead=FLAGS;break
    C(JA,"CF0_and_ZF0","CF_ZF"); C(JAE,"CF0","CF"); C(JB,"CF1","CF");
    C(JBE,"CF1_or_ZF1","CF_ZF"); C(JE,"ZF1","ZF"); C(JNE,"ZF0","ZF");
    C(JG,"ZF0_and_SF_eq_OF","ZF_SF_OF"); C(JGE,"SF_eq_OF","SF_OF");
    C(JL,"SF_ne_OF","SF_OF"); C(JLE,"ZF1_or_SF_ne_OF","ZF_SF_OF");
    C(JO,"OF1","OF"); C(JNO,"OF0","OF"); C(JP,"PF1","PF"); C(JNP,"PF0","PF");
    C(JS,"SF1","SF"); C(JNS,"SF0","SF"); C(JMP,"unconditional","none");
#undef C
  default: break;
  }
}
void describeLeafEdges(ReadLeafBranch &b, const cs_insn &ins,
                       std::uint32_t codeBegin, std::uint32_t codeEnd,
                       std::uint32_t limit, std::uint32_t nextRuntime) {
  const auto &x=ins.detail->x86;
  b.instructionBytes=ins.size;
  std::int64_t delta=0;
  if (ins.id==X86_INS_JA && ins.size==2 && ins.bytes[0]==0x77) {
    b.encoding="short_JA_rel8";b.displacementBytes=1;delta=std::int8_t(ins.bytes[1]);
  } else if (ins.id==X86_INS_JA && ins.size==6 && ins.bytes[0]==0x0f && ins.bytes[1]==0x87) {
    std::int32_t v{};std::memcpy(&v,ins.bytes+2,4);
    b.encoding="near_JA_rel32";b.displacementBytes=4;delta=v;
  }
  b.displacement=delta;
  const auto fall=std::int64_t(ins.address)+ins.size, taken=fall+delta;
  b.edgesExact=b.displacementBytes && x.op_count==1 && x.operands[0].type==X86_OP_IMM &&
      !x.prefix[0] && !x.prefix[1] && !x.prefix[2] && !x.prefix[3] &&
      fall>=0 && fall<=UINT32_MAX && taken>=0 && taken<=UINT32_MAX &&
      x.operands[0].imm==taken;
  if (!b.edgesExact) return;
  b.taken=std::uint32_t(taken);b.fallthrough=std::uint32_t(fall);
  b.takenInCode=codeBegin<=b.taken && b.taken<codeEnd;
  b.fallthroughInCode=codeBegin<=b.fallthrough && b.fallthrough<codeEnd;
  b.takenInWindow=readIdentityTarget<=b.taken && b.taken-readIdentityTarget<limit;
  b.fallthroughInWindow=readIdentityTarget<=b.fallthrough && b.fallthrough-readIdentityTarget<limit;
  b.takenForward=b.taken>ins.address;b.fallthroughForward=b.fallthrough>ins.address;
  b.takenRuntimeCross=nextRuntime && b.taken>=nextRuntime;
  b.fallthroughRuntimeCross=nextRuntime && b.fallthrough>=nextRuntime;
}
const char *leafEdgeRefusal(const ReadLeafBranch &b) {
  if (!b.edgesExact) return "target_JA_encoding_or_RVA_overflow_refused";
  if (!b.takenForward || !b.fallthroughForward) return "target_JA_backedge_or_loop_refused";
  if (!b.takenInCode || !b.fallthroughInCode) return "target_JA_edge_outside_code_region";
  if (b.takenRuntimeCross || b.fallthroughRuntimeCross) return "target_JA_edge_crosses_runtime_region";
  if (!b.takenInWindow || !b.fallthroughInWindow) return "target_JA_edge_outside_byte_allowance";
  return nullptr;
}
ReadMovContract describeMov(const cs_insn &ins,csh decoder,const Slice &state,
    const ReadFlow &flow,bool parent,std::size_t imageSize,std::uint32_t codeBegin,std::uint32_t codeEnd) {
  ReadMovContract m{};m.decoded=true;m.isMov=ins.id==X86_INS_MOV;m.site=std::uint32_t(ins.address);
  copyName(m.mnemonic,sizeof(m.mnemonic),ins.mnemonic);
  const auto &x=ins.detail->x86;m.operandCount=x.op_count;m.instructionBytes=ins.size;
  m.addressWidth=x.addr_size;m.decodedEflags=x.eflags;m.parentState=parent;
  m.memoryAccessKnown=true;
  const auto registerName=[&](ReadMovContract::Operand &o,unsigned id) {
    o.parentGpr=cmpParent(id);o.parentXmm=xmm(id);
    copyName(o.name,sizeof(o.name),cs_reg_name(decoder,id));
    copyName(o.kind,sizeof(o.kind),o.parentGpr>=0?"GPR":o.parentXmm>=0?"XMM":"other_register");
    if (o.parentGpr>=0) {
      o.conservativeParent=reg(id)<0;
      // Full parent alias is metadata; partial semantics remain unsupported.
      static constexpr unsigned names[]={X86_REG_RAX,X86_REG_RCX,X86_REG_RDX,X86_REG_RBX,
        X86_REG_RSP,X86_REG_RBP,X86_REG_RSI,X86_REG_RDI,X86_REG_R8,X86_REG_R9,
        X86_REG_R10,X86_REG_R11,X86_REG_R12,X86_REG_R13,X86_REG_R14,X86_REG_R15};
      copyName(o.parentName,sizeof(o.parentName),cs_reg_name(decoder,names[o.parentGpr]));
      const auto v=state.s.g[o.parentGpr];o.originKind=v.kind;o.originRoot=v.root;o.originOffset=v.off;
      if (parent) o.registerTags=flow.g[o.parentGpr];
    } else if (o.parentXmm>=0) {
      copyName(o.parentName,sizeof(o.parentName),o.name);
      if (parent) o.registerTags=flow.x[o.parentXmm];
    }
  };
  for (unsigned j=0;j<x.op_count;++j) {
    const auto &op=x.operands[j];
    if (op.type==X86_OP_MEM) {
      ++m.memoryOperands;m.memoryRead |= (op.access&CS_AC_READ)!=0;m.memoryWrite |= (op.access&CS_AC_WRITE)!=0;
      m.memoryRmw |= (op.access&(CS_AC_READ|CS_AC_WRITE))==(CS_AC_READ|CS_AC_WRITE);
      m.memoryAccessKnown &= op.access!=0;
    }
    if (j>=m.operands.size()) continue;
    auto &o=m.operands[j];o.width=op.size;o.access=op.access;
    if (op.type==X86_OP_REG) registerName(o,op.reg);
    else if (op.type==X86_OP_IMM) {copyName(o.kind,sizeof(o.kind),"immediate");o.immediate=op.imm;}
    else if (op.type==X86_OP_MEM) {
      copyName(o.kind,sizeof(o.kind),"memory");
      copyName(o.base,sizeof(o.base),cs_reg_name(decoder,op.mem.base));
      copyName(o.index,sizeof(o.index),cs_reg_name(decoder,op.mem.index));
      copyName(o.segment,sizeof(o.segment),cs_reg_name(decoder,op.mem.segment));
      o.baseParent=cmpParent(op.mem.base);o.indexParent=cmpParent(op.mem.index);
      o.scale=op.mem.scale;o.displacement=op.mem.disp;
      if (o.baseParent>=0) {
        const auto v=state.s.g[o.baseParent];o.baseKind=v.kind;o.baseRoot=v.root;o.baseOffset=v.off;
        if (parent) o.baseTags=flow.g[o.baseParent];
      }
      if (o.indexParent>=0) {
        const auto v=state.s.g[o.indexParent];o.indexKind=v.kind;o.indexRoot=v.root;o.indexOffset=v.off;
        if (parent) o.indexTags=flow.g[o.indexParent];
      }
      m.addressTags |= o.baseTags|o.indexTags;
      const auto address=aliasAddress(state.s,ins,op,0);
      // Symbolic callee-entry root numbers alone are not typed caller provenance.
      m.addressExact=x.addr_size==8 && address.kind==pointer &&
          op.mem.segment==X86_REG_INVALID && op.mem.index==X86_REG_INVALID &&
          (parent || op.mem.base==X86_REG_RIP);
      if (m.addressExact) {
        m.addressRoot=address.root;m.addressOffset=address.off;
        if (address.root==1) {
          m.aliasMayMask=movHero;m.addressClass="Hero_Transform_conditional";
          if (overlaps(address.off,op.size,0x10c,0x110)) m.aliasMayMask |= movField10c;
        } else if (address.root==2 || address.root==4) {
          const auto off=address.off+(address.root==2?0x28:0);
          const auto sp=state.s.g[4];
          if (off>=-4096 && off+op.size<=4096 && sp.kind==pointer && sp.root==4) {
            m.aliasMayMask=off<sp.off?movPrivateStack|movUnknown:movCallerHome;
            m.addressClass=off<sp.off?"below_entry_SP_unowned_candidate":"caller_or_home_stack_conditional";
            if (overlaps(off,op.size,0x28,0x68)) m.aliasMayMask |= movSource;
          } else {m.addressExact=false;m.addressClass="stack_extent_unproven";}
        } else if (address.root==globalRoot && address.off>=0 && std::uint64_t(address.off)<imageSize &&
            op.size<=imageSize-std::uint64_t(address.off)) {
          m.aliasMayMask=movImage;m.imageRangeKnown=true;m.addressClass="image_address_mutability_unproven";
          m.codeRangeKnown=address.off>=codeBegin && std::uint64_t(address.off)+op.size<=codeEnd;
        } else { m.addressExact=false;m.addressClass="raw_entry_token_or_range_unproven"; }
      } else m.addressClass=op.mem.index!=X86_REG_INVALID?"indexed_address_unproven"
          : op.mem.segment!=X86_REG_INVALID?"segment_address_unproven"
          : !parent?"entry_tokens_not_typed_addresses":"loaded_raw_bits_or_unknown_address";
    } else copyName(o.kind,sizeof(o.kind),"unsupported");
  }
  if (!m.memoryOperands) m.aliasMayMask=0;
  cs_regs reads{},writes{};std::uint8_t nr{},nw{};
  m.effectsKnown=cs_regs_access(decoder,&ins,reads,&nr,writes,&nw)==CS_ERR_OK;
  bool flagsRead=false,flagsWrite=false;
  for (unsigned j=0;j<nr;++j) flagsRead |= reads[j]==X86_REG_EFLAGS;
  for (unsigned j=0;j<nw;++j) {
    if (writes[j]==X86_REG_EFLAGS) {flagsWrite=true;continue;}
    const int g=cmpParent(writes[j]),v=xmm(writes[j]);
    if (g>=0) {
      if (reg(writes[j])>=0) m.fullGprWrites |= 1u<<g;else m.partialGprWrites |= 1u<<g;
      m.stackPointerWrite |= g==4;
    } else if (v>=0) m.xmmWrites |= 1u<<v;
    else ++m.otherRegisterWrites;
  }
  m.flagsPreserved=m.effectsKnown && !flagsRead && !flagsWrite && !x.eflags;
  m.pureLoad=m.isMov && m.effectsKnown && m.flagsPreserved && !m.stackPointerWrite &&
      !m.otherRegisterWrites && x.op_count==2 && x.operands[0].type==X86_OP_REG &&
      x.operands[0].access==CS_AC_WRITE && x.operands[1].type==X86_OP_MEM &&
      x.operands[1].access==CS_AC_READ && m.memoryOperands==1 && !m.memoryWrite;
  // All memory contents/loaded-value tags remain unknown; no semantic state update.
  m.loadedMayTags=m.memoryRead?7u:0u;
  m.reason=m.memoryWrite||m.memoryRmw?"target_MOV_memory_write_or_RMW_refused"
      : m.memoryOperands && m.pureLoad?"target_MOV_loaded_value_provenance_unproven"
      : m.memoryOperands?"target_MOV_memory_form_or_access_unproven"
      : "target_MOV_no_memory_existing_register_rules_only";
  return m;
}
int vectorParent(unsigned id) {
  if (id>=X86_REG_XMM0 && id<=X86_REG_XMM31) return int(id-X86_REG_XMM0);
  if (id>=X86_REG_YMM0 && id<=X86_REG_YMM31) return int(id-X86_REG_YMM0);
  if (id>=X86_REG_ZMM0 && id<=X86_REG_ZMM31) return int(id-X86_REG_ZMM0);
  return -1;
}
ReadVectorContract describeVector(const cs_insn &ins,csh decoder,const Slice &state,
    const ReadFlow &flow,bool parent,bool tagsKnown,std::size_t imageSize,std::uint32_t codeBegin,std::uint32_t codeEnd) {
  ReadVectorContract v{};v.detail=describeMov(ins,decoder,state,flow,parent,imageSize,codeBegin,codeEnd);
  auto &m=v.detail;const auto &x=ins.detail->x86;v.isVmovups=ins.id==X86_INS_VMOVUPS;v.tagsKnown=tagsKnown;
  unsigned at=0;
  while (at<ins.size && (ins.bytes[at]==0x64 || ins.bytes[at]==0x65 || ins.bytes[at]==0x67)) ++at;
  unsigned opcode=0;
  if (at+2<ins.size && ins.bytes[at]==0xc5) {
    v.encoding="VEX2";v.encodingKnown=(ins.bytes[at+1]&3)==0 && ((ins.bytes[at+1]>>3)&15)==15;
    v.encodedBits=128u<<((ins.bytes[at+1]>>2)&1);opcode=ins.bytes[at+2];
  } else if (at+3<ins.size && ins.bytes[at]==0xc4) {
    v.encoding="VEX3";v.encodingKnown=(ins.bytes[at+1]&31)==1 && (ins.bytes[at+2]&3)==0 && ((ins.bytes[at+2]>>3)&15)==15;
    v.encodedBits=128u<<((ins.bytes[at+2]>>2)&1);opcode=ins.bytes[at+3];
  } else if (at+4<ins.size && ins.bytes[at]==0x62) {
    v.encoding="EVEX";const auto p=ins.bytes[at+3];const unsigned length=(p>>5)&3;
    v.encodingKnown=(ins.bytes[at+1]&7)==1 && (ins.bytes[at+2]&7)==4 && length<3;
    v.encodedBits=length<3?128u<<length:0;opcode=ins.bytes[at+4];
    v.mask=p&7;v.masked=v.mask!=0;v.zeroMask=(p&128)!=0;v.broadcast=(p&16)!=0;
  }
  v.encodingKnown &= opcode==0x10 || opcode==0x11;
  v.rounding=x.avx_sae || x.avx_rm!=X86_AVX_RM_INVALID;
  for (unsigned j=0;j<x.op_count && j<v.operands.size();++j) {
    const auto &op=x.operands[j];auto &o=v.operands[j];o.decodedAccess=op.access;o.resolvedAccess=op.access;
    o.broadcast=op.avx_bcast;o.zeroMask=op.avx_zero_opmask;v.broadcast |= op.avx_bcast!=X86_AVX_BCAST_INVALID;
    if (j<2) o.detail=m.operands[j];
    else {o.detail.width=op.size;o.detail.access=op.access;copyName(o.detail.kind,sizeof(o.detail.kind),op.type==X86_OP_MEM?"memory":op.type==X86_OP_REG?"other_register":"unsupported");}
    if (op.type==X86_OP_REG) {
      o.vectorParent=vectorParent(op.reg);copyName(o.detail.name,sizeof(o.detail.name),cs_reg_name(decoder,op.reg));
      if (o.vectorParent>=0) {
        o.vectorBits=op.size*8;v.vectorBits=std::max(v.vectorBits,o.vectorBits);
        copyName(o.detail.kind,sizeof(o.detail.kind),o.vectorBits==128?"XMM":o.vectorBits==256?"YMM":"ZMM");
        copyName(o.detail.parentName,sizeof(o.detail.parentName),o.detail.name);
        if (tagsKnown && o.vectorParent<16) o.detail.registerTags=flow.x[o.vectorParent];
        if (!j) v.destinationParent=o.vectorParent;
      }
    }
    if (op.type==X86_OP_MEM) {
      v.vectorBits=std::max(v.vectorBits,unsigned(op.size)*8);
      copyName(o.detail.base,sizeof(o.detail.base),cs_reg_name(decoder,op.mem.base));copyName(o.detail.index,sizeof(o.detail.index),cs_reg_name(decoder,op.mem.index));
      copyName(o.detail.segment,sizeof(o.detail.segment),cs_reg_name(decoder,op.mem.segment));o.detail.scale=op.mem.scale;o.detail.displacement=op.mem.disp;
      o.detail.baseParent=cmpParent(op.mem.base);o.detail.indexParent=cmpParent(op.mem.index);
      for (unsigned k=0;k<2;++k) {
        const int g=k?o.detail.indexParent:o.detail.baseParent;
        if (g<0) continue;
        const auto a=state.s.g[g];
        if (k) {o.detail.indexKind=a.kind;o.detail.indexRoot=a.root;o.detail.indexOffset=a.off;o.detail.indexTags=tagsKnown?flow.g[g]:0;}
        else {o.detail.baseKind=a.kind;o.detail.baseRoot=a.root;o.detail.baseOffset=a.off;o.detail.baseTags=tagsKnown?flow.g[g]:0;}
      }
      m.addressTags |= o.detail.baseTags|o.detail.indexTags;
      const auto address=aliasAddress(state.s,ins,op,0);
      m.addressExact=x.addr_size==8 && address.kind==pointer && op.mem.segment==X86_REG_INVALID &&
          op.mem.index==X86_REG_INVALID && (parent || op.mem.base==X86_REG_RIP);
      if (m.addressExact) {
        m.addressRoot=address.root;m.addressOffset=address.off;
        if (address.root==1) {m.aliasMayMask=movHero;m.addressClass="Hero_Transform_conditional";
          if (overlaps(address.off,op.size,0x10c,0x110)) m.aliasMayMask |= movField10c;
        } else if (address.root==2 || address.root==4) {
          const auto off=address.off+(address.root==2?0x28:0);const auto sp=state.s.g[4];
          if (off>=-4096 && off+op.size<=4096 && sp.kind==pointer && sp.root==4) {
            m.aliasMayMask=off<sp.off?movPrivateStack|movUnknown:movCallerHome;
            m.addressClass=off<sp.off?"below_entry_SP_unowned_candidate":"caller_or_home_stack_conditional";
            if (overlaps(off,op.size,0x28,0x68)) m.aliasMayMask |= movSource;
          } else {m.addressExact=false;m.addressClass="stack_extent_unproven";}
        } else if (address.root==globalRoot && address.off>=0 && std::uint64_t(address.off)<imageSize && op.size<=imageSize-std::uint64_t(address.off)) {
          m.aliasMayMask=movImage;m.imageRangeKnown=true;m.addressClass="image_address_mutability_unproven";
          m.codeRangeKnown=address.off>=codeBegin && std::uint64_t(address.off)+op.size<=codeEnd;
        } else {m.addressExact=false;m.addressClass="raw_entry_token_or_range_unproven";}
      } else m.addressClass=op.mem.index!=X86_REG_INVALID?"indexed_address_unproven":op.mem.segment!=X86_REG_INVALID?"segment_address_unproven":!parent?"entry_tokens_not_typed_addresses":"loaded_raw_bits_or_unknown_address";
    }
  }
  // Capstone5 can label VMOVUPS opcode11 stores as memory READ. Resolve only
  // exact decoded operand layout against its authenticated encoding; retain both.
  v.encodingKnown &= v.vectorBits==v.encodedBits;
  v.directionKnown=v.isVmovups && v.encodingKnown && m.memoryOperands==1 &&
      ((opcode==0x10 && v.destinationParent>=0 && x.operands[x.op_count-1].type==X86_OP_MEM) ||
       (opcode==0x11 && x.operands[0].type==X86_OP_MEM && x.operands[x.op_count-1].type==X86_OP_REG && vectorParent(x.operands[x.op_count-1].reg)>=0));
  if (v.directionKnown) {
    for (unsigned j=0;j<x.op_count;++j) {
      v.operands[j].resolvedAccess=j?CS_AC_READ:((v.masked && !v.zeroMask && opcode==0x10)?CS_AC_READ|CS_AC_WRITE:CS_AC_WRITE);
      v.accessConflict |= v.operands[j].decodedAccess!=v.operands[j].resolvedAccess;
    }
    m.memoryRead=opcode==0x10;m.memoryWrite=opcode==0x11;m.memoryAccessKnown=true;
  } else if (v.isVmovups && v.encodingKnown && m.memoryOperands && opcode==0x11) m.memoryWrite=true;
  cs_regs reads{},writes{};std::uint8_t nr{},nw{};
  if (cs_regs_access(decoder,&ins,reads,&nr,writes,&nw)==CS_ERR_OK) {
    m.otherRegisterWrites=0;
    for (unsigned j=0;j<nw;++j) {
      const int bank=vectorParent(writes[j]);
      if (bank>=0) v.vectorWrites |= 1u<<bank;
      else if (writes[j]!=X86_REG_EFLAGS && cmpParent(writes[j])<0) ++m.otherRegisterWrites;
    }
  }
  v.destinationRepresented=v.destinationParent>=0 && v.destinationParent<16 && v.vectorBits==128;
  v.upperZero=v.encodingKnown && v.destinationParent>=0;
  v.upperEffect=v.upperZero?"zero_above_encoded_VL":"unknown_or_no_vector_destination";
  // Slice has four32bit lanes/XMM and ReadFlow has one XMM tag bank. No YMM/ZMM
  // upper-state representation exists: no new load semantics can be granted here.
  m.pureLoad=v.directionKnown && x.op_count==2 && !v.masked && !v.zeroMask && !v.broadcast && !v.rounding && m.memoryRead && !m.memoryWrite && !m.memoryRmw && m.flagsPreserved &&
      !m.fullGprWrites && !m.partialGprWrites && !m.stackPointerWrite && !m.otherRegisterWrites;
  m.loadedMayTags=m.memoryRead?readMayTagTop:0;
  m.reason=!v.isVmovups?"target_vector_site_not_VMOVUPS"
      :m.memoryWrite||m.memoryRmw?"target_VMOVUPS_memory_write_or_RMW_refused"
      :!v.encodingKnown?"target_VMOVUPS_encoding_unproven"
      :v.masked||v.zeroMask||v.broadcast||v.rounding?"target_VMOVUPS_EVEX_mask_merge_or_modifier_refused"
      :!std::strcmp(v.encoding,"EVEX")?"target_VMOVUPS_EVEX_not_modeled"
      :m.pureLoad?"target_VMOVUPS_upper_vector_state_not_represented"
      :"target_VMOVUPS_form_or_access_unproven";
  return v;
}
ReadSignExtendContract describeSignExtend(const cs_insn &ins,csh decoder,const Slice &state,
    const ReadFlow &flow,bool parent,bool tagsKnown,std::size_t imageSize,std::uint32_t codeBegin,std::uint32_t codeEnd) {
  ReadSignExtendContract s{};s.detail=describeMov(ins,decoder,state,flow,parent,imageSize,codeBegin,codeEnd);
  auto &m=s.detail;const auto &x=ins.detail->x86;s.isMovsxd=ins.id==X86_INS_MOVSXD;s.tagsKnown=tagsKnown;
  s.encodingKnown=ins.size>=3 && (ins.bytes[0]&0xf8)==0x48 && ins.bytes[1]==0x63 &&
      !x.prefix[0] && !x.prefix[1] && !x.prefix[2] && !x.prefix[3];
  if (s.encodingKnown) s.encoding="REX_W_opcode63";
  if (x.op_count==2) {
    s.sourceBits=x.operands[1].size*8;s.destinationBits=x.operands[0].size*8;
    if (x.operands[0].type==X86_OP_REG) s.destinationParent=reg(x.operands[0].reg);
    if (x.operands[1].type==X86_OP_REG) s.sourceParent=reg(x.operands[1].reg);
    for (unsigned j=0;j<2;++j) {
      auto &op=m.operands[j];if (tagsKnown && op.parentGpr>=0) op.registerTags=flow.g[op.parentGpr];
      if (tagsKnown && op.baseParent>=0) op.baseTags=flow.g[op.baseParent];
      if (tagsKnown && op.indexParent>=0) op.indexTags=flow.g[op.indexParent];
      m.addressTags |= op.baseTags|op.indexTags;
    }
  }
  s.full64=s.destinationParent>=0 && s.destinationBits==64 && m.effectsKnown && m.flagsPreserved &&
      m.fullGprWrites==(1u<<s.destinationParent) && !m.partialGprWrites && !m.xmmWrites && !m.otherRegisterWrites;
  s.registerForm=s.isMovsxd && s.encodingKnown && ins.size==3 && (ins.bytes[2]&0xc0)==0xc0 &&
      x.op_count==2 && s.full64 && s.sourceBits==32 && s.sourceParent>=0 &&
      x.operands[0].access==CS_AC_WRITE && x.operands[1].access==CS_AC_READ && !m.memoryOperands && !m.stackPointerWrite;
  s.sourceTags=tagsKnown && s.sourceParent>=0?flow.g[s.sourceParent]:0;
  s.resultTags=m.memoryRead?readMayTagTop:s.sourceTags;
  m.reason=m.memoryWrite||m.memoryRmw?"target_MOVSXD_memory_write_or_RMW_refused"
      :m.stackPointerWrite?"target_MOVSXD_stack_destination_refused"
      :m.memoryOperands?"target_MOVSXD_memory_source_unproven_no_load_rule"
      :!s.registerForm?"target_MOVSXD_encoding_width_or_effect_unproven"
      :"target_MOVSXD_full64_from_reg32_supported";
  return s;
}
ReadShiftContract describeShift(const cs_insn &ins,csh decoder,const Slice &state,
    const ReadFlow &flow,bool parent,bool tagsKnown,std::size_t imageSize,std::uint32_t codeBegin,std::uint32_t codeEnd) {
  ReadShiftContract s{};s.detail=describeMov(ins,decoder,state,flow,parent,imageSize,codeBegin,codeEnd);
  auto &m=s.detail;const auto &x=ins.detail->x86;s.isShl=ins.id==X86_INS_SHL;s.tagsKnown=tagsKnown;
  unsigned at=0;
  while (at<ins.size && (ins.bytes[at]==0x66 || ins.bytes[at]==0x67 || ins.bytes[at]==0x64 || ins.bytes[at]==0x65)) ++at;
  if (at<ins.size && (ins.bytes[at]&0xf0)==0x40) ++at;
  const unsigned opcode=at<ins.size?ins.bytes[at]:0;
  const bool implicit=opcode==0xd0 || opcode==0xd1, immediate=opcode==0xc0 || opcode==0xc1,
      cl=opcode==0xd2 || opcode==0xd3;
  s.encodingKnown=s.isShl && at+1<ins.size && ((ins.bytes[at+1]>>3)&7)==4 && (implicit||immediate||cl);
  if (s.encodingKnown) s.encoding=implicit?"group2_D0_D1_slash4":immediate?"group2_C0_C1_slash4_imm8":"group2_D2_D3_slash4_CL";
  if (x.op_count==2) {
    s.destinationBits=x.operands[0].size*8;s.destinationParent=x.operands[0].type==X86_OP_REG?cmpParent(x.operands[0].reg):-1;
    s.full64=s.destinationBits==64;s.zeroExtend32=s.destinationBits==32;s.partial=s.destinationBits==8 || s.destinationBits==16;
    s.countMask=s.destinationBits==64?63:(s.destinationBits==8 || s.destinationBits==16 || s.destinationBits==32)?31:0;
    if (s.encodingKnown && implicit && x.operands[1].type==X86_OP_IMM && x.operands[1].imm==1 && !x.encoding.imm_size) {
      s.countSource="implicit_1";s.countKnown=true;s.encodedCount=1;
    } else if (s.encodingKnown && immediate && x.operands[1].type==X86_OP_IMM && x.encoding.imm_size==1 &&
        x.encoding.imm_offset==ins.size-1 && std::uint8_t(x.operands[1].imm)==ins.bytes[ins.size-1]) {
      s.countSource="immediate";s.countKnown=true;s.encodedCountBits=8;s.encodedCount=ins.bytes[ins.size-1];
    } else if (s.encodingKnown && cl && x.operands[1].type==X86_OP_REG && x.operands[1].reg==X86_REG_CL && x.operands[1].access==CS_AC_READ) {
      s.countSource="CL";s.countParent=1;s.flagsDynamic=true;
    }
    for (unsigned j=0;j<2;++j) {
      auto &op=m.operands[j];
      if (tagsKnown && op.parentGpr>=0) op.registerTags=flow.g[op.parentGpr];
      if (tagsKnown && op.baseParent>=0) op.baseTags=flow.g[op.baseParent];
      if (tagsKnown && op.indexParent>=0) op.indexTags=flow.g[op.indexParent];
      m.addressTags |= op.baseTags|op.indexTags;
    }
  }
  constexpr unsigned arithmetic=0x8d5,cf=1,af=0x10,of=0x800;
  if (s.countKnown && s.countMask) {
    s.effectiveCount=s.encodedCount&s.countMask;s.operandUnchanged=!s.effectiveCount;
    s.countClass=!s.effectiveCount?"zero":s.effectiveCount==1?"one":s.effectiveCount<s.destinationBits?"multiple_within_width":"at_or_above_width";
    if (!s.effectiveCount) s.flagsPreserved=arithmetic;
    else {
      s.flagsUndefined=af|(s.effectiveCount>1?of:0)|(s.effectiveCount>=s.destinationBits?cf:0);
      s.flagsDefined=arithmetic&~s.flagsUndefined;
    }
  } else if (s.flagsDynamic) {
    s.countClass="dynamic_masked_count_no_numeric_observation";s.flagsPreserved=arithmetic;
    s.flagsDefined=arithmetic&~af;s.flagsUndefined=af|of|(s.partial?cf:0);
  }
  s.destinationTags=m.memoryOperands?readMayTagTop:tagsKnown && s.destinationParent>=0?flow.g[s.destinationParent]:0;
  s.countTags=tagsKnown && s.countParent>=0?flow.g[s.countParent]:0;
  s.resultTags=s.destinationTags|s.countTags;s.flagsBefore=tagsKnown?flow.flags:0;
  s.definedFlagTags=(s.effectiveCount || s.flagsDynamic)?s.resultTags:0;
  s.flagsAfter=s.countKnown?(s.effectiveCount?s.definedFlagTags|(s.flagsUndefined?readMayTagTop:0):s.flagsBefore):readMayTagTop;
  // Only canonical no-prefix register32/64 constant-count encodings. Decode
  // register effects independently; never let a mnemonic alone grant admission.
  cs_regs reads{},writes{};std::uint8_t nr{},nw{};
  bool effects=cs_regs_access(decoder,&ins,reads,&nr,writes,&nw)==CS_ERR_OK;
  unsigned destinations=0,flagWrites=0;
  for (unsigned j=0;j<nr;++j) effects &= x.op_count==2 && reads[j]==x.operands[0].reg;
  for (unsigned j=0;j<nw;++j) {
    if (writes[j]==X86_REG_EFLAGS) ++flagWrites;
    else if (x.op_count==2 && writes[j]==x.operands[0].reg) ++destinations;
    else effects=false;
  }
  const unsigned rexBytes=x.rex?1:0;
  const bool canonical=s.encodingKnown && !x.prefix[0] && !x.prefix[1] && !x.prefix[2] && !x.prefix[3] &&
      (!x.rex || x.rex==0x40 || x.rex==0x41 || x.rex==0x48 || x.rex==0x49) &&
      at==rexBytes && ins.size==rexBytes+2+(immediate?1:0) && (ins.bytes[at+1]&0xc0)==0xc0;
  s.registerForm=canonical && s.countKnown && (s.full64||s.zeroExtend32) && s.destinationParent>=0 &&
      s.destinationParent!=4 && x.op_count==2 && x.operands[0].type==X86_OP_REG &&
      x.operands[0].access==(CS_AC_READ|CS_AC_WRITE) && !m.memoryOperands && effects && nr==1 &&
      destinations==1 && flagWrites==1 && m.fullGprWrites==(1u<<s.destinationParent) &&
      !m.partialGprWrites && !m.xmmWrites && !m.otherRegisterWrites && !m.stackPointerWrite;
  m.reason=m.memoryOperands?"target_SHL_memory_RMW_refused":m.stackPointerWrite?"target_SHL_stack_destination_refused"
      :s.flagsDynamic?"target_SHL_CL_count_not_statically_known_refused":s.partial?"target_SHL_partial_destination_refused"
      :!s.registerForm?"target_SHL_encoding_count_width_or_effect_unproven":"target_SHL_constant_GPR_form_supported";
  return s;
}
struct LeafCfgResult {
  bool complete{}, effectsDiffer{};
  unsigned size{}, instructions{}, visits{}, paths{};
  std::uint32_t barrier{};
  const char *reason="target_cfg_not_started";
  char mnemonic[32]="none";
  ReadCmpContract cmp{};
  ReadLeafBranch branch{};
  ReadMovContract mov{};
  ReadVectorContract vector{};
  ReadSignExtendContract signExtend{};
  ReadShiftContract shift{};
  ReadLeafBoundary boundary{};
  std::array<ReadLeafOutcome,readLeafPathCap> outcomes{};
};
// Exactly one pinned JA, both forward legs, no generic CFG/recursive expansion.
// With symbolic=true, tags are entry-carrier masks, mapped only through supplied
// current parent-call tags. Caller mode repeats the disjointness check per state.
LeafCfgResult walkPinnedLeaf(const unsigned char *code,unsigned limit,
    std::uint32_t codeBegin,std::uint32_t codeEnd,std::uint32_t nextRuntime,
    const ReadEntryTags *entry,bool symbolic,Slice &merged,ReadFlow &mergedTags,std::size_t imageSize,
    ReadImageLoad *load=nullptr,IdentityRead sourceRead=nullptr,IdentityPageQuery pageQuery=nullptr,void *sourceContext=nullptr,ReadUnknownLoad *unknownLoad=nullptr,bool vector256=false,bool signExtend=false,bool shift=false) {
  static_assert(readLeafBranchCap==1 && readLeafPathCap==2 && readLeafVisitCap==64,"Review bounded fan-out before changing budgets");
  LeafCfgResult out{};
  struct Path { Slice slice{};ReadFlow flow{};unsigned pc{},clobberGpr{},clobberXmm{};
    bool flagsKnown{},actualTags{};std::uint32_t producer{},edgeFrom{},edgeTarget{}; };
  Path *current=nullptr;unsigned currentPath=0;
  // A carrier mask is NOT a may-tag set. Convert exactly once before injecting
  // intrinsic top7; subsequent admitted operations use the actual three-bit lattice.
  auto actualize=[&](Path &p) {
    if (!symbolic || p.actualTags) return true;
    if (!entry || !entry->samples) return false;
    ReadFlow basis;basis.g=entry->gpr;basis.x=entry->xmm;
    for (unsigned k=0;k<16;++k) { p.flow.g[k]=carrierTags(p.flow.g[k],basis);p.flow.x[k]=carrierTags(p.flow.x[k],basis); }
    p.flow.flags=carrierTags(p.flow.flags,basis);p.flow.high=entry->xmmHigh;p.flow.above256Zero=entry->above256Zero;p.actualTags=true;return true;
  };
  auto fail=[&](const cs_insn *ins,std::uint32_t at,const char *why) {
    out.barrier=at;out.reason=why;
    if (current) {
      auto tags=*current;
      out.boundary.path=currentPath;out.boundary.edgeFrom=tags.edgeFrom;out.boundary.edgeTarget=tags.edgeTarget;
      if (actualize(tags)) {out.boundary.tagsKnown=true;out.boundary.gpr=tags.flow.g;out.boundary.xmm=tags.flow.x;out.boundary.xmmHigh=tags.flow.high;out.boundary.above256Zero=tags.flow.above256Zero;out.boundary.vectorTracked=tags.flow.vectorTracked;out.boundary.flagsTags=tags.flow.flags;}
    }
    copyName(out.mnemonic,sizeof(out.mnemonic),ins?ins->mnemonic:"decode");return out;
  };
  if (!code || !limit || limit>readCallByteCap) return fail(nullptr,readIdentityTarget,"target_cfg_byte_cap");
  Decode d;
  if (cs_open(CS_ARCH_X86,CS_MODE_64,&d.h)!=CS_ERR_OK) return fail(nullptr,readIdentityTarget,"target_cfg_decoder_unavailable");
  cs_option(d.h,CS_OPT_DETAIL,CS_OPT_ON);cs_option(d.h,CS_OPT_SKIPDATA,CS_OPT_OFF);
  d.p=cs_malloc(d.h);d.n=1;
  if (!d.p) return fail(nullptr,readIdentityTarget,"target_cfg_decoder_allocation_failed");
  std::array<Path,readLeafPathCap> paths{};
  paths[0].slice=merged;paths[0].flow=mergedTags;
  std::array<std::uint16_t,readCallByteCap> owner{};
  unsigned scheduled=1;
  bool haveReturn=false;
  for (unsigned path=0;path<scheduled;++path) {
    auto &p=paths[path];current=&p;currentPath=path;
    for (;;) {
      const auto at=readIdentityTarget+p.pc;
      if (p.pc>=limit) return fail(nullptr,at,"target_cfg_path_falls_outside_allowance_without_RET");
      if (owner[p.pc] && owner[p.pc]!=p.pc+1) return fail(nullptr,at,"target_cfg_edge_into_instruction_body");
      if (++out.visits>readLeafVisitCap) return fail(nullptr,at,"target_cfg_visit_cap");
      const std::uint8_t *cursor=code+p.pc;std::size_t left=limit-p.pc;std::uint64_t address=at;
      if (!cs_disasm_iter(d.h,&cursor,&left,&address,d.p)) return fail(nullptr,at,"target_cfg_instruction_decode_failed");
      const auto &ins=*d.p;const auto &x=ins.detail->x86;
      for (unsigned j=p.pc;j<p.pc+ins.size;++j)
        if (owner[j] && owner[j]!=p.pc+1) return fail(&ins,at,"target_cfg_edge_into_instruction_body");
      if (!owner[p.pc]) {
        if (++out.instructions>readCallInstructionCap) return fail(&ins,at,"target_cfg_instruction_cap");
        for (unsigned j=p.pc;j<p.pc+ins.size;++j) owner[j]=static_cast<std::uint16_t>(p.pc+1);
      }
      out.size=std::max(out.size,p.pc+ins.size);
      if (ins.id==X86_INS_RET && ins.size==1 && ins.bytes[0]==0xc3 && !x.op_count) {
        if (unknownLoad && unknownLoad->enabled && !actualize(p)) return fail(&ins,at,"target_unknown_load_current_tags_required");
        auto &e=out.outcomes[out.paths++];e.ret=at;e.flagsTags=p.flow.flags;
        e.clobberGpr=p.clobberGpr;e.clobberXmm=p.clobberXmm;e.above256Zero=p.flow.above256Zero;
        for (unsigned k=0;k<16;++k) for (unsigned bit=0;bit<3;++bit) {
          if (p.flow.g[k]&(1u<<bit)) e.returnedGpr[bit] |= 1u<<k;
          if (p.flow.x[k]&(1u<<bit)) e.returnedXmm[bit] |= 1u<<k;
          if (p.flow.high[k]&(1u<<bit)) e.returnedXmmHigh[bit] |= 1u<<k;
        }
        if (!haveReturn) { merged=p.slice;mergedTags=p.flow;haveReturn=true; }
        else {
          for (unsigned k=0;k<16;++k) {
            if (!equal(merged.s.g[k],p.slice.s.g[k]) || mergedTags.g[k]!=p.flow.g[k] || mergedTags.x[k]!=p.flow.x[k] || mergedTags.high[k]!=p.flow.high[k] || mergedTags.above256Zero[k]!=p.flow.above256Zero[k]) out.effectsDiffer=true;
            if (!equal(merged.s.g[k],p.slice.s.g[k])) merged.s.g[k]={};
            mergedTags.g[k] |= p.flow.g[k];mergedTags.x[k] |= p.flow.x[k];mergedTags.high[k] |= p.flow.high[k];mergedTags.above256Zero[k] &= p.flow.above256Zero[k];
            for (unsigned lane=0;lane<4;++lane) {
              auto &v=merged.s.x[k*4+lane];const auto &other=p.slice.s.x[k*4+lane];
              if (!equal(v,other)) { out.effectsDiffer=true;v={}; }
            }
          }
          out.effectsDiffer |= mergedTags.flags!=p.flow.flags ||
              out.outcomes[0].clobberGpr!=e.clobberGpr || out.outcomes[0].clobberXmm!=e.clobberXmm;
          mergedTags.vectorTracked |= p.flow.vectorTracked;mergedTags.flags |= p.flow.flags;
          mergedTags.logicalTestFlags &= p.flow.logicalTestFlags;
          mergedTags.pinnedTestFlags &= p.flow.pinnedTestFlags;
        }
        break;
      }
      if (group(ins,CS_GRP_JUMP)) {
        if (at!=readIdentityBranch || ins.id!=X86_INS_JA || out.branch.expanded)
          return fail(&ins,at,"target_cfg_branch_budget_or_unmodeled_transfer");
        describeLeafBranch(out.branch,ins,p.flagsKnown,p.producer,p.flow);
        describeLeafEdges(out.branch,ins,codeBegin,codeEnd,limit,nextRuntime);
        if (const auto why=leafEdgeRefusal(out.branch)) return fail(&ins,at,why);
        cs_regs reads{},writes{};std::uint8_t nr{},nw{};
        if (cs_regs_access(d.h,&ins,reads,&nr,writes,&nw)!=CS_ERR_OK || nr!=1 || reads[0]!=X86_REG_EFLAGS || nw ||
            !p.flagsKnown || p.producer!=readIdentityTarget || !out.cmp.modelApplied ||
            out.cmp.operands[0].width!=4 || out.cmp.operands[0].parent!=8 ||
            std::strcmp(out.cmp.operands[1].kind,"immediate") || out.cmp.operands[1].immediate!=0x3fff)
          return fail(&ins,at,"target_JA_exact_CMP_flags_producer_required");
        if (symbolic && (!entry || !entry->samples)) return fail(&ins,at,"target_JA_current_call_tags_required");
        ReadFlow actual;
        if (symbolic) { actual.g=entry->gpr;actual.x=entry->xmm; }
        const unsigned tags=symbolic?carrierTags(out.branch.entryCarrierMask,actual):p.flow.flags;
        out.branch.samples=1;out.branch.flagsTags=tags;
        if (tags&7u) return fail(&ins,at,"target_JA_tracked_condition_dependence_refused");
        if (scheduled>=readLeafPathCap) return fail(&ins,at,"target_cfg_path_cap");
        paths[scheduled]=p;paths[scheduled].edgeFrom=at;paths[scheduled].edgeTarget=out.branch.taken;
        paths[scheduled++].pc=out.branch.taken-readIdentityTarget;
        p.edgeFrom=at;p.edgeTarget=out.branch.fallthrough;
        p.pc=out.branch.fallthrough-readIdentityTarget;out.branch.expanded=true;
        continue;
      }
      if (group(ins,CS_GRP_CALL) || group(ins,CS_GRP_RET) || group(ins,CS_GRP_INT) || group(ins,CS_GRP_IRET))
        return fail(&ins,at,"target_cfg_nested_call_or_transfer_refused");
      if (at==readIdentityTarget && ins.id==X86_INS_CMP) {
        out.cmp=describeCmp(ins,d.h);
        if (!out.cmp.formSupported) return fail(&ins,at,out.cmp.reason);
        p.flow.flags=carrierTags(out.cmp.flagsEntryGprMask,p.flow);p.flow.logicalTestFlags=p.flow.pinnedTestFlags=false;
        p.flagsKnown=true;p.producer=at;out.cmp.modelApplied=true;
        p.pc+=ins.size;continue;
      }
      if (at==readIdentityMov) {
        out.mov=describeMov(ins,d.h,p.slice,p.flow,!symbolic,imageSize,codeBegin,codeEnd);
        if (out.mov.memoryOperands) {
          const bool exact=ins.id==X86_INS_MOV && ins.size==7 && x.op_count==2 && x.addr_size==8 &&
              !x.prefix[0] && !x.prefix[1] && !x.prefix[2] && !x.prefix[3] &&
              x.operands[0].type==X86_OP_REG && x.operands[0].reg==X86_REG_RAX && x.operands[0].size==8 &&
              x.operands[1].type==X86_OP_MEM && x.operands[1].size==8 && x.operands[1].mem.base==X86_REG_RCX &&
              x.operands[1].mem.index==X86_REG_INVALID && x.operands[1].mem.segment==X86_REG_INVALID &&
              x.operands[1].mem.disp==0x100 && out.mov.pureLoad;
          const auto base=p.slice.s.g[1];
          const bool addressProven=symbolic?entry && entry->imageLoadAddressVerified &&
              entry->imageLoadSource==readImageSource && !entry->gpr[1]
              : base.kind==pointer && base.root==globalRoot && base.off==readImageBaseOrigin && !p.flow.g[1];
          if (!exact || !addressProven || !load) return fail(&ins,at,out.mov.reason);
          if (unknownLoad && unknownLoad->enabled) {
            if (symbolic && !beginPinnedUnknownLoad(*unknownLoad,*load,std::uint32_t(imageSize),sourceRead,pageQuery,sourceContext))
              return fail(&ins,at,unknownLoad->reason);
            if (!symbolic && (!unknownLoad->admitted || !unknownLoad->targetCodeRechecked || !unknownLoad->metadataSame || !unknownLoad->pageSame))
              return fail(&ins,at,"target_unknown_load_current_metadata_proof_required");
            if (!actualize(p)) return fail(&ins,at,"target_unknown_load_current_tags_required");
            p.slice.s.g[0]={unknown,globalRoot,readImageSource};p.flow.g[0]=readMayTagTop;p.clobberGpr |= 1;
            out.mov.loadedMayTags=readMayTagTop;out.mov.reason="target_MOV_exact_source_unknown_may_top_modeled";
            p.pc+=ins.size;continue;
          }
          if (symbolic) *load=beginReadImageLoad(true,readImageSource,std::uint32_t(imageSize),sourceRead,pageQuery,sourceContext);
          if (symbolic?!load->firstRead:!load->admitted || !load->stable || !load->targetCodeRechecked || !load->firstRead || !load->finalRead || load->source!=readImageSource)
            return fail(&ins,at,load->reason);
          p.slice.s.g[0]={memoryWide,globalRoot,readImageSource};p.flow.g[0]=0;p.clobberGpr |= 1;
          out.mov.loadedMayTags=0;out.mov.reason="target_MOV_exact_image_load_conditionally_modeled";
          p.pc+=ins.size;continue;
        }
      }
      if (at==readIdentityVector && ins.id==X86_INS_VMOVUPS) {
        out.vector=describeVector(ins,d.h,p.slice,p.flow,!symbolic,!symbolic||p.actualTags,imageSize,codeBegin,codeEnd);
        if (vector256) {
          auto &v=out.vector;auto &m=v.detail;
          const auto source=p.slice.s.g[2];
          v.sourceVerified=symbolic?entry && entry->samples && entry->vectorSourceVerified && !entry->gpr[2]
              :source.kind==pointer && source.root==1 && source.off==0 && !p.flow.g[2];
          const bool exact=ins.size==4 && ins.bytes[0]==0xc5 && ins.bytes[1]==0xfc && ins.bytes[2]==0x10 && ins.bytes[3]==0x02 &&
              x.op_count==2 && x.addr_size==8 && !x.prefix[0] && !x.prefix[1] && !x.prefix[2] && !x.prefix[3] &&
              v.encodingKnown && v.encodedBits==256 && v.vectorBits==256 && v.destinationParent==0 &&
              x.operands[0].type==X86_OP_REG && x.operands[0].reg==X86_REG_YMM0 && x.operands[0].size==32 && x.operands[0].access==CS_AC_WRITE &&
              x.operands[1].type==X86_OP_MEM && x.operands[1].size==32 && x.operands[1].access==CS_AC_READ &&
              x.operands[1].mem.base==X86_REG_RDX && x.operands[1].mem.index==X86_REG_INVALID &&
              x.operands[1].mem.segment==X86_REG_INVALID && x.operands[1].mem.scale==1 && !x.operands[1].mem.disp &&
              m.pureLoad && !m.memoryWrite && !m.memoryRmw && !v.masked && !v.zeroMask && !v.broadcast && !v.rounding;
          if (exact && v.sourceVerified && unknownLoad && unknownLoad->enabled && actualize(p)) {
            p.flow.pinnedYmm0Unknown();for (unsigned k=0;k<4;++k) p.slice.s.x[k]={};p.clobberXmm |= 1;
            v.modeled=v.destinationRepresented=v.upperStateRepresented=true;
            m.addressExact=m.addressSame=true;m.addressRoot=1;m.addressOffset=0;m.aliasMayMask=movHero;
            m.addressClass="Hero_Transform_conditional";m.loadedMayTags=readMayTagTop;
            m.reason="target_VMOVUPS_exact_YMM0_unknown_halves_modeled";p.pc+=ins.size;continue;
          }
          if (exact && !v.sourceVerified) m.reason="target_VMOVUPS_current_Hero_source_not_proven";
        }
        return fail(&ins,at,out.vector.detail.reason);
      }
      if (at==readIdentitySignExtend && ins.id==X86_INS_MOVSXD) {
        auto &s=out.signExtend;s=describeSignExtend(ins,d.h,p.slice,p.flow,!symbolic,!symbolic||p.actualTags,imageSize,codeBegin,codeEnd);
        if (!signExtend || !s.registerForm) {
          if (!signExtend && s.registerForm) s.detail.reason="target_MOVSXD_diagnostic_only_semantics_disabled";
          return fail(&ins,at,s.detail.reason);
        }
        if (!actualize(p)) return fail(&ins,at,"target_MOVSXD_current_tags_required");
        const auto sourceTags=p.flow.g[s.sourceParent];
        s.detail.operands[0].registerTags=p.flow.g[s.destinationParent];s.detail.operands[1].registerTags=sourceTags;
        p.slice.s.g[s.destinationParent]={unknown,-1,0};p.flow.g[s.destinationParent]=sourceTags;
        p.clobberGpr |= 1u<<s.destinationParent;s.modeled=s.tagsKnown=true;s.sourceTags=s.resultTags=sourceTags;
        s.detail.reason="target_MOVSXD_pinned_reg32_signextend_full64_modeled";p.pc+=ins.size;continue;
      }
      if (at==readIdentityShift && ins.id==X86_INS_SHL) {
        auto &s=out.shift;s=describeShift(ins,d.h,p.slice,p.flow,!symbolic,!symbolic||p.actualTags,imageSize,codeBegin,codeEnd);
        if (!shift || !s.registerForm) {
          if (!shift && s.registerForm) s.detail.reason="target_SHL_diagnostic_only_semantics_disabled";
          return fail(&ins,at,s.detail.reason);
        }
        if (!actualize(p)) return fail(&ins,at,"target_SHL_current_tags_required");
        s=describeShift(ins,d.h,p.slice,p.flow,!symbolic,true,imageSize,codeBegin,codeEnd);
        // At count0 retain the operand and flags. A 32-bit destination still
        // zero-extends its parent; drop pointer provenance even for count0.
        if (s.effectiveCount || s.zeroExtend32) {
          p.slice.s.g[s.destinationParent]={unknown,-1,0};p.flow.g[s.destinationParent]=s.resultTags;
          p.clobberGpr |= 1u<<s.destinationParent;
        }
        if (s.effectiveCount) {
          p.flow.flags=s.flagsAfter;p.flow.logicalTestFlags=p.flow.pinnedTestFlags=false;
          p.flagsKnown=true;p.producer=at;
        }
        s.modeled=true;s.detail.reason="target_SHL_pinned_constant_GPR_flags_top_modeled";p.pc+=ins.size;continue;
      }
      for (unsigned j=0;j<x.op_count;++j) if (x.operands[j].type==X86_OP_MEM)
        return fail(&ins,at,"target_cfg_memory_operand_refused");
      if (ins.id==X86_INS_NOP || ins.id==X86_INS_ENDBR64) { p.pc+=ins.size;continue; }
      cs_regs reads{},writes{};std::uint8_t nr{},nw{};
      if (cs_regs_access(d.h,&ins,reads,&nr,writes,&nw)!=CS_ERR_OK) return fail(&ins,at,"target_cfg_register_effects_unknown");
      unsigned incoming=0;
      for (unsigned j=0;j<nr;++j) {
        if (reads[j]==X86_REG_EFLAGS || (reg(reads[j])<0 && xmm(reads[j])<0)) return fail(&ins,at,"target_cfg_partial_or_flag_input_refused");
        incoming |= p.flow.get(reads[j]);
      }
      for (unsigned j=0;j<nw;++j)
        if (reg(writes[j])==4 || (writes[j]!=X86_REG_EFLAGS && reg(writes[j])<0 && xmm(writes[j])<0))
          return fail(&ins,at,"target_cfg_stack_or_partial_write_refused");
      const bool pair=x.op_count==2 && x.operands[0].type==X86_OP_REG && x.operands[1].type==X86_OP_REG;
      const bool scalar=pair && reg(x.operands[0].reg)>=0 && reg(x.operands[1].reg)>=0 &&
          (x.operands[0].size==4 || x.operands[0].size==8) && x.operands[0].size==x.operands[1].size;
      const bool move=pair && (ins.id==X86_INS_MOV || ins.id==X86_INS_MOVD || ins.id==X86_INS_MOVAPS || ins.id==X86_INS_MOVUPS);
      const bool zero=pair && x.operands[0].reg==x.operands[1].reg && (ins.id==X86_INS_XOR || ins.id==X86_INS_XORPS || ins.id==X86_INS_PXOR);
      const bool flags=scalar && (ins.id==X86_INS_CMP || ins.id==X86_INS_TEST);
      if (x.prefix[0] || (!move && !zero && !flags) || !p.slice.step(ins,false)) return fail(&ins,at,"target_cfg_instruction_effect_refused");
      if (zero || (flags && ins.id==X86_INS_CMP && x.operands[0].reg==x.operands[1].reg)) incoming=0;
      for (unsigned j=0;j<nw;++j) {
        if (!p.flow.put(writes[j],incoming)) return fail(&ins,at,"target_cfg_register_effects_unknown");
        if (reg(writes[j])>=0) p.clobberGpr |= 1u<<reg(writes[j]);
        if (xmm(writes[j])>=0) p.clobberXmm |= 1u<<xmm(writes[j]);
        if (writes[j]==X86_REG_EFLAGS) {
          p.flow.logicalTestFlags=ins.id==X86_INS_TEST || (zero && ins.id==X86_INS_XOR);p.flow.pinnedTestFlags=false;
          p.flagsKnown=true;p.producer=at;
        }
      }
      p.pc+=ins.size;
    }
  }
  out.complete=out.paths==scheduled && out.branch.expanded;
  out.reason=out.complete?"target_all_JA_paths_normal_RET_effects_modeled":"target_cfg_incomplete";
  return out;
}
void sampleCmpBoundary(ReadTargetIdentity &t, const ReadFlow &flow) {
  // Partial stable prefix evidence cannot grant identity/effect admission.
  if (!t.enabled || t.target!=readIdentityTarget || !t.tableValid || !t.lookupQueried || !t.stable) return;
  auto &cmp=t.cmp;
  if (cmp.decoded && cmp.site==readIdentityTarget) {
    ++cmp.samples;
    for (auto &op:cmp.operands) if (op.parent>=0) op.parentTags |= flow.g[op.parent];
    if (cmp.formSupported) cmp.flagsTags |= carrierTags(cmp.flagsEntryGprMask,flow);
  }
  if (t.branch.decoded && t.branch.flagsDependencyKnown) {
    ++t.branch.samples;t.branch.flagsTags |= carrierTags(t.branch.entryCarrierMask,flow);
  }
}
void sampleMovBoundary(ReadTargetIdentity &t,const Slice &caller,const ReadFlow &flow,std::size_t imageSize) {
  if (!t.enabled || t.target!=readIdentityTarget || !t.tableValid || !t.stable ||
      !t.branch.expanded || !t.mov.decoded || t.mov.site!=readIdentityMov ||
      t.scanBytes<=readIdentityMov-readIdentityTarget) return;
  Decode d;
  if (cs_open(CS_ARCH_X86,CS_MODE_64,&d.h)!=CS_ERR_OK) return;
  cs_option(d.h,CS_OPT_DETAIL,CS_OPT_ON);cs_option(d.h,CS_OPT_SKIPDATA,CS_OPT_OFF);
  d.p=cs_malloc(d.h);d.n=1;if (!d.p) return;
  const std::uint8_t *cursor=t.snapshot.data()+readIdentityMov-readIdentityTarget;
  std::size_t left=t.scanBytes-(readIdentityMov-readIdentityTarget);std::uint64_t address=readIdentityMov;
  if (!cs_disasm_iter(d.h,&cursor,&left,&address,d.p) || d.p->size!=t.mov.instructionBytes) return;
  auto child=caller;child.s.g[4]=plus(caller.s.g[4],-8); // existing natural CALL delta, no live stack read.
  auto fresh=describeMov(*d.p,d.h,child,flow,true,imageSize,t.codeBegin,t.codeEnd);
  if (t.unknownLoad.admitted && fresh.pureLoad && fresh.addressExact && fresh.addressRoot==globalRoot &&
      fresh.addressOffset==readImageSource && !fresh.addressTags) {
    fresh.loadedMayTags=readMayTagTop;fresh.reason="target_MOV_exact_source_unknown_may_top_modeled";
  }
  if (t.load.admitted && fresh.pureLoad && fresh.addressExact && fresh.addressRoot==globalRoot &&
      fresh.addressOffset==readImageSource && !fresh.addressTags) {
    fresh.loadedMayTags=0;fresh.reason="target_MOV_exact_current_stable_image_load_modeled";
  }
  auto &m=t.mov;
  if (!m.samples) {m=fresh;m.samples=1;return;}
  ++m.samples;
  m.addressSame &= fresh.addressExact==m.addressExact && fresh.addressRoot==m.addressRoot &&
      fresh.addressOffset==m.addressOffset && !std::strcmp(fresh.addressClass,m.addressClass);
  m.aliasMayMask |= fresh.aliasMayMask;m.addressTags |= fresh.addressTags;m.loadedMayTags |= fresh.loadedMayTags;
  for (unsigned j=0;j<2;++j) {
    m.operands[j].registerTags |= fresh.operands[j].registerTags;
    m.operands[j].baseTags |= fresh.operands[j].baseTags;m.operands[j].indexTags |= fresh.operands[j].indexTags;
  }
  if (!m.addressSame) {m.addressExact=false;m.aliasMayMask |= movUnknown;m.addressClass="conflicting_analyzed_addresses";}
}
void sampleVectorBoundary(ReadTargetIdentity &t,const Slice &caller,const ReadFlow &flow,std::size_t imageSize) {
  auto &v=t.vector;
  if (!t.enabled || !t.tableValid || !t.stable || !t.unknownLoad.admitted || !t.branch.expanded ||
      !v.detail.decoded || v.detail.site!=readIdentityVector || (!t.vector256Enabled && t.barrier!=readIdentityVector) ||
      !t.scanBytes) return;
  if (v.replayVisits>4096-readLeafVisitCap) {
    ++v.replayRefused;v.detail.addressExact=v.detail.addressSame=false;v.detail.aliasMayMask=movAnyAlias;
    v.detail.addressClass="vector_parent_replay_budget_exhausted";return;
  }
  auto child=caller;child.s.g[4]=plus(caller.s.g[4],-8);auto tags=flow;
  auto load=t.load;auto unknown=t.unknownLoad;
  const auto cfg=walkPinnedLeaf(t.snapshot.data(),t.scanBytes,t.codeBegin,t.codeEnd,t.nextRuntime,
      nullptr,false,child,tags,imageSize,&load,nullptr,nullptr,nullptr,&unknown,t.vector256Enabled,t.signExtendEnabled,t.shiftEnabled);
  if (cfg.signExtend.detail.decoded) {
    auto fresh=cfg.signExtend;auto &old=t.signExtend;fresh.detail.samples=old.detail.samples+1;
    if (old.detail.samples) {
      fresh.sourceTags |= old.sourceTags;fresh.resultTags |= old.resultTags;
      fresh.detail.addressSame=old.detail.addressSame && fresh.detail.addressExact==old.detail.addressExact &&
          fresh.detail.addressRoot==old.detail.addressRoot && fresh.detail.addressOffset==old.detail.addressOffset;
      fresh.detail.addressTags |= old.detail.addressTags;fresh.detail.aliasMayMask |= old.detail.aliasMayMask;
      for (unsigned j=0;j<2;++j) {fresh.detail.operands[j].registerTags |= old.detail.operands[j].registerTags;
        fresh.detail.operands[j].baseTags |= old.detail.operands[j].baseTags;fresh.detail.operands[j].indexTags |= old.detail.operands[j].indexTags;}
      if (!fresh.detail.addressSame) {fresh.detail.addressExact=false;fresh.detail.aliasMayMask |= movUnknown;fresh.detail.addressClass="conflicting_analyzed_signextend_addresses";}
    }
    t.signExtend=fresh;
  }
  if (cfg.shift.detail.decoded) {
    auto fresh=cfg.shift;const auto &old=t.shift;fresh.detail.samples=old.detail.samples+1;
    if (old.detail.samples) {
      fresh.destinationTags |= old.destinationTags;fresh.countTags |= old.countTags;fresh.resultTags |= old.resultTags;
      fresh.flagsBefore |= old.flagsBefore;fresh.flagsAfter |= old.flagsAfter;fresh.definedFlagTags |= old.definedFlagTags;
      fresh.detail.addressSame=old.detail.addressSame && fresh.detail.addressExact==old.detail.addressExact &&
          fresh.detail.addressRoot==old.detail.addressRoot && fresh.detail.addressOffset==old.detail.addressOffset;
      fresh.detail.addressTags |= old.detail.addressTags;fresh.detail.aliasMayMask |= old.detail.aliasMayMask;
      for (unsigned j=0;j<2;++j) {fresh.detail.operands[j].registerTags |= old.detail.operands[j].registerTags;
        fresh.detail.operands[j].baseTags |= old.detail.operands[j].baseTags;fresh.detail.operands[j].indexTags |= old.detail.operands[j].indexTags;}
      if (!fresh.detail.addressSame) {fresh.detail.addressExact=false;fresh.detail.aliasMayMask |= movUnknown;fresh.detail.addressClass="conflicting_analyzed_shift_addresses";}
    }
    t.shift=fresh;
  }
  const unsigned visits=v.replayVisits+cfg.visits;
  if (!cfg.vector.detail.decoded || (!t.vector256Enabled && cfg.barrier!=readIdentityVector)) {
    v.replayVisits=visits;++v.replayRefused;v.detail.addressExact=v.detail.addressSame=false;
    v.detail.aliasMayMask=movAnyAlias;v.detail.addressClass="current_parent_vector_replay_refused";return;
  }
  auto fresh=cfg.vector;fresh.replayVisits=visits;fresh.replayRefused=v.replayRefused;fresh.detail.samples=v.detail.samples+1;
  if (v.detail.samples || v.replayRefused) {
    fresh.detail.addressSame=v.detail.addressSame && fresh.detail.addressExact==v.detail.addressExact &&
        fresh.detail.addressRoot==v.detail.addressRoot && fresh.detail.addressOffset==v.detail.addressOffset;
    fresh.detail.addressTags |= v.detail.addressTags;fresh.detail.aliasMayMask |= v.detail.aliasMayMask;
    for (unsigned j=0;j<fresh.operands.size();++j) {
      fresh.operands[j].detail.registerTags |= v.operands[j].detail.registerTags;
      fresh.operands[j].detail.baseTags |= v.operands[j].detail.baseTags;fresh.operands[j].detail.indexTags |= v.operands[j].detail.indexTags;
    }
    if (!fresh.detail.addressSame) {fresh.detail.addressExact=false;fresh.detail.aliasMayMask |= movUnknown;fresh.detail.addressClass="conflicting_analyzed_vector_addresses";}
  }
  v=fresh;
}
bool readCallLeaf(CalleeAlias &a, const cs_insn &call, Slice &caller,
                  ReadFlow &flow) {
  auto &c = a.readDependency.call;
  auto fail = [&](std::uintptr_t at, const char *mnemonic, const char *why) {
    if (!c.barrier) {
      c.barrier = at; c.reason = why;
      copyName(c.unresolvedMnemonic, sizeof(c.unresolvedMnemonic), mnemonic);
    }
    return false;
  };
  if (!c.direct || !c.identity.instructionVerified)
    return fail(call.address, call.mnemonic, "read_call_indirect_or_invalid_target_no_chase");
  sampleCmpBoundary(c.targetIdentity,flow);
  sampleMovBoundary(c.targetIdentity,caller,flow,a.imageSize);
  sampleVectorBoundary(c.targetIdentity,caller,flow,a.imageSize);
  if (c.targetIdentity.enabled && !c.targetIdentity.admitted)
    return fail(c.targetIdentity.barrier?c.targetIdentity.barrier:call.address,
                c.targetIdentity.mnemonic,c.targetIdentity.reason);
  const auto begin = c.identity.begin, end = c.identity.end;
  if (c.identity.target==0x351eb00 || (begin<=0x351eb00 && 0x351eb00<end))
    return fail(call.address,call.mnemonic,"read_call_deferred_producer_refused");
  if (end>begin && (end-begin>readCallByteCap ||
      (begin<=call.address && call.address<end)))
    return fail(call.address,call.mnemonic,"read_call_boundary_cap_or_recursion");
  if (!c.authenticated || !c.identity.boundaryVerified || !c.code)
    return fail(call.address, call.mnemonic, "read_call_target_boundary_hash_unavailable");
  if (begin != c.identity.target || end <= begin || end > a.imageSize ||
      end - begin != c.size || c.size > readCallByteCap ||
      (begin <= 0x351eb00 && 0x351eb00 < end) ||
      (begin <= call.address && call.address < end))
    return fail(call.address, call.mnemonic, "read_call_boundary_cap_recursion_or_deferred_target");
  Decode d;
  if (!c.targetIdentity.branch.expanded && (!d.run(c.code, c.size, begin) || d.n > readCallInstructionCap))
    return fail(begin, "decode", "read_call_full_decode_or_instruction_cap");
  auto state = caller; auto tags = flow;
  const auto oldSp = state.s.g[4];
  auto sp = plus(oldSp, -8);
  if (oldSp.kind != pointer || oldSp.root != 4 || sp.kind != pointer ||
      sp.off < -4096 || sp.off + 8 > 4096 || tags.g[4] ||
      overlaps(sp.off, 8, 0, 8) || overlaps(sp.off, 8, 0x28, 0x68))
    return fail(call.address, call.mnemonic, "read_call_return_slot_alias_or_stack_unknown");
  state.s.g[4] = sp;
  if (!state.s.save(sp, {constant,0,0,std::uint32_t(call.address+call.size)},8,0) ||
      !tags.save(sp,8,0))
    return fail(call.address, call.mnemonic, "read_call_stack_slot_cap");
  if (c.targetIdentity.branch.expanded) {
    const auto &proof=c.targetIdentity;
    if (begin!=readIdentityTarget || proof.target!=begin || !proof.enabled || !proof.admitted ||
        !proof.stable || !proof.cfgComplete || !proof.finalCodeRechecked || proof.size!=c.size ||
        proof.branch.site!=readIdentityBranch || !proof.branch.edgesExact ||
        std::memcmp(c.code,proof.snapshot.data(),c.size))
      return fail(begin,"snapshot","read_call_CFG_identity_pin_or_snapshot_mismatch");
    const auto cfg=walkPinnedLeaf(c.code,c.size,c.targetIdentity.codeBegin,c.targetIdentity.codeEnd,
        c.targetIdentity.nextRuntime,nullptr,false,state,tags,a.imageSize,&c.targetIdentity.load,nullptr,nullptr,nullptr,&c.targetIdentity.unknownLoad,c.targetIdentity.vector256Enabled,c.targetIdentity.signExtendEnabled,c.targetIdentity.shiftEnabled);
    c.visits+=cfg.visits;
    if (!cfg.complete || c.visits>4096)
      return fail(cfg.barrier?cfg.barrier:begin,cfg.mnemonic,c.visits>4096?"read_call_cumulative_visit_cap":cfg.reason);
    if (!equal(state.s.g[4],sp)) return fail(begin,"rsp","read_call_stack_not_restored");
    state.s.g[4]=oldSp;
    caller=state;flow=tags;++c.modeledStates;++c.branchExpansions;
    c.returnPaths=cfg.paths;c.pathEffectsDiffer |= cfg.effectsDiffer;c.returnedFlagsTags |= tags.flags;
    for (unsigned p=0;p<cfg.paths;++p) {
      auto &e=c.outcomes[p];const auto &next=cfg.outcomes[p];
      if (c.modeledStates==1) e=next;
      else {
        c.pathEffectsDiffer |= e.flagsTags!=next.flagsTags || e.returnedGpr!=next.returnedGpr || e.returnedXmm!=next.returnedXmm || e.returnedXmmHigh!=next.returnedXmmHigh || e.above256Zero!=next.above256Zero;
        e.flagsTags |= next.flagsTags;e.clobberGpr |= next.clobberGpr;e.clobberXmm |= next.clobberXmm;for (unsigned k=0;k<16;++k) e.above256Zero[k] &= next.above256Zero[k];
        for (unsigned bit=0;bit<3;++bit) { e.returnedGpr[bit] |= next.returnedGpr[bit];e.returnedXmm[bit] |= next.returnedXmm[bit];e.returnedXmmHigh[bit] |= next.returnedXmmHigh[bit]; }
      }
    }
    for (unsigned p=0;p<cfg.paths;++p) {
      c.clobberGpr |= cfg.outcomes[p].clobberGpr;c.clobberXmm |= cfg.outcomes[p].clobberXmm;
      for (unsigned bit=0;bit<3;++bit) { c.returnedGpr[bit] |= cfg.outcomes[p].returnedGpr[bit];c.returnedXmm[bit] |= cfg.outcomes[p].returnedXmm[bit];c.returnedXmmHigh[bit] |= cfg.outcomes[p].returnedXmmHigh[bit]; }
    }
    if (!c.barrier) c.reason=c.pathEffectsDiffer?"bounded_JA_all_RET_paths_conservatively_merged_path_dependent":"bounded_JA_all_RET_paths_same_effect";
    return true;
  }
  for (unsigned i = 0; i < d.n; ++i) {
    const auto &ins = d.p[i]; const auto &x = ins.detail->x86;
    if (++c.visits > 4096)
      return fail(ins.address, ins.mnemonic, "read_call_cumulative_visit_cap");
    if (ins.id == X86_INS_RET && x.op_count == 0) {
      if (!equal(state.s.g[4], sp))
        return fail(ins.address, ins.mnemonic, "read_call_stack_not_restored");
      state.s.g[4] = oldSp;
      caller = state; flow = tags; ++c.modeledStates;
      for (unsigned k = 0; k < 16; ++k)
        for (unsigned bit = 0; bit < 3; ++bit) {
          if (tags.g[k] & (1u << bit)) c.returnedGpr[bit] |= 1u << k;
          if (tags.x[k] & (1u << bit)) c.returnedXmm[bit] |= 1u << k;
          if (tags.high[k] & (1u << bit)) c.returnedXmmHigh[bit] |= 1u << k;
        }
      c.returnedFlagsTags |= tags.flags;
      if (!c.barrier) c.reason = "bounded_register_only_leaf_normal_return";
      return true;
    }
    if (group(ins, CS_GRP_CALL) || group(ins, CS_GRP_JUMP) ||
        group(ins, CS_GRP_RET) || group(ins, CS_GRP_INT) || group(ins, CS_GRP_IRET))
      return fail(ins.address, ins.mnemonic, "read_call_nested_or_control_depth_limit");
    if (ins.id == X86_INS_NOP || ins.id == X86_INS_ENDBR64) continue;
    if (ins.id==X86_INS_CMP && ins.address==readIdentityTarget &&
        c.identity.target==readIdentityTarget && c.targetIdentity.enabled && c.targetIdentity.admitted) {
      const auto cmp=describeCmp(ins,d.h);
      if (!cmp.formSupported) return fail(ins.address,ins.mnemonic,cmp.reason);
      tags.flags=carrierTags(cmp.flagsEntryGprMask,tags);
      tags.logicalTestFlags=tags.pinnedTestFlags=false;
      // Decoded flags-only effect: leave every Slice GPR/XMM/stack and tag intact.
      continue;
    }
    cs_regs reads{}, writes{}; std::uint8_t nr{}, nw{};
    if (cs_regs_access(d.h, &ins, reads, &nr, writes, &nw) != CS_ERR_OK)
      return fail(ins.address, ins.mnemonic, "read_call_register_access_unknown");
    unsigned incoming = 0;
    for (unsigned k = 0; k < nr; ++k) {
      if (reads[k] == X86_REG_EFLAGS ||
          (reg(reads[k]) < 0 && xmm(reads[k]) < 0))
        return fail(ins.address, ins.mnemonic, "read_call_partial_or_flag_input_unmodeled");
      incoming |= tags.get(reads[k]);
    }
    for (unsigned j = 0; j < x.op_count; ++j) if (x.operands[j].type == X86_OP_MEM) {
      const auto &op = x.operands[j]; auto at = aliasAddress(state.s,ins,op,0);
      if (at.kind == pointer && at.root == 2) { at.root = 4; at.off += 0x28; }
      const bool store = (op.access & CS_AC_WRITE) ||
          (j==0 && ins.id!=X86_INS_CMP && ins.id!=X86_INS_TEST);
      if (store && (incoming & 1)) c.memoryEscape = true;
      const char *why = at.kind != pointer ? "read_call_memory_alias_unknown"
          : at.root == 1 ? "read_call_Transform_memory_effect_refused"
          : at.root == 4 && overlaps(at.off,op.size,0x28,0x68)
              ? "read_call_source_span_memory_effect_refused"
          : at.root == 4 && at.off >= sp.off
              ? "read_call_caller_home_or_return_memory_effect_refused"
          : "read_call_memory_effect_outside_register_only_model";
      return fail(ins.address, ins.mnemonic, why);
    }
    bool regPair = x.op_count == 2 && x.operands[0].type == X86_OP_REG &&
                   x.operands[1].type == X86_OP_REG;
    bool scalarPair = regPair && reg(x.operands[0].reg)>=0 && reg(x.operands[1].reg)>=0 &&
        (x.operands[0].size==4 || x.operands[0].size==8) &&
        x.operands[0].size==x.operands[1].size;
    bool move = regPair && (ins.id==X86_INS_MOV || ins.id==X86_INS_MOVD ||
                                ins.id==X86_INS_MOVAPS || ins.id==X86_INS_MOVUPS);
    bool zero = regPair && x.operands[0].reg==x.operands[1].reg &&
        (ins.id==X86_INS_XOR || ins.id==X86_INS_XORPS || ins.id==X86_INS_PXOR);
    bool flags = scalarPair && (ins.id==X86_INS_TEST || ins.id==X86_INS_CMP);
    if (x.prefix[0] || (!move && !zero && !flags))
      return fail(ins.address, ins.mnemonic, "read_call_instruction_outside_register_leaf_model");
    if (zero || (flags && ins.id==X86_INS_CMP && x.operands[0].reg==x.operands[1].reg))
      incoming = 0;
    for (unsigned k = 0; k < nw; ++k) {
      if (reg(writes[k]) == 4 || !tags.put(writes[k], incoming))
        return fail(ins.address, ins.mnemonic, "read_call_partial_or_stack_register_write");
      if (reg(writes[k]) >= 0) c.clobberGpr |= 1u << reg(writes[k]);
      if (xmm(writes[k]) >= 0) c.clobberXmm |= 1u << xmm(writes[k]);
      if (writes[k] == X86_REG_EFLAGS) {
        tags.logicalTestFlags = ins.id==X86_INS_TEST;
        tags.pinnedTestFlags = false;
      }
    }
    if (!state.step(ins,false))
      return fail(ins.address, ins.mnemonic, "read_call_register_provenance_unmodeled");
  }
  return fail(end,"fallthrough","read_call_no_bounded_normal_return");
}
template <class Nodes>
void inspectReadDependency(CalleeAlias &a, Decode &d, const Nodes &nodes,
                           const Slice &input, unsigned writeIndex) {
  auto &r = a.readDependency;
  if (!r.site) return;
  if (!a.exactWrite.site || writeIndex >= d.n) {
    r.reason = "read_dependency_write_anchor_missing";
    r.flowComplete = r.orderComplete = false;
    return;
  }
  if (++r.calls == 1) r.flowComplete = r.orderComplete = true;
  auto stop = [&](const cs_insn &ins, const char *why) {
    r.flowComplete = false;
    if (!r.barrier) {
      r.barrier = ins.address; r.reason = why;
      copyName(r.unresolvedMnemonic, sizeof(r.unresolvedMnemonic), ins.mnemonic);
    }
  };
  unsigned readIndex = unsigned(d.n);
  for (unsigned j = 0; j < d.n; ++j)
    if (d.p[j].address == r.site) readIndex = j;
  if (readIndex == d.n) {
    r.orderComplete = false;
    r.flowComplete = false; r.barrier = r.site;
    r.reason = "selected_read_not_on_authenticated_instruction_boundary";
    copyName(r.unresolvedMnemonic, sizeof(r.unresolvedMnemonic), "instruction_boundary");
    return;
  }
  auto &test = r.test;
  if (test.site) {
    for (unsigned j = 0; j < d.n; ++j)
      if (d.p[j].address == test.site) describeTest(test, d.p[j], d.h);
    if (!test.decoded) test.reason = "test_not_on_authenticated_instruction_boundary";
  }
  auto &nested = r.call;
  if (nested.site) {
    for (unsigned j = 0; j < d.n; ++j) if (d.p[j].address == nested.site) {
      const auto &ins = d.p[j]; nested.decoded = ins.id == X86_INS_CALL;
      copyName(nested.mnemonic,sizeof(nested.mnemonic),ins.mnemonic);
      copyName(nested.operand,sizeof(nested.operand),ins.op_str);
      const auto exact = exactCall(a.code,a.size,d.p[0].address,a.imageSize,
          unsigned(ins.address-d.p[0].address),unsigned(ins.address+ins.size-d.p[0].address));
      const auto old = nested.identity;
      if (old.target != exact.target) nested.authenticated = false;
      nested.identity = exact;
      nested.identity.begin=old.begin; nested.identity.end=old.end;
      nested.identity.boundaryVerified=old.boundaryVerified && old.target==exact.target;
      nested.direct = !std::strcmp(exact.kind,"direct_call");
      if (!nested.samples) nested.reason = "decoded_call_not_reached_by_flow";
    }
  }
  // Ordering is structural, over the fully checked forward local CFG, with
  // CALL fallthrough explicitly conditional on normal return (no expansion).
  struct Order { unsigned pc{}; bool read{}, write{}, after{}; };
  std::array<Order, aliasBranchCeiling + 1> order{};
  unsigned pending = 1;
  while (pending) {
    auto w = order[--pending];
    for (;;) {
      const auto &ins = d.p[w.pc]; const auto &node = nodes[w.pc];
      if (++r.orderVisits > 4096) {
        r.orderComplete = false; stop(ins, "read_order_visit_cap"); return;
      }
      if (w.pc == readIndex) { w.read = true; w.after = w.write; }
      if (w.pc == writeIndex) w.write = true;
      if (node.ret) {
        ++r.orderPaths;
        r.orderMask |= w.read && w.write ? (w.after ? 2u : 1u)
                     : w.read ? 4u : w.write ? 8u : 16u;
        break;
      }
      if (group(ins, CS_GRP_INT) || group(ins, CS_GRP_IRET) ||
          group(ins, CS_GRP_RET) || !node.count) {
        r.orderComplete = false; stop(ins, "read_order_control_unresolved");
        return;
      }
      if (node.count == 2) {
        if (pending == order.size()) {
          r.orderComplete = false; stop(ins, "read_order_pending_cap"); return;
        }
        order[pending] = w; order[pending++].pc = unsigned(node.next[1]);
      }
      w.pc = unsigned(node.next[0]);
    }
  }
  struct Work { Slice slice{}; ReadFlow flow{}; unsigned pc{}; bool wrote{}; };
  std::array<Work, aliasBranchCeiling + 1> todo{};
  todo[0].slice = input; pending = 1;
  while (pending) {
    auto w = todo[--pending];
    for (;;) {
      const auto &ins = d.p[w.pc]; const auto &node = nodes[w.pc];
      const auto &x = ins.detail->x86;
      if (++r.visits > 4096) { stop(ins, "read_flow_visit_cap"); return; }
      if (node.ret) {
        if (!equal(w.slice.s.g[4], input.s.g[4])) {
          stop(ins, "read_flow_stack_not_restored"); break;
        }
        ++r.paths;
        for (unsigned k = 0; k < 16; ++k) {
          if (w.flow.g[k] & 1) r.returnedGpr |= 1u << k;
          if (w.flow.x[k] & 1) r.returnedXmm |= 1u << k;
          if (w.flow.vectorTracked && (w.flow.high[k]&1)) r.returnedXmmHigh |= 1u<<k;
          if (w.flow.g[k] & 2) r.newValueGpr |= 1u << k;
        }
        r.returnedFlags |= (w.flow.flags & 1) != 0;
        if (w.flow.pinnedTestFlags) test.returnedFlagsTags |= w.flow.flags;
        for (const auto &s : w.flow.stack)
          r.returnedCallerStack |= s.used && (s.tags & 1) &&
                                  s.off + s.width > input.s.g[4].off;
        break;
      }
      if (node.branch) {
        const unsigned tags = w.flow.logicalTestFlags
            ? testBranchTags(ins.id, w.flow.flags) : w.flow.flags;
        if (node.cond && w.flow.pinnedTestFlags) {
          ++test.consumerSamples; test.consumerTags |= tags;
          if (!test.firstConsumer) {
            test.firstConsumer = ins.address;
            test.consumerKind = "local_Jcc";
            copyName(test.consumerMnemonic, sizeof(test.consumerMnemonic), ins.mnemonic);
          }
          if (tags && !test.firstDependentConsumer) test.firstDependentConsumer = ins.address;
        }
        if (node.cond && (tags & 1)) {
          r.controlUse = true;
          if (!r.firstControl) r.firstControl = ins.address;
          if (!r.firstUse) r.firstUse = ins.address;
        }
        if (node.count == 2) {
          if (pending == todo.size()) { stop(ins, "read_flow_pending_cap"); return; }
          todo[pending] = w; todo[pending++].pc = unsigned(node.next[1]);
        }
        w.pc = unsigned(node.next[0]); continue;
      }
      if (group(ins, CS_GRP_CALL) && ins.address == nested.site) {
        ++nested.samples;
        for (unsigned k=0;k<16;++k) {
          auto &arg=nested.gpr[k]; const auto value=w.slice.s.g[k];
          if (nested.samples>1 && (arg.kind!=int(value.kind) || arg.root!=value.root ||
              arg.offset!=value.off || arg.bits!=value.bits)) arg.same=false;
          arg.kind=int(value.kind); arg.root=value.root; arg.offset=value.off; arg.bits=value.bits;
          arg.tags |= w.flow.g[k]; nested.xmmTags[k] |= w.flow.x[k];nested.xmmHighTags[k] |= w.flow.high[k];
          nested.above256Zero[k]=nested.samples==1?w.flow.above256Zero[k]:nested.above256Zero[k] && w.flow.above256Zero[k];
        }
        nested.flagsTags |= w.flow.flags;
        for (const auto &slot:w.flow.stack)
          if (slot.used && slot.off+slot.width>w.slice.s.g[4].off)
            nested.callerStackTags |= slot.tags;
        if (!readCallLeaf(a,ins,w.slice,w.flow)) {
          if (nested.memoryEscape) {
            r.memoryEscape=true;
            if (!r.firstEscape) r.firstEscape=nested.barrier;
          }
          r.flowComplete=false;
          if (!r.barrier) {
            r.barrier=nested.barrier; r.reason=nested.reason;
            copyName(r.unresolvedMnemonic,sizeof(r.unresolvedMnemonic),nested.unresolvedMnemonic);
          }
          break;
        }
        if (!node.count) { stop(ins,"read_flow_call_fallthrough"); break; }
        w.pc=unsigned(node.next[0]); continue;
      }
      if (group(ins, CS_GRP_CALL) || group(ins, CS_GRP_INT) ||
          group(ins, CS_GRP_IRET) || group(ins, CS_GRP_RET)) {
        stop(ins, "read_flow_call_or_control_no_expansion"); break;
      }
      if (ins.id == X86_INS_NOP || ins.id == X86_INS_ENDBR64 ||
          ins.id == X86_INS_VZEROUPPER) {
        if (!node.count) { stop(ins, "read_flow_fallthrough"); break; }
        w.pc = unsigned(node.next[0]); continue;
      }
      if (test.site && ins.address == test.site) {
        ++test.samples;
        unsigned tags = 0;
        for (unsigned j = 0; j < 2; ++j) {
          auto &op = test.operands[j];
          if (op.parent >= 0) {
            const auto parentTags = w.flow.g[op.parent];
            op.parentTags |= parentTags; tags |= parentTags;
          }
        }
        if (!test.formSupported) {
          stop(ins, test.reason); break;
        }
        ++test.modeledStates; test.flagsTags |= tags;
        test.reason = "modeled_read_only_TEST_AND_flags_possible_dependence";
        // TEST never writes its operands and never reads the old flags. A
        // register tested against itself is NOT a zero idiom (unlike CMP/XOR).
        w.flow.flags = tags;
        w.flow.logicalTestFlags = w.flow.pinnedTestFlags = true;
        if ((tags & 1) && !r.firstUse) r.firstUse = ins.address;
        if (!node.count) { stop(ins, "read_flow_fallthrough"); break; }
        w.pc = unsigned(node.next[0]); continue;
      }
      cs_regs reads{}, writes{}; std::uint8_t nr{}, nw{};
      if (cs_regs_access(d.h, &ins, reads, &nr, writes, &nw) != CS_ERR_OK) {
        stop(ins, "read_flow_register_access_unavailable"); break;
      }
      unsigned incoming = 0;
      bool readRegsOk = true;
      for (unsigned k = 0; k < nr; ++k) {
        readRegsOk &= reads[k] == X86_REG_RIP || reads[k] == X86_REG_EFLAGS ||
                      reg(reads[k]) >= 0 || xmm(reads[k]) >= 0;
        incoming |= w.flow.get(reads[k]);
        if (reads[k] == X86_REG_EFLAGS && w.flow.pinnedTestFlags) {
          // Non-Jcc flag users need their own model; don't guess AF/CF effects.
          if (!test.firstConsumer) {
            test.firstConsumer = ins.address;
            test.consumerKind = "unsupported_flag_use";
            copyName(test.consumerMnemonic, sizeof(test.consumerMnemonic), ins.mnemonic);
          }
          stop(ins, "test_flags_nonbranch_consumer_unmodeled"); readRegsOk = false;
        }
      }
      if (!readRegsOk) { stop(ins, "read_flow_partial_or_unmodeled_register"); break; }
      unsigned selectedOperands = 0;
      bool unknownRead = false;
      for (unsigned j = 0; j < x.op_count; ++j) {
        const auto &op = x.operands[j];
        if (ins.id == X86_INS_LEA || op.type != X86_OP_MEM ||
            (!(op.access & CS_AC_READ) && (op.access & CS_AC_WRITE))) continue;
        const auto at = aliasAddress(w.slice.s, ins, op, 0);
        const bool known = at.kind == pointer && x.addr_size == 8;
        const bool field = known && at.root == 1 &&
                           overlaps(at.off, op.size, 0x10c, 0x110);
        int overlap = -1;
        const char *cls = "unknown";
        bool exact = known;
        if (known && at.root == 1) { overlap = field ? 1 : 0; cls = "transform"; }
        else if (known && (at.root == 2 || at.root == 4) &&
                 at.off + (at.root == 2 ? 0x28 : 0) >= -4096 &&
                 at.off + (at.root == 2 ? 0x28 : 0) + op.size <= 4096) {
          overlap = 0; cls = at.root == 2 ? "source_conditional_TEB" : "stack_conditional_TEB";
        } else if (known && at.root == globalRoot && at.off > 0 &&
                   std::uint64_t(at.off) < a.imageSize &&
                   op.size <= a.imageSize - std::uint64_t(at.off)) {
          overlap = 0; cls = "image_conditional_regions";
        } else { exact = false; cls = known ? "other_unproven" : "unknown"; }
        if (field) r.fieldOrderMask |= w.wrote ? 2u : 1u;
        unsigned tags = w.flow.load(at, op.size);
        if (w.pc == readIndex) {
          ++selectedOperands;
          if (r.samples && (r.root != (known ? at.root : -1) ||
              r.offset != at.off || r.width != op.size || r.overlap != overlap)) {
            r.sameAddress = false; exact = false;
          }
          ++r.samples; r.decoded = true; r.root = known ? at.root : -1;
          r.offset = at.off; r.width = op.size; r.overlap = overlap;
          r.addressExact = r.sameAddress && exact; r.addressClass = cls;
          r.displacement = op.mem.disp;
          r.scale = op.mem.scale; r.operandIndex = j;
          copyName(r.mnemonic, sizeof(r.mnemonic), ins.mnemonic);
          copyName(r.base, sizeof(r.base), cs_reg_name(d.h, op.mem.base));
          copyName(r.index, sizeof(r.index), cs_reg_name(d.h, op.mem.index));
          if (ins.id == X86_INS_CMP || ins.id == X86_INS_TEST) {
            r.carrierKind = "flags"; copyName(r.carrier, sizeof(r.carrier), "rflags");
          } else if (x.op_count >= 2 && x.operands[0].type == X86_OP_REG &&
                     (reg(x.operands[0].reg) >= 0 || xmm(x.operands[0].reg) >= 0)) {
            r.carrierKind = "register";
            copyName(r.carrier, sizeof(r.carrier), cs_reg_name(d.h, x.operands[0].reg));
          } else r.carrierKind = "unsupported_read_effect";
          tags |= 1u | (field && w.wrote ? 2u : 0u);
        } else if (field) tags |= w.wrote ? 2u : 4u;
        incoming |= tags;
        unknownRead |= !exact;
      }
      if ((w.pc == readIndex && selectedOperands != 1) || unknownRead ||
          (w.pc == readIndex && !r.sameAddress)) {
        stop(ins, "read_address_unknown_indexed_conflicting_or_multiple"); break;
      }
      if (w.pc == readIndex && !r.decoded) {
        stop(ins, "selected_instruction_is_not_a_modeled_read"); break;
      }
      // Exact zero idioms kill incoming value dependence; no flag narrowing.
      if (x.op_count == 2 && x.operands[0].type == X86_OP_REG &&
          x.operands[1].type == X86_OP_REG &&
          x.operands[0].reg == x.operands[1].reg &&
          (ins.id == X86_INS_XOR || ins.id == X86_INS_PXOR ||
           ins.id == X86_INS_XORPS || ins.id == X86_INS_CMP)) incoming = 0;
      if ((incoming & 1) && w.pc != readIndex && !r.firstUse) r.firstUse = ins.address;
      const auto oldSpTags = w.flow.g[4];
      if (ins.id == X86_INS_POP) incoming |= w.flow.load(w.slice.s.g[4], 8);
      bool badWrite = false, fieldStore = false;
      for (unsigned j = 0; j < x.op_count; ++j) {
        const auto &op = x.operands[j];
        const bool possibleStore = (op.access & CS_AC_WRITE) ||
            (j == 0 && x.op_count > 1 && ins.id != X86_INS_CMP && ins.id != X86_INS_TEST);
        if (op.type != X86_OP_MEM || !possibleStore) continue;
        const auto at = aliasAddress(w.slice.s, ins, op, 0);
        fieldStore = w.pc == writeIndex && ins.id == X86_INS_MOV &&
                     op.size == 4 && at.kind == pointer && at.root == 1 && at.off == 0x10c;
        if (fieldStore && (incoming & 1)) {
          r.memoryEscape = true; if (!r.firstEscape) r.firstEscape = ins.address;
        }
        if (at.kind == pointer && at.root == 4) {
          if (!w.flow.save(at, op.size, incoming)) badWrite = true;
        } else if (!fieldStore) {
          if (incoming & 1) {
            r.memoryEscape = true; if (!r.firstEscape) r.firstEscape = ins.address;
          }
          badWrite = true;
        }
      }
      if (ins.id == X86_INS_PUSH && !w.flow.save(plus(w.slice.s.g[4], -8), 8, incoming)) badWrite = true;
      if (badWrite) { stop(ins, "read_flow_nonstack_memory_or_stack_cap_unresolved"); break; }
      bool regsOk = true;
      for (unsigned k = 0; k < nw; ++k) {
        regsOk &= w.flow.put(writes[k], incoming);
        if (writes[k] == X86_REG_EFLAGS)
          w.flow.logicalTestFlags = w.flow.pinnedTestFlags = false;
      }
      if (ins.id == X86_INS_PUSH || ins.id == X86_INS_POP) w.flow.g[4] = oldSpTags;
      if (!regsOk) { stop(ins, "read_flow_partial_or_unmodeled_register"); break; }
      if (fieldStore) w.wrote = true;
      else if (!globalLea(w.slice, ins, 0, a.imageSize) && !w.slice.step(ins, false)) {
        stop(ins, "read_flow_instruction_or_stack_effect_unresolved"); break;
      }
      if (!node.count) { stop(ins, "read_flow_fallthrough"); break; }
      w.pc = unsigned(node.next[0]);
    }
  }
  if (!r.decoded) r.flowComplete = false;
  if (r.flowComplete)
    r.reason = r.returnedGpr || r.returnedXmm || r.returnedXmmHigh || r.returnedFlags || r.returnedCallerStack
                 ? "surviving_carrier_writer_relevance_unresolved"
             : r.memoryEscape ? "read_value_persisted_cross_invocation_unresolved"
             : r.controlUse ? "read_control_flow_downstream_effect_unresolved"
             : "local_value_dead_at_returns_cross_invocation_unresolved";
}
bool aliasCallee(CalleeAlias &a, CallEffect &call, const Slice &writer) {
  a.resolved = a.sourceNoWrite = a.transformNoWrite = false;
  a.positionProtected = a.relevantEffectsResolved = false;
  auto fail = [&](std::uintptr_t at, const char *mnemonic, const char *effect,
                  const char *reason) {
    a.barrier = at;
    std::memset(a.mnemonic, 0, sizeof(a.mnemonic));
    for (unsigned i = 0; i + 1 < sizeof(a.mnemonic) && mnemonic[i]; ++i)
      a.mnemonic[i] = mnemonic[i];
    a.barrierEffect = effect;
    a.reason = reason;
    call.noWrite = false;
    call.barrier = at;
    call.reason = reason;
    std::memcpy(call.mnemonic, a.mnemonic, sizeof(call.mnemonic));
    return false;
  };
  if (!a.authenticated || !call.instructionVerified || !call.boundaryVerified ||
      call.begin != call.target || call.end - call.begin != a.size ||
      a.size > aliasByteCap || ++a.analyses > 16)
    return fail(call.target, "identity", "identity_or_analysis_cap",
                "exact_callee_identity_or_call_analysis_cap_refused");
  auto sp = writer.s.g[4];
  if (sp.kind != pointer || sp.root != 4 || sp.off > 0 || sp.off < -3960)
    return fail(call.call, "call", "caller_stack",
                "writer_precall_stack_provenance_unresolved");
  const std::int64_t entrySp = sp.off - 8;
  // The natural CALL push itself must not destroy a tracked caller carrier.
  for (const auto &slot : writer.s.stack)
    if (slot.used && slot.off < sp.off && slot.off + slot.width > entrySp)
      return fail(call.call, "call", "return_area",
                  "natural_call_return_overlaps_writer_saved_slot");
  const unsigned argRegs[] = {1, 2, 8, 9};
  for (unsigned i = 0; i < 4; ++i) {
    auto v = writer.s.g[argRegs[i]];
    const char *origin =
        v.kind == pointer && v.root == 1   ? "entry_writer_RCX_transform"
        : v.kind == pointer && v.root == 2 ? "entry_writer_RDX_source"
        : v.kind == pointer && v.root == 4 ? "writer_entry_RSP_relative"
        : v.kind == pointer && v.root == globalRoot ? "exact_image_RIP_relative"
        : v.kind == constant ? "constant_bits_NOT_pointer"
                             : "unknown_argument";
    if (a.analyses == 1) {
      a.argumentOrigins[i] = origin;
      a.argumentOffsets[i] = v.off;
    } else if (std::strcmp(a.argumentOrigins[i], origin) ||
               a.argumentOffsets[i] != v.off) {
      a.argumentOrigins[i] = "conflicting_writer_path_origins";
      a.argumentsComplete = false;
      return fail(call.call, "call", "argument_provenance_conflict",
                  "writer_paths_disagree_on_exact_call_argument_origin");
    }
    if (v.kind == pointer &&
        (v.root == 2 || (v.root == 4 && v.off < 0x38 && v.off + 16 > 0x28)))
      a.sourceArgMask |= 1u << i;
    if (v.kind == pointer && v.root == 1)
      a.transformArgMask |= 1u << i;
  }
  a.argumentsComplete = true;
  Decode d;
  if (!d.run(a.code, a.size, call.begin))
    return fail(call.begin, "decode_failure", "decode",
                "callee_alias_complete_decode_failed");
  struct Node {
    std::array<int, 2> next{{-1, -1}};
    unsigned count{};
    bool branch{}, cond{}, ret{};
  };
  std::array<Node, aliasByteCap> nodes{};
  std::array<int, aliasByteCap> boundaries{};
  boundaries.fill(-1);
  for (unsigned i = 0; i < d.n; ++i)
    boundaries[d.p[i].address - call.begin] = int(i);
  if ((a.edi.enabled && !a.exactWrite.site) ||
      (a.exactWrite.site &&
       (a.exactWrite.site < call.begin || a.exactWrite.site >= call.end ||
        boundaries[a.exactWrite.site - call.begin] < 0)))
    return fail(a.exactWrite.site, "instruction_boundary",
                "exact_write_identity",
                "exact_write_site_outside_callee_or_not_instruction_aligned");
  // Count the *entire* authenticated decode first. Never discover the budget
  // by accepting a prefix or by taking only one observed branch successor.
  a.localBranches = a.conditionalBranches = 0;
  a.blocks = a.edges = a.blocksVisited = a.edgesVisited = 0;
  a.backedges = a.indirectTransfers = a.nestedCalls = 0;
  a.cfgComplete = false;
  for (unsigned i = 0; i < d.n; ++i) {
    if (group(d.p[i], CS_GRP_JUMP)) {
      ++a.localBranches;
      if (conditional(d.p[i].id))
        ++a.conditionalBranches;
    }
    if (group(d.p[i], CS_GRP_CALL))
      ++a.nestedCalls;
  }
  a.branchBudget = a.localBranches; // exact count, zero speculative margin
  a.pathBudget = a.conditionalBranches <= aliasBranchCeiling
                     ? 1u << a.conditionalBranches
                     : 0;
  a.stateBudget = a.pathBudget ? 2 * a.pathBudget - 1 : 0;
  std::array<bool, aliasByteCap> leader{}, visitedBlocks{};
  std::array<std::array<bool, 2>, aliasByteCap> visitedEdges{};
  std::array<unsigned, aliasByteCap> block{};
  leader[0] = true;
  for (unsigned i = 0; i < d.n; ++i) {
    auto &ins = d.p[i];
    auto &g = nodes[i];
    const auto &x = ins.detail->x86;
    if (group(ins, CS_GRP_JUMP)) {
      g.branch = true;
      g.cond = conditional(ins.id);
      if (x.op_count != 1 || x.operands[0].type != X86_OP_IMM) {
        ++a.indirectTransfers;
        return fail(ins.address, ins.mnemonic, "indirect_transfer",
                    "callee_indirect_transfer_no_expansion");
      }
      if (ins.id != X86_INS_JMP && !g.cond)
        return fail(ins.address, ins.mnemonic, "unsupported_branch_form",
                    "callee_only_regular_flag_Jcc_and_direct_JMP_supported");
      auto dest = x.operands[0].imm;
      if (dest <= std::int64_t(ins.address)) {
        ++a.backedges;
        return fail(ins.address, ins.mnemonic, "backedge",
                    "callee_backedge_no_termination_guess");
      }
      if (dest < std::int64_t(call.begin) || dest >= std::int64_t(call.end) ||
          boundaries[dest - call.begin] < 0)
        return fail(ins.address, ins.mnemonic, "nonlocal_or_boundary",
                    "callee_nonlocal_or_mid_instruction_branch_refused");
      g.next[g.count++] = boundaries[dest - call.begin];
      leader[g.next[0]] = true;
      if (g.cond) {
        if (i + 1 == d.n)
          return fail(ins.address, ins.mnemonic, "fallthrough",
                      "callee_conditional_leaves_boundary");
        g.next[g.count++] = int(i + 1);
      }
      if (i + 1 < d.n)
        leader[i + 1] = true;
      // Count is known before imposing the separate hard resource ceiling.
      if (a.branchBudget > aliasBranchCeiling)
        return fail(ins.address, ins.mnemonic, "branch_resource_ceiling",
                    "exact_decoded_branch_count_exceeds_owned_path_storage");
    } else if (ins.id == X86_INS_RET && x.op_count == 0) {
      g.ret = true;
      if (i + 1 < d.n)
        leader[i + 1] = true;
    } else if (i + 1 < d.n)
      g.next[g.count++] = int(i + 1);
  }
  for (unsigned i = 0; i < d.n; ++i) {
    if (leader[i])
      ++a.blocks;
    block[i] = a.blocks - 1;
    for (unsigned j = 0; j < nodes[i].count; ++j)
      if (nodes[i].branch || leader[nodes[i].next[j]])
        ++a.edges;
  }
  a.cfgComplete = true;
  struct Work {
    Slice slice{};
    unsigned pc{};
  };
  // Depth-first enumeration needs at most one pending state per Jcc depth.
  // It avoids a large heap/stack array of 256 full provenance states.
  std::array<Work, aliasBranchCeiling + 1> todo{};
  unsigned pending = 1, states = 1;
  todo[0].slice = writer;
  todo[0].slice.s.g[4] = plus(sp, -8);
  const auto initial = todo[0].slice.s;
  if (a.edi.enabled &&
      !ediDefinitions(a, d, nodes, todo[0].slice,
                      unsigned(boundaries[a.exactWrite.site - call.begin])))
    return fail(a.edi.barrier, a.edi.mnemonic,
                "EDI_value_provenance_unresolved", a.edi.reason);
  inspectReadDependency(a, d, nodes, todo[0].slice,
                        unsigned(boundaries[a.exactWrite.site ? a.exactWrite.site - call.begin : 0]));

  a.stackLow = std::min(a.stackLow, entrySp);
  unsigned gpr = 0xf07, xmmMask = 0x3f;
  auto recordWrite = [&](const cs_insn &ins, Value addr, unsigned width,
                         const State &state) {
    AliasKind kind = unknownPointer;
    Value normalized = addr;
    if (normalized.kind == pointer && normalized.root == 2) {
      normalized.root = 4;
      normalized.off += 0x28;
    }
    auto overlap = [](std::int64_t at, unsigned n, std::int64_t lo,
                      std::int64_t hi) {
      return at < hi && at + std::int64_t(n) > lo;
    };
    bool safe = false;
    if (normalized.kind == pointer && normalized.root == 4) {
      if (overlap(normalized.off, width, 0x28, 0x38))
        kind = sourceExact;
      else if (overlap(normalized.off, width, 0, 8))
        kind = callerArea; // writer entry return slot is also protected
      else if (overlap(normalized.off, width, entrySp, entrySp + 8))
        kind = callerArea;
      else if (normalized.off >= entrySp + 8 &&
               normalized.off + width <= entrySp + 40) {
        kind = callerArea;
        safe = true;
      } else if (state.g[4].kind == pointer && state.g[4].root == 4 &&
                 normalized.off >= state.g[4].off &&
                 normalized.off + width <= entrySp) {
        kind = privateStack;
        safe = true;
      } else if (addr.kind == pointer && addr.root == 2 &&
                 normalized.off >= -4096 && normalized.off + width <= 4096) {
        kind = otherDisjoint;
        safe = true;
      }
      if (safe && (normalized.off < -4096 || normalized.off + width > 4096))
        safe = false;
    } else if (addr.kind == pointer && addr.root == 1)
      kind =
          overlap(addr.off, width, 0, 64) ? transformExact : transformPossible;
    else if (addr.kind == pointer && addr.root == globalRoot && addr.off > 0 &&
             std::uint64_t(addr.off) < a.imageSize &&
             width <= a.imageSize - std::uint64_t(addr.off)) {
      kind = imageGlobal;
      safe = true;
      a.usesGlobals = true;
    }
    const char *exactFailureEffect = nullptr, *exactFailureReason = nullptr;
    if (a.exactWrite.site == ins.address) {
      auto &t = a.exactWrite;
      if (addr.kind == pointer && addr.root == 1)
        a.transformWriteSeen = true;
      if (!t.destinationSame) {
        exactFailureEffect = "conflicting_address_provenance";
        exactFailureReason =
            "exact_write_reaching_destination_roots_or_offsets_conflict";
        kind = unknownPointer;
        normalized.root = -1;
        normalized.off = 0;
        safe = false;
      } else if (ins.id == X86_INS_MOV && width == 4 && t.destinationExact &&
                 addr.off == 0x10c && !t.carrierOverlap) {
        // Exact address classification is retained even if the value/effect
        // cannot be certified. A refused write is still in the write-site map.
        kind = transformExact;
        if (!t.valueSame || !t.valueExact) {
          exactFailureEffect = "value_provenance_unresolved";
          exactFailureReason =
              "exact_write_value_unknown_computed_or_reaching_origins_conflict";
        } else {
          for (unsigned k = 0; k < a.reads; ++k)
            if (readMayDepend(a.readSites[k])) {
              t.reloadDependency = true;
              t.dependencySite = a.readSites[k].at;
              exactFailureEffect = "earlier_read_provenance_dependency";
              exactFailureReason =
                  "exact_transform_write_may_change_a_decoded_read_carrier";
              break;
            }
          if (!exactFailureEffect) {
            // Only this site/offset/width is modeled. A real Transform write
            // never gains a literal Transform-no-write certificate.
            safe = true;
            t.modeled = true;
          }
        }
      }
    }
    a.barrierRoot = addr.kind == pointer ? addr.root : -1;
    a.barrierOffset = addr.off;
    a.barrierWidth = width;
    bool found = false;
    for (unsigned j = 0; j < a.writes; ++j)
      if (a.writeSites[j].at == ins.address) {
        auto &old = a.writeSites[j];
        found = true;
        if (old.root != normalized.root || old.offset != normalized.off ||
            old.width != width || old.kind != kind) {
          --a.counts[old.kind];
          old.kind = unknownPointer;
          old.root = -1;
          old.offset = 0;
          a.barrierRoot = -1;
          a.barrierOffset = 0;
          ++a.counts[unknownPointer];
          return fail(ins.address, ins.mnemonic,
                      "conflicting_address_provenance",
                      "callee_write_address_differs_across_reaching_states");
        }
        break;
      }
    if (!found) {
      if (a.writes == aliasWriteCap)
        return fail(ins.address, ins.mnemonic, "write_cap",
                    "callee_unique_write_cap_exceeded");
      a.writeSites[a.writes++] = {ins.address, width, normalized.root,
                                  normalized.off, kind};
      ++a.counts[kind];
    }
    if (exactFailureEffect)
      return fail(ins.address, ins.mnemonic, exactFailureEffect,
                  exactFailureReason);
    if (!safe)
      return fail(
          ins.address, ins.mnemonic, aliasKindName(kind),
          "callee_write_not_proven_disjoint_from_both_protected_objects");
    if (normalized.root == 4) {
      a.stackLow = std::min(a.stackLow, normalized.off);
      a.stackHigh = std::max(a.stackHigh, normalized.off + width);
      for (const auto &slot : writer.s.stack)
        if (slot.used &&
            overlap(normalized.off, width, slot.off, slot.off + slot.width))
          return fail(ins.address, ins.mnemonic, "writer_saved_slot_alias",
                      "callee_write_overlaps_precall_writer_saved_carrier");
    }
    return true;
  };
  while (pending) {
    auto work = todo[--pending];
    for (;;) {
      auto &ins = d.p[work.pc];
      auto &g = nodes[work.pc];
      const auto &x = ins.detail->x86;
      if (++a.visits > 4096)
        return fail(ins.address, ins.mnemonic, "visit_cap",
                    "callee_total_instruction_visit_cap_exceeded");
      if (!visitedBlocks[block[work.pc]]) {
        visitedBlocks[block[work.pc]] = true;
        ++a.blocksVisited;
      }
      auto edge = [&](unsigned j) {
        if ((g.branch || leader[g.next[j]]) && !visitedEdges[work.pc][j]) {
          visitedEdges[work.pc][j] = true;
          ++a.edgesVisited;
        }
      };
      if (g.branch) {
        for (unsigned j = 0; j < g.count; ++j)
          edge(j);
        if (g.cond) {
          if (pending == todo.size() || states + 2 > a.stateBudget)
            return fail(ins.address, ins.mnemonic, "state_cap",
                        "callee_local_state_cap_exceeded");
          todo[pending] = work;
          todo[pending++].pc = unsigned(g.next[0]);
          states += 2;
          work.pc = unsigned(g.next[1]);
        } else
          work.pc = unsigned(g.next[0]);
        continue;
      }
      if (g.ret) {
        if (!equal(work.slice.s.g[4], initial.g[4]))
          return fail(ins.address, ins.mnemonic, "stack_return",
                      "callee_stack_not_restored_at_return");
        for (unsigned j = 0; j < 16; ++j) {
          if (!equal(work.slice.s.g[j], initial.g[j]))
            gpr |= 1u << j;
          for (unsigned lane = 0; lane < 4; ++lane)
            if (!equal(work.slice.s.x[j * 4 + lane], initial.x[j * 4 + lane]))
              xmmMask |= 1u << j;
        }
        if (++a.paths > 16 * aliasPathCeiling)
          return fail(ins.address, ins.mnemonic, "path_cap",
                      "callee_total_return_path_cap_exceeded");
        break;
      }
      if (group(ins, CS_GRP_CALL) || group(ins, CS_GRP_RET) ||
          group(ins, CS_GRP_IRET) || group(ins, CS_GRP_INT))
        return fail(ins.address, ins.mnemonic, "nested_or_unsupported_control",
                    "callee_nested_call_or_unsupported_transfer_no_expansion");
      describeWrite(a, work.slice.s, ins, d.h);
      if (a.edi.enabled && ins.address == a.exactWrite.site) {
        auto &t = a.exactWrite;
        t.valueSame = a.edi.complete && a.edi.same;
        t.valueExact = t.valueSame && a.edi.exact;
        t.valueKind = !std::strcmp(a.edi.origin.producerKind, "computed")
                          ? "computed"
                      : a.edi.origin.kind == BitKind::entryRegister ? "register"
                      : a.edi.origin.kind == BitKind::memoryLoad
                          ? "memory"
                          : bitKindName(a.edi.origin.kind);
        t.valueTag = 100 + int(a.edi.origin.kind);
        t.valueRoot = a.edi.origin.root;
        t.valueOffset = a.edi.origin.offset;
        t.valueBits = a.edi.origin.bits;
      }

      if (!recordRead(a, work.slice.s, ins, 0))
        return fail(ins.address, ins.mnemonic,
                    "read_provenance_dependency_or_cap",
                    "exact_transform_write_reload_alias_unknown_or_read_cap");
      bool move = ins.id == X86_INS_MOV || ins.id == X86_INS_MOVSS ||
                  ins.id == X86_INS_VMOVSS || ins.id == X86_INS_MOVSD ||
                  ins.id == X86_INS_VMOVSD || ins.id == X86_INS_MOVAPS ||
                  ins.id == X86_INS_MOVUPS || ins.id == X86_INS_MOVDQA ||
                  ins.id == X86_INS_MOVDQU || ins.id == X86_INS_VMOVAPS ||
                  ins.id == X86_INS_VMOVUPS || ins.id == X86_INS_VMOVDQA ||
                  ins.id == X86_INS_VMOVDQU;
      bool memoryWrite = false, globalWrite = false;
      if (ins.id == X86_INS_PUSH && x.op_count == 1 &&
          x.operands[0].type == X86_OP_REG && x.operands[0].size == 8) {
        auto next = plus(work.slice.s.g[4], -8);
        auto allocated = work.slice.s;
        allocated.g[4] = next;
        if (!recordWrite(ins, next, 8, allocated))
          return false;
      }
      for (unsigned j = 0; j < x.op_count; ++j)
        if (x.operands[j].type == X86_OP_MEM &&
            ((x.operands[j].access & CS_AC_WRITE) || (j == 0 && move))) {
          memoryWrite = true;
          auto address = aliasAddress(work.slice.s, ins, x.operands[j], 0);
          if (!recordWrite(ins, address, x.operands[j].size, work.slice.s))
            return false;
          if (!move || j != 0 || x.op_count != 2)
            return fail(ins.address, ins.mnemonic, "unsupported_write_effect",
                        "callee_write_instruction_semantics_not_supported");
          globalWrite =
              address.root == globalRoot ||
              (a.exactWrite.modeled && ins.address == a.exactWrite.site);
        }
      bool modeled = false;
      if (globalWrite)
        modeled = true; // Explicit store only; no registers change.
      else if (globalLea(work.slice, ins, 0, a.imageSize))
        modeled = true;
      else
        modeled = work.slice.step(ins, false);
      if (!modeled)
        return fail(ins.address, ins.mnemonic,
                    memoryWrite ? "unsupported_write_effect"
                                : "unsupported_instruction_effect",
                    "callee_exact_relevant_instruction_effect_unresolved");
      auto currentSp = work.slice.s.g[4];
      if (currentSp.kind != pointer || currentSp.root != 4 ||
          currentSp.off < -4096 || currentSp.off > entrySp)
        return fail(ins.address, ins.mnemonic, "stack_bounds",
                    "callee_stack_outside_bounded_frame");
      if (!g.count)
        return fail(ins.address, ins.mnemonic, "fallthrough",
                    "callee_path_has_no_authenticated_return");
      edge(0);
      work.pc = unsigned(g.next[0]);
    }
  }
  call.gprClobber |= gpr;
  call.xmmClobber |= xmmMask;
  call.privateStackLow = std::min(call.privateStackLow, a.stackLow - entrySp);
  call.noWrite = true;
  call.effect = "source_span_no_write_proven";
  call.barrier = 0;
  std::memset(call.mnemonic, 0, sizeof(call.mnemonic));
  std::memcpy(call.mnemonic, "none", 4);
  call.reason = "all_bounded_callee_paths_source_and_transform_disjoint_"
                "conditional_on_runtime_regions";
  a.resolved = a.sourceNoWrite = true;
  a.transformNoWrite = !a.transformWriteSeen;
  a.positionProtected = a.relevantEffectsResolved = true;
  if (a.transformWriteSeen)
    call.effect = "source_no_write_and_Position_provenance_effects_resolved";
  a.barrier = 0;
  a.barrierEffect = "none";
  std::memset(a.mnemonic, 0, sizeof(a.mnemonic));
  std::memcpy(a.mnemonic, "none", 4);
  if (a.transformWriteSeen)
    call.reason = "all_bounded_callee_paths_source_no_write_Position_and_read_"
                  "provenance_resolved_conditional_on_runtime_regions";
  a.reason = call.reason;
  return true;
}
} // namespace
Writer branchWriter(const unsigned char *b, std::size_t n,
                    const std::array<unsigned, 4> &sites, CallEffect *call,
                    CalleeAlias *alias) noexcept {
  Writer r{};
  r.branchAware = true;
  Decode d;
  if (n > writerByteCap || !d.run(b, n)) {
    r.reason = "complete_decode_failed_or_exact_writer_span_over_183";
    return r;
  }
  r.decoded = true;
  auto fail = [&](unsigned at, const char *mnemonic, const char *reason,
                  unsigned edge = 0, bool hasEdge = false) {
    r.barrier = at;
    r.barrierValid = true;
    r.barrierEdge = edge;
    r.barrierHasEdge = hasEdge;
    std::memset(r.barrierMnemonic, 0, sizeof(r.barrierMnemonic));
    for (unsigned i = 0; i + 1 < sizeof(r.barrierMnemonic) && mnemonic[i]; ++i)
      r.barrierMnemonic[i] = mnemonic[i];
    r.xyz = false;
    r.w = false;
    r.allRelevant = false;
    r.reason = reason;
  };
  struct Node {
    std::array<int, 2> next{{-1, -1}};
    unsigned count{}, relevant{}, block{};
    bool jump{}, cond{}, ret{};
  };
  std::array<Node, writerByteCap> graph{};
  std::array<int, writerByteCap + 1> boundary{};
  boundary.fill(-1);
  std::array<bool, writerByteCap> leader{};
  leader[0] = true;
  for (unsigned i = 0; i < d.n; ++i)
    boundary[unsigned(d.p[i].address)] = int(i);
  // Decode the entire local graph first; every jump stays forward, local and
  // instruction-aligned. Unknown indirect transfers cannot hide a backedge.
  for (unsigned i = 0; i < d.n; ++i) {
    auto &ins = d.p[i];
    auto &x = ins.detail->x86;
    auto &g = graph[i];
    auto at = unsigned(ins.address);
    if (group(ins, CS_GRP_JUMP)) {
      g.jump = true;
      g.cond = conditional(ins.id);
      if (!r.branches)
        r.firstBranch = at;
      if (++r.branches > branchCap) {
        fail(at, ins.mnemonic, "local_branch_cap_exceeded");
        return r;
      }
      if ((ins.id != X86_INS_JMP && !g.cond) || x.op_count != 1 ||
          x.operands[0].type != X86_OP_IMM) {
        fail(at, ins.mnemonic, "unsupported_or_indirect_local_control_flow");
        return r;
      }
      auto dest = x.operands[0].imm;
      if (dest < 0 || std::uint64_t(dest) >= n) {
        fail(at, ins.mnemonic, "branch_leaves_exact_writer", unsigned(dest),
             true);
        return r;
      }
      if (dest <= std::int64_t(at)) {
        fail(at, ins.mnemonic, "loop_or_backedge_refused", unsigned(dest),
             true);
        return r;
      }
      if (boundary[unsigned(dest)] < 0) {
        fail(at, ins.mnemonic, "branch_target_not_instruction_boundary",
             unsigned(dest), true);
        return r;
      }
      auto target = unsigned(boundary[unsigned(dest)]);
      g.next[g.count++] = int(target);
      leader[target] = true;
      if (g.cond) {
        if (i + 1 == d.n) {
          fail(at, ins.mnemonic, "conditional_fallthrough_leaves_writer");
          return r;
        }
        g.next[g.count++] = int(i + 1);
      }
      if (i + 1 < d.n)
        leader[i + 1] = true;
    } else if (ins.id == X86_INS_RET && x.op_count == 0) {
      g.ret = true;
      if (i + 1 < d.n)
        leader[i + 1] = true;
    } else if (group(ins, CS_GRP_RET) || group(ins, CS_GRP_IRET)) {
      fail(at, ins.mnemonic, "unsupported_local_return");
      return r;
    } else if (i + 1 < d.n) {
      g.next[g.count++] = int(i + 1);
    }
  }
  for (unsigned i = 0; i < d.n; ++i) {
    if (leader[i])
      ++r.blocks;
    graph[i].block = r.blocks - 1;
  }
  for (unsigned i = 0; i < d.n; ++i)
    if (i + 1 == d.n || leader[i + 1] || graph[i].jump || graph[i].ret)
      r.edges += graph[i].count;
  if (r.blocks > blockCap || r.edges > edgeCap) {
    fail(0, "graph_caps", "local_block_or_edge_cap_exceeded");
    return r;
  }
  for (unsigned c = 0; c < 4; ++c) {
    if (sites[c] >= n || boundary[sites[c]] < 0) {
      fail(sites[c], "site_boundary", "exact_store_not_instruction_boundary");
      return r;
    }
    auto &ins = d.p[unsigned(boundary[sites[c]])];
    auto &x = ins.detail->x86;
    if ((ins.id != X86_INS_MOV && ins.id != X86_INS_MOVSS &&
         ins.id != X86_INS_VMOVSS) ||
        x.op_count != 2 || x.operands[0].type != X86_OP_MEM ||
        x.operands[0].size != 4) {
      fail(sites[c], ins.mnemonic, "exact_site_is_not_supported_dword_store");
      return r;
    }
    for (unsigned other = 0; other < c; ++other)
      if (sites[other] == sites[c]) {
        fail(sites[c], ins.mnemonic, "duplicate_expected_store_site");
        return r;
      }
    r.stores[c] = sites[c];
    graph[unsigned(boundary[sites[c]])].relevant |= 1u << c;
  }
  r.exactSites = true;
  // Forward edges make one reverse pass sufficient. Suffix effects that
  // cannot reach a named store are outside this certificate, not certified.
  for (unsigned i = unsigned(d.n); i > 0; --i) {
    auto &g = graph[i - 1];
    for (unsigned e = 0; e < g.count; ++e)
      g.relevant |= graph[unsigned(g.next[e])].relevant;
  }
  struct Work {
    Slice slice{};
    unsigned pc{};
  };
  std::array<Work, pathCap> todo{};
  unsigned pending = 1;
  r.statesScheduled = 1;
  std::array<Value, 4> origins{};
  std::array<bool, 4> consistent{{true, true, true, true}};
  std::array<bool, blockCap> visitedBlocks{};
  std::array<std::array<bool, 2>, writerByteCap> visitedEdges{};
  while (pending) {
    auto work = todo[--pending];
    for (;;) {
      if (++r.visits > visitCap) {
        fail(unsigned(d.p[work.pc].address), "visit_cap",
             "local_instruction_visit_cap_exceeded");
        return r;
      }
      auto index = work.pc;
      auto &g = graph[index];
      auto &ins = d.p[index];
      auto at = unsigned(ins.address);
      if (!visitedBlocks[g.block]) {
        visitedBlocks[g.block] = true;
        ++r.blocksVisited;
      }
      if (!g.relevant) {
        if (++r.pathsExamined > pathCap) {
          fail(at, ins.mnemonic, "local_path_cap_exceeded");
          return r;
        }
        break;
      }
      auto edge = [&](unsigned e) {
        if (g.jump || index + 1 == d.n || leader[index + 1])
          if (!visitedEdges[index][e]) {
            visitedEdges[index][e] = true;
            ++r.edgesVisited;
          }
      };
      if (g.jump) {
        if (g.cond) {
          if (pending == todo.size() || r.statesScheduled + 2 > stateCap) {
            fail(at, ins.mnemonic, "local_state_cap_exceeded");
            return r;
          }
          todo[pending] = work;
          todo[pending++].pc = unsigned(g.next[0]);
          r.statesScheduled += 2;
          edge(0);
          edge(1);
          work.pc = unsigned(g.next[1]);
          continue;
        }
        edge(0);
        work.pc = unsigned(g.next[0]);
        continue;
      }
      if (alias &&
          !recordRead(*alias, work.slice.s, ins, call ? call->writer : 0)) {
        alias->resolved = alias->relevantEffectsResolved = false;
        alias->barrier = (call ? call->writer : 0) + at;
        copyName(alias->mnemonic, sizeof(alias->mnemonic), ins.mnemonic);
        alias->barrierEffect =
            "writer_reload_provenance_dependency_or_read_cap";
        alias->reason =
            "modeled_transform_write_may_affect_writer_reload_provenance";
        fail(at, ins.mnemonic, alias->reason);
        return r;
      }
      auto old = work.slice.mask;
      bool callApplied = false;
      if (group(ins, CS_GRP_CALL) && call && call->call == call->writer + at) {
        call->relevant = true;
        const auto &x = ins.detail->x86;
        bool anchor =
            call->instructionVerified && call->boundaryVerified &&
            call->decoded && x.op_count == 1 &&
            x.operands[0].type == X86_OP_IMM &&
            call->writer + ins.address + ins.size == call->ret &&
            call->writer + std::uintptr_t(x.operands[0].imm) == call->target;
        if (alias && !alias->authenticated) {
          fail(at, ins.mnemonic, alias->reason);
          return r;
        }
        if (alias && !anchor) {
          alias->resolved = alias->sourceNoWrite = alias->transformNoWrite =
              false;
          alias->reason = "exact_writer_callsite_return_or_target_mismatch";
          fail(at, ins.mnemonic, alias->reason);
          return r;
        }
        if (alias && !aliasCallee(*alias, *call, work.slice)) {
          fail(at, ins.mnemonic, alias->reason);
          return r;
        }
        bool exact =
            anchor && call->noWrite &&
            (!alias || (alias->sourceNoWrite && alias->positionProtected &&
                        alias->relevantEffectsResolved));
        auto sp = work.slice.s.g[4];
        // Natural CALL pushes its return in the writer's stack. The existing
        // per-invocation TEB proof binds source to entry RSP+0x28; neither that
        // span nor any modeled saved carrier may overlap return/shadow space.
        bool stackSafe = sp.kind == pointer && sp.root == 4 && sp.off <= 0 &&
                         sp.off >= -4096;
        if (stackSafe && !alias)
          for (auto &slot : work.slice.s.stack)
            if (slot.used && slot.off < sp.off + 32 &&
                slot.off + slot.width > sp.off - 8 + call->privateStackLow)
              stackSafe = false;
        if (!exact || !stackSafe) {
          fail(at, ins.mnemonic,
               !stackSafe ? "call_stack_return_or_shadow_alias_unresolved"
                          : call->reason);
          return r;
        }
        r.callStackLow =
            std::min(r.callStackLow, sp.off - 8 + call->privateStackLow);
        if (alias) {
          r.callStackLow = std::min(r.callStackLow, alias->stackLow);
          r.callStackHigh = std::max(r.callStackHigh, alias->stackHigh);
        }
        for (unsigned reg = 0; reg < 16; ++reg) {
          if (call->gprClobber & (1u << reg))
            work.slice.s.g[reg] = {};
          if (call->xmmClobber & (1u << reg))
            for (unsigned lane = 0; lane < 4; ++lane)
              work.slice.s.x[reg * 4 + lane] = {};
        }
        callApplied = true;
      }
      if (!callApplied &&
          !(alias && globalLea(work.slice, ins, call ? call->writer : 0,
                               alias->imageSize)) &&
          !work.slice.step(ins, true)) {
        fail(at, ins.mnemonic,
             "unsupported_relevant_writer_instruction_or_memory_effect");
        return r;
      }
      auto added = work.slice.mask & ~old;
      for (unsigned c = 0; c < 4; ++c)
        if (added & (1u << c)) {
          if (at != sites[c]) {
            fail(at, ins.mnemonic,
                 "store_RVA_or_destination_component_mismatch");
            return r;
          }
          auto value = work.slice.out[c];
          if (!r.reachingStates[c])
            origins[c] = value;
          else
            consistent[c] &= equal(origins[c], value);
          ++r.reachingStates[c];
          r.mask |= 1u << c;
          r.prefixEnd = std::max(r.prefixEnd, unsigned(ins.address + ins.size));
        }
      // A named site must actually write its required target on every reaching
      // path. Merely visiting a same-RVA stack/foreign-target write is not
      // proof.
      for (unsigned c = 0; c < 4; ++c)
        if (at == sites[c] && !(added & (1u << c))) {
          fail(at, ins.mnemonic, "exact_store_target_not_entry_RCX_component");
          return r;
        }
      if (!g.count) {
        fail(at, ins.mnemonic, "relevant_path_terminated_without_store");
        return r;
      }
      edge(0);
      work.pc = unsigned(g.next[0]);
    }
  }
  if (alias && alias->resolved) {
    alias->exactWrite.complete = true;
    alias->edi.writerStatesComplete = true;
  }
  r.bounded = true;
  r.prefix = r.mask == 15;
  for (unsigned c = 0; c < 3; ++c) {
    bool good = r.reachingStates[c] && consistent[c] &&
                equal(origins[c], {memory, 2, std::int64_t(c * 4)});
    if (good) {
      r.componentProofMask |= 1u << c;
      r.paths[c] = "all_reaching_states_entry_RDX_bitcopy";
    } else
      r.paths[c] = !r.reachingStates[c] ? "no_entry_path_reaches_store"
                   : !consistent[c]     ? "conflicting_reaching_provenance"
                                        : "not_exact_entry_RDX_origin";
  }
  r.xyz = r.componentProofMask == 7;
  r.allRelevant = r.xyz;
  classifyW(r, origins[3], r.reachingStates[3] && consistent[3]);
  if (!r.xyz)
    for (unsigned c = 0; c < 3; ++c)
      if (!(r.componentProofMask & (1u << c))) {
        r.barrierValid = true;
        r.barrier = sites[c];
        std::memcpy(r.barrierMnemonic,
                    d.p[unsigned(boundary[sites[c]])].mnemonic,
                    sizeof(r.barrierMnemonic) - 1);
        break;
      }
  r.reason =
      r.xyz ? "all_relevant_local_paths_prove_exact_RDX_XYZ_NOT_whole_function_"
              "or_write_permission"
            : "exact_store_reachable_provenance_conflict_or_missing_RDX_origin";
  return r;
}
Caller caller(const unsigned char *b, std::size_t n,
              unsigned callOffset) noexcept {
  Caller r{};
  Decode d;
  r.decoded = d.run(b, n);
  if (!r.decoded) {
    r.reason = "caller_complete_decode_failed_or_span_over_8192";
    return r;
  }
  Slice p;
  for (std::size_t k = 0; k < d.n; ++k) {
    auto &i = d.p[k];
    if (i.address == callOffset && i.id == X86_INS_CALL) {
      auto sp = p.s.g[4], dx = p.s.g[2];
      r.prefix = sp.kind == pointer && sp.root == 4;
      r.rspOffset = sp.off;
      r.rdxOffset = dx.off;
      r.rdxLocal = r.prefix && dx.kind == pointer && dx.root == 4 &&
                   dx.off == sp.off + 0x20;
      r.owned =
          r.rdxLocal && sp.off < 0 && dx.off >= sp.off + 32 && dx.off <= -16;
      for (unsigned j = 0; j < 3; ++j)
        for (auto &s : p.s.stack)
          if (s.used && s.off == sp.off + 32 + 4 * j && s.width == 4 &&
              s.v.kind != unknown)
            r.producers[j] = s.producer;
      r.constructed =
          r.prefix && std::all_of(r.producers.begin(), r.producers.end(),
                                  [](unsigned v) { return v != 0; });
      r.reason =
          r.rdxLocal
              ? (r.owned ? "bounded_local_RDX_assignment_before_exact_call"
                         : "RDX_points_outside_proven_caller_allocation")
              : (r.constructed
                     ? "XYZ_stack_construction_found_RDX_assignment_unproven"
                     : "RDX_inherited_or_not_exact_local_next_edge_is_incoming_"
                       "producer");
      return r;
    }
    if (!p.step(i, false)) {
      r.barrier = unsigned(i.address);
      std::memcpy(r.barrierMnemonic, i.mnemonic, sizeof(r.barrierMnemonic) - 1);
      r.reason = "caller_prefix_barrier_before_exact_call";
      return r;
    }
  }
  r.reason = "call_not_instruction_boundary";
  return r;
}
Storage storage(std::uintptr_t rsp, std::uintptr_t dx, std::uintptr_t lo,
                std::uintptr_t hi, bool reads, bool same,
                const Caller &c) noexcept {
  Storage r{};
  r.bounded = rsp >= lo && rsp <= hi && hi - rsp >= 0x38 && dx == rsp + 0x28;
  r.sameBytes = r.bounded && reads && same;
  r.callerOwned = r.sameBytes && c.prefix && c.rdxLocal && c.owned;
  return r;
}
Relation relation(const unsigned char *b, std::size_t size,
                  std::uintptr_t begin, std::uintptr_t ret,
                  std::uintptr_t consumer, Read read) noexcept {
  Relation r{};
  Decode d;
  r.decoded = d.run(b, size, begin);
  if (!r.decoded)
    return r;
  for (std::size_t k = 0; k < d.n; ++k) {
    auto &i = d.p[k];
    if (i.address + i.size != ret || i.id != X86_INS_CALL ||
        i.detail->x86.op_count != 1)
      continue;
    r.call = i.address;
    auto &a = i.detail->x86.operands[0];
    if (a.type != X86_OP_IMM) {
      r.kind = "indirect";
      r.reason = "anchored_indirect_dispatch_target_unproven";
      return r;
    }
    r.target = std::uintptr_t(a.imm);
    auto next = r.target;
    for (unsigned hop = 0; hop <= 2; ++hop) {
      r.next = next;
      r.hops = hop;
      if (next == consumer) {
        r.reaches = true;
        r.kind = hop ? "wrapper_tail_jump" : "direct";
        r.reason = "anchored_call_and_exact_direct_jump_chain";
        return r;
      }
      if (hop == 2 || !read)
        break;
      unsigned char code[5]{};
      if (!read(next, code, 2))
        break;
      std::int64_t delta{};
      unsigned length{};
      if (code[0] == 0xe9 && read(next, code, 5)) {
        std::int32_t v{};
        std::memcpy(&v, code + 1, 4);
        delta = v;
        length = 5;
      } else if (code[0] == 0xeb) {
        delta = std::int8_t(code[1]);
        length = 2;
      } else
        break;
      if (next > UINTPTR_MAX - length)
        break;
      auto end = next + length;
      if ((delta >= 0 && end > UINTPTR_MAX - std::uint64_t(delta)) ||
          (delta < 0 && end < std::uint64_t(-delta)))
        break;
      next = delta >= 0 ? end + std::uintptr_t(delta)
                        : end - std::uintptr_t(-delta);
    }
    r.reason = "direct_callee_or_tail_destination_is_exact_next_producer_edge";
    return r;
  }
  return r;
}
namespace {
ReadImageLoad classifyImageLoad(bool verified,std::uint32_t source,std::uint32_t imageSize,
    IdentityRead read,IdentityPageQuery query,void *ctx) {
  ReadImageLoad p{};p.attempted=true;p.source=source;
  auto fail=[&](const char *why) {p.reason=why;return p;};
  auto grab=[&](std::uint32_t at,void *dst,unsigned n) {
    return read && at<=imageSize && n<=imageSize-at && read(ctx,at,dst,n);
  };
  auto u16=[](const unsigned char *b) {std::uint16_t v{};std::memcpy(&v,b,2);return v;};
  auto u32=[](const unsigned char *b) {std::uint32_t v{};std::memcpy(&v,b,4);return v;};
  if (!verified || source!=readImageSource) return fail("image_load_exact_build_address_pin_required");
  if (source>=imageSize || 8u>imageSize-source) return fail("image_load_source_range_outside_image");
  if ((source&4095u)+8u>4096u) return fail("image_load_cross_page_refused");
  unsigned char dos[64]{},nt[264]{};
  if (!grab(0,dos,64) || u16(dos)!=0x5a4d) return fail("image_load_PE_DOS_invalid");
  const auto pe=u32(dos+60);
  if (pe<64 || pe>0x100000 || !grab(pe,nt,264) || u32(nt)!=0x4550 ||
      u16(nt+4)!=0x8664 || u16(nt+20)!=240 || u16(nt+24)!=0x20b ||
      u32(nt+24+56)!=imageSize || u32(nt+24+108)!=16 || !u16(nt+6) || u16(nt+6)>96)
    return fail("image_load_PE64_image_identity_or_shape_invalid");
  std::memcpy(p.metadataPrivate.data(),dos,64);std::memcpy(p.metadataPrivate.data()+64,nt,264);p.metadataBytes=328;
  const std::uint64_t sections=std::uint64_t(pe)+264;
  if (sections>imageSize || std::uint64_t(u16(nt+6))*40>imageSize-sections) return fail("image_load_PE_section_table_outside_image");
  unsigned whole=0;
  for (unsigned i=0;i<u16(nt+6);++i) {
    unsigned char sec[40]{};
    if (!grab(std::uint32_t(sections)+i*40,sec,40)) return fail("image_load_PE_section_table_unreadable");
    std::memcpy(p.metadataPrivate.data()+p.metadataBytes,sec,40);p.metadataBytes+=40;
    const auto begin=u32(sec+12),size=u32(sec+8),flags=u32(sec+36);
    if (begin>imageSize || size>imageSize-begin) return fail("image_load_PE_section_range_invalid");
    if (size && source<std::uint64_t(begin)+size && begin<std::uint64_t(source)+8) {
      ++p.sectionOverlaps;
      if (begin<=source && std::uint64_t(source)+8<=std::uint64_t(begin)+size) {
        ++whole;p.sectionBegin=begin;p.sectionEnd=begin+size;p.sectionFlags=flags;
        std::memcpy(p.sectionName,sec,8);p.sectionName[8]=0;
        p.sectionReadable=(flags&0x40000000u)!=0;p.sectionWritable=(flags&0x80000000u)!=0;
        p.sectionExecutable=(flags&0x20000000u)!=0;
      }
    }
  }
  p.sectionUnique=whole==1 && p.sectionOverlaps==1;
  // Direct authenticated directory ranges only; never chase descriptor pointers.
  for (unsigned index : {1u,9u,10u,11u,12u,13u}) {
    const auto begin=u32(nt+24+112+index*8),size=u32(nt+24+116+index*8);
    if ((begin==0)!=(size==0) || begin>imageSize || size>imageSize-begin)
      return fail("image_load_loader_directory_shape_invalid");
    if (size && source<std::uint64_t(begin)+size && begin<std::uint64_t(source)+8) {
      p.loaderOverlap=true;p.loaderDirectory=index;
    }
  }
  p.loaderRangesChecked=p.metadataValid=true;
  p.pageQueried=query && query(ctx,source,p.page);
  if (!p.sectionUnique) return fail("image_load_cross_section_or_ambiguous_source");
  if (!p.sectionReadable || p.sectionWritable) return fail("image_load_section_not_readonly");
  if (p.loaderOverlap) return fail("image_load_known_loader_directory_refused");
  if (!p.pageQueried) return fail("image_load_page_query_unavailable");
  const auto &page=p.page;
  if (!page.sameAllocation || !page.rangeInImage || page.type!=0x1000000 || page.state!=0x1000)
    return fail("image_load_mapping_not_committed_same_image");
  if (page.begin>source || !page.bytes || page.begin>imageSize || page.bytes>std::uint64_t(imageSize)-page.begin ||
      std::uint64_t(source)+8>std::uint64_t(page.begin)+page.bytes)
    return fail("image_load_page_range_or_cross_boundary_refused");
  if (page.protect!=0x02 && page.protect!=0x20)
    return fail("image_load_page_writable_COW_guard_noaccess_or_unsupported");
  p.policyPassed=true;p.reason="image_load_PE_page_readonly_before_value";return p;
}
bool sameImagePage(const ReadImagePage &a,const ReadImagePage &b) {
  return a.sameAllocation==b.sameAllocation && a.rangeInImage==b.rangeInImage &&
      a.begin==b.begin && a.bytes==b.bytes && a.state==b.state && a.type==b.type &&
      a.protect==b.protect && a.allocationProtect==b.allocationProtect;
}
}
ReadImageLoad beginReadImageLoad(bool verified,std::uint32_t source,std::uint32_t imageSize,
    IdentityRead read,IdentityPageQuery query,void *ctx) noexcept {
  auto p=classifyImageLoad(verified,source,imageSize,read,query,ctx);
  if (!p.policyPassed) return p;
  ++p.valueReadAttempts;p.firstRead=read(ctx,source,p.valuePrivate.data(),8);
  p.reason=p.firstRead?"image_load_first_qword_read_pending_final_check":"image_load_first_qword_read_failed";
  return p;
}
bool finishReadImageLoad(ReadImageLoad &p,std::uint32_t imageSize,
    IdentityRead read,IdentityPageQuery query,void *ctx) noexcept {
  p.admitted=p.stable=false;
  if (!p.policyPassed || !p.firstRead || p.source!=readImageSource || p.width!=8) return false;
  if (p.valueReadAttempts!=1) {p.reason="image_load_recheck_budget_or_state_invalid";return false;}
  const auto now=classifyImageLoad(true,p.source,imageSize,read,query,ctx);
  p.pageRechecked=now.pageQueried;p.finalPage=now.page;
  p.metadataSame=p.metadataBytes==now.metadataBytes && !std::memcmp(p.metadataPrivate.data(),now.metadataPrivate.data(),p.metadataBytes);
  p.pageSame=p.pageRechecked && sameImagePage(p.page,now.page);
  if (!now.policyPassed) {p.reason=now.reason;return false;}
  if (!p.metadataSame || !p.pageSame) {p.reason="image_load_PE_metadata_or_page_protection_changed";return false;}
  unsigned char again[8]{};++p.valueReadAttempts;p.finalRead=read && read(ctx,p.source,again,8);
  if (!p.finalRead) {p.reason="image_load_final_qword_read_failed";return false;}
  if (std::memcmp(p.valuePrivate.data(),again,8)) {p.reason="image_load_qword_changed_across_proof";return false;}
  p.stable=p.admitted=true;p.reason="exact_qword_readonly_image_current_window_stable";return true;
}
namespace {
bool pinnedUnknownMetadata(const ReadImageLoad &p,std::uint32_t imageSize) {
  const auto &page=p.page;
  return p.source==readImageSource && p.width==8 && p.metadataValid && p.sectionUnique &&
      p.sectionReadable && p.sectionWritable && !p.sectionExecutable &&
      p.sectionBegin==0x9b24000 && p.sectionEnd==0xc565a28 && p.sectionFlags==0xc0000040 &&
      !std::strcmp(p.sectionName,".data") && p.loaderRangesChecked && !p.loaderOverlap &&
      p.pageQueried && page.sameAllocation && page.rangeInImage && page.state==0x1000 && page.type==0x1000000 &&
      page.protect==0x04 && page.begin<=readImageSource && page.begin<=imageSize && page.bytes &&
      page.bytes<=std::uint64_t(imageSize)-page.begin && std::uint64_t(readImageSource)+8<=std::uint64_t(page.begin)+page.bytes &&
      !p.valueReadAttempts && !p.firstRead && !p.finalRead && !p.admitted;
}
}
bool beginPinnedUnknownLoad(ReadUnknownLoad &u,ReadImageLoad &p,std::uint32_t imageSize,
    IdentityRead read,IdentityPageQuery query,void *ctx) noexcept {
  u.attempted=true;u.admitted=false;
  p=classifyImageLoad(true,readImageSource,imageSize,read,query,ctx);
  u.metadataPassed=pinnedUnknownMetadata(p,imageSize);
  u.reason=u.metadataPassed?"pinned_unknown_top_metadata_pending_recheck":"pinned_unknown_top_current_source_metadata_refused";
  return u.metadataPassed;
}
bool finishPinnedUnknownLoad(ReadUnknownLoad &u,ReadImageLoad &p,std::uint32_t imageSize,
    IdentityRead read,IdentityPageQuery query,void *ctx) noexcept {
  u.admitted=false;
  if (!u.enabled || !u.attempted || !u.metadataPassed || u.metadataRechecked) {
    u.reason="pinned_unknown_top_recheck_state_or_budget_invalid";return false;
  }
  u.metadataRechecked=true;
  const auto now=classifyImageLoad(true,readImageSource,imageSize,read,query,ctx);
  u.metadataSame=p.metadataBytes==now.metadataBytes && !std::memcmp(p.metadataPrivate.data(),now.metadataPrivate.data(),p.metadataBytes);
  u.pageSame=now.pageQueried && sameImagePage(p.page,now.page);
  if (!pinnedUnknownMetadata(now,imageSize) || !u.metadataSame || !u.pageSame) {
    u.reason="pinned_unknown_top_source_metadata_or_page_changed";return false;
  }
  u.reason="pinned_unknown_top_metadata_rechecked_no_value_read";return true;
}
ReadEntryTags currentReadEntryEvidence(const ReadCallContract &c) noexcept {
  ReadEntryTags e{};e.samples=c.samples;
  for (unsigned k=0;k<16;++k) {e.gpr[k]=c.gpr[k].tags;e.xmm[k]=c.xmmTags[k];}
  const auto &base=c.gpr[1];
  e.imageLoadAddressVerified=c.samples && base.same && base.kind==pointer && base.root==globalRoot &&
      base.offset==readImageBaseOrigin && !base.tags;
  if (e.imageLoadAddressVerified) e.imageLoadSource=readImageSource;
  const auto &vector=c.gpr[2];e.vectorSourceVerified=c.samples && vector.same && vector.kind==pointer && vector.root==1 && vector.offset==0 && !vector.tags;
  e.xmmHigh=c.xmmHighTags;e.above256Zero=c.above256Zero;
  return e;
}
ReadTargetIdentity authenticateReadTarget(bool verified, std::uint32_t target,
    std::uint32_t imageSize, IdentityRead read, IdentityLookup lookup, void *ctx, const ReadEntryTags *entryTags,IdentityPageQuery pageQuery,bool pinnedUnknownLoad,bool pinnedVector256,bool pinnedSignExtend,bool pinnedShift) noexcept {
  static_assert(readCallByteCap==128 && readCallInstructionCap<=32,
                "No-unwind admission may not silently widen c23 leaf caps");
  ReadTargetIdentity out{}; out.enabled=true; out.target=target;out.unknownLoad.enabled=pinnedUnknownLoad;out.vector256Enabled=pinnedVector256;out.signExtendEnabled=pinnedSignExtend;out.shiftEnabled=pinnedShift;
  if (entryTags && entryTags->samples) {
    out.entryTagsKnown=true;out.entryTagSamples=entryTags->samples;
    for (unsigned k=0;k<16;++k) if ((entryTags->gpr[k]|entryTags->xmm[k])&~7u) out.entryTagsKnown=false;
  }
  auto fail=[&](const char *why,std::uint32_t at,const char *mn="metadata") {
    out.reason=why;out.barrier=at;copyName(out.mnemonic,sizeof(out.mnemonic),mn);return out;
  };
  auto grab=[&](std::uint32_t at,void *dst,unsigned n) {
    return at<=imageSize && n<=imageSize-at && read && read(ctx,at,dst,n);
  };
  auto u16=[](const unsigned char *b) { std::uint16_t v{};std::memcpy(&v,b,2);return v; };
  auto u32=[](const unsigned char *b) { std::uint32_t v{};std::memcpy(&v,b,4);return v; };
  if (!verified || target!=readIdentityTarget || !read || !lookup)
    return fail("target_exact_build_call_or_pin_not_admitted",target);
  if (target>=imageSize) return fail("target_outside_admitted_image",target);
  unsigned char dos[64]{},nt[264]{};
  if (!grab(0,dos,sizeof(dos)) || u16(dos)!=0x5a4d)
    return fail("target_PE_DOS_header_unavailable_or_invalid",target);
  const auto pe=u32(dos+60);
  if (pe<64 || pe>0x100000 || !grab(pe,nt,sizeof(nt)) || u32(nt)!=0x4550 ||
      u16(nt+4)!=0x8664 || u16(nt+24)!=0x20b || u32(nt+24+56)!=imageSize ||
      u16(nt+20)<240 || u16(nt+20)>4096 || !u16(nt+6) || u16(nt+6)>96 ||
      u32(nt+24+108)<4)
    return fail("target_PE64_headers_malformed",target);
  const std::uint64_t sections=std::uint64_t(pe)+24+u16(nt+20);
  if (sections>imageSize || std::uint64_t(u16(nt+6))*40>imageSize-sections)
    return fail("target_section_table_malformed",target);
  unsigned containing=0;
  for (unsigned i=0;i<u16(nt+6);++i) {
    unsigned char sec[40]{};
    if (!grab(std::uint32_t(sections)+i*40,sec,40))
      return fail("target_section_table_unreadable",target);
    const auto begin=u32(sec+12),size=u32(sec+8),flags=u32(sec+36);
    if (begin>imageSize || size>imageSize-begin)
      return fail("target_section_range_malformed",target);
    if (begin<=target && target-begin<size) {
      ++containing;
      // Executable code, readable and not writable; never admit data/RWX.
      if ((flags&0x60000020u)==0x60000020u && !(flags&0x80000000u)) {
        out.codeBegin=begin;out.codeEnd=begin+size;
      }
    }
  }
  if (containing!=1 || !out.codeEnd)
    return fail(containing>1?"target_code_section_ambiguous":"target_outside_readonly_executable_code",target);
  const auto table=u32(nt+24+112+3*8),tableSize=u32(nt+24+116+3*8);
  out.directoryRva=table;out.directoryBytes=tableSize;
  out.tableEntries=tableSize/12;
  out.directoryPairValid=(table==0)==(tableSize==0);
  out.directoryRangeInImage=table<=imageSize && tableSize<=imageSize-table;
  out.directorySizeDivisible=tableSize%12==0;
  out.byteCapExceeded=tableSize>readRuntimeTableByteCap;
  out.entryCapExceeded=out.tableEntries>readRuntimeTableEntryCap;
  if (!out.directoryPairValid)
    return fail("target_runtime_directory_RVA_size_pair_malformed",target);
  if (!out.directoryRangeInImage)
    return fail("target_runtime_directory_outside_admitted_image",target);
  if (!out.directorySizeDivisible)
    return fail("target_runtime_directory_size_not_entry_aligned",target);
  if (out.byteCapExceeded || out.entryCapExceeded) {
    out.validationSkippedByCap=true;
    return fail(out.byteCapExceeded?"target_runtime_metadata_byte_cap_exceeded"
        :"target_runtime_entry_cap_exceeded",target);
  }
  out.tableBudgetAdmitted=true;
  std::uint32_t previousEnd=0,staticBegin=0,staticEnd=0,staticUnwind=0;
  unsigned char chunk[64*12]{};
  for (unsigned i=0;i<out.tableEntries;) {
    const auto count=std::min(64u,out.tableEntries-i);
    if (!grab(table+i*12,chunk,count*12)) {
      out.tableReadFailed=true;
      return fail("target_runtime_table_unreadable",table+i*12);
    }
    out.metadataBytesRead+=count*12;
    for (unsigned j=0;j<count;++j) {
      const auto *entry=chunk+j*12;
      const auto begin=u32(entry),end=u32(entry+4),unwind=u32(entry+8);
      if (begin>=end || end>imageSize || begin<previousEnd || !unwind || unwind>=imageSize)
        return fail("target_runtime_metadata_malformed_or_overlapping",table+(i+j)*12);
      previousEnd=end;
      ++out.validatedEntries;
      if (begin<=target && target<end) { staticBegin=begin;staticEnd=end;staticUnwind=unwind; }
      if (begin>target && (!out.nextRuntime || begin<out.nextRuntime)) out.nextRuntime=begin;
    }
    i+=count;
  }
  out.tableReadComplete=out.tableValid=true;
  out.lookupQueried=true;out.lookup=lookup(ctx,target);
  const auto &rf=out.lookup;
  if (!rf.readable || (rf.found && (!rf.sameImage || rf.begin>=rf.end ||
      rf.end>imageSize || target<rf.begin || target>=rf.end)))
    return fail("target_runtime_lookup_malformed_or_wrong_image",target);
  out.runtimeBegin=rf.begin;out.runtimeEnd=rf.end;
  if ((staticEnd!=0)!=rf.found || (rf.found &&
      (staticBegin!=rf.begin || staticEnd!=rf.end || staticUnwind!=rf.unwind)))
    return fail("target_runtime_lookup_and_PE_table_disagree",target);
  unsigned limit=0;
  if (rf.found) {
    out.method="runtime_function";
    if (rf.begin!=target) return fail("target_inside_runtime_entry_not_begin",target);
    if (rf.end>out.codeEnd) return fail("target_runtime_span_crosses_code_section",target);
    if (rf.end-target>readCallByteCap) return fail("target_runtime_span_exceeds_leaf_cap",target);
    limit=rf.end-target;
  } else {
    out.missingRuntime=true;out.method="strict_single_target_no_unwind_linear";
    limit=std::min(readCallByteCap,out.codeEnd-target);
    if (out.nextRuntime) limit=std::min(limit,out.nextRuntime-target);
    // Check dynamic/runtime regions before reading any candidate bytes there.
    for (unsigned k=1;k<limit;++k) {
      auto region=lookup(ctx,target+k);
      if (!region.readable || (region.found && (!region.sameImage ||
          region.begin!=target+k || region.end<=region.begin || region.end>imageSize)))
        return fail("target_runtime_region_ambiguous_during_scan",target+k);
      if (region.found) { limit=k;out.nextRuntime=region.begin;break; }
    }
  }
  if (!limit) return fail("target_code_or_runtime_region_limit",target);
  out.scanBytes=limit;
  unsigned char again[readCallByteCap]{};
  if (!grab(target,out.snapshot.data(),limit)) return fail("target_guarded_snapshot_unavailable",target);
  if (!grab(target,again,limit)) return fail("target_guarded_recheck_unavailable",target);
  if (std::memcmp(again,out.snapshot.data(),limit)) return fail("target_guarded_snapshot_unstable",target);
  out.stable=true;
  if (rf.found) out.size=limit;
  else {
    Decode d;
    if (cs_open(CS_ARCH_X86,CS_MODE_64,&d.h)!=CS_ERR_OK)
      return fail("target_leaf_decoder_unavailable",target);
    cs_option(d.h,CS_OPT_DETAIL,CS_OPT_ON);cs_option(d.h,CS_OPT_SKIPDATA,CS_OPT_OFF);
    d.p=cs_malloc(d.h);if (!d.p) return fail("target_leaf_decoder_allocation_failed",target);
    d.n=1;
    const std::uint8_t *cursor=out.snapshot.data();std::size_t left=limit;
    std::uint64_t address=target;Slice registers;ReadFlow dependencies;
    for (unsigned k=0;k<16;++k) { dependencies.g[k]=1u<<k;dependencies.x[k]=1u<<(16+k); }
    bool flagsKnown=false;std::uint32_t flagsProducer=0;
    while (left) {
      const auto at=std::uint32_t(address);
      if (!cs_disasm_iter(d.h,&cursor,&left,&address,d.p))
        return fail("target_leaf_instruction_decode_failed",at,"decode");
      const auto &ins=*d.p;const auto &x=ins.detail->x86;
      if (++out.instructions>readCallInstructionCap)
        return fail("target_leaf_instruction_cap",at,ins.mnemonic);
      if (ins.id==X86_INS_RET && ins.size==1 && ins.bytes[0]==0xc3 && x.op_count==0) {
        out.size=unsigned(address-target);break;
      }
      if (group(ins,CS_GRP_CALL) || group(ins,CS_GRP_JUMP) || group(ins,CS_GRP_RET) ||
          group(ins,CS_GRP_INT) || group(ins,CS_GRP_IRET)) {
        if (group(ins,CS_GRP_JUMP)) {
          describeLeafBranch(out.branch,ins,flagsKnown,flagsProducer,dependencies);
          describeLeafEdges(out.branch,ins,out.codeBegin,out.codeEnd,limit,out.nextRuntime);
        }
        if (at==readIdentityBranch && ins.id==X86_INS_JA) {
          Slice merged;ReadFlow tags;
          for (unsigned k=0;k<16;++k) { tags.g[k]=1u<<k;tags.x[k]=1u<<(16+k); }
          const auto cfg=walkPinnedLeaf(out.snapshot.data(),limit,out.codeBegin,out.codeEnd,out.nextRuntime,
              out.entryTagsKnown?entryTags:nullptr,true,merged,tags,imageSize,&out.load,read,pageQuery,ctx,&out.unknownLoad,out.vector256Enabled,out.signExtendEnabled,out.shiftEnabled);
          out.cmp=cfg.cmp;out.branch=cfg.branch;out.mov=cfg.mov;out.vector=cfg.vector;out.signExtend=cfg.signExtend;out.shift=cfg.shift;out.instructions=cfg.instructions;
          out.paths=cfg.paths;out.visits=cfg.visits;out.cfgComplete=cfg.complete;
          out.pathEffectsDiffer=cfg.effectsDiffer;out.boundary=cfg.boundary;out.outcomes=cfg.outcomes;
          if (out.unknownLoad.attempted) {
            if (!finishPinnedUnknownLoad(out.unknownLoad,out.load,imageSize,read,pageQuery,ctx)) {
              out.boundary={};return fail(out.unknownLoad.reason,readIdentityMov,"mov");
            }
            if (!grab(target,again,limit) || std::memcmp(again,out.snapshot.data(),limit)) {
              out.stable=false;out.boundary={};out.unknownLoad.reason="pinned_unknown_top_target_code_changed";
              return fail(out.unknownLoad.reason,readIdentityMov,"mov");
            }
            out.unknownLoad.targetCodeRechecked=out.unknownLoad.admitted=true;
            out.unknownLoad.reason="exact_source_unknown_may_top_current_proof_no_value_read";
          }
          if (out.load.firstRead && !finishReadImageLoad(out.load,imageSize,read,pageQuery,ctx)) {
            out.mov.loadedMayTags=7;out.mov.reason=out.load.reason;
            return fail(out.load.reason,readIdentityMov,"mov");
          }
          if (out.load.firstRead) {
            if (!grab(target,again,limit) || std::memcmp(again,out.snapshot.data(),limit)) {
              out.load.admitted=false;out.load.reason="image_load_target_snapshot_changed";
              out.mov.loadedMayTags=7;out.mov.reason=out.load.reason;
              return fail(out.load.reason,readIdentityMov,"mov");
            }
            out.load.targetCodeRechecked=true;
          }
          if (!cfg.complete) return fail(cfg.reason,cfg.barrier,cfg.mnemonic);
          // All paths must pass before any span/hash can exist; pin current bytes again.
          if (!out.load.targetCodeRechecked && !out.unknownLoad.targetCodeRechecked && (!grab(target,again,limit) || std::memcmp(again,out.snapshot.data(),limit))) {
            out.stable=false;return fail("target_cfg_final_snapshot_changed",target,"snapshot");
          }
          out.finalCodeRechecked=true;out.size=cfg.size;
          out.method="strict_single_target_tag_disjoint_JA_all_paths";
          break;
        }
        return fail("target_leaf_control_or_nested_call_refused",at,ins.mnemonic);
      }
      if (at==readIdentityTarget && ins.id==X86_INS_CMP) {
        out.cmp=describeCmp(ins,d.h);
        if (!out.cmp.formSupported) return fail(out.cmp.reason,at,ins.mnemonic);
        dependencies.flags=carrierTags(out.cmp.flagsEntryGprMask,dependencies);
        dependencies.logicalTestFlags=false;flagsKnown=true;flagsProducer=at;
        out.cmp.modelApplied=true;
        continue;
      }
      for (unsigned j=0;j<x.op_count;++j) if (x.operands[j].type==X86_OP_MEM)
        return fail("target_leaf_memory_operand_refused",at,ins.mnemonic);
      if (ins.id==X86_INS_NOP || ins.id==X86_INS_ENDBR64) continue;
      cs_regs reads{},writes{};std::uint8_t nr{},nw{};
      if (cs_regs_access(d.h,&ins,reads,&nr,writes,&nw)!=CS_ERR_OK)
        return fail("target_leaf_register_effects_unknown",at,ins.mnemonic);
      unsigned incoming=0;
      for (unsigned j=0;j<nr;++j) {
        if (reads[j]==X86_REG_EFLAGS || (reg(reads[j])<0 && xmm(reads[j])<0))
          return fail("target_leaf_partial_or_flag_input_refused",at,ins.mnemonic);
        incoming |= dependencies.get(reads[j]);
      }
      for (unsigned j=0;j<nw;++j)
        if (reg(writes[j])==4 || (writes[j]!=X86_REG_EFLAGS && reg(writes[j])<0 && xmm(writes[j])<0))
          return fail("target_leaf_stack_or_partial_register_write_refused",at,ins.mnemonic);
      const bool pair=x.op_count==2 && x.operands[0].type==X86_OP_REG && x.operands[1].type==X86_OP_REG;
      const bool scalar=pair && reg(x.operands[0].reg)>=0 && reg(x.operands[1].reg)>=0 &&
          (x.operands[0].size==4 || x.operands[0].size==8) && x.operands[0].size==x.operands[1].size;
      const bool move=pair && (ins.id==X86_INS_MOV || ins.id==X86_INS_MOVD ||
          ins.id==X86_INS_MOVAPS || ins.id==X86_INS_MOVUPS);
      const bool zero=pair && x.operands[0].reg==x.operands[1].reg &&
          (ins.id==X86_INS_XOR || ins.id==X86_INS_XORPS || ins.id==X86_INS_PXOR);
      const bool flags=scalar && (ins.id==X86_INS_CMP || ins.id==X86_INS_TEST);
      if (x.prefix[0] || (!move && !zero && !flags) || !registers.step(ins,false))
        return fail("target_leaf_instruction_effect_not_admitted",at,ins.mnemonic);
      if (zero || (flags && ins.id==X86_INS_CMP && x.operands[0].reg==x.operands[1].reg)) incoming=0;
      for (unsigned j=0;j<nw;++j) {
        if (!dependencies.put(writes[j],incoming))
          return fail("target_leaf_register_effects_unknown",at,ins.mnemonic);
        if (writes[j]==X86_REG_EFLAGS) {
          dependencies.logicalTestFlags=ins.id==X86_INS_TEST || (zero && ins.id==X86_INS_XOR);
          flagsKnown=true;flagsProducer=at;
        }
      }
    }
    if (!out.size) return fail(out.nextRuntime && target+limit==out.nextRuntime
        ? "target_leaf_no_RET_before_runtime_region" : "target_leaf_no_RET_before_code_or_byte_cap",
        target+limit,"fallthrough");
  }
  // Recheck runtime coverage over the entire selected no-unwind span.
  for (unsigned k=0;k<(rf.found?1u:out.size);++k) {
    const auto now=lookup(ctx,target+k);
    if (!now.readable || now.found!=rf.found || (now.found &&
        (!now.sameImage || now.begin!=rf.begin || now.end!=rf.end || now.unwind!=rf.unwind))) {
      out.load.admitted=false;out.unknownLoad.admitted=false;if (out.load.attempted) out.mov.loadedMayTags=7;
      return fail("target_runtime_coverage_changed_before_admission",target+k);
    }
  }
  out.admitted=true;out.reason=rf.found?"runtime_boundary_current_snapshot_admitted"
      : out.cfgComplete?"pinned_JA_all_forward_RET_paths_admitted":"strict_linear_single_entry_RET_exit_set_admitted";
  return out;
}
} // namespace ws::rdx
