# v0.1c46 scalar observer gate runtime result — 2026-11-01

Branch: `codex/v01c20-edi-value-provenance`.

## Owner runtime facts

The supplied `v0.1c46 / package 0.1.71` log ran on the pinned Spider-Man2.exe 2.810.0.0 build with SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9` and accepted the exact target-build gate.

The WallRun scalar plan itself reported enabled with the expected five direct 4-byte offsets:

`0x208 / 0x220 / 0x238 / 0x250 / 0x40C`.

The c45 Gravity presence diagnostic was correctly disabled.

One real verified WallRun was captured:

- Init = 1
- Entry = 1
- Update = 1889
- Deinit = 1
- duration = about 22.273624 s
- Update rate = about 84.809/s

The supplied video also shows a sustained ordinary wall run during the test interval, so insufficient WallRun activity is not the explanation.

However, the c46 log contains:

- zero `wallrun_update_scalar_sample` records;
- zero `wallrun_update_scalar_summary` records;
- zero `surface_source_session` records.

This must **not** be interpreted as the five fields reading zero, being unreadable, or being absent.

## Root cause

Static source inspection after the runtime showed:

- `telemetry.cpp::settings()` intentionally sets `result.hooks.surfaceTelemetry=false`;
- `wallrun_hooks.cpp` still calls `msm2::source::begin/observe/finish` during verified WallRun lifecycle/object observation;
- but c46 `surface_source.hpp` returned from `begin`, `observe`, and `finish` on `!runtimeObservationEnabled` before the newly added scalar observer could arm/sample/finalize.

Therefore the five-field scalar observer never executed in c46 even though `wallrun_update_scalar_plan enabled=1` was printed.

Classification:

- verified WallRun lifecycle: **VERIFIED-RUNTIME**;
- five-field plan admission: **VERIFIED-RUNTIME**;
- c46 scalar field values/changes: **UNKNOWN / NOT OBSERVED**;
- c46 zero sample count as engine evidence: **REJECTED**;
- cause of missing samples: **VERIFIED-STATIC** from current source control flow.

## c47 correction

`v0.1c47 / package 0.1.72` keeps legacy rich surface telemetry disabled but moves only the five scalar operations across that gate:

- scalar session begin occurs before the legacy telemetry early return;
- five direct owner+offset 4-byte reads occur before the legacy telemetry early return;
- scalar summary finalization occurs before the legacy telemetry early return;
- old vector/root/child-pointer observation remains behind `runtimeObservationEnabled` and therefore remains disabled in this build.

A startup/session log key `wallrun_update_scalar_session` was added so the next runtime proves that scalar capture actually armed. Static regression now explicitly checks that `surfaceTelemetry=false` remains present while scalar begin/observe/finish precede the corresponding early-return gates.

No new native hook, pointer dereference, game call, gameplay write, gravity write, movement write, orientation write, adhesion write, or native-source override was added.

## Required next runtime

After the owner full updater pipeline passes for c47, repeat only one ordinary WallRun:

left ~3 s -> right ~3 s -> down ~5 s -> natural Deinit.

The next log must contain `wallrun_update_scalar_session` and then either scalar samples/summaries or an explicit bounded read-failure summary. Only that c47 result can answer whether these five direct Update fields are useful.
