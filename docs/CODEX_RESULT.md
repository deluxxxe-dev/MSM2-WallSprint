# Codex result — v0.1c38r1 orientation-only reversal proof

2026-10-30 JST. Branch `codex/v01c20-edi-value-provenance`.
Baseline: 97a11291ca847ba4f28ce2553c03ae8983328dcb.

## STATUS

**partial** — fixed orientation-only design implemented and functional checks
passed. Canonical full hooks command is blocked by an unchanged pre-existing
c67 test-runner working-directory defect. No scope expansion to fix that defect;
no recommendation to deploy through a failing Update-WS pipeline.

## CHANGED

- `source/include/Math/reversal_heading.hpp`: isolated pure post-entry heading state/math.
- `source/src/NativeHooks/native_source_override.cpp`: private heading state/tick,
  observation reset at existing stale/end boundaries, <=64 heading events/session.
- `source/src/reversal_heading_tests.cpp`, `source/tools/test_reversal_heading.sh`,
  `source/tools/test.sh`: 10 focused sanitizer groups in the portable chain.
- `source/src/msm2/native_source_override_smoke.cpp`: owned-clock/displacement
  A/B publication/store/stale/Deinit case in existing standalone MinHook fixture.
- `source/include/version.hpp`, `source/mod/info.json`, `source/README_TEST.md`:
  runtime **v0.1c38r1**, package **0.1.57**, accurate bounded-experiment metadata.
- `analysis/V01C38R1_REVERSAL_HEADING.md`, `docs/PROJECT_STATE.md`, this result.
- Generated `BUILD_MANIFEST.json`, `MSM2-WS-v0.1c38r1.script.disabled`,
  `MSM2-WS-v0.1c38r1.zip`, `MSM2-WS-v0.1c38r1-source.zip`:
  mechanically validated experimental outputs, not full-pipeline acceptance.

## RESULT

Entry Transition is byte-unchanged. After entry reaches t=1, separate bounded
observation uses distinct fresh native updates and validated Hero position:
>=100ms, dt<=250ms, wall-tangent speed>=2, dot(normalized tangent,f0)>=0.8 or
<=-0.8, and two consecutive NEW-sign observations spanning>=100ms. Thresholds
are experimental. Slow/ambiguous/same-sign samples clear pending confirmation;
invalid/gap/stale/end/session history cannot bridge evidence. No use of incoming
source Forward as vanilla intent.

An accepted reversal completes a deterministic positive 180-degree turn about
the SAME stored normal in0.25 active seconds, with no mid-turn retargeting.
Right/Forward are rebuilt and proper rotation validated; Up remains the normal.
Final Forward is component-exact saved f0/-f0, avoiding cumulative drift.
A private post-entry active clock freezes at existing stale boundaries rather
than reusing the entry clock's clamped t=1 rebasing. It does not extend the
five-second real source budget or renew permission. Completion rebases motion.

