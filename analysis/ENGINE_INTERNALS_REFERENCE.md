# MSM2 Engine Internals Reference — WallSprint-derived seed

Purpose: compact, reusable technical knowledge extracted from WallSprint research for future Marvel's Spider-Man 2 PC mod development, debugging, and cross-project analysis.

This is **not** a progress log and **not** a license to mutate every listed field. Detailed proof remains in project-local analysis files. Shared reusable facts are also synchronized to `deluxxxe-dev/MSM2-Wisdoms`.

Canonical source project:
- repository: `deluxxxe-dev/MSM2-WallSprint`
- current implementation line: `codex/v01c20-edi-value-provenance`
- organized baseline: `5192f05f32313919fdf4049bd734b971655d163f` (dated baseline, not a pin)

## Evidence vocabulary

- **VERIFIED-RUNTIME** — observed on the authenticated target build in actual game execution with required gates.
- **VERIFIED-STATIC** — established from authenticated bounded disassembly/control/data-flow analysis.
- **OBSERVED-CORRELATION** — values matched, but causal ownership/writability is not fully proven.
- **HYPOTHESIS** — plausible interpretation requiring evidence.
- **UNKNOWN** — deliberately unresolved; do not assign semantics.
- **REJECTED** — a prior interpretation or shortcut current evidence does not support.

Never silently promote correlation to causality, ownership, lifetime, semantic type, or writability.

## Exact target build

**VERIFIED-RUNTIME / admission gate**

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

All RVAs below are build-specific unless revalidated on another executable.

## WallRun lifecycle

**VERIFIED-RUNTIME**

```text
Init   0x14233C0
Entry  0x14235C0
Update 0x1424530
Deinit 0x1416660
```

Reusable lesson: lifecycle ownership and data ownership are separate. Captured state must be invalidated/restored on verified teardown rather than assumed globally valid.

## Orientation / Transform source paths

### Common writer

**VERIFIED-RUNTIME**

```text
writer: 0x2CFC580
WallRun caller return: 0x2CFC8F3
source: original entry RSP+0x28

+0x00 Right    Vec4
+0x10 Up       Vec4
+0x20 Forward  Vec4
+0x30 Position Vec4
```

### Secondary/reset-copy writer

**VERIFIED-RUNTIME**

```text
writer: 0x2CFC3C0
caller return: 0x2CFC84B
```

This is an independently authenticated path with its own entry source basis/Position.

Reusable lesson: one successful transform hook is not ownership proof; another legitimate writer may overwrite it. WallSprint required dual-path observation, retention, lifecycle and reset diagnostics.

Source: `analysis/DUAL_SOURCE_OWNERSHIP.md` and related orientation analysis.

## Position writer chain

### Final writer

**VERIFIED-RUNTIME + VERIFIED-STATIC**

```text
function 0x2CFC900..0x2CFC9B7
0x2CFC936 -> Transform+0x30
0x2CFC93C -> Transform+0x34
0x2CFC942 -> Transform+0x38
0x2CFC945 -> Transform+0x3C
callsite 0x351EEAB -> 0x2CFC900
return   0x351EEB0
observed surrounding second return 0x351A020
```

### Writer-entry source

**OBSERVED-CORRELATION**

```text
RDX = entry RSP+0x28
sampled [RDX] XYZ == exact executed Position-store XYZ bits
```

Accepted W observation remains source W=0, destination W=1. Do not infer homogeneous-coordinate semantics from `1.0` alone.

This is not yet proof that the storage is uniquely owned or safely writable.

Sources: `analysis/RDX_XYZ_SOURCE_CONTRACT.md`, `analysis/MOVEMENT_ENTRY_SOURCE_CANDIDATES.md`, `docs/PROJECT_STATE.md`.

## Nested callee on Position-writer path

**VERIFIED-STATIC with accepted runtime authentication**

```text
writer call 0x2CFC927
return      0x2CFC92C
callee      0x2B23560..0x2B235E9
callee hash 2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
```

### Native Transform write

