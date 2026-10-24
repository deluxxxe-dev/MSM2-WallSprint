# Next task — distinguish valid-large PE runtime table from malformed metadata after accepted c24

## Status

The v0.1c24 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c24-20261025-033837.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

## Accepted c24 facts

The selected-read walk still reaches:

```text
0x2B235D4 -> direct target 0x2CF7FE0
raw GPR carrier index 0 tags=0x1
origin=mem32 Hero Transform+0x10C
```

C24 narrowed target-identity failure to PE/runtime metadata validation:

```text
target_rva=0x2CF7FE0
code_region_begin_rva=0x1000
code_region_end_rva=0x52A41EC
runtime_lookup_found=0
runtime_lookup_same_image=1
runtime_lookup_readable=1
PE_runtime_table_valid=0
PE_runtime_entries=527161
scan_bytes=0
admitted_bytes=0
instructions=0
snapshot_stable=0
reason=target_runtime_table_malformed_or_cap
```

The c24 implementation cap is 262144 runtime-function entries. Therefore this result does **not** establish that the PE exception directory is malformed, nor that the exact target truly lacks a runtime-function entry. No no-unwind leaf span/hash/effect was admitted.

Current unresolved outputs remain:

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

See `analysis/V01C24_RUNTIME_RESULT.md` and `analysis/V01C24_TARGET_IDENTITY.md`.

## Current bounded Codex task

Implement the smallest fail-closed refinement necessary to distinguish a structurally malformed PE exception/runtime-function table from a structurally valid table that merely exceeds c24's fixed 262144-entry cap. Only if the exact-build metadata becomes fully validated may the existing strict c24 target-identity/no-unwind path continue.

### 1. Split cap refusal from malformed metadata

Do not keep the combined `target_runtime_table_malformed_or_cap` as the only outcome.

Report separately at minimum:

- exception directory RVA and byte size validity;
- entry-size divisibility / exact derived entry count;
- in-image/readable range validity;
- configured metadata byte/entry budget;
- explicit `entry_cap_exceeded` versus structural malformed reasons;
- whether any validation was skipped because of the cap.

Do not treat `527161 > 262144` as evidence of corruption by itself.

### 2. Permit larger-table validation only under a justified bounded budget

If the exact exception directory is structurally valid, extend validation only enough to cover this exact build while remaining bounded and fixture-tested.

Preferred properties:

- derive work from the authenticated directory byte size rather than blindly trusting a count field;
- keep a hard metadata-byte ceiling and hard entry ceiling, justified in code/docs/tests;
- chunk reads; do not allocate/copy an unbounded table;
- validate all entries needed by the existing c24 contract: ordered/nonoverlapping begin/end, in-image ranges, bounded unwind RVA, no arithmetic overflow;
- preserve guarded read failure as exact refusal;
- preserve exact-build/image/code-section gates;
- do not scan arbitrary memory outside the admitted PE exception directory.

A simple larger ceiling is acceptable only if it is explicitly bounded, sufficient for the authenticated directory, and covered by owned over-cap/malformed fixtures. Do not silently make the limit effectively unbounded.

### 3. Decide target runtime-entry status only after full metadata validation

After the table is structurally valid under the admitted budget:

- determine whether any runtime-function entry contains `0x2CF7FE0`;
- compare with the existing OS runtime lookup result;
- if an exact/containing entry exists, authenticate it under the ordinary path or stop at the exact disagreement/interior-target reason;
- if **no** containing entry exists and OS lookup consistently reports none, only then permit the existing c24 strict no-unwind leaf admission to run.

Do not infer a no-unwind function from OS lookup alone while PE table validation is incomplete.

### 4. Reuse c24 no-unwind rule unchanged unless evidence requires a narrow fix

If the target reaches the strict missing-entry fallback, keep the existing constraints:

- exact target `0x2CF7FE0` only;
- <=128 bytes / <=32 instructions unless a separately justified owned-fixture change is necessary;
- guarded stable current-code snapshot;
- linear fully modeled register-only path ending in normal RET;
- no memory operand, stack construction, branch, nested CALL, indirect transfer, unsupported partial effect, or crossing another runtime region;
- no naive first-RET function finder.

If it fails, report the next exact instruction/reason and stop.

### 5. Resume c23 call-effect analysis only after identity admission

Only after target identity passes may the existing register-only selected-read effect model classify whether tag `1` is killed, returned, or reaches another exact boundary.

Even successful local completion does not settle:

```text
whole writer XYZ/W causality
cross_invocation_feedback
caller/storage ownership
lifetime/exclusivity
writability
```

### 6. Preserve safety gates

Still forbidden:

- Position/source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- guessed ABI/type semantics;
- PAGE_GUARD/data-page mutation shortcuts;
- weakening ownership/lifetime/exclusivity requirements.

Safety state remains:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

### 7. Owned tests / handoff

Add fixtures covering at minimum:

- valid exception table just below old 262144 cap;
- structurally valid table above old cap and below new bounded budget;
- table exceeding the new hard budget;
- byte size not divisible by runtime-function entry size;
- arithmetic/range overflow;
- unordered/overlapping entries;
- invalid unwind RVA;
- exact target with containing entry;
- exact target with fully validated missing entry + OS lookup none;
- lookup/table disagreement;
- no regression of c24 strict no-unwind rules;
- no regression of c23 call carrier, c22 TEST/JE, c21 same-field read, c20 EDI provenance, c19 Transform protection.

Run the relevant full build/regression chain. Update a successor analysis/result file, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable confirmed/corrected facts in `deluxxxe-dev/MSM2-Wisdoms` when accessible.

If current authenticated metadata/current-code evidence suffices after the parser/budget fix, do not request another runtime test merely by habit. If a new capture is genuinely required, emit only the minimum new metadata fields and request the shortest A/E-style test necessary. Commit and push all authorized changes to the current development line.
