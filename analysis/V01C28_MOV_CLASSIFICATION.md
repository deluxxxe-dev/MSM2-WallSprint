# C28 exact pinned MOV operand/address diagnosis — Outcome C

## Accepted evidence / missing fact

VERIFIED-RUNTIME diagnostic under exact Spider-Man2.exe2.810.0.0
SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9:
c25 complete527161-entry/6325932-byte runtime table and actual queried no-entry
at0x2CF7FE0; c26 CMP r8d,0x3FFF flags-only/tag0; c27 exact JA0x2CF7FE7 shortrel8,
2bytes/displacement42, fallthrough0x2CF7FE9/taken0x2CF8013, both forward/in-window/
no runtime crossing, expanded with unknown numerical outcome. It immediately
refuses MOV0x2CF7FE9 memory operand:3 visits/3 unique instructions/0 returns,
no target identity/hash/effects. Deinit/retention clean, hard kills/Position/
source-position/corrections0. Evidence: V01C27_RUNTIME_RESULT.md. Actual MOV
operands/access/address/value provenance are absent; do not guess from mnemonic.

## Decision: classification, no new load semantics

The task permits an exact read-only load only with independently justified
source-region AND loaded-value provenance. Compact evidence cannot supply that
proof. C28 therefore adds the minimum operand/address diagnostics and continues
to refuse every memory effect. No immutable-image load rule, destination zero/
upper-lane model, partial read/write permission or generic memory emulator added.
A potentially sound rule for an unrelated fixture is not evidence about this
actual unknown MOV. Read-only-looking/image-global/code addresses alone cannot
supply value/lifetime/immutability proof. Outcome C until exact compact metadata.

## Bounded descriptor

Only exact reachable instruction0x2CF7FE9 in the c27 walker is described, from the
same stable guarded snapshot after c27 tags/branch guards. At most2 operands:
class (GPR/XMM/immediate/memory/unsupported), exact alias/parent and width/access;
base/index/scale/signed displacement/segment/address width; decoder full register
access and flag mask/preservation, full/partial GPR/XMM/other/SP writes; explicit
read/write/RMW direction. A pure load FORM is distinct from semantic admission.
Unexpected opcode/forms are classified but never gain memory permission; writes/
RMW immediately refuse even if numerically disjoint from Position.

No memory contents/physical register values are read or logged. Instruction
immediates/displacements and abstract root/offset provenance are metadata.
Decoded destination writes describe the instruction if executed, not MOD writes.
Partial-register parent tags are conservative metadata only; no partial or XMM
upper-lane/zero-extension semantics are added. Original register-only operations
at this site remain governed by the unchanged c27 rules.

## Address versus value / parent state refinement

Initial identity Slice roots are raw entry tokens. They cannot be named Hero or
a typed pointer from an index. Metadata first records that scope without claiming
an effective address. When the existing parent sidecar reaches this CALL, decode
only the saved pinned instruction again and classify using its owned abstract
Slice/ReadFlow state. Apply the existing natural CALL SP delta(-8), never read
stack memory. Samples are analyzed caller states, not actual CPU observations.

For known unindexed64-bit/segment-free affine origins, report conditional Hero
Transform/field+0x10C, writer source, caller/home stack, below-entry-SP unowned
candidate, or in-image/code address range. Below-SP is not proven allocation.
Writer source alias uses the conservative existing captured stack window
writer-entry RSP+[0x28,0x68), not a new write span. Other roots, loaded raw bits,
indexed/segmented/32-bit/overflow/range ambiguity retain unknown aliases. Exact
base/index/scale/displacement still describe the expression when the effective
address is unproven. RIP image offsets are anchored instruction RVAs, never an
assumption of immutable image globals. Code-range membership does not add a
read-value permission/snapshot/region proof.

Track incoming register/base/index tags1/2/4 separately. These are address/input
possible-dependence, not the loaded value's tags. All memory-read value tags stay
UNKNOWN with conservative may-tag upper bound7, never an untainted0 or a semantic
state update. Register destination tags describe its incoming parent state.
Across analyzed parent paths, union tags/possible aliases; conflicting addresses
lose exactness. First-state origin fields are explicitly scoped with samples and
same-address status. Samples0 means no actual parent refinement, not tag0 proof.

Alias bit masks: Hero1,field10C2,source4,caller/home8,private candidate16,image32,
unknown64; ambiguous addresses retain127. These conditional abstract classes
are neither physical read/write permissions nor semantic ABI declarations.

## Unchanged boundaries and tests

Only new stop reasons distinguish memory-write/RMW, pure-load value unproven and
unmodeled memory forms/access. The c27 1-JA/2-path/128byte/32unique-instruction/
64visit contract, metadata budgets, double/final snapshots/runtime coverage and
per-state conditions remain. A failed MOV prevents every span/hash/effect and
blocks whole target; the other safe leg never authenticates it independently.
No new proof pass, reader, pointer chase, syscall, hook or producer traversal.

Eight new owned groups (36 identity total) retain all c19-c27/21 call-read groups:
- exact MOV direction/classes/access/width/full writes/preserved flags;
- actual parent addresses vs symbolic tokens/loaded raw bits and1/2/4 tags;
- indexed/segmented/address32 expression metadata without guessed addresses;
- Hero/field/source/home/below-SP aliases, correct CALL delta and conflicting paths;
- stores/RMW/immediates refuse;
- partial/full GPR/XMM metadata with no new load/destination semantics;
- image/code addresses remain refused and no value observed;
- stable/reachable/pin/tag gates, other-site memory and no one-leg identity.
No actual MOV bytes are reconstructed. Results/counts: CODEX_RESULT/RELEASE.

Actual compact MOV/operand/address records are genuinely needed; one A/E ~3s
capture per unchanged5s limit suffices, with existing safety/next barrier. Do not
request a body/data dump or habitual repeat once accepted evidence is sufficient.
All writable/caller-owned/exclusive/lifetime/downstream-conflict gates0; Position/
source-position/input/movement0. Producer0x351EB00 deferred; no gameplay/camera/
adhesion/animation/physics/SF/ordinary-mod changes. Prior workspace/Overstrike
updater/WINMM recovery helpers remain byte-identical.