Only the existing Published basis changes. A/B readers, source stores and their
coherent-publication, ownership/current-code/thread/lifecycle/freshness gates
are unchanged. Position/W, compact scales/metadata preserved. Existing F8,
normal/identity failures, synchronous close and five-second end remain binding.
C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` unchanged. No Position,
input/velocity, animation/camera, adhesion, config-asset, new hook/watch/VEH/VCH,
producer, c73, shared launcher/loader/SF/other-repository changes.

## Validation — exact commands, expectation, actual outcome

All commands run from `/workspace/WallSprint` with
`export PATH=/workspace/toolchains/analysis-python/bin:$PATH`, except the explicit
cwd reproduction. Wine execution is not native Windows/game verification.
PASS counts below are emitted per-command records, not unique summed test cases;
the focused10 groups are deliberately also included in portable23.

| Exact command | Expected | Actual |
| --- | --- | --- |
| `bash source/tools/test_reversal_heading.sh` | exit0; owned fixture groups pass | exit0; PASS 10, SKIP 0, UNAVAILABLE 0 |
| `bash source/tools/build_windows.sh` | exit0; MinGW/Clang Windows DLL builds with warnings as errors | exit0; PASS 0, SKIP 0, UNAVAILABLE 0 |
| `bash source/tools/test.sh` | exit0; owned fixture groups pass | exit0; PASS 23, SKIP 0, UNAVAILABLE 0 |
| `bash source/tools/smoke_windows.sh` | exit0; owned fixture groups pass | exit0; PASS 7, SKIP 0, UNAVAILABLE 0 |
| `bash source/tools/test_hooks_windows.sh` | exit0; owned fixture groups pass | exit1; PASS 830, SKIP 5, UNAVAILABLE 17; FAIL: unchanged c67 cwd defect, described below |
| `python3 source/tools/test_slot_publication_post_c67_boundary_fix.py` | exit0; owned fixture groups pass | exit0; PASS 14, SKIP 0, UNAVAILABLE 0 |
| `python3 source/tools/test_slot_publication_post_c67_boundary_fix2_quick.py` | exit0; owned fixture groups pass | exit0; PASS 1, SKIP 0, UNAVAILABLE 0 |
| `python3 source/tools/test_slot_publication_post_c69_stitched.py` | exit0; owned fixture groups pass | exit0; PASS 22, SKIP 0, UNAVAILABLE 0 |
| `python3 source/tools/test_slot_publication_post_c69_boundary.py` | exit0; owned fixture groups pass | exit0; PASS 28, SKIP 0, UNAVAILABLE 0 |
| `python source/tools/release.py` | exit0; package/import/export/version/CRC checks; distinct r1 artifacts | exit0; PASS 1, SKIP 0, UNAVAILABLE 0 |

### Final exact-endpoint follow-up

Review tightened final Forward from a re-normalized endpoint to component-exact
saved f0/-f0 while retaining rebuilt Right/proper-frame validation. Added a
strict diagonal endpoint assertion. This prevents floating-point endpoint
renormalization from alternating by an ulp; it changes no input threshold,
clock, guard or source store. After this helper change, reran all affected
math/portable/Windows build/smoke/native source checks:

| Exact command | Expected | Final actual |
| --- | --- | --- |
| `bash source/tools/test_reversal_heading.sh` | exit0; relevant groups/build/package pass | exit0; PASS 10, SKIP 0, UNAVAILABLE 0 |
| `bash source/tools/build_windows.sh` | exit0; relevant groups/build/package pass | exit0; PASS 0, SKIP 0, UNAVAILABLE 0 |
| `bash source/tools/test.sh` | exit0; relevant groups/build/package pass | exit0; PASS 23, SKIP 0, UNAVAILABLE 0 |
| `bash source/tools/smoke_windows.sh` | exit0; relevant groups/build/package pass | exit0; PASS 7, SKIP 0, UNAVAILABLE 0 |
| `bash source/tools/test_native_source.sh` | exit0; relevant groups/build/package pass | exit0; PASS 10, SKIP 0, UNAVAILABLE 0 |
| `python source/tools/release.py` | exit0; relevant groups/build/package pass | exit0; PASS 1, SKIP 0, UNAVAILABLE 0 |

The native command includes both corrected standalone r1 A/B reversal case and
retained real-position-watch integration. No unrelated all-hooks rerun after
this precision-only follow-up: its unchanged cwd defect is independently
reproduced, and the affected helper consumers were explicitly revalidated.
The earlier830-PASS canonical attempt is recorded as that earlier attempt,
not as a final all-hooks PASS. Packaging was regenerated after final docs.

### Two distinct failures, retained and explained

1. Initial hooks attempt: exit5 after16 PASS/3 optional SKIP. The new owned
   reversal fixture case was initially placed **after** fixture hook/memory
   teardown, causing a Wine access violation in prepare. Moved that new case
   before teardown; no production change was needed. Re-ran the full hooks
   command because the fixture changed and the remaining chain had not run.
   The corrected r1 case passes real owned MinHook A/B stores, coherent midpoint/
   final basis, Position/W/metadata preservation and stale freeze/resume/Deinit.
2. Corrected canonical hooks run: **830 PASS / 5 SKIP / 17 UNAVAILABLE, then exit1**.
   `test_hooks_windows.sh` changes cwd to `source`; unchanged
   `tools/test_slot_publication_post_c67_boundary_fix.py:11` uses
   `runpy.run_path('source/tools/test_slot_publication_post_c67_boundary.py', ...)`.
   It therefore opens nonexistent `source/source/tools/...`. These two files
   have no diff from task baseline. Exact independent reproduction:

   ```bash
   (cd source && python3 tools/test_slot_publication_post_c67_boundary_fix.py)
   ```

   Actual: exit1, FileNotFoundError at that path. Root invocation passes14 groups;
   all remaining tail commands in the table pass separately. This does **not**
   change the canonical command to PASS or bypass the updater gate. Per current
   task, the unrelated existing runner is not modified.

### Coverage / unavailable / unrun

Focused10 groups: unchanged entry/Position/W, straight hold, confirmed reversal,
finite proper-rotation midpoint/Up and final sign, slow/oblique/normal-only hold,
distinct updates and cleared ambiguity, >250ms gap, stale pause and completion
rebase, repeated turns/no retarget or drift, invalid data and fresh-session reset,
compact scale/metadata contract. Full native fixture retains prior identity,
normal, collision, lifecycle, stale/cleanup/timeout/F8 and dual-source regressions.
New reversal case is the standalone fixture only; the existing real-position-watch
integration regression also passes, but does not itself exercise a turn.

- SKIP5: existing optional private original-byte/native-body/installer/writer-
  normalization/accepted-build capture replays absent; owned negatives and
  synthetic decoder/native fixtures ran. No required r1 test was silently skipped.
- UNAVAILABLE17: retained Wine VCH evidence (c40 reset/Position2, c49 bridge6,
  c50 identity7, c51 post-return2). Never promoted to native PASS.
- UNAVAILABLE1 additional evidence group: actual native Windows/game r1 behavior,
  visual facing response and engine/animation ownership. Cloud has no owner game.
- BLOCKED1 integration gate: canonical full hooks cwd defect above. No code/design
  subtask was substituted. Tail checks and package commands were run explicitly.

No further test reruns are needed after publication-record-only documentation
changes; release/package is regenerated for final documentation and CRC checked.
No game, local deployment, reinstall, new native probe or human test requested.

## Scope/package checks

`git diff --check` and `git diff --cached --check`: exit0. Scope/package
audit: unchanged entry math and A/B store/capture bodies confirmed, all three
artifact CRCs pass; source ZIP helper/tests equal checkout bytes; script internal
version is0.1.57. Exact additional package check:

```bash
python3 - <<'PY_PACKAGE'
from pathlib import Path
import zipfile, json
for name in ['MSM2-WS-v0.1c38r1.zip','MSM2-WS-v0.1c38r1-source.zip','MSM2-WS-v0.1c38r1.script.disabled']:
    with zipfile.ZipFile(name) as z:
        assert z.testzip() is None
        if name.endswith('-source.zip'):
            for suffix in ['include/Math/reversal_heading.hpp','src/reversal_heading_tests.cpp','tools/test_reversal_heading.sh']:
                member = next(x for x in z.namelist() if x.endswith(suffix))
                assert z.read(member) == (Path('source') / suffix).read_bytes()
        if name.endswith('.script.disabled'):
            assert json.loads(z.read('info.json'))['version'] == '0.1.57'
