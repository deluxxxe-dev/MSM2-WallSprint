# Next task — WallSprint v0.1c2 Real-Runtime Compact Proof Repair

## Objective

v0.1c1 implemented the intended lock-free dual-source orientation ownership path, but the real game never reached either source override because the new Path-B compact-3x3 startup proof failed closed.

The target executable itself is still the verified build. Do not change or weaken exact-build safety. Fix the proof implementation so it can correctly decode the real accepted-build Path-B caller/writer and recover the already-characterized nine compact copy pairs.

This remains orientation-only. Do not add adhesion, movement, animation, camera, physics, gravity, parkour, swing or combat.

## Implementation base — IMPORTANT

Do **not** start from main source alone.

The v0.1c1 implementation is published at:

```text
branch: codex/v01c1-compact-path-b
commit: 71437fac6c0640af01725a46e1b49c5627c739bb
```

Use that branch/commit as the implementation base, then read the latest handoff docs from `origin/main`:

- `AGENTS.md`
- `docs/PROJECT_STATE.md`
- `analysis/V01C1_RUNTIME_RESULT.md`
- this `docs/CODEX_NEXT.md`

Preserve all c1 lock-free snapshot, Path-A, Path-B write-boundary and retention logic unless runtime evidence here specifically requires a correction.

## Authoritative v0.1c1 runtime result

User report: no visible orientation change.

The package was correct:

```text
WallSprint v0.1c1 package_version=0.1.19
scope=shared_orientation_source_snapshot
path_b_override=1
```

The executable itself passed:

```text
Spider-Man2.exe 2.810.0.0
exe_file_sha256=c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
target_build_gate=ACCEPTED
```

But the new proof failed:

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

Then all source ownership was correctly disabled:

```text
native_source_config admitted=0 caller_body_hash_verified=0
DUAL_SOURCE_ADMISSION admitted=0 copy_hook_authenticated=0
PATH_B_LAYOUT_CONFIG enabled=0 path_b_override=0
RESET_TRACE_CONFIG admitted=0
PATH_A_OVERRIDE_COUNT=0
PATH_B_OVERRIDE_COUNT=0
```

The existing path-A writer itself still authenticated and installed:

```text
orientation_source_hook installed=1
writer_rva=0x2cfc580
exact_hash=3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6
unique_AOB=1
known_prefix=1
runtime_boundary=1
```

Therefore this is a c1 proof-decoder/self-check bug, not a changed game build and not evidence against the b9 compact source characterization.

Detailed runtime handoff: `analysis/V01C1_RUNTIME_RESULT.md`.

## Ground-truth Path-B map from b9

Writer:

```text
RVA 0x2CFC3C0 .. 0x2CFC57C
size 444
SHA256 ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196
```

Caller:

```text
function 0x2CFC7D0 .. 0x2CFC850
call return RVA 0x2CFC84B
```

Exact runtime source provenance on A and E:

```text
RDX = writer-entry RSP + 0x28
RDX inside current-thread stack
[RDX,RDX+0x24) inside stack
```

Nine direct-copy pairs to prove from authenticated original writer bytes:

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

Do not hard-code admission solely from this list. The startup proof must independently recover and verify it from the exact accepted-build original bytes.

## Required implementation

### 1. Diagnose why the real decoder stops at 5 bytes

Instrument the compact proof parser/decoder so failure reports at least:

```text
PATH_B_PROOF_DECODE_FAILURE
role=caller|writer
function_rva=
byte_offset=
absolute_rva=
remaining_bytes=
first_bytes_hex=
decoder_backend=
decoder_status=
reason=
```

The current `decoded_prefix_bytes=5` is insufficient.

Use the actual unhooked accepted-build bytes, not a synthetic fixture, for the startup proof.

### 2. Use a decoder that fully supports the real instruction stream

The proof must decode the complete instruction sequences needed to authenticate:

- caller construction of the local compact source and the direct call returning at `0x2CFC84B`;
- writer direct loads from RDX and stores to RCX for all nine orientation floats.

If the c1 custom/minimal decoder cannot reliably handle the real instruction set/prefixes, replace that proof path with an already-vendored/runtime-safe decoder that supports the encountered x64 instructions, or use the project's existing proven Capstone-based offline/static decoder where appropriate.

Do not simply skip undecodable instructions. A proof that silently skips bytes must not admit writes.

### 3. Separate build identity from proof identity in diagnostics

Do not report the verified executable as `exact_build=0` merely because a later proof component failed.

Log distinct fields, for example:

```text
exe_build_verified=1
path_a_writer_verified=1
path_b_writer_verified=1
path_b_caller_verified=0|1
path_b_map_verified=0|1
dual_source_admitted=0|1
```

