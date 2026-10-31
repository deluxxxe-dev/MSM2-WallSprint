# Codex result — presence-hook admission audit BLOCKED, no runtime observer

2026-10-31 JST; branch `codex/v01c20-edi-value-provenance`.
Baseline `fc55924f01d730bddf0efb3346166befc6cda895`, fetched/fast-forwarded
from028b24b. No main merge. Five pre-existing untracked archives/bundle preserved.

## STATUS — explicit incomplete runtime result

**RUNTIME_PRESENCE_OBSERVER_NOT_IMPLEMENTED / ADMISSION_AUDIT_PARTIAL_BLOCKED.**
Owner native contract155119 accepted. No discovery, canonical/relocation/name or
generic MAY contract acquisition repeated. Request/optional gravity-owner sites
have accepted identities, but actual native unwind metadata and new-site
trampoline/unwind/continuation transparency are not supplied in the summary.
The task explicitly requires independent hook admission, not just hash/pattern.
Cannot truthfully claim the requested runtime diagnostic exists or was tested.

Concrete audit: Request minimum MOV prefix7bytes, owner5bytes are whole instruction
candidates; no RSP adjustment, original nonvolatile home-slot store retained.
Real vendored HDE64 authored fixture confirms lengths under Wine. Existing MinHook
copies >=5 then tail-jumps, but actual new-site generated trampoline/Windows unwind
not executed. Existing WR nativeSpan validates entry/range only. Old WS_THUNK kind/
dispatch can call writer/lifecycle callbacks and old drain requires WR; not suitable
unchanged for ground-only observation. Old module pin/retained trampolines preserve
code residency, do not alone prove new phase/ring/disable/quiescence behavior.

C52 covers VCH/DR/final-CONTEXT integration, not automatically every inline entry
hook. No fabricated blanket C52 blocker or bypass. Missing dynamic unwind table
alone also does not prove failure for this RSP-preserving prefix. Exact missing
guarantee is NEW SITE prologue/unwind/trampoline/continuation transparency plus
new observer forwarding/cleanup proof. Full current UNWIND_INFO is needed first;
actual original EXE absent in cloud. No new live access/write/hook performed.

## CHANGED / concrete safe next evidence

* `analysis/PRESENCE_HOOK_ADMISSION_AUDIT.md`: specific source/whole-instruction/
  unwind/trampoline/lifetime/admission audit, proof limits, C52 applicability,
  remaining blocker and ONE focused offline hook-site audit operation.
* `source/tools/audit_presence_hook_sites.py`, `test_audit_presence_hook_sites.py`:
  exact SHA/TWO accepted full hashes/ranges, first whole instruction span/effects,
  actual .pdata-selected UNWIND_INFO/version/prologue/codes/frame/handler/chain
  metadata/hash, no raw body/code export. Always hook_admitted=false and
  observer_implemented=false. This is NOT a tested runtime diagnostic.
* `source/tools/presence_hook_hde_fixture.cpp`: authored minimal prefix decoder
  fixture, existing vendored library, no native-game execution or new binary in Git.
* Windows `Audit-Presence-Hooks.ps1` / `test_presence_hook_audit.ps1`: fresh4 owned
  Python dependencies, no discovery profile, existing WSL Capstone preflight,
  collision-safe private JSON;3 mock groups.
* PROJECT_STATE prepended accepted contract/new audit BLOCKED status; this result.
  CODEX_NEXT unchanged. All runtime/config/package/version/C52/shared tools/SF
  unchanged; r7/package0.1.63 remains. No new install ZIP or .script/version/log.

Output `logs/MSM2-WS-presence-hook-audit-YYYYMMDD-HHMMSS.json`, optional suffix.
Native prefix/unwind data stays compact metadata, no EXE/full function upload.
No Update-WS/Install/game/F8/ON-OFF/arming/restoration instructions: no new observer
or INI option exists. Actual phase presence/counters/zero-hit summaries unavailable.

## Exact checks / expected / actual

1. `/workspace/toolchains/analysis-python/bin/python3 source/tools/test_audit_presence_hook_sites.py`
   Expected authored request7/owner5 prefix parsing, UNWIND_SAVE offset decode,
   version/op/slot/order/prologue/runtime/hash refusal, unsupported prefix never
   admits, no body/bytes, exclusive CLI output. Actual **9 PASS**. Fixtures prove
   audit/parser only, not game hook safety or identity certification.
2. `/workspace/toolchains/analysis-python/bin/python3 source/tools/test_resolve_motion_gravity.py`
   Expected reused PE/CFG primitives unchanged; actual **15 PASS**. No owner scan.
