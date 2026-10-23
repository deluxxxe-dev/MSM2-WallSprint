# WallSprint v0.1c1 runtime result

User report: no visible orientation change.

Authoritative runtime log: `WallSprint(20261023-032618).log`.

## Build identity

The target executable itself passed the normal build gate:

```text
WallSprint v0.1c1 package_version=0.1.19
Spider-Man2.exe 2.810.0.0
exe_file_sha256=c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
target_build_gate=ACCEPTED
```

Do not interpret the later `RESET_TRACE_SELF_CHECK exact_build=0` as a different executable. That field is part of the composite c1 admission/self-check after the new proof failed.

## Actual failure

The c1 compact Path-B startup proof failed closed before either orientation source path was allowed to write:

```text
PATH_B_COMPACT3X3_PROOF
admitted=0
caller_verified=0
writer_verified=1
rdx_to_rcx_map_verified=0
source_bytes=0x24
decoded_prefix_bytes=5
pairs=0
reason=instruction_or_dataflow_not_proven
```

Consequent global admission:

```text
native_source_config ... admitted=0 caller_body_hash_verified=0
DUAL_SOURCE_ADMISSION admitted=0 copy_hook_authenticated=0
PATH_B_LAYOUT_CONFIG enabled=0 path_b_override=0
RESET_TRACE_CONFIG admitted=0
```

Counters remained:

```text
PATH_A_OVERRIDE_COUNT=0
PATH_A_REJECT_COUNT=0
PATH_A_STATE_LOCK_CONTENTION=0
PATH_B_OVERRIDE_COUNT=0
PATH_B_REJECT_COUNT=0
```

Therefore the user-visible no-change result is expected: c1 never reached either path-A or path-B source modification.

## Important distinction

The existing common orientation writer itself still authenticated and installed:

```text
orientation_source_hook installed=1
writer_rva=0x2cfc580
exact_hash=3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6
unique_AOB=1
known_prefix=1
runtime_boundary=1
```

The blocker is specifically the new c1 Path-B compact-3x3 static proof implementation, not a changed game build and not evidence that the b9 compact 3x3 runtime characterization was wrong.

The strongest symptom is `decoded_prefix_bytes=5`: the proof decoder stopped almost immediately on the real accepted-build instruction stream and never discovered any of the nine RDX-to-RCX copy pairs (`pairs=0`).

## Implementation provenance

c1 implementation is published on:

```text
branch: codex/v01c1-compact-path-b
commit: 71437fac6c0640af01725a46e1b49c5627c739bb
```

`docs/CODEX_RESULT.md` on that branch reports synthetic/offline tests passing, but the real game runtime proof above overrides those fixtures for the runtime gate.

## Next requirement

Do not weaken or bypass the proof gate.

Repair the proof so it can decode the full authenticated real-game Path-B caller/writer bytes and independently recover the nine compact 3x3 direct-copy pairs. Add diagnostics for first decode failure offset/opcode/reason so a future failure is actionable.

The next implementation task must start from the c1 implementation branch/commit above, not from main source alone.

No adhesion, movement, camera, animation, physics, parkour, swing or combat work is authorized.
