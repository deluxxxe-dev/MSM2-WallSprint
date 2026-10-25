# c29 — exact image-backed qword current-proof rule (prepared)

## Accepted evidence and scope

Base822993e, implementation line codex/v01c20-edi-value-provenance; no main merge.
VERIFIED-RUNTIME c28 (V01C28_RUNTIME_RESULT.md): exact MOV0x2CF7FE9 is
`mov rax,qword ptr [rcx+0x100]`,7bytes/full64 RAX only/flags preserved.
Current parent RCX is same affine image-root16+204285056, tag0; source8bytes
at image-root16+204285312. Incoming RAX selected tag1. Image address alone
does not establish loaded-value provenance. C25 full527161-entry validation/
queried no-entry, c26 CMP tag0 and c27 JA both forward edges remain accepted.
Actual PE source section/current mapped page/value equality/next barrier UNKNOWN.

## Implementation and proof order

ReadEntryTags obtains samples/current union tags and exact same-origin RCX
from currentReadEntryEvidence, never a guessed ABI or live pointer value.
First parent pass collects this proof, second pass uses it; same two-pass bound,
reset seeds and identical target bytes required. Only pinned7byte full64
MOV RAX,[RCX+0x100], RCX tags0, this single source RVA are eligible.

Before any value read, validate guarded PE64 DOS/NT and1..96 section headers
against accepted current executable/SizeOfImage. Keep exact metadata privately
(max4168bytes) for equality, not just a collision-prone checksum. All8 source
bytes must fit exactly one readable non-writable section, with no other
section overlap. Reject malformed/out-of-image/ambiguous/cross-page ranges.

Reject overlap with direct PE import(1), TLS(9), load-config(10), bound-import(11),
IAT(12), delay-import(13) directory ranges. Validate pairs/ranges. This checks
only those direct ranges; no descriptor/pointer chasing or claim that every
loader-mutated referenced subrange has been enumerated. Non-writable section
and current-page gates independently remain mandatory.

VirtualQuery must report the whole source in the same image allocation, inside
image bounds, MEM_COMMIT/MEM_IMAGE, with current protection exactly READONLY
or EXECUTE_READ. Reject RW/execute-RW/COW/guard/noaccess/unsupported flags,
wrong allocation/type/state, mapping boundary or overflow. AllocationProtect is
reported and compared; current Protect determines current non-writability.
Query is diagnostic even for a writable PE section; no value read then occurs.

Only after structural/page gates: guarded read exactly8bytes before bounded
target walk, then reclassify metadata/page and guarded reread exactly8bytes
after it. Equality and identical metadata/page required. Max2 value read
attempts; failure/change/repeated finish refuses. Also recheck target bytes
when the walk stops at a later unsupported instruction, before granting a
local-load result. Runtime coverage and final target code gates still precede
complete target identity. Code/reproof/coverage failure invalidates load proof.

The private bytes are compared only: no log/raw hash/numeric interpretation,
no use as pointer, no second dereference. Loaded Slice origin is opaque
memoryWide(image-root16,sourceRVA), never a pointer. On success only full64
RAX tags become0, prior RAX tag1 replaced; all other registers/flags/memory/SP
are unchanged. Current actual caller state rechecks RCX exact origin and tags,
so another caller state or load form cannot reuse this permission.

## All-path and safety limits

Unchanged target0x2CF7FE0, one JA/two paths/128bytes/32unique instructions/64visits.
Both outcomes explored without a numerical choice. Every path must modeled-RET
and pass final current code/runtime checks for begin/end/hash and effects.
Conservative merge of all returned tags/origins; one unsafe leg refuses target.
After an admitted exact load, the next unsupported op is reported precisely.
A changed/unreadable source overrides that barrier back to MOV0x2CF7FE9.

No generic load rule, other memory read/write/RMW, stack effect, nested CALL,
second branch, indirect/backedge, unsupported partial destination or cap increase.
No new hooks, data protection changes, producer0x351EB00 traversal, raw values
or proprietary bodies. Position/source-position/input/movement changes0.
Ownership/lifetime/exclusive downstream/writable source remain unproven0.

## Evidence limits and needed observation

VERIFIED-STATIC (owned fixtures): executable proof policy, equality/failure and
state/effect rules. This is bounded current-proof stability, not an atomic
snapshot, permanent image immutability or global lifetime guarantee. Two equal
reads do not exclude intervening changes or future changes. No actual game
source protection/value stability or all-target success is inferred from fixtures.

Minimum real evidence: IMAGE_SOURCE/PAGE/VALUE plus existing exact next barrier,
MOV/JA/current identity and safety/cleanup records in one normal A~3sec / fully
end / move / E~3sec capture. Neither raw qword nor raw game bytes is requested.
If source is writable, stop without values; do not broaden permission to progress.

## Reproduction

Build source/tools/build_windows.sh; portable test.sh; smoke_windows.sh;
test_hooks_windows.sh (includes identity44, retained read/call21 and workspace13).
New8 groups cover structural/page metadata, loader overlaps, guarded equality/
failures, metadata/page/code mismatch, full RAX overwrite and state preservation,
caller-proof mismatch, all-path effects and exact next barrier. All prior
c19-c28/native safeguards retained. Final results/hashes: CODEX_RESULT and release JSON.
