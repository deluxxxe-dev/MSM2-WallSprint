# r6 — downward motion / orientation interaction isolation

2026-10-31 JST; baseline66f203807fd2f9f5afb95bf603b5275b13433225.
Branch codex/v01c20-edi-value-provenance; no main merge.
Owner evidence: R5_NUMBERED_OWNER_RUNTIME_20261031.md; prior original-route
review: FLAT_WALL_LATERAL_ROUTE_REVIEW.md. This supersedes old tooling-next-action
and r5 deployment-unknown wording for the accepted owner game session only.

## Established explanation and exact remaining boundary

VERIFIED-RUNTIME (supplied): owner reports down-input, but session4 accepts
horizontal +Z/-Z targets from real deltas0.220336914/-0.161712646/0.170776367/
-0.225021362, speeds3.44–4.79, not tiny subthreshold noise. At17.828 it accepts
(0,-0.204101562,-0.960220337), proving SOME down component exists. Up path also
executes. Exact stick/video/native clock correlation is absent.
VERIFIED-STATIC: heading::Reversal projects world displacement onto saved wall
normal, confirms measured direction, then publishes a proper basis through A/B.
The source adapter copies rows in the established Right/Up/Forward order. No
screen-axis/sign shortcut, vertical special refusal, or pure-math defect accounts
for these horizontal observations. Targeted owner-value replay and existing
three-wall down/diagonal/antipode/compact tests support this narrow conclusion,
not native causality. No threshold/timer/basis repair is justified.

HYPOTHESIS A: original native steering/contact can produce horizontal travel in
this maneuver. HYPOTHESIS B: WS's published Actor/source frame feeds later native
movement-coordinate calculation. Could also combine. Neither is established.
A/B overwrite orientation, preserve Position/W and B scale/metadata; that alone
proves neither independence from movement nor causal feedback. Incoming A/B axes
may already contain earlier WS output: never label them independent input intent.

The authenticated Update entry ends0x14246d6; four known branches target missing
0x14248f9. No current original consumer body proves the movement frame. Prefix
calls remain untyped/unidentified. No new pointer/object read, signature, hook,
input inventory/c73 sweep or guessed motion API is added.
ONE precise offline acquisition remains if original semantics must be resolved:
from exact on-disk EXE SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9,
read executable/file-backed RVA0x14248f9 max128 bytes, cap at earlier section/runtime
boundary and first relevant control boundary; attach block SHA, exact range and
covering runtime/unwind metadata. Existing slot_publication.parse_pe/file_range
and pinned Capstone5.0.6 are primitives, NOT permission to run analyze_image/sweep.
No matching EXE/current continuation exists in this cloud evidence; this read is
UNAVAILABLE. No full EXE/raw body upload/commit is requested. Chat can arrange this
single bounded offline helper action; another game startup captures only the same
entry fragment. The separate fallthrough at0x14246d6 remains unresolved.

## Minimal runtime diagnostic, not a downward movement fix

Version v0.1c38r6/package0.1.62. R5 heading math/constants unchanged byte-for-byte.
A worker-owned value-only ring receives existing snapshot position/Actor axes,
surface normal, accepted displacement target and published heading, event,
session/generation/native update/native timestamp/GetTickCount64/pause-aware clock.
All vectors/delta/tangent in a record share that snapshot/baseline; this is NOT
an atomic native frame, post-writer callback, physics velocity or animation basis.
Independent100ms diagnostic baseline samples even during turns and after eight
heading holds, without adding confirmation votes or affecting source publication.
A gap remains visible in dt_ms; regression rebases. Native lifecycle and close
reason are recorded in final summary and existing lifecycle logs.

16-record ring, minimum100ms/distinct update, 2s windows, max16 windows/session
(max256 sample lines +16 periodic summaries +one final summary). Partial window
flush on close; overwritten/throttled/budget-suppressed totals explicit. After
budget depletion no sample logging; final summary reports suppression. New session
resets buffer. Original heading64/hold8/movement40 caps remain; new late snapshots
are independent diagnostic evidence, not a claim of complete event coverage.
PATH_B_INCOMING_BASIS uses existing captured validated compact rows only, first64
writes, with session/generation/update/ms/commit QPC. A already logs all original
axes in NATIVE_SOURCE_OVERRIDE. No additional game-memory access in either path.
Raw B row lengths are retained; comparisons must normalize using existing scale
metadata. Published heading in worker means proposed publication, not proof of a
later A/B commit; correlate existing commit_qpc/writes and lifecycle identity.

What distinguishes hypotheses: compare world tangent, actual worker Actor basis,
measured target/publication, and incoming A/B rows across native update identities.
It can expose frame/translation ordering/correlation and late hold intervals. It
cannot by itself establish input consumption, native use of axes, natural vanilla
contact behavior or final rendered facing. Those still require the bounded native
consumer proof / separately controlled owner comparison. Do not promote temporal
correlation to causality. No unchanged r5 test repeat requested here; Chat supplies
numbered comparison after implementation.

## Preserved boundaries

Speed2/33ms/two samples/120ms/12deg/8deg, entry0.25s, native lifecycle/Deinit,
stale/identity/owner/generation/4096/A-B proper-basis/C52 rules unchanged. No
Position/velocity/gravity/input/animation writes; no target-down forcing, speed
change, neutral/RT idle retention, direct vertical entry, diagonal gain or camera
work. These remain backlog. No SF/shared toolkit/profile/DLL mutation.

Validation and publication are recorded in docs/CODEX_RESULT.md. Fixture coverage
proves buffer/adapter contracts only, not game ownership or downward success.

Diagnostic refinement: record both pinned projection normal and existing sampled
owner normal. Preserve latest non-hold decision with its OWN timestamp/update,
so100ms snapshot throttling cannot silently lose low-speed rejection reasons.
Decision time may precede snapshot; it is not same-frame input evidence.

Timing limitation: bounded log bursts can perturb worker/I/O timing on real Windows.
Owned31s continuation is regression evidence, not a zero-observer-effect guarantee.
Final summary lifecycle_state is the state observed on entry to end(), before the
existing close operations; correlate the existing native Deinit/exit records for
actual closure. Diagnostics themselves never authorize continued publication.
