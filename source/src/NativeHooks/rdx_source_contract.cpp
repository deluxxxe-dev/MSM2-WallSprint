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
