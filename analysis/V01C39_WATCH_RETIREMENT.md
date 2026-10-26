# c39 — exact-slot watch admission stopped at exception-context retirement

Task base `750367f`, branch `codex/v01c20-edi-value-provenance`.
The explicit c39 fallback applies: **stop fail-closed when the existing watch
engine cannot safely support the exact slot without weakening cleanup/ownership**.
No c39 slot watch is installed, no new watch API/configuration is exposed, and
there is no c39 game release. Runtime remains v0.1c38/package0.1.56.

## Evidence classes and retained target

Accepted owner c38 evidence remains in V01C38_SLOT_PUBLICATION_SCAN_RESULT.md:
exact authenticated image, writable/readable slot RVA0xC2D2580, local writers0,
1360 resolved affine operands, instruction cap5M reached, gaps737 and unresolved
operands1278483. Neither zero candidates nor the external c28 reader is absence
or publication proof. Reader0x2CF7FE9 is7bytes/READ8, accepted external origin
0xC2D2480+0x100; its post-instruction site would be0x2CF7FF0, not the reader RVA.
No actual game image/slot/current c39 access is read or inferred in this task.

**VERIFIED-STATIC, current owned source:** orientation_trace.cpp owns immutable
per-thread leases and uses hardware DR0/DR1, bounded snapshots/installation,
readback, foreign-slot refusal, suspend/resume retry and tombstones. Its normal
VEH path copies GPRs/XMM/stack-related metadata and reads12/16 watched bytes.
That path cannot be repointed at this8byte slot: it would violate the non-value
contract and read past the watched slot. `own`/`restore` also encode the two
existing Up/Position configurations, not a generic single-slot RW8/WRITE8 lease.

The stronger blocker is retirement of **saved exception contexts**. The handler
decrements `positionInflight` before returning CONTINUE_EXECUTION. An interval
remains before the OS applies `EXCEPTION_POINTERS.ContextRecord`. A coordinator
can then suspend that thread, restore its current debug registers, close its
lease handle and report cleanup_pending0. `activeLeases()` only sees installed/
suspended bookkeeping. It does not prove that the pending exception CONTEXT
cannot later restore the retired DR configuration. Position's inflight counter
guards summary/entry draining, not OS exception-return retirement or new arming.

## Owned execution reproduction (Wine; NOT game/real-Windows evidence)

New `source/src/NativeHooks/watch_retirement_smoke.cpp` includes the **unmodified**
production engine, as existing owned hardware tests do. Only its test translation
unit wraps AddVectoredExceptionHandler. The registered wrapper calls the actual
engine handler, then pauses its return with two OS events. No CONTEXT fields,
production lease flags, caps, deadlines or memory protections are forged/changed.
The pause makes a legal exception-return scheduling interval deterministic; it
does not show its frequency in the game. The wrapper is absent from the DLL.

Both real owned reset and Position watch paths reproduce this sequence:

1. A separate owned native thread performs one scalar store into owned memory.
   A real hardware breakpoint invokes the production VEH and captures one event.
2. After the engine handler returns, while OS exception return is held, capture
   is still open and `positionInflight=0`.
3. The coordinator closes through `pollReset(false,...)`. It reports
   `cleanup_pending=0`, `activeLeases=false`, `current=-1`. A real suspended-thread
   GetThreadContext verifies all saved DR registers restored at that instant.
4. Allow the exception to return. A new suspended-thread readback shows
   `own(context,oldLease)==3`: the saved exception CONTEXT re-enabled DR0/DR1,
   despite unchanged inactive lease bookkeeping. This is the missing guarantee.
5. A later owned store invokes the immutable tombstone path, captures no new
   event, and restores the saved slots. Position restores its exact DR6 snapshot;
   legacy reset preserves its existing status-bit policy (OS reserved DR6 bits
   can normalize). This later recovery is **not** bounded cleanup during silence.

The focused script has4 PASS records, two per process/mode. Initial new-fixture
runs reproduced steps1-4 but incorrectly demanded the stronger Position DR6
snapshot contract from legacy reset in step5. A diagnostic showed only reserved
DR6 bits0 ->0xFFFF0FF0 differed. The new fixture now asserts each existing policy
explicitly; no prior assertion/deadline/cap or production code changed. The same
test also runs Position with exact full-DR snapshot equality.

Command (after the normal Windows cross-build):

```bash
bash source/tools/test_watch_retirement.sh
```

The new script is included in mandatory test_hooks_windows.sh. It uses real
Wine thread context/DR/VEH operations and owned memory/functions only, no game
function/body fixture, private log, game slot read or proprietary code.

## Why c39 does not arm, even with a single-phase alternative

**REJECTED:** cleanup_pending0, activeLeasesfalse, current=-1 or inflight0 alone
is a retirement/phase-transition certificate. They can all hold while a saved
exception context still contains the old watch configuration. Tombstones avoid
an unhandled later matching exception but depend on another hit for recovery.
No hit does not prove hardware unarmed. A new RW8 ->WRITE8 phase or a reset/
Position handoff could compete with this delayed context; no safe combination
is assumed. A single RW8 window avoids re-arm but still lacks bounded close/
timeout/kill restoration proof, so it does not discharge the same prerequisite.

This is not a claim that ordinary reset/Position tests fail, that the game has
shown this race, or that all possible implementations are impossible. It is a
specific reproducible counterexample to reusing the existing cleanup indication
as the stronger c39 guarantee. Existing production code is unchanged, including
its known limitation. No broad debugger, PAGE_GUARD, arbitrary detour or weakened
lease ownership is added to bypass the blocker.

Before authorizing a c39 mode, a bounded mechanism must account for retired
saved exception contexts and keep cleanup/serialization pending until retired
configurations cannot be reapplied. It must also preserve exact owned-slot and
foreign-state restoration and fail closed on inability to establish retirement.
Checking a counter immediately before returning from VEH is not sufficient:
exception return itself still lies after that check. Extend the owned delayed-
return regression to prove the new mechanism, rather than delete/skip the case.
The value-free event/arm path and unique current-code post-RIP identity mapping
also still need implementation after this prerequisite; merely changing an
address/DR7 bitfield in the existing value-capture path is forbidden.

## Validation boundary and required next work

The c39 requested address derivation/RW8/WRITE8, reader current-byte mapping,
ambiguous-site refusal, access/overlap classification, phase budgets and
per-thread exact-slot coverage tests are **NOT IMPLEMENTED / NOT CLAIMED**.
No slot mode exists to test. Existing debugger/foreign-slot/snapshot/error/cap/
timeout/Deinit/F8/identity/cleanup regressions remain in the mandatory full suite;
their success must not erase the new retirement counterexample. This task's new
regression establishes the specific permitted stop condition, not completion of
the positive access-capture acceptance matrix.

Detailed full build/regression/reproduction results and commit are in
docs/CODEX_RESULT.md and V01C39_WATCH_STATUS.json. The task file is unchanged.
No v0.1c39 ZIP/script/log is generated; existing c38 artifacts remain unchanged.
Git contains the new fixture, script and audit. Future package builds include
this audit alongside accepted c38 evidence; a new game deployment is unnecessary.

## Unchanged proof and write gates

C37 offset[0,0xFFFC0]/stride64/width32/endExclusive0xFFFE0 remains separate from
base/publication/all-writer/allocation/lifetime/ownership, all unproven. C36
escape1/pending1/contained0/control-alias1/CFGblocked1/continuation0 remains.
Producer0x351EB00 stays excluded/deferred. No slot-content read, numeric loaded
pointer, pointer chase, register/stack/vector dump, slot write or publication-
chain interpretation is added. All new MOD Position/source/input/movement/
adhesion/camera/animation/physics writes0. Existing game behavior, exact image/
code guards, native semantics, SF/ordinary mods/shared tools/updater unchanged.

No repeated offline scan or A/E/wall-face/standing/landing/fast-travel test is
requested. The owner need not run Update-WS/Install mods/Run modded game for this
test-only admission audit. Next work is a separately reviewed bounded retirement
mechanism, followed by the deferred non-value exact-slot mode and its tests.
