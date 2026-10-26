# c41 — owned post-return continuation witness

Base4b39a82, codex/v01c20-edi-value-provenance. Current AGENTS/manual/rules/
workspace/state/reporting/NEXT, c39/c40 evidence and the existing watch/helper/
owned tests were reviewed, plus relevant Wisdoms references. No main merge.

## Result boundary

**An actual post-OS-return witness is feasible in the explicitly bounded owned
Wine fixture.** It is not integrated into production, is not a general saved-
context retirement certificate and is not a native-Windows/CFG/CET validation.
Runtime stays v0.1c38/package0.1.56. No c41 game release or exact-slot mode.
The original c39/c40 counterexamples remain intact and continue to reproduce
the production engine's known limitation.

New sources: NativeHooks/continuation_witness_smoke.cpp and
continuation_witness_harness.S; tools/test_continuation_witness.sh is mandatory
within test_hooks_windows.sh and also runs separately. All are owned test code;
none enters build_windows.sh's DLL source/link list.

## Exact executable contract

Each context gets its own15byte immutable stub:

```text
C7 05 <disp32> <imm32>   MOV dword ptr [RIP+marker_disp], context_token
E9 <disp32>             JMP rel32 original_continuation_RIP
```

No CALL/RET/indirect JMP, scratch GPR, PUSH/POP, stack adjustment, flag operation,
vector/FPU/XSTATE instruction or application-data access occurs in the stub.
The sole data write is4bytes of dedicated aligned witness storage, and the sole
control effect is a direct jump to the exact saved original RIP. Literal bytes,
Capstone5.0.6 complete2instruction decode, effects/operands, signed displacements,
original target and current RX identity are checked. Wrong opcode/target/current
bytes, alignment, signed-rel32 limits and address overflow have owned negatives.
The code is generated independently; no game/system function body is copied into
it.16 current target-code bytes are retained privately only to reject mutation;
no code/value dump is emitted in results or committed.

The handler changes only its owned saved ContextRecord.Rip, after the unchanged
production handler has processed the real owned hardware event. Context flags,
debug registers, GPRs, RFLAGS, RSP and vector fields are not forged/rewritten to
obtain a PASS. TF/RF or missing CONTROL/INTEGER state are outside the supported
boundary and reject; arbitrary original RIP is not accepted for the hardware
event. It must be the exported post-store label in the owned assembly harness.

## Ordering, identity and outstanding contexts

The first instruction's nonzero token store can occur only after the OS resumes
the redirected saved context. Before releasing the actual exception return,
the marker remains0. Coordinator reads use InterlockedCompareExchange(p,0,0),
which cannot create a nonzero marker. Initialization is zero; no test/controller
code writes the nonzero token. Executable stubs are entered only by redirecting
the bound worker's saved context in this controlled fixture.

Tickets are immutable per exact worker thread handle/id, watch lease/window1,
generation101 and nonzero context sequence. Every context has separate code,
marker address and expected token; no reuse occurs within a thread lifetime.
Wrong thread/lease/generation/context keys refuse. C++ release/acquire publication
and owned preparation events publish records before any permitted redirection.
This is a controlled non-adversarial owned-process proof, not protection against
an external debugger or arbitrary code deliberately jumping into the stub.

One actual hardware breakpoint is followed, where requested, by1 or2 synchronous
owned RaiseException contexts inside the still-outstanding outer handler. Those
software exceptions are explicitly part of the fixture model, not game hooks.
Close is requested while inner and outer contexts are both outstanding. Inner
acknowledgment must leave the outer debt pending. The multiple case opens another
independent inner context before allowing the outer return:3 distinct tokens,
never one collapsed counter. Unmodeled worker exceptions poison admission.

The **test-owned coordinator** calls cancelReset/kill to close capture, but refuses
the old engine's final pollReset release while any ticket is unacknowledged.
Unchanged watch deadline/kill do not discharge a ticket. After all exact witnesses
and current code/identity checks, it permits final restoration and verifies3
stable DR readbacks. The worker performs no second watched store. Existing reset
policy and Position's saved snapshot are preserved; a synthetic foreign DR2 is
kept exact. Initial occupied DR0..3/GD or debugger presence refuses the probe.

At the context cap, an untracked nested return poisons admission permanently;
even all known markers cannot certify final retirement. This negative case
tears down only after the owned thread terminates, not by claiming successful
live-thread restoration. JSON explicitly reports retirement_accepted=false.

## Allocation, lifetime, ABI and state preservation

