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

## c19/c20/c21 accepted results

C19 established:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX provenance = writer-entry Hero Transform
destination = Hero Transform +0x10C
```

C20 established the bounded local reaching definition for EDI:

```text
producer RVA = 0x2B2356F
producer kind = memory_load
origin kind = memory_load
root = analyzer-internal globalRoot 16
offset = 0x0C323788
reaching definitions = 1
all local reaching states examined = 1
same local value origin = 1
value provenance exact = 1
```

C21 classified the earlier exact same-field read:

```text
0x2B23578: mov EAX,[RCX+0x10C]
address = Hero Transform+0x10C
first possible use = 0x2B23581
first control dependence = 0x2B23583
memory_escape = 0
```

Semantic meaning of the memory origin, EDI, and `Transform+0x10C` remains unknown.

## Accepted c22 runtime result — TEST/JE resolved

Evidence: `analysis/V01C22_RUNTIME_RESULT.md`.

```text
0x2B235BE: test r8b,r8b
parent R8 selected/same-field tags = 0x0
flags_may_depend_tags = 0x0
0x2B235C1: je
consumer_may_depend_tags = 0x0
first_possible_dependent_branch_rva = 0x0
returned_flags_tags = 0x0
```

Thus that exact TEST/JE pair is not a carrier of the selected same-field read for the analyzed state.

## Accepted c23 runtime result — direct CALL target identified

Evidence: `analysis/V01C23_RUNTIME_RESULT.md`.

```text
0x2B235D4: direct call
target_rva=0x2CF7FE0
exact_target=1
```

Selected `Transform+0x10C` dependence reaches the call boundary in raw GPR carrier index 0 (`gpr_tags=0x1`, mem32 Hero Transform+0x10C). Target boundary/hash was unavailable, so no effect analysis was admitted.

## Accepted c24 runtime result — PE runtime-table cap blocker

Evidence: `analysis/V01C24_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c24-20261025-033837.log`.

Target identity now fails at a more precise metadata boundary:

```text
target_rva=0x2CF7FE0
admitted=0
method=none
runtime_lookup_found=0
runtime_lookup_same_image=1
runtime_lookup_readable=1
PE_runtime_table_valid=0
PE_runtime_entries=527161
scan_bytes=0
admitted_bytes=0
snapshot_stable=0
reason=target_runtime_table_malformed_or_cap
```

C24's PE runtime-function entry cap is 262144. Therefore the current result does **not** prove a malformed exception directory and does **not** prove a missing-unwind/no-entry target. It proves only that structural table validation did not complete under the configured cap.

The selected-read carrier remains present at the CALL boundary; no target code span/hash/effect was admitted:

```text
boundary_verified=0
snapshot_authenticated=0
snapshot_sha256=unavailable
modeled_states=0
effect_certificate=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
writable_source_proven=0
```

Runtime safety remained closed: MOD Position writes 0. Session 1 hit the bounded 5-second diagnostic window once before verified Deinit; session 2 exited normally via verified native Deinit. This does not authorize gameplay writes.

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
18. c22 `0x2B235BE TEST r8b,r8b` / `0x2B235C1 JE` classification ✅ actual game
19. c23 `0x2B235D4` direct target `0x2CF7FE0` + carrier classification ✅ actual game
20. c24 target identity failure narrowed to PE runtime-table validation cap ✅ actual game
21. distinguish valid-large runtime table from malformed metadata; retry exact target identity ▶ CURRENT
22. resume full writer XYZ/W causal proof ⏸ after local effect classification
23. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
24. safe writable upstream movement source ⬜ NOT AUTHORIZED
25. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
26. adhesion/contact refinement ⏸ deferred
27. animation/camera/later traversal ⬜

## Current action

Do not reinterpret the c24 refusal as proof of a no-unwind leaf. First separate `entry_cap_exceeded` from genuinely malformed PE exception metadata. Validate the exact exception-directory size/range/count under a justified bounded metadata budget; if structurally valid, complete the ordered/nonoverlap/runtime-RVA checks without weakening exact-build fail-closed behavior. Only after a valid table plus OS lookup establish no containing runtime-function entry may the existing strict c24 no-unwind leaf rule run.

Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.

Deferred user-facing features are tracked in `docs/FEATURE_ROADMAP.md` and remain intentionally out of scope until the movement ownership foundation is safe.

## Prepared c25 implementation — complete table result pending

v0.1c25/package0.1.43 separates directory structure from explicit byte/entry
limits, admits up to600000 entries/8MiB in64-entry chunks, and reports read/
validated coverage and actual lookup execution.An aligned527161-entry table would require6325932
bytes and fit; actual directory bytes/divisibility and structure remain unproven. C24 cap refusal
preceded lookup: found0/same_image1/readable1 were defaults, not OS evidence.
Only fully validated metadata permits target lookup and the unchanged strict
c24 identity/leaf path. All old effect/ownership/mutation gates remain closed.
Actual c25 table/lookup/next boundary needs the new compact RUNTIME_TABLE record;
shortest A/E capture is about3s each, below the unchanged5second test limit.
See analysis/V01C25_RUNTIME_TABLE_BUDGET.md / V01C25_GATE.md.
