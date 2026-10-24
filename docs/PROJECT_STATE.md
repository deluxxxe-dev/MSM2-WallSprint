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

Actual A/E-style run on v0.1c20 / package0.1.38:

```text
session1: 4219 ms orientation summary / 4.214959 s verified WallRun
  verified Deinit, source active until Deinit, retention PASS
  final actor Up dot surface=1
  stale suspend/resume=1/1, max stale 103 ms, recovered
  hard kills=0
  Position writes by MOD=0, watch cleanup restored

session2: 4390 ms orientation summary / 4.392723 s verified WallRun
  verified Deinit, source active until Deinit, retention PASS
  final actor Up dot surface=1.00000012
  stale0, hard kills=0
  Position writes by MOD=0, watch cleanup restored
```

Both sessions retained 8 true entry snapshots / 8 complete invocation maps.

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

C20 now establishes the bounded local reaching definition for EDI under the authenticated actual-game callee:

```text
producer RVA = 0x2B2356F
producer kind = memory_load
origin kind = memory_load
root = 16 = analyzer globalRoot / exact RIP-relative executable-image address
offset = 0x0C323788
reaching definitions = 1
all local reaching states examined = 1
same local value origin = 1
value provenance exact = 1
```

This supersedes the old statement that EDI local provenance is unknown. It does **not** assign semantic meaning to the image-global value, EDI, or `Transform+0x10C`.

## Current research blocker — exact earlier read at 0x2B23578

The c20 local value proof did not finish the full callee/writer-effect proof:

```text
all_writer_call_states_examined=0
relevant_transform_effects_resolved=0
transform_position_protected_range_unmodified=0
source_span_no_write_proven=0
XYZ/W causal proof incomplete
```

The exact next dependency site is:

```text
0x2B23578
```

Current conservative classification:

```text
earlier_read_provenance_dependency
reason=exact_transform_write_may_change_a_decoded_read_carrier
```

The read occurs before the native write at `0x2B2358E`. The next task is therefore to classify its exact address/value provenance and ordering/value-flow relationship, not to skip directly to producer ownership.

Do not assume either that the earlier read is irrelevant or that it is causal. Same-invocation flow, any surviving returned carrier, and possible cross-invocation persistence must be distinguished or explicitly left unresolved.

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
17. `0x2B23578` earlier-read dependency classification ▶ CURRENT
18. resume full writer XYZ/W causal proof ⏸ after read/effect classification
19. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
20. safe writable upstream movement source ⬜ NOT AUTHORIZED
21. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
22. adhesion/contact refinement ⏸ deferred
23. animation/camera/later traversal ⬜

## c21 diagnostic preparation

v0.1c21/package0.1.39 adds compact READ_DEPENDENCY/READ_FLOW metadata for the
pinned read0x2B23578 under unchanged current-code authentication. Structural
local ordering and conservative possible value flow are separate; surviving
GPR/XMM/flags/caller stack and later reload/control/unsupported points are logged.
The sidecar does not grant an effect or writer certificate and does not bypass
the old refusal. Cross-invocation feedback/downstream XYZ/W remain unknown.
Actual c21 classification is pending the next game log; see
analysis/V01C21_READ_DEPENDENCY.md and docs/CODEX_RESULT.md for preparation/results.

## Current action

Run the completed diagnostic build through installed Update-WS, then shared
Overstrike Install mods -> Run modded game. A/cardinal ~4s -> fully end WallRun
-> move/fast-travel -> E/diagonal ~4s; return the newest complete versioned WS log.
Use READ_DEPENDENCY/READ_FLOW with existing VALUE/WRITE/ALIAS/WRITER and safety/
finalization summaries to adjudicate only the current exact read blocker.
Do not merge main, traverse producer0x351EB00 or enable gameplay writes.
