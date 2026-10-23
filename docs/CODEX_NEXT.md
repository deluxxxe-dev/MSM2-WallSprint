# Next task — WallSprint v0.1c3 Third Reset Writer Multithread Trace

## Implementation base

Do not rebuild from old main source.

Use the published c2 implementation:

```text
base branch: codex/v01c2-proof-fix
implementation commit: 2d26710034dcf695fa7bb2da763f117faf6171db
published c2 head: 727fcdc1594e36e6a1ffaabf5569eee89375b871
```

A branch has already been created from the c2 head:

```text
codex/v01c3-third-writer-trace
```

Work on that branch (or an equivalent descendant of the exact c2 head). Preserve all c2 A/B behavior.

Read first:

- `analysis/V01C2_RUNTIME_RESULT.md`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_RESULT.md` from `codex/v01c2-proof-fix`

## New authoritative c2 runtime result

The real-game startup proof now passes:

```text
PATH_B_COMPACT3X3_PROOF admitted=1
caller_verified=1
writer_verified=1
rdx_to_rcx_map_verified=1
decoded_prefix_bytes=444
caller_decoded_bytes=128
pairs=9
```

Both known orientation source paths also work in the real game.

A control wall reaches:

```text
Path A applied_up=(0,0,1)
Path B applied_up=(0,0,1)
actor_up=(0,0,1)
actor_up_dot_surface=1
```

Diagonal E similarly reaches approximately:

```text
N398=(-0.707107,0,+0.707107)
Path B applied_up~=N398
actor_up_dot_surface~=1
```

The user reports an obvious visible change, but normal WallRun and surface-relative WallRun orientations rapidly alternate.

The log shows the same failure: after valid A/B Surface Up commits, Actor Up abruptly becomes World Up again.

Representative pattern:

```text
actor_up=(surface Up) dot_surface=1
...
actor_up=(0,1,0) dot_surface=0
```

Retention resets after only a few native updates / tens of milliseconds and never reaches the gate.

Path A lock contention is no longer the problem:

```text
PATH_A_STATE_LOCK_CONTENTION=0
```

Late WorldUp inputs to known path A are detected and overridden (`LATE_COMMON_WORLDUP_OVERRIDE=1`), but Actor Up still returns to World Up afterward.

## Why `NEW_RESET_AFTER_DUAL_OVERRIDE` did not identify the writer

The c2 Up hardware watch was primary-thread-only:

```text
threads_enumerated=0
installed=1
complete_thread_coverage=0
primary_only=1
primary_native_thread=<runtime thread>
```

Captured primary-thread writer events are the already-known `0x2CFC580 / 0x2CFC8F3` path and they write the requested Surface Up.

At multiple SurfaceUp -> WorldUp transitions, worker observation changes to World Up without a corresponding hardware writer event.

The strongest current hypothesis is a third writer on another process thread or otherwise outside the currently armed thread set.

## Objective

Identify the exact remaining writer. This package is diagnostic-first.

**Do not modify a third writer/source path in c3.**

Keep c2 Path A and Path B overrides enabled so the unknown writer has a clear Surface Up state to overwrite.

No adhesion, movement, animation, camera, physics, gravity, parkour, swing or combat.

## Required implementation

### 1. Preserve c2 exact-build and A/B gates

Do not weaken:

- exact exe version/SHA gate;
- c2 Capstone compact proof;
- writer/caller identity;
- lifecycle / Hero / Transform / thread / stack / freshness / kill gates;
- Path A Position preservation;
- Path B exact `[RDX,RDX+0x24)` boundary and metadata preservation;
- direct Actor Transform writes remain disabled.

Startup must still show the c2 compact proof PASS.

### 2. Add bounded process-thread Hero-Up write watch

The current `primary_only=1` watch is insufficient.

For a verified active WallRun session, enumerate the threads belonging to the current Spider-Man2 process and arm the Hero Transform Up range on every safely eligible thread for a bounded diagnostic window.

Watch the same 12 bytes:

```text
Transform + 0x10 .. +0x1B
```

using two hardware write breakpoints where possible (8 bytes + 4 bytes), preserving each thread's existing debug-register state.

Requirements:

- do not overwrite foreign/in-use DR slots;
- do not leave a thread suspended if any operation fails;
- restore each modified thread's exact previous debug-register state when the window ends, session closes, Hero/Transform changes, kill occurs, DLL shutdown occurs, or an exception path aborts setup;
- no PAGE_GUARD and no page-protection changes;
- no direct game calls;
- cap enumeration/installation to a conservative finite maximum (e.g. 128 process threads) and log if the cap is exceeded;
- log skipped/failed thread IDs and reasons only in bounded summary form;
- keep the window short enough for diagnosis, approximately from before final interpolation completion through the first repeated reset period (target roughly elapsed 0.85 to 1.50 s, or equivalent bounded trigger tied to the first near-surface Up observation).

If a new process thread appears during the diagnostic window, one bounded rescan is acceptable, but do not poll continuously.

Startup/runtime summary:

```text
MULTITHREAD_UP_WATCH
session=
process_threads_seen=
eligible=
installed=
skipped_foreign_dr=
failed=
complete_thread_coverage=0|1
window_start_elapsed=
window_end_elapsed=
```

### 3. Classify every captured Up write

For each breakpoint hit, capture at least:

```text
thread_id=
rip=
module=
module_offset_after=
return_address=
caller_rva=
caller2_rva=
up_before=
up_after=
dot_before_surface=
dot_after_surface=
relative_us_from_last_path_a_override=
relative_us_from_last_path_b_override=
```

Classify known writers:

```text
KNOWN_PATH_A    writer function 0x2CFC580
KNOWN_PATH_B    writer function 0x2CFC3C0
UNKNOWN_WRITER  anything else that actually changes current Hero Up
```

Do not assume the return address is valid until bounded stack/readability checks pass.

### 4. Emit a dedicated first unexplained reset event

When current Hero Up was sufficiently surface-relative immediately beforehand and a captured write moves it materially toward World Up, emit once per session:

```text
THIRD_WRITER_EVENT
session=
thread_id=
rip=
writer_rva=
caller=
caller_rva=
caller2=
caller2_rva=
up_before=
up_after=
dot_before_surface=
dot_after_surface=
dot_before_world=
dot_after_world=
relative_us_from_last_path_a_override=
relative_us_from_last_path_b_override=
```

Suggested trigger semantics (adjust conservatively if needed):

```text
dot_before_surface >= 0.90
AND dot_after_surface <= 0.50
AND dot_after_world > dot_before_world
```

The event is evidence only. Do not patch that writer in c3.

### 5. Capture enough static context for the next task

Once an unknown writer RIP is found, capture bounded identifying information sufficient for offline decode without committing full private game functions:

```text
THIRD_WRITER_IDENTITY
rip_after=
function_begin_rva=
function_end_rva=
function_size=
function_sha256=
caller_rva=
caller2_rva=
thread_id=
```

If needed, write the raw function bytes only to the local private runtime log marked `private_do_not_distribute=1`; do not commit them or include them in ZIPs.

Prefer hashes, RVAs and a small instruction window in Git analysis documents.

### 6. Do not let the old trace deadline hide later resets

c2's primary watch stops around the early retention period. The c3 multithread watch must remain armed long enough to cover the repeated visible flicker after interpolation reaches `t=1`.

The purpose is not a longer general profiler. It is a short bounded window specifically around the first post-dual-override WorldUp transition.

### 7. Keep retention measurement

Continue measuring:

```text
dot(ActorUp,N398) >= 0.98
```

for >=300 ms or >=20 distinct native updates.

c3 is allowed to retain the existing gate, but do not expect PASS until the third writer is owned. Do not authorize adhesion merely because brief Surface Up observations occur.

## Human test

A Control is sufficient for the primary c3 capture:

- one left-to-right WallRun, ~1.5-2 s.

If no `THIRD_WRITER_EVENT` is captured on A, test E once.

The visible rapid alternating orientation is useful confirmation, but the runtime event is authoritative.

## PASS criteria

Primary c3 success:

```text
c2 compact proof still PASS
Path A override count > 0
Path B override count > 0
MULTITHREAD_UP_WATCH installed > 1 thread
THIRD_WRITER_EVENT captured
```

Strong success:

- exact third writer function boundaries/RVA/hash identified;
- caller/caller2 and executing thread identified;
- same unknown writer repeats across A/E or repeated A runs;
- all modified debug-register states restored cleanly;
- no crash, transform corruption, position corruption or metadata write.

If multithread coverage is incomplete and the reset remains uncaptured, report exact skipped thread reasons and design the next read-only capture strategy. Do not fall back to direct Actor spam writes.

## Required outputs

Update/create:

```text
analysis/V01C2_RESULT.md
analysis/THIRD_WRITER_TRACE.md
analysis/V01C3_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c3_third_writer_trace.zip
```

## Distribution / provenance

Follow `AGENTS.md` and `docs/DEV_RULES.md`.

Do not commit user raw logs, video, full captured game functions, or private runtime byte dumps. Keep the implementation independently derived and suitable for eventual Nexus Mods distribution.
