# V01C62 owner offline result — accepted VERIFIED-STATIC

2026-10-28 JST. Owner supplied `MSM2-WS-v0.1c62-post-c59-stitched.json` produced from the pinned private `Spider-Man2.exe` with the published c62 offline scanner.

## Classification

**VERIFIED-STATIC** / `POST_C59_RUNTIME_BEGIN_STITCH_SCANNER_READY`.

Authenticated offline/static evidence only. Not runtime/gameplay evidence. No writer/publication/ownership/lifetime/CFG/EH/watch/pointer/qword/production permission follows.

## Artifact identity

```text
file_sha256 = b8b784d484e7b758588dbc446e8d90ab18eda1e638120369b62072129fb96060
file_bytes = 9374
tool_version = v0.1c62
schema_version = 1
contract = exact_c61_terminal_phase_origin_full_prefix_all_successor_continuation_v1
image_sha256 = c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
accepted_c61_classifier_identity = a828ec479e0f3279ddaf117d76ff827652155c357a8415606cbe2924e479d349
accepted_c61_record_set_identity = f5bc25085c62e7f9a32eab74ade7af97e076c9b03c99f796cc06d179ad5f724e
accepted_c61_result_identity = 7982be4b9e602fd4c01d84d2e1c9e8c169ed330508bd31ae79206c72ad969341
```

The accepted c56→c61 identities match the previously accepted chain and evidence is marked read-only.

## Exact owner counts

```text
root_transactions_attempted = 162472
root_transactions_completed = 103933
root_transactions_refused = 58539

c61_post_boundary_instances_available = 6566
c61_boundary_instances_considered = 6566
c61_boundary_instances_admitted = 3204
c61_boundary_instances_refused = 3362

c61_post_architectural_edges_available = 7206
c61_post_exact_edges_available = 6946
c61_exact_edges_considered = 6946
c61_exact_edges_admitted = 3765
c61_exact_edges_refused = 3181

continuation_instances_created = 8798
continuation_instances_deduplicated = 51
continuation_instances_refused = 6590
rows_reached_by_continuation = 1317
address_facts_crossing_runtime_rows = 2635
c61_certified_crossings_with_facts = 13
c61_address_facts_crossing_rows = 16
call_edges_with_preserved_facts_crossing_rows = 0
cross_row_cap_refusals = 44
cross_row_cycles_seen = 0

post_c61_uncertified_boundary_refusals = 2923
new_cross_row_candidate_count = 0
reference_candidate_count = 0
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
```

C61 phase admission remains phase-specific:

```text
considered:
  same_instance_after_c59_preflight_before_row_fixpoint = 5759
  post_c59_reachable_context = 805
  post_c59_reachable_dedup_stored_earlier_path = 2
admitted:
  same_instance_after_c59_preflight_before_row_fixpoint = 2800
  post_c59_reachable_context = 403
  post_c59_reachable_dedup_stored_earlier_path = 1
refused:
  same_instance_after_c59_preflight_before_row_fixpoint = 2959
  post_c59_reachable_context = 402
  post_c59_reachable_dedup_stored_earlier_path = 1
```

All-successor preflight refusal inputs remained the accepted c61 classes:

```text
inside_unique_runtime_span_nonentry = 245
executable_file_backed_outside_runtime_metadata = 15
```

C61 certificate mismatch count:

```text
context_phase_origin_or_full_prefix_mismatch = 15
```

Root refusal reasons after c62:

```text
inside_unique_runtime_span_nonentry = 31696
executable_file_backed_outside_runtime_metadata = 18700
uncertified_post_c61_runtime_boundary = 2923
indirect_call = 4884
indirect_jump = 253
summary_cap_refused = 35
system_or_unsupported_transfer = 31
instruction_cap = 9
decode_gap = 5
uncertified_later_runtime_boundary = 3
```

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false. `candidate_sweep_complete=false`, `writer_enumeration_complete=false`, C52 production review FAIL remains binding.

## Interpretation

1. C62 successfully converts 3204 of the 6566 c61-classified boundary instances into exact, all-successor-safe continuation for the exact authenticated context. This is 48.80% of the c61 boundary-instance domain; it is not a global runtime-row permission.
2. 3765 of 6946 exact c61 edges are actually usable inside completed c62 root transactions. Exact-edge availability remains distinct from safe instance/root admission.
3. Compared with accepted c60, completed roots increase 100729→103933 (+3204), reached continuation rows 794→1317 (+523), and address facts crossing runtime rows 2062→2635 (+573).
4. The next bounded unknown wave shrinks 6566→2923 terminal roots (−3643, 55.48%). This is the natural c63 diagnostic domain.
5. The three c61 terminal phases remain materially distinct. The c62 result demonstrates phase-specific admission/refusal and explicitly reports 15 full-prefix/context/phase/origin mismatches; no certificate may be borrowed across those differences.
6. Zero candidates remains an incomplete observation, not proof of no writer. Indirect control, EH/alternate entries, executable gaps, unsupported semantics, deferred producer, and publication/ownership/completeness domains remain open.

## Next bounded decision

Proceed to c63 as a separate OFFLINE/STATIC diagnostic classifier for the exact first terminal `uncertified_post_c61_runtime_boundary` of each accepted c62 refused root. Reconstruct/authenticate the complete accepted c56→c62 chain; classify only the outgoing architectural successors of those exact 2923 terminal contexts. Admit no new edge, propagate no new fact, emit no candidate, and preserve c62 output byte-for-byte/identity-wise. Keep root/row/state/path/full-prefix/c61 phase+origin and c62 context identity exact; do not globalize records or explore beyond the terminal refusal.
