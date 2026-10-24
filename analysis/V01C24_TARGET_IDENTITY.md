# c24 exact target identity / strict missing-unwind leaf

**C24 ACTUAL RESULT ACCEPTED:** V01C24_RUNTIME_RESULT.md supersedes original
preparation notes:527161 derived entries exceeded old262144 cap; no target
identity admitted. Lookup fields at that refusal were defaults, not an executed
OS probe (see c24 source and V01C25_RUNTIME_TABLE_BUDGET.md). C25 separates
structure/caps/coverage under a finite larger metadata budget.

## Accepted input / missing facts

V01C23_RUNTIME_RESULT.md accepts direct CALL0x2B235D4 ->0x2CF7FE0 and selected
tag1 in raw GPR0 (mem32 Transform+0x10C). Target begin/end0, boundary/hash
unavailable, modeled_states0. This does not identify why lookup failed or what
the target does. C22 TEST r8b/r8b -> JE remains disjoint. Partial return EAX mask1
and prefix memory_escape0 do not certify the unexamined CALL path.

No authenticated game body/PE runtime table is available in cloud. We do not
reconstruct a body from its address/hash or distribute/request private code.
Actual c24 reason/span/hash/effect remain PENDING compact identity diagnostics.

## Boundary classification

Only the exact admitted build/direct-call target0x2CF7FE0 opts in. Owned
authenticateReadTarget uses guarded image-RVA and runtime-lookup callbacks; it
does not read arbitrary pointers or discover other function targets.

1. Verify in-image target, MZ/PE64/AMD64/SizeOfImage, bounded section table and
   exactly one containing readable executable code section without write flags.
   Reject data/RWX, overlapping sections, malformed ranges and inaccessible headers.
2. Validate the PE exception directory: in-image,12byte entries, at most262144,
   readable64-entry chunks, ordered nonoverlapping begin/end and bounded unwind
   RVA. This validates structural regions, not full unwind-program semantics.
3. Compare exact OS runtime lookup with the containing PE entry. Wrong module,
   unreadable/invalid lookup, lookup/table disagreement and ambiguous metadata
   refuse. An entry containing the target with another begin is a distinct
   refusal. Missing metadata is never relabeled as an existing runtime boundary.
4. A matching exact runtime entry keeps the c23 maximum128byte span and code-
   section/current-snapshot requirements. This ordinary identity path does not
   itself certify the target's effects; c23 may refuse a memory/control effect.

The no-entry fallback is considered only if both the structurally valid PE table
and OS lookup establish no containing runtime entry at the exact pinned target.
The next PE region and OS lookups over at most128candidate bytes bound the scan;
dynamic regions are detected before reading their bytes. Candidate bytes cannot
cross the code section or another runtime region. No other edge/body is followed.

## Strict linear proof, not RET search

Read the bounded candidate window twice through guarded reads and require exact
equality. Decode from the exact target with Capstone5.0.6, no byte skipping and
at most32instructions/128bytes. The overflow-detecting decode may report33,
but no effect of that instruction is admitted. Branches (including forward Jcc), CALL/indirect/
interrupt transfers, memory operands, partial/unknown register effects, stack
construction/change and unsupported instructions refuse at the exact opcode.
There is no recursive CFG/function finder or expanded cap.

Only c23's register-only move/vector copy/full self-XOR/32-64bit CMP/TEST plus
NOP/ENDBR and a plain normal C3 RET can qualify. Existing Slice must also accept
the register effects. All preceding instructions are decoded, modeled and have
one fallthrough successor. Consequently the reached RET is the sole reachable
normal exit from this single direct entry. A branch around an earlier RET is
rejected at the branch, never truncated at the convenient RET. A missing RET
before a runtime/code/cap boundary refuses. The admitted span is this modeled
single-entry exit set, not a recovered compiler-symbol/global function extent.

Recheck runtime coverage over every selected no-unwind byte (or matching exact
runtime entry) before admission. Hash only the admitted prefix after stable
capture; hash failure revokes admission. Identity is current startup snapshot
evidence, not an independent historical digest or future per-invocation proof.
Changes after that snapshot remain outside this diagnostic certificate's scope.

## Resume / gates / logs

Only admitted begin/end/size/hash and bytes feed the existing c23 register-only
effect sidecar. Failed identity propagates its precise RVA/mnemonic/reason,
without consuming a nested effect state or silently invalidating registers.
Successful identity may still fail at c23's next exact effect boundary; local
completion does not settle prior control dependence, XYZ/W or cross-invocation.
No c19/c20 read/effect refusal is waived and no ownership/write certificate issued.

New MOVEMENT_CALLEE_READ_TARGET_IDENTITY reports region bounds, lookup fields,
PE table validity/count, missing-entry state, method, scan/selected sizes,
instruction count, snapshot equality and exact barrier/reason. Existing READ_CALL
reports admitted begin/end/hash and READ_FLOW reports continuation/next barrier.
No full bytes, live data dump, new hook, guessed ABI or game-function call.
PE/runtime metadata and the single bounded code snapshot are read only.

Existing exact build/current-code/CFG guards, State/Slice/c19-c23 models, native
lifecycle/orientation/raw-thunk ABI/pairing/hardware cleanup/deadlines/assertions
remain. writable_source_proven/caller_owned/ownership_exclusive/
source_lifetime_exclusive/downstream_conflicts_proven_absent0;
Position/source-position/input/movement writes0. Producer0x351EB00 deferred;
no movement/adhesion/camera/animation/physics, data-page mutation or PAGE_GUARD.

## Owned validation / actual capture

Nine independently authored sparse PE/code fixture groups cover safe no-entry
linear completion and pin controls; branch/first-RET trap/nested CALL; memory/
stack/partial refusal; static/dynamic region crossing and metadata overlaps;
unstable/unavailable snapshot and runtime coverage changes; exact runtime entry/
interior/lookup disagreement/cap; invalid image/code/PE/table; byte/instruction/
RET limits; identity-to-c23 selected live/killed return integration and precise
refusal propagation with all prior effect gates closed. Existing21 c21-c23 groups
and full regression chain remain. No private game bytes are used.

Prepared Outcome C: actual target boundary reason and optional proof are not
known from compact c23 evidence. The new field must be captured once to resolve
this question; this is not a habitual rerun of completed direct-call decoding.
Update-WS -> shared Overstrike Install mods -> Run modded game,
A~4s -> fully end WallRun -> move/fast-travel -> E~4s. Return newest complete
MSM2-WS/logs/MSM2-WS-v0.1c24-YYYYMMDD-HHMMSS.log privately. Only TARGET_IDENTITY
is new; old CALL/READ/TEST/value/effect/safety/cleanup remain in the same log.
No full function dump/video unless a visible anomaly. Final tests/hashes/commit:
CODEX_RESULT and V01C24_RELEASE.json.
