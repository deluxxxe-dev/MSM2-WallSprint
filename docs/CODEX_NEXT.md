# Next task — WallSprint v0.1b8 Dual-Source Orientation Ownership

## Objective

v0.1b7 identified the immediate reset path on both A and E.

Do not abandon the successful source-injection architecture and do not globally patch transform writers.

The next goal is to make both verified WallRun orientation source paths feed the same SurfaceFrame orientation so the Hero Actor remains surface-relative long enough to pass the orientation-retention gate.

This task is still orientation-only. Do not add adhesion, movement, animation, camera, parkour, swing, combat, or physics fixes.

## Important repository continuity note

A previous Codex cloud task produced the tested v0.1b7 package, but those working-tree changes may not have been committed to GitHub `main`.

Before implementing b8:

1. inspect the current repository;
2. determine whether v0.1b7 pre-arm/common-writer trace support is present;
3. if absent, reconstruct the required b7 support from the existing b6 source plus:
   - `analysis/V01B7_RUNTIME_RESULT.md`
   - `docs/PROJECT_STATE.md`
   - the previous b7 requirements in git history if useful.

Do not assume an isolated cloud task inherits an earlier cloud working tree.

## v0.1b7 runtime facts to treat as ground truth

### Source path A — existing common writer

Writer:

`RVA 0x2CFC580`

WallRun caller return:

`RVA 0x2CFC8F3`

The known RDX source transform layout for relevant Hero calls is:

```text
+0x00 Right
+0x10 Up
+0x20 Forward
+0x30 Position
```

The current stack-local source override reaches the actual Hero Transform and produces Surface-relative Actor Up briefly.

However, v0.1b7 also captured later `0x2CFC580 / 0x2CFC8F3` calls with:

```text
source Up = WorldUp
source_overridden_this_entry = 0
```

One strong A sample changed full Surface Up back to World Up about 14.9 ms after the prior override.

Therefore b8 must make the admission logic cover every exact valid Hero/WallRun call from this caller during active WallSprint, including the late World-Up invocation, not only the currently admitted subset.

### Source path B — immediate reset writer

Repeated on both A and E:

```text
writer function RVA 0x2CFC3C0
function range 0x2CFC3C0..0x2CFC57C
size 444 bytes
captured runtime SHA256 ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196
caller return RVA 0x2CFC84B
caller2 RVA 0x35184A5
```

This path acts roughly 0.8–1.0 ms after the WallSprint source commit in representative samples.

Captured function bytes show direct dword copies from RDX to RCX across the basis, including Up at:

```text
RCX + 0x10 <- RDX + 0x10
RCX + 0x14 <- RDX + 0x14
RCX + 0x18 <- RDX + 0x18
```

In captured events:

- RCX = current Hero Transform;
- RDX = nearby stack-local address;
- representative post-prologue relation was approximately RDX = RSP + 0x60;
- E shows this writer sequentially driving a Surface-like Up toward `(0,1,0)`.

Do not use the stale `RESET_ENTRY_SOURCE` from the common-writer hook as the source for this second writer. Its `entry_correlated=0` was explicit. b8 must capture the actual entry arguments of `0x2CFC3C0` itself.

## Required implementation

### 1. Preserve exact safety gates

All native modification remains accepted-build-only and fail-closed.

Required minimum checks before any source modification:

- exact executable version/hash gate passes;
- verified WallRun lifecycle active;
- current Hero and current Hero Transform are known and stable;
- elapsed WallRun >= 0.70 s;
- Surface N398 finite, normalized and valid for current planar vertical-wall test;
- kill switch inactive;
- target pointer and source pointer readable;
- target exactly matches current Hero Transform;
- caller identity exactly matches the authenticated path being handled.

Direct Actor Transform writes remain disabled.

### 2. One shared WallSprint orientation state

Do not calculate two independent transitions that can drift.

Create/retain one per-WallRun-session orientation state containing at least:

```text
entryTime
entryBasis / entryQuaternion
entryForward
target SurfaceFrame basis
current interpolated basis
```

Entry delay remains about 0.70 s.

Rotate time remains about 0.25 s.

The same interpolated Right/Up/Forward at a given session time must be supplied to both native source paths.

### 3. Fix common-writer path coverage

For `0x2CFC580`:

Only alter a source when all exact guards pass, including:

```text
caller return RVA == 0x2CFC8F3
RCX == current Hero Transform
RDX is the expected readable caller-local source matrix
verified WallRun active
elapsed >= 0.70
```

But do not artificially restrict this to only one invocation per native update if that causes the later World-Up call to pass through unmodified.

Every qualifying current-Hero call from `0x2CFC8F3` during active WallSprint should receive the current interpolated SurfaceFrame orientation.

Keep Position unchanged.

Add diagnostics:

```text
COMMON_SOURCE_OVERRIDE
session=
elapsed=
caller_rva=
source_ptr=
source_up_before=
source_up_after=
late_worldup_source_detected=
position_unchanged=1
```

If an invocation would previously have had `source_overridden_this_entry=0` but now qualifies and is converted from World Up, log:

```text
LATE_COMMON_WORLDUP_OVERRIDE=1
```

### 4. Authenticate and instrument `0x2CFC3C0`

Resolve the accepted-build runtime function boundary and exact bytes/hash before installing anything.

Target:

```text
begin RVA 0x2CFC3C0
end RVA 0x2CFC57C
size 444
expected captured SHA256 ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196
```

Be careful if a local hook changes the runtime prefix: authenticate original bytes before patching, as done for existing hooks.

Use an ABI-neutral raw-entry hook/thunk. Do not invent a C++ typed game signature.

At entry capture raw:

```text
RCX
RDX
R8
R9
caller return RVA
thread id
RSP / stack bounds
```

### 5. Exact admission for the second path

Only consider modifying `0x2CFC3C0` source when:

```text
caller return RVA == 0x2CFC84B
RCX == current Hero Transform
verified WallRun active
elapsed >= 0.70
same expected primary/native WallRun ownership context
RDX readable
RDX source matrix sane
RDX lies inside the current thread stack or otherwise passes an equally strong caller-local provenance check
```

Log the actual source layout before changing it.

Do not assume `RDX+0x10` is Up solely from the old common-writer layout; verify the matrix values at this function's own entry. The static copy instructions strongly suggest the same transform basis layout, but b8 should record the proof.

Expected diagnostic:

```text
RESET_COPY_SOURCE_ENTRY
session=
elapsed=
caller_rva=0x2cfc84b
RCX=
RDX=
RSP=
source_is_stack_local=
source_right=
source_up=
source_forward=
source_position=
source_up_dot_world=
source_up_dot_surface=
```

### 6. Modify only the second caller-local source

If all guards pass and the entry source proves to be the expected transform layout, replace only orientation rows:

```text
RDX + [0x00, 0x30)
```

with the shared current WallSprint Right/Up/Forward basis.

Do not modify Position at `+0x30`.

Do not NOP or skip `0x2CFC3C0`.

Do not globally modify calls to this writer.

Do not directly overwrite RCX/Hero Transform.

Diagnostic:

```text
RESET_COPY_SOURCE_OVERRIDE
session=
elapsed=
source_ptr=
original_up=
applied_up=
applied_up_dot_surface=
position_unchanged=1
caller_verified=1
target_verified=1
source_stack_verified=1
```

### 7. Keep a bounded write-watch for verification

Retain the pre-armed primary-thread Hero Up hardware watch in a reduced verification role.

The purpose now is to answer:

- did `0x2CFC3C0` still write World Up after source override?
- did any other writer appear?
- did the late `0x2CFC580` call still write World Up?
- how long did Surface Up survive?

Do not let heavy tracing create large event/drop storms. Reduce diagnostic volume where possible now that the two known paths are identified.

### 8. Retention metric

After transition completion (roughly elapsed >= 0.95 s), continuously compute:

```text
actor_up_dot_surface
actor_up_dot_world
```

PASS orientation ownership when:

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

while WallRun lifecycle remains active.

Log:

```text
ORIENTATION_RETENTION_PASS=1
held_ms=
held_updates=
```

### 9. Failure classification

If orientation still resets, classify the first actual reset after both source overrides:

```text
NEW_RESET_AFTER_DUAL_OVERRIDE
writer_rva=
caller_rva=
latency_ms=
up_before=
up_after=
```

Do not respond by adding direct Actor spam writes.

### 10. No adhesion yet

Even if orientation retention succeeds, do not add the 0.5 m wall standoff controller in this package.

b8 success only authorizes the next milestone to begin adhesion.

## Expected visible behavior

If b8 succeeds, the user should finally be able to see Spider-Man rotate relative to the wall because the orientation should survive across frames.

Camera remains world-up, so a successful full Surface Up transition will look visually like Spider-Man has rotated sideways relative to the screen/world.

Animation may look wrong. That is acceptable for this milestone.

## Human test

A Control:

- current known flat wall near E;
- one left-to-right WallRun;
- use whatever wall length is available; about 1.5 s is enough if transition/retention is reached.

E:

- exact same known diagonal face;
- one left-to-right WallRun;
- about 2 s is enough.

Crawl is not required.

Record video if convenient, but the runtime log remains authoritative for the retention gate.

## Required outputs

Update/create:

```text
analysis/V01B7_RESULT.md
analysis/RESET_COPY_WRITER_ANALYSIS.md
analysis/DUAL_SOURCE_OWNERSHIP.md
analysis/V01B8_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1b8_dual_source_ownership.zip
```

## PASS criteria

Minimum:

- actual entry args for `0x2CFC3C0 / caller 0x2CFC84B` captured and source layout proven;
- exact current-Hero + WallRun guards pass;
- its caller-local source orientation is replaced without Position modification;
- every qualifying late `0x2CFC8F3 / 0x2CFC580` World-Up source is also overridden.

Primary success:

```text
ORIENTATION_RETENTION_PASS=1
```

on at least one test wall.

Strong success:

- retention passes on A and E;
- no unknown reset path remains during the observed WallRun window;
- no crash / transform corruption / position corruption.

## Distribution/provenance

Follow `docs/DEV_RULES.md`.

Use only WallSprint-owned source plus independent runtime/static analysis of Spider-Man2.exe. Reference mods remain research hints only. Do not copy their code, binaries, or assets.