The coordinator, not VEH, allocates8KiB private committed storage per ticket.
A bounded256-attempt,64KiB-aligned search stays within signed-rel32 reach of its
current continuation. The first page changes RW -> RX, followed by successful
FlushInstructionCache; the marker page stays RW/non-executable. No RWX, PAGE_GUARD,
arbitrary code patch or cross-process action. Allocation/protection/decode/current-
identity failure emits a failed owned probe, never a retirement certificate.

Marker acknowledgment is NOT proof the following JMP has finished fetching.
Code/data remain mapped until the actual worker thread handle is signaled.
Attempts to free while alive refuse, both before and after acknowledgment.
Only after bounded owned join may VirtualFree run. Timeout/assertion failure
terminates this isolated owned probe process with compact FAIL JSON; it does
not attempt unsafe cleanup in a game or claim continued production safety.

The owned Win64 harness has explicit .seh_proc/push/stack/XMM unwind metadata
and restores callee-saved GPR/XMM state. It places nontrivial values in all GPRs,
RFLAGS and YMM0..15 and snapshots immediately before its single scalar store
and at the original continuation before further test actions. It compares:

- 16 GPRs including RSP, and full observable PUSHFQ RFLAGS (TF/RF excluded);
- 512bytes covering YMM0..15, plus512byte FXSAVE legacy FP/MXCSR/XMM state;
- 64bytes of active-frame stack canaries.

Snapshot instrumentation itself uses an owned stack and stores; the generated
stub does not. No claim is made that OS exception machinery preserves inactive
stack space below RSP. Other XSTATE components (e.g. AVX512/AMX/PKRU) are not
runtime-measured here; the stub does not address/use them by literal construction.
No full-XSTATE payload preservation claim is inferred from the YMM sample.
AVX/OSXSAVE/XCR0 support is checked before executing the owned AVX harness.

## Mitigations and exact native-Windows question

GetProcessMitigationPolicy queries DynamicCode, CFG and UserShadowStack policy.
Any reported nonzero flags refuse; unknown policy on native Windows refuses.
Wine may expose compatibility API behavior, so its zero flags are recorded but
are not promoted to native-Windows protection evidence. Current owned Wine probes
returned known=true and flags0 for all3. Direct JMP avoids an indirect branch/
RET in the stub, but that does not prove OS saved-RIP validation/CET compatibility.
Enabled protections are outside this candidate's declared boundary; no policy is
disabled or changed to make the test run.

Canonical MicrosoftDocs sdk-api (docs branch) references inspected:
memoryapi/nf-memoryapi-{virtualalloc,virtualprotect}.md;
processthreadsapi/nf-processthreadsapi-{flushinstructioncache,getprocessmitigationpolicy}.md;
winnt/ns-winnt-process_mitigation_user_shadow_stack_policy.md. They document
RW allocation/executable permission/cache coherency and SetContextIpValidation/
shadow-stack restrictions; no extra undocumented API or assumed game ABI is used.

**Still UNKNOWN:** the same owned redirection/state/DR sequence on actual Windows
under the reported supported mitigation boundary. The standalone owned artifact
in analysis/native-probes/c41 answers that OS question without a game launch.
Its --position --multiple --json invocation returns one compact JSON result.
Source/binary hashes, source commit, licenses, exact command and outcomes are in
that folder's README/PROBE_MANIFEST.json and CODEX_RESULT.md. Active/unknown
native mitigations return REFUSED, not PASS; those configurations remain unsupported.

## Validation and next authorization boundary

The focused script runs close/deadline/kill/nested/multiple/cap in reset/Position:
12 process cases,10 positive bounded witnesses plus2 expected cap refusals.
The original retirement script still runs unchanged (14 observation/recovery
PASS,2 VCH UNAVAILABLE). Required full build/native/watch/scanner/workspace
results, exact commands, initial compile corrections and independent Git/package
verification are in CODEX_RESULT.md / V01C41_WITNESS_STATUS.json.

This fixture controls all exception-generating operations and registers every
supported context before allowing return. It does not solve arbitrary production
dispatch-before-registration races, all-thread/new-thread coverage, thread/lease
reuse, unmodeled reentrancy, process-wide lifecycle/admission or mitigation-
enabled Windows continuation handling. A separately authorized integration task
must review those and native evidence before trusting any production retirement
or enabling a c39-style slot watch. The c39/c40 production gap remains.

All c37 range/c36 escape/CFG and publication/allocation/lifetime/ownership gates
remain unchanged. Producer0x351EB00 stays deferred. No slot qword, game state,
source/Position/input/movement/gameplay write, SF/shared-tool/updater change or
new game release. No Update-WS/game/WallRun/offline scan/log is requested.
