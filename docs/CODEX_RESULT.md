# Codex result — v0.1c38r2 equal-clock correctness /33ms tuning

2026-10-31 JST. STATUS: success (scoped implementation and cloud validation).
Branch `codex/v01c20-edi-value-provenance`.
Baseline `3ba11ef601ef3baa0150a60a7fa19fd05c322f97`; main not merged.
Implementation publication: pending below; final documentation commit is reported in chat.

## Changes / accepted evidence

See `analysis/V01C38R2_REVERSAL_LATENCY.md` for bounded reasoning and supplied
r1 evidence. Owner confirms r1 reversal with perceived initial moonwalk; logged
474/358/366/353ms candidate-to-completion is NOT input latency. Same-ms update160
->161 reset is reproduced independently; no assertion that it explains all delay.

A: equal-time different-update WAIT preserves observation position/time/update,
pending sign/count/time and active turn pose. Same-update calls are non-evidence.
Transport regression bookkeeping is distinct from observation/render baseline;
no new freshness, vote or fabricated dt. Real regression/gap and existing stale/
identity/session boundaries remain. B: one experimental33ms constant controls
observation and confirmation minimum only; still two consecutive eligible samples.
Synthetic acceptance66ms after baseline is not a measured owner input latency.

Changed helper, focused tests, native source smoke fixture, version.hpp/info.json,
README_TEST, analysis, PROJECT_STATE and this result; generated BUILD_MANIFEST,
MSM2-WS-v0.1c38r2.zip, MSM2-WS-v0.1c38r2-source.zip and
MSM2-WS-v0.1c38r2.script.disabled. Runtime v0.1c38r2 / package0.1.58.
Old r1 artifacts and five pre-existing untracked archives/bundle preserved.

## Separate A100 evidence / exact reproducible setup

Initial A before tuning: `bash source/tools/test_reversal_heading.sh`, expected
11 PASS with original100ms conditions; actual11 PASS, exit0. After final ordering
refinement, rechecked final A100 independently, not merely100ms timestamps under33.
The following owned temporary fixture setup was executed (equivalent setup was
used to prepare final A); no runtime seam/configuration switch was introduced:

```bash
python3 - <<'A_FIXTURE'
from pathlib import Path
p=Path('/tmp/ws-r2-A-final');(p/'include/Math').mkdir(parents=True,exist_ok=True)
h=Path('source/include/Math/reversal_heading.hpp').read_text()
(p/'include/Math/reversal_heading.hpp').write_text(h.replace('observationIntervalMs=33.','observationIntervalMs=100.'))
t=Path('source/src/reversal_heading_tests.cpp').read_text()
a=t.index(' // B: tuning-only');b=t.index(' source_math::Matrix m{};',a)
(p/'reversal_heading_A.cpp').write_text(t[:a]+t[b:])
A_FIXTURE
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -I/tmp/ws-r2-A-final/include -Isource/include /tmp/ws-r2-A-final/reversal_heading_A.cpp -o /tmp/ws-r2-A-final/test_A
/tmp/ws-r2-A-final/test_A
```

Expected/actual: exit0,11 PASS groups. Original10 groups plus A history regression
retained. Only B-specific33ms static_assert/tuning case omitted in A phase.
Temporary header verified byte-equal to final production except100 vs33 constant.

Header SHA256 A100: `a3d82837335b51d75a2d543ff0e61d028cc75219e90227e2d291e50159a68672`; final B33: `c495d1c5dbcfefae86a2bc11649707672d44bb6572d9062690151ecbc0fb8f04`.

## Final exact validation commands / expected / actual

Commands executed from repository root with
`export PATH=/workspace/toolchains/analysis-python/bin:$PATH`.
Counts below are emitted PASS lines/groups, not summed unique assertions;
portable repeats focused, and hooks repeats native suites.

