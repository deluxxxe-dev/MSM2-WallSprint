# Next task — WallSprint v0.1c1 Compact Path-B Override + Lock-Free Path-A Admission

## Objective

v0.1b9 completed the path-B layout characterization needed for a narrow write experiment.

The next goal is to make both verified orientation source paths feed the same shared WallSprint SurfaceFrame basis long enough to pass the orientation-retention gate.

This task is still orientation-only. Do **not** add adhesion, movement, animation, camera, physics, parkour, swing or combat.

Read first:

- `analysis/V01B9_RUNTIME_RESULT.md`
- `docs/PROJECT_STATE.md`
- prior path-B analysis files under `analysis/`

## Runtime facts from v0.1b9 to treat as ground truth

Build:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

Path A:

```text
caller return RVA 0x2CFC8F3
writer RVA 0x2CFC580
```

Path B:

```text
caller function 0x2CFC7D0..0x2CFC850
caller return RVA 0x2CFC84B
writer RVA 0x2CFC3C0
writer range 0x2CFC3C0..0x2CFC57C
writer SHA256 ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196
```

For exact path-B entries on both A and E:

```text
RDX = RSP + 0x28
rdx_inside_stack_bounds=1
source_span_inside_stack_bounds=1
expected_rsp_relation_match=1
source_readable=1
```

The path-B source is a compact 3x3 orientation basis occupying exactly nine floats:

```text
RDX + 0x00, +0x04, +0x08
RDX + 0x0C, +0x10, +0x14
RDX + 0x18, +0x1C, +0x20
```

Authenticated writer bytes directly copy them into Hero orientation slots as:

```text
RDX + 0x00 -> RCX + 0x00
RDX + 0x04 -> RCX + 0x04
RDX + 0x08 -> RCX + 0x08

RDX + 0x0C -> RCX + 0x10
RDX + 0x10 -> RCX + 0x14
RDX + 0x14 -> RCX + 0x18

RDX + 0x18 -> RCX + 0x20
RDX + 0x1C -> RCX + 0x24
RDX + 0x20 -> RCX + 0x28
```

Runtime values reproduce the same semantics on A and E: the second row remains approximately World Up while path A has already tilted Hero Up toward Surface Up.

v0.1b9 also exposed a separate path-A reliability problem:

```text
COMMON_OVERRIDE_REJECT reason=state_lock_contention
```

occurred at very high frequency. The hot native hook must not depend on acquiring a shared mutex/try-lock in order to decide whether to override a qualifying current-Hero call.

## Required implementation

### 1. Preserve exact build and fail-closed behavior

All native writes remain exact-build-only.

Before any path-B source modification require at minimum:

```text
exact build accepted
verified WallRun lifecycle active
elapsed >= 0.70 s
kill switch inactive
RCX == current Hero Transform
caller return RVA == 0x2CFC84B
RDX readable
RDX == writer-entry RSP + 0x28
RDX and [RDX,RDX+0x24) inside current thread stack bounds
anchored compact-3x3 writer map self-check passed
shared WallSprint orientation snapshot valid for this session
```

Direct Actor Transform writes remain disabled.

### 2. Formalize the path-B store map with an anchored decoder/self-check

v0.1b9 captured the authenticated original writer/caller bytes, but runtime logging still reported:

```text
static_store_proof=0
requires_anchored_decode=1
```

Before enabling path-B writes, implement an accepted-build startup self-check that decodes the original unhooked bytes and proves the nine direct-copy pairs listed above.

Do not enable path-B modification if any instruction boundary, source offset, destination offset, function hash/range or caller identity differs.

Log once at startup:

```text
PATH_B_COMPACT3X3_PROOF admitted=1|0
caller_verified=
writer_verified=
rdx_to_rcx_map_verified=
source_bytes=0x24
```

Do not package full private captured game bytes in the distribution ZIP.

### 3. One shared lock-free WallSprint orientation snapshot

Both path A and path B must consume the same current interpolated orientation for a WallRun session.

Publish an immutable/lock-free snapshot from the lifecycle/update side containing at least:

```text
session generation
active flag
current Hero Transform pointer
elapsed / transition progress
current Right
current Up
current Forward
surface N398
```

Native writer hooks must be able to read a coherent snapshot without waiting on a mutex and without `try_lock` rejection.

Acceptable approaches include an atomic generation/seqlock-style snapshot or double-buffered immutable publication. Do not introduce blocking inside the native hook.

The goal is that exact qualifying calls are never rejected merely because another thread temporarily owns a state lock.

Diagnostic:

```text
PATH_A_LOCKFREE_SNAPSHOT_READ
session=
generation=
coherent=1
```

Keep this diagnostic bounded; do not spam every call.

### 4. Path A — override every exact qualifying Hero call

