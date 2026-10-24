# WallSprint v0.1c19 actual-game result

Verdict: **Diagnostic PASS / exact value-provenance blocker identified.**

Actual log: `WallSprint_20261024-111325.log` (private user log; do not commit raw log).

## Build / logging
- WallSprint v0.1c19 / package 0.1.37
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- target gate accepted
- dedicated `log_wallsprint/WallSprint_YYYYMMDD-HHMMSS.log`
- shared fallback disabled

## A/E regression
```text
A/session1: 3844 ms, verified Deinit, retention PASS, Up dot surface=1, stale0, hard0, Position writes0
E/session2: 4094 ms, verified Deinit, retention PASS, Up dot surface=1, stale0, hard0, Position writes0
```
Both sessions finalized 8 true entry snapshots and 8 complete invocation maps. XYZ match mask remained `0x7`; source W remained 0 and destination W remained 1. Cleanup restored normally.

## c19 result
The former c18 possible alias is now exact:
```text
site=0x2B2358E
instruction=mov
width=4
destination=[RCX+0x10C]
dst provenance=entry writer RCX / Hero Transform
address class=hero_transform_exact
may_alias_source=0
may_alias_transform=1
write_overlaps_position_range=0
write_overlaps_transform_identity_or_pointer_carrier=0
```
Thus the destination itself is no longer ambiguous and does not directly overlap Position `+0x30..+0x3F`.

The new exact blocker is the **value written by this MOV**:
```text
value source register=EDI
value_source_kind=unknown
value_source_provenance_exact=0
all_reaching_states_same_value_origin=0
all_reaching_states_examined=0
write_affects_writer_reload_provenance=unknown
write_effect_relevant_to_xyz=unknown
write_effect_relevant_to_w=unknown
reason=exact_write_value_unknown_computed_or_reaching_origins_conflict
```
Therefore c19 must not model the Transform+0x10C write as irrelevant yet. Writer XYZ/W causality remains unproven even though runtime XYZ bit correlation remains `0x7`.

## Gate
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

## Exact next edge
Trace only the provenance of `EDI` reaching `0x2B2358E`, including all local reaching paths, and determine whether the written value can influence later Position/source-carrier reads. Do not traverse producer `0x351EB00` and do not implement movement.