# Next task — c56 deterministic sharded full runtime-metadata sweep

## Status

Accepted owner real-image result:

- `analysis/V01C55_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C55_OWNER_OFFLINE_RESULT.md`

Evidence is **VERIFIED-STATIC**. Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

C55 on the exact pinned image admitted real summaries and reduced CALL-clobber unresolved from 324714 to 301194, but the main scan still stopped at the 5,000,000-instruction global cap after considering only 174498 of 527161 runtime functions. 352663 runtime metadata rows remain unconsidered. No local exact-slot candidate was found, but writer/candidate completeness remains false.

Before work, read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, c38/c53/c54/c55 slot-publication evidence/tools/tests, and relevant Wisdoms references.

## Current bounded task

C56 is **offline/static coverage only**. Do not add new value semantics or weaken any c53-c55 refusal rule.

Implement a deterministic sharded/resumable sweep over the complete authenticated runtime-function metadata table so the existing c55 analysis can consider every row without one process-global 5M-instruction cap stopping all later rows.

Preferred shape: a separate `v0.1c56` tool/mode that runs bounded shards and emits one compact aggregate.

### Required guarantees

- exact pinned image SHA and pinned decoder remain mandatory;
- runtime metadata row identity/order is deterministic and authenticated;
- each shard names an exact disjoint half-open row-index interval `[start,end)`;
- the aggregate accepts only one exact image/tool/schema/contracts set;
- intervals must be non-overlapping and exactly cover `[0, runtime_functions_in_metadata)`;
- duplicate, missing, overlapping, out-of-order, wrong-image, wrong-contract, stale or malformed shard evidence refuses;
- per-region c53/c54/c55 caps/refusal semantics remain unchanged;
- c55 direct-entry and c54 runtime-entry summary contracts remain unchanged;
- deferred producer exclusion and external c28 anchor separation remain unchanged;
- no candidate may escape from an incomplete/refused region;
- aggregate counts are exact sums only where categories are additive; do not pretend a sharded count is comparable when semantics differ;
- bounded candidate records must preserve enough provenance to identify source shard and runtime row;
- a row that hits a local region/callee/direct-entry cap is still **considered but refused**, not silently omitted;
- after successful aggregate, `runtime_functions_not_considered` should be `0`;
- this does **not** make writer enumeration complete: executable gaps, EH/alternate entries, indirect targets, refused regions and unsupported semantics remain explicit blockers;
- no slot qword read, pointer chase, producer traversal, hardware watch, PAGE_GUARD, production integration, gameplay or Update-WS/game launch.

### Required diagnostics

Aggregate output must include at minimum:

```text
repository_result
image_authenticated / image_sha256
runtime_functions_in_metadata
runtime_rows_covered
runtime_functions_not_considered
shard_count
shard_intervals
shard_failures
main instructions/bytes/regions modeled/refused
c54 runtime summary attempted/admitted/refused
c55 direct-entry attempted/admitted/refused
call_edges_with_any_preserved_origin
preserved_origin_register_facts_across_calls
call_clobber_all_address_origins_remaining
unresolved_memory_reasons
reference/reader/writer/rmw/address_only candidate counts
candidate_sweep_complete
writer_enumeration_complete
all publication/allocation/base/range/lifetime/ownership/destination/CFG/EH gates
```

Keep `candidate_sweep_complete=false` and `writer_enumeration_complete=false` unless a separately justified proof actually closes every currently listed domain; c56 is not expected to do so.

### Focused tests

Add a focused command, preferably:

```bash
python3 source/tools/test_slot_publication_sharded.py
```

Cover at minimum:

- deterministic partition of synthetic metadata rows;
- exact complete interval coverage accepted;
- missing/duplicate/overlap/out-of-range/wrong-order shard refuses;
- wrong image/schema/tool/contract refuses;
- shard local cap marks affected rows refused but does not omit them;
- aggregate additive counters are exact;
- candidate provenance retains shard/row identity;
- c55 direct-entry behavior remains unchanged inside shards;
- deferred producer remains excluded;
- external c28 anchor is not duplicated as a local candidate;
- no slot value read/pointer chase/runtime instrumentation;
- historical c38/c53/c54/c55 tests remain green.

## Repository result

Emit exactly one:

```text
SHARDED_SLOT_PUBLICATION_SCANNER_READY
SHARDED_SLOT_PUBLICATION_SCANNER_BLOCKED
```

`READY` means the deterministic full-table sharding/aggregation mechanism is fail-closed and tested. It does not mean the real image has been fully swept yet and does not prove a writer/publication source.

If the exact image is unavailable in Codex, publish **one owner OFFLINE command** that performs all shards and aggregation locally and emits one compact c56 JSON. No game launch, Update-WS, Overstrike or runtime log.

## Required validation

Run from repository root:

```bash
python3 source/tools/test_slot_publication.py
python3 source/tools/test_static_access_v2.py
python3 source/tools/test_slot_publication_cfg.py
python3 source/tools/test_slot_publication_callee.py
python3 source/tools/test_slot_publication_direct_entry.py
python3 source/tools/test_slot_publication_sharded.py
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If the focused test has a different exact filename, run/report that command instead.

Expected:
- all historical behavior remains green;
- sharded aggregate is deterministic and fail-closed;
- synthetic complete coverage can produce `runtime_functions_not_considered=0`;
- no semantic broadening or production/gameplay change.

Report every executed command, expected/actual result, PASS/SKIP/UNAVAILABLE/BLOCKED, every unrun test with reason, branch+SHA, and exact owner action.
