# c30 — exact writable-image qword as unknown may-tag top (prepared)

## Accepted input

Basebd120c5 on codex/v01c20-edi-value-provenance, no main merge.
VERIFIED-RUNTIME c29 (V01C29_RUNTIME_RESULT.md) source0xC2D2580/8bytes in
.data0x9B24000..0xC565A28, characteristics0xC0000040 readable/writable/nonexec.
Same allocation committed MEM_IMAGE current PAGE_READWRITE; c29 correctly
refused tag0 BEFORE values (read_attempts0), no local/target admission.
This rejects the immutable/tag0 route, not evidence of any observed taint.
C25 full527161 table/no-entry, c26 flags-only CMP/tag0, c27 short JA edges and
c28 exact7byte full64 RAX <- [RCX+0x100]/tag0 current RCX source remain accepted.

## Narrow abstract effect

Production explicitly selects pinnedUnknownLoad mode. Existing readonly/value
proof API is retained unchanged for its c29 regression; c30 never starts that
value-reading path. The shared bounded metadata classifier is used only for
PE/page checks and still reports the c29 readonly refusal separately.

Only pinned MOV0x2CF7FE9,7bytes/address64/2operands, RAX64 <- mem64[RCX+0x100],
no prefixes/index/segment, decoded pure read/access/flags-only preservation,
exact parent RCX image-root16+204285056/tags0 and same-source samples may qualify.
Current source0xC2D2580 must still be wholly in accepted exact readable/writable
nonexec .data range/flags, unique/no overlapping section, no explicitly checked
direct loader-directory overlap; current whole8byte mapping must be same image/
MEM_IMAGE/MEM_COMMIT/PAGE_READWRITE, valid bounds. This intentionally narrow
accepted-metadata pin refuses changed/unsupported mapping; it is not a generic
writable-memory rule. Before/after PE/page metadata must compare exactly and
target code is rechecked even on a later barrier. No source-value read occurs.

The three tracked categories form a powerset lattice: bit1 selected, bit2
postwrite, bit4 other-prewrite. Union is bitwise OR and top=1|2|4=7. Every
actual combination of these categories is a subset of7. Assigning top states
unknown possible dependence, never observed presence of all categories.

The precise effect is only RAX abstract value <- unknown(globalRoot16,source),
RAX tags <-7; other GPR/XMM/flags/SP/memory unchanged. Unknown origin is not
pointer/constant/semantic type/ownership. Prior abstract RAX is structurally
replaced, but selected bit1 remains possible via top7; never call this killing
selected dependence. No equality/numeric qword read/log/interpretation/chase.

## Carrier masks and actual category tags are different domains

Identity prewalk historically uses32 bits of entry-carrier dependencies, not
the three actual category tags. Injecting literal7 into that mask would falsely
mean dependence only on entry RAX/RCX/RDX and could disappear when entry tags0.
C30 converts all registers/flags on the eligible path exactly once through
current supplied entry tags BEFORE assigning intrinsic7. That path thereafter
uses existing actual-category propagation. The other JA leg is converted before
return/merge. Barriers map any remaining carrier masks through current entry
tags; samples0 is unknown. Already-actual c23 caller walk never remaps tags.
The authorized JA is before the load, so its original exact carrier-mask/current
tag gate is unchanged; every later branch still refuses.

## Continuation, identity and diagnostics

Existing admitted register moves/zeroes/CMP/TEST propagate upper bounds, with
normal zero-idiom precision only for modeled operations. Recheck actual caller
source/tags on every analyzed state; conflicting/different parent refuses.
Analyze both paths under unchanged1JA/2paths/128bytes/32unique/64visits.
Next unsupported memory/stack/CALL/indirect/branch/partial/backedge/crossing/cap
refuses at its exact RVA/mnemonic/reason; record path and incoming JA edge plus
full GPR/XMM/flags may-tags immediately BEFORE that instruction, no lookahead.
No opportunistic support for the next effect. If metadata/code changes, refuse
back at MOV without treating a later diagnostic as trusted current state.

Only all modeled normal RETs + unchanged final code/runtime coverage yield
target span/hash. C23 repeats state checks and conservative union of both
returned tags/origins, keeping differing effects explicit. One safe leg or local
MOV success is not a target/effect certificate. A successful target does not
complete writer XYZ/W/feedback/ownership/lifetime/exclusivity/writability.

## Safety and evidence limits

No new hook/game call, source-qword read/log, guessed ABI, PAGE_GUARD/protection
mutation or producer0x351EB00 traversal. Position/source-position/input/movement
writes0; ownership/lifetime/exclusive/writable-source gates0. Existing orientation
override and all its native lifecycle/ABI/assertions/deadlines remain unchanged.

VERIFIED-STATIC/owned fixtures only: narrow rule, domain conversion, top union,
metadata/code refusal and next-barrier telemetry. Actual c30 next instruction/
path/tag-state/full target completion UNKNOWN; compact c29 evidence lacks the
next bytes and no private target body is in the repository. Minimum live capture
therefore genuinely needed: UNKNOWN_LOAD + LEAF_BARRIER/TAGS/CFG and existing
JA/current identity/safety/cleanup, A~3s/end/move/E~3s. No raw qword/body requested.

## Reproduction

Full build_windows.sh, test.sh, smoke_windows.sh, test_hooks_windows.sh. Identity
52 groups (8 new), retained read/call21, all c19-c29 regression; no private
game fixture required. In particular all entry tags0 still produces intrinsic
top7, copied top persists, other state remains, actual caller disagreement/
unsafe leg/metadata/code/runtime changes refuse, all path return effects union.
Exact final commands/counts/hashes in CODEX_RESULT.md / V01C30_RELEASE.json.
