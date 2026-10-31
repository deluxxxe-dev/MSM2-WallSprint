# Codex result — CORRECTED valid CALL-return primary, revision3

2026-10-31; branch `codex/v01c20-edi-value-provenance`.
Baseline `100a1e32259cd64290755ae66573e35c66336ab2`, fetched/fast-forwarded;
no main merge. Five pre-existing untracked archives/bundle preserved.
Implementation SHA/publication recorded after push.

## STATUS

**CORRECTED_PRIMARY_VALID_CALL_RETURN / OWNED_WINE_PASS /
NEW_BINARY_NATIVE_ACCEPTANCE_PENDING.** Production remains false/unintegrated.
This corrects the demonstrated synthetic caller setup, not just instrumentation.

Accepted actual Windows v2: invalid-entry primary146 PASS/55 FAIL, genuine
CALL-return55/55 PASS, context/frame-only variants0/55. All invalid-entry failures
returned frame.pc=expected-1, correct SP; context RIP stayed expected. This is
observed native behavior, not a documented Windows internal cause or Windows bug.
No PC±1 tolerance or acceptance of the erroneous primary was introduced.

Revision3 primary is equivalent to supplied v2 genuine-return path across all55
positions, with same exact PC/SP/nonvolatile expectations. Full new executable
and4 new prerequisites have not run native: do not claim all-v3 native PASS.
ONE corrected native acceptance run below; no more context-permutation loop.

## CHANGED

- `fixture.cpp`: valid owned CALL-return primary for BOTH StackWalk and direct
  VirtualUnwind; same expected PC/SP and register assertions. Original55 caller
  checks retained. Caller shadow/frame/outer slot explicitly consistent. Old
  invalid function-ENTRY sentinel retained as separate non-admitting diagnostic.
- Four new hard checks: return inside exact caller RUNTIME_FUNCTION; real E8 CALL
  target and exact after-call label;40byte unwind metadata; actual CALL/return
  once with original return value. Caller ASM is unchanged from v2. Runtime table
  address is used for code inspection, avoiding compiler assumptions about C++
  object arithmetic across separately declared assembly labels.
- `test_presence_native.sh`, `test_presence_stackwalk_diagnostics.py`: unsafe
  legacy restore must fail exact32 named checks, not just any32. Corrected valid
  caller observed same16 StackWalk+16 VirtualUnwind failures at steps1..8,both
  sites. No new passing count substituted to hide bad restore metadata.
- `test_run_presence_owned.py`: revision3/110 traces and synthetic55-failure report
  remains FAIL. Synthetic150/55 classification is not measured Windows evidence.
- Current EXE+source manifest, manifest-v2.json archive, derived v3 validation,
  analysis/PRESENCE_VALID_CALLER_CORRECTION_20261031.md and current state/README.
  Old v1/v2 hashes/results remain in Git and archived manifests/summaries.

Artifact: `analysis/native-probes/presence/presence-owned.exe`, revision3.
SHA256 `8d529dada651c1b4c7b1aa4ffc97b3c97896a3cfaa1c534a728679278220ab08`;
size1534133 bytes. Runtime remains v0.1c38r7/package0.1.63. No production gameplay/
config/package/ownership/restore/slot/C52/shared environment/SF changes.

## Validation — exact commands, expected / actual

| Command | Expected | Actual |
| --- | --- | --- |
| `bash source/tools/test_presence_native.sh` | Original strict201 checks under valid caller plus4 genuine-caller prerequisites pass | exit0;205 PASS/0 FAIL in Wine; primary55/55, new binary native unavailable |
| `bash source/tools/test_presence_native.sh legacy-counterexample` | Corrected valid caller must still reject unsafe mid-restore metadata at exact same positions | exit0 regression;173 PASS/32 expected FAIL;16 StackWalk+16 VirtualUnwind, both sites steps1..8 |
| `python3 source/tools/test_presence_stackwalk_diagnostics.py` | All55 primary checks tied only to valid caller;110 traces; same direct/StackWalk expectations; exact bad restore failure set | exit0;4 unittest methods PASS |
| `WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' python3 source/tools/test_run_presence_owned.py` | Real byte staging/hash/Unicode/cleanup/refusal, revision3 outputs, synthetic native failure preserved | exit0;6 unittest methods PASS; nested205 not counted twice |
| `git diff --check` | No whitespace errors | exit0 |
| Python hashlib size/SHA/source comparison to current manifest | Published EXE/build inputs exactly match tested build | PASS |

PASS:4 focused command runs;205 fixed assertions,4 diagnostic test methods,
6 runner methods and1 expected-failure regression are distinct reporting units.
Legacy32 are expected failures with exact names, not a successful unsafe design.
Supplied prior native55 genuine-return PASS is separately recorded; old invalid
primary55 FAIL remains historical and is not relabeled as a pass.

UNAVAILABLE:2 current groups — actual native Windows execution of full corrected
v3 binary and actual PowerShell5.1 wrapper execution. Equivalent supplied native
path evidence is documented, but no entire-new-binary native claim or production
certificate is issued. Legacy under corrected valid caller verified in Wine;
no new native execution of legacy asserted.

SKIP:5 unchanged full-runtime commands (`bash source/tools/build_windows.sh`,
`bash source/tools/test.sh`, `bash source/tools/smoke_windows.sh`,
`bash source/tools/test_hooks_windows.sh`, `python3 source/tools/package.py`).
CODEX_NEXT explicitly requests fixture-only validation, not unchanged pipeline.

BLOCKED: production observer integration/acceptance pending corrected native
acceptance. No new game EXE acquisition, naming scan, attachment or gameplay test.

## Owner — ONE CORRECTED native acceptance invocation

Same fetch/run workflow stages the NEW corrected revision3 EXE; this is not an
unchanged v2 instrumentation rerun. No game/Update-WS/compiler/build/deploy needed.
Paste entire block into Windows PowerShell:

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

Return only the new
`Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-presence-owned-*.json`.
Expected diagnostic_revision3, valid primary55/55 and full205 PASS/0 FAIL;
production_admitted=false. Old invalid-sentinel diagnostic may fail and does not
change the valid-primary verdict. Preserve JSON on any genuine failure.

After accepted corrected native result, next task can resume Request-only
runtime diagnostic implementation. Optional Gravity cannot block independently
valid Request evidence. No extra game-body/name scan or current game update.
