# Codex result — Steering Probe retirement, then v0.1c38r3

2026-10-31 JST. STATUS: success (two sequential scoped implementations).
Branch `codex/v01c20-edi-value-provenance`; main never merged.

## Published prerequisite BEFORE r3

Steering Probe retirement implementation
`faace6f6412fcc5c467166fd7f116c2e8cbe1473`; final prerequisite head
`b0683a5af79a522d596937c842321ea420f1b8ac`. Initial push rejected due concurrent
roadmap commits; fetched/merged SAME development branch, retaining all docs,
normal push succeeded. `python3 /tmp/ws-r2-publication.py` independently fetched
GitHub into /tmp/ws-steering-docs-publication, matched fetched head and ls-remote
and27 changed file blobs from3ba11ef baseline. Only THEN r3 runtime edits began.
Detailed cause/scope/checks: analysis/STEERING_PROBE_RETIREMENT.md;
retirement result also preserved at prerequisite commit:docs/CODEX_RESULT.md.

Cause: archive-only sparse protection missed direct stage discovery. Public
Overstrike recursively detects stages plus archive members; new paths default
checked, while old path selection is retained. No WS profile mutation found;
actual owner's duplicate paths UNKNOWN. Exact known Fast10/Slow5 metadata/member/
reverse-one-byte pinned vanilla SHA identifies retirement candidates ONLY inside
WS. Backups preserve bytes/relative paths/hash outside Mods Library in unique
game-root/MSM2-WS-Retired-Probes/retired-*; clean tracked stages stay in Git but
are sparse-excluded. Unknown/foreign inputs, dirty tracked work/reparse refuse;
SF/ordinary mods/profile/order untouched. Builders/research retained.

Prerequisite commands / expected / actual:

| Command | Expected | Actual |
| --- | --- | --- |
| `python3 source/tools/test-workspace.py` | original13 plus duplicate/idempotence/collision/identity/dirty/reparse/sparse/build-stage cases | exit0;18 PASS groups |
| `python3 environment/test_mod_environment.py` | shared environment isolation/roundtrip | exit0;28 tests OK |
| `python3 environment/test_start_overstrike_preflight.py` | retained preflight boundaries | exit0;PASS |
| `python source/tools/release.py` | retirement module/CRC/unchanged r2 bytes | exit0;PASS |
| `python3 /tmp/ws-retire-dist-check.py` | stale output contamination refuses; research-stage input excluded | exit0;PASS |

Detailed owned sentinel/fixture setup and initial fixture-only correction in
analysis. SKIP0/BLOCKED0 for prerequisite. Owner UI/paths UNAVAILABLE; native
reruns omitted ONLY in tooling phase by explicit task, r2 runtime bytes equal.
Guarded old-updater source refresh and outside-WS-copy handling in LOCAL_WORKSPACE,
mod-tools OVERSTRIKE/MANUAL. No broad library scan/removal or profile rewrite.

## r3 exact scope / supplied evidence

r3 baseline is published prerequisite b0683a5 above. Runtime v0.1c38r3,
package0.1.59. Only helper named reversalTurnDurationMs=120 and progress divisor
change post-entry reversal250->120ms. No other production code/timing change.
Timing expectations in focused/native fixtures adjusted, safety assertions kept;
version.hpp/info.json/README_TEST and analysis/PROJECT_STATE/result updated.
Artifacts: MSM2-WS-v0.1c38r3.zip, MSM2-WS-v0.1c38r3-source.zip,
MSM2-WS-v0.1c38r3.script.disabled and BUILD_MANIFEST.json. Older r2 distribution
retains retirement implementation; original five untracked legacy artifacts kept.

