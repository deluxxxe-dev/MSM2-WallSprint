# c38 — bounded register-affine exact-slot candidates

Base7ed084f / codex/v01c20-edi-value-provenance, no main merge.
Read c28/c37/current scanner and accepted evidence. VERIFIED-STATIC/OFFLINE owner
c37 scan authenticated image/slot but direct-only candidates0, bytes17859460,
gaps737, unresolved1279672, incomplete coverage/writers/base/lifetime/containment.
Zero direct candidates is not absence. VERIFIED-RUNTIME c37 confirms offset range;
early upward sessions do not invalidate startup instruction proof. No new A/E.

## Exact local static representation

Only offline tools/slot_publication.py semantics change.16 GPR parents carry an
optional exact image RVA, reason and hop count, not memory contents or ABI types.
Supported RIP-relative LEA -> full64 GPR, full64 register copy, checked LEA from
known full64 base + signed displacement, no index/segment/address32. Resolve the
entire operand interval within image and without modulo64 wrap. No ADD/SUB,
absolute-immediate pointer or other arithmetic is required/supported. No memory
value is read, even if the source slot address is known. MOV reg,[memory] records
an address reference if provable, then invalidates the destination as loaded-
unknown. Read sources before invalidating destinations for self-copy/affine LEA.
Every partial/unsupported write kills the whole parent, never a stale pointer.

Bound lifetime: control jump/call/return/interrupt/system transfer clears ALL
origins, no ABI preservation or call-return value. Decode gaps, runtime-region
entries/ends,64instruction window and8hop chain also reset/refuse. No call is
followed. State never crosses excluded spans (fresh state per span). This is a
local linear-block provenance conditional on that decoded sequence, NOT global
execution-entry/dominance proof. Unscanned indirect entries/overlaps remain why
candidates/all-writer proof cannot be complete; no generic CFG/dataflow engine.

## Output and c28 external anchor

Reference candidates only when full effective image interval overlaps the exact
8byte slot. Report site/width/decoded-conservative access/may-write/source class/
effective RVA/address proof class (direct_rip/direct_absolute/register_affine_static).
Address-only LEA references are distinguished from memory operands. No bytes,
raw values/private paths/full bodies or semantic pool/array/allocation names.
Direct and register-affine writer counts are separate, plus aggregate writers.
Unresolved primary-reason counts partition aggregate operands: unknown base,
loaded base, partial/unsupported mutation, boundary loss, indexed, segment,
address width/outside image/wrap, window/hop/unknown-effects. State resets are
separate counts; excluded region/gap/cap metadata stays explicit. These are not
alias absence claims. selected_byte_sweep_complete is not candidate_sweep_complete;
candidate_sweep_complete and writer_enumeration_complete always remain false.

Known c28 consumer0x2CF7FE9 requires its accepted parent RCX image-root16 RVA
0xC2D2480 plus displacement0x100. Pure callee-entry scanning has no such RCX fact.
Do not hard-code a local result or inject origin into sweep state. After exact
image authentication, independently check only the current7byte MOV RAX,[RCX+100]
load/form/access/site in unique readonly executable code; combine with explicitly
accepted c28 parent evidence in external_anchors, class authenticated_external_anchor.
If current instruction mismatches/unavailable/excluded, refuse the anchor. This is
separate accepted external evidence, not a local scan hit or a writer/publication
claim. Tests own a synthetic LEA parent chain to resolve base+100 locally, while
an isolated callee consumer stays unresolved without the external fact.

Anchor work is bounded/reserved WITHIN original total caps:7bytes/1instruction/
at most1record, subtract before general sweep. Root bytes/instruction counters
include this work, coverage does not. File512MiB/code64MiB/5M instructions/
256records/runtime600000/8MiB limits unchanged. Deferred producer containing
runtime span (or whole executable section if no boundary) remains excluded and
never decoded/followed. Runtime entry/end metadata resets state as well.

## Evidence distinctions / safety

Owner c37 scan and accepted c28 address proof are supplied evidence. c38 tool
success below is owned fixture evidence only: no authenticated actual EXE in this
cloud, so new real-image candidate counts and unresolved distributions UNKNOWN,
not synthesized from prior counts or external anchor. Actual owner rerun is the
next evidence, compact JSON only. No need for executable upload/live observations.
Candidate writers/readers are starting points for a FUTURE bounded publication
chain, not permission to follow arbitrary calls or traverse producer0x351EB00.
Multiple/one/zero candidates cannot establish uniqueness/no writers/allocation/
extent/lifetime/ownership/disjointness. Even a complete selected byte pass cannot
prove every possible indirect writer or instruction entry. No candidate report
is imported automatically into native admission or memory/address proof state.

Native c37 offset0..0xFFFC0/stride64/endExclusive0xFFFE0 and c36 escape1/pending1/
contained0/control-alias1/CFG0 are byte-unchanged. No new opcode/typed ABI/raw
qword/vector/register values/pointer chase/generic memory model/MOD writes.
All ownership/lifetime/conflict and Position/source/input/movement/adhesion/
camera/animation/physics gates0. No shared tooling/SF/ContextualIdle/ordinary mod
changes. No WS main merge; NEXT preserved.

## Owned validation / next owner action

20 Python groups: retained8 direct/hash/overlap/cap/deferred tests plus12 new
RIP-LEA read/write, full copy, signed affine/self update, adjacent/outside/wrap,
loaded base, partial/full unsupported mutation, branch/call/indirect/system reset,
indexed/segment, window/hop/runtime/decode, current external anchor mismatch/
pinned decoder. Owned PE integration reserves anchor caps, keeps anchors outside
local references and embeds a synthetic producer writer that stays excluded.
No proprietary body fixture. Full native/workspace regressions and independent
source build/focused/package checks in CODEX_RESULT / V01C38_RELEASE.json.
Initial owned negative-displacement fixture wrongly expected an off-slot address
to appear as a slot candidate; corrected its synthetic origin/expectation, no
production policy or prior assertion weakened. Final focused20 pass.

After push: Update-WS and rerun offline scanner only, using current c38 BUILD.md
PowerShell block. Return logs/MSM2-WS-v0.1c38-slot-publication.json to ChatGPT.
No WallRun/A-E/Install mods/Run modded game cycle is needed for this tooling task.
