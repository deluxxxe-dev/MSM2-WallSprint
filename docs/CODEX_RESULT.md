# Codex result — native StackWalk failure, instrumented owned fixture v2

2026-10-31; branch `codex/v01c20-edi-value-provenance`.
Baseline `24a438a225d89dc13528910863d212f5e236fcb3`, fetched/fast-forwarded.
No main merge; five old untracked archives/bundle preserved.
Implementation SHA/publication recorded after push.

## STATUS — partial investigation, production FAIL CLOSED

**INSTRUMENTED_FIXTURE_DELIVERED / ROOT_CAUSE_UNKNOWN.**
Accepted native predecessor:146 PASS/55 FAIL, all55 StackWalk caller assertions
including unhooked originals. This remains genuinely FAIL. It is not overturned
by201 Wine PASS or successful direct RtlVirtualUnwind/ABI checks. Native v2 has
not run in cloud. No production integration, runtime/config/package/gameplay/SF/
shared-environment/C52 change. Runtime remains v0.1c38r7/package0.1.63.

Completed authorized fallback: a NEW instrumented owned EXE and exact source/hash
manifest, rather than an unchanged rerun, parser or request for game bytes.
Primary Microsoft StackWalk64/STACKFRAME64 docs were reviewed from pinned
MicrosoftDocs source (Learn direct403). Exact references/findings are in
analysis/PRESENCE_STACKWALK_DIAGNOSTICS_20261031.md.

## Findings / changed files

Known source facts: old zero-initialized CONTEXT_FULL has zero segment/EFlags;
AddrFrame=RSP differs from context RBP; sentinel points at function ENTRY rather
than actual call return. Documentation recommends valid CONTEXT and identifies
x64 frame pointer as RBP/RDI, not necessarily used. These are candidate setup/oracle
issues, NOT proven reasons for native55 failures. No unjustified fix or relaxed
predicate was introduced. GetLastError generally is not set by StackWalk64;
zero/observed callback error must not be treated as a root-cause code.

- `source/tests/presence/fixture.cpp`: genuinely unhooked controls first, before
  MH_Initialize. Same55 original assertions, expected PC/SP equality,3-step cap.
  Four independently labeled experiments per sample: baseline unchanged;
  RtlCaptureContext-seeded test context/frame=RSP; same/frame=RBP; baseline with
  a genuine owned CALL return PC. Alternative results never OR into primary PASS.
- `source/tests/presence/fixture.S`: independently authored owned caller/return
  label for that diagnostic only; original observer/legacy restore code unchanged.
- JSON diagnostic_revision2: initial PC/SP/frame/context RBP/segments/flags,
  expected caller, synthetic/thread stack bounds, each BOOL/cleared last-error/
  PC/SP/FP/return/context PC/SP, bounded lookup/module callback events, independent
  VirtualUnwind results and loaded DbgHelp file/API version. No memory byte dumps.
  Default ReadMemoryRoutine=NULL remains valid current-process reader. DbgHelp
  calls stay serial; no arbitrary process/game attachment or pointer reads.
- `test_presence_stackwalk_diagnostics.py`:4 coverage/accounting tests ensure all55
  primary results survive,220 diagnostic traces/55 independent direct API records,
  original controls first, bounded events/bounds and legacy32 failure detection.
- `test_run_presence_owned.py`:6 tests include actual staged Wine/Unicode run and
  **synthetic** native146/55 failure classification/JSON retention. This is not
  native execution and is never counted as native PASS.
- `native-probes/presence/manifest.json`, `presence-owned.exe`: current revision2.
  Old manifest-v1.json/Wine JSONs and Git predecessor retained. New summary is
  stackwalk-v2-validation.json. Current README/state/analysis supersede old pending
  native status while preserving its original evidence.

EXE SHA256: `c6fc4fff0473c76bf353cbbaa3df40e66d076920871cb05bb7b03e08289e8850`
Size1548946 bytes; diagnostic_revision2; production_admitted=false.

Wisdoms shared debugging note published as `25c53d3` (native FAIL preserved).

## Exact validation / expected vs actual

