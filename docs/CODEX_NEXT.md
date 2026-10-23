# Next task — WallSprint v0.1b9 Reset-Source Layout Characterization

## Objective

v0.1b8 authenticated and hooked the second orientation reset path, but correctly refused to modify its RDX block because the source did not validate as the same affine matrix layout used by the common writer.

Do not weaken the safety checks merely to force a write.

The next task is diagnostic-first: determine the actual source representation and exact orientation-bearing fields used by:

```text
caller return RVA 0x2CFC84B
        ↓
writer RVA 0x2CFC3C0
        ↓
current Hero Transform
```

Only after the source semantics are proven should a later package attempt a second-path override.

This remains orientation-only. Do not add adhesion, movement, animation, camera, physics, parkour, swing or combat.

## v0.1b8 runtime facts to treat as ground truth

Build gate passed for Spider-Man2.exe 2.810.0.0 / SHA256:

`c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`

Second path startup/authentication passed:

```text
RESET_COPY_HOOK admitted=1 installed=1
writer_rva=0x2cfc3c0
caller_return_rva=0x2cfc84b
DUAL_SOURCE_ADMISSION admitted=1
```

But on both A and E the actual source entry failed closed:

```text
source_readable=1
source_is_stack_local=0
layout_valid=0
override=0
reason=caller_local_provenance_or_source_sanity_failed
```

Representative entry:

```text
RDX = RSP + 0x28
RDX lies numerically inside the captured current-thread stack bounds
```

The raw 0x40 bytes do not match the normal affine 4x4 source layout used by `0x2CFC580`.

Therefore do **not** assume:

```text
RDX+0x00 Right
RDX+0x10 Up
RDX+0x20 Forward
RDX+0x30 Position
```

for this second writer.

Path A remains valid and should be preserved for correlation:

```text
0x2CFC8F3 -> 0x2CFC580
```

Its source override still briefly drives Hero Up toward Surface Up.

Detailed evidence is in:

- `analysis/V01B8_RUNTIME_RESULT.md`
- `docs/PROJECT_STATE.md`

## Required implementation

### 1. Keep all existing exact-build / fail-closed behavior

Do not introduce writes to the second source path in this package unless the task itself proves the exact fields beyond reasonable ambiguity.

Default expected behavior is read-only characterization for path B.

Direct Actor Transform writes remain disabled.

### 2. Static decode the exact path

Authenticate and decode:

```text
caller function 0x2CFC7D0 .. 0x2CFC850
call site returning at 0x2CFC84B
writer 0x2CFC3C0 .. 0x2CFC57C
```

Use the original accepted-build bytes before local hook patching.

Produce a concise instruction-level data-flow map answering:

- where RDX comes from at the call site;
- whether RDX is `RSP+constant`, a nested pointer, a temporary object, or a mixed structure;
- every writer instruction that reads `RDX + offset` before it writes orientation-relevant bytes to RCX;
- which RCX offsets are direct copies from which RDX offsets;
- which RCX orientation components are later computed rather than directly copied;
- whether R8/R9 or constants participate in the orientation result.

Do not infer field names from visual resemblance alone.

### 3. Capture path-B entry data with explicit stack math

For exact verified calls only, log:

```text
RESET_COPY_LAYOUT_ENTRY
session=
elapsed=
thread_id=
caller_rva=
RCX=
RDX=
R8=
R9=
RSP=
stack_limit=
stack_base=
rdx_minus_rsp=
rdx_inside_stack_bounds=
source_readable=
raw_source_0x80=
```

Important: fix the misleading old `source_is_stack_local` diagnostic so it separately reports:

```text
rdx_inside_stack_bounds=
expected_rsp_relation_match=
```

Do not collapse these into one boolean.

### 4. Extract same-offset scalar/vector candidates without assuming a matrix

From `[RDX+0x00, RDX+0x80)`:

- list finite float values at 4-byte alignment;
- list Vec2/Vec3/Vec4 candidates only as observations;
- record pointer-looking qwords separately;
- do not dereference pointer-looking qwords unless the static data flow proves the writer actually dereferences them and the read can be bounded safely.

For A and E, compare the same offsets against:

```text
WorldUp
N398
current path-A interpolated Up
Hero Up before path-B call
Hero Up after path-B writer stores
```

Score correlations but label them candidates until static flow confirms use.

### 5. Correlate exact writer stores

The hardware Up watch already shows multiple post-store RIPs inside `0x2CFC3C0`.

For each relevant store that changes current Hero Up, decode the exact instruction and report:

```text
PATH_B_STORE
instruction_rva=
destination_rcx_offset=
source_kind=RDX_direct | register | SIMD_computed | constant | other
source_rdx_offset=... if direct
before=
after=
```

The goal is to establish which path-B source values actually cause the World-Up reset.

### 6. Preserve path-A interpolation only as a reference signal

Keep the existing common-writer source override available so path B receives a visibly non-World-Up current Hero state to overwrite.

Do not abort the whole WallSprint diagnostic session merely because path B layout is unknown. Instead:

```text
path_b_override=0
path_b_characterization_active=1
```

and continue bounded tracing through the diagnostic window.

This avoids v0.1b8 ending characterization immediately after the first failed path-B sanity check.

### 7. Diagnose late path-A World-Up calls separately

Continue to log exact `0x2CFC8F3 / 0x2CFC580` calls after elapsed >= 0.70.

If an exact valid Hero call remains unmodified, log the admission failure reason explicitly rather than only `source_overridden_this_entry=0`:

```text
COMMON_OVERRIDE_REJECT
reason=
caller_ok=
target_ok=
stack_ok=
source_layout_ok=
lifecycle_ok=
elapsed_ok=
```

Do not broaden admission without evidence.

### 8. No path-B write unless a strict proof gate passes

A write experiment in this package is optional and should occur only if all of the following are established during implementation/runtime characterization:

- exact source field(s) used for Hero orientation are statically mapped;
- same field semantics reproduce on both A and E;
- the field values correlate correctly with World-Up reset behavior;
- source provenance is caller-local and bounded;
- modifying those exact fields does not require touching position/pointers/metadata.

If any condition remains uncertain, keep v0.1b9 read-only for path B and report the next exact hypothesis.

### 9. Human test

A Control:

- one left-to-right WallRun, ~1.5 s if available.

E:

- same known diagonal face;
- one left-to-right WallRun, ~2 s.

Crawl not required.

Visible behavior is not required for this diagnostic package. The runtime map is the success criterion.

## PASS criteria

Primary diagnostic success:

- actual `0x2CFC84B -> 0x2CFC3C0` RDX provenance established;
- exact writer data flow from RDX/registers/constants to Hero orientation stores documented;
- same relevant source offsets/semantics reproduced on A and E;
- no unsupported assumption that path B is the same matrix format as path A.

Strong success:

- exact field(s) responsible for World-Up reset identified sufficiently to design a narrowly scoped v0.1c/path-B source override.

Do not authorize adhesion yet. Orientation retention still must pass first.

## Required outputs

Update/create:

```text
analysis/V01B8_RESULT.md
analysis/PATH_B_CALLER_DECODE.md
analysis/PATH_B_SOURCE_LAYOUT.md
analysis/PATH_B_STORE_MAP.md
analysis/V01B9_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1b9_path_b_layout.zip
```

## Distribution/provenance

Follow `docs/DEV_RULES.md` and `AGENTS.md`.

Use only WallSprint-owned source plus independent runtime/static analysis of Spider-Man2.exe. Do not copy reference-mod code, binaries or assets.
