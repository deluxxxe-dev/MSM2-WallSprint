> Accepted c26 actual outcome: CMP r8d,0x3FFF flags-only/tag0 crossed; JA0x2CF7FE7 condition tag0 but numerical outcome unknown is the next barrier. See V01C26_RUNTIME_RESULT.md and successor V01C27_JA_FANOUT.md. Pending-form/linear-only statements below are preparation history.

# C26 exact pinned CMP / strict linear next boundary

## Accepted evidence and scope

VERIFIED-RUNTIME supplied compact c25 evidence (analysis/V01C25_RUNTIME_RESULT.md):
exact Spider-Man2.exe2.810.0.0 SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9;
directory0xC566000/6325932bytes/527161 entries fully read and structurally valid,
actual OS lookup queried and no containing entry for0x2CF7FE0,
next runtime0x2CF8020; stable64-byte snapshot stops on first CMP.
No CMP operands/width/immediate/branch are in the compact evidence. Do not
reconstruct private bytes or infer a semantic ABI from mnemonic/register indices.
C25 A/E ~3.86/~4.03s had clean native Deinit/retention and hard kills/Position0.

## Conditional implementation, not an actual-form claim

VERIFIED-STATIC/owned-fixture: describe only the instruction at pinned0x2CF7FE0
before deciding effects. Capture at most two explicit operand descriptors:
class, byte width, exact alias and parent name/index, decoded access, instruction
immediate, memory address metadata (never a memory value), decoded EFLAGS mask.
Compare decoder complete register reads/writes against all explicit operands:
only EFLAGS may be written, every explicit register read must be present and
no hidden input allowed. Require exactly MODIFY_CF/PF/AF/ZF/SF/OF, no prefixes,
first operand full32/64-bit GPR, second same-width GPR or immediate. This is a
runtime-checked candidate form, not a guess about the actual missing instruction.

The narrow new register/immediate semantics are local to this exact entry and
its authenticated identity sidecar. No general Slice/emulator/parent CFG model
or arbitrary CMP site gains immediate/partial/memory permission. Existing
full-register register/register comparisons elsewhere retain their old model.
No partial semantics are added: low/high8 and16 aliases are described only with
conservative parent-tag metadata, then refused. Memory value tags remain UNKNOWN.
Unsupported form/width/prefix/effect stops at the exact CMP RVA; no target identity.

For admitted CMP, CF/PF/AF/ZF/SF/OF possible-dependence tags are the union of read
GPR tags; immediate constants contribute0. Exact same register aliases subtract
themselves and produce constant flags; operands/tags are preserved. No old flags
are consumed and no GPR/XMM/stack/memory is modified. This is possible dependence,
not a value computation, branch-direction decision, ABI type or effect certificate.

## Identity versus prefix evidence

C24/c25 metadata/code-region/lookup/double-read gates run first and are retained.
Only <=128bytes/<=32instructions to plain C3 RET, with a modeled linear prefix,
no alternate exit/branch/memory/stack/partial/nested call and no runtime-region
crossing, admits begin/end/size; runtime coverage is rechecked before admission.
Only this complete identity can be hashed and supplied to c23 call effects.

A refusal can still expose metadata for the stable bounded prefix. Symbolic
entry-carrier masks are propagated through only the old admitted register leaf
operations and the new pinned CMP. GPR/XMM copies and flags overwrites retain
provenance. At the already analyzed parent CALL, actual tag1/2/4 unions are mapped
onto those masks without replaying/calling the game or accepting a target span.
Partial operands record only conservative parent tags; memory values stay unknown.
No analyzed path means samples0/tags unknown, never an observed untainted result.

A next Jcc logs exact RVA/mnemonic/condition/flags read and producer, along with
flag dependency only when its entire admitted prefix makes that mapping known.
Incoming flags without a modeled producer remain unknown. Every branch refuses,
even tag0 or CMP-self constant flags. Neither possible branch outcome is excluded
or explored. Other unmodeled consumers stop at their exact effect boundary.
No branch/recursive expansion/general function finder is added.

## Tests and remaining evidence

Six new owned groups added to identity suite (21 total, retaining15 c24/c25),
plus existing21 read/call groups and c19-c22/full native pipeline:
- full32/64-bit register/immediate, signed constant, stable RET and operand preservation;
- selected1/postwrite2/other-prewrite4/disjoint/full-pair and same-alias flag unions;
- low/high8/16 alias metadata refusal and memory unknown/refusal;
- JE/JNE/JB/JG exact next-boundary/condition/selected/disjoint dependence with no hash;
- overwritten flags, GPR/XMM copies, incoming flags unknown, unsupported flag consumer;
- stable/pin/runtime crossing/complete527161-entry table and late-invalid-tail guards.
Fixtures are independently authored nonexecuted CPU bytes, never reconstructed game code.

Outcome C until actual CMP metadata exists. Genuine minimum missing evidence:
MOVEMENT_CALLEE_READ_CMP + READ_CMP_OPERAND and next READ_LEAF_BRANCH/TARGET_IDENTITY/
READ_CALL from exact admitted current build. If they show a safe form, conditional
model advances only to its next boundary; if memory/partial/unknown, it refuses.
No cloud claim of actual CMP support, target RET/hash/effect or downstream proof.
One short A/E ~3s each under unchanged5s hard limit is justified because compact
c25 lacks the operands; don't ask for body dumps, static samples or repeated tests
once sufficient evidence is accepted.

## Unchanged authority

writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement0.
Producer0x351EB00 deferred; no gameplay/adhesion/camera/animation/physics work,
new hooks, live value observation, PAGE_GUARD, guessed ABI or SF/ordinary mod changes.
Complete local return flow is separate from whole writer XYZ/W, cross-invocation,
caller/storage/lifetime ownership and writability. No effect certificates issued.