3. `/workspace/toolchains/analysis-python/bin/python3 source/tools/test_verify_motion_gravity.py`
   Expected inherited identity/PE dependencies unchanged; actual **9 PASS**.
4. `python3 source/tools/test_discover_ground_support.py`
   Expected shared strict PE/range/hash helper unchanged; actual **18 PASS**.
5. `XDG_CACHE_HOME=/tmp/ws-ground-pwsh/cache XDG_CONFIG_HOME=/tmp/ws-ground-pwsh/config XDG_DATA_HOME=/tmp/ws-ground-pwsh/data /tmp/ws-proxy-pwsh/pwsh -NoProfile -File source/tools/windows/test_presence_hook_audit.ps1`
   Expected fresh4 dependencies/no profile, Japanese/space paths, collision/old
   output/dirty source/fake EXE preserved, unavailable WSL/decoder refusal;
   actual **3 PASS mock groups**, no actual Windows/game audit result inferred.
6. `/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix -static -I source/vendor/minhook/src/hde source/tools/presence_hook_hde_fixture.cpp source/vendor/minhook/src/hde/hde64.c -o /tmp/ws-presence-audit/hde_prefix.exe`
   then `WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 /tmp/ws-presence-audit/hde_prefix.exe`
   Expected existing HDE64 lengths3+4/5 without errors. Actual **1 PASS group**,
   exit0. Wine missing winedevice warnings did not prevent fixture PASS. Not actual
   MinHook relocation/native EH/ABI forwarding/counter code; no reference DLL run.
7. `git diff --check`: expected whitespace clean; actual PASS.
8. `python3 /tmp/ws-presence-scope.py`: expected only8 intended offline audit/tests/
   docs paths, runtime/task/config/package preserved and actual Bypass snippet;
   actual PASS, snippet extracted for parser.
9. `XDG_CACHE_HOME=/tmp/ws-ground-pwsh/cache XDG_CONFIG_HOME=/tmp/ws-ground-pwsh/config XDG_DATA_HOME=/tmp/ws-ground-pwsh/data /tmp/ws-proxy-pwsh/pwsh -NoProfile -Command '$t=$null; $e=$null; [void][System.Management.Automation.Language.Parser]::ParseFile("/tmp/ws-presence-owner.ps1",[ref]$t,[ref]$e); if($e.Count){throw ($e|Out-String)}; "PASS presence audit bootstrap syntax"'`
   Expected new command syntax valid; actual PASS parser, not owner execution.

**Focused checks PASS55 / SKIP1 / UNAVAILABLE1 / BLOCKED1.** Separate whitespace/
scope/parser/publication checks not included in fixture PASS counts.
SKIP1 = full runtime build/package/release suites: no runtime diff/release,
only offline audit helpers and one owned decoder fixture.
UNAVAILABLE1 = actual current native .xdata/unwind audit, original EXE absent here.
BLOCKED1 = requested runtime observer and its original-forwarding/zero-effect/
queue cap/thread race/F8/ground arming/zero-hit/phase/unload tests: unimplemented
pending independent new-site hook admission. Not converted to synthetic PASS.
No invented diagnostic version/startup line/admission/counter result.

## Source audit / shared knowledge / publication

Inspected `wallrun_hooks.cpp::nativeSpan/start/stop`, `hook_thunks.S::WS_THUNK`,
MinHook `trampoline.c::CreateTrampolineFunction`, hook.c freeze/IP mapping and
analysis/V01C52_GATE.md. Search RtlAddFunctionTable/unwind/XSTATE/dispatch revealed
no new-site admission guarantee. No unrelated publisher/DR scan. Full audit table
and precise caveats in analysis, including why C52 is not blanket inline veto.

Wisdoms audit-boundary/source facts pushed
`91a6ec6c0d30a74d2b467236cb81294f1e886c98`; ls-remote agrees. WS audit commit and
independent publication readback follow below.

## NEED

ONE fresh offline audit block from analysis/final, return new presence-hook-audit
JSON. No unchanged contract/r7 game test, runtime update, phase test or new config.
Then resolve actual prologue/unwind agreement and prove new-site relocation/
forwarding/continuation on owned native fixtures before implementing/releasing
observation. The task's runtime goal remains BLOCKED, not declared completed.

### Verified audit publication

Audit/tool commit `de40cb44accd6fee49159e5635ff26c48db37033` pushed on task branch.
`python3 /tmp/ws-presence-publication.py`: expected independent GitHub fetch
FETCH_HEAD/local HEAD/ls-remote equality and exact8-file bytes againstfc55924;
actual PASS atde40cb4. Record-only commit follows, then same readback repeated.
Unchanged audit tests need no rerun for this docs-only record. Publication PASS
is not runtime diagnostic completion/admission. Final branch/record SHA in handoff.
