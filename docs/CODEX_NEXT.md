# Next task — WallSprint v0.1c14 RDX Stack-Local Source Contract

## Branch
```text
codex/v01c14-rdx-source-contract
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `analysis/V01C13_RUNTIME_RESULT.md`, and c13 `docs/CODEX_RESULT.md` first.

## Accepted c13 actual-game evidence
Combined A -> fast travel -> E log is accepted. Both sessions preserved verified Deinit, orientation retention, hard kill0 and Position writes0.

C13 captured 16 true writer-entry snapshots total and linked every sampled invocation to all four `0x2CFC900` Position stores.

Repeated source relation on all 16 invocations:
```text
RDX = entry RSP + 0x28
candidate20 = guarded [RDX] read
candidate23 = stack snapshot at RSP+0x28
XYZ exactly equal the executed Position XYZ store bits
source W=0
Position W=1
exact_store_match_mask=0x7
observed_store_mask=0xF
```

Observed runtime chain remains:
```text
0x351EEAB -> 0x2CFC900
return 0x351EEB0
caller2 observed 0x351A020
runtime stack verified
caller2 static relation unresolved
```

`writable_source_proven=0` remains authoritative.

## Objective
Authenticate the exact RDX stack-local XYZ source contract and its producer. Separately prove W behavior. Keep movement/source/input read-only.

## Required work
1. Repair c13 reporting: final summary must reflect real true-entry captures and add XYZ-only stable-candidate aggregation instead of requiring W equality.
2. Prove or reject decoded writer causality `[RDX+0/+4/+8] -> Transform +0x30/+0x34/+0x38`, including the previously unsupported writer edge around +0xA.
3. Prove the source of Transform `+0x3C`; classify conservatively as constant_one / independent_register / independent_memory / computed / unknown.
4. Prove exact caller storage identity for `RDX == entry RSP+0x28`, stack bounds, object size and lifetime.
5. Trace the bounded producer that fills that XYZ stack object before `0x351EEAB`, using `0x351A020` only as an observed runtime anchor. Resolve direct/wrapper/tail/indirect/unresolved relation without guessing.
6. Establish whether the payload is final target Position XYZ or another representation only from independent evidence. Do not infer velocity/displacement/direction/root motion from resemblance.
7. Before any future write permission, prove source lifetime, Hero/session/thread ownership, downstream conflict behavior and exclusive intended translation effect.

Emit focused evidence such as:
```text
MOVEMENT_ENTRY_CAPTURE_SUMMARY
MOVEMENT_RDX_XYZ_PROOF
MOVEMENT_POSITION_W_PROOF
MOVEMENT_RDX_STORAGE_PROOF
MOVEMENT_RDX_PRODUCER_PROOF
```

## Safety
No Actor/Transform Position write, RDX source mutation, Path A Position mutation, input rewrite, custom velocity/gravity, active adhesion, camera/animation/traversal work, PAGE_GUARD/data-page protection mutation, guessed game ABI/call, raw user logs or full private game bodies.

`Position_writes_by_mod=0` remains mandatory.

## Human test
If runtime evidence is still required after implementation:
```text
A ~4 s -> fully end WallRun -> move/fast travel -> E ~4 s -> send one complete log
```
Video only for a visible anomaly or a specifically requested reproduction.

## Gate
Only set `writable_source_proven=1` if XYZ/W causality, producer provenance, lifetime, downstream conflicts and exclusive intended Hero translation effect are all independently proven. Even then, create a separate minimal movement-write task; do not silently implement movement in c14.