**VERIFIED-RUNTIME + VERIFIED-STATIC**

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX provenance = writer-entry Hero Transform
destination = Hero Transform+0x10C
```

The four-byte destination is numerically disjoint from Position `+0x30..+0x3F` and the protected Transform identity/Position region.

`Transform+0x10C` semantic field meaning remains **UNKNOWN**.

### EDI local value origin — c20 update

**VERIFIED-STATIC under authenticated actual-game code identity; accepted from actual c20 run**

The previous statement “EDI value provenance unknown” is narrowed. The local reaching definition is now exact:

```text
EDI at 0x2B2358E
  producer RVA: 0x2B2356F
  producer kind: memory_load
  origin kind: memory_load
  origin root: 16
  origin offset: 0x0C323788
  reaching definitions: 1
  all local reaching states examined: yes
  same local value origin: yes
```

In the analyzer implementation, root `16` is `globalRoot`: an exact RIP-relative address inside the authenticated executable image. It is **not** a GPR number and is not a recovered engine type/field name.

The semantic meaning of the image-global value and EDI remains **UNKNOWN**.

Sources: `analysis/V01C20_RUNTIME_RESULT.md`, `analysis/V01C20_EDI_VALUE_PROVENANCE.md`, `source/src/NativeHooks/rdx_source_contract.cpp`.

### Current exact effect blocker

**VERIFIED-STATIC diagnostic boundary from actual c20 run**

```text
0x2B23578
earlier_read_provenance_dependency
```

The read occurs before the native `0x2B2358E` write. Current conservative analysis cannot yet decide whether that pre-write read/value carrier makes the `+0x10C` write relevant to downstream writer XYZ/W state.

Important reusable lesson: **instruction ordering and causal relevance are separate proofs.** A read-before-write should not be automatically declared irrelevant if state can persist across invocations or the read value survives into later state; but a conservative dependency stop is also not itself proof of causality.

The next bounded research must classify read address/root/offset/width, path ordering and the minimal relevant value flow.

## Mutation / ownership status

Current gate remains:

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

Producer/caller path `0x351EB00` remains deferred.

**REJECTED current shortcut:** directly writing final Position merely because its final writer is known.

## Runtime research practices generalized from WS

1. Authenticate exact executable identity before trusting RVAs/layouts/hooks.
2. Treat equality as correlation until the causal chain is proven.
3. Separate address provenance, value provenance, semantic type, lifetime, ownership and writability.
4. A successful hook/write does not prove exclusivity; find later writers/reset paths.
5. Pair entry registers and downstream stores by exact invocation identity.
6. Analyze every relevant bounded control-flow path or refuse precisely.
7. Unknown instructions/aliases/reads/calls/caps should fail closed at an exact blocker.
8. Do not infer pointer types from register names/numeric values.
9. Do not infer semantics from familiar constants such as `0`, `1.0`, normalized vectors.
10. Distinguish runtime evidence, static conclusions and hypotheses.
11. Verify cleanup/teardown, not only successful active behavior.
12. A local value proof is not a whole-effect proof; a whole-effect proof is not ownership/writability.
13. Read-before-write ordering alone does not settle cross-invocation or surviving-carrier causality.
14. Keep raw private logs/full proprietary game bodies out of reusable documentation when compact derived evidence is sufficient.

## Workspace / tooling knowledge

**VERIFIED-RUNTIME on real PC**

```text
<game>\Mods Library\Mods-Developing\
├─ MSM2-SF\
└─ MSM2-WS\

<game>\Overstrike\Overstrike.exe
WorkingDirectory=<game>
```

Ordinary mods + SF + WS were accepted together in one shared environment. Project-specific updaters still must not mutate sibling projects.

WS runtime logs:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

## Maintenance rule

This is a living derived index, not the primary proof record. When new evidence confirms/narrows/disproves a reusable fact, update both the project-local proof/index and `deluxxxe-dev/MSM2-Wisdoms` when practical. Scope old/new RVAs to authenticated builds and mark speculation explicitly.

Do not store raw user logs, complete proprietary native bodies, credentials, personal secrets or unnecessary binary fragments.