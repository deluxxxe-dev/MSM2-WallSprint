# WallSprint v0.1c14 actual-game runtime result

## Verdict
**Diagnostic PASS / exact static blocker identified. Movement/source writes remain unauthorized.**

Target build accepted: Spider-Man2.exe 2.810.0.0, SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`, v0.1c14/package0.1.32.

Session1/A: 4453 ms, verified Deinit, retention pass, final Up dot surface=1, stale0, hard kill0, Position writes0.
Session2/E: 4172 ms, verified Deinit, retention pass, final Up dot surface=1.00000012, stale0, hard kill0, Position writes0.
Entry hook and Position-watch cleanup restored normally.

C14 repairs the c13 final summary: both sessions end with `source_capture_ready=1`, `true_entry_snapshot=1`, `true_entry_snapshots=8`, `complete_invocation_maps=8`, `writable_source_proven=0`, `Position_writes_by_mod=0`.

Actual runtime again reproduces `RDX == entry RSP+0x28`, pointer candidate20 / stack candidate23 XYZ exact store-bit match (`mask=0x7`), sourceW0/destinationW1.

The new independent static contract stops before proving causality:
```text
writer_complete_decode=1
entry_store_prefix=0
xyz_RDX_0_4_8_paths=0
W_kind=unknown
W_classified=0
first_barrier_rva=0x2CFC91F
first_barrier_instruction=jne
exact_next_producer_rva=0x351EB00
writable_source_proven=0
reason=exact_position_store_sites_mismatch_or_prefix_incomplete
```

Per invocation `entry_store_xyz_match_mask=0x7` remains observed, but `static_prefix_proven=0` and `xyz_causality_proven=0`. W source classification is likewise blocked before exact store `0x2CFC945`.

Storage is narrowed to a 16-byte TEB-bounded current-invocation stack object with `RDX == entry RSP+0x28 == precall RSP+0x20`, but caller ownership/exclusivity remains unproven. The parent around observed second return `0x351A020` has anchored call `0x351A01B` with direct target/exact next edge `0x351EB00`; construction semantics and lifetime remain unknown.

`MOVEMENT_ENTRY_CAPTURE_SUMMARY` is still emitted too early and reports zeros before capture activation; final `MOVEMENT_LINEAGE_SUMMARY` and actual entry records are authoritative. Re-emit/fix this summary at diagnostic finalization.

Gate remains:
```text
writable_source_proven=0
Position/source/input/movement writes=0
```

Exact next task: bounded branch-aware proof inside authenticated writer `0x2CFC900..0x2CFC9B7`, beginning with conditional barrier `0x2CFC91F`, to prove/reject the three RDX XYZ paths to exact stores `0x2CFC936/93C/942` and classify W at `0x2CFC945`. Do not chase or mutate `0x351EB00` until the writer-side causal contract is proven.
