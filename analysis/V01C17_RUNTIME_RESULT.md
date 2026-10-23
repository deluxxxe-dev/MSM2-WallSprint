# WallSprint v0.1c17 actual-game runtime result

## Verdict
**Diagnostic PASS / narrower exact callee CFG blocker.** No movement/source mutation is authorized.

## Build and logging
- v0.1c17 / package 0.1.35
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- exact build admitted
- dedicated logging verified at game-root `log_wallsprint/`
- actual filename matched `WallSprint_YYYYMMDD-HHMMSS.log`
- `shared_logs_fallback=0`

## A/E regression
A/session1:
- duration 3875 ms
- verified Deinit=1
- retention pass=1
- final Up dot surface=1
- stale suspend/resume=0
- hard kill=0
- Position writes by mod=0

E/session2:
- duration 4390 ms
- verified Deinit=1
- retention pass=1
- final Up dot surface=1
- stale suspend/resume=0
- hard kill=0
- Position writes by mod=0

Both sessions finalized 8 true entry snapshots and 8 complete invocation maps; XYZ source candidate 20 matched all 8 with mask 0x7. Source W remained observed 0 and destination W observed 1.

## c17 call argument contract
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
This is useful progress: call-time RCX/RDX are Transform-derived, while the authenticated entry-RDX 16-byte source span is not one of those two argument aliases.

## Exact c17 blocker
Authenticated callee remains:
```text
0x2B23560..0x2B235E9
hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
```

C17 stopped before classifying any callee memory write because its bounded CFG refused at:
```text
next_barrier_rva=0x2B235C1
next_barrier_instruction=je
next_barrier_effect=indirect_or_branch_cap
reason=callee_indirect_transfer_or_branch_cap_refused
```

Observed c17 alias summary:
```text
memory_writes_seen=0
source_span_no_write_proven=0
transform_no_write_proven=0
all_relevant_write_aliases_resolved=0
```
Therefore this is not evidence that the callee performs no writes. It means the analysis stopped before reaching/finishing relevant write sites.

Writer causal proof remains blocked at call `0x2CFC927`; `xyz_causality_proven=0`, W remains unclassified, and `writable_source_proven=0`.

## Safety gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```
Do not inspect/mutate producer `0x351EB00` yet.

## Next smallest task
C18 should handle only the authenticated callee's local branch structure beginning at `0x2B235C1: je`, under strict bounds, so the existing address-provenance engine can reach and classify actual memory write sites. Do not broaden into general interprocedural tracing or movement implementation.