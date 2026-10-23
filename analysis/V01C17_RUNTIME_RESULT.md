# WallSprint v0.1c17 actual-game runtime result

## Verdict
**Diagnostic PASS / narrower exact callee CFG blocker.** No movement/source mutation is authorized.

## Build and logging
- v0.1c17 / package 0.1.35
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- exact build admitted
- dedicated logging actual-game verified at game-root `log_wallsprint/`
- real filename matched `WallSprint_YYYYMMDD-HHMMSS.log`
- `shared_logs_fallback=0`

## A/E regression
A/session1: 3875 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0.

E/session2: 4390 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0.

Both sessions finalized 8 true entry snapshots / 8 complete maps with XYZ candidate20 matching all8 (`mask=0x7`), source W0 and destination W1.

## Call argument proof
At writer call `0x2CFC927`:
```text
arg_rcx_origin=entry_writer_RCX_transform
arg_rdx_origin=entry_writer_RCX_transform
arg_r8_origin=constant_bits_NOT_pointer
arg_r9_origin=unknown_argument
source_span_alias_mask=0x0
transform_alias_mask=0x3
proof_complete=1
```

## Exact blocker
Authenticated callee remains `0x2B23560..0x2B235E9`, hash `2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d`.

C17 stopped before classifying any memory write:
```text
memory_writes_seen=0
source_span_no_write_proven=0
transform_no_write_proven=0
all_relevant_write_aliases_resolved=0
next_barrier_rva=0x2B235C1
next_barrier_instruction=je
next_barrier_effect=indirect_or_branch_cap
reason=callee_indirect_transfer_or_branch_cap_refused
```
This does not prove the callee has no writes; the bounded local CFG stopped first.

Writer causality stays blocked at call `0x2CFC927`; `xyz_causality_proven=0`, W remains unclassified, and `writable_source_proven=0`.

## Safety gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```
Producer `0x351EB00` remains deferred.

## Next
C18 should resolve only the authenticated callee's local CFG beginning at `0x2B235C1: je`, deriving a small exact-function branch bound from the decoded 137-byte callee and then reusing the c17 write-address provenance classifier.