PY_PACKAGE
```
 Review confirms shared
`Math/source_orientation.hpp`, all A/B capture/store bodies, reset_trace/hardware/
VEH/VCH code and existing canonical hooks/c67 wrapper are unchanged. Version
parser in workspace.py accepts alphanumeric suffix r1. ZIP/script CRC, internal
version0.1.57, r1 DLL strings, and inclusion of helper/tests checked; source/archive
bytes are compared again with independent GitHub readback after publication.
Package validation proves mechanics, not full updater acceptance or real-game effect.

## Branch / publication

Branch `codex/v01c20-edi-value-provenance`.
Initial implementation commit `31a464402c93f4efc9df17688f443eb68e19ad4d` was
normally pushed and independently fetched into `/tmp/ws-steering-docs-publication`.
All16 changed source/docs/manifest/artifact blobs were byte-identical to GitHub
readback (SHA256 comparison recorded in cloud scratch). The final strict-endpoint
follow-up and its artifacts will also be verified before handoff; final code
commit SHA is recorded in the subsequent publication record.

## NEED

Chat-side decision only: authorize a separate bounded repair of the pre-existing
c67 test-runner cwd defect before declaring the canonical Update-WS pipeline ready.
No alternate movement design is needed; no owner deployment is requested while
that required command fails. Actual r1 gameplay proof remains pending thereafter.