Supplied r2 log: six turns302/304/302/303/322/303ms candidate-to-completion,
41–54ms candidate-to-accepted,248–270ms accepted-to-completion. These emitted
line intervals are not exact internal clocks or input latency. Owner still sees
lag; ChatGPT's42.918s video overview/125ms sampling is qualitative, not measured
animation/input onset or exact shared clock alignment. Session3 one25ms stale
pause; sessions2/3/4 expected five-second stop, verified Deinit/Up/Position writes0.
Late acceptance without completion near timeout is not automatically a failure.
This supersedes r2-runtime-UNKNOWN, not proof of r3 effect or animation ownership.
Raw private log/video not inspected anew or redistributed here.

Chosen120ms is not a measured vanilla match or total input latency. Keep33ms
observation/confirmation, two consecutive samples, speed2/dot0.8/max gap250,
equal-clock WAIT, proper signed180degree/Up/exact endpoint/no drift/no retarget,
entry0.25s, stale/session/identity/freshness/source lifecycle/5s budget, A/B source/
Position/W/scale/metadata and native safety gates. No animation/input API/INI,
asset/c73/producer/new watch/hook/VEH/VCH/Position/velocity/gameplay write or shared
tooling change in r3. Source entry/callsite/sampler and published retirement
files byte-equal to b0683a5; helper diff equals only constant+divisor substitution.

## r3 validation — exact commands / expected / actual

Repository root, `export PATH=/workspace/toolchains/analysis-python/bin:$PATH`.
Initial `bash source/tools/test_reversal_heading.sh` also passed12 groups exit0.
Full final sequence below (PASS labels/groups are not unique assertion totals;
portable repeats focused and hooks repeats native tests):

| Command | Expected | Actual |
| --- | --- | --- |
| `bash source/tools/test_reversal_heading.sh` | 12 focused groups: unchanged acceptance at66, proper midpoint60, no completion119, exact120 endpoint; freeze/repeat/regression contracts | exit0; 12 PASS / 0 SKIP / 0 UNAVAILABLE; 0.65s |
| `bash source/tools/build_windows.sh` | matching r3 Windows build/version/package | exit0; 0 PASS / 0 SKIP / 0 UNAVAILABLE; 38.68s |
| `bash source/tools/test.sh` | portable regression including unchanged entry0.25s | exit0; 25 PASS / 0 SKIP / 0 UNAVAILABLE; 7.7s |
| `bash source/tools/smoke_windows.sh` | Windows/Wine smoke invariants | exit0; 7 PASS / 0 SKIP / 0 UNAVAILABLE; 11.72s |
| `bash source/tools/test_native_source.sh` | real owned MinHook A/B midpoint/final store, stale freeze, Position/W/compact metadata | exit0; 10 PASS / 0 SKIP / 0 UNAVAILABLE; 66.65s |
| `bash source/tools/test_hooks_windows.sh` | full canonical chain, existing counterexamples/fail-closed unavailable certificates | exit0; 900 PASS / 5 SKIP / 17 UNAVAILABLE; 349.76s |
| `python source/tools/release.py` | matching package metadata/export/import/script and CRC checks | exit0; 1 PASS / 0 SKIP / 0 UNAVAILABLE; 1.62s |

Focused midpoint: accepted66 ->126 (=+60);185 (=+119) still turning;
186 (=+120) exact endpoint. Existing entry125/250ms midpoint stays unchanged.
All12 repeated reversals/noise/ambiguity/Up/diagonal exact endpoints and metadata
assertions preserved. Native fixture only midpoint cadence target110->50ms into
turn, leaving real34ms sample waits/120ms stale pause/A-B assertions unchanged.
First canonical attempt was stopped after235.98s (exit143) when unchanged
orientation_trace_smoke.exe made no progress for several minutes:12 earlier PASS,
2 private SKIP, no completion claimed. Only that owned process was terminated
with `kill -TERM 206438`. Re-executed exact `bash source/tools/test_hooks_windows.sh`
in the same environment without source/skip/command changes; final result above.
This is a transient owned Wine/fixture execution hang, not proven r3 causality.
Nonfatal Wine winedevice.exe c0000135 warnings
were emitted, but test executables/commands returned0; no warning silencing or
environment bypass was added. Metadata wording/report-only packaging after binary
checks does not alter tested runtime code; required release regenerated afterward.

