#include "NativeHooks/movement_lineage.hpp"
#include <algorithm>
#include <capstone/capstone.h>
#include <cmath>
#include <cstdio>
#include <cstring>
namespace ws::lineage {
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
  bool run(const unsigned char *b, std::size_t z, unsigned &failure) {
    if (!b || !z || z > 8192 ||
        cs_open(CS_ARCH_X86, CS_MODE_64, &h) != CS_ERR_OK)
      return false;
    cs_option(h, CS_OPT_DETAIL, CS_OPT_ON);
    cs_option(h, CS_OPT_SKIPDATA, CS_OPT_OFF);
    n = cs_disasm(h, b, z, 0, 0, &p);
    unsigned end = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (!p[i].id || !p[i].detail || p[i].address != end)
        break;
      end += p[i].size;
    }
    failure = end;
    return end == z;
  }
};
int gpr(unsigned r) {
  switch (r) {
#define G(R, E, N)                                                             \
  case X86_REG_##R:                                                            \
  case X86_REG_##E:                                                            \
    return N
    G(RAX, EAX, 0);
    G(RCX, ECX, 1);
    G(RDX, EDX, 2);
    G(RBX, EBX, 3);
    G(RSP, ESP, 4);
    G(RBP, EBP, 5);
    G(RSI, ESI, 6);
    G(RDI, EDI, 7);
    G(R8, R8D, 8);
    G(R9, R9D, 9);
    G(R10, R10D, 10);
    G(R11, R11D, 11);
    G(R12, R12D, 12);
    G(R13, R13D, 13);
    G(R14, R14D, 14);
    G(R15, R15D, 15);
#undef G
  default:
    return -1;
  }
}
int xmm(unsigned r) {
  return r >= X86_REG_XMM0 && r <= X86_REG_XMM15 ? int(r - X86_REG_XMM0) : -1;
}
bool group(const cs_insn &p, unsigned g) {
  for (unsigned i = 0; i < p.detail->groups_count; ++i)
    if (p.detail->groups[i] == g)
      return true;
  return false;
}
struct State {
  std::array<unsigned, 16> g{};
  std::array<unsigned, 64> x{};
  struct Slot {
    std::int64_t off{};
    unsigned v{}, width{};
    bool used{};
  };
  std::array<Slot, 64> stack{};
};
unsigned node(Proof &p, Expr e) {
  for (unsigned i = 1; i < p.nodes; ++i) {
    auto &a = p.expr[i];
    if (a.op == e.op && a.a == e.a && a.b == e.b && a.reg == e.reg &&
        a.lane == e.lane && a.offset == e.offset && a.bits == e.bits)
      return i;
  }
  if (p.nodes == maxNodes) {
    p.exhausted = true;
    return 0;
  }
  p.expr[p.nodes] = e;
  return p.nodes++;
}
unsigned ptr(Proof &p, unsigned a, std::int64_t off) {
  if (!a || off < -(1ll << 32) || off > (1ll << 32))
    return 0;
  if (!off)
    return a;
  auto e = p.expr[a];
  if (e.op == Op::add && e.b == 0) {
    off += e.offset;
    a = e.a;
  }
  return node(p, {Op::add, a, 0, 0, 0, off, 0});
}
bool root(const Proof &p, unsigned a, unsigned reg, std::int64_t &off) {
  off = 0;
  if (!a || a >= p.nodes)
    return false;
  auto e = p.expr[a];
  if (e.op == Op::add && e.b == 0) {
    off = e.offset;
    e = p.expr[e.a];
  }
  return e.op == Op::gpr && e.reg == reg;
}
unsigned address(Proof &p, const State &s, const cs_x86_op &a) {
  if (a.type != X86_OP_MEM || a.mem.index != X86_REG_INVALID ||
      a.mem.segment != X86_REG_INVALID)
    return 0;
  int g = gpr(a.mem.base);
  return g >= 0 ? ptr(p, s.g[g], a.mem.disp) : 0;
}
unsigned load(Proof &p, State &s, unsigned a, unsigned width = 4) {
  std::int64_t off{};
  if (root(p, a, 4, off))
    for (auto &slot : s.stack)
      if (slot.used && off < slot.off + slot.width && slot.off < off + width)
        return slot.off == off && slot.width == width ? slot.v : 0;
  return a ? node(p, {Op::load, a, 0, 0, 0, 0, width}) : 0;
}
void stackStore(Proof &p, State &s, unsigned a, unsigned v,
                unsigned width = 8) {
  std::int64_t off{};
  if (!root(p, a, 4, off))
    return;
  for (auto &t : s.stack)
    if (t.used && off < t.off + t.width && t.off < off + width)
      t.used = false;
  for (auto &t : s.stack)
    if (!t.used) {
      t = {off, v, width, true};
      return;
    }
  p.exhausted = true;
}
void init(Proof &p, State &s) {
  for (unsigned i = 0; i < 16; ++i)
    s.g[i] = node(p, {Op::gpr, 0, 0, i});
  for (unsigned i = 0; i < 64; ++i)
    s.x[i] = node(p, {Op::xmm, 0, 0, i / 4, i % 4});
}
unsigned value(Proof &p, State &s, const cs_x86_op &a, unsigned lane = 0) {
  if (a.type == X86_OP_MEM)
    return load(p, s, ptr(p, address(p, s, a), 4 * lane), a.size == 8 ? 8 : 4);
  if (a.type == X86_OP_IMM)
    return node(p, {Op::constant, 0, 0, 0, 0, 0, std::uint32_t(a.imm)});
  if (a.type == X86_OP_REG) {
    int g = gpr(a.reg), x = xmm(a.reg);
    if (g >= 0)
      return s.g[g];
    if (x >= 0)
      return s.x[4 * x + lane];
  }
  return 0;
}
void bad(Proof &p, unsigned at) {
  if (!p.unsupported)
    p.firstUnsupported = at;
  if (p.unsupported < p.unsupportedOffsets.size())
    p.unsupportedOffsets[p.unsupported] = at;
  ++p.unsupported;
}
bool step(Proof &p, State &s, const cs_insn &ins, bool collect) {
  auto &x = ins.detail->x86;
  unsigned at = unsigned(ins.address);
  if (ins.id == X86_INS_NOP || ins.id == X86_INS_ENDBR64)
    return true;
  if (ins.id == X86_INS_RET) {
    p.returned = true;
    return true;
  }
  if (group(ins, CS_GRP_CALL) || group(ins, CS_GRP_JUMP) ||
      group(ins, CS_GRP_INT) || p.returned) {
    bad(p, at);
    s = {};
    return false;
  }
  if (ins.id == X86_INS_PUSH && x.op_count == 1 &&
      x.operands[0].type == X86_OP_REG && gpr(x.operands[0].reg) >= 0 &&
      gpr(x.operands[0].reg) != 4) {
    s.g[4] = ptr(p, s.g[4], -8);
    stackStore(p, s, s.g[4], value(p, s, x.operands[0]));
    return true;
  }
  if (ins.id == X86_INS_POP && x.op_count == 1 &&
      x.operands[0].type == X86_OP_REG && gpr(x.operands[0].reg) >= 0) {
    auto g = gpr(x.operands[0].reg);
    if (g == 4) {
      bad(p, at);
      return false;
    }
    s.g[g] = load(p, s, s.g[4], 8);
    s.g[4] = ptr(p, s.g[4], 8);
    return true;
  }
  if (x.op_count < 2) {
    bad(p, at);
    return false;
  }
  auto &d = x.operands[0];
  auto &src = x.operands[1];
  int dg = d.type == X86_OP_REG ? gpr(d.reg) : -1,
      dx = d.type == X86_OP_REG ? xmm(d.reg) : -1;
  if ((ins.id == X86_INS_ADD || ins.id == X86_INS_SUB) && dg >= 0 &&
      d.size == 8 && src.type == X86_OP_IMM) {
    s.g[dg] = ptr(p, s.g[dg], (ins.id == X86_INS_ADD ? 1 : -1) * src.imm);
    return true;
  }
  if (ins.id == X86_INS_LEA && dg >= 0 && d.size == 8) {
    s.g[dg] = address(p, s, src);
    if (s.g[dg])
      return true;
    bad(p, at);
    return false;
  }
  bool scalar = ins.id == X86_INS_MOVSS || ins.id == X86_INS_VMOVSS;
  bool move = scalar || ins.id == X86_INS_MOV || ins.id == X86_INS_MOVUPS ||
              ins.id == X86_INS_MOVAPS || ins.id == X86_INS_VMOVUPS ||
              ins.id == X86_INS_VMOVAPS || ins.id == X86_INS_MOVD;
  if (move && x.op_count == 2) {
    if (d.type == X86_OP_MEM) {
      auto a = address(p, s, d);
      std::int64_t off{};
      if (root(p, a, 4, off)) {
        if (d.size != 4 && d.size != 8) {
          bad(p, at);
          return false;
        }
        stackStore(p, s, a, value(p, s, src), d.size);
        return true;
      }
      if (!root(p, a, 1, off) || off < 48 || off + d.size > 64 || off % 4 ||
          !(d.size == 4 || d.size == 16)) {
        bad(p, at);
        return false;
      }
      for (unsigned lane = 0; lane < d.size / 4; ++lane) {
        unsigned v = value(p, s, src, lane),
                 component = unsigned(off - 48) / 4 + lane;
        if (collect) {
          if (p.storeCount == maxStores || p.mask & (1u << component)) {
            bad(p, at);
            return false;
          }
          auto &t = p.stores[p.storeCount++];
          t.offset = at;
          t.after = at + ins.size;
          t.component = component;
          t.value = v;
          t.base = ptr(p, a, 4 * lane);
          t.sourceReg =
              src.type == X86_OP_REG
                  ? unsigned(xmm(src.reg) >= 0 ? xmm(src.reg)
                                               : std::max(0, gpr(src.reg)))
                  : 0;
          t.vector = src.type == X86_OP_REG && xmm(src.reg) >= 0;
          t.sourceLane = lane;
          t.liveGpr = s.g;
          t.liveXmm = s.x;
          p.mask |= 1u << component;
        }
        if (!v)
          bad(p, at);
      }
      return true;
    }
    if (dg >= 0) {
      if (d.size != 8 && d.size != 4) {
        bad(p, at);
        return false;
      }
      s.g[dg] = value(p, s, src);
      if (d.size == 4 && p.expr[s.g[dg]].op == Op::load &&
          p.expr[s.g[dg]].bits != 4) {
        s.g[dg] = 0;
        bad(p, at);
      }
      if (d.size == 4 && p.expr[s.g[dg]].op != Op::load &&
          p.expr[s.g[dg]].op != Op::constant) {
        s.g[dg] = 0;
        bad(p, at);
      }
      return true;
    }
    if (dx >= 0) {
      unsigned lanes = scalar || ins.id == X86_INS_MOVD ? 1 : 4;
      auto old = s;
      for (unsigned lane = 0; lane < lanes; ++lane)
        s.x[4 * dx + lane] = value(p, old, src, lane);
      if (lanes == 1 && (src.type == X86_OP_MEM || ins.id == X86_INS_MOVD))
        for (unsigned lane = 1; lane < 4; ++lane)
          s.x[4 * dx + lane] = node(p, {Op::constant});
      return true;
    }
  }
  Op op = Op::unknown;
  bool scalarOp = false;
  switch (ins.id) {
  case X86_INS_ADDSS:
  case X86_INS_VADDSS:
    scalarOp = true;
    [[fallthrough]];
  case X86_INS_ADDPS:
  case X86_INS_VADDPS:
    op = Op::add;
    break;
  case X86_INS_SUBSS:
  case X86_INS_VSUBSS:
    scalarOp = true;
    [[fallthrough]];
  case X86_INS_SUBPS:
  case X86_INS_VSUBPS:
    op = Op::sub;
    break;
  case X86_INS_MULSS:
  case X86_INS_VMULSS:
    scalarOp = true;
    [[fallthrough]];
  case X86_INS_MULPS:
  case X86_INS_VMULPS:
    op = Op::mul;
    break;
  default:
    break;
  }
  if (op != Op::unknown && dx >= 0 && (x.op_count == 2 || x.op_count == 3)) {
    auto old = s;
    for (unsigned lane = 0; lane < (scalarOp ? 1u : 4u); ++lane) {
      unsigned a = value(p, old, x.op_count == 3 ? src : d, lane),
               b = value(p, old, x.operands[x.op_count - 1], lane);
      s.x[4 * dx + lane] = a && b ? node(p, {op, a, b}) : 0;
      if (!s.x[4 * dx + lane])
        bad(p, at);
    }
    if (scalarOp && x.op_count == 3)
      for (unsigned lane = 1; lane < 4; ++lane)
        s.x[4 * dx + lane] = value(p, old, src, lane);
    return true;
  }
  bad(p, at);
  s = {};
  return false;
}
void textExpr(const Proof &p, unsigned n, char *out, std::size_t size,
              unsigned depth) {
  if (!size)
    return;
  if (!n || n >= p.nodes || depth > 12) {
    std::snprintf(out, size, "unknown");
    return;
  }
  auto &e = p.expr[n];
  char a[192]{}, b[192]{};
  if (e.a)
    textExpr(p, e.a, a, sizeof(a), depth + 1);
  if (e.b)
    textExpr(p, e.b, b, sizeof(b), depth + 1);
  switch (e.op) {
  case Op::gpr:
    std::snprintf(out, size, "entry_gpr%u", e.reg);
    break;
  case Op::xmm:
    std::snprintf(out, size, "entry_xmm%u.lane%u", e.reg, e.lane);
    break;
  case Op::load:
    std::snprintf(out, size, "load%u(%s)", e.bits * 8, a);
    break;
  case Op::constant:
    std::snprintf(out, size, "bits_0x%x", e.bits);
    break;
  case Op::add:
    if (!e.b) {
      std::snprintf(out, size, "(%s%+lld)", a, (long long)e.offset);
      break;
    }
    [[fallthrough]];
  case Op::sub:
  case Op::mul:
    std::snprintf(out, size, "(%s%s%s)", a,
                  e.op == Op::mul   ? "*"
                  : e.op == Op::sub ? "-"
                                    : "+",
                  b);
    break;
  default:
    std::snprintf(out, size, "unknown");
  }
}
bool recover(const Store &t, unsigned id, const Context &c, std::uint64_t &raw,
             bool vector) {
  if (!vector)
    for (unsigned i = 0; i < 16; ++i)
      if (t.liveGpr[i] == id) {
        raw = c.gpr[i];
        return true;
      }
  if (vector && c.floating)
    for (unsigned i = 0; i < 64; ++i)
      if (t.liveXmm[i] == id) {
        std::uint32_t b{};
        std::memcpy(&b, &c.xmm[i], 4);
        raw = b;
        return true;
      }
  return false;
}
bool pointer(const Proof &p, unsigned id, const Store &t, const Context &c,
             std::uintptr_t &out, unsigned depth = 0) {
  if (!id || depth > 16)
    return false;
  std::uint64_t raw{};
  if (recover(t, id, c, raw, false)) {
    out = raw;
    return true;
  }
  auto e = p.expr[id];
  if (e.op == Op::add && !e.b) {
    std::uintptr_t a{};
    if (!pointer(p, e.a, t, c, a, depth + 1))
      return false;
    if (e.offset >= 0) {
      if (a > UINTPTR_MAX - std::uint64_t(e.offset))
        return false;
      out = a + e.offset;
    } else {
      if (a < std::uint64_t(-e.offset))
        return false;
      out = a - std::uint64_t(-e.offset);
    }
    return true;
  }
  return false;
}
bool eval(const Proof &p, unsigned id, const Store &t, const Context &c,
          Read read, float &out, unsigned &budget, unsigned depth = 0) {
  if (!id || id >= p.nodes || depth > 16 || !budget)
    return false;
  --budget;
  auto e = p.expr[id];
  std::uint64_t raw{};
  if (recover(t, id, c, raw, true) ||
      (e.op != Op::add && e.op != Op::sub && e.op != Op::mul &&
       recover(t, id, c, raw, false))) {
    auto b = std::uint32_t(raw);
    std::memcpy(&out, &b, 4);
    return std::isfinite(out);
  }
  if (e.op == Op::constant) {
    std::memcpy(&out, &e.bits, 4);
    return std::isfinite(out);
  }
  if (e.op == Op::load) {
    std::uintptr_t a{};
    return e.bits == 4 && read && pointer(p, e.a, t, c, a) &&
           a <= UINTPTR_MAX - 4 && read(a, &out, 4) && std::isfinite(out);
  }
  float a{}, b{};
  if (!e.b || !eval(p, e.a, t, c, read, a, budget, depth + 1) ||
      !eval(p, e.b, t, c, read, b, budget, depth + 1))
    return false;
  out = e.op == Op::add   ? a + b
        : e.op == Op::sub ? a - b
        : e.op == Op::mul ? a * b
                          : NAN;
  return std::isfinite(out);
}
unsigned substitute(const Proof &from, unsigned id, Proof &to, State &s,
                    unsigned depth = 0) {
  if (!id || id >= from.nodes || depth > 16)
    return 0;
  auto e = from.expr[id];
  if (e.op == Op::gpr)
    return s.g[e.reg];
  if (e.op == Op::xmm)
    return s.x[e.reg * 4 + e.lane];
  if (e.op == Op::constant)
    return node(to, e);
  unsigned a = substitute(from, e.a, to, s, depth + 1);
  if (!a)
    return 0;
  if (e.op == Op::load)
    return load(to, s, a, e.bits);
  if (e.op == Op::add && !e.b)
    return ptr(to, a, e.offset);
  unsigned b = substitute(from, e.b, to, s, depth + 1);
  return b ? node(to, {e.op, a, b}) : 0;
}
bool hasOp(const Proof &p, unsigned id, Op op, unsigned &budget,
           unsigned depth = 0) {
  if (!id || id >= p.nodes || depth > 32 || !budget)
    return false;
  --budget;
  auto e = p.expr[id];
  if (e.op == op && (op != Op::add || e.b))
    return true;
  return (e.a && hasOp(p, e.a, op, budget, depth + 1)) ||
         (e.b && hasOp(p, e.b, op, budget, depth + 1));
}
} // namespace
Proof writer(const unsigned char *b, std::size_t n) noexcept {
  Proof p{};
  try {
    Decode d;
    p.bytes = unsigned(n);
    p.decoded = d.run(b, n, p.failure);
    if (!p.decoded) {
      p.reason = "complete_decode_failed_or_span_over_8192";
      return p;
    }
    State s{};
    init(p, s);
    for (std::size_t i = 0; i < d.n; ++i)
      step(p, s, d.p[i], true);
    p.sourceMap = p.mask == 15 && p.storeCount == 4 && !p.exhausted;
    for (unsigned i = 0; i < p.storeCount; ++i)
      if (p.unsupported && p.firstUnsupported < p.stores[i].after)
        p.sourceMap = false;
    p.complete = p.returned && p.sourceMap && !p.unsupported;
    p.reason =
        p.exhausted ? "bounded_expression_or_stack_capacity_exhausted"
        : p.complete
            ? "complete_straight_line_XYZW_lineage_NOT_exclusive_semantics"
        : p.unsupported ? "unsupported_effect_or_CFG_edge_at_first_offset"
                        : "partial_or_missing_XYZW_or_return";
  } catch (...) {
    p.reason = "lineage_internal_failure";
  }
  return p;
}
Call callsite(const unsigned char *b, std::size_t n, std::uint64_t begin,
              std::uint64_t ret, std::uint64_t target) noexcept {
  Call r{};
  try {
    Decode d;
    r.decoded = d.run(b, n, r.failure);
    if (!r.decoded) {
      r.reason = "caller_complete_decode_failed_or_span_over_8192";
      return r;
    }
    if (ret <= begin || ret - begin > n) {
      r.reason = "return_outside_caller";
      return r;
    }
    for (std::size_t i = 0; i < d.n; ++i) {
      auto &p = d.p[i];
      if (p.address + p.size != ret - begin)
        continue;
      r.offset = unsigned(p.address);
      if (p.id != X86_INS_CALL || p.size != 5 || b[p.address] != 0xe8) {
        r.reason = "return_not_anchored_E8";
        return r;
      }
      std::int32_t delta{};
      std::memcpy(&delta, b + p.address + 1, 4);
      r.verified = std::int64_t(ret) + delta == std::int64_t(target);
      r.reason = r.verified ? "complete_caller_anchored_direct_call"
                            : "direct_target_mismatch";
      return r;
    }
    r.reason = "return_not_instruction_boundary";
  } catch (...) {
    r.reason = "callsite_internal_failure";
  }
  return r;
}
Caller caller(const unsigned char *b, std::size_t n, unsigned offset,
              const Proof &w) noexcept {
  Caller r{};
  try {
    Decode d;
    unsigned failure{};
    r.decoded = d.run(b, n, failure);
    if (!r.decoded) {
      r.reason = "caller_decode_failed";
      return r;
    }
    State s{};
    init(r.slice, s);
    for (std::size_t i = 0; i < d.n; ++i) {
      auto &p = d.p[i];
      if (p.address == offset) {
        for (unsigned k = 0; k < 4; ++k) {
          if (!w.sourceMap)
            break;
          r.payload[w.stores[k].component] =
              substitute(w, w.stores[k].value, r.slice, s);
        }
        r.sliceComplete = !r.unsupported && !r.slice.exhausted &&
                          std::all_of(r.payload.begin(), r.payload.end(),
                                      [](unsigned a) { return a != 0; });
        bool add = false, mul = false;
        for (auto id : r.payload) {
          unsigned budget = 256;
          add |= hasOp(r.slice, id, Op::add, budget);
          budget = 256;
          mul |= hasOp(r.slice, id, Op::mul, budget);
        }
        r.shape = add && mul ? "add_and_multiply_untyped"
                  : add      ? "add_untyped"
                  : mul      ? "multiply_untyped"
                             : "copy_or_unknown";
        r.reason =
            r.sliceComplete
                ? "typed_dt_direction_speed_and_exclusive_effects_unproven"
                : "caller_slice_barrier_or_payload_provenance_unresolved";
        return r;
      }
      if (group(p, CS_GRP_CALL) || group(p, CS_GRP_JUMP) ||
          group(p, CS_GRP_RET)) {
        s = {};
        r.barrier = unsigned(p.address);
        ++r.unsupported;
        continue;
      }
      auto count = r.slice.unsupported;
      step(r.slice, s, p, false);
      if (count != r.slice.unsupported) {
        ++r.unsupported;
        r.barrier = unsigned(p.address);
      }
      r.producerOffset = unsigned(p.address);
    }
    r.reason = "call_offset_not_found";
  } catch (...) {
    r.reason = "caller_inspection_failed";
  }
  return r;
}
void describe(const Proof &p, unsigned n, char *b, std::size_t z) noexcept {
  textExpr(p, n, b, z, 0);
}
Sample sample(const Proof &p, unsigned after, const Context &c,
              std::uintptr_t target, const float observed[4],
              Read read) noexcept {
  Sample r{};
  if (!p.sourceMap || !observed || !target || target > UINTPTR_MAX - 64)
    return r;
  for (unsigned i = 0; i < p.storeCount; ++i) {
    auto &t = p.stores[i];
    if (t.after != after)
      continue;
    r.found = true;
    r.component = t.component;
    r.finalStore = t.after == p.stores[p.storeCount - 1].after;
    std::uintptr_t dest{};
    if (!pointer(p, t.base, t, c, dest) ||
        dest != target + 48 + 4 * t.component)
      continue;
    r.target = true;
    bool ok = true;
    unsigned budget = 64;
    for (unsigned j = 0; j < p.storeCount; ++j) {
      auto &v = p.stores[j];
      float f{};
      if (eval(p, v.value, t, c, read, f, budget)) {
        r.payload[v.component] = f;
        r.mask |= 1u << v.component;
      } else
        ok = false;
    }
    r.readable = r.stable = ok;
    r.fullRow = ok;
    r.wObserved = ok;
    r.match = ok && !std::memcmp(r.payload, observed, 16);
    return r;
  }
  return r;
}
} // namespace ws::lineage
