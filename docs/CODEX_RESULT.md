# Codex result — reversal START timing review

STATUS: success (bounded source review); earlier-intent behavior change unsupported.
2026-10-31 JST. Branch codex/v01c20-edi-value-provenance.
Baseline08028ff2da3891a02f691a9af97452b9bf25c92f, latest branch fetched/fast-forwarded;
no main merge. Publication SHA recorded after normal push below.

## Changes / conclusions

Documentation ONLY: analysis/REVERSAL_START_TIMING_REVIEW.md,
docs/PROJECT_STATE.md current handoff and this result. No runtime instrumentation,
source/test/package/version/asset/environment/profile/SF change. No archive output.
Current v0.1c38r3/package0.1.59 and all three existing r3 artifacts stay byte-equal.
r3's faster120ms rotation is not accepted as the requested earlier START solution.

Actual callers/line references, gates, publication/consumer and lifecycle paths
are inventoried in analysis. Trigger waits for displacement, distinct33ms samples,
speed2/dot0.8 and two consecutive new-sign observations. Acceptance sets logical
turnStart; that call retains previous basis, later positive-time sample rotates,
worker publication and actual A/B commit remain separate. Initial0.70s admission
and0.25s interpolation are not repeated per-reversal delays. No1.1s timer exists
to subtract; owner-requested1.1s is not a measured input latency.

Five-second source timer starts at Transition.enter(), not reversal. Reversal
never resets it; stale clock pausing does not extend the real budget. Source exit
hard-closes override, not native WallRun. Supplied session1 timeout17:10:57.972Z
and native summary17:11:00.241Z/session2 same5s timeout establish distinct lifetimes,
not reversal-caused release, unlimited straight override, or video/log alignment.

Existing bounded events distinguish detector candidate/acceptance/completion,
source timeout and native Deinit. They are not exhaustive rejection/observation/
first-changed-publication/animation telemetry; log caps/drops and prepublication
close race remain explicit. No further logging of already sampled displacement
can reveal the decisive missing earlier intent, so no redundant runtime diagnostic
or new package is justified for this review. No assertion that all historical
substage timings are reconstructed. No guessed input API, offset or timer change.

ONE missing fact: authenticated wall-relative requested-direction/sign BEFORE
position reversal, with same session/native update and monotonic time/lifetime
identity. Existing Snapshot/lifecycle do not supply it; source axes can contain
our own output. No supported smallest behavior patch meets the requested lead
without that evidence. Chat judges a later bounded task; no raw-full-log request,
new input capture, future movement/c73 or owner game/update instructions here.

## Verification commands / expected / actual

Read AGENTS required manuals, state/result, latest NEXT and r1/r2/r3 analyses;
Wisdoms WALLSPRINT/MSM2_DEVELOPMENT_MANUAL/INTERNALS reference (read-only checkout
afb1513c80edff9bf11f19b7a36baf3894bb6429). No new reusable engine fact or Wisdoms
change; findings are local source semantics and supplied timeout evidence.
Targeted `nl -ba ... | sed -n ...` and `rg -n ...` reads of actual helper, native
source module, telemetry, source_lifecycle, hook_thunks, immediate_reset_trace,
sdk_bridge and INI support analysis path inventory. This is STATIC review, not
an executed game timing measurement. Two exploratory rg reads used nonexistent
snapshot/input header guesses; corrected to actual sdk_bridge.hpp/source files,
no fictitious API/interface inferred.

| Command / procedure | Expected | Actual |
| --- | --- | --- |
| `git diff --check` | no whitespace errors | exit0; PASS |
| `git diff --exit-code 08028ff -- source environment BUILD_MANIFEST.json MSM2-WS-v0.1c38r3.zip MSM2-WS-v0.1c38r3-source.zip MSM2-WS-v0.1c38r3.script.disabled` | source/environment/manifest/artifacts unchanged | exit0, empty diff; PASS |
| Python identity check below | existing script/DLL match manifest and package0.1.59 | exit0; PASS |

Exact read-only identity check executed:

```bash
python3 - <<'PY'
from pathlib import Path
import hashlib,json,zipfile
m=json.loads(Path('BUILD_MANIFEST.json').read_text());p=Path(m['artifact'])
assert hashlib.sha256(p.read_bytes()).hexdigest()==m['sha256']
with zipfile.ZipFile(p) as z:
 assert hashlib.sha256(z.read('WallSprint.dll')).hexdigest()==m['dll_sha256']
 assert json.loads(z.read('info.json'))['version']=='0.1.59'
print('PASS unchanged v0.1c38r3/package0.1.59 script/DLL identity against manifest')
PY
```

Script SHA256 d17b24b8a8aef06dbb0a5f41302cbcfcf0cdf7f68e09f5a2e0b5c670bcc427cc;
DLL SHA2569d624d56d71d60e8f44ea354b8228fc05bc531889febc34076d98213968dbe40.

Automated review checks3 PASS; manual path/condition inventory completed.
SKIP7: test_reversal_heading.sh, test.sh, build_windows.sh, smoke_windows.sh,
test_native_source.sh, test_hooks_windows.sh and release.py intentionally NOT
rerun: task explicitly says docs-only must not rebuild unchanged binaries. No
source/test or package changed; no previous900 PASS run represented as current.
UNAVAILABLE1 evidence group: authenticated pre-motion intent / owner frame timing;
no access/authenticated data, not inferred from appearance or provided excerpts.
BLOCKED0 for this review. Behavior advancement remains evidence-blocked as above.

All A/B/native admission, stale/cleanup, C52 and five-second gates unchanged.
No new output artifact or runtime log/version. Owner-side next operation: none
for this review; no Update-WS/game run required. Detailed current docs live in
Git; prior ZIP documentation is historical and intentionally not regenerated.

## Publication

Normal commit/push, then independent remote fetch/head and changed-blob comparison;
actual commit recorded below. No legacy untracked archive/bundle staged or removed.

Review commit `0aeb9f6f9945ee8d4dbed5a04bf49da230722df1` pushed normally:
08028ff->0aeb9f6. `python3 /tmp/ws-start-publication.py` exit0: independent
GitHub fetch in /tmp/ws-steering-docs-publication, FETCH_HEAD/ls-remote/local HEAD
match; all3 changed blobs from08028ff equal local files. The script fetches the
named development branch and compares `git show FETCH_HEAD:<path>` bytes for each
`git diff --name-only 08028ff HEAD` path. Same verification repeated after this
record-only commit. Final SHA in chat avoids self-reference. No rebuild/repackage.
