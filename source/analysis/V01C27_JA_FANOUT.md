> Accepted c27 actual: short JA0x2CF7FE7 disp42 -> taken0x2CF8013/fall0x2CF7FE9, both forward edges admitted for fan-out; first fallthrough MOV memory effect refuses,0 returns/identity. See V01C27_RUNTIME_RESULT.md and successor V01C28_MOV_CLASSIFICATION.md. Earlier pending-edge statements below are preparation history.

# C27 exact JA edges and bounded all-path leaf proof

## Accepted evidence

VERIFIED-RUNTIME diagnostic on exact admitted Spider-Man2.exe2.810.0.0
(SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9):
c25 fully validates527161 entries/6325932 bytes, queries actual OS lookup and
finds no containing runtime entry at0x2CF7FE0, next region0x2CF8020. C26 classifies
CMP r8d,0x3FFF at0x2CF7FE0, flags-only CF/PF/AF/ZF/SF/OF, all relevant R8/flag
tags0. Next exact JA0x2CF7FE7 uses CF/ZF from that CMP, condition tags0; numerical
outcome UNKNOWN. Target span/hash/effects unadmitted. Source:
V01C26_RUNTIME_RESULT.md. C26 native Deinit/retention passed with hard kills and
Position/source-position/corrections0. Neither branch target nor path bytes
are in compact evidence. Do not invent them or request a complete game body.

## Edge classification before fan-out

Only pinned JA0x2CF7FE7 with its exact accepted CMP producer is eligible.
Describe short77 rel8 or near0F87 rel32, encoded lengths/displacement, exact
signed arithmetic taken and fallthrough, agreement with full decoder, both
forward relations, code-section/128-byte-window inclusion and runtime crossing.
Check signed RVA underflow/overflow; no wrapped edge. Existing PE full-table,
static/dynamic runtime-region limits and stable double snapshot run first.
No candidate code is read past the first authenticated runtime-region boundary.
No semantic function or ABI names follow from an RVA/register index.

## Current-state tag gate / bounded two-pass owner

C26 tags0 are accepted for that capture, not a permanent assumption about R8.
Initial current-code proof classifies exact edges and stops at the branch if
current call tags are missing. The existing parent read sidecar gathers current
GPR/XMM tag unions from analyzed call states. The runtime owner may then reprove
metadata/current code once with those tags, checking equality to the first
snapshot, reset analysis to its original seed and rerun the parent sidecar.
At most2 proof/parent passes. Entry tags contain only1/2/4, samples must be nonzero.
No data dereferences, new hooks or live game calls are used to obtain tag state.
This is bounded source analysis, not CPU invocation counts or a global absence
claim about unsampled/unresolved parent paths. Any incomplete parent proof stays
incomplete. Effects recheck condition tags independently on each actual analyzed
state even after identity passed the earlier aggregate. A dependent state refuses.

## One exact split, all reachable returns

Hard ceilings:1 pinned conditional,2 paths,128-byte target allowance,32 unique
reachable decoded instructions,64 cumulative visits. Both exact forward edges
are always explored; numerical outcome is never guessed, including tags0.
Owned per-path Slice/ReadFlow state is cloned before either leg executes.
A byte ownership map rejects overlapping decode/instruction-body targets even
if one path already returned. Decode only reachable instructions; intervening
unreachable padding is not executed or used as an instruction stream.
No second conditional, JMP/indirect transfer, loop/backedge, memory operand,
stack construction/change, nested CALL, partial write/unmodeled flag consumer
or unsupported effect is admitted. The c26 full-GPR read-only CMP model and
existing register-only leaf operations are the only data semantics.

No target span exists until both paths end in plain unprefixed C3 RET. A third
guarded read checks the whole bounded snapshot, followed by runtime coverage
recheck over the entire selected contiguous span (including gaps). Only then
begin/end/size/hash may represent the complete bounded reachable exit set.
Unused gap bytes belong to the hash/span, never a claim of modeled effects.
Stable current snapshots are not atomic execution attestation and do not rule
out transient future code writes. Runtime-boundary identity for ordinary targets
and legacy linear proof remain unchanged when the pinned branch is not used.

## Call effects and conservative merge

Only a fully admitted span feeds c23. The common bounded walker re-decodes both
reachable paths, repeats current-state tag refusal and leaves the original
caller untouched if either fails. Preserve c23 stack/return-slot alias limits
and modeled CALL/normal-return SP bookkeeping; no guessed volatile ABI clobbers.
Union returned selected/postwrite/other GPR/XMM/flags tags and clobber masks.
Different abstract value origins merge to unknown; tags retain every possible
carrier. Record each RET RVA/effect and explicit path dependence. Across analyzed
parent states per-path summaries also union; never choose the convenient leg.
No local completion issues an effect/write certificate or resolves whole writer
XYZ/W, cross-invocation, caller storage/lifetime/exclusivity or writability.

## Owned fixtures / evidence limits

Seven new groups in identity suite (28 total), existing21 read/call and c19-c26
regressions retained. Independently authored opcode fixtures, no captured body:
short/near exact edges, signed bounds/code/window exits; current tag1/2/4 refusal
and per-state recheck; two identical/different live-killed return effects in both
orders; backward/self edges; static/dynamic runtime crossings on either edge;
overlapping instruction bodies; one safe RET plus each forbidden effect/second
branch/missing RET; instruction/path/byte bounds; double/final snapshot and
runtime coverage changes; accepted CMP regression and full527161-entry table.
Main/full-chain and actual source-ZIP rebuild counts are in CODEX_RESULT/RELEASE.

Actual C27 edges/path results remain UNKNOWN/Outcome C until compact live guarded
metadata exists. Request only branch-edge/CFG/next-barrier and existing safety
records from a shortest A/E ~3s capture, each below unchanged5s. No full target
bytes or unnecessary repeat test if future accepted evidence is sufficient.

## Safety and operational baseline

Position/source-position/input/movement writes0, all writable/caller-owned/
ownership/lifetime/exclusive/downstream-conflict gates0. Producer0x351EB00 deferred;
no adhesion/camera/animation/physics advancement, PAGE_GUARD, guessed types,
third-party body/code reuse or SF/ordinary mod changes. Preserve incoming7f0c4d9
Overstrike updater/WINMM repair helpers and all workspace sparse/archive rules.
