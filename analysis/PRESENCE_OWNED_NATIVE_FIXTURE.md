# Presence hook: actual OWNED MinHook fixture — 2026-10-31

## Result / boundary

**OWNED_FIXTURE_WINE_PASS / NATIVE_WINDOWS_UNAVAILABLE /
RUNTIME_PRESENCE_OBSERVER_NOT_IMPLEMENTED.** This is a concrete executable
handoff, not another native-byte/JSON-parser acquisition. Owner audit161445 is
accepted and all original entry/unwind inputs are now resolved. Prior statements
that native unwind metadata is missing are historical and superseded.

Baseline `3ea14b28cdd7bddb0f41ccd6659e48bc5d8a6a16`;
branch `codex/v01c20-edi-value-provenance`. No main merge. Runtime remains
v0.1c38r7/package0.1.63. No production hook/restore/handler/ownership/lease/cleanup,
exact-slot watch, game/Position/gameplay write, config, SF or Overstrike changes.

## Concrete implementation/evidence

`source/tests/presence/fixture.S/.cpp` independently author Request/Gravity-shaped
prologues and unwind layouts from accepted effects, without game bodies. Real
vendored MH_CreateHook/MH_EnableHook produces/exercises two original forwarding
trampolines. Assertions independently inspect actual patched rel32 entry, absolute
relay destination/displacement, copied whole instructions via actual HDE64,
absolute continuation destination/displacement and observer indirect tail slot.
Request steals7bytes (3+4), Gravity5bytes. Optional Gravity failure is independently
identifiable; no shared production admission is made.

Request prologue23, push16, allocation1120 and delayed SAVE_NONVOL at23 with
RBX1136/RBP1144/RSI1152; Gravity prologue15, push11, allocation48, delayed saves
RBX64/RSI72. Actual version/flags/code slots/frame metadata is checked. Table-backed
unwind tests cover all instruction-boundary offsets through those prologues,
including continuations +7/+5. Tests retain live nonvolatiles before delayed saves
and force restoration from home slots after full prologue. Gravity's RSI home
slot is entry+16, not Request's entry+24. An initial fixture-oracle mapping error
was corrected; no native layout was changed to fit an erroneous oracle.

Original/hooked comparisons execute both paths and compare volatile arguments,
return PC/stack arguments, flags, all YMM0–15 lanes and nonvolatiles/returns.
A separate WS-owned leaf oracle additionally exposes entry R11 and flags that
the native-shaped MOV/SUB would overwrite and otherwise hide from comparison.

Proposed TEST-ONLY observer preserves flags/volatile GPRs, uses no SIMD/vector
instructions and never calls C++ from the hook. It records only opaque registers,
return PC, thread identity and raw TSC into16 static slots. One CAS attempt,
no retries/hot file I/O/allocation/mutex. Eight threads exercise800 invocations;
16 records +784 suppressed, all800 forwarded exactly once. Concurrent close
forwards all8000 calls and preserves capacity. Writers are joined before fixture
reads/reset; this is not an implemented production publication/drain protocol.
No pointers are dereferenced, no WR lease is needed to run this fixture; empty
capture is explicit. Code/trampolines remain resident to process exit, not removed
or freed on disable. This does not certify reclamation or arbitrary saved contexts.

## Actual failing design and safer alternative

A single unwind range spanning pushfq/volatile pushes and the reverse pop/popfq
sequence **fails32 restore assertions** (16 positions, StackWalk64 and VirtualUnwind
at each) after RSP advances. The sequence is not a recognized canonical Windows
nonvolatile epilogue, so stale allocation metadata yields a wrong caller frame.
Normal function return and ABI snapshot PASS alone missed this issue.

