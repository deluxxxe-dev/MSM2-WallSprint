# c16 exact writer-local call effect contract

## VERIFIED input / UNKNOWN runtime result

Compact c15 actual evidence is accepted: A3860ms/E4297ms, verified Deinit/retention, hard0/stale0/Position writes0,8 complete entry maps each and finalized capture summaries. The bounded writer proof crossed JNE0x2CFC91F and stopped at CALL0x2CFC927. RDX=entryRSP+0x28/XYZ mask7/sourceW0/destinationW1 are correlation, not write authorization. See V01C15_RUNTIME_RESULT.md.

Cloud has neither the current private183-byte writer nor its callee. **Actual call target, boundary, hash, effect and next blocker are UNKNOWN until c16 A/E.** Independently authored nonexecuted fixtures establish the mechanism only. No engine function name/type/physics semantics is inferred.

## Exact edge and code identity

The complete183-byte decoder anchors the single call at writer+0x27, return+0x2c. Direct CALL must land in the admitted image and outside the writer. Indirect form is labeled but never dereferenced. Missing/misaligned/return/target/recursive/out-of-image cases refuse. Existing exact exe/SHA/full writer baseline/current owned writer/caller gates remain mandatory.

For this single target only, runtime RtlLookupFunctionEntry must report the same image and an entry equal to the call target, end within the image and a span <=1024. Guarded full-byte capture/SHA256 and immediate second-byte comparison establish current startup identity. No historical independent target digest is claimed. Overlap with writer or deferred0x351EB00 is refused without reading the target. A thunk/wrapper is labeled at an exact entry JMP, never traversed. Larger/missing/interior boundaries stop with target/boundary/cap reason.

MOVEMENT_WRITER_CALL_CONTRACT prints target/boundary/hash/call-kind/relevance/effect/clobber masks and exact callee barrier/mnemonic. No code bytes are logged. A compatible summary additionally rechecks the full target bytes/hash and runtime boundary in the worker for every sampled invocation before XYZ/W proof. Code identity samples are not atomic execution attestation; existing admitted-store/close and incomplete-watch limitations remain.

## Conservative bounded leaf effect language

No recursion, nested/indirect-call expansion, branch search, fixpoint or arbitrary pointer chasing. The fully decoded target has a single linear entry prefix through a plain RET. Only explicit known register/flags moves, LEA, selected arithmetic/CMP/TEST/NOP are accepted. Every unproven memory write, implicit/unsupported effect, callee branch/call or unmodeled register effect stops at its exact RVA. Only decoded, source-disjoint private stack writes are permitted; ABI alone never establishes memory safety. Decoded reads are not executed by the mod. Unknown pointer writes remain source_span_effect_unknown, never promoted as safe or asserted to definitely overlap source. No source_span_may_write positive is fabricated.

A successful source-no-write leaf proves that memory writes are confined to its explicitly allocated private stack below callee-entry RSP, at most4096bytes. PUSH/POP and register/stack moves use the independent provenance model; memory writes outside the active allocated private span, return/home-slot writes, unbalanced/out-of-range SP and unsupported implicit effects refuse. Known saves/restores of GPR and XMM lanes are decoded before deciding which nonvolatiles survive; no ABI promise supplies restoration. Explicit nonvolatile writes not proved restored invalidate those carriers. All Win64 volatile RAX/RCX/RDX/R8..11 and XMM0..5 are invalidated regardless of decoded behavior. Caller saved slots must be disjoint from the complete possible callee private-stack/return/shadow footprint.

The static certificate is **conditional on per-invocation entryRDX=entryRSP+0x28/current TEB bounds/equal guarded bytes and nonstack Transform**. The lowest possible callee write/return offset is carried into the writer certificate and must fit above the invocation's captured TEB StackLimit. That makes the entire private write span stack-local and disjoint from source and the nonstack Transform; crossing the captured stack bound refuses. These checks apply to call-dependent W as well as XYZ. An offline certificate alone does not assert pointer nonaliasing. Subsequent writer reloads must use independently preserved aliases, never volatile-carrier equality.

All c15 relevant branch paths remain enumerated; only the exact authenticated call summary can resume at+0x2c. Each named Position store must still have exact entry-RDX XYZ origin on every reaching state. W constant/register/memory/computed/unknown remains independent. A later unsupported writer instruction stops at its exact site. Failed callee effects retain the local call barrier plus narrower target/callee effect metadata.

## Diagnostic outcome and scope

Outcome A: compatible authenticated no-write call summary and all relevant XYZ paths, or a strictly later exact writer blocker. Outcome B: exact target/boundary/effect/callee instruction blocker. Static classification supplies that narrower evidence without adding a call observation hook. Natural-call before/after equality would not establish absent transient writes or provenance, so no extra detour/return interception is justified for this bounded B outcome. ABI/hardware/cleanup regression tests cover the unchanged raw entry mechanism; there is no new observation ABI to test.

No producer0x351EB00 traversal/hook/read expansion. Historical caller_owned/ownership_exclusive/source_lifetime_exclusive/downstream_conflicts_proven_absent remain0. Writable_source_proven stays unconditionally0. No Position/RDX/source/input/movement/adhesion/camera/animation writes, data-page protection change or PAGE_GUARD. C7-c15 native policies, thunk, finalization and stop/restoration remain preserved.

## Owned verification

New test_writer_call.sh covers no-write continuation, volatile GPR/XMM invalidation versus source reload, explicit nonvolatile clobbers, source XYZ/unknown/stack/implicit writes, indirect/target/return/site/boundary/cap refusal, local branch reconvergence, callee thunk/branch/nested-call refusal, later exact writer barrier, private-stack preservation/overlap/limits, return/shadow saved-slot refusal and W constant/register/memory/computed/unknown. All CPU bytes are independently authored and nonexecuted. Full prior native suites and an isolated source-ZIP build are recorded in CODEX_RESULT and V01C16_RELEASE.json.