Fail closed remains mandatory; this is a diagnostic clarity fix, not a safety relaxation.

### 4. Recover all nine pairs from real bytes

Successful startup must log:

```text
PATH_B_COMPACT3X3_PROOF
admitted=1
caller_verified=1
writer_verified=1
rdx_to_rcx_map_verified=1
source_bytes=0x24
decoded_prefix_bytes=<complete required span>
pairs=9
```

Additionally emit a bounded one-time map summary containing the nine recovered source/destination offsets. Do not log/package full private game-function captures.

### 5. Preserve the c1 lock-free shared snapshot

Once proof admission succeeds, retain c1 behavior:

- no worker-state mutex/try-lock dependency inside Path A or Path B native hot hooks;
- one coherent shared session/generation/Transform/Right/Up/Forward/N398 snapshot;
- exact lifecycle/thread/target/stack/staleness/kill gates;
- `PATH_A_STATE_LOCK_CONTENTION=0` should remain true.

Do not regress to the b9 state-lock design.

### 6. Preserve exact Path-B write boundary

For admitted exact calls after elapsed >= 0.70:

- RDX must equal entry RSP+0x28;
- source/target/current Hero/lifecycle must verify;
- validate sane finite row magnitudes;
- write exactly `[RDX+0x00,RDX+0x24)`;
- preserve all bytes at and after `RDX+0x24`;
- preserve row magnitudes while replacing directions with the shared WallSprint Right/Up/Forward basis;
- verify metadata remains unchanged.

Do not write Actor Transform directly.

### 7. Preserve Path-A every-call override

For exact qualifying `0x2CFC8F3 -> 0x2CFC580` current-Hero calls after elapsed >= 0.70:

- use the same lock-free shared orientation snapshot;
- modify only Right/Up/Forward source rows;
- preserve Position;
- override every qualifying invocation, including late World-Up source calls;
- bounded reject diagnostics must explain real gate failures.

### 8. Runtime verification

Keep the bounded prearmed Hero Up hardware watch.

If both source paths commit and another writer resets Up, log:

```text
NEW_RESET_AFTER_DUAL_OVERRIDE
writer_rva=
caller_rva=
latency_ms=
up_before=
up_after=
```

Do not compensate with direct Actor spam writes.

### 9. Retention gate

Require:

```text
dot(ActorUp,N398) >= 0.98
```

for either >=0.30 s or >=20 distinct native updates in the same active WallRun session.

Log:

```text
ORIENTATION_RETENTION_PASS=1
held_ms=
held_updates=
```

Only this gate can authorize the later adhesion milestone.

## Tests

Add a regression using the **actual accepted-build byte sequences already independently captured/represented by project analysis**, sufficient to reproduce the c1 runtime decode failure. The test must fail under the old c1 proof decoder and pass after the repair.

Also retain negative tests:

- byte mutation / hash mismatch;
- wrong caller return;
- missing one of nine pairs;
- duplicate/ambiguous pair;
- source/destination offset outside expected orientation region;
- decoder failure mid-stream;
- stack relation mismatch;
- metadata boundary write attempt.

Synthetic fixtures alone are not sufficient for this bug.

## Human test

After startup proof is confirmed admitted, test:

A Control:
- one left-to-right WallRun, about 1.5 s.

E diagonal:
- same known diagonal face;
- one left-to-right WallRun, about 2 s.

Expected visible success: Spider-Man clearly rotates sideways relative to world/screen because the camera remains world-up.

## PASS criteria

Implementation gate:

```text
PATH_B_COMPACT3X3_PROOF admitted=1
caller_verified=1
writer_verified=1
rdx_to_rcx_map_verified=1
pairs=9
```

and no state-lock dependency in the hot hooks.

Runtime primary gate:

```text
PATH_A_OVERRIDE_COUNT > 0
PATH_B_OVERRIDE_COUNT > 0
ORIENTATION_RETENTION_PASS=1
```

No adhesion yet even if runtime retention succeeds.

## Required outputs

Update/create:

```text
analysis/V01C1_RESULT.md
analysis/PATH_B_PROOF_DECODER_FIX.md
analysis/DUAL_SOURCE_RETENTION.md
analysis/V01C2_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c2_real_runtime_proof_fix.zip
```

Persist implementation on a new pushed branch based on c1, e.g. `codex/v01c2-proof-fix`, and record exact branch/commit SHA in `docs/CODEX_RESULT.md`.

## Distribution / provenance

Follow `AGENTS.md` and `docs/DEV_RULES.md`.

Use only WallSprint-owned source plus independent runtime/static analysis of Spider-Man2.exe. Do not copy reference-mod code, binaries or assets.
