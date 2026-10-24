# v0.1c25 actual-game runtime result

## Evidence scope

Owner-supplied private runtime log:

```text
MSM2-WS-v0.1c25-20261025-042121.log
```

This file records only compact derived evidence. The raw log and proprietary target bytes are not committed.

## Build admission

Accepted target remains:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

The exact-build gate accepted.

## c25 result — c24 cap ambiguity resolved

C25 fully validates the PE exception/runtime-function directory under the new explicit bounded budget:

```text
directory_rva=0xC566000
directory_bytes=6325932
entry_bytes=12
derived_entries=527161
RVA_size_pair_valid=1
directory_range_in_image=1
entry_size_divisible=1
metadata_byte_cap=8388608
entry_cap=600000
byte_cap_exceeded=0
entry_cap_exceeded=0
budget_admitted=1
entry_validation_skipped_by_cap=0
metadata_bytes_read=6325932
entries_validated=527161
guarded_read_failed=0
all_entries_read_and_validated=1
table_structurally_valid=1
runtime_lookup_queried=1
```

Therefore c24's `527161 > 262144` refusal was a diagnostic budget cap, not evidence of structural corruption.

## Exact target runtime-entry status

For the accepted direct target:

```text
target_rva=0x2CF7FE0
runtime_lookup_queried=1
runtime_lookup_found=0
runtime_lookup_same_image=1
runtime_lookup_readable=1
next_runtime_rva=0x2CF8020
PE_runtime_table_valid=1
PE_runtime_entries=527161
missing_runtime_entry=1
```

With the PE table fully structurally validated and the OS lookup actually queried, the exact target has no containing runtime-function entry under the authenticated current image. This permits the previously prepared strict single-target no-unwind fallback to run; it does not by itself establish a semantic function boundary.

## Strict no-unwind fallback result

The fallback begins at the exact pinned target and obtains a stable bounded snapshot:

```text
method=strict_single_target_no_unwind_linear
scan_bytes=64
snapshot_stable=1
instructions=1
admitted=0
admitted_bytes=0
barrier_rva=0x2CF7FE0
barrier_instruction=cmp
reason=target_leaf_instruction_effect_not_admitted
```

The first decoded instruction is `cmp`, but its exact effect form is not admitted by the current strict leaf effect model. Therefore no target span/hash is authenticated and the c23 call-effect sidecar does not consume a modeled nested state:

```text
target_begin_rva=0
target_end_rva=0
boundary_verified=0
snapshot_authenticated=0
snapshot_sha256=unavailable
modeled_states=0
effect_certificate=0
```

Do not infer the CMP operands, width, ABI meaning, branch outcome, or target semantic identity from the mnemonic alone.

## Selected-read flow state

The selected `Hero Transform+0x10C` read dependence still reaches the CALL boundary in raw GPR carrier index 0:

```text
site=0x2B235D4
gpr=0
gpr_tags=0x1
origin_kind=mem32
root=Hero Transform
offset=0x10C
```

The bounded selected-read flow now reports the nested target instruction itself as the exact next blocker:

```text
complete_local_flow=0
first_unresolved_rva=0x2CF7FE0
first_unresolved_instruction=cmp
returned_gpr_may_depend_mask=0x1
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
reason=target_leaf_instruction_effect_not_admitted
```

C22 TEST/JE disjointness and c23 direct-call resolution remain intact.

## Runtime safety

Two A/E-style WallRun sessions completed normally:

- session 1: ~3.86 s, verified native Deinit, retention pass, hard kills 0;
- session 2: ~4.03 s, verified native Deinit, retention pass, hard kills 0;
- MOD Position writes remained 0;
- adhesion remained read-only with source-position writes 0;
- `writable_source_proven=0` remains closed.

No movement/source/input/Position mutation is authorized by c25.

## Accepted conclusion

C25 resolves the c24 ambiguity as follows:

1. the 527161-entry PE runtime-function table is structurally valid under the bounded 8 MiB / 600000-entry budget;
2. the OS runtime lookup was actually queried and agrees that no containing runtime-function entry exists for `0x2CF7FE0`;
3. the strict pinned no-unwind fallback therefore legitimately starts;
4. the fallback stops immediately at `0x2CF7FE0: cmp` because that exact instruction effect is not admitted;
5. target span/hash/effect, downstream XYZ/W relevance, cross-invocation feedback, ownership and writability remain unresolved.

The next proof obligation is the smallest exact classification/model of the CMP instruction sufficient to decide whether the strict no-unwind walk may continue without weakening the existing fail-closed rule.