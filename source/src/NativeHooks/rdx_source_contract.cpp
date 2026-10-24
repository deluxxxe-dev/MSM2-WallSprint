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
struct ReadFlow {
  std::array<unsigned, 16> g{}, x{};
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
  if (!d.run(c.code, c.size, begin) || d.n > readCallInstructionCap)
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
        }
      c.returnedFlagsTags |= tags.flags;
      if (!c.barrier) c.reason = "bounded_register_only_leaf_normal_return";
      return true;
    }
    if (group(ins, CS_GRP_CALL) || group(ins, CS_GRP_JUMP) ||
        group(ins, CS_GRP_RET) || group(ins, CS_GRP_INT) || group(ins, CS_GRP_IRET))
      return fail(ins.address, ins.mnemonic, "read_call_nested_or_control_depth_limit");
    if (ins.id == X86_INS_NOP || ins.id == X86_INS_ENDBR64) continue;
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
          arg.tags |= w.flow.g[k]; nested.xmmTags[k] |= w.flow.x[k];
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
    r.reason = r.returnedGpr || r.returnedXmm || r.returnedFlags || r.returnedCallerStack
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
} // namespace ws::rdx
