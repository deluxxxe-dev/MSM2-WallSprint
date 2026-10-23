# c13 entry-source candidates — correlation, no write contract

## Bounded origins

Each admitted true entry records raw16 GPRs, XMM0..15,64 caller-stack argument bytes at entry RSP+0x28, three pointed16-byte snapshots at RDX/R8/R9, and Hero Position4 before body execution. No pointer chain or generic memory dump is followed. Raw capture never calls a candidate velocity/displacement/direction/root motion.

The worker compares27 fixed origins:16 XMM Vec4 lane groups, four groups of four GPR low32 bits, three direct argument pointers and four16-byte stack groups. It emits readable flags, exact raw bits/value4, exact written-component mask and origin/offset. A16-bit lane_to_store_match_matrix additionally compares every entry lane with every observed component (bit lane*4+component), exposing scalar/swizzled correlations such as XMM2.lane0 matching Y without pretending a Vec4-aligned source or causal feed. Current-Position equality and observed four-store equality are distinguished; a delta_like label requires finite nonzero XYZ change and component correlation, and is never an engine displacement type. W is compared bitwise without naming its meaning.

Only four anchored stores belonging to one explicit entry invocation can supply a full mask. Written values are decoded from store-source register/immediate context; guarded post-store target snapshots may include concurrent/other-writer effects and are not substitutes for those values. Missing/duplicate events remain partial/rejected. Cap/drops/unwatched threads and incomplete original semantics cannot establish absence of conflicts.

`stable_across_invocations=0`, `stable_across_surfaces=0`, `source_stable=0`, `typed_semantics=unknown` and `writable_source_proven=0` remain conservative. Repeated exact matching origins are counted separately; repetition/equality is not stable storage provenance or causal exclusivity. Actual A/E comparison must be adjudicated from human c13 records. The finite-difference SurfaceFrame record is diagnostic and does not identify native velocity.

## Caller2 classification

C12's verified primary return0x351EEB0 remains mandatory; second observed return is recorded by bounded unwind at the real entry and checked again when stores are paired. The second return need not equal a direct E8 expectation. Startup fully decodes a bounded caller2 function and anchors its actual call instruction. An E8 targeting the exact containing caller is direct; a register/memory CALL is indirect; wrapper requires a direct call to an exact guarded E9 tail jump to that caller; everything else is unresolved. Runtime stack verification and static relation flags are independent, so c12 direct_target_mismatch is never converted into a guessed wrapper.

## Source admission / next exact edge

Actual c13 game entries are PENDING. Cloud owned fixtures prove the mechanism and locate a direct argument-pointer copy only in independently written test code; they are not evidence about the game function. No actual game candidate is authenticated or writable.

If a real origin matches the four store values on A and E, the next edge is the origin's construction/lifetime in caller0x351EEA7..0x351EEC5 and its observed caller2 producer path. If only partial or no match occurs, use the exact component masks, raw entry registers/stack offsets and captured store-source values to identify the missing transform/computation (including unsupported writer+0xA) rather than guessing semantics. Full original CFG/XYZW causal feed, source lifetime, native Hero/session/thread ownership, downstream conflict and exclusive intended effects remain necessary for a future separate proof/write task.

Path A source+0x30 stays rejected. Existing owner+0x320/+0x338 remain reference points, not velocity fields. Position/input/source/movement/adhesion/gravity/camera/animation/visual contact remain unchanged. No future rewrite is enabled automatically.
