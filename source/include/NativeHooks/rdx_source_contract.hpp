#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
namespace ws::rdx {
// Bounded local slices, independent of the unchanged c12 analyzer.
// A certificate covers entry through the named stores, never the whole CFG.
inline constexpr unsigned writerByteCap = 183, blockCap = 16, edgeCap = 24,
                          branchCap = 4, pathCap = 16, stateCap = 31,
                          visitCap = 4096;
inline constexpr unsigned calleeByteCap = 1024;
// One exact call only. Target bytes are supplied by the authenticated runtime
// owner; this offline API never reads process memory or follows another edge.
struct CallEffect {
  bool instructionVerified{}, boundaryVerified{}, decoded{}, noWrite{},
      relevant{};
  std::uintptr_t writer{}, call{}, ret{}, target{}, begin{}, end{}, barrier{};
  unsigned gprClobber{}, xmmClobber{};
  std::int64_t privateStackLow{};
  const char *kind = "malformed/unresolved";
  const char *effect = "source_span_effect_unknown";
  const char *reason = "exact_call_not_authenticated";
  char mnemonic[32] = "none";
};
CallEffect exactCall(const unsigned char *, std::size_t,
                     std::uintptr_t writerRva, std::size_t imageSize,
                     unsigned callOffset, unsigned returnOffset) noexcept;
void classifyCallee(CallEffect &, const unsigned char *, std::size_t,
                    std::uintptr_t begin, std::uintptr_t end) noexcept;
inline constexpr unsigned aliasByteCap = 137, aliasWriteCap = 24;
// Absolute allocation ceiling, not the admitted CFG's branch budget. The
// complete authenticated decode derives its own exact branch/path/state caps.
inline constexpr unsigned aliasBranchCeiling = 8, aliasPathCeiling = 256;
enum AliasKind {
  privateStack,
  callerArea,
  sourceExact,
  sourcePossible,
  transformExact,
  transformPossible,
  imageGlobal,
  otherDisjoint,
  unknownPointer,
  aliasKinds
};
struct AliasWrite {
  std::uintptr_t at{};
  unsigned width{};
  int root{};
  std::int64_t offset{};
  AliasKind kind = unknownPointer;
};
struct ExactWriteContract {
  // Opt-in exact instruction RVA; production pins 0x2B2358E under the existing
  // complete callee identity. Zero retains the c18 behavior.
  std::uintptr_t site{}, dependencySite{};
  unsigned reachingStates{}, width{}, scale{};
  int root = -1, valueTag{}, valueRoot = -1;
  std::int64_t offset{}, displacement{}, valueOffset{};
  std::uint32_t valueBits{};
  bool decoded{}, destinationExact{},
      destinationSame = true, valueSame = true, valueExact{}, modeled{},
      complete{}, positionOverlap{}, carrierOverlap{}, reloadDependency{};
  const char *valueKind = "unknown";
  const char *destinationOrigin = "unknown";
  char mnemonic[32] = "none", base[32] = "none", index[32] = "none",
       sourceOperand[32] = "none";
};
inline constexpr unsigned aliasReadCap = 64;
struct AliasRead {
  std::uintptr_t at{};
  int root = -1;
  std::int64_t offset{};
  unsigned width{};
};
inline constexpr unsigned valueDefinitionCap = 16, valueChainCap = 8;
enum class BitKind { unknown, constant, entryRegister, memoryLoad, computed };
struct BitOrigin {
  BitKind kind = BitKind::unknown;
  int root = -1;
  std::int64_t offset{};
  std::uint32_t bits{}, addend{};
  std::uintptr_t producer{};
  const char *producerKind = "unknown";
  unsigned chainSize{};
  std::array<std::uintptr_t, valueChainCap> chain{};
};
struct EdiValueContract {
  bool enabled{}, complete{}, same = true, exact{}, writerStatesComplete{};
  unsigned paths{}, visits{}, reachingStates{}, definitions{};
  BitOrigin origin{}, conflict{};
  std::array<BitOrigin, valueDefinitionCap> producers{};
  std::uintptr_t barrier{};
  char mnemonic[32] = "none";
  const char *reason = "EDI_trace_not_enabled";
};
const char *bitKindName(BitKind) noexcept;
// One pinned TEST only. Parent tags for low-byte reads are conservative, not
// a byte-value proof. No partial register writes or effect certificates.
struct ReadTestContract {
  struct Operand {
    char kind[16] = "none", name[16] = "none", base[16] = "none",
         index[16] = "none", segment[16] = "none";
    unsigned width{}, scale{}, parentTags{};
    int parent = -1;
    std::int64_t displacement{}, immediate{};
    bool conservativeParent{};
  };
  std::uintptr_t site{}, firstConsumer{}, firstDependentConsumer{};
  unsigned operandCount{}, samples{}, modeledStates{}, flagsTags{},
      consumerSamples{}, consumerTags{}, returnedFlagsTags{};
  bool decoded{}, formSupported{}, flagsOnly{}, lowByteRead{};
  std::array<Operand, 2> operands{};
  char mnemonic[32] = "none", consumerMnemonic[32] = "none";
  const char *consumerKind = "none";
  const char *reason = "test_diagnostic_not_enabled";
};
// Diagnostic sidecar only: cannot grant any alias/effect/write certificate.
struct ReadDependencyContract {
  std::uintptr_t site{}, barrier{}, firstUse{}, firstControl{}, firstEscape{};
  unsigned calls{}, visits{}, paths{}, orderVisits{}, orderPaths{}, samples{}, width{}, scale{}, operandIndex{}, orderMask{},
      fieldOrderMask{}, returnedGpr{}, returnedXmm{}, newValueGpr{};
  int root = -1, overlap = -1;
  std::int64_t offset{}, displacement{};
  bool decoded{}, sameAddress = true, addressExact{}, orderComplete{}, flowComplete{},
      controlUse{}, memoryEscape{}, returnedFlags{}, returnedCallerStack{};
  const char *addressClass = "unknown", *carrierKind = "unknown";
  const char *reason = "read_dependency_not_enabled";
  char mnemonic[32] = "none", base[32] = "none", index[32] = "none",
       carrier[32] = "none", unresolvedMnemonic[32] = "none";
  ReadTestContract test{};
};
struct CalleeAlias {
  // Only authenticated current target bytes; never dereferenced process
  // pointers.
  const unsigned char *code{};
  std::size_t size{}, imageSize{};
  bool authenticated{}, argumentsComplete{}, resolved{}, sourceNoWrite{},
      transformNoWrite{}, usesGlobals{}, positionProtected{},
      relevantEffectsResolved{}, transformWriteSeen{};
  ExactWriteContract exactWrite{};
  EdiValueContract edi{};
  ReadDependencyContract readDependency{};
  unsigned reads{};
  std::array<AliasRead, aliasReadCap> readSites{};
  unsigned analyses{}, visits{}, paths{}, writes{}, sourceArgMask{},
      transformArgMask{};
  std::array<const char *, 4> argumentOrigins{
      {"unknown", "unknown", "unknown", "unknown"}};
  std::array<std::int64_t, 4> argumentOffsets{};
  std::array<unsigned, aliasKinds> counts{};
  std::array<AliasWrite, aliasWriteCap> writeSites{};
  unsigned localBranches{}, conditionalBranches{}, branchBudget{}, pathBudget{},
      stateBudget{}, blocks{}, edges{}, blocksVisited{}, edgesVisited{},
      backedges{}, indirectTransfers{}, nestedCalls{};
  bool cfgComplete{};
  std::int64_t stackLow{}, stackHigh{};
  std::uintptr_t barrier{};
  int barrierRoot{};
  std::int64_t barrierOffset{};
  unsigned barrierWidth{};
  const char *barrierEffect = "none";
  const char *reason = "callee_alias_identity_not_authenticated";
  char mnemonic[32] = "none";
};
const char *aliasKindName(AliasKind) noexcept;
struct Writer {
  bool decoded{}, prefix{}, xyz{}, w{}, wConstant{};
  int wSourceReg{};
  std::int64_t wSourceOffset{}, callStackLow{}, callStackHigh{};
  unsigned mask{}, barrier{}, prefixEnd{};
  std::array<unsigned, 4> stores{};
  const char *wKind = "unknown";
  std::uint32_t wBits{};
  char barrierMnemonic[32] = "none";
  bool branchAware{}, bounded{}, exactSites{}, allRelevant{}, barrierValid{},
      barrierHasEdge{};
  unsigned blocks{}, edges{}, branches{}, blocksVisited{}, edgesVisited{},
      firstBranch{}, pathsExamined{}, statesScheduled{}, visits{},
      barrierEdge{}, componentProofMask{};
  std::array<unsigned, 4> reachingStates{};
  std::array<const char *, 3> paths{{"unproven", "unproven", "unproven"}};
  const char *reason = "missing_writer";
};
Writer writer(const unsigned char *,
              std::size_t) noexcept; // retained c14 straight-line API
// Enumerates every relevant forward local path under hard caps. Never reads
// process memory, follows new call edges, narrows branches from equality or
// runs a fixpoint. Optional call summary is conditional on the per-invocation
// entry RDX==entry RSP+0x28, nonstack Transform and callStackLow within
// the captured current-thread TEB stack bounds.
Writer branchWriter(const unsigned char *, std::size_t,
                    const std::array<unsigned, 4> &exactStores,
                    CallEffect *call = nullptr,
                    CalleeAlias *alias = nullptr) noexcept;
struct Caller {
  bool decoded{}, prefix{}, rdxLocal{}, owned{}, constructed{};
  std::int64_t rspOffset{}, rdxOffset{};
  unsigned barrier{};
  std::array<unsigned, 3> producers{};
  char barrierMnemonic[32] = "none";
  const char *reason = "missing_caller";
};
Caller caller(const unsigned char *, std::size_t, unsigned callOffset) noexcept;
using Read = bool (*)(std::uintptr_t, void *, std::size_t);
struct Relation {
  bool decoded{}, reaches{};
  std::uintptr_t call{}, target{}, next{};
  unsigned hops{};
  const char *kind = "unresolved";
  const char *reason = "missing_anchored_call";
};
// Only an anchored CALL and up to two exact direct JMP thunks (E9/EB).
// Indirect dispatch records the concrete operand site, never an assumed target.
Relation relation(const unsigned char *, std::size_t, std::uintptr_t begin,
                  std::uintptr_t ret, std::uintptr_t consumer, Read) noexcept;
struct Storage {
  bool bounded{}, sameBytes{}, callerOwned{};
};
Storage storage(std::uintptr_t rsp, std::uintptr_t rdx, std::uintptr_t low,
                std::uintptr_t high, bool reads, bool sameBytes,
                const Caller &) noexcept;
} // namespace ws::rdx
