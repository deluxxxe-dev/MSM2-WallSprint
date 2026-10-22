# Next task — WallSprint v0.1b7 Pre-Armed Reset Writer Capture

## Objective

Do not redesign the successful native source override.

The current blocker is that Actor Up briefly becomes surface-relative, then returns to World Up about 10–20 ms later.

v0.1b6 failed to identify the reset writer because its diagnostic trace was disabled/not admitted and attempted to arm roughly 68–88 ms after the relevant source commit.

Implement v0.1b7 so the reset path is already being watched **before** the 0.70 s source override begins.

## Required behavior

### 1. Keep existing source override

Retain the v0.1b5/v0.1b6 source override path:

- exact accepted build only;
- verified WallRun lifecycle active;
- source override starts at elapsed >= 0.70 s;
- caller return RVA `0x2CFC8F3`;
- same current Hero Transform;
- verified caller-local stack source matrix;
- orientation rows only;
- no Position modification;
- direct Actor Transform write remains disabled;
- adhesion/movement/camera/animation remain disabled.

### 2. Pre-arm at 0.60 s

For each verified WallRun session:

- at elapsed >= 0.60 s, arm reset diagnostics;
- source override still begins at 0.70 s;
- diagnostics remain active through approximately 1.10 s or until reset candidate is captured;
- disarm immediately on WallRun Deinit, kill switch, identity loss, or diagnostic completion.

Do not wait for worker polling to observe a surface-like Actor Up before arming.

### 3. Fix admission

The v0.1b7 package is not valid if reset tracing silently remains disabled.

Add startup/self-check logs equivalent to:

```text
RESET_TRACE_SELF_CHECK
exact_build=1
common_writer_hook=1
reset_trace_enabled=1
reset_trace_admitted=1
primary_thread_prearm_supported=1
```

If any required field is false, clearly log:

```text
V01B7_DIAGNOSTIC_NOT_ADMITTED
```

and do not call the diagnostic package successful.

### 4. Trace all common-writer calls to current Hero Transform

The known common writer is:

`RVA 0x2CFC580`

Between roughly 0.60–1.10 s of the verified WallRun session, record **all calls targeting the current Hero Transform**, not only caller `0x2CFC8F3`.

For each matching invocation, capture read-only:

```text
COMMON_WRITER_TRACE
session=
elapsed=
thread_id=
caller_rva=
RCX=
RDX=
R8=
R9=
source_right=
source_up=
source_forward=
source_up_dot_world=
source_up_dot_surface=
actor_up_before=
N398=
source_overridden_this_entry=
```

Only interpret RDX as the known source transform when pointer/readability/sanity checks pass.

The purpose is to see whether a second caller invokes the same writer with World Up immediately after the WallSprint surface-relative call.

### 5. Pre-armed hardware Up write watch

Also use the existing hardware watch mechanism on current Hero Transform Up `[+0x10,+0x1C)`.

Important change from b6:

- install on the known primary native WallRun thread first;
- do not enumerate/suspend a large thread set before securing the primary thread;
- at source override time, log that the watch was already armed.

Required status log:

```text
PREARM_STATUS
session=
elapsed=
primary_thread=
DR0_installed=
DR1_installed=
trace_already_armed_before_override=
```

`trace_already_armed_before_override=1` is required for a successful diagnostic session.

Additional bounded thread coverage may be added after the primary thread, but full-process coverage is less important than being armed in time.

### 6. Hardware write event

For a write to the watched Hero Up region, log:

```text
RESET_TRACE_EVENT
session=
relative_us_from_last_override=
thread_id=
RIP=
writer_rva=
return_address=
caller_rva=
write_address=
up_before=
up_after=
dot_before_surface=
dot_after_surface=
dot_before_world=
dot_after_world=
N398=
```

Use the actual observed data and keep caveats about breakpoint RIP being post-store where applicable.

### 7. Candidate classification

Classify a reset candidate when the observed transition is genuinely surface-like to world-up, approximately:

```text
dot(before, N398) >= 0.50
dot(after, WorldUp) >= 0.98
```

Log:

```text
RESET_WRITER_CANDIDATE=1
```

Resolve native function boundary, begin/end RVA, size, hash, and caller where safe and evidence-backed.

### 8. Correlate with common writer

If a hardware reset event occurs inside `0x2CFC580`, correlate it to the matching `COMMON_WRITER_TRACE` entry using thread/timing/target/caller evidence.

If correlated, report:

```text
RESET_PATH_CORRELATED=1
common_writer_reset=1
reset_caller_rva=...
reset_source_up=...
```

If the reset RIP belongs to another writer, report `common_writer_reset=0` and preserve that function as the new reset-writer candidate for offline analysis.

### 9. Do not patch the reset yet

This version is diagnostic-first.

Do not:

- NOP the reset writer;
- skip an unknown caller;
- globally force Surface Up;
- spam direct Actor writes;
- add adhesion/movement/animation/camera/physics fixes.

First identify the actual reset path.

## Human test

A Control:

- one left-to-right WallRun;
- approximately 1.5 s is enough;
- no Crawl required.

Face E:

- same known diagonal wall;
- one left-to-right WallRun;
- approximately 2 s is enough;
- no Crawl required.

Visible behavior may still appear unchanged. This task is successful based on the diagnostic capture, not visible rotation.

## PASS criteria

Minimum useful session:

```text
trace_already_armed_before_override=1
RESET_TRACE_EVENT captured >= 1
```

Primary success:

- actual surface-like → WorldUp reset write identified, or
- a second current-Hero common-writer invocation with WorldUp source is identified immediately after the surface-relative source invocation.

Strong verification requires the same reset writer/caller path to reproduce on explicitly confirmed A and E sessions.

## Required outputs

Update/create:

```text
analysis/V01B6_RESULT.md
analysis/PREARMED_RESET_TRACE.md
analysis/COMMON_WRITER_RESET_CALLS.md
analysis/RESET_WRITER_VERIFICATION.md
analysis/V01B7_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Build/package name:

```text
WallSprint_v0.1b7_prearmed_reset_capture.zip
```

## Provenance / distribution constraints

Follow `docs/DEV_RULES.md`.

This task must remain based on WallSprint runtime instrumentation and independent Spider-Man2.exe analysis. Do not copy code, assets, binaries, or proprietary payloads from reference mods.