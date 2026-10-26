# c42 — per-bit boundary for the owned continuation probe

Base7ad4e55, codex/v01c20-edi-value-provenance. Read current AGENTS/manual/rules/
workspace/state/reporting/NEXT, c39-c41 evidence, current probe/helper/tests and
relevant Wisdoms references. No main merge. Only test/probe admission and its
auditable reporting change; no production watch or mitigation setting changes.

## Supplied native result versus this task's evidence

**VERIFIED-RUNTIME, owner standalone Windows:** original c41 at ded0ce0 returned
REFUSED with all3 queries known and raw flags[0,0,256]. The old any-nonzero rule
stopped before the witness. This is neither a native witness PASS nor FAIL.
`V01C41_NATIVE_WINDOWS_RESULT.md`, the c41 binary and manifest remain preserved.

**VERIFIED-DOCUMENTATION / checked SDK:**0x100 in UserShadowStackPolicy is
CetDynamicApisOutOfProcOnly. It restricts calls to2 named CET dynamic APIs from
inside the target process; it does not itself enable HSP or IP validation.
The exact candidate calls neither API and does not rely on their registration.
This justifies eligibility under this bit, not native witness correctness.

**OWNED-SYNTHETIC:** pure classifier tests admit[0,0,0x100] while rejecting every
relevant/unknown/reserved enabled bit and failed query. These are DWORD/parser
tests, never substituted into GetProcessMitigationPolicy in the actual probe.
**OWNED-WINE:** existing witness/state/DR/lifetime tests still execute with actual
Wine-returned flags. Native execution of the refreshed probe remains UNKNOWN.

## Documentation and bit-layout sources

Canonical MicrosoftDocs/sdk-api, docs branch, sdk-api-src/content:

- winnt/ns-winnt-process_mitigation_dynamic_code_policy.md
- winnt/ns-winnt-process_mitigation_control_flow_guard_policy.md
- winnt/ns-winnt-process_mitigation_user_shadow_stack_policy.md
- processthreadsapi/nf-processthreadsapi-setprocessdynamicehcontinuationtargets.md
- processthreadsapi/nf-processthreadsapi-setprocessdynamicenforcedcetcompatibleranges.md

These were read from raw.githubusercontent.com/MicrosoftDocs/sdk-api/docs/.
Checked layout: microsoft/win32metadata, main,
generation/WinSDK/RecompiledIdlHeaders/um/winnt.h, downloaded SHA256
`d66df443218279ed2ffe7718b5347e64bf712dbfa5d25a3b0692787169405c69`.
All19 named mask/name pairs were compared to this SDK's sequential1bit fields.
The local MinGW header lacks some new Audit/XFG fields, so it is not treated as
proof those newer positions are harmless. No SDK function bodies are vendored.

RelevantRefuse means a documented setting governs a path the full probe uses
and compatibility has not been established; it does NOT assert observed failure
under that setting. UnknownRefuse also fails closed. Only documented unused-path
permissions/restrictions are Irrelevant. Decisions apply solely to this exact
candidate/dependency review and must be re-reviewed if its APIs change.

## Per-bit decision table

