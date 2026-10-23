# c12 writer 0x2CFC900 — bounded read-only lineage

## Evidence status

VERIFIED HUMAN c11: the accepted build wrote Hero Transform +0x30/+0x34/+0x38/+0x3c at RVAs 0x2CFC936/93c/942/945. A/cardinal and E/diagonal translation correlated with return 0x351EEB0 and second return 0x351A020. See V01C11_RUNTIME_RESULT.md. This supplies measured addresses, not a full writer body or ABI.

C12 GAME PROOF PENDING: cloud has no private original 0x2CFC900 body or running game. No actual function end/size/hash/source register is invented. Startup MOVEMENT_WRITER_PROOF obtains RtlLookupFunctionEntry boundaries inside the exact admitted image; Begin must equal 0x2cfc900. Complete Capstone decoding requires a contiguous full span, no skipdata, <=1024 writer bytes. Hash is current runtime identity under the existing exact-exe gate, never exclusive semantics. No body/disassembly is logged or shipped.

## Independent analyzer

movement_lineage.cpp builds a bounded expression DAG from entry GPRs, four lanes of each XMM, constant bits, pointer aliases, width-specific memory reads and supported scalar/packed add/sub/multiply. It follows 64-bit pointer MOV/LEA/stack adjustment, supported PUSH/POP/saved stack slots and exact scalar or Vec4 Position stores to entry RCX+0x30..0x3f. MOVSS memory/MOVD upper-lane zeroing is modeled; unsupported vector lane rules are refused. Partial-register ambiguities, indexed/segmented addresses, calls, branches, duplicate/partial stores, unknown effects and expression exhaustion cannot obtain a complete certificate.

Every instruction is decoded. A complete straight-line certificate requires one value for each component and RET without unsupported effects. A separately labeled supported prefix may map all four stores before a later unsupported effect/CFG edge; it never certifies the rest of the function. First unsupported offset plus up to16 exact unsupported offsets identify the missing slice. MOVEMENT_WRITER_SOURCE_COMPONENT records per-component store RVA/destination/expression and whole-function status. No source equality confers write permission.

## Runtime observations and limits

Existing c11 Position hardware exceptions already capture GPR/XMM and bounded unwound returns. C12 uses those data, with no entry detour or new executable breakpoint. For a statically verified writer/direct call, exact 0x351EEB0 and observed second return0x351A020, it reconstructs only expressions recoverable from surviving post-store aliases. It verifies the cached entire writer and exact E8 call bytes again at the exception; when caller2's direct relation is proven its E8 is rechecked too. Sources are guarded trap-time reads, not function-entry snapshots. Unknown/changed aliases deny reconstruction. Source identity stable remains0; W is sampled as bits and its engine meaning remains unknown.

At most16 eligible reconstruction attempts/window,64 expression visits/attempt, depth16,256 nodes,8 store slots (valid maps exactly4 components). The result is16 payload bytes; at most64 four-byte reads may be needed. New reads refuse uncommitted/guard/noaccess/cross-region memory without changing protection. Existing event cap256, writer identity16, printed4/identity,600ms window, eight windows/process, snapshot128 threads/no rescans remain. Only the existing diagnostic helper checks freshness/lifecycle and owns cleanup; no secondary ownership migration. No allocation, decoder/hash/log/SDK/game call is added to the exception path. LastError and existing debug restoration remain.

Exact next game evidence: startup MOVEMENT_WRITER_PROOF, MOVEMENT_WRITER_SOURCE_COMPONENT, MOVEMENT_LINEAGE_UNSUPPORTED and per-session MOVEMENT_WRITER_SOURCE_MAP/MOVEMENT_LINEAGE_SUMMARY. If unsupported before the stores, the exact instruction RVA is the next analysis edge. If prefix maps but reconstruction fails, require surviving-source/call-chain evidence rather than assuming an entry capture.
