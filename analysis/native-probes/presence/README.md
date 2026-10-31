# Current revision3 — CORRECTED valid CALL-return primary

Native v2 supplied genuine-CALL-return55/55 PASS while invalid-entry primary failed55
(PC=expected-1, exact SP). Revision3 now uses that proven valid setup as PRIMARY
for both StackWalk and VirtualUnwind, retaining exact expected PC/SP/nonvolatiles.
This is a correction, not another context instrumentation loop. No ±1 tolerance,
OR, skipped assertion or production admission. Four new prerequisites verify
actual CALL bytes/target/end, caller RUNTIME_FUNCTION/unwind and real execution.

Fixed205 Wine PASS; legacy173 PASS/32 expected failures under VALID caller. Exact
failure set:2 sites*8 restore positions*2 APIs, unchanged. Two traces/sample now:
primary_valid_call_return and invalid_function_entry_sentinel_diagnostic (110).
Invalid diagnostic success/failure cannot alter valid-primary result. Direct API
and primary use same return slot/expected PC/SP. Caller frameE+8..E+47,outer return
slotE+48=0; no fictitious nominal return symbol. See
analysis/PRESENCE_VALID_CALLER_CORRECTION_20261031.md and CODEX_RESULT.

Run same focused commands below; diagnostic unittest remains4 methods, runner6.
Current EXE/manifest are revision3, exact source hashes; manifest-v2.json archives
previous c6fc4fff... binary identity (binary preserved in Git e402c7c). v1/v2 Wine
JSONs and validation summaries remain historical. New summary:
valid-caller-v3-validation.json. New binary native acceptance remains UNAVAILABLE;
supplied v2 equivalent path55/55 is positive native evidence but not all-v3 PASS.
ONE corrected acceptance invocation, no game/build/deploy/Update-WS; return JSON
with diagnostic_revision3. production_admitted=false. Current runtime unchanged.

---

Historical revision2 specification follows; its permutations/counts are not v3:

# Current revision2 — native StackWalk failure instrumentation

Predecessor native Windows result is **146 PASS/55 FAIL**, including unhooked
controls. Cause remains UNKNOWN; Wine201 PASS does not override this result.
Current EXE is diagnostic_revision2, instrumented rather than a proven native fix.
See analysis/PRESENCE_STACKWALK_DIAGNOSTICS_20261031.md and CODEX_RESULT.

All55 baseline StackWalk assertions and exact caller PC/SP equality remain.
Unhooked controls execute before MinHook initialization. Four traces per sample
(220 total) independently compare baseline setup, captured-context seeding,
RBP frame initialization and a genuine OWNED CALL-return sentinel. Alternate
trace success never makes baseline PASS. Each trace logs initial/expected/returned
addresses, thread/backing stack bounds, <=3 BOOL/error/frame steps and <=24 lookup/
module callback events. Error is cleared before the API; it is not assumed to
be authoritative. Direct VirtualUnwind results and DbgHelp file/API versions
are separate. Default memory reader remains NULL with valid current-process handle.
No arbitrary memory bytes are dumped, only owned addresses/register metadata.

Focused validation adds `python3 source/tools/test_presence_stackwalk_diagnostics.py`
(4 tests); runner now has6 tests, including synthetic native146/55 FAIL retention
(not measured Windows). Fixed201 Wine PASS and exact32 legacy failures remain.
Old wine_fixed.json, wine_legacy_counterexample.json, validation.json and
manifest-v1.json describe revision1 and are retained as historical evidence.
New summarized results: stackwalk-v2-validation.json. Current manifest.json and
presence-owned.exe describe revision2. Original binary remains in Git7439b04.
New EXE/source hashes are checked together; no runtime/package/config change.
Owner invocation is unchanged in form but stages the NEW instrumented binary.
No game/Update-WS; return JSON with diagnostic_revision2. Native v2 is UNAVAILABLE
until owner execution/review; production_admitted always false.

---

# WS-owned presence-hook fixture v1

This standalone EXE runs only independently authored fixture functions in its own
process. No game discovery, game file read, attachment, injection, game ABI call,
production observer, game data write, or shared SF/Overstrike change.

