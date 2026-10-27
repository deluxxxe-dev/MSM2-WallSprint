# V01C56 owner offline result — accepted full runtime-metadata coverage

Evidence class: **VERIFIED-STATIC** (exact authenticated on-disk image; no game/runtime execution).

## Source / identity

- owner updated `codex/v01c20-edi-value-provenance` through published c56 commit `33d60b1ebc2193839ad921f3678b2f64dc824935`
- tool: `v0.1c56`, schema `1`
- exact image authenticated: `true`
- image SHA-256: `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- repository result: `SHARDED_SLOT_PUBLICATION_SCANNER_READY`
- owner operation was OFFLINE/STATIC only; no game, Update-WS, Overstrike, hardware watch, slot qword read, or pointer chase.

The owner command was the published c56 sharded scanner against the exact private executable, writing compact JSON under the WS logs directory. Personal absolute paths are intentionally not retained here.

## Accepted c56 facts

The deterministic sharded sweep covered the entire authenticated runtime-function metadata table:

```text
runtime_functions_in_metadata = 527161
runtime_rows_covered = 527161
runtime_functions_not_considered = 0
shard_count = 4119
shard_failures = []
```

Main row outcomes:

```text
regions_considered = 527161
regions_modeled = 204770
regions_refused = 322390
regions_excluded = 1
instructions_decoded = 16927172
decoded_bytes = 69007023
```

The one excluded row is the already-deferred producer region. Therefore c56 closes the **runtime-metadata row omission caused by the prior process-global 5M instruction cap**. It does not close executable-code coverage outside that metadata domain.

No local exact-slot candidate was found:

```text
reference_candidate_count = 0
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
candidate_sweep_complete = false
writer_enumeration_complete = false
```

The external c28 reader remained separately authenticated and was not counted as a local candidate.

## Refusal distribution

The dominant row-level refusal classes are:

```text
unsupported_runtime_boundary = 162472
indirect_call = 153992
indirect_jump = 2998
decode_gap = 1758
system_or_unsupported_transfer = 605
instruction_cap = 450
summary_cap_refused = 111
branch_into_middle_bytes = 4
deferred_region_excluded = 1
```

`unsupported_runtime_boundary` plus `indirect_call` account for about 98.16% of refused metadata rows. This makes boundary-exit classification the next bounded static question before attempting any broader writer-completeness claim.

## Shard-local summary counters

The aggregate reports:

```text
c54_runtime_entry_summary_attempted = 19889
c54_runtime_entry_summary_admitted = 2361
c54_runtime_entry_summary_refused = 17528
direct_entry_summary_attempted = 14441
direct_entry_summary_admitted = 7366
direct_entry_summary_refused = 7075
call_edges_with_any_preserved_origin = 5073
preserved_origin_register_facts_across_calls = 5642
call_clobber_all_address_origins_remaining = 932595
```

These counters have explicit c56 scope:

`additive_row_work_and_shard_local_proof_attempts_not_global_unique_callees_or_single_c55_comparison`

Therefore `932595` must **not** be compared numerically with c55's single-process `301194` as if it were a regression or improvement. The c56 counters are additive shard-local work/attempt counts, while c55's accepted figures retain their own historical semantics.

## Remaining blockers / non-claims

The c56 JSON explicitly retains these open domains:

- executable gaps outside runtime metadata;
- deferred producer;
- refused regions / unsupported semantics;
- alternate entries / EH;
- indirect targets and writers;
- publication, allocation, ownership and lifetime.

Accordingly c56 does **not** prove writer absence, publication source, base/allocation/range/lifetime/ownership/destination containment, CFG continuation, production integration, gameplay safety, or exact-slot writability. C52 production-watch FAIL remains binding.

## Next bounded question

Classify every `unsupported_runtime_boundary` exit on the exact authenticated image before allowing any cross-row control-flow admission. The classifier must distinguish source edge kind and exact target relation to runtime metadata / metadata-free RX / unsupported domains. Classification alone must not carry register facts across rows or change candidate output.