For exact `0x2CFC8F3 -> 0x2CFC580` calls during active WallSprint after elapsed >= 0.70:

- validate current Hero target and existing padded source layout;
- use the lock-free shared snapshot;
- supply the current shared Right/Up/Forward basis;
- preserve Position;
- do not restrict to one call per update;
- do not allow `state_lock_contention` to be an admission outcome.

Log bounded counters:

```text
PATH_A_OVERRIDE_COUNT=
PATH_A_REJECT_COUNT=
PATH_A_STATE_LOCK_CONTENTION=0
```

If a valid World-Up source is encountered and converted, retain `LATE_COMMON_WORLDUP_OVERRIDE=1`.

### 5. Path B — override only the compact 3x3 source payload

For exact verified `0x2CFC84B -> 0x2CFC3C0` calls after elapsed >= 0.70, modify only:

```text
[RDX + 0x00, RDX + 0x24)
```

Do **not** touch `RDX+0x24` or any later bytes.

Interpret the three rows as the compact basis written into Hero Right/Up/Forward.

Preserve row magnitudes/per-axis scale:

```text
sx = length(original row0)
sy = length(original row1)
sz = length(original row2)

new row0 = shared Right   * sx
new row1 = shared Up      * sy
new row2 = shared Forward * sz
```

Require finite, sane nonzero row magnitudes before writing. Use conservative bounds and fail closed if the compact source is degenerate or unexpectedly scaled.

Do not alter pointers, metadata or any source bytes after +0x20.

Diagnostic:

```text
PATH_B_COMPACT3X3_OVERRIDE
session=
elapsed=
source_ptr=
rdx_minus_rsp=0x28
original_row_lengths=(...)
original_up=(...)
applied_up=(...)
applied_up_dot_surface=
bytes_written=0x24
metadata_untouched=1
caller_verified=1
target_verified=1
stack_verified=1
```

### 6. Keep the hardware Up watch as verification only

Use the bounded pre-armed primary-thread Hero Up watch to verify:

- path B no longer immediately restores World Up;
- no late path-A World-Up write survives;
- whether any new reset writer appears after both overrides.

If another writer resets orientation after both paths were successfully overridden, log:

```text
NEW_RESET_AFTER_DUAL_OVERRIDE
writer_rva=
caller_rva=
latency_ms=
up_before=
up_after=
```

Do not respond by adding direct Actor spam writes.

### 7. Orientation-retention gate

After transition completion, require:

```text
dot(ActorUp, N398) >= 0.98
```

for either:

```text
>= 0.30 seconds
```

or

```text
>= 20 distinct native updates
```

while the same verified WallRun session remains active.

Log:

```text
ORIENTATION_RETENTION_PASS=1
held_ms=
held_updates=
```

This is the only result that authorizes the following adhesion milestone.

### 8. No adhesion yet

Even if retention succeeds, this package must not add the 0.5 m standoff controller, surface-relative movement, animation or camera work.

## Expected visible behavior

Unlike v0.1b9, this package should produce an obvious visible change **if orientation ownership succeeds**.

Because the camera is still world-up, Spider-Man should rotate sideways relative to the screen/world as Actor Up approaches wall Surface Up.

Animation may look incorrect. That is acceptable for this milestone.

If there is still no visible change, the runtime log must identify whether:

- path-A override failed,
- path-B compact override failed,
- both succeeded but another writer reset orientation,
- or retention did not last long enough.

## Human test

A Control:

- one left-to-right WallRun, about 1.5 s if available.

E diagonal:

- same known physical diagonal face;
- one left-to-right WallRun, about 2 s.

Crawl not required.

Video is useful for visible orientation, but runtime log remains authoritative.

## PASS criteria

Minimum implementation success:

- anchored compact-3x3 map proof passes;
- path-A hooks no longer reject due to state-lock contention;
- exact current-Hero path-B stack source at RSP+0x28 is modified only in first 0x24 bytes;
- row magnitude/per-axis scale preserved;
- no direct Actor write, Position write or metadata write.

Primary runtime success:

```text
ORIENTATION_RETENTION_PASS=1
```

on at least one test wall.

Strong success:

- retention passes on both A and E;
- no `NEW_RESET_AFTER_DUAL_OVERRIDE` during observed window;
- no crash, transform corruption or position corruption.

## Required outputs

Update/create:

```text
analysis/V01B9_RESULT.md
analysis/PATH_B_COMPACT3X3_PROOF.md
analysis/DUAL_SOURCE_RETENTION.md
analysis/V01C1_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c1_compact_path_b_override.zip
```

## Distribution / provenance

Follow `AGENTS.md` and `docs/DEV_RULES.md`.

Use only WallSprint-owned source plus independent static/runtime analysis of Spider-Man2.exe. Do not copy reference-mod code, binaries or assets.
