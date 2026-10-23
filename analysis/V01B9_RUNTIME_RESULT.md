# v0.1b9 runtime result — 2026-10-23

## Summary

User reported no visible change. That is expected for this package: startup explicitly reports `path_b_override=0` and `path_b_characterization_active=1`. v0.1b9 was diagnostic/read-only for path B.

Build gate passed for Spider-Man2.exe 2.810.0.0 / SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.

The important result is that path-B source provenance and compact orientation layout are now sufficiently constrained for a narrow write experiment.

## Path B provenance — reproduced on A and E

For exact caller `0x2CFC84B -> 0x2CFC3C0`, every captured entry in the relevant window reported:

```text
RDX = RSP + 0x28
rdx_inside_stack_bounds=1
source_span_inside_stack_bounds=1
expected_rsp_relation_match=1
source_readable=1
```

Representative A:

```text
RSP = 0x60062FD5C8
RDX = 0x60062FD5F0
rdx_minus_rsp = 40 (0x28)
```

Representative E used the same `RSP + 0x28` relation.

Therefore the v0.1b8 `source_is_stack_local=0` result was a misleading admission diagnostic, not evidence that the source was outside the thread stack.

## Static caller construction — compact 3x3 source

Authenticated original caller range:

```text
0x2CFC7D0 .. 0x2CFC850
return RVA 0x2CFC84B
```

The captured caller bytes show the caller constructing nine float values in a local stack buffer, then executing the equivalent of:

```text
lea rdx, [original_rsp - 0x38]
call 0x2CFC3C0
```

At the callee entry this becomes exactly `RDX = RSP + 0x28` because of the caller frame plus pushed return address.

The nine local floats are formed from three groups of three scalar products. They occupy:

```text
RDX + 0x00, +0x04, +0x08
RDX + 0x0C, +0x10, +0x14
RDX + 0x18, +0x1C, +0x20
```

This is a compact 3x3 orientation/basis representation, not path A's padded affine 4x4 layout.

## Static writer mapping

Authenticated writer:

```text
0x2CFC3C0 .. 0x2CFC57C
SHA256 ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196
```

The captured writer prefix contains direct 32-bit copies that map the compact source into the Hero transform orientation slots:

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

So the first 0x24 bytes of path B are the three orientation rows used to overwrite Hero Right/Up/Forward. Bytes after `RDX+0x20` are not part of this proven direct-copy orientation payload and must not be modified by the next experiment.

The v0.1b9 runtime logger did not emit `PATH_B_STORE`; `PATH_B_ENTRY_LINK` still said `static_store_proof=0 requires_anchored_decode=1`. The next implementation should formalize the mapping above with an anchored instruction decoder/self-check before enabling any write, rather than trusting handwritten offsets alone.

## Runtime source values confirm the compact basis interpretation

### A control wall

Representative first nine source floats correspond approximately to:

```text
row0 = (0, 0, -1)
row1 = (0, 1,  0)
row2 = (1, 0,  0)
```

while path A had already driven Hero Up to an interpolated Surface-relative value such as:

```text
Hero/path-A Up = (0, 0.981090546, 0.193549454)
```

The path-B source therefore still carried the world-relative orientation that resets the actor.

### E diagonal wall

Representative first nine source floats correspond approximately to:

```text
row0 = (+0.7071066, 0, -0.7071066)
row1 = (0, +0.9999991, 0)
row2 = (+0.7071064, -0, +0.7071064)
```

while path A had already driven Hero Up toward diagonal Surface Up, e.g.:

```text
Hero/path-A Up = (-0.136860132, 0.981090546, +0.136860147)
```

and later:

```text
Hero/path-A Up = (-0.5, 0.707106829, +0.5)
```

Again, path B retains a World-Up second row and overwrites the Surface-relative actor orientation.

This reproduces the same source semantics on both A and E.

## Reset remains immediate

On E, after path A writes a Surface-relative Up, `0x2CFC3C0 / 0x2CFC84B` begins clearing the X component and restoring Y toward World Up roughly 0.9 ms later in representative samples.

Retention therefore remains failed. No `ORIENTATION_RETENTION_PASS` was observed.

## Path A admission problem also exposed

v0.1b9 produced a very large number of:

```text
COMMON_OVERRIDE_REJECT reason=state_lock_contention
```

including during the A/E test windows. This means path-A admission still depends on a contended state lock in a hot native hook and can let valid World-Up calls pass without evaluation.

The next orientation-ownership experiment should remove this lock dependency from the hook path. Prefer a precomputed immutable/lock-free published WallSprint orientation snapshot that hooks can read without blocking or `try_lock` rejection.

## Conclusion

v0.1b9 achieved the useful diagnostic milestone:

- path-B source provenance is caller-local and bounded;
- the exact local relation is consistently `RDX = RSP + 0x28`;
- the source is a compact 3x3 basis payload in the first 0x24 bytes;
- the writer maps those nine floats directly into Hero Right/Up/Forward slots;
- A and E runtime data both show the path-B Up row remains World Up and causes the reset.

The next package may attempt a narrowly scoped path-B compact-3x3 override, but only after an anchored decode/self-check proves the direct-copy map at startup. It should also eliminate path-A `state_lock_contention` by using a lock-free published orientation snapshot.

Do not add adhesion, movement, animation, camera or physics yet.