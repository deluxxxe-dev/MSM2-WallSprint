# c31 — exact VMOVUPS metadata; vector semantics remain refused

## Accepted input / decision

Baseb78cd42, codex/v01c20-edi-value-provenance, no main merge.
VERIFIED-RUNTIME c30 (V01C30_RUNTIME_RESULT.md): exact MOV0x2CF7FE9/source
0xC2D2580 abstract effect admitted, RAX unknown origin/top7, no qword reads,
no observed taint/selected dependence kill. New barrier0x2CF7FF0 VMOVUPS,
fallthrough path0 from JA0x2CF7FE7 ->0x2CF7FE9; pre-state RAX7/other GPR/XMM0/
flags0. Target incomplete4instructions/4visits/0returns/identity/hash/effects.

Actual VMOVUPS operands/width/direction/source/upper-lane behavior UNKNOWN.
No private target bytes are available in the repository. Do not guess a vector
form from a mnemonic or an owned fixture. Outcome C: compact classification
first, with no new vector-memory semantics or generic whole-vector state.

## Diagnostic implementation

Classify only reachable pinned VMOVUPS0x2CF7FF0 from the existing authenticated
current bounded snapshot, before the generic memory refusal. Reuse the existing
descriptor for scalar/address facts; add up to8 decoded operands, explicit
XMM/YMM/ZMM parent indices/widths, decoder access and encoding-resolved access,
VEX2/VEX3/EVEX family/VL and mask/zero/merge/broadcast/rounding metadata.
Exact memory base/index/scale/signed displacement/segment/address width and
conditional Hero/field10C/source/home/below-SP/image/unknown aliases are separate
from loaded-value provenance. Unknown loaded RAX remains non-pointer/top7.

For current parent provenance, replay only the existing bounded walk on saved
bytes with owned caller Slice/ReadFlow and natural CALL SP delta(-8), no live
stack/data/vector read. Existing actual branch/source gates are repeated, c30
source metadata proof required, vector always stops. Cumulative replay visits
cap4096; each walk keeps original64visit/128byte/32unique/1JA/2path limits.
Union address/tag/alias samples, lose exactness on conflicting or failed parent
replay; below-SP remains unowned candidate, not private allocation. Tags apply
only to GPR/address and represented low128 XMM bank; YMM/ZMM upper tags UNKNOWN.

Reports VECTOR / VECTOR_OPERAND / VECTOR_ADDRESS / VECTOR_ARCH alongside
unchanged c30 UNKNOWN_LOAD, pre-stop path/edge/tag state and native safety.
Only abstract metadata/instruction constants, no raw bytes/vector/memory values.

## Decoder access evidence: don't admit an apparent read without verification

VERIFIED-STATIC/independently authored fixtures: Capstone5.0.6 can label VEX
VMOVUPS opcode11 memory destinations CS_AC_READ and report no register writes.
That instruction is still a store. Retain decoded access and separately resolve
direction only when VMOVUPS id, VEX/EVEX map/pp/VL/opcode and decoded operand
layout agree: opcode10 load, opcode11 store. Memory writes/RMW refuse immediately,
including stores whose decoder reports READ. Masked vector destination merge
(read+write) is not a memory RMW. This does not authorize any write or correct
other decoders generically. Unknown/inconsistent encodings remain refused.

## Why no load admission

Current Slice holds four32bit lanes per XMM and ReadFlow one tag bank per XMM
(16banks). No YMM/ZMM upper-half state exists. VEX128 overwrites XMM and clears
upper vector state above encoded VL; VEX256 writes beyond represented128bits.
EVEX can add higher registers/masks/merge/zero/broadcast/fault suppression.
Decoder XMM writes alone do not prove the complete architectural destination
effect is represented. Report known encoded upper clearing separately from
upper_state_represented=0. No new whole-register abstraction or invisible upper
dependence assumption. All these forms refuse; no full-XMM unknown-top load test
is applicable because no complete eligible actual architectural form is proven.

Pure read metadata carries unknown loaded may bound7, not tag0 or actual taint,
and does not update any vector register. Memory store/RMW reason is immediate;
unmasked pure VEX load stops with upper_vector_state_not_represented; EVEX/mask/
merge/modifiers or unproven forms get distinct refusal. Next instruction is not
looked ahead or newly supported. One safe taken JA leg cannot grant identity.

## Unchanged contracts / safety

C30 exact qword top7/source metadata/domain conversion and zero value reads
unchanged. C29 writable .data tag0 refusal and c25 full527161 runtime table
unchanged. Both JA paths/full modeled RET/code/runtime gates/caps remain;
no identity/hash/effect or ownership/write certificate issued. All preexisting
scalar/CMP/JA/MOV/readonly policies, native ABI/lifecycle/assertions/deadlines and
workspace/shared launchers preserved. No source/Position/input/movement/adhesion/
camera/animation/physics writes, PAGE_GUARD/data-page mutation, new hooks/game
calls, guessed ABI, producer0x351EB00 traversal, private bodies/log publication.

## Tests and minimum needed real evidence

8 owned groups, identity60/read21 plus full chain. Cover encoding/width/access
and decoder-store discrepancy, actual-parent aliases/tags/conflicts, unsupported
upper/masked/EVEX state, exact pin/snapshot/unsafe-leg refusal, no value reads,
unchanged c30 top and full527161 table. Details final CODEX_RESULT/release JSON.

Actual vector metadata is absent from accepted compact evidence and no current
game snapshot can be read in cloud. Minimum genuinely needed new A~3s/end/move/
E~3s capture: VECTOR/OPERAND/ADDRESS/ARCH plus existing UNKNOWN_LOAD/barrier/CFG/
path/tag/safety/cleanup records. Neither raw vector/qword nor full body requested.
Fixture forms are not claimed as the actual game instruction.
