# V01C37 runtime result — offset proof confirmed; publication base still unresolved

Status: **ACCEPTED-RUNTIME for the bounded c37 diagnostic**.

Source log supplied by owner:

```text
MSM2-WS-v0.1c37-20261026-164009.log
```

Target identity remained exact:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## Test-condition note

The owner reported that the first attempt did not attach cleanly to the wall and briefly entered a vertical-walking / upward WallRun before the intended test.

The log supports that report. Early sessions were short and predominantly classified by the existing position-difference heuristic as `WallRunUp` / `Mixed`:

- session 1: ~0.096 s, Mixed;
- session 2: ~0.605 s, WallRunUp;
- session 3: ~0.161 s, WallRunUp;
- session 4: ~0.372 s, WallRunUp.

These early sessions are **not used as the intended stable A/E-style WallRun samples**.

Later sessions 5 and 6 are the usable stable sessions:

- session 5: ~3.77 s, `WallRunSide`, verified Deinit;
- session 6: ~4.32 s, `WallRunSide`, verified Deinit.

Both retain retention pass, no hard kill, MOD `Position_writes=0`, `source_position_writes=0`, and no adhesion correction writes.

## c37 address-component result

The c37 startup/current-code diagnostic independently confirms the new fallthrough offset proof before any gameplay session quality matters:

```text
predicate=unsigned_R8D_not_above_CMP_immediate
compare_bound=16383
R8D_min=0
R8D_max=16383
MOVSXD_proven=1
sign_min=0
sign_max=16383
SHL_proven=1
shift_count=6
offset_min=0
offset_max_inclusive=1048512      # 0xFFFC0
offset_stride=64
store_width=32
end_extent_exclusive=1048544      # 0xFFFE0
overflow_wrap_excluded=1
offset_proven=1
live_values_read=0
```

Interpretation: the JA fallthrough path provides a verified numeric range for the **relative R8 offset only**. This is separate from dependency tags and does not prove the unknown base.

## RAX publication/base remains unresolved

The exact qword source slot remains:

```text
slot_rva=0xC2D2580
slot_writable=1
```

Current runtime diagnostic reports:

```text
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
live_qword_reads=0
pointer_chase=0
reason=exact_slot_writer_provenance_not_enumerated_by_bounded_target_walk
```

Therefore the exact store remains uncontained:

```text
0x2CF7FFB: vmovups ymmword ptr [R8+RAX], ymm0
memory_escape=1
pending_unknown_write=1
store_destination_contained=0
control_alias_unexcluded=1
CFG_blocked=1
continuation_allowed=0
```

No next opcode is inferred.

## Decision

The initial vertical/upward WallRun attempts do **not** invalidate c37 because the new offset/publication diagnostics are startup/current-code proof results, not deductions from those motion samples. Stable sessions 5 and 6 also preserve the established lifecycle/safety behavior.

Do not repeat the same game capture. The next useful evidence is the owner-run offline exact-slot publication candidate scan against the exact on-disk `Spider-Man2.exe`, producing `MSM2-WS-v0.1c37-slot-publication.json` without observing the live qword value.