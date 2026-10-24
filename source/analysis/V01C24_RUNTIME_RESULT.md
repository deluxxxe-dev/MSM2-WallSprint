# v0.1c24 actual-game runtime result

Source: owner-supplied `MSM2-WS-v0.1c24-20261025-033837.log`.

## Target admission

Authenticated target build remained:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
target_build_gate=ACCEPTED
```

## Exact c24 result

The previously accepted direct call remains:

```text
0x2B235D4 -> 0x2CF7FE0
```

C24's target-identity diagnostic reached the PE/runtime-function metadata gate and refused before any code span was admitted:

```text
target_rva=0x2CF7FE0
admitted=0
method=none
code_region_begin_rva=0x1000
code_region_end_rva=0x52A41EC
runtime_lookup_found=0
runtime_lookup_same_image=1
runtime_lookup_readable=1
PE_runtime_table_valid=0
PE_runtime_entries=527161
scan_bytes=0
admitted_bytes=0
instructions=0
snapshot_stable=0
reason=target_runtime_table_malformed_or_cap
```

The c24 design cap is 262144 PE runtime-function entries. Therefore this result does **not** prove that the exception/runtime table is malformed, and it does **not** prove that `0x2CF7FE0` is a no-unwind leaf. The current evidence only proves that full table validation did not complete under the configured cap.

The selected same-field read still reaches the call boundary in raw GPR carrier index 0:

```text
gpr=0
gpr_tags=0x1
origin_kind=mem32
root=Hero Transform
offset=0x10C
```

No call-body effect was modeled:

```text
boundary_verified=0
snapshot_authenticated=0
snapshot_sha256=unavailable
modeled_states=0
effect_certificate=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
writable_source_proven=0
```

## Runtime safety / lifecycle

No MOD Position writes were enabled (`Position_writes=0`).

Session 1 exceeded the bounded diagnostic test window and recorded one intentional `bounded_test_window_5seconds` hard-kill before verified Deinit. It still closed through the verified native Deinit path. Session 2 ran approximately 4.16 s and exited through `verified_native_Deinit`; retention passed and MOD Position writes remained zero. The hard-kill counter remained process-cumulative in the session-2 summary.

This is not movement-source authorization and does not relax any existing ownership/lifetime/write gate.

## Accepted conclusion

C24 is accepted as **Outcome C at the metadata-cap boundary**:

- target executable code region admission reached;
- OS runtime lookup reports no containing entry for the exact target under the diagnostic;
- PE runtime table validation did not complete because the table reports 527161 entries while the current design cap is 262144;
- malformed-vs-cap-exceeded is not yet separated;
- no no-unwind leaf span/hash is admitted;
- selected-read call effect remains unknown.

## Next proof obligation

Before attempting the strict no-unwind leaf rule, distinguish a structurally malformed exception directory from a structurally valid table that merely exceeds the current arbitrary entry cap. If the exact-build PE exception directory is structurally valid and can be checked under a justified bounded metadata budget, complete that validation and only then decide whether the target truly has no containing runtime-function entry.

Producer `0x351EB00` remains deferred. Position/source/input/movement writes remain forbidden.