| Policy | Mask | Checked field | Classification | Exact-candidate reason |
| --- | --- | --- | --- | --- |
| DynamicCode | 0x1 | ProhibitDynamicCode | RELEVANT_REFUSE | Own generated code must transition RW to RX |
| DynamicCode | 0x2 | AllowThreadOptOut | IRRELEVANT_TO_C41_CANDIDATE | Permits opt-out via SetThreadInformation/ThreadDynamicCodePolicy; candidate never calls it and does not opt out |
| DynamicCode | 0x4 | AllowRemoteDowngrade | UNKNOWN_REFUSE | Cross-process policy transition contract is outside the established fixture |
| DynamicCode | 0x8 | AuditProhibitDynamicCode | UNKNOWN_REFUSE | Named SDK field; canonical structure documentation gives no sufficient semantics |
| DynamicCode | 0xFFFFFFF0 | reserved/unknown | UNKNOWN_REFUSE | No supported contract |
| ControlFlowGuard | 0x1 | EnableControlFlowGuard | RELEVANT_REFUSE | Full probe uses indirect callbacks/API pointers; direct stub alone cannot certify the process path |
| ControlFlowGuard | 0x2 | EnableExportSuppression | RELEVANT_REFUSE | Governs exported indirect API targets used by the full path; complete compatibility unproved |
| ControlFlowGuard | 0x4 | StrictMode | RELEVANT_REFUSE | All loaded DLLs must support CFG; complete image set not certified |
| ControlFlowGuard | 0x8 | EnableXfg | UNKNOWN_REFUSE | SDK defines position, corresponding XFG contract not established |
| ControlFlowGuard | 0x10 | EnableXfgAuditMode | UNKNOWN_REFUSE | SDK defines position, audit/XFG contract not established |
| ControlFlowGuard | 0xFFFFFFE0 | reserved/unknown | UNKNOWN_REFUSE | No supported contract |
| UserShadowStack | 0x1 | EnableUserShadowStack | RELEVANT_REFUSE | HSP governs real returns/OS context compatibility, not just stub shape |
| UserShadowStack | 0x2 | AuditUserShadowStack | UNKNOWN_REFUSE | Audit requires an underlying HSP boundary that has not been proved |
| UserShadowStack | 0x4 | SetContextIpValidation | RELEVANT_REFUSE | Candidate redirects saved RIP; IP-validation compatibility unproved |
| UserShadowStack | 0x8 | AuditSetContextIpValidation | UNKNOWN_REFUSE | Audit does not certify the context-restoration boundary |
| UserShadowStack | 0x10 | EnableUserShadowStackStrictMode | RELEVANT_REFUSE | Strict hardware-return/context behavior unproved |
| UserShadowStack | 0x20 | BlockNonCetBinaries | RELEVANT_REFUSE | No complete CET-compatible image contract |
| UserShadowStack | 0x40 | BlockNonCetBinariesNonEhcont | RELEVANT_REFUSE | No complete EH-continuation image metadata contract |
| UserShadowStack | 0x80 | AuditBlockNonCetBinaries | UNKNOWN_REFUSE | Audit is not a compatibility certificate for all loaded images |
| UserShadowStack | 0x100 | CetDynamicApisOutOfProcOnly | IRRELEVANT_TO_C41_CANDIDATE | Solely restricts2 unused CET dynamic registration APIs; does not govern the APIs actually used |
| UserShadowStack | 0x200 | SetContextIpValidationRelaxedMode | UNKNOWN_REFUSE | Relaxed validation still requires a context execution contract; never erases relevant validation bits |
| UserShadowStack | 0xFFFFFC00 | reserved/unknown | UNKNOWN_REFUSE | No supported contract |

Known masks are0xF/0x1F/0x3FF; irrelevant masks0x2/0/0x100; relevant masks
0x1/0x7/0x75. The rest is UnknownRefuse, including named fields with insufficient
semantic evidence. Mixed bits retain each class mask: irrelevant0x100 cannot
hide HSP, validation, another policy's prohibition or any high reserved bit.
Failed/unknown queries refuse on **both native Windows and Wine**, stricter than
the old Wine-only unknown-query allowance. No raw flags are cleared/rewritten.

## Exact dependency review

