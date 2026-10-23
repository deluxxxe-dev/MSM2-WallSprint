# c15 exact writer branch contract

## Accepted real boundary

C14 runtime at the exact admitted build fully decoded writer0x2CFC900..0x2CFC9B7 but stopped on0x2CFC91F JNE. Actual16 entries/four stores each still show RDX/stack XYZ mask7 with W0->1. These observations do not prove causality. See V01C14_RUNTIME_RESULT.md.

## Implementation / owned proof

New bounded local branch API in rdx_source_contract.cpp follows both successors and compares provenance across every relevant path, within183bytes/block16/edge24/branch4/path16/state31/visit4096 caps. No symbolic condition solving or interprocedural expansion is introduced. Shared scalar/vector/stack instruction semantics remain the c14 model; encountering another unsupported relevant instruction reports its exact RVA/mnemonic and stops.

Owned183-byte fixtures place a conditional at+0x1F and stores exactly+0x36/+0x3c/+0x42/+0x45. They are independently authored CPU examples, not the private game body. Positive reconvergence must retain exact RDX XYZ origin and separate W1. One-leg X/Y/Z mutations, selective store bypass versus unreachable stores,4 nested branches/16 paths,5th-branch overflow, self/backedges, indirect/outside/mid-instruction targets, unsupported/call/unknown-memory effects, wrong store RVA/target and W independent/computed/conflicting origins cover failure and relevant-path semantics. Test outputs/final counts are in CODEX_RESULT and V01C15_RELEASE.json.

The raw entry fixture explicitly drains before activation, captures actual entry, and only finalizes once after stop. A second never-activated session reports zero at stop. Existing concurrent/nested hardware store pairing, ABI/identity/hash/guard/caller/freshness refusals and foreign-patch cleanup remain regression requirements.

## Actual result required

The cloud cannot assert the actual function's next instruction after crossing JNE because its private body is not supplied. Startup in the exact game emits BRANCH_CONTRACT, then existing per-invocation XYZ/W records validate the path versus real captured stores. Diagnostic outcome A is exact XYZ path+separate W proof; outcome B is one precise narrower local blocker. Neither outcome permits movement. No producer hook/follow-up into0x351EB00 is part of c15.
