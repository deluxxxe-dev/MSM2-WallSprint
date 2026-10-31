# Codex result — actual OWNED MinHook presence fixture, native Windows pending

2026-10-31; branch `codex/v01c20-edi-value-provenance`.
Baseline `3ea14b28cdd7bddb0f41ccd6659e48bc5d8a6a16`, fetched/current.
No main merge; five pre-existing untracked archives/bundle preserved.
Implementation commit `e082819a4c477c13ea6b36156b78a0242f25a304`.
Pushed to canonical GitHub branch; independent checkout fetch, exact FETCH_HEAD,
ls-remote and byte-for-byte readback of all16 changed file blobs PASS using
`python3 /tmp/ws-presence-publication.py` (baseline3ea14b2, no game data).
This final publication record is a subsequent documentation-only commit.
Wisdoms full SHA `80a554c864f2679f1515d48032de624e3910a333` independently
confirmed via canonical ls-remote. Final WS record-commit SHA is reported in chat;
implementation and all artifact blobs are ancestors of that published head.

## STATUS

**PARTIAL — OWNED_FIXTURE_WINE_PASS / NATIVE_WINDOWS_UNAVAILABLE /
RUNTIME_PRESENCE_OBSERVER_NOT_IMPLEMENTED.**

Accepted owner audit161445 supplies original prefix/unwind metadata. Those inputs
are no longer missing. Remaining Codex fixture work was implemented and executed:
real vendored MinHook trampolines, two owned equivalent prologues, a proposed
preserving observer and actual Windows unwind API calls under Wine. No repeated
owner byte acquisition or offline parser-only handoff.

A concrete unsafe first design was found: single-range metadata fails32
mid-restore unwind assertions despite normal ABI return passing. A materially
safer split-range restore design passes the same checks under Wine. The old
failure is retained and reproducible, not masked. **Native Windows is not Wine**;
the task explicitly permits this standalone Windows-required handoff. Production
admission is false. No claim this fixture proves all native game behavior.

## CHANGED

- `source/tests/presence/fixture.S/.cpp`: owned real MinHook transfer/forwarding/
  GPR/flags/YMM/stack/return tests, original delayed save/unwind/continuation,
  every observer push/restore boundary, cap/zero-hit/thread/close tests; old bad
  unwind counterexample remains executable.
- `source/tools/test_presence_native.sh`: build and execute fixed fixture or
  assert exactly32 expected legacy failures. No proprietary function bodies.
- `source/tools/run_presence_owned.py`, `windows/Run-Presence-Owned.ps1`,
  `test_run_presence_owned.py`: fetch/pin, byte-preserving binary staging with
  exact SHA/size checks, own-temp execution and compact WS-logs JSON. Wrong
  origin/branch/hash/missing-logs refuse. No game-file access or WSL needed.
- `analysis/native-probes/presence/`: real1.3MB runnable EXE, manifest/source
  hashes, derived Wine assertion JSONs and detailed scope/reproduction README.
- `analysis/PRESENCE_OWNED_NATIVE_FIXTURE.md`, prior audit supersession note and
  `docs/PROJECT_STATE.md`: findings, limits, exact test matrix and next boundary.
- Wisdoms `docs/WALLSPRINT_INTERNALS.md`: reusable normal-return-vs-mid-restore
  unwind failure, delayed home saves, real allocator lookup and native evidence
  limits. Published commit `80a554c` (no SF code/shared environment change).

Artifact SHA256:
`885cb79fb9e6fc0b4d9e7a8957c30f5679a07ede4bfe6dabf271598e3baa1d5e`.
Runtime remains **v0.1c38r7/package0.1.63**; this is standalone
`WS-owned-presence-fixture-v1`, not a new installed diagnostic release.

## Tests — commands / expectations / actuals

