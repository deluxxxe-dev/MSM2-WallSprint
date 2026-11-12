# M17 safe contact / prompt detection / stop / live camera / position mirrors — 2026-11-12 JST

Task baseline b992148. Read both M16 owner-result parts and M15 derived results;
repository/CLAUDE rules remain unchanged. No raw owner attachments/game execution
in cloud. Both seamless movement and retained face-gravity fall remain mandatory;
new runtime acceptance is UNKNOWN until the requested owner run.

## Task 1 — contact-bounded probe and safe landing
A release starts a0.20s probe with at most0.10m cumulative -u travel; planar
motion remains. Clean declaration also needs fresh native game feedback at/after
0.20s: worker time alone cannot certify absence of contact. Each hero SetPosition
call, including later calls in one frame, checks the game's desired-current +u
step. During probe, push>0.01m at original plane_d<0 aborts, projects back to the
ORIGINAL plane and ignores fall detection1s. During full fall, push>0.01m lands
at the saved position BEFORE the previous model step, not the already penetrated
current point. Original-plane-only mathematical landing is retired: an infinite
plane is not a surface probe. Normal face-jump plane landing remains unchanged.

The quiet-game-fall rule cannot re-place a plane deeper than original_d=-0.05m;
selected contact is handled first, providing the explicit contact exception.
Face-jump and stop-hop landings suppress new releases0.5s. B/new WS/F8 and axes
writer scope are preserved. Contact stops model velocity and returns the native
position argument; no new engine call/ABI/collision guess. Probe clean, abort,
contact and original distance/push/budget are logged. Before-step is the bounded
rollback point, not a verified engine collision position. Genuine free fall and
ledge contact still require owner evidence.

Owned actual-helper ASan/UBSan tests cover cumulative budget/planar preservation,
native feedback before clean, push thresholds, rollback classification and deep
quiet-landing refusal. The production Werror compile initially found the retired
plane-only helper unused; removed that obsolete helper/test with its retired
behavior. All retained/new tests and production compile pass.

## Task 2 — rolling game-down velocity
Replace four consecutive per-frame rises with overlap-weighted displacement/time
means over[now-0.1,now] and[now-0.2,now-0.1]. Both windows require complete native
coverage. Trigger current>3m/s and increase>1.5m/s, using actual QPC sample times;
invalid/gap/ineligible history resets. Log detect_delay_s from first full window
>1 in the current episode, and both means. If downward walking already exceeded1
for a long time, this literal episode age can include walking; it is not a
fabricated estimate of the physical fall onset. Steady<=6 does not trigger.

Launch carry is reconstructed from the newest ten qualified walking samples
BEFORE the first detection window, with a64-entry bounded history; insufficient
pre-window evidence gives zero, never the game's early fall velocity. The first
new noisy fixture exposed contamination by low-speed early-fall samples; the
pre-window selection fixes it and the same carry assertion passes. Formatting
warnings were fixed under Werror, without changing assertions/thresholds.
Owned tests cover noisy26m/s² onset/delay<=0.2, steady6m/s, gap refusal and
continued/deep/contact behavior. Production compile passes. Physical onset,
no false walking probes and the resulting contact classification remain owner
acceptance; probe safety is retained independently of detector noise.

## Task 3 — absolute stop apex and bounded post-landing observation
The stop hop solves h=0.8-d0, r=sqrt(2h)+sqrt(1.6), g_hop=r²/0.35²,
v=sqrt(2h*g_hop). Thus apex is0.8m from the plane, not start+0.15m, with the
same0.35s return. Near-face d0>=0 and<0.8 is admitted; out-of-domain starts refuse.
Normal face jump remains33/1.8m. Logs include apex/d0/v/g and actual airtime.
After a hop landing, observe a continuous native1s neutral/pinned window:
all raw game steps<1e-5m gives crawl_suspect=1, movement gives0, unavailable/gap/
new input/new air reports unknown. This is a suspicion metric, not a body-pose
classifier; the video decides stands/crawls. Probe samples never come from worker
polls. If it crawls, report it;0.95 is the next experiment, not implemented here.
Owned actual integration/math, frozen/gap/input observation and unchanged normal
jump fixtures pass, as does production compile.

## Task 4 — validate eight live setter-m0 hypotheses before use
Existing setter capture publishes coherent m0/m1 with a serialized odd/even
sequence, owner and timestamp; a busy capture is dropped, never mixed. New code
does not write the setter or discover another camera. New CAMERA_MAP switch is
code-default0/shipped1. Current m0 must be fresh<=150ms, finite unit-like coherent
rows with row1 matching face up. Owner/normal change resets validation.

While native walking>2m/s, predict from projected row2,-row2,row0,-row0, each with
both signs of forward x n. Only the existing continuous0.3s steady input/walk
window trains samples; post-landing/contact guard and world-down>6 evidence
cannot train on a fall. Keep256 samples, eight median errors. Admission is
median<10deg over>=40 samples with three row2 directions pairwise>30deg apart.
A bounded adjacency-bitset triangle test checks the three-direction condition
exactly. Lowest admitted median is used; tied candidates must agree for the
CURRENT stick, otherwise fallback rather than guessing unobserved handedness.
Old mapping remains the fallback; no unvalidated basis changes steering/zips.

