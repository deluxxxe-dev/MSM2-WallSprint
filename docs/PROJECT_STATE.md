# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Repository / branch policy
- Canonical remote: `https://github.com/deluxxxe-dev/MSM2-WallSprint.git`
- Current implementation line: `codex/v01c20-edi-value-provenance`
- Owner-confirmed organized baseline: `5192f05f32313919fdf4049bd734b971655d163f`
- Continue from later commits on this branch; do not reset to the baseline.
- `main` is heavily diverged and must **not** be routinely merged into this branch.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve exact-build fail-closed behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw user logs/full private game bodies in public artifacts.

## Accepted c20 runtime result

Evidence: `analysis/V01C20_RUNTIME_RESULT.md`.

Actual A/E-style run on v0.1c20 / package0.1.38 retained verified lifecycle/orientation behavior, no hard kills, no MOD Position writes, and 8 true entry snapshots / 8 complete invocation maps in each session.

## Verified movement chain

```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
exact callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```

Runtime source relation remains `RDX=entry RSP+0x28`; sampled `[RDX] XYZ` equals exact executed Position XYZ bits. This remains correlation until the writer/effect/ownership chain completes.

## c20 result — EDI local value provenance resolved

C19 established:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX provenance = writer-entry Hero Transform
destination = Hero Transform +0x10C
```

C20 established the bounded local reaching definition for EDI under the authenticated actual-game callee:

```text
producer RVA = 0x2B2356F
producer kind = memory_load
origin kind = memory_load
root = analyzer-internal root 16
offset = 0x0C323788
reaching definitions = 1
all local reaching states examined = 1
same local value origin = 1
value provenance exact = 1
```

This does not assign semantic meaning to the memory origin, EDI, or `Transform+0x10C`.

## Accepted c21 runtime result — exact same-field pre-write read

Owner-supplied runtime log:

```text
MSM2-WS-v0.1c21-20261025-013952.log
```

C21 classified the previous `0x2B23578` blocker precisely:

```text
site=0x2B23578
instruction=mov
read width=4
address=Hero Transform+0x10C
base=RCX
carrier=EAX
same exact address for analyzed states=1
```

Therefore the earlier read is an exact read of the same field later written at `0x2B2358E`.

The bounded local flow also establishes:

```text
first possible use=0x2B23581
control_may_depend=1
first control dependence=0x2B23583
memory_escape=0
```

This is possible local control dependence, not completed Position causality.

## Current research blocker — 0x2B235BE `test`

C21 read-flow analysis remains incomplete at:

```text
first_unresolved_rva=0x2B235BE
first_unresolved_instruction=test
reason=read_flow_partial_or_unmodeled_register
```

Current unresolved outputs:

```text
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
effect_certificate=0
```

The next task is the smallest bounded extension needed to model/classify the exact `test` operands/flags and continue only until the next exact local proof boundary.

Do not infer that `Transform+0x10C` controls XYZ/W merely because the read may affect local control flow. Do not infer that the later write is irrelevant merely because it is disjoint from Position offsets.

## Current safety gate

```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```

Producer `0x351EB00` remains deferred. No movement, adhesion, camera, animation or physics advancement is authorized.

## Local workspace — accepted on real PC

Canonical workspace:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v<version>.script
├─ Update-WS.cmd
├─ logs\
├─ _source\
└─ _archive\
```

Shared ordinary mods + SF + WS, shared Overstrike, modded launch and versioned WS/SF runtime logs are accepted. See `analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md` and `docs/LOCAL_WORKSPACE.md`.

## Current stages

1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact writer callsite ✅
7. true writer-entry capture ✅
8. RDX stack-local XYZ correlation ✅
9. c14 JNE blocker identified ✅
10. c15 writer branch crossing ✅
11. c16 exact call target/boundary/hash ✅
12. c17 call argument provenance ✅
13. c18 actual callee CFG / first real writes ✅
14. c19 exact Transform+0x10C destination ✅
15. local/shared workspace acceptance ✅
16. c20 EDI local value provenance ✅ actual game
17. c21 exact `Transform+0x10C` pre-write read classification ✅ actual game
18. `0x2B235BE test` / selected-read flow completion ▶ CURRENT
19. resume full writer XYZ/W causal proof ⏸ after local effect classification
20. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
21. safe writable upstream movement source ⬜ NOT AUTHORIZED
22. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
23. adhesion/contact refinement ⏸ deferred
24. animation/camera/later traversal ⬜

## Current action

Continue from the accepted c21 evidence. Model/classify only the missing `0x2B235BE test` semantics and the selected-read value/flag flow needed to decide downstream relevance or identify the next exact blocker. Preserve all current safety gates, do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.

## Prepared c22 implementation — actual classification pending

v0.1c22/package0.1.40 adds the bounded pinned TEST operand/flags diagnostic
(analysis/V01C22_READ_TEST_FLOW.md).32/64bit register TEST and read-only low8bit
aliases at the exact site retain conservative parent tags; no general partial
register write/address model changes. CF/OF constant conditions are separate
from possible ZF/SF/PF dependence; both CFG legs stay enumerated. Existing
c19/c20 read/effect refusal and all ownership/mutation gates stay closed.
Exact actual TEST operands/local completion/next blocker remain PENDING a new
versioned c22 A/E log. Accepted c21 evidence is now in V01C21_RUNTIME_RESULT.md.
Use Update-WS, Install mods -> Run modded game, A~4s -> fully end -> move -> E~4s.
Do not advance producer0x351EB00 or movement/adhesion/camera/animation/physics.
