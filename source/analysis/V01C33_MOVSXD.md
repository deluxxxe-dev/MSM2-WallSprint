# c33 — pinned MOVSXD classification and conditional register sign extension

## Accepted input / distinction

Basef609ee0, codex/v01c20-edi-value-provenance, no main merge.
VERIFIED-RUNTIME V01C32_RUNTIME_RESULT.md: exact VEX2.256 YMM0 m256[RDX]/Hero
load now modeled with low/high128 top7 and above256 zero, no data reads.
C30 RAX unknown/top7 retained. New barrier MOVSXD0x2CF7FF4/path0 from JA
0x2CF7FE7 ->0x2CF7FE9,5instructions/5visits/0returns/identity/effects.
Actual MOVSXD operands/source/destination/width/memory use UNKNOWN.

Do not guess actual operands from mnemonic or fixture. Classify first in the
authenticated current snapshot. A narrow register32-to-full64 form can be
conditionally modeled from exact current decoder/encoding evidence without
knowing numerical values. Memory forms remain diagnostic-only until a real
exact-source contract exists; no memory rule is invented from owned fixtures.

## Exact eligibility and abstract effect

Only reachable MOVSXD0x2CF7FF4, length3, REX.W+opcode63/ModRM register form,
no prefixes/memory, source GPR32 and destination GPR64, decoded READ/WRITE,
complete sole full destination write, no partial/vector/flags/SP/other effects.
RSP destination refuses. Verify before changing state, under the same current
code/branch/cap/runtime guards; production explicitly enables this pinned rule.
Wrong site/source width/MOVSX/MOVZX/partial/memory/stack forms remain refused.

Sign extension copies source31:0 and replicates source bit31 into dest63:32.
Dependency mapping therefore conservatively copies the whole source parent's
may-tag set (including any over-approximation from untracked bit granularity).
Read source tags BEFORE full destination replacement. Never OR unrelated old
destination tags into the new result. If source and destination share a parent,
retaining that source-parent upper bound is still sound. Do not infer numeric
sign, constantness, zero/sign bit, pointer identity or semantic ABI/type.
Destination Slice origin becomes opaque unknown/non-pointer, not the original
pointer or stale destination origin. No raw source-value read/log/interpretation.

Only the destination changes. Other GPRs, flags/SP/memory, vector low/high128
and above256 knowledge are preserved. C30 RAX7 is unchanged unless that exact
MOVSXD itself architecturally replaces RAX; that deliberate full write then
uses only proven source tags, not a hidden narrowing of unrelated state.

## Memory and metadata

MOVSXD source memory gets exact source/destination/access/width/full-write
metadata, memory direction, base/index/scale/displacement/segment/address width,
current-parent conditional Hero/field10C/source/home/below-SP/image/unknown
aliases and source/address tags. Any memory source refuses because the actual
form/source is still absent from accepted evidence; image address/tag0 does not
prove loaded tags0. Unknown loaded may7 is metadata, not an executed abstract
load rule. Stores/RMW/unresolved/loaded-pointer/stack sources get refusals, no
pointer chase or second dereference. Below-SP is not private allocation.

Reuse existing bounded actual-parent replay to refine diagnostics, not a new
traversal: original4096 cumulative replay visits and64visit/128byte/32unique/
1JA/2path limits. Aggregate source/address tags/aliases, lose exactness on
conflicts. MOVSXD/OPERAND/ADDRESS records distinguish semantic form, modeling,
source-parent tags vs loaded unknown values and current instruction stability.

## Continuation / safety

Only admitted register form advances to the next exact unsupported effect.
No next new instruction or memory form is opportunistically modeled. Both JA
legs/all normal modeled RETs/final code/runtime/source checks are required for
target span/hash. Return tags union conservatively, vector halves/zero knowledge
remain correct. One safe leg never certifies a refused other leg.

C25 full527161 table, c29 readonly refusal, c30 qword/source proof and c31/c32
vector width/access/source/state policy remain unchanged. No generic integer/
memory/vector emulator, guessed ABI, new hook/game call/data-page mutation,
raw values or complete private body. Position/source-position/input/movement/
adhesion/camera/animation/physics writes0; ownership/lifetime/exclusive/
writable-source gates0; producer0x351EB00 deferred. Existing native ABI/
lifecycle/assertions/deadlines and workspace/launchers unchanged.

## Tests and real evidence

Owned8 new groups, identity76/read-call21 plus full chain. Register64-from32
metadata/tag propagation, all non-SP destination banks, stale-tag replacement/
same-parent source, memory address/alias refusal, wrong sites/widths/prefixes/
MOVSX/MOVZX/stores/RMW, current-parent replay, vector preservation, next barrier/
unsafe leg/code/table/returned unions. No numerical source bits needed.
Full validation/reproduction/hashes in CODEX_RESULT and V01C33_RELEASE.json.

Actual form and next opcode are missing from compact c32 evidence; cloud has no
current private target bytes. Minimum necessary A~3s/end/move/E~3s capture:
MOVSXD/OPERAND/ADDRESS plus existing next barrier/path/low-high vector/GPR/flags/
CFG/current identity/safety/cleanup. No raw memory/vector/body dump requested.
Owned conditional register cases are not actual game form or whole-writer
XYZ/W/feedback/ownership/lifetime/exclusivity/writability proof.
