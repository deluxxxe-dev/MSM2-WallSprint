# v0.1c23 actual-game runtime result

## Evidence

Owner-supplied private runtime log:

```text
MSM2-WS-v0.1c23-20261025-025936.log
```

Target admission remained the expected build:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

This file records only compact derived evidence. The private raw log is not committed.

## Accepted c23 result

The c22 selected-read walk blocker at `0x2B235D4` is an exact direct call:

```text
site_rva=0x2B235D4
form=direct_call
target_rva=0x2CF7FE0
exact_target=1
```

The selected `Transform+0x10C` read dependence is still present at the call boundary in raw GPR carrier index 0:

```text
gpr=0
gpr_tags=0x1
origin_kind=mem32
root=Hero Transform
offset=0x10C
```

Other observed raw GPR carriers include Hero Transform / image-global / caller-stack origins, but no semantic ABI/type names are assigned from those raw coordinates.

The exact new proof boundary is target identity, not call-target decoding:

```text
target_begin_rva=0
target_end_rva=0
boundary_verified=0
snapshot_authenticated=0
snapshot_sha256=unavailable
modeled_states=0
reason=read_call_target_boundary_hash_unavailable
```

Therefore the nested target was not expanded and no call effect certificate is admitted.

Current selected-read outputs remain:

```text
complete_local_flow=0
first_unresolved_rva=0x2B235D4
first_unresolved_instruction=call
returned_gpr_may_depend_mask=0x1
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
effect_certificate=0
writable_source_proven=0
```

The c22 `TEST r8b,r8b` / `JE` result remains disjoint from the selected read and unchanged.

## Runtime safety / lifecycle

Two A/E-style WallRun sessions completed normally.

Session 1:

```text
wallrun_duration_ms=4078
source_exit_reason=verified_native_Deinit
verified_deinit=1
retention_pass=1
Position_writes=0
first_hard_kill=none
hard_kill_calls=0
```

Session 2:

```text
wallrun_duration_ms=4156
source_exit_reason=verified_native_Deinit
verified_deinit=1
retention_pass=1
Position_writes=0
first_hard_kill=none
hard_kill_calls=0
```

No movement/source/input mutation is authorized by this result.

## Accepted interpretation

C23 proves the direct target RVA and proves that the selected same-field read reaches the call boundary in at least one raw carrier. It does **not** prove what the target does with that carrier.

The next task is to classify why current target boundary/hash authentication failed for `0x2CF7FE0` and, only if a strict bounded proof is possible, authenticate one no-unwind/leaf target span and resume the existing register-only effect analysis. Otherwise stop at the exact identity/boundary reason.

Producer `0x351EB00` remains deferred. Position/source/input/movement writes remain forbidden.