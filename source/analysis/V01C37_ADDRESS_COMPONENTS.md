# c37 — independent offset-range and exact-slot publication tracks

Base87b378f, codex/v01c20-edi-value-provenance; no main merge.
Accepted V01C36_RUNTIME_RESULT.md proves c36 store escape/control hard barrier.
Current manual/rules/workspace remain unchanged; no identical movement recapture.

## VERIFIED-STATIC path proof from accepted authenticated sequence

CMP is32bit unsigned R8D vs decoded immediate. JA is CF=0 AND ZF=0 (strict unsigned
above); its fallthrough is R8D<=immediate, including equality. Both outcomes are
mandatory; no path is inferred from frequency or tags. Implementation derives
bounds from the decoded immediate, never assumes an expected offset maximum.
For the accepted immediate it proves fall R8D=[0,16383], taken=[16384,UINT32_MAX].
The taken predicate does not gain the fallthrough's nonnegative sign proof.

Pinned MOVSXD R8,R8D on fallthrough: entire input range <=INT32_MAX, so sign
extension is exact nonnegative64bit, range unchanged. Pinned64bit SHL effective6:
first require max<=UINT64_MAX>>count, derive stride1<<count and shifted bounds.
Offsets=[0,1048512] inclusive (0..0xFFFC0), step64. Adding32byte width is separately
checked against overflow: maximum store end exclusive1048544=0xFFFE0, inclusive
last byte0xFFFDF. This is relative OFFSET alignment/extent only, not absolute
base alignment, allocation size, lifetime or base+offset nonwrap.

Minimal range representation is per cloned path, R8 only, tied to exact existing
CMP/JA/MOVSXD/SHL sites/effects. Any other accepted R8 write invalidates the fact;
wrong source/destination/count or overflow cannot reuse stale stage proofs.
Proof bits reset before each stage. No generic arithmetic/dataflow engine. Current
code equality and parent replay guard the completed offset fact. No GPR/may-tag/
vector/flag state changes: RAX7/R8zero/YMM halves7/above256zero/flags7 and c36
escape1/pending1/contained0/control-blocker/CFG0 remain. Both paths still required.

## RAX publication track — precise refusal and available tooling

Exact slot0xC2D2580 is writable image .data/PAGE_READWRITE; c29 immutable/tag0
route remains disproven, c30 loaded qword remains opaque NON-pointer top7.
No authenticated full executable/static code image exists in this cloud checkout.
Thus actual exact-slot reference/writer enumeration is NOT PERFORMED, counts are
unknown (not zero), source/allocation/range/lifetime/complete-writers/base proof
all false. Current bounded target snapshot contains a slot consumer, not proof of
all publishers. No typed pointer/pool/array/owner is guessed from [R8+RAX].

New tools/slot_publication.py is a bounded OFFLINE exact-slot reference candidate
scanner, with exact file SHA and pinned Capstone5.0.6 CLI guards. No hash/fixture
bypass is exposed. File512MiB/code64MiB/5M instruction/256reference/table600000/
8MiB caps. Parse PE64 sections/runtime table, exclude containing runtime region
for deferred0x351EB00; if no containing entry, exclude whole containing executable
section. Never decode its body or follow a call. Direct RIP-relative/absolute
memory operands overlapping the8byte slot produce site/width/access/source-kind
metadata only. Operand0 is conservatively a possible writer despite decoder
READ caveats; unsupported indirect/base-index/segment sources stay unresolved.
No live qword, pointer chase, vector values, function/body/bytes/immediate dumps,
credentials or private absolute paths. On-disk file bytes are hashed/decoded, not
interpreted as a live slot value or an allocation pointer.

These are candidates, not authenticated execution-entry/all-writer assertions:
x86 linear sweep may have gaps/data/alternate entries, caps/excluded producer
limit coverage, indirect/register-based aliases are not enumerated. Complete
candidate byte sweep is distinct from writer completeness, ALWAYS false here.
Zero candidates do not prove no writers; multiple writer candidates are ambiguous;
one direct writer does not prove source/allocation/lifetime. No generic cross-
image dataflow or guessed publication chain is fabricated. Directly relevant
loader metadata would require evidence linking it to this exact slot; none is
asserted from unrelated PE metadata.

Runtime SLOT_PUBLICATION explicitly reports scan-not-performed in the bounded
startup walk and opaque source; no offline report is implicitly trusted/imported.
The new JSON is manually reviewable evidence for a future bounded publisher
chain, not a certificate. Instructions in BUILD.md run it against the owner's
existing local EXE without starting the game or uploading the binary. Actual
report cannot be generated in cloud; this limitation is recorded, not hidden.

## Combination and safety

A range alone cannot contain the store; base proof alone cannot either. Even a
hypothetical base contract must prove all writers/publication source/allocation/
full extent/lifetime/critical disjointness plus nonwrapping base addition and
cover offset endExclusive. No current producer sets such a contract. The existing
c36 hard-stop remains, no later opcode or memory map/continuation is added.
New OFFSET_RANGE/SLOT_PUBLICATION diagnostics separate proof classes and tags.

No new live observation class is required for the offset proof. For the base,
missing evidence is exact current file references, authenticated writer entry/
source chain, complete writer/indirect-alias coverage and region/lifetime contract.
An offline candidate report can narrow future investigation but cannot supply all
of those facts automatically. Identical A/E logs, stable values, writable pages
or pointer-looking numbers do not establish them. No deferred producer traversal,
generic memory/store/AVX semantics, guessed ABI or ownership relaxation.
Position/source/input/movement/adhesion/camera/animation/physics writes0; all
caller-owned/lifetime/exclusive/conflict/writable-source gates0. Shared tooling,
SF/ordinary mods/native assertions/cleanup and existing caps unchanged.

## Owned validation

6 new identity groups (110 total) plus8 Python groups. Unsigned JA boundaries
around3FFF/4000 and sign boundary, alternative decoded bounds, sign/shift/extent
nonwrap, stale clobber invalidation, separate base/offset proof, current-code/
parent/c36 escape/full527161 table. Owned exact-slot readers/writers, ambiguity,
indirect/incomplete/capped coverage, adjacent overlap, missing allocation/lifetime,
exact hash guard and deferred runtime-region exclusion. Full chain and independent
extracted source/package validation in CODEX_RESULT / V01C37_RELEASE.json.
Native/tool input count231 (one header and two owned Python tools added).