| Command | Expected | Actual |
| --- | --- | --- |
| `bash source/tools/test_presence_native.sh` | Existing strict201 assertions preserved, new diagnostics bounded/serialized | exit0;201 PASS/0 FAIL in Wine; all4 configurations55/55; native unavailable |
| `bash source/tools/test_presence_native.sh legacy-counterexample` | Old bad restore metadata must still fail exactly32 checks | exit0 regression;169 PASS/32 expected FAIL;16 StackWalk +16 VirtualUnwind failures |
| `python3 source/tools/test_presence_stackwalk_diagnostics.py` | All55 primary predicates unaltered, independent variants/data and bounds present, no missing controls | exit0;4 tests PASS |
| `WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' python3 source/tools/test_run_presence_owned.py` | Actual byte staging/hash/Unicode/cleanup/refusal plus synthetic native FAIL retention | exit0;6 tests PASS; nested Wine fixture not counted twice |
| `git diff --check` | No whitespace errors | exit0 |
| Python hashlib comparison of EXE and all build inputs to manifest | Exact matching SHA/size/source inputs | PASS |

PASS:4 focused command runs;201 fixed assertions,4 diagnostic unittest methods,
6 runner methods and1 expected-failure regression are distinct reporting units.
No sum counting nested201 again. Legacy32 are deliberately expected failures,
not silently removed. Known prior native result is146 PASS/55 FAIL, separate.

UNAVAILABLE:2 current groups — native Windows instrumented EXE/API execution and
real Windows PowerShell5.1 launcher execution. Cloud has Wine; this cannot
establish the native failure cause or claim a native correction.

SKIP:5 unchanged whole-runtime commands (`bash source/tools/build_windows.sh`,
`bash source/tools/test.sh`, `bash source/tools/smoke_windows.sh`,
`bash source/tools/test_hooks_windows.sh`, `python3 source/tools/package.py`).
CODEX_NEXT explicitly says focused fixture validation only, no unchanged whole-
runtime suites for this diff. No production source/config/runtime/package change.

BLOCKED:1 production observer integration/acceptance group; true native success
and causal diagnosis have not been established. No game test or release is claimed.

## Owner — ONE standalone instrumented invocation

This stages the NEW revision2 binary despite using the same fetch-run workflow.
No Update-WS, compiler, game build/deploy/launch or attachment. Paste entire block
into Windows PowerShell:

```powershell
& {
    $src = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\Mods Library\Mods-Developing\MSM2-WS\_source"
    $branch = 'codex/v01c20-edi-value-provenance'
    $origin = & git -C $src remote get-url origin
    if ($LASTEXITCODE -ne 0 -or $origin -ne 'https://github.com/deluxxxe-dev/MSM2-WallSprint.git') { throw 'Wrong WS origin' }
    & git -C $src fetch origin $branch
    if ($LASTEXITCODE -ne 0) { throw 'Fetch failed' }
    $rev = & git -C $src rev-parse "origin/$branch"
    if ($LASTEXITCODE -ne 0 -or $rev -notmatch '^[0-9a-f]{40}$') { throw 'Remote head unavailable' }
    $code = @(& git -C $src show ($rev + ':source/tools/windows/Run-Presence-Owned.ps1'))
    if ($LASTEXITCODE -ne 0) { throw 'Owned runner unavailable' }
    $file = Join-Path ([IO.Path]::GetTempPath()) ('WS-owned-' + [guid]::NewGuid().ToString('N') + '.ps1')
    try {
        [IO.File]::WriteAllText($file, (($code -join "`n") + "`n"), (New-Object Text.UTF8Encoding($false)))
        & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $file -SourceRoot $src
        if ($LASTEXITCODE -ne 0) { throw 'Owned fixture failed/unavailable; return the JSON if generated' }
    }
    finally { if (Test-Path -LiteralPath $file) { Remove-Item -LiteralPath $file -Force } }
}
```

Return only the newly created
`Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-presence-owned-*.json`.
Confirm diagnostic_revision2; failure JSON is useful and retained. Primary checks
are deliberately unchanged: native146/55 can recur, but this time each failure
has observed-vs-expected frames and independent comparisons. Alternative successes
do not mean the required baseline passed or that production is admitted.

Next Codex review starts with unhooked Request+0 before MinHook: FALSE vs TRUE,
first/next-frame progression, exact PC/SP, callback/table/base and initial context.
Then compare seeded/RBP/genuine-return trials and direct VirtualUnwind separately.
Fix only a demonstrated setup/metadata fault, keep strict assertions and old32
counterexample, and require actual native success before Request-only integration.
