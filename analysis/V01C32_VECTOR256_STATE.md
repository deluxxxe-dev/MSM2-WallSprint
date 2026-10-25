# c32 — bounded256 dependency state for one exact YMM load

## Accepted input / scope

Baseb12d7fe, codex/v01c20-edi-value-provenance, no main merge.
VERIFIED-RUNTIME V01C31_RUNTIME_RESULT.md fixes VMOVUPS0x2CF7FF0:4byte VEX2
VL256, YMM0 <- m256[RDX], no index/segment/displacement, current RDX Hero root1
offset0/tag0 and Hero-only alias. No memory write/RMW, GPR/SP/flags changes.
C31 correctly refused unrepresented upper128. C30 RAX unknown/top7/no qword
reads and c25 full527161 table/c26 CMP/c27 both JA paths remain accepted.

## Representation and narrow rule

Owned vector_dependency.hpp adds16 low128 XMM aliases,16 high128 category-tag
banks and per-parent above256-known-zero markers. Upper initial state is top7,
never silently clean. Only dependency categories are stored, no vector bits/
lane values/type/meaning. XMM reads low only; YMM dependency reads union halves.
Legacy XMM writes preserve upper and above-state knowledge. Unmodeled VEX128
is refused rather than falling through legacy semantics. Generic YMM writes,
masks/merge and all ZMM/EVEX/>256 uses still refuse. A zero-above marker does
not grant permission to consume arbitrary ZMM state.

Production explicitly selects pinnedVector256. Only the exact4byte VEX2.256
VMOVUPS YMM0,[RDX] at0x2CF7FF0 may write the abstraction. Decode access/width/
operands/prefix/modifiers plus current same Hero root1/offset0/address tags0
are rechecked. Supplied identity evidence comes from actual same-origin parent
samples, not an ABI guess; actual c23 walk repeats the source check per state.
No source-value reader is invoked. Both halves replace prior state with top7,
low Slice origins become opaque unknown, above256 marker becomes known zero.
Other vector parents, GPR (including RAX7), flags/SP/memory remain unchanged.

Top7 is may-depend on any tracked1/2/4 category, not observed taint or selected
dependence kill, and no tag0/constant/pointer/matrix interpretation. Identity
carrier masks still convert once through current entry tags before intrinsic
top injection. Upper category tags are separate from carrier-mask domain.

## Flow and all-path proof

ReadEntryTags/ReadCallContract carry current source/high-state evidence; low and
high return masks remain separate. Clone both halves per path, union possible
category tags at merges and intersect above256 zero knowledge. Different return
effects remain explicit. Legacy XMM zero may clear low tags but must retain
possible selected high tags. Parent read-dependency diagnostics retain selected
high results after new vector tracking becomes active. No scalar-only historical
proof is strengthened by unknown initial upper state.

C31 classification remains available in diagnostic-only mode; c32 models no
other new instruction. Continue after this load only until the next unsupported
effect. Report pre-instruction GPR/low128/high128/flags, path/edge/RVA/mnemonic/
reason. All target span/hash/effects require both modeled normal RETs, final
current code/runtime/source checks and unchanged1JA/2paths/128bytes/32unique/
64visits; parent visits4096, no deeper CALL or source discovery. Code or runtime
change invalidates current admission. One safe other leg never grants identity.

## Safety / validation / needed live evidence

No new hook/game call/data-page mutation, no vector/qword value read/log/numeric
interpretation/pointer chase, no generic AVX/AVX512 engine or guessed ABI. All
Position/source-position/input/movement/adhesion/camera/animation/physics writes
remain0; ownership/lifetime/exclusive/writable-source gates0, producer0x351EB00
deferred. Existing orientation override/native lifecycle/ABI/assertions unchanged.

Owned8 new groups (identity68, retained read/call21): full256 replacement/aliases,
legacy vs VEX refusal, high preservation/parents, exact current source/encoding/
width/register pins, no data reads, above256 marker with ZMM refusal, next
barrier/unsafe leg/current code/table, conservative return halves/zero knowledge.
Full build/test/reproduction counts and hashes in CODEX_RESULT/V01C32_RELEASE.json.

Actual next target instruction is absent from compact c31 evidence and cloud
has no live process/current private body. Minimum genuinely required capture:
VECTOR256/STATE/RETURN plus existing next barrier/path/tag/CFG/current identity/
safety/cleanup, A~3s/end/move/E~3s. No raw source/vector/code dump requested.
Owned complete-target fixtures are not actual full writer XYZ/W/feedback/
storage/ownership/lifetime/exclusivity/writability proof.
