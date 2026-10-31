# Owned StackWalk64 native failure investigation — diagnostic revision2

## Result

**INSTRUMENTED_FIXTURE_DELIVERED / ROOT_CAUSE_UNKNOWN / PRODUCTION_FAIL_CLOSED.**
Native predecessor remains genuinely **146 PASS /55 FAIL**. The55 failures include
unhooked originals as well as generated code. Direct RtlVirtualUnwind/ABI/table/
trampoline/race results do not override failed StackWalk assertions. Wine201 PASS
is not native PASS. No production integration or new game acquisition.

Baseline24a438a on codex/v01c20-edi-value-provenance; no main merge.
Accepted input: PRESENCE_OWNED_WINDOWS_FAIL_20261031.md. Previous source/manifest/
Wine evidence remains in Git7439b04; manifest-v1.json and original Wine JSONs are
also retained in native-probes/presence. They describe the predecessor, not v2.

## Contract review and proven source facts

Microsoft primary StackWalk64 documentation, pinned MicrosoftDocs/sdk-api commit
`656e9656bfaef34dd29cd7ee9e1146aec17e924c`, files
`sdk-api-src/content/dbghelp/nf-dbghelp-stackwalk64.md` and
`ns-dbghelp-stackframe64.md`:

- PC/frame/stack must be initialized. CONTEXT is mutable and a valid record is
  recommended. x64 AddrStack is RSP; AddrFrame is RBP or RDI and not always used.
- NULL ReadMemoryRoutine uses the default reader with a valid process handle.
  Our GetCurrentProcess handle is valid and consistent across symbol/walk calls;
  callbacks use actual RtlLookupFunctionEntry and allocation/image base. No
  invented dynamic table or change to memory-reader behavior was introduced.
- API generally does not set last-error. Clear it immediately before call and
  retain the observed value; zero is not a diagnosis and callback API effects
  must not be interpreted as an authoritative StackWalk failure code.
- DbgHelp is single threaded. All owned unwind sampling runs serially before
  actual fixture race threads; no concurrent DbgHelp invocation is added.
- The docs do not promise the same first-call/current-frame progression as Wine.
  Record each returned PC/SP/FP/return/CONTEXT instead of assuming a particular
  first-call skip or weakening caller/stack equality.

Canonical API pages are https://learn.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-stackwalk64
and https://learn.microsoft.com/en-us/windows/win32/api/dbghelp/ns-dbghelp-stackframe64 .
Direct Learn access was403 in this environment; primary Microsoft-owned source
was retrieved successfully via pinned raw.githubusercontent.com. No unofficial
blog was substituted as Windows contract evidence.

**VERIFIED-STATIC fixture issues requiring differential evidence:** old CONTEXT
is zero initialized with CONTEXT_FULL but zero segments/EFlags; AddrFrame is set
to RSP while the supplied CONTEXT.Rbp is a test sentinel; the synthetic return
address is the ENTRY of ws_test_invoke_probe, not an actual CALL return PC.
Each is a candidate oracle/setup difference, **not a demonstrated root cause**.
No source-based guess is presented as proof fixing55 native failures.

## Implemented discriminator / unchanged predicates

1. Original Request/Gravity controls now execute before MH_Initialize, proving
   they are genuinely unhooked controls, not merely restored after disable.
2. All55 original StackWalk assertions and exact expected caller PC/SP equality
   remain. Same3-call maximum, no blanket success, skip, alternative-path OR,
   stack masking, replacing StackWalk with VirtualUnwind or relaxed metadata.
3. Four separately labeled traces per sample (220 total): original zero-context/
   frame=RSP baseline; RtlCaptureContext-seeded context with test registers/RIP/RSP
   overwritten and frame=RSP; same seeded context with frame=RBP; original context
   with a separate genuine OWNED CALL-return PC. Alternate traces are diagnostics
   only and cannot make the primary assertion PASS.
4. Genuine return PC comes from new WS-owned presence_unwind_caller's CALL to an
   owned stub and its exported after-call label. No proprietary body/native game
   method is invoked. It isolates sentinel choice without changing baseline.
5. Every trace records initial PC/SP/frame and CONTEXT.Rbp, expected PC/SP, owned
   backing-array bounds and actual thread stack bounds (GetCurrentThreadStackLimits),
   segments/flags, <=3 returned BOOL/error/PC/SP/FP/return-PC/CONTEXT PC/SP steps,
   and <=24 function-table/module callback events. Extra event count is explicit.
   Addresses describe this process's owned code/stack only; no memory byte dump.
   The default memory reader remains NULL, not a new invasive process reader.
6. Direct RtlVirtualUnwind outputs are separately recorded: table/base, exact
   restored PC/SP/nonvolatiles and expected PC/SP. Table-less cases are explicitly
   unavailable for this direct API, not silently passed or given fake metadata.
7. Environment remains native-vs-Wine detection, plus loaded DbgHelp file version
   resource (explicit known flag) and API version. File version is not assumed
   from the API version or system filename. No system DLL is replaced.

Legacy single-range restore failure remains executable and rejects32 assertions.
No observer thunk logic/restore layout was weakened; added code is measurement
and an owned call-return control. Production source/config/packages unchanged.

## Actual tests / handoff

Commands and exact results are in CODEX_RESULT and stackwalk-v2-validation.json.
Fixed201 Wine assertions PASS; legacy169 PASS/32 expected FAIL (regression PASS);
4 new diagnostic coverage/accounting tests and6 runner tests PASS. Runner tests
include a **synthetic** native146/55 result that must stay FAIL and retain JSON;
it is a classification regression, never measured native evidence.
Wine comparisons: all4 configurations pass55/55 in fixed code; legacy configurations
retain the16 StackWalk failures (plus16 direct-unwind failures). Therefore cloud
cannot discriminate native root cause. Native revision2 and real PowerShell5.1
remain UNAVAILABLE. Unchanged whole-runtime regressions/build/package were not
rerun, as CODEX_NEXT explicitly requests fixture-focused validation only.

The updated runnable EXE and exact source manifest are published in the same
native-probes/presence directory. ONE owner invocation in CODEX_RESULT stages
this revised artifact and returns WS-presence-owned JSON with diagnostic_revision2.
This is a new instrumented experiment, not an unchanged rerun or file audit.
No Update-WS, compiler, game build/deploy/launch/attachment/EXE upload required.

Next review: first unhooked original+0 trace; distinguish FALSE/no progress from
TRUE returning wrong PC or SP; inspect each subsequent step and callback base/
function-entry lookup. Compare captured/RBP/actual-return experiments separately,
and compare independent VirtualUnwind. Fix only a demonstrated misuse/metadata
error; rerun unchanged strict predicates before considering Request-only runtime
integration. If all variants still fail, use their concrete native failure details
rather than another general scanner or accepting Wine as a production certificate.