## SKIP / UNAVAILABLE / BLOCKED / unperformed

Canonical optional private datasets (not redistributed),5 SKIP:
- SKIP: private original-byte positive replay unavailable; independent wrong-hash rejection executed; later bounded-reader admission is TEST-ONLY
- SKIP: private native replay files not distributed; synthetic AVX/CFG tests above executed
- SKIP: optional private game-body installer replay not redistributed; independent native tests continue
- SKIP: optional private584byte game-writer normalization replay; independent synthetic decoder/source tests ran
- SKIP: private accepted-build capture replay; set WALLSPRINT_PATH_B_CAPTURE to its PATH_B_FUNCTION/PATH_B_CODE log (never redistribute). Independent proof tests execute in test_dual_source.sh.

17 UNAVAILABLE are retained Wine/native certificate boundaries: c40 reset/Position
VCH2, c49 bridge6, c50 native identity7, c51 post-return2. Registration not treated
as dispatch/order proof; no native Windows substitute or certificate. C39-c52
counterexamples/C52 review FAIL remain. BLOCKED0. Every required cloud command
executed. Actual owner r3 game effect/UI/animation synchronization additionally
UNAVAILABLE/UNKNOWN: no owner PC/game runtime access, no deployment or human game
test prose authorized here. No owner action required for cloud completion. Future
movement roadmap is planning only, not permission to implement it now.

Shared Wisdoms WALLSPRINT/MSM2_DEVELOPMENT_MANUAL/WALLSPRINT_INTERNALS references
reviewed read-only from /tmp/ws-current-wisdoms-review. Reusable scanner finding
preserved in project analysis; Wisdoms synchronization deferred to scoped shared
handoff, no unrelated-repository modification during isolated runtime experiment.

## Final scope/package/publication verification

`python3 /tmp/ws-r3-final-audit.py` verifies the byte/member conditions below.

`git diff --check` / `git diff --cached --check`:exit0. Python byte comparison
against b0683a5 verifies source/src/NativeHooks/native_source_override.cpp,
Math/source_orientation.hpp and all published updater/retirement/package/build
files unchanged; helper equals only named120ms constant+progress divisor delta.
ZIP testzip() None and final helper/focused/native source member bytes equal;
script info.json version0.1.59 matches runtime label in DLL. Script exports/imports
checked by release. No visible stage/nested archive member in r3 source/install
ZIPs. No raw private data or legacy untracked file staged. Final release rerun
only for included documentation, not a substitute for binary tests.

Normal push and independent remote readback recorded after actual verification.
Final chat SHA identifies record-only commit without self-reference.

### Verified r3 publication

Implementation commit `95ff38478c66767b47a70e89880ba399caa0527e`.
`git push origin codex/v01c20-edi-value-provenance`:success, b0683a5->95ff384.
`python3 /tmp/ws-r3-publication.py`:exit0; independent GitHub fetch into
/tmp/ws-steering-docs-publication, FETCH_HEAD/ls-remote/local HEAD equal that SHA;
all14 changed file blobs against b0683a5 equal local files (including both ZIPs).
The script executes `git -C /tmp/ws-steering-docs-publication fetch origin
codex/v01c20-edi-value-provenance`, `git ... rev-parse FETCH_HEAD`, `git ls-remote
origin refs/heads/codex/v01c20-edi-value-provenance` and compares every
`git show FETCH_HEAD:<path>` byte string with local paths from
`git diff --name-only b0683a5 HEAD`.

This record-only update regenerates included result documentation with
`python source/tools/release.py`; `python3 /tmp/ws-r3-final-audit.py` passes again.
Runtime script bytes remain equal to implementation commit. No need to rerun
unchanged binary tests for record-only ZIP documentation changes. Same normal
push and independent readback repeated for final record commit; final full SHA
reported in chat (no self-referential commit hash). No owner deployment/request.
