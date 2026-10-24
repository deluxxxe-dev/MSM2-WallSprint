# c22 pinned TEST operand / flag flow

**ACTUAL C22 ACCEPTED:** V01C22_RUNTIME_RESULT.md supersedes the original
preparation/pending notes below: TEST r8b/r8b -> JE has no selected tags; the
next exact barrier is CALL0x2B235D4. No complete writer/ownership proof follows.
C23 preparation: V01C23_READ_CALL_FLOW.md.

## Accepted input and evidence boundary

V01C21_RUNTIME_RESULT.md accepts exact read0x2B23578 from Transform+0x10C
intoEAX, before write0x2B2358E, possible use0x2B23581 and control0x2B23583.
The actual remaining TEST0x2B235BE operands are UNKNOWN in cloud. We do not
reconstruct/request/distribute the current private137/183-byte game functions.

C22 is bounded Outcome C preparation. Only the authenticated current callee
snapshot is inspected at the pinned TEST site, after the existing identity,
complete CFG and c20 EDI guards. No new hook, live data read, game call, code
dump, ABI guess or protected-effect certificate is introduced. The old c19/c20
earlier-read/effect refusal remains even when a local owned flow completes.

## Narrow semantics

The opted-in instruction must decode as TEST with exactly two same-width GPR
operands and no LOCK/REP prefix. Decoder register access must independently
show only operand reads and an EFLAGS write. Supported widths are32/64bits,
plus read-only low8bit aliases at this one pinned instruction. No partial GPR
write is modeled. High-byte AH/BH/CH/DH,16bit, immediate, memory/indexed forms,
other opcode/effects and non-Jcc consumers still stop exactly.

For the low-byte case, the parent GPR's entire possible-dependence mask is
retained conservatively. This can over-report byte dependence, never clear it
or prove which byte contains the value. General reg()/State/Slice, pointer
provenance, partial-write refusal and all c19/c20 certificates are unchanged.
An unpinned byte TEST retains c21's original refusal.

TEST does not write its GPR operands, does not read old flags and is not a
self-zero idiom: TEST EAX/EAX can depend onEAX. AND-result ZF/SF/PF may depend
on either operand; CF/OF are0; AF is undefined. No numeric flag value is guessed.
JB/JAE/JO/JNO therefore have no value dependence on this TEST's operands;
other regular local Jcc retain conservative possible dependence. Both CFG legs
are always enumerated, including constant-condition branches. An intervening
flag write kills the TEST-to-consumer link. Non-Jcc flag consumers stop rather
than inferring effects involving undefined AF or expanding the CPU model.

Masks remain1 selected read,2 post-write same-field reload,4 other pre-write
same-field read. TEST operand masks, result flags, consuming branches and flags
surviving a RET keep these distinctions. Parent masks are unions over analyzed
reaching states; zero samples/early blockers are not complete coverage. Existing
READ_FLOW reports selected-value GPR/XMM/caller stack/control/escape and next
barrier. Possible control dependence is not propagated as a numerical proof or
as a claim that any downstream XYZ/W store actually changes.

## Compact logs / caps

- MOVEMENT_CALLEE_READ_TEST: decoded mnemonic/form, flags-only effect,
  low-byte scope, analyzed/modeled state counts, possible flag tags and reason.
- MOVEMENT_CALLEE_READ_TEST_OPERAND (at most2): exact operand class/name/width,
  parent GPR/tag union and low-byte overapproximation; unsupported memory forms
  show base/index/segment/scale/displacement, immediate forms their literal.
- MOVEMENT_CALLEE_READ_TEST_FLAGS: first observed local flag consumer and kind,
  consumer tag union, first possible dependent branch, surviving flag tags.

All report only compact single-instruction metadata, never full bytes. Current
READ_DEPENDENCY/READ_FLOW and exact first unresolved instruction/RVA remain.
Each record stays below the4096-byte logger limit. Existing4096visit,9pending,
128stack-slot,8branch,16writer-call-input and no-recursion/no-call-expansion caps
are unchanged. A TEST decoded but not reached is explicitly separate from a
modeled reaching state; a mid-instruction anchor is not silently rounded.

## Owned validation and actual next step

Eight additional fixture groups retain the eight c21 groups and c19/c20 gates.
They cover both register operands/self-test, killed/dead/unrelated values,
low-byte/DIL/64bit forms, all16 regular Jcc, killed flags, separate post-write/
other-field tags, high-byte/16bit/immediate/indexed/non-TEST refusal, unmodeled
flag consumers, CALL and authentication/disable/boundary controls. Fixtures are
independently authored, nonexecuted CPU inputs at fake0x1000/0x2000, not game
reconstruction. Final full-build/standalone results belong in CODEX_RESULT and
V01C22_RELEASE.json.

Actual TEST operands, flow completion or next exact unsupported point remain
PENDING the new log. Local completion does not settle XYZ/W effects or
cross-invocation feedback. Producer0x351EB00 and all movement-source ownership/
write gates stay deferred/0. Existing native orientation/lifecycle/ABI/hardware
cleanup stays unchanged.

Use Update-WS -> Install mods -> Run modded game; A~4s -> fully end WallRun ->
move/fast-travel -> E~4s. Return the newest complete private
MSM2-WS/logs/MSM2-WS-v0.1c22-YYYYMMDD-HHMMSS.log. The three TEST record families
are the only new fields requested. No raw function body or video is needed
unless a visible anomaly occurs.
