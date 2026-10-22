# v0.1b7 runtime result — pre-armed reset capture

Source: user runtime log `WallSprint(20261022-190952).log` on the accepted Spider-Man2.exe 2.810.0.0 build.

## Result

v0.1b7 succeeded at the diagnostic objective. The primary native WallRun thread was pre-armed before source override and captured the reset path.

### Session 1 — A control

- N398 approximately +Z.
- Pre-arm succeeded: DR0/DR1 installed before the override.
- Existing `0x2CFC580` / caller return `0x2CFC8F3` successfully wrote the interpolated surface-relative source into the Hero Transform.
- A second writer then reset that orientation almost immediately:
  - writer function RVA `0x2CFC3C0`
  - runtime function range `0x2CFC3C0..0x2CFC57C`
  - size 444 bytes
  - captured runtime SHA256 `ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196`
  - caller return RVA `0x2CFC84B`
  - caller2 RVA `0x35184A5`
  - same primary native thread
  - reset latency observed around 0.85–0.90 ms after the WallSprint source commit.
- The function writes the watched Up row through three 32-bit stores. Captured function bytes show direct dword copies from RDX to RCX across the transform basis, including the Up row at `+0x10/+0x14/+0x18`.
- At the reset event, RCX was the current Hero Transform and RDX was a nearby thread-stack address. In the captured A event, post-prologue RDX was approximately RSP + 0x60.

A later second reset path also exists:

- writer `0x2CFC580`
- caller return `0x2CFC8F3`
- source Up = World Up
- `source_overridden_this_entry=0`
- observed about 14.9 ms after a surface-relative commit in the strongest A sample.

This means the existing common-writer source override does not currently cover every relevant `0x2CFC8F3` invocation.

### Session 2 — E diagonal wall

- N398 approximately `(-0.707107, 0, +0.707107)`.
- Pre-arm succeeded again.
- The same immediate writer/caller pair reproduced:
  - writer `0x2CFC3C0`
  - caller return `0x2CFC84B`
  - caller2 `0x35184A5`
  - same primary native thread.
- During a representative interpolated surface Up near `(-0.5, 0.7071, +0.5)`, the watched writes from `0x2CFC3C0` sequentially drove the Hero Up row toward World Up: X was cleared, Y was replaced by approximately 1, and Z was cleared.
- RDX again pointed into a nearby stack-local region while RCX was the current Hero Transform.

The A/E reproduction strongly establishes `0x2CFC3C0` / `0x2CFC84B` as the immediate orientation-reset source path for the tested WallRun state.

## Architectural interpretation

The current observed pipeline is approximately:

```text
WallSprint source override
  caller 0x2CFC8F3
  writer 0x2CFC580
        ↓
Hero gets Surface-relative Up
        ↓ ~0.8–1.0 ms
second source/copy path
  caller 0x2CFC84B
  writer 0x2CFC3C0
        ↓
Hero basis copied back toward World-relative orientation
        ↓ later
another 0x2CFC580 / 0x2CFC8F3 call may also provide World Up
```

Therefore do not globally NOP either writer. The next implementation should preserve the native pipeline and replace the caller-local orientation source matrices only for exact verified WallSprint calls targeting the current Hero Transform.

## Next gate

Proceed to v0.1b8 dual-source orientation ownership:

1. keep the working `0x2CFC580` source override;
2. cover every valid late World-Up `0x2CFC8F3` Hero call during WallSprint, not only the currently admitted subset;
3. authenticate and instrument `0x2CFC3C0` only for caller return `0x2CFC84B` + current Hero + verified WallRun;
4. verify its RDX source is readable/stack-local and modify orientation rows only;
5. feed the same per-session interpolated SurfaceFrame basis into both source paths;
6. keep Position, adhesion, movement, animation, camera and physics untouched;
7. verify retained Actor Up for >=0.30 s / 20 updates before moving to adhesion.
