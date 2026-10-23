# WallSprint v0.1c8 real-game runtime result

## Test

User tested `WallSprint v0.1c8` / package `0.1.26` on the same large A-direction wall with one continuous WallRun. Private runtime log and video were returned; raw user files are not committed.

Target build was accepted:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

Implementation under test:

```text
branch: codex/v01c8-kill-cause-attribution
implementation commit: 98c1d45108c89262750616c7258414cf762cac55
published head: 75af734d4d32d5e3a77087cbee6b2247b9617ced
version: v0.1c8 / package 0.1.26
```

## Result

c8 succeeded diagnostically but failed lifecycle-wide orientation hold.

The actor rotated to the intended Surface Up and retention passed:

```text
ORIENTATION_RETENTION_PASS=1
held_ms=312
held_updates=20
```

The orientation remained stable through about 1.7 s of WallRun, with Path A and Path B both repeatedly applying Surface Up.

At native elapsed `1.750000`, a Path B hook entry arrived on thread `35948` while the verified WallRun/native owner thread was `10680`. c8 treated that mismatch as a fatal invariant and hard-closed the whole source session:

```text
NATIVE_SOURCE_HARD_KILL
session=1
elapsed=1.750000
reason=path_b_wrong_thread
origin=path_b
thread_id=35948
native_thread=10680
expected_thread=10680
wallrun_active=1
wallrun_session=1
```

The detailed c8 attribution showed that all other source/integrity gates were still healthy:

```text
input_auth=1
copy_auth=1
compact_proof=1
caller_read_ok=1
caller_patch_match=1
source_failed_gate=none
observation_failed_hook=none
observation_failure_mask=0x0
all four WallRun hook patches read/match
```

The mismatching-thread Path B entry also retained strong Path B provenance:

```text
RCX == current Hero Transform
entry_return RVA == 0x2CFC84B
entry_source - entry_rsp == 0x28
native lifecycle token == coherent_active
same session/owner/generation/accepted epoch
```

The source then exited:

```text
NATIVE_SOURCE_EXIT
reason=path_b_wrong_thread
duration_since_source_entry=1.047000
applied_writes=128
held_updates=49
held_ms=797
stability_observed=1
```

Immediately afterward the independently verified WallRun continued normally. Final WallRun lifecycle was:

```text
Init=1
Entry=1
Update=279
Deinit=1
duration=4.405647 s
end_reason=verified_deinit
```

Therefore `path_b_wrong_thread` was a false session-fatal assumption, not a real WallRun end, hook-patch corruption, caller corruption, Hero/Transform change, or stale-update failure.

No `NATIVE_SOURCE_STALE_*` event occurred in this run.

Position/adhesion remained read-only:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
```

## Important observation

All ordinary logged Path B entries before the failure used thread `10680`. The first observed secondary-thread entry was thread `35948` at 1.750 s. The bounded multithread Up watch had already ended at native elapsed 1.500 s, so it could not classify this later write.

This real runtime disproves the current invariant:

```text
Path B thread must always equal the verified WallRun/native update thread.
```

At least one Path B call can target the active Hero Transform through the exact verified caller/stack relation on another process thread.

## Next target

Do not remove thread/provenance safety globally.

For c9, introduce a narrowly admitted secondary-thread Path B path. A secondary-thread Path B call may apply the already-published orientation basis only when all non-thread provenance remains exact, including:

- active same verified WallRun session and owner;
- same Hero and Transform;
- same generation / accepted epoch / collision epoch;
- coherent fresh native lifecycle token;
- exact verified Path B caller return `0x2CFC84B`;
- current-thread stack bounds and `RDX = RSP + 0x28`;
- compact 3x3 proof and readable source span;
- all source code / WallRun patch integrity gates healthy;
- finite/valid compact basis layout.

The secondary call must not replace or migrate the expected native WallRun thread identity. It is a per-call Path B store admission only.

Emit bounded evidence for secondary-thread calls and keep wrong caller/stack/target/integrity/identity conditions fail-closed or rejected according to existing semantics.

Primary c9 goal: the real WallRun must retain Surface Up through the same type of secondary Path B call and close only on verified Deinit.
