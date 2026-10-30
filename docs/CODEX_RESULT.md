# Codex result — r5 wall-plane displacement-facing orientation proof

2026-10-31 JST. STATUS: implementation validated and published; game/body acceptance pending.
Branch `codex/v01c20-edi-value-provenance`, baseline
`e1ad781caaabc1933a5fbe422e0b1c0343f6503a`; no main merge.
Runtime **v0.1c38r5 / package0.1.61**. Publication recorded after commit/push.

## Result / changes / safety

Existing game translation already supports up/down/diagonal. Replaced only binary
+/-entry Forward heading target with measured world wall-tangent direction;
proper basis with Up=authenticated wall normal, coherent A/B writer path retained.
Named experimental12deg heading deadband and8deg first-candidate stability;
33ms/two samples/speed>=2/gap<=250ms, entry0.25s and turn120ms unchanged.
Shortest signed rotation, positive numerical180 tie, no mid-turn retarget,
low-speed hold, nonmixing candidate evidence and no incremental-matrix drift.
See `analysis/V01C38R5_WALL_PLANE_HEADING.md` for exact definitions and limits.

Minimal runtime changes are heading calculation, diagnostic schema2 and bounded
hold logs; no new memory/API/hook/writer. WALL_PLANE_HEADING reports candidate /
accepted target / published vectors, radians, speed, session/native-update/wall /
pause-aware clock; binary sign fields are removed. Existing64 total heading logs,
at most8 hold logs; source permission never depends on log budget.
Source transition/compact math/ownership/lifecycle/INI/shared tooling are unchanged.
Existing100ms freshness/native/identity/stale/Deinit/disable and independent4096
write budgets remain. C52 FAIL/watch disabled unchanged. No Position/input/
translation/speed/animation/idle/adhesion/corner patch. Earlier INPUT START remains
deferred. Proper source basis is NOT proof of body/animation playback acceptance.

Files: pure heading header/tests; minimal native adapter/logging; owned MinHook
fixture; version/info/readmes/BUILD; package metadata and selected analysis copy;
PROJECT_STATE/result/new analysis and versioned distribution/manifests.
Original five untracked historical ZIP/bundle files retained unstaged.

## Exact validation / expected / actual

Repository root, configured toolchains. Canonical hooks environment:
`export PATH=/workspace/toolchains/analysis-python/bin:$PATH`.
PASS counts are emitted label/groups, not summed unique assertions; focused and
native cases repeat within canonical chains. All table commands exited0.

| Exact command | Expected | Actual PASS / SKIP / UNAVAILABLE |
|---|---|---|
| `bash source/tools/test_reversal_heading.sh` | original transport/timing/retention plus full-plane17 groups | 17 / 0 / 0 |
| `bash source/tools/build_windows.sh` | Windows warnings-as-errors build with v0.1c38r5/0.1.61 | 0 / 0 / 0 (build success, no PASS banner) |
| `bash source/tools/test.sh` | ASan/UBSan portable and focused math | 30 / 0 / 0 |
| `bash source/tools/smoke_windows.sh` | Windows/Wine lifecycle/log/build-gate smoke | 7 / 0 / 0 |
| `bash source/tools/test_native_source.sh` | owned actual-clock vertical/diagonal coherent A/B and31s/reversal/lifecycle regression | 12 / 0 / 0 |
| `bash source/tools/test_hooks_windows.sh` | complete canonical hooks/accounting/cleanup/counterexample/offline/workspace chain | 904 / 5 / 17 |
| `python3 source/tools/package.py` | script DLL/version/INI/source/licence/CRC/import checks | 1 / 0 / 0 |

Focused cases cover +/-vertical, all four/shallow diagonals on three world-wall
orientations, no alternating binary heading for noisy dominant ascent/descent,
genuine full reversal preserved, incompatible candidates cannot accumulate,
shortest +/-pi seam and deterministic180 tie,120 quarter-turn loops/no drift,
WAIT/stale/reset/gap/low-speed and A/B Position/W/scale/metadata preservation.
Actual native fixture verifies vertical/diagonal source publications/real owned
MinHook A/B stores, plus original31s/>20-reversal/session/Deinit safety case.
Final canonical hooks recompiles/reruns native fixtures including final diagnostic
cap; first standalone run is supplementary. Final build/portable/smoke were repeated
when diagnostics/tests were finalized. No later runtime-code changes.