| Command | Expected | Actual |
| --- | --- | --- |
| `bash source/tools/test_reversal_heading.sh` | 12 focused groups, same-ms history and33/66ms acceptance plus retained invariants | exit0; 12 PASS / 0 SKIP / 0 UNAVAILABLE; 0.69s |
| `bash source/tools/build_windows.sh` | Windows DLL/package builds with matching r2 version | exit0; 0 PASS / 0 SKIP / 0 UNAVAILABLE; 37.16s |
| `bash source/tools/test.sh` | portable regression groups including focused tests | exit0; 25 PASS / 0 SKIP / 0 UNAVAILABLE; 7.65s |
| `bash source/tools/smoke_windows.sh` | Windows/Wine smoke invariants | exit0; 7 PASS / 0 SKIP / 0 UNAVAILABLE; 11.7s |
| `bash source/tools/test_native_source.sh` | actual owned A/B hook/store publication, unchanged Position/W/compact metadata and stale freeze | exit0; 10 PASS / 0 SKIP / 0 UNAVAILABLE; 66.47s |
| `bash source/tools/test_hooks_windows.sh` | full canonical chain, retained counterexamples and fail-closed unavailable native certificates | exit0; 895 PASS / 5 SKIP / 17 UNAVAILABLE; 363.8s |
| `python source/tools/release.py` | package/export/import/version validation and both ZIP CRCs | exit0; 1 PASS / 0 SKIP / 0 UNAVAILABLE; 1.59s |

B/combined:12 PASS, including one candidate insufficient, duplicate timestamps
cannot confirm/advance/freshen, backwards wall/native (also after WAIT), real
>250ms gap, low-speed/ambiguous hold, repeated reversal and proper250ms turn/Up.
A10011 PASS is separate evidence, not an additional unique total for combined.
Build has zero PASS-labeled lines but exit0, not zero executed checks.
BLOCKED:0. All required commands executed; no required cloud test omitted.

## Intermediate failures and corrections

First native run exit3 at pendingCount assertion: old fixture backdated observation
clocks while real worker ticks could share a millisecond. Fixed only test timing:
actual34ms candidate waits, fresh16ms cadence to midpoint/completion and actual
120ms stale pause. Existing assertions retained; production callsite untouched.
An intermediate overly broad WAIT draft used every seen timestamp as baseline;
existing ambiguity assertion failed (exit134) in A/combined. Separated regression
metadata from observation/render baselines; no assertion removed. Final A100 and
all seven required commands rerun after corrections; all exit0. Prior preliminary
build/portable/smoke passes are not substituted for this final run. Subsequent
report-only packaging does not require repeating unchanged binary tests.

## SKIP / UNAVAILABLE / unperformed owner checks

Canonical5 SKIP are optional private replay datasets (not distributed):
- SKIP: private original-byte positive replay unavailable; independent wrong-hash rejection executed; later bounded-reader admission is TEST-ONLY
- SKIP: private native replay files not distributed; synthetic AVX/CFG tests above executed
- SKIP: optional private game-body installer replay not redistributed; independent native tests continue
- SKIP: optional private584byte game-writer normalization replay; independent synthetic decoder/source tests ran
- SKIP: private accepted-build capture replay; set WALLSPRINT_PATH_B_CAPTURE to its PATH_B_FUNCTION/PATH_B_CODE log (never redistribute). Independent proof tests execute in test_dual_source.sh.

Canonical17 UNAVAILABLE = c40 reset/Position VCH2, c49 bridge6, c50 native identity7,
c51 post-return2. Wine did not dispatch required VCH/native post-return evidence;
registration is not dispatch/order proof. No native Windows positive substituted.
Existing c39-c52 refusal/counterexamples remain. Owner r2 game response additionally
UNAVAILABLE/UNKNOWN: no access to owner Windows/game run; not requested/deployed
by this task. No real-game latency/animation/input-intent claim. No owner-side
operation is required to complete this bounded cloud task; no game test prose.

## Scope / publication checks

Production native_source_override.cpp (including MOVEMENT_DELTA_SAMPLE100ms),
entry transition, A/B store layout, guard/budget, reset_trace, hooks/watch/VCH,
shared checkers/launcher/loader and all SF/other MOD code unchanged from baseline.
Speed2, dot+/-0.8, max gap250ms, fixed positive250ms turn and exact endpoints stay.
C52 production review FAIL and write gates remain; no Position/gameplay/new input/
velocity writes or animation/camera/asset/producer/c73 change. No owner deployment.
Wisdoms read-only: no shared engine fact established; scoped helper evidence is
preserved here and in analysis, no other-repository change authorized this task.

Final documentation/source package regenerated with `python source/tools/release.py`;
ZIP CRC/member equality and `git diff --check` verified before publication.
Normal push only; independently fetched GitHub head and compared changed blobs.
Publication record below is completed after actual push verification.
