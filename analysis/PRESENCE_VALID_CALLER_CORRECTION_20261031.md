# Corrected primary CALL-return oracle — revision3

## Result and accepted evidence

**CORRECTED_PRIMARY_VALID_CALL_RETURN / OWNED_WINE_PASS /
NEW_BINARY_NATIVE_ACCEPTANCE_PENDING. Production=false.**

Baseline100a1e32259cd64290755ae66573e35c66336ab2, branch
codex/v01c20-edi-value-provenance. No main merge or runtime/config/game change.
Read accepted PRESENCE_WINDOWS_V2_ORACLE_RESULT_20261031.md: actual Windows v2
invalid-entry primary146 PASS/55 FAIL; genuine-CALL-return diagnostic55/55 PASS.
All invalid-entry caller PCs were expected-1 with exact SP; context RIP retained
expected PC. Captured-context/RBP-only changes0/55. This demonstrates a synthetic
caller-oracle setup problem. It does not prove an undocumented DbgHelp internal
cause or a Windows bug; do not generalize the observed -1 or add tolerance.

## Correction, not more instrumentation

`unwind()` now installs `presence_unwind_return` in synthetic [entry], and requires
that SAME exact PC and entry+8 SP in the primary StackWalk predicate. Direct
RtlVirtualUnwind uses the same return slot and expected PC/SP. All55 existing
StackWalk,48 direct unwind and55 table-presence assertions remain; no deleted
failure, OR variants, PC±1 adjustment, mask, skip or weaker nonvolatile condition.

The caller is unchanged WS-owned assembly: real SUB RSP,40, CALL owned stub,
exported after-call label, ADD RSP,40, RET, with existing version1 allocation
metadata. Four new hard prerequisites verify exact RUNTIME_FUNCTION containment,
actual E8 CALL target/end address, real40byte unwind metadata and execution of the
actual CALL/return once with original return value. This is not merely assigning
a new nominal symbol. Call bytes are read via the runtime function-table address;
C++ arithmetic across separately declared assembly function labels is avoided.

Synthetic callee entry E: return atE; saved home slotsE+8/+16/+24 remain in caller
shadow space. Caller RSP at return isE+8; its40byte allocation spansE+8..E+47 and
its own outer return slotE+48 is explicitly zero to terminate further walking.
Backing array bounds remain checked. Both APIs see this identical valid setup.
The outer slot was already zero in v2's zero-initialized array; this makes the
caller layout explicit, not a new padding/tolerance rule.

The old invalid function-ENTRY sentinel is retained in a separately labeled
`invalid_function_entry_sentinel_diagnostic` trace. It cannot alter primary result.
No more captured-context/frame permutations: two traces per position,110 total.
Native Windows may reject that diagnostic while Wine accepts it; neither fact
is a requirement for the valid-caller primary to pass.

## Equivalence to supplied native success / limits

Revision3 primary is semantically identical to v2's supplied
`baseline_context_genuine_call_return` path: same zero-initialized CONTEXT/test
registers, frame=RSP, same genuine assembly return label and caller metadata,
same synthetic stack/home slots, same55 sampled positions, callbacks,3-call cap
and exact PC/SP equality. No assembly or production source changed this task.
Direct unwind now matches that same valid caller instead of the invalid sentinel.
Already supplied55/55 is positive native evidence for that equivalent owned path.

However revision3 binary/source layout and four new prerequisite assertions have
not been executed on actual Windows in cloud; Windows behavior of the corrected
legacy regression is not newly measured either. Therefore do not label the entire
new executable native PASS or issue production admission. ONE corrected binary
acceptance run is delivered, not another permutation/instrumentation-only loop.

## Regression proof

Fixed:205 Wine PASS/0 FAIL (original201 assertions plus4 genuine-caller checks).
Legacy with corrected valid caller:173 PASS/32 expected FAIL. Inspected exact
identities: both sites,restore steps1..8,each StackWalk caller and direct
VirtualUnwind stack/nonvolatile assertion. This is the SAME16+16 failures as
before, now under valid caller setup. No changed count was substituted to conceal
bad metadata. Shell and Python tests now require the exact failure-name set,
not just any32 failures. Primary caller restoration failures remain effective.

Four diagnostic unittest methods verify valid-primary linkage, two-trace coverage,
controls before MinHook, bounded frames/callbacks, same expectations in both APIs
and exact unsafe-restore rejection. Six runner methods verify real Git/hash/Unicode
staging/cleanup/refusal and a synthetic55-failure native report remains FAIL.
Synthetic classification is not measured native evidence.

Exact executed commands/results are in CODEX_RESULT. No unchanged full-runtime
pipeline: fixture-only task explicitly excludes it. Runtime observer unintegrated;
v0.1c38r7/package0.1.63 unchanged; C52/slot/gameplay/ownership/SF/shared environment
unchanged. Old v1/v2 manifests/evidence/hashes retained; current EXE+source manifest
is revision3. No proprietary full bodies or raw private logs published.

Owner: one standalone fetch/run from CODEX_RESULT, no game/Update-WS/build/deploy.
Return JSON with diagnostic_revision3. Expected valid primary55/55 and full205/0,
production=false. If a genuine primary fails, retain exact failure and stop; do
not infer native acceptance from Wine or the invalid diagnostic. After accepted
corrected native result, next task may resume Request-only observer work, without
another EXE capture/name scan or optional-Gravity dependency.
