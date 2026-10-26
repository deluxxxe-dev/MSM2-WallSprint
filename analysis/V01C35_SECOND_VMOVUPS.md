# c35 — independently pinned second VMOVUPS classification

## Evidence, not mnemonic inference

Base96cabbc, codex/v01c20-edi-value-provenance; no WS main merge.
VERIFIED-RUNTIME V01C34_RUNTIME_RESULT.md: SHL0x2CF7FF7 is R8,imm8=6, masked6,
64bit, result tags0, flags may-top7 from undefined AF/OF. Remote keyboard/mouse
capture: movement observations are that input condition, not controller equivalence.
Next VMOVUPS0x2CF7FFB/path0, edge0x2CF7FE7 ->0x2CF7FE9,7instructions/7visits/
0returns/identity/effects. Incoming RAX7/R8zero/YMM0 low-high7/above256zero/flags7.
Actual second encoding, vector width, operands/direction/address remain UNKNOWN.
No authenticated private current-code snapshot exists in cloud. Runtime decodes
its bounded authenticated snapshot; no game body dump or numerical values needed.
Owned full-load and store fixtures are not actual second-site game bytes.

## VERIFIED-STATIC owned conditional policy

Separate pin0x2CF7FFB, separate ReadNextVectorContract and VECTOR2 records. Reuse
c31 descriptor without altering its original site rule; retain decoded vs encoding-
resolved access, including Capstone5.0.6 opcode11 store-as-READ caveat. A store
must refuse even if decoder metadata says READ. Exact memory width, VEX2/VEX3/
EVEX/legacy distinction, source/destination parents, address components and current
symbolic aliases are independent facts for this site. Non-vector instructions
retain original handling/caps; do not intercept an unrelated NOP at this address.

Only conditional canonical4byte VEX2 opcode10 full XMM/YMM <- [RDX+0], no index/
displacement/segment/legacy prefix, address64, scale1, exact decoded/resolved
access, pure load, complete sole vector-parent write, flags/GPR/SP/other-memory
preserved can reuse c32 state. Destination0..15, width16 or32bytes. VEX3, legacy
SSE, register-register, memory store/RMW, EVEX/masks/merge/zero-mask/broadcast/
rounding/fault suppression, wider/partial/unsupported state and every other
address/form remain refused. This is not generic AVX or a new arbitrary-memory
load model. The accepted earlier Hero source range covers this same16/32bytes.
No other Hero offset/stack/image/mutable-global address is authorized by alias.

## Current source, not stale entry permission

Require c30 current source/code metadata and c32 vector mode. Verify CURRENT
RDX pointer origin root/offset0/tags0 and supplied current Hero evidence in the
symbolic proof; actual-parent replay independently requires Hero root1/offset0.
The symbolic raw entry-root2 is a callee-entry token mapped only through that
current parent proof, never a semantic ABI type. If MOVSXD/SHL replaced RDX with
opaque bits, tags0/old entry-Hero fact cannot revive its pointer origin. No qword
loaded bits become pointers; no second dereference/numerical narrowing.

## Data and upper-state effect

Represented16 parents, low128/high128 may tags and above256-known-zero only.
- VEX128: destination low <- unknown top7; high128 <-0 (architectural zero),
  above256 known zero. Old high tags are deliberately killed only on this written
  destination; no stale merge. XMM aliases destination low.
- VEX256: destination low/high <-unknown top7; above256 known zero.
- Every other parent, GPR (including RAX7/R8zero), SP/memory and flags are preserved.
  SHL flags7 never clear in a vector move.
No vector/memory values read, observed taint, constant numerical result or guessed
type. Above256 zero knowledge never authorizes a ZMM/EVEX read/instruction. Store
forms preserve vector architecture but are not admitted effects on memory.

## Diagnostics and scope

VECTOR2: exact length/encoding/opcode/width/direction/access and complete other
writes, source proof/snapshot status/model decision. OPERAND: register/class/
parent/width, decoder vs resolved access, low/high/above256 source tags (YMM
source uses both halves, not XMM-only tags), address base/index/scale/signed
displacement/segment/address width and symbolic origin roots/offsets/tags.
ADDRESS: current-parent samples, exactness/conflict, conditional Hero/field10C/
writer-source/home/below-SP/image/unknown alias may-set; loaded may-top versus
address tags are distinct. ARCH: upper zero/preserve/mask/merge/broadcast/round/
fault-suppression metadata, destination before/after low/high and flags unchanged,
state-applied separately from representability. Wider/unrepresented source tags
are unknown/top, never assumed low128-only. Decoded potential memory writes are
not executed diagnostic writes. Below-SP is not proven private allocation.

Reuse only existing actual-parent replay and4096 cumulative /64 per-walk visit
limits. Union tags/aliases, intersect above-zero knowledge/source-proof/applied
status across samples, lose exactness on conflicts. No extra traversal, pointer
chase, game call or hook. Exact next unsupported instruction/path/edge/GPR/vector/
flags/CFG record remains. Both JA legs/all RETs/current code/PE/runtime/128byte/
32unique/64visit/1branch/2path gates unchanged. No opportunistic next opcode rule.
C19-c34 and all ownership/lifetime/exclusive/writable-source gates remain; Position/
source/input/movement/adhesion/camera/animation/physics writes0. Producer0x351EB00
deferred; shared launch/updater/SF/ordinary mods unaffected.

## Tests / missing evidence

9 new groups, identity96/read-call21 plus full build/regression and independent
extracted source build/focused/package verification. Covers exact metadata,
16destinations/128vs256/high zero/full replacement, store decoder caveat and
source low/high tags, current-parent aliases/unknown index/opaque RAX, stale
entry Hero after MOVSXD destination replacement, wrong address/width/site/form/
EVEX/masked/wider/RMW refusal, SHL flags7, both-path returns/current code/full
527161 table/next barrier. Original instruction-cap/native assertions retained.
Detailed results/hashes/reproduction in CODEX_RESULT / V01C35_RELEASE.json.

Actual new VMOVUPS operands/direction/address and next boundary remain UNKNOWN.
One shortest A~3s/end/move/E~3s capture of VECTOR2/OPERAND/ADDRESS/ARCH plus
existing boundary/path/GPR/vector low-high/flags/CFG/current identity/safety/
cleanup is needed, with input method noted. No raw values/private body requested.
A store or unresolved address will correctly remain refused, even if low/high
state is represented and numerical address tags happen to be0.