| Dependency | Actual use / evidence | Consequence |
| --- | --- | --- |
| SetProcessDynamicEHContinuationTargets | No source call, dynamic lookup string or import | 0x100's restriction on this API is unused |
| SetProcessDynamicEnforcedCetCompatibleRanges | No source call, dynamic lookup string or import | Same; no CET range registration required by this candidate under admitted settings |
| SetThreadContext | Actual import; debug-register setup/readback/restore in owned watch and fixture | Not a direct RIP-launch API in this candidate; not ignored when reviewing context-policy effects |
| RtlRestoreContext | No explicit source call/import; canonical EH API docs state it participates in SEH | OS return path can depend on it; absence of an import is NOT absence of OS dependency |
| Saved ContextRecord.Rip | Actual test-owned redirection | Reason IP-validation settings remain refused |
| Indirect CALL/JMP or RET in generated stub | None, literal/current decoder proof preserved | Does not certify the rest of the executable, which has ordinary calls/returns/callbacks |
| CFG registration | No explicit SetProcessValidCallTargets or equivalent registration | Default executable-page behavior is not an enabled-CFG certificate |
| CET-compatible / EH-continuation image metadata | Not supplied/certified; observed PE Debug/LoadConfig directories0 | Win64 .pdata/.seh unwind metadata is not /CETCOMPAT or /guard:ehcont proof |
| Dynamic-code opt-out / mitigation setter | No SetThreadInformation(ThreadDynamicCodePolicy), SetProcessMitigationPolicy or policy mutation | AllowThreadOptOut is unused permission; no protection is disabled/relaxed |

Source review covers the owned probe, unchanged watch/helper, included linked
owned helpers and MinHook. PE import review confirms GetProcAddress and
SetThreadContext, no restricted CET APIs or explicit RtlRestoreContext. Dynamic
lookups in the probe are wine_get_version and GetProcessMitigationPolicy. This
is a bounded dependency review, not a claim about all internal OS implementation.

## Implementation, reporting and validation

`source/include/NativeHooks/continuation_mitigations.hpp` is a pure classifier
used only by the standalone test/probe. Every policy reports known/raw flags,
ADMIT/REFUSE/QUERY_UNKNOWN_REFUSE, separate class masks and enabled-bit names,
classes/reasons. Query-unknown raw data is retained but not interpreted as a
known bit contract. Unknown policies/bit indexes refuse.

The actual executable obtains all3 real query results and applies this helper;
it has no flag-injection option or API override. JSON schema2/taskc42/witness_task
c41 identifies the new binary. Success, REFUSED and FAIL all contain classifier
interpretation; actual query data is not replaced with the synthetic tuple.
`native_candidate_probe_passed` replaces the old "unmitigated" label because an
admitted irrelevant permission/restriction can be nonzero. Production remains
unintegrated. Internal --cap cases preserve FAIL_CLOSED_EXPECTED separately.

`bash source/tools/test_continuation_mitigations.sh` runs portable sanitizer/
Werror pure tests plus strict Python JSON parsing:10 PASS groups. It tests zero,
the observed synthetic tuple, all96 individual bits, known combinations, every
reserved position, query-unknown regardless of raw values, invalid policy/bit,
and explicitly synthetic admitted/mixed/unknown JSON fixtures. No classifier
fixture is supplied to native GetProcessMitigationPolicy. It is also mandatory
in test_hooks_windows.sh; focused counts overlap the full total.

All required commands/counts, unchanged c41/c39/c40 semantics and refreshed
artifact provenance are in CODEX_RESULT.md / V01C42_STATUS.json. The c41 harness,
ledger/context/identity/state/restoration/cap/lifetime code and production watch
are unchanged. No original deadlines/caps/assertions are weakened.

## Native follow-up and scope

Only after the required tests pass is the refreshed artifact placed in
analysis/native-probes/c42, with source/input/binary hashes and notices. The c41
artifact and its accepted native refusal are retained. The next Windows command
from the c42 probe directory remains a single standalone, game-free invocation:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Expected if the same supported flags still apply: classifier admits, and the
actual owned witness runs. Its outcome may legitimately be OWNED_WITNESS_PASS,
REFUSED or FAIL; **none is predeclared from the classifier tests**. One additional
owner native run is required to answer that OS-level question after publication.
No game/Overstrike/Update-WS/WallRun/log/slot scan, setting changes, production
integration, slot reads/writes, producer traversal or new game release. Runtime
v0.1c38/package0.1.56 and all prior proof/gameplay-write gates remain unchanged.