| Exact command | Expected | Actual |
| --- | --- | --- |
| `bash source/tools/test_presence_native.sh` | Actual MinHook spans/transfers, original-vs-hooked ABI and all sampled original/tramp/relay/observer unwind positions pass; bounded/racing forwarding preserved | exit0,201 assertions PASS,0 FAIL under Wine; native Windows group UNAVAILABLE |
| `bash source/tools/test_presence_native.sh legacy-counterexample` | Reject wrong single-range unwind only at known restore positions | exit0 expected-failure regression;169 assertions PASS,exactly32 expected FAIL; no unexpected failure |
| `WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' python3 source/tools/test_run_presence_owned.py` | Real Git/binary extraction, actual Wine invocation with Unicode output, cleanup, mismatch/origin/branch/log refusal | exit0,5 unittest PASS; nested201 fixture checks are not counted twice; native result UNAVAILABLE |
| `bash source/tools/build_windows.sh` | Existing unchanged runtime builds | exit0 |
| `bash source/tools/test.sh` | Portable/ASan/UBSan regressions preserved | exit0 |
| `bash source/tools/smoke_windows.sh` | Existing synthetic host build-gate refusal/log/stop behavior preserved | exit0 |
| `bash source/tools/test_hooks_windows.sh` | Existing hook/counterexample/static-proof suites run; native/private evidence stays unavailable/skipped | exit0; existing5 SKIP markers and17 UNAVAILABLE markers retained (see below) |
| `git diff --check` | No whitespace errors | exit0 |
| Artifact/source SHA256 comparison against `analysis/native-probes/presence/manifest.json` using Python hashlib | EXE and every recorded build input exactly match manifest | PASS |

Fixed fixture:201 assertions PASS; separate runner:5 test methods PASS;
legacy regression:1 PASS (32 deliberately failing assertions expected).
All7 main executable commands completed exit0. No native Windows tests promoted.
Existing hook-suite status lines (separate from current fixture assertions):
5 SKIP = private original-byte replay, private native-body replay, private installer
replay, private584byte normalization replay, accepted-build capture replay.
17 UNAVAILABLE =2 older VCH hardware-event dispatch cases,6 c49 SEARCH bridge
cases,7 c50 exact-native-identity cases and2 c51 VCH bridge cases. These are
explicit status-message counts, not unique aggregate test-case totals; the
full wrapper is exit0 because it preserves these expected evidence limits.
No native dispatch certificate or private-byte positive replay was invented.

**UNAVAILABLE (2 current groups):** actual Windows owned EXE/API results; actual
Windows PowerShell5.1 wrapper execution. Cloud has Wine, not a real Windows kernel
or PowerShell5.1. The same owned executable tests the missing guarantee locally.
Prior hook-suite SKIP/UNAVAILABLE markers are listed separately below.

**SKIP (1 current command):** `python3 source/tools/package.py` / new runtime
release/package publication. No runtime diff/admission or version change; packaging
unchangedr7 would not validate this standalone EXE. No new game package claimed.

**BLOCKED (1 integration group):** production phase/drain/startup/F8/real-game
mismatch/disable/unload acceptance: observer is deliberately not integrated before
native fixture review. Retained code does not prove arbitrary saved-CONTEXT
retirement. No unsafe removal/free, C52 bypass, partial setter hook, pointee read,
RIP/DR repair, native request/gameplay write, or production lifetime relaxation.
YMM0–15 measured; no claim of measured AVX-512/XSTATE or asynchronous game EH.
Original and worker phase tests are OWNED, not actual ground/WR evidence.

## Owner — one standalone command, no game / Update-WS

Paste this entire block into **Windows PowerShell**. Existing installed `_source`
may remain at its old checkout: this fetches and stages the helper from a pinned
remote commit without pull/checkout/build/deploy. Only native OWNED fixture runs.

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

Expected: native Windows fixture201 PASS/0 FAIL and production_admitted=false;
if FAIL/UNAVAILABLE, retain exact assertion results. Return only the new
`Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-presence-owned-*.json`.
No Spider-Man2 launch, game attachment, Install mods or Update-WS requested.
After native review, implement Request-only ground/WR observation if admitted;
optional Gravity remains independently gated. No downward movement fix promised.