The owned Request/Gravity prologues reproduce accepted *effects and unwind
layout*, not proprietary full function bodies. Real vendored MinHook creates and
executes each trampoline/relay. Existing WS-owned probe assembly supplies fixed
GPR/stack arguments, flags and all YMM0–15 lanes. A separate leaf oracle exposes
entry flags/R11 overwritten by the native-shaped prologues. An observer captures
opaque RCX/RDX/R8/R9, return PC, thread ID, raw TSC and site kind into 16 records.
It never dereferences arguments or calls C++/I/O/allocators/mutexes in the hook.
One CAS attempt reserves a record; contention/full capacity count as suppressed.
Records are consumed **after joining fixture threads**, not by a production worker.

`fixture.S` has a reproducible legacy single-unwind-range counterexample. Its
volatile-pop/POPFQ restore sequence is not a Windows canonical epilogue: after
RSP advances, the original allocation metadata restores the wrong frame. The
proposed safer fixture splits each restore step into its own unwind range with
the correct residual allocation (including zero at final tail). This experimental
metadata must pass actual Windows APIs before any production use.

`StackWalk64` uses the real `RtlLookupFunctionEntry` callback and actual allocation
base (not DbgHelp module-only lookup, which cannot identify MinHook allocations).
Each context/stack is fabricated from owned known effects. Checks cover original
prologue instruction boundaries, original continuations +7/+5, actual trampolines
(start/intermediate instruction/absolute tail), actual relays, observer push
boundaries and **every restore/tail boundary**. Table-backed frames additionally
call `RtlVirtualUnwind` and require exact restored RSP/RIP/nonvolatiles. Table
presence/absence is asserted, not silently skipped. No fake runtime table is
registered to make MinHook appear unwindable. Home-slot saves use their actual
*delayed* offset23/15 metadata. Pre-full-prologue registers remain live; completed
prologue tests clobber the registers and require home-slot restoration.

Important limits: synthetic stack states through real APIs are not asynchronous
exception delivery or a proof of every game-body state, CET/mitigation policy,
thread-suspension lifetime, saved-CONTEXT retirement, no-return exception cleanup,
AVX-512/XSTATE hardware coverage or safe mapping removal. The observer uses no
SIMD instructions; measured register coverage is all 256-bit YMM0–15. No general
unwind/ownership certificate follows. Disabled trampolines/module storage are
retained until process exit; code is not removed/freed. Thread-close tests wait
for fixture joins before reading/resetting records. No production two-second
phase/F8/drain implementation is claimed.

## Reproduce (cloud, existing toolchains)

```bash
bash source/tools/test_presence_native.sh
bash source/tools/test_presence_native.sh legacy-counterexample
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' python3 source/tools/test_run_presence_owned.py
```

Fixed:201 assertion PASS,0 FAIL under Wine. Native-Windows group UNAVAILABLE.
Legacy:169 PASS,32 expected restoration FAIL; shell regression PASS only when
exactly those32 failures occur. Runner:5 unittest PASS including real byte-for-byte
Git extraction, SHA/size checks, Wine EXE invocation with Unicode output pathname,
temporary cleanup and wrong origin/branch/hash/missing-logs refusal.

## Windows owner operation

Use the single fetch-and-stage command in `docs/CODEX_RESULT.md`. It stages the
PowerShell runner from the fetched SHA, uses Windows `py -3` to extract the EXE
**as bytes**, checks this manifest, runs outside Mods Library, and returns only
`MSM2-WS/logs/MSM2-WS-presence-owned-<unique>.json`. No WSL/compiler/Update-WS/game
needed. Existing checkout, active script and settings stay unchanged; only Git
fetch refs and one new report are written. The EXE is deleted from its own temp
folder after exit. Run may produce FAIL/UNAVAILABLE; return that JSON unchanged.
The real PowerShell5.1/native Windows launch has not run in cloud.

The JSON explicitly distinguishes Wine/native Windows and always reports
`production_admitted=false`. Even native fixture PASS needs review before a
Request-only runtime observer is implemented. Optional Gravity proof must not
block a separately admitted Request diagnostic.

Manifest records exact EXE hash/size and build source hashes. MinHook license is
in `source/vendor/minhook/LICENSE.txt`; static MinGW runtime uses its upstream
runtime exception. No third-party mod/game binary is included.
