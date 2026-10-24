# c21 exact read-dependency diagnostic

**C21 ACTUAL RESULT ACCEPTED:** V01C21_RUNTIME_RESULT.md supersedes the
original preparation/pending notes below: exact Transform+0x10C MOV intoEAX,
possible control0x2B23583, next TEST0x2B235BE operand/flow blocker. Full XYZ/W,
cross-invocation and ownership remain UNKNOWN. C22 preparation is documented
separately in V01C22_READ_TEST_FLOW.md.

## Evidence and scope

VERIFIED-STATIC under accepted actual-game c20 identity: EDI at0x2B2358E comes
from0x2B2356F, a memory load of exact executable-image globalRoot16+0x0C323788.
This narrows value origin, not the semantic meaning of EDI or Transform+0x10C.
The current full-effect blocker is the earlier read0x2B23578. See
V01C20_RUNTIME_RESULT.md. Its opcode/address/carrier remain UNKNOWN in cloud;
no private function body is requested, reconstructed or published.

C21 adds a diagnostic-only sidecar under the existing authenticated current
callee0x2B23560..0x2B235E9/full hash and exact writer/call/exe guards. It reads
only the existing authenticated static code snapshot, never live game data.
Legacy State/Slice/address/value admission, readMayDepend/recordRead and all
c19/c20 certificates remain unchanged. The sidecar cannot set a call no-write,
Position-protection, relevant-effects, XYZ/W, ownership or writability flag.

This task prepares bounded Outcome C: precise read/order/possible-value-flow
metadata and its first narrower unsupported/surviving-carrier/feedback blocker.
Even a dead value in owned local fixtures does not bypass the existing read
dependency gate. Actual A/B/C adjudication needs the new authenticated game log.

## Structural ordering versus data flow

The already validated fully decoded forward CFG is enumerated along both Jcc
successors. Ordering is conditional on CALL normal return/fallthrough, with no
call expansion. A path on which a call does not return is not a certificate of
subsequent execution. Scope is one local invocation, not nested invocations.
Per-path read/write occurrence is retained at each RET. Order mask:

| Bit value | Meaning |
|---|---|
| 1 | selected read before exact write on a paired local path |
| 2 | selected read after exact write on a paired local path |
| 4 | selected read without write on a local path |
| 8 | write without selected read on a local path |
| 16 | neither on a local path |

Mixed presence is reported separately from mixed paired order. Valid forward
single-site CFGs cannot reverse two fixed instruction addresses; backedges and
nonlocal transfers retain the existing refusal. Branch fixtures also exercise
additional same-field reads before/after the write at distinct sites.

A separate conservative possible-dependence walk uses copied Slice state,
Capstone register access and bounded stack tags. It does not expand CALLs or
guess unsupported instructions/partial-register effects. Masks distinguish the
selected read value(1), a later same-field value after the write(2), and other
pre-write same-field reads(4). They are diagnostic origins, never pointer types.
Complete self-zero idioms kill dependence; partial stack overwrite conservatively
retains surviving-byte tags. No algebraic or branch-condition guessing is used.

GPR/XMM/flags and caller-stack values that may survive a local return are reported.
Private dead-frame slots are distinct from caller/home slots. The first possible
use, flag-dependent branch, escaping memory effect or unsupported CALL/instruction
is reported. Possible taint flow is not a numerical or causal XYZ/W proof.
Structural order completion does not imply data-flow completion or call effects.
Order coverage is local to the analyzed writer call inputs, not a substitute for
VALUE/WRITE/ALIAS/WRITER's separate whole-writer coverage. Same-field order mask
1/2/3 means observed before/after/both in the analyzed flow; an incomplete walk
does not prove that an absent read/order is impossible. If the selected read is
itself after the write, its selected tag and post-write tag are both retained.

Each ordering/data-flow walk has4096 cumulative visits,9 pending states and the
existing complete CFG's8-branch ceiling. Data flow has128 stack tag slots.
Counters aggregate over the existing maximum16 analyzed writer call inputs.
On a cap, no completeness is inferred; the visit that detects overflow may report4097.

## Read metadata and boundaries

For the selected0x2B23578 instruction, compact metadata includes mnemonic,
operand index/class, width, base/index/scale/displacement, address root/offset,
carrier register or flags, and overlap with Transform+[0x10C,0x110).
Overlap uses1/0/-1 for yes/disjoint/unknown.

Exact Transform roots permit an exact numeric offset comparison. Source/stack
disjointness remains conditional on the existing runtime TEB exclusion, and
image globals on authenticated image bounds/region exclusion. Other/unknown/
indexed/truncated/conflicting addresses do not obtain exactness. Conflicting
states are marked same_address=0; displayed operands/address are the last analyzed
state, not a merged canonical address. No semantic field/type is invented.

No later-invocation persistence is proved. The new write, pre-write read and
later reload are separate tags. A remaining returned register/flags/caller slot,
control branch or memory escape requires the downstream writer/effect proof;
an expanded callee or cross-invocation model is not silently introduced here.

## Logging / test build

New records emitted by the existing worker prepare/report path:

- MOVEMENT_CALLEE_READ_DEPENDENCY: enabled/decoded/address/carrier/order mask,
  all local order paths examined and normal-return-only scope.
- MOVEMENT_CALLEE_READ_FLOW: completed local flow, possible return masks/flags/
  caller stack, new-field-value mask, field-order mask, possible first uses and
  exact unsupported blocker. It always reports effect_certificate=0,
  downstream_XYZ_W_relevance=unknown and cross_invocation_feedback=unknown.

Eight owned fixture groups cover the task's required before/dead/surviving/
after/mixed-path/disjoint/unknown/conflicting/c20/c19 negatives, plus flags,
caller homes, partial spill/partial-register, later reload, CALL and cap cases.
They are independently authored nonexecuted137/183-byte fixtures, not game bodies.
Full validation results and exact commit/package hashes are in CODEX_RESULT and
V01C21_RELEASE.json. Package label v0.1c21 / version0.1.39; normal Update-WS flow,
one active root script, archive-excluding independent clone, shared Overstrike
and canonical versioned logs remain. The logger fixture now derives the label
from authoritative version.hpp while independently checking timestamp/path/suffix.

## Human capture and unchanged gates

Update-WS -> Install mods -> Run modded game. A/cardinal about4s -> fully end
WallRun -> move/fast-travel -> E/diagonal about4s. Return the newest complete
MSM2-WS/logs/MSM2-WS-v0.1c21-YYYYMMDD-HHMMSS.log privately. No body dump/video
is needed unless a visible anomaly occurs. Check new READ_DEPENDENCY/READ_FLOW
alongside VALUE/WRITE/ALIAS/WRITER and existing finalization/cleanup/safety records.

writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent remain0; Position/source-position/input/
movement writes0. Producer0x351EB00, movement, adhesion, camera, animation and
physics remain deferred. Actual c21 read classification and causality remain
PENDING; accepted c20 EDI origin and c20 runtime safety evidence remain intact.