Safer independently authored fixture: each restore step has its own unwind range
with exactly the residual stack allocation, including zero before tail jump.
Both APIs now recover exact caller RSP/RIP/nonvolatiles at every restore boundary
in Wine. The original failure remains reproducible with `legacy-counterexample`;
regression succeeds only for exactly32 failures confined to those restore checks.
No production unwind/gate was weakened or silently patched. Native Windows must
validate this experimental split-range metadata before production use.

An initial module-only DbgHelp function-table callback could not locate allocator
regions: that was not proof MinHook leaf unwinding fails. The final stack walker
uses actual RtlLookupFunctionEntry and VirtualQuery allocation base, not invented
dynamic table entries. Table presence/absence itself is asserted at each sample.
MinHook trampoline/relay bytes genuinely have no RUNTIME_FUNCTION in this fixture;
no-RSP-change instruction effects are exercised through actual StackWalk64.

## What exactly was measured

201 assertions PASS under **Wine**, zero unexpected FAIL. `StackWalk64` and
`RtlVirtualUnwind` run against actual generated code addresses but **fabricated
OWNED contexts/stacks** corresponding to known instruction effects. This is not
actual Windows kernel exception delivery, arbitrary asynchronous game sampling,
all game function-body states, all mitigations/CET, XSTATE/AVX-512 measurement or
an ownership/lifetime certificate. Fixture native Windows group remains
UNAVAILABLE. An actual Windows result is required, not a repeated file audit.

`analysis/native-probes/presence/` contains the real runnable EXE, exact build
input hashes, manifest, derived Wine assertion JSONs and detailed scope README.
No proprietary code or private game log is included. Native owner runs this EXE
only, not Spider-Man2. The result always has production_admitted=false, even if
all owned native checks pass. Next Codex must review native result, then implement
a Request-only observer if independently admitted; optional Gravity must not block
Request. Production two-second ground/WR phases, drains, startup summaries, F8 and
integration mismatch/cleanup tests are **not implemented or passed by this handoff**.

## Executed commands / expected and actual

| Exact command | Expected | Actual |
| --- | --- | --- |
| `bash source/tools/test_presence_native.sh` | Actual MinHook and preserving owned observer pass ABI/transfers/all sample unwinds/caps/races | exit0;201 PASS,0 FAIL; Wine; native group UNAVAILABLE |
| `bash source/tools/test_presence_native.sh legacy-counterexample` | Wrong restore metadata stays reproducibly rejected | exit0 regression;169 passing assertions, exactly32 expected failing restore assertions; native group UNAVAILABLE |
| `WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' python3 source/tools/test_run_presence_owned.py` | Byte-preserving real Git extraction, manifest refusal and actual owned execution including Unicode path | exit0;5 unittest PASS; nested fixture201 PASS; native remains UNAVAILABLE |
| `bash source/tools/build_windows.sh` | Existing current runtime builds without production changes | exit0 |
| `bash source/tools/test.sh` | Existing portable/ASan/UBSan regressions preserved | exit0 |
| `bash source/tools/smoke_windows.sh` | Existing synthetic Windows/Wine build-gate/log/stop regressions preserved | exit0 |
| `bash source/tools/test_hooks_windows.sh` | Existing hook/counterexample/static proof regressions preserved; native-only/private evidence not promoted | exit0;5 explicit private-replay SKIP markers,17 native/VCH UNAVAILABLE markers preserved |
| `git diff --check` | No whitespace errors | exit0 |

Distinct reporting units:7 main executable commands, focused201 assertions,
5 runner test methods and1 expected-failure regression. Do not sum nested
fixture201 again or count its expected32 failures as a new production regression.
Existing hook-suite explicit SKIP/UNAVAILABLE messages are separately recorded in
CODEX_RESULT. Native fixture and Windows PowerShell5.1 invocation are unavailable
in cloud (2 groups). Production integration/windows/cleanup gameplay acceptance
remains BLOCKED pending native review and subsequent implementation. New game
package/release is SKIP (no admitted runtime diff; packaging unchangedr7 would not
validate this owned EXE). No new game/Update-WS/Install step requested.
