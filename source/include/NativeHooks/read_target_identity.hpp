#pragma once
#include "NativeHooks/vector_dependency.hpp"
#include <array>
#include <cstdint>
namespace ws::rdx {
inline constexpr std::uint32_t readIdentityTarget = 0x2cf7fe0;
// C25 actual complete table:527161 entries/6325932 bytes. Finite budget:
// <=600000 entries, <=8MiB, at most9375 guarded64-entry reads.
inline constexpr std::uint32_t readRuntimeTableEntryCap = 600000,
    readRuntimeTableByteCap = 8u*1024*1024;
struct RuntimeRegion {
  bool found{}, sameImage = true, readable = true;
  std::uint32_t begin{}, end{}, unwind{};
};
// Readers operate on owned fixtures or guarded RVAs of the admitted image.
using IdentityRead = bool (*)(void *, std::uint32_t, void *, unsigned);
using IdentityLookup = RuntimeRegion (*)(void *, std::uint32_t);
// C26 metadata/tag diagnostic only. Aliases of partial registers are described,
// never granted partial read/write semantics. Constants are instruction immediates.
struct ReadCmpContract {
  struct Operand {
    char kind[16] = "none", name[16] = "none", parentName[16] = "none",
        base[16] = "none", index[16] = "none", segment[16] = "none";
    unsigned width{}, access{}, parentTags{}, scale{};
    int parent = -1;
    std::int64_t immediate{}, displacement{};
    bool conservativeParent{}, valueTagsKnown{};
  };
  std::uint32_t site{}, flagsEntryGprMask{};
  unsigned operandCount{}, samples{}, flagsTags{};
  std::uint64_t decodedEflags{};
  bool decoded{}, formSupported{}, flagsOnly{}, hasMemory{}, modelApplied{};
  std::array<Operand, 2> operands{};
  const char *reason = "pinned_cmp_not_decoded";
};
inline constexpr std::uint32_t readIdentityBranch = 0x2cf7fe7;
inline constexpr unsigned readLeafBranchCap=1, readLeafPathCap=2, readLeafVisitCap=64;
inline constexpr std::uint32_t readImageSource=204285312, readImageBaseOrigin=readImageSource-0x100;
struct ReadImagePage {
  bool sameAllocation{},rangeInImage{};
  std::uint32_t begin{},state{},type{},protect{},allocationProtect{};
  std::uint64_t bytes{};
};
using IdentityPageQuery=bool (*)(void *,std::uint32_t,ReadImagePage &);
struct ReadImageLoad {
  bool attempted{},metadataValid{},sectionUnique{},sectionReadable{},sectionWritable{},sectionExecutable{},
      loaderRangesChecked{},loaderOverlap{},pageQueried{},policyPassed{},firstRead{},finalRead{},
      pageRechecked{},metadataSame{},pageSame{},stable{},targetCodeRechecked{},admitted{};
  std::uint32_t source{},width=8,sectionBegin{},sectionEnd{},sectionFlags{},loaderDirectory{};
  unsigned sectionOverlaps{},valueReadAttempts{},metadataBytes{};
  char sectionName[9]="none";
  ReadImagePage page{},finalPage{};
  const char *reason="image_load_not_attempted";
  // Private bounded comparison buffers: never logged, interpreted or dereferenced.
  std::array<unsigned char,4168> metadataPrivate{};
  std::array<unsigned char,8> valuePrivate{};
};
ReadImageLoad beginReadImageLoad(bool buildAndAddressVerified,std::uint32_t source,
    std::uint32_t imageSize,IdentityRead,IdentityPageQuery,void *) noexcept;
bool finishReadImageLoad(ReadImageLoad &,std::uint32_t imageSize,
    IdentityRead,IdentityPageQuery,void *) noexcept;
// Separate from entry-carrier masks: top of the three-category may-tag lattice.
inline constexpr unsigned readMayTagTop=1u|2u|4u;
struct ReadUnknownLoad {
  bool enabled{},attempted{},metadataPassed{},metadataRechecked{},metadataSame{},
      pageSame{},targetCodeRechecked{},admitted{};
  const char *reason="pinned_unknown_load_not_attempted";
};
struct ReadLeafBoundary {
  bool tagsKnown{};
  unsigned path{};
  std::uint32_t edgeFrom{},edgeTarget{};
  unsigned flagsTags{};
  std::array<unsigned,16> gpr{},xmm{},xmmHigh{};
  std::array<bool,16> above256Zero{};
  bool vectorTracked{};
};
bool beginPinnedUnknownLoad(ReadUnknownLoad &,ReadImageLoad &,std::uint32_t,
    IdentityRead,IdentityPageQuery,void *) noexcept;
bool finishPinnedUnknownLoad(ReadUnknownLoad &,ReadImageLoad &,std::uint32_t,
    IdentityRead,IdentityPageQuery,void *) noexcept;
struct ReadEntryTags {
  bool imageLoadAddressVerified{},vectorSourceVerified{};
  std::uint32_t imageLoadSource{};
  unsigned samples{};
  std::array<unsigned,16> gpr{}, xmm{};
  std::array<unsigned,16> xmmHigh=ReadVectorTags{}.high;
  std::array<bool,16> above256Zero{};
};
struct ReadLeafOutcome {
  std::uint32_t ret{};
  unsigned flagsTags{}, clobberGpr{}, clobberXmm{};
  std::array<unsigned,3> returnedGpr{}, returnedXmm{},returnedXmmHigh{};
  std::array<bool,16> above256Zero{};
};
struct ReadLeafBranch {
  std::uint32_t site{}, flagsProducer{}, entryCarrierMask{};
  unsigned samples{}, flagsTags{}, instructionBytes{}, displacementBytes{};
  std::int64_t displacement{};
  std::uint32_t taken{}, fallthrough{};
  bool decoded{}, conditional{}, flagsDependencyKnown{}, edgesExact{},
      takenInCode{}, fallthroughInCode{}, takenInWindow{}, fallthroughInWindow{},
      takenForward{}, fallthroughForward{}, takenRuntimeCross{}, fallthroughRuntimeCross{},
      expanded{};
  const char *encoding = "unsupported";
  char mnemonic[32] = "none";
  const char *condition = "unknown", *flagsRead = "unknown";
};
inline constexpr std::uint32_t readIdentityMov = 0x2cf7fe9;
enum MovAlias : unsigned { movHero=1, movField10c=2, movSource=4,
    movCallerHome=8, movPrivateStack=16, movImage=32, movUnknown=64,
    movAnyAlias=127 };
// Diagnostic only: no loaded value or load semantics are supplied by this record.
struct ReadMovContract {
  struct Operand {
    char kind[20]="none",name[16]="none",parentName[16]="none",
        base[16]="none",index[16]="none",segment[16]="none";
    unsigned width{},access{},scale{},registerTags{},baseTags{},indexTags{};
    int parentGpr=-1,parentXmm=-1,baseParent=-1,indexParent=-1;
    int originKind{},originRoot=-1,baseKind{},baseRoot=-1,indexKind{},indexRoot=-1;
    std::int64_t displacement{},immediate{},originOffset{},baseOffset{},indexOffset{};
    bool conservativeParent{};
  };
  std::uint32_t site{};
  unsigned instructionBytes{},operandCount{},addressWidth{},memoryOperands{},samples{},
      fullGprWrites{},partialGprWrites{},xmmWrites{},otherRegisterWrites{},aliasMayMask=movAnyAlias,
      addressTags{},loadedMayTags{};
  int addressRoot=-1;
  std::int64_t addressOffset{};
  std::uint64_t decodedEflags{};
  bool decoded{},isMov{},effectsKnown{},flagsPreserved{},memoryAccessKnown{},
      memoryRead{},memoryWrite{},memoryRmw{},pureLoad{},stackPointerWrite{},
      parentState{},addressExact{},addressSame=true,imageRangeKnown{},codeRangeKnown{};
  std::array<Operand,2> operands{};
  char mnemonic[32]="none";
  const char *addressClass="not_classified",*reason="pinned_MOV_not_decoded";
};
inline constexpr std::uint32_t readIdentityVector=0x2cf7ff0;
struct ReadVectorContract {
  struct Operand {
    ReadMovContract::Operand detail{};
    int vectorParent=-1;
    unsigned vectorBits{},decodedAccess{},resolvedAccess{},broadcast{};
    bool zeroMask{};
  };
  ReadMovContract detail{};
  std::array<Operand,8> operands{};
  unsigned vectorBits{},encodedBits{},vectorWrites{},mask{},replayVisits{},replayRefused{};
  int destinationParent=-1;
  bool modeled{},sourceVerified{},isVmovups{},encodingKnown{},tagsKnown{},directionKnown{},accessConflict{},
      masked{},zeroMask{},broadcast{},rounding{},upperZero{},upperPreserved{},
      destinationRepresented{},upperStateRepresented{};
  const char *encoding="unsupported",*upperEffect="unknown";
};
inline constexpr std::uint32_t readIdentitySignExtend=0x2cf7ff4;
struct ReadSignExtendContract {
  ReadMovContract detail{};
  bool isMovsxd{},encodingKnown{},registerForm{},full64{},tagsKnown{},modeled{};
  int destinationParent=-1,sourceParent=-1;
  unsigned sourceBits{},destinationBits{},sourceTags{},resultTags{};
  const char *encoding="unsupported";
};
inline constexpr std::uint32_t readIdentityShift=0x2cf7ff7;
// Arithmetic EFLAGS bit positions, not entry-carrier masks. Undefined flags
// inject intrinsic may-top into the aggregate; never treat them as tag0.
struct ReadShiftContract {
  ReadMovContract detail{};
  bool isShl{},encodingKnown{},countKnown{},registerForm{},full64{},zeroExtend32{},
      partial{},tagsKnown{},modeled{},operandUnchanged{},flagsDynamic{};
  int destinationParent=-1,countParent=-1;
  unsigned destinationBits{},encodedCountBits{},encodedCount{},countMask{},effectiveCount{},
      destinationTags{},countTags{},resultTags{},flagsBefore{},flagsAfter{},definedFlagTags{},
      flagsDefined{},flagsUndefined{},flagsPreserved{};
  const char *encoding="unsupported",*countSource="unknown",*countClass="unknown";
};
inline constexpr std::uint32_t readIdentityNextVector=0x2cf7ffb;
// Separate site/contract: the first VMOVUPS never licenses this instruction.
struct ReadNextVectorContract {
  ReadVectorContract vector{};
  unsigned encodingBytes{},opcode{},memoryWidth{},flagsBefore{},flagsAfter{},
      destinationLowBefore{},destinationHighBefore{},destinationLowAfter{},destinationHighAfter{};
  std::array<unsigned,8> low{},high{};
  std::array<bool,8> represented{},above256Zero{};
  bool compatibleLoad{},faultSuppressionPossible{},legacy{},stateApplied{};
};
struct ReadTargetIdentity {
  bool nextVectorEnabled{},shiftEnabled{},signExtendEnabled{},vector256Enabled{},enabled{}, admitted{}, missingRuntime{}, stable{}, tableValid{};
  bool directoryPairValid{}, directoryRangeInImage{}, directorySizeDivisible{},
      tableBudgetAdmitted{}, entryCapExceeded{}, byteCapExceeded{},
      validationSkippedByCap{}, tableReadFailed{}, tableReadComplete{}, lookupQueried{};
  std::uint32_t target{}, codeBegin{}, codeEnd{}, runtimeBegin{}, runtimeEnd{},
      nextRuntime{}, tableEntries{}, scanBytes{}, size{}, instructions{}, barrier{};
  std::uint32_t directoryRva{}, directoryBytes{}, metadataBytesRead{}, validatedEntries{};
  RuntimeRegion lookup{};
  ReadCmpContract cmp{};
  ReadLeafBranch branch{};
  ReadMovContract mov{};
  ReadVectorContract vector{};
  ReadSignExtendContract signExtend{};
  ReadShiftContract shift{};
  ReadNextVectorContract nextVector{};
  ReadImageLoad load{};
  ReadUnknownLoad unknownLoad{};
  ReadLeafBoundary boundary{};
  std::array<ReadLeafOutcome,readLeafPathCap> outcomes{};
  bool entryTagsKnown{}, cfgComplete{}, finalCodeRechecked{}, pathEffectsDiffer{};
  unsigned entryTagSamples{}, paths{}, visits{};
  const char *method = "none", *reason = "target_identity_not_enabled";
  char mnemonic[32] = "none";
  std::array<unsigned char, 128> snapshot{};
};
ReadTargetIdentity authenticateReadTarget(bool buildAndCallVerified,
    std::uint32_t target, std::uint32_t imageSize, IdentityRead,
    IdentityLookup, void *context, const ReadEntryTags *entryTags = nullptr,
    IdentityPageQuery pageQuery = nullptr, bool pinnedUnknownLoad = false, bool pinnedVector256 = false, bool pinnedSignExtend = false, bool pinnedShift = false, bool pinnedNextVector = false) noexcept;
}
