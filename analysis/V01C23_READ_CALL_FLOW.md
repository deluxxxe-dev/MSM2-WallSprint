# c23 pinned CALL classification / one register-only leaf

## Accepted input / missing evidence

V01C22_RUNTIME_RESULT.md accepts TEST r8b/r8b0x2B235BE -> JE0x2B235C1,
parent/flags/consumer tags0. It resolves only that pair. The read-flow suffix
stops at CALL0x2B235D4; one other normal-return path retains selected EAX.
Those partial-path return masks and memory_escape0 do not certify the CALL path.
Exact target/arguments/body are absent from compact c22 evidence. They remain
UNKNOWN here; no private function is reconstructed, requested or published.

## Static metadata / identity

The current authenticated137-byte parent snapshot pins only0x2B235D4. A full
decode confirms CALL and instruction/return boundaries. Direct immediate targets
are decoded as image RVAs; indirect forms are named but never pointer-chased.
All16 raw GPR possible-dependence tags and address/value provenance,16 XMM tag
masks, flags and possibly visible parent-stack tags are aggregated over reaching
states. Displayed origins are the last state, with same-origin/conflict markers;
tags are unions. Kinds/roots are analyzer coordinates, never game ABI types.

Pointer root1 denotes Hero Transform, root2 writer-entry RDX source, root4
writer-relative caller stack, root16 image-global. Unknown roots and loaded/raw
bits are not declared disjoint. The conventional Win64 argument and volatile/
nonvolatile masks are reference carrier sets, not a guessed function signature.

At configure only, an exact in-image target may obtain runtime-function metadata
and at most128bytes of current code, SHA256 and a second guarded byte comparison.
Entry must equal the function begin. Overlapping parent/writer functions and any
function containing deferred0x351EB00 are excluded before code reads. An exact
0x351EB00 target is excluded before lookup. Larger/missing boundaries report
their exact metadata/refusal. No other edge is captured. This is startup current
snapshot identity, not an independently pinned historical hash, live execution
trace or later per-invocation effect certificate. All old identity gates remain.

## Minimal optional effect model

Only this supplied authenticated leaf can be evaluated, depth1. Full decode is
limited to128bytes/32instructions;4096 cumulative nested visits over all parent
states. Existing outer4096visit/8branch/9pending/16writer-input limits remain.

Supported: full GPR register copies, MOVD/register vector copies, full self-XOR,
and32/64bit register CMP/TEST with modeled flags, plus NOP/ENDBR64 and RET.
Existing Slice semantics must also accept each instruction. No partial register
writes, stack adjustment, memory accesses, local branch, nested call, recursion,
unknown flag consumer or general CPU emulation. A normal RET must be reached.
Exact byte effects determine preserved/modified registers; an unknown call never
clears volatile tags and continues. Return GPR/XMM/flags retain masks1 selected,
2 post-write same-field reload,4 other pre-write same-field read.

CALL's implicit return-address slot is modeled only in the abstract stack:
known root4 SP, bounded[-4096,4096], no protected writer return/source overlap,
then an untainted constant return slot. Nested entry SP is caller SP-8 and the
unchanged balanced register-only RET restores it. No process memory is written.
Failure discards the temporary state; successful local effects may continue the
outer sidecar only. Old State/Slice/c19/c20 certificate logic is unchanged.

Every explicit memory operand is an exact next boundary. Report known Transform,
source-span, caller/home/return or unknown/indexed alias separately. A store
whose address/value may carry selected tags records possible memory escape;
no numerical/causal claim or safe write permission follows. Even private-memory
effects stop outside the register-only model. Unsupported instructions and
further CALLs report their exact nested RVA/mnemonic without following them.

## Output / proof boundary

- MOVEMENT_CALLEE_READ_CALL: form/operand, exact target and boundary/current hash
  when available, state/model counts, flags/stack tags, exact effect blocker.
- MOVEMENT_CALLEE_READ_CALL_CARRIER (16): GPR index/tag/kind/root/offset/conflict,
  alias class and same-index XMM tag. No live register payload/memory dump.
- MOVEMENT_CALLEE_READ_CALL_RETURN: all three return-tag masks, exact modified
  GPR/XMM sets over analyzed prefixes, returned flags and explicit modeled-
  normal-return coverage for the return masks only.

Existing READ_FLOW still reports global completion/partial returns/next barrier.
All new records fit the4096-byte logger limit. A leaf returning selected RAX or
XMM proves only that narrow modeled dependence, not protected XYZ/W relevance.
Local completion cannot eliminate prior control dependence or cross-invocation
feedback. The old read/effect refusal cannot be waived by this sidecar.

Prepared bounded Outcome C: exact actual target/effect/next barrier require the
new metadata because compact c22 evidence omits them. If an authenticated leaf
fits, the diagnostic advances until the next boundary/return; it never grants
whole-writer XYZ/W, source ownership, lifetime, exclusivity or mutation permission.
Producer0x351EB00 and Position/source-position/input/movement writes stay0/deferred.

## Owned tests / human capture

Five additional independently authored fixture groups cover direct selected/
disjoint inputs, returned GPR/XMM/flags, exact killed carriers, caller-home escape,
Transform/source alias refusal, indirect/missing identity/partial/nested CALL,
caps/no RET/indexed unknown alias. All16 c21/c22 groups remain. No fixture contains
captured game code. Full results/hashes: CODEX_RESULT/V01C23_RELEASE.json.

The cloud has no authenticated game snapshot containing this target, so a new
capture is necessary for these new fields, not a routine rerun of the resolved
TEST question. Update-WS -> Install mods -> Run modded game; A~4s -> fully end
WallRun -> move/fast-travel -> E~4s. Return newest complete private
MSM2-WS/logs/MSM2-WS-v0.1c23-YYYYMMDD-HHMMSS.log. New fields are only the three
READ_CALL record families. No body dump/video unless a visible anomaly occurs.
