# WS WallRun movement/facing separation — bounded source review

2026-10-30 JST. Source baseline `28c5fa6721818e3c5c8e3bdc7c190db7de7c85b3`,
branch `codex/v01c20-edi-value-provenance`. Documentation only.

## Accepted evidence; extraction investigation closed for now

**VERIFIED-STATIC (ChatGPT-inspected supplied second export, not rehashed here):**

```text
hero_wallrunsteeringsetup_standard(1).config
544 bytes
SHA256 d83095b5e68cc6fa20c95bc044345f7f46fd6c0a35a682df25374b140dfcec2d
vanilla SHA256 32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b
exactly one changed byte: file offset 0x219, 75 -> 5
Fast at file offset 0x218 remains 100
```

The intended Slow5 payload is present. Owner-workflow evidence supports an
installed replacement at the later extraction time; source/copy hashes and a
complete GUI trace were not supplied. This does not authenticate the earlier
game session's installed state. The earlier vanilla export does not establish
installation failure; do not repeat the extraction investigation by default.

**VERIFIED-RUNTIME (owner observation only):** Slow5 produced no visible change.
Standard/Slow runtime selection and actual consumption remain **UNKNOWN**;
do not infer that all isolation steps were independently verified.
The accepted Fast A/B also shows the WS moonwalk-like symptom with probe ON and
OFF, not numerical Transform/animation causality. Apply the stopping rule:
no additional steering values, c73, or broad movement investigation.

## Current runtime path — VERIFIED-STATIC (WS source)

| Location / function | Narrow finding |
| --- | --- |
| `source/src/msm2/telemetry.cpp:106` `run`, especially 111–117 and 130–142 | Configures the legacy writer (disabled) and native-source module; worker calls `msm2::native_source::tick` and drains its logs. This is the reachable WS orientation path, conditional on existing configuration/admission/lifecycle guards. |
| `source/src/NativeHooks/native_source_override.cpp:348` `configure`, 352–359 | Admission requires enabled/sourceOverride plus existing guards. Existing diagnostic declares `movement=0 animation=0 position_write=0`; this source review does not prove the owner's configuration/admission during a reversal. |
| Same file:392 `tick`, 450–465 | Once the entry delay passes, builds `baseline` from the current Hero Transform prefix/position, calls `state.transition.enter` only while the transition is inactive, and retains `state.entryBasis`. Later samples use `incoming=state.entryBasis`, not a newly selected movement heading. Publishes the interpolated axes as `p.basis`. |
| `source/include/Math/source_orientation.hpp:20` `Transition::enter`, 27 `sample` | Target Up is the normalized entry wall normal. Target Forward is entry Transform Forward projected onto that surface and normalized; Right is rebuilt by cross products, then Forward re-orthogonalized. `sample` slerps the saved entry quaternion to the saved target; it never accepts movement/input/facing arguments or recomputes target from current displacement. |
| `source/src/msm2/orientation_source.cpp:88` `ws_source_capture` → native-source `capture`/`capturePath` at 482–509 | Path A substitutes **all three** published orientation rows in the authenticated caller-local matrix; `writeStackOrientation` at `source/src/GameInterop/stack_source.cpp:41` writes 48 bytes and preserves the Position/W row. This is a source write, not a direct Actor/Transform store by WS. |
| `source/src/NativeHooks/reset_copy_source.cpp:65` `ws_reset_copy_capture` → `captureCopy` at native-source 512, especially 561/596 | Path B uses the same published basis via `compactReplacement` (`source/include/Math/compact_basis.hpp:15`); preserves each incoming row's scale and the 28 metadata bytes, writes the 36-byte compact orientation block via `stack_source.cpp:33`. It does not select a different heading. |

### What happens on direction reversal

There is **no current WS runtime movement-direction selector or reversal branch**
in this path: no directional input/velocity argument enters `Transition` or the
A/B basis replacement. Its movement sampling is read-only. The selection of the
game's displacement/animation direction is outside this inspected WS code;
identifying its engine owner would require new evidence, not guessing an API.

Within the same active transition/session, changing displacement alone does not
negate Forward/Right or rebuild the target. During interpolation all axes can
change due to the entry-to-wall rotation. At `t=1`, Up remains the entry surface
normal and Forward/Right remain the entry-derived surface heading. This is
**not** an Up-only patch that leaves incoming Forward/Right intact: both source
paths replace all orientation axes. A new session can choose a new entry basis;
invalid/changed normals, lifecycle/guard failure and the existing bounded window
can end the override (`tick` 414–427, 457–460). No new lifetime claim is made.

