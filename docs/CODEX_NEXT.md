# Next evidence gate — obtain exact-slot publication candidates after accepted c37

## Status

c37 implementation/static proof is complete and the owner-supplied actual-game log is accepted for diagnostic confirmation:

```text
MSM2-WS-v0.1c37-20261026-164009.log
```

See:

- `analysis/V01C37_ADDRESS_COMPONENTS.md`
- `analysis/V01C37_PUBLICATION_STATUS.json`
- `analysis/V01C37_RUNTIME_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

## Accepted c37 result

The JA fallthrough path now has a sound numeric range fact, separate from dependency tags:

```text
R8D fallthrough = 0..0x3FFF
MOVSXD -> 0..0x3FFF
SHL 6 -> relative offsets 0..0xFFFC0 inclusive
stride = 64
32-byte store end extent exclusive = 0xFFFE0
overflow/wrap excluded = 1
offset_proven = 1
```

The c37 actual-game startup diagnostic confirms the same proof with no live numeric value reads.

The owner reported an initial failed wall attachment. Runtime sessions 1-4 are short `Mixed`/`WallRunUp` attempts; sessions 5-6 are stable `WallRunSide` sessions. This does not affect the c37 address result because OFFSET_RANGE/SLOT_PUBLICATION are startup/current-code diagnostics rather than deductions from those motion samples.

The base/publication half remains unresolved:

```text
slot_rva=0xC2D2580
slot_writable=1
reference_scan_performed=0
writer_enumeration_complete=0
publication_source=opaque_loaded_qword_not_pointer
source_known=0
allocation_proven=0
range_proven=0
lifetime_proven=0
critical_disjointness_proven=0
base_proven=0
offset_proven=1
destination_contained=0
CFG_continuation=0
```

Therefore retain:

```text
memory_escape=1
pending_unknown_write=1
store_destination_contained=0
control_alias_unexcluded=1
CFG_blocked=1
continuation_allowed=0
```

No next opcode may be manufactured.

## Current required action — owner-run offline scan, not another game capture

The next useful artifact is the bounded offline exact-slot candidate report produced locally from the exact on-disk `Spider-Man2.exe` using the already-shipped c37 tool:

```text
source/tools/slot_publication.py
```

Expected output:

```text
logs/MSM2-WS-v0.1c37-slot-publication.json
```

The exact supported PowerShell command is documented in `source/BUILD.md` under `c37 address proof and offline exact-slot candidates`.

Do **not** request another A/E/WallRun capture for this gate.

## If Codex is invoked before the JSON exists

Do not modify implementation merely to create progress. Confirm the accepted c37 state and report that the required next evidence is the owner-run exact-slot JSON.

Do not scan or upload a private EXE through GitHub. Do not infer writer completeness from zero/one/multiple direct candidates.

## After the JSON is supplied

The next bounded analysis task is to inspect its authenticated exact-slot candidate metadata and decide whether a narrower publication/writer chain can be proved without live qword observation.

Required distinctions:

- direct candidate references vs complete writer enumeration;
- decoded write candidate vs proven publisher;
- publication source vs allocation base;
- allocation base vs valid extent/lifetime;
- proven base vs merely numeric address;
- base proof vs already-proven R8 offset range.

A direct candidate by itself does **not** prove source kind, allocation, range, lifetime, ownership, or critical non-overlap.

If candidate metadata permits a bounded authenticated writer/publication chain, follow only that exact chain and stop at the first unproved indirect/call/source/lifetime boundary. Otherwise record the precise missing proof and remain fail-closed.

Do not traverse the separately deferred movement producer `0x351EB00`.

## Safety gates remain closed

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Still forbidden:

- MOD Position/source-position/input/movement writes;
- gameplay/adhesion/camera/animation/physics changes;
- live qword/pointer logging or pointer chasing;
- guessed ABI/type/pool/array semantics;
- generic arbitrary memory/store emulation;
- weakening c36 control-alias barrier or all-path requirements.