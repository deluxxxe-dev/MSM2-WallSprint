# MSM2 Engine Internals Reference — WallSprint-derived seed

Purpose: compact, reusable technical knowledge extracted from WallSprint research for future Marvel's Spider-Man 2 PC mod development, debugging, and cross-project analysis.

This is **not** a progress log and **not** a license to mutate every listed field. It is a current knowledge map. Detailed proof remains in the linked WallSprint analysis files.

Canonical source project at the time of this snapshot:
- repository: `deluxxxe-dev/MSM2-WallSprint`
- branch: `codex/v01c20-edi-value-provenance`
- organized baseline: `5192f05f32313919fdf4049bd734b971655d163f`

## Evidence vocabulary

Every reusable fact should carry one of these meanings.

- **VERIFIED-RUNTIME** — observed on the authenticated target build in actual game execution with the required runtime gates.
- **VERIFIED-STATIC** — established from authenticated bounded disassembly/control-flow/data-flow analysis for the target build.
- **OBSERVED-CORRELATION** — values matched in runtime captures, but causal ownership or safe writability is not yet proven.
- **HYPOTHESIS** — plausible interpretation still requiring evidence.
- **UNKNOWN** — deliberately unresolved; do not assign semantics.
- **REJECTED** — a prior interpretation or mutation path that current evidence does not support.

Do not silently promote `OBSERVED-CORRELATION` to causal ownership, or an address/register relationship to an engine semantic type.

## Exact target build

**VERIFIED-RUNTIME / project admission gate**

- executable: `Spider-Man2.exe`
- version: `2.810.0.0`
- SHA256: `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`

WallSprint intentionally fails closed outside the authenticated build. RVAs below are build-specific unless independently revalidated on another executable.

## WallRun lifecycle

**VERIFIED-RUNTIME**

- Init: `0x14233C0`
- Entry: `0x14235C0`
- Update: `0x1424530`
- Deinit: `0x1416660`

Reusable lesson: lifecycle ownership is a separate question from data ownership. State captured during WallRun must be invalidated/restored on verified Deinit or other explicit fail-closed teardown paths rather than assumed to remain valid globally.

## Orientation / transform source paths

### Common orientation writer path

**VERIFIED-RUNTIME**

- writer: `0x2CFC580`
- WallRun caller return: `0x2CFC8F3`
- source: original entry `RSP+0x28`

The source layout observed/used by WallSprint is:

```text
+0x00  Right    Vec4
+0x10  Up       Vec4
+0x20  Forward  Vec4
+0x30  Position Vec4
```

### Secondary/reset-copy orientation path

**VERIFIED-RUNTIME**

- writer: `0x2CFC3C0`
- caller return: `0x2CFC84B`

WallSprint treats this as an independently authenticated path with its own entry source basis/Position, not as a duplicate alias of the common writer source.

### Reusable orientation lesson

A transform can be rewritten successfully at one writer and still be overwritten later by another legitimate engine writer. Identifying one successful mutation site is therefore not equivalent to proving orientation ownership. WallSprint required dual-path observation, retention checks, lifecycle checks, and later-reset diagnostics before treating wall-relative orientation as stable.

Source: `analysis/DUAL_SOURCE_OWNERSHIP.md` and related orientation analysis.

## Position writer chain

### Final writer

**VERIFIED-RUNTIME + VERIFIED-STATIC**

Function span:

```text
0x2CFC900 .. 0x2CFC9B7
```

Exact stores:

```text
0x2CFC936 -> Transform +0x30
0x2CFC93C -> Transform +0x34
0x2CFC942 -> Transform +0x38
0x2CFC945 -> Transform +0x3C
```

Exact callsite:

```text
0x351EEAB -> 0x2CFC900
return: 0x351EEB0
```

A second observed return in the surrounding lineage is `0x351A020`.

### Entry source relationship

**OBSERVED-CORRELATION, not yet complete causal ownership**

At the true writer entry:

```text
RDX = entry RSP + 0x28
```

Sampled `[RDX]` XYZ matched the exact XYZ bits written by the executed Position stores across accepted captures. The current accepted contract still distinguishes this exact bit correlation from proof that this storage is the uniquely owned, safely writable upstream movement source.

Observed W relationship in the accepted c19 line:

```text
source W = 0
destination W = 1
```

Do not infer homogeneous-coordinate semantics from the numeric `1.0` alone.

Source: `docs/PROJECT_STATE.md`, `analysis/RDX_XYZ_SOURCE_CONTRACT.md`, `analysis/MOVEMENT_ENTRY_SOURCE_CANDIDATES.md`.

## Nested callee on the Position-writer path

### Exact call and callee

**VERIFIED-STATIC / accepted prior runtime authentication**

Writer call:

```text
0x2CFC927
return 0x2CFC92C
```

Pinned callee:

```text
0x2B23560 .. 0x2B235E9
```

Accepted callee hash:

```text
2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
```

### Known Transform write

