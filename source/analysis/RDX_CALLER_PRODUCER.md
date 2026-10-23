# c14 caller-local storage / producer investigation

## Accepted facts and limitations

C13 actual A/E observed RDX=entryRSP+0x28 on all16 entries, readable pointer20/stack23 bytes with equal XYZ and W0. Exact call0x351EEAB->0x2CFC900/return0x351EEB0 and observed second return0x351A020 are authenticated. Second-return runtime unwind is verified; direct static relation remains unproven. A runtime function boundary is not proof of a C++ owner or local-variable declaration.

## Exact storage identity

C14 captures the existing thread's TEB StackLimit/StackBase with the entry. A16-byte inspected span must fit these bounds without overflow, RDX must exactly equal entryRSP+0x28 (=precallRSP+0x20), and separately guarded pointer/stack snapshots must agree. Transform must not alias that stack for a causal certificate. Reads never change protection. `stack_object_size=16` describes inspected bytes, not an authenticated engine type or declared object size.

The independent caller slice follows the exact authenticated30-byte caller prefix up to0x351EEAB. It tracks RSP adjustments/aliases and overlapping width-specific local writes. Static `caller_owned=1` additionally requires a proven LEA/MOV-derived RDX local whose entire16-byte span lies in the newly allocated caller frame above its32-byte outgoing home area and below the caller entry return slot. An inherited RDX is never declared locally owned because its runtime address happens to be near RSP. This is a frame/allocation lifetime bound, not exclusive use or thread/engine semantics. `MOVEMENT_RDX_STORAGE_PROOF` distinguishes current-snapshot identity from static allocation and ownership.

## Bounded producer edge

The startup function containing observed return0x351A020 is runtime-bounded/read/fully decoded, <=8192 bytes, under the same exact image. Its complete current bytes are rechecked at the worker before a producer record. Only the instruction ending at the return anchors a CALL. A direct destination equal to the tiny caller is direct. Up to two exact in-image executable E9/EB jumps can establish wrapper_tail_jump. An anchored register/memory CALL is indirect with target unproven; a different callee/unsupported wrapper reports unresolved plus the exact direct target and next destination RVA. There is no arbitrary pointer chase or general CFG exploration.

A separate straight-line slice of that bounded parent up to the anchored call reports any exact pre-call XYZ stack writes and its first unsupported barrier. These are explicitly `parent_candidate_is_causal=0`: a callee/wrapper stack transfer and typed producer contract are not inferred. The tiny caller's exact local write RVAs are also emitted when independently established. `producer_rva=0` remains if construction is not proven. All construction_kind/uses_current_position/uses_dt/direction_like/speed_like fields stay unknown without independent evidence.

Code identity scope is startup plus worker recheck, **not an atomic producer instruction snapshot**. Runtime observed chain and static relationship have separate fields; a different observed second return cannot borrow the cached static proof. No new producer hook, return interception, memory watch, game call or guessed ABI is installed. A bounded static refusal narrows the next edge to the named call target, stack write or barrier rather than forcing a relationship.

## Pending actual-game contract

C14 game writer/producer code has not been available to the cloud, so these analyses are capabilities and owned-test evidence, not an actual-game verdict. Actual c14 logs must establish the decoded XYZ/W path and the exact remaining producer edge. Final target Position versus another representation, producer provenance, full lifetime, unrelated sharing, later overwrite/conflicts and exclusive intended Hero translation are UNKNOWN. Existing incomplete watch coverage cannot prove the absence of later writers. Writable_source_proven stays0 even if the local copy or W constant is proved. Any future rewrite requires a separate task.