Live mapping is evaluated from the latest coherent matrix for each steering/zip,
logged mapping_source=camera_live only when used. Invalid/stale/unreadable/busy/
owner-reset/sample/coverage/median/ambiguous reasons are explicit. Takeoff's
accepted walking direction and existing neutral-velocity zip fallback remain.
WS_CAMERA_MAP_CHECK every0.5s while walking,<=200/process, shows8 latest errors,
8 medians, rows,n,coverage/mask/qualification/reason even when not yet steady.
A5s full camera circle may fail the0.3s steadiness requirement; report that data,
not fabricated admission. No camera angle or candidate-position write.

Owned actual-helper ASan/UBSan fixtures validate a known candidate across3 poses,
40-sample and single-pose refusals, ambiguous current directions, owner reset,
invalid rows and256 ring overwrites; required INI defaults are checked. Production
compile caught formatting warnings, fixed without changing test conditions.
Actual setter mapping, sample coverage and zip direction remain UNKNOWN.

## Task 5 — position mirrors only, guarded one-hop objects
At each admitted snapshot, copy Transform position from its guarded0x30 row.
Read hero pointers only in[0,0x400),8-aligned/canonical/non-null, skip self and
duplicates, at most64 DISTINCT attempted objects. Read at most0x200 bytes per
object, stopping at first unreadable0x100 block; no recursive chase. Partial or
unreadable data cannot silently disappear from G/W match denominators. Buffers
are process-local in the existing static seven-label reservoir (about16MiB),
never on a native stack or dumped to logs/Git.

Query 12byte,4-aligned vectors in hero0..0xFFF and those objects. Require>=10 G
and W snapshots and>=90% matches within Euclidean0.05m of Transform position in
EACH label. Every retained G/W snapshot counts, including missing/bad reads.
Child identity is the pointer-role offset AND exact address; changed identities
cannot fabricate J/L lag. Print L/J mean distances with valid sample counts (NaN
when unavailable), rank larger L/J lag first and cap32 lines. These are bounded
correlations, NOT camera ownership/controller semantics or permission to write.
A summary gives label/position coverage and an explicit covered/unknown empty
list. No candidate position is used by movement/camera code.

The main telemetry sample/tick is100ms, so merely setting StateCadence50ms was
insufficient. Add a small diagnosticPoll declaration/call in the existing10ms
worker loop; all capture logic remains camera_ref, gated by installed exact-
build/!F8/existing default-OFF gravity-test INI and50ms cadence. It samples a
fresh Hero at50ms without increasing other camera/gameplay tick rates or adding
a thread/hook. The slower duplicate state tick is removed. Existing B/60s/loss/
exit reports/labels/finite Transform means remain. Production A/H and old intent
queries are retired after the supplied M16 covered-empty result; historical owned
helper tests remain but no real first4KiB ground/air search is repeated.

Owned ASan/UBSan tests cover root/child matches and real lag,90% denominators,
invalid Transform samples, address/path changes,32 cap,64 attempted objects,
noncanonical/duplicate/unreadable blocks and no deeper chase. Formatting Werror
findings were fixed, assertions preserved. Production compile passes; actual
camera/controller mirror list and true50ms in-game cadence remain owner checks.

## Final QA before full verification
Do not assume setter rows form a mutually orthogonal camera rotation: the
existing M6s evidence has row0 held while row2 changes. Evaluate all eight
hypotheses independently; a degenerate projected row invalidates its candidates
only. Each admitted candidate needs its OWN40 samples/three separated row2
poses/median<10. Add candidate_n and camera_owner to check lines. Tests preserve
usable row2 hypotheses when row0 is zero or nonorthogonal. Native stop-position
freeze evidence is unchanged; if native callbacks cease, the existing worker
can finish the observation as UNKNOWN after1s, never as frozen/standing proof.
Focused camera/landing fixtures and production Werror compile pass.

Consulted canonical Wisdoms main3817f5e495292e935d74a5b76d59c639c02c85cc
(WALLSPRINT_INTERNALS): it already records M16 false-fall/contact and stop-hop
runtime facts plus Transform/camera lag. No new engine fact established in cloud,
no duplicate shared edit or publication claim. All new models remain hypotheses
until the requested owner run; bounded vector matches remain correlations.

## Cloud verification on final source tree

`build_windows.sh` PASS; `test.sh` **64 PASS**; `smoke_windows.sh` **7 PASS**;
`test_hooks_windows.sh` **907 PASS**,5 historical private-data SKIP and17
UNAVAILABLE (no owner runtime fixtures); `package.py` **2 PASS**. Final-source
hook attempts: 1. Exact per-attempt exits/log hashes/assertions are in the validation
JSON. No manual skips, fixture deadline extension or assertion relaxation.

Package: `source/output/MSM2-WS-v0.1c48/MSM2-WS-v0.1c48.script.disabled`.
CRC PASS; exactly info.json/WallSprint.ini/WallSprint.dll; packaged DLL equals
the new build and FACE_CAMERA_MAP/AIR_FACING/WALKOFF all1. DLL SHA256
`a8bb540564dd319c858097153210bcd05f9c287e2931e04f98972cb4ebe8cec7`;
package SHA256 `7c391adeea99d648c89acc41de298880203202fb24e23728a9cde00eec5bd40c`.
Artifact/log hashes identify cloud outputs; they do not replace real-game proof.

Final source tree `db323df9a6433dfc8f7967b1fb2e3bc346f556c1`; task commits and QA persisted above.
Real-game owner acceptance remains UNKNOWN.