This is the smallest static separation relevant to the symptom: displacement
can be handled elsewhere while WS retains an entry-derived orientation source.
That is a **HYPOTHESIS for the observed visible mismatch**, not a demonstrated
cause. Source substitution does not prove the final Transform stayed fixed or
that animation/mesh presentation follows that Transform after other writers.

### Do not mistake dormant helpers for current movement behavior

`source/include/wallsprint.hpp:30` `ws::Controller::tick` selects input direction
at 51–56 and constructs a direction-aligned pose. `source/include/planar_mvp.hpp:37`
`ws::planar::Controller::update` selects projected velocity/input at 55–70.
Targeted class-usage searches find instantiations only in `core_tests.cpp` and
`planar_mvp_tests.cpp`; no runtime instantiation was found. The production link
list (`source/tools/build_windows.sh:18`–42), telemetry call path, and hard-disabled
`source/src/msm2/actor_writer.cpp:8`–16 agree. Parsing the old movement INI option
(`telemetry.cpp:85`) does not activate that dormant controller. Its behavior must
not be used to explain the current game reversal.

## Existing diagnostics: two numerical quantities, no animation-facing channel

| Quantity | Existing source/log and limit |
| --- | --- |
| Observed displacement direction | `native_source_override.cpp:471`–474 emits bounded `MOVEMENT_DELTA_SAMPLE`: world delta, finite-difference velocity, and projections onto **published `p.basis`**, not an independently observed facing basis. Requires active source, `t>=1`, distinct native updates and >=100 ms; 40 sample attempts/session. `Math/movement_delta.hpp:5` defines the calculation. Not physics velocity or input intent. |
| Sampled Actor/Transform Forward | `msm2/sdk_bridge.cpp:30`–44 `sampleCallback` reads Hero's Transform prefix. `diagnostics.hpp:35`–36 maps Forward to row2. `telemetry.cpp:154`–159 logs `Actor axes ... Forward` and position-difference velocity when debug/availability/timing permit (ordinary log interval >=500 ms). `orientation_log.cpp:15`–26 diagnostic blocks also log rows/actor mapping and motion. These are worker snapshots, not guaranteed post-writer or reversal-synchronous samples. |
| Source Forward versus actual retention | `NATIVE_SOURCE_OVERRIDE` at native-source:626 logs original/published-target/applied Forward for Path A; its `target_forward` is the **current published interpolated basis**, not necessarily the final entry target before `t=1`, and is not a final Actor readback. `NATIVE_SOURCE_ACTOR_OBSERVATION` at 628 / `observe` 316–342 verifies **Up only**; retention PASS is not Forward/facing retention proof. Path B's primary override summary at 624 reports Up/scales, not a corresponding full Forward observation. |
| Visible animation/body facing | No independent mesh/skeleton/root-motion/animation-facing measurement, directional-input record or numeric three-way reversal comparison was found in these reachable paths and existing log schemas. `Snapshot` (`msm2/sdk_bridge.hpp:7`) contains Transform data, not animation state. Key markers in `diagnostics.hpp:37` are state labels, not a facing measurement. |

Existing logs can distinguish displacement from sampled Transform Forward **if
valid corresponding records cover the event**. They cannot by themselves
separate Transform orientation from visible animation facing. No reversal-specific
numeric runtime evidence was supplied for this task; do not turn schema availability
into a claim that such an event was measured. No instrumentation was added.

## Exact remaining fact / handoff to ChatGPT

**UNKNOWN:** during the same observed reversal and active-source interval,
does displacement reverse while Hero Transform Forward remains entry-derived,
or does Transform Forward turn while only the visible body/animation retains
the previous facing? Was the existing override admitted/active at that event?
The visual report and static code alone do not decide these alternatives.
ChatGPT owns the next evidence/design judgment. This review stops here; it
requests no new capture/game run, patch, watch or movement design.

Runtime remains v0.1c38; C52
`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` and all existing proof/write
gates are unchanged. No source/tool/config/package/game/other-repository changes.
Relevant Wisdoms references were read only; cross-repository edits are forbidden
by this task, so any later reusable-finding synchronization remains Chat-side.
