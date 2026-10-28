# V01C66 OWNER OFFLINE RESULT

Date: 2026-10-28 JST
Branch: `codex/v01c20-edi-value-provenance`
Evidence class: **VERIFIED-STATIC** owner OFFLINE result.

## Artifact

Owner result file:

`MSM2-WS-v0.1c66-post-c63-stitched.json`

Physical file SHA256:

`eb4e44fddb67dafd64700e19e846173e60cbaddce95860686e24e543e650cc1c`

Size: 12602 bytes.

Repository result:

`POST_C63_RUNTIME_BEGIN_STITCH_SCANNER_READY`

Tool/schema/contract:

- tool `v0.1c66`
- schema `1`
- contract `exact_c65_full_prefix_terminal_reason_provenance_all_successor_continuation_v1`

Exact image authentication passed for:

`c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`

The result states `c56_through_c65_evidence_read_only=true`.

## Accepted chain authentication

C66 independently reconstructed the accepted c56→c65 chain before continuing. The accepted c65 identities in the c66 result match the owner c65 artifact:

- c65 classifier identity: `e213e4e137efb57f36a3a716280ed9c83f9641e599fed26daeb2bedcc758bb99`
- c65 authenticated record-set SHA256: `dd2964a012eff48f01dfd2f3d49b6acb70e7e40ba44cf2cda3534f1f9e2eff22`
- c65 result identity: `dd4ff3451faa9735f88756e16222a36fc80cafbf51f113bc12f6f06b6dc62780`

The c65 result identity was independently recomputed as SHA256 of the canonical sorted compact JSON encoding and matches c66 exactly.

All earlier accepted c57-c64 identities in the c66 authentication header match the previously accepted chain. No c65 certificate mismatches were reported.

## C66 continuation result

Original-root domain remained unchanged:

```text
root_transactions_attempted = 162472
root_transactions_completed = 105541
root_transactions_refused = 56931
```

Compared with accepted c64:

```text
completed roots: 105083 -> 105541  (+458)
refused roots:   57389  -> 56931   (-458)
rows reached:    1549   -> 1683    (+134)
address Facts:   2881   -> 3044    (+163)
```

This is mechanism/coverage progress only. It is not writer/publication/ownership completeness.

### C65-recorded boundary instances

```text
available = 998
considered = 998
admitted = 458
refused = 540
```

Admission rate: 458 / 998 = 45.8918%.

`admitted` is counted only when the complete root transaction finishes successfully after exact c65 record matching and all-successor preflight. It is not merely a target-relation lookup success.

### C65 exact successor edges

```text
available = 1021
considered = 1021
admitted = 513
refused = 508
```

Edge admission rate: 513 / 1021 = 50.2449%.

The 1021 value remains an edge count, not a safe root/instance count. All-successor fail-closed semantics remain binding.

### C65 all-successor preflight relation refusals

```text
inside_unique_runtime_span_nonentry = 51
executable_file_backed_outside_runtime_metadata = 8
```

These are edge-level preflight diagnostics. They must not be treated as unique-root counts or writer census.

### Phase accounting

Considered / admitted / refused:

```text
same_instance_after_c63_preflight_before_row_fixpoint: 836 / 417 / 419
post_c63_reachable_context:                           77 / 27 / 50
post_c63_reachable_dedup_stored_earlier_path:          2 /  1 /  1
prior_c63_authorization_converted_earlier_refusal:    83 / 13 / 70
```

Reason accounting:

```text
used_c63_source_later_instruction:                 836 / 417 / 419
current_context_post_c63_reachable:                 79 /  28 /  51
prior_c63_authorization_converted_c62_post:         82 /  12 /  70
prior_c63_authorization_converted_pre_c59_later:     1 /   1 /   0
```

The 83 converted-earlier-refusal records remain a separate authenticated provenance class. They were not relabeled as natural post-c63 reachability and did not borrow earlier c61/c63 authorization.

## Carried address facts

```text
c65_certified_crossings_with_facts = 8
c65_address_facts_crossing_rows = 11
call_edges_with_preserved_facts_crossing_rows = 0
```

These counts demonstrate that some previously carried exact address facts survived c65-authorized boundaries. They do not establish a writer or publication chain.

## Next bounded unknown wave

The exact next fail-closed reason is:

```text
uncertified_post_c65_runtime_boundary = 458 roots
```

This is now the smallest bounded continuation-domain unknown exposed by c66.

Other final root refusal reasons remain:

```text
inside_unique_runtime_span_nonentry = 31858
executable_file_backed_outside_runtime_metadata = 18717
indirect_call = 5551
indirect_jump = 258
system_or_unsupported_transfer = 34
summary_cap_refused = 37
instruction_cap = 9
decode_gap = 6
uncertified_later_runtime_boundary = 3
uncertified_post_c65_runtime_boundary = 458
```

The `remaining_boundary_refusal_relation_counts` in aborted roots are edge-callback diagnostics and are not a unique writer census.

## Candidate result

```text
reference_candidate_count = 0
new_cross_row_candidate_count = 0
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
```

Historical c64 candidate output remained unchanged with SHA256:

`4f53cda18c2baa0c0354bb5f9a3ecbe5ed12ab4d8e11ba873c2f11161202b945`

Zero candidates remains incomplete evidence because:

```text
candidate_sweep_complete = false
writer_enumeration_complete = false
```

Therefore c66 does **not** prove writer absence.

## Safety gates

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false. In particular:

- no live slot qword read
- no pointer chase
- no hardware watch
- no game use
- no production integration
- no ownership/lifetime/destination containment proof
- no complete candidate sweep or writer enumeration

C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding.

## Interpretation

C66 is accepted as **VERIFIED-STATIC**. It materially advances the bounded static continuation: 458 of the 998 c65-recorded boundary instances now belong to fully completed roots, adding 134 reached rows and 163 cross-row address Facts relative to c64. No writer candidate was found.

The root cause is still unresolved. The next smallest investigation is not another broad scan: classify only the first terminal `uncertified_post_c65_runtime_boundary` for the exact 458 refused roots, without admitting any new edge or propagating any new Fact. That should be c67.