**VERIFIED-RUNTIME + VERIFIED-STATIC**

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
```

Proven relationship:

```text
RCX provenance = writer-entry Hero Transform
destination = Hero Transform +0x10C
```

The `+0x10C..+0x10F` write is numerically disjoint from Position `+0x30..+0x3F` and from the protected Transform identity/Position range used by the current analysis.

**UNKNOWN:** semantic meaning of `Transform+0x10C`.

**UNKNOWN / current c20 blocker:** exact value provenance of `EDI` at `0x2B2358E`.

Do not assign an engine field name to `+0x10C` or an ABI type/meaning to EDI without new evidence.

Source: `analysis/V01C19_TRANSFORM_WRITE_CONTRACT.md`, `analysis/V01C20_EDI_VALUE_PROVENANCE.md`.

## Current c20 value-provenance model

The prepared c20 analysis follows only bounded local reaching definitions for EDI. It can describe exact local relationships such as:

- raw low-32-bit callee-entry register identity;
- register copies;
- constants;
- exact modeled memory-load origins;
- exact simple affine/add/sub relationships;
- bounded stack save/load propagation.

It intentionally refuses opaque, partial-register, conflicting, unsupported control-flow, capped, or otherwise unresolved value chains rather than inventing semantics.

Important distinction:

```text
local value proof != complete writer effect proof
complete writer effect proof != safe mutation ownership
```

Even if EDI becomes locally explainable, later reads/effects, caller ownership, source lifetime, competing writers, and downstream conflicts remain separate gates.

## Mutation / ownership status

As of the c20 start point:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position writes by MOD=0
source writes by MOD=0
input writes by MOD=0
movement writes by MOD=0
```

Producer/caller path `0x351EB00` remains deferred until the stricter writer/value contracts justify advancing to it.

**REJECTED as a current engineering shortcut:** directly writing final Position merely because the Position writer is known. The project is intentionally tracing upstream ownership first to avoid fighting engine physics, animation, collision, or other writers.

## Runtime research practices that generalized well

These are methodological rules learned during WallSprint and are candidates for reuse in SF and future MSM2 mods.

1. Authenticate the exact executable before trusting RVAs, hashes, layouts, or hooks.
2. Treat observed equality as correlation until the complete causal chain is proven.
3. Separate address provenance, value provenance, semantic type, lifetime, ownership, and writability; they are different proofs.
4. A successful hook/write does not prove exclusivity. Look for later writers and reset paths.
5. Capture exact invocation identity when pairing entry registers with downstream stores; do not combine unrelated samples because their values look similar.
6. When control flow branches, analyze every relevant bounded path or explicitly refuse the proof.
7. Unknown/unsupported instructions, aliases, reads, calls, or caps should fail closed and identify the next exact blocker.
8. Do not infer pointer types from register names or numerical values.
9. Do not infer engine semantics from familiar-looking constants (`0`, `1.0`, normalized vectors) alone.
10. Keep runtime evidence, static conclusions, and hypotheses explicitly distinct in documentation and logs.
11. Preserve teardown/restore verification; successful active behavior is insufficient without verified cleanup.
12. Avoid publishing raw user logs or complete proprietary game-function bodies when compact derived evidence is sufficient.

## Workspace / tooling knowledge currently verified

**VERIFIED-RUNTIME on the user's real PC**

Shared MSM2 development environment:

```text
<game>\Mods Library\Mods-Developing\
├─ MSM2-SF\
└─ MSM2-WS\
```

Shared Overstrike:

```text
<game>\Overstrike\Overstrike.exe
WorkingDirectory=<game>
```

Accepted real-PC run showed ordinary mods + SF + WS together in the shared environment and produced versioned WS/SF logs from the same modded-game run.

WS logs:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

This is infrastructure knowledge, not evidence that WS tooling may mutate SF or ordinary mods. Project-specific updaters remain isolated owners of their own roots.

Source: `analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md`, `docs/LOCAL_WORKSPACE.md`.

## Maintenance rule

This reference is a **living derived index**, not the primary proof record.

When new runtime/static evidence:
- confirms a reusable fact, add or strengthen its evidence label and cite the project analysis source;
- narrows a claim, edit the existing entry instead of leaving two contradictory 'truths';
- disproves a claim, mark the old interpretation **REJECTED** and record the replacement/correction;
- changes a build-specific RVA/layout, keep the old build scoped to its executable identity rather than silently rewriting it as universal;
- remains speculative, keep it under **HYPOTHESIS** or **UNKNOWN**.

Do not copy raw user logs, full proprietary native bodies, personal filesystem data, credentials, or unnecessary binary fragments into the shared knowledge base.

## Planned centralization

This file is the WallSprint-derived seed for a future organization-level MSM2 technical knowledge repository under `deluxxxe-dev`. Once that repository exists, this content should be copied into the centralized knowledge base and WallSprint should keep only project-specific proof/history plus a pointer to the shared reference.