SKIP5: optional private original-body/native/game-writer584byte/accepted-build
capture replays not available/distributed. Independent synthetic negative/proof
cases still execute. No private logs redistributed or fabricated replay PASS.
UNAVAILABLE17 (canonical): c40 VCH not dispatched2; c49 native bridge certificate6;
c50 native conjunct7; c51 no VCH bridge2. These retain fail-closed classifications.
Actual owner game/body/descending-speed/input/animation acceptance is additionally
UNAVAILABLE here, not part of the17 fixture labels and not PASS. No BLOCKED code
subtask; no safety/certificate/watch weakening to make a case pass.

Additional commands:
- `python3 -m py_compile source/tools/package.py source/tools/release.py`: PASS.
- `git diff --check`: PASS.
- `git diff --exit-code e1ad781 -- source/include/Math/source_orientation.hpp source/include/Math/compact_basis.hpp source/include/NativeHooks/ownership_guards.hpp source/src/NativeHooks/source_lifecycle.cpp source/mod/WallSprint.ini environment source/tools/workspace.py`: PASS, no changes.
- `python3 source/tools/release.py`: final source/install ZIP generation, CRC and
  versioned disabled script/root manifest; packaging checks above rerun. Results
  and hashes recorded in analysis/V01C38R5_RELEASE.json after final generation.
- `python3 /tmp/ws-r5-audit.py`: verifies protected source/INI/toolkit equality,
  unchanged session/freshness/write metadata, r5 manifest, current script/DLL SHA,
  both ZIP CRC/source equality/shared KIT dependencies and honest schema2.

## Handoff

Outputs: MSM2-WS-v0.1c38r5.zip, MSM2-WS-v0.1c38r5-source.zip,
MSM2-WS-v0.1c38r5.script.disabled, BUILD_MANIFEST.json. Existing r4 outputs retained.
No private raw logs/game function bodies/research stages or foreign binaries added.
No new reusable engine ABI/ownership fact established; this is a project-local
experimental direction model. Wisdoms references consulted; no engine fact promoted.
After verified publication, owner updates via installed Update-WS.cmd. Chat supplies
the game test procedure. New log: MSM2-WS/logs/MSM2-WS-v0.1c38r5-YYYYMMDD-HHMMSS.log.
Below2-speed descent can hold old facing; assess logged actual speed before future
threshold discussion. Do not claim descending-speed/animation or earlier-input fix.

Final release and protected-code/source/ZIP/hash/schema audit: PASS (exit0).
Release emits1 PASS/0 SKIP/0 UNAVAILABLE; audit1 PASS.
Initial release attempt found the new analysis only at repository root while the
existing packager reads source/analysis; added the identical source-archive copy
and reran release/audit successfully. No runtime-code change; native suites were
not repeated for this packaging/documentation-only correction.
Versioned artifact hashes: analysis/V01C38R5_RELEASE.json.

## Publication confirmed — 2026-10-31 JST

Implementation/artifacts commit:
`ee0308512f627371bb715a49d8f8bbd27e192bff`.
`git push origin codex/v01c20-edi-value-provenance`: PASS.
`python3 /tmp/ws-r5-publication.py`: PASS, independent clone fetch/FETCH_HEAD and
ls-remote equal local implementation HEAD, all21 changed file blobs match exact
local bytes (including script/source/install ZIPs). `git diff e1ad781 HEAD --check`
PASS. Final documentation-only publication record is pushed and verified with the
same script; no code/artifact edits, no regression rebuild necessary.
Owner update now uses installed Update-WS.cmd; no download links. Chat supplies
human procedure. Cloud results do not certify actual body/animation acceptance.
