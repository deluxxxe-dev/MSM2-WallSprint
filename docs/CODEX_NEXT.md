# Next task — c70 checkpoint-consuming post-c69 continuation

## Status

Corrected c69 owner OFFLINE result is accepted as **VERIFIED-STATIC**.

Canonical result:

- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c69 result:

```text
MSM2-WS-v0.1c69-post-c67-runtime-boundary.json
physical SHA256 e57b7f39ea476e356397f944d8aadb14841b27874736a60c1a82b982eb86537f
repository_result POST_C67_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool v0.1c69 / schema 1
contract exact_c68_first_terminal_post_c67_boundary_full_prefix_provenance_diagnostic_v1
classifier identity 4f42b0d04346eb5c7b10c8fe0592edd3a3f19fcb463d66a0adf40be01fd25049
semantic result identity 08002e4c3aef2c7cfa037040b6f09f7d1b6e247d0b446daea1ffdb2569aa01fb
record-set SHA256 20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed
```

Accepted checkpoint:

```text
MSM2-WS-v0.1c69-authenticated-prefix-checkpoint.json
physical SHA256 d7b8dae21e9e0478e8ea5d8136c7f19156cd5afca2ff80f167dce48a2e61b6b5
checkpoint contract authenticated_c68_prefix_first_post_c67_records_v1
checkpoint identity b734f8ab2f0a24e002c8b5c2ed1dbbfe75c9411cc773ec1394a1cf330c9b3a28
record count 191
record-stream SHA256 20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed
```

Exact executable:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## Accepted c69 measured domain

```text
post-c67 terminal instances classified = 191
edge-backed terminal instances          = 190
transaction terminal instances          = 1
architectural edges                      = 209
exact_unique_runtime_begin edges         = 196
inside_unique_runtime_span_nonentry      = 13
```

Per-terminal grouping from the authenticated 191-record stream:

```text
177 instances: every architectural successor is exact_unique_runtime_begin
191 edges: exact successors belonging to those 177 complete instances
13 instances: at least one interior successor
1 instance: transaction-only terminal with no instruction/edge
```

The 13 non-exact edge-backed instances are:

```text
8 instances: one interior successor only
5 instances: one exact successor + one interior successor
```

The five mixed cases must remain wholly fail-closed. Do not partially admit their exact arm.

The transaction-only terminal is root `164519`, current row `164524`, event `instance`, reason `transaction_instance_converted_uncertified_post_c65_runtime_boundary`. It has no authenticated source instruction or successor and must not be converted into an edge.

## Purpose

Implement c70 as the smallest OFFLINE/STATIC continuation stage that **actually consumes** the accepted c69 authenticated checkpoint and continues only the 177 complete all-successor-exact c69 terminal instances.

This stage has two inseparable goals:

1. prove that the checkpoint can replace the historical c56→c68 replay without weakening provenance;
2. perform one bounded continuation wave through the exact c69 successors and stop at the first new post-c69 unknown boundary.

Do not define behavior beyond that first new boundary.

## Checkpoint consumer requirements

Add an explicit checkpoint-consumer path. The owner fast path must not silently fall back to a full c56→c68 replay.

Before using any c69 record, authenticate at least:

- exact executable SHA256;
- checkpoint physical SHA256 when an accepted owner path is used;
- checkpoint identity;
- checkpoint version and contract;
- accepted c68 physical/result/authentication identities embedded in the checkpoint;
- accepted c69 result physical SHA256 and semantic result identity;
- c69 classifier identity;
- c69 authenticated record-stream SHA256 and record count;
- relevant tool-source identities;
- schema/contracts;
- analysis limits/profile identities required by the consumer.

Any mismatch, corruption, missing record, duplicate, ordering change, source/tool drift or unsupported checkpoint version must fail closed. Do not trust filename, timestamp or path.

The owner checkpoint path should require the accepted c69 result JSON and checkpoint JSON explicitly enough to bind both physical artifacts. Do not treat the checkpoint's embedded result alone as proof of the accepted physical c69 result file.

## Equivalence proof before owner use

Owned fixtures must provide both:

- a full historical replay reference path; and
- the new checkpoint-seeded c70 path.

For the same fixture, require exact equality of all c70-observable semantics that can affect acceptance or output, including:

- selected c69 terminal records;
- source row/instruction identity where edge-backed;
- incoming state identity;
- boundary path/provenance identity;
- successor state/path derivation;
- target preflight/admission/refusal;
- first new terminal reason;
- candidate records/counts;
- row/fact/cap accounting relevant to c70 output.

The fixture proof must fail if checkpoint state/provenance is insufficient to reproduce the full-replay result. Do not patch over differences by ignoring fields.

## State reconstruction rule

C69 records explicitly describe an authenticated **entry seed**, not an instruction-level must-state.

Therefore c70 must not copy `checkpoint_incoming_state` directly to a successor as if it were the state at `source_instruction_rva`.

For each selected edge-backed record, derive the successor state by re-evaluating only the authenticated current source-row semantics needed to reach the c69 terminal under current exact code/metadata and the sealed c69 source/provenance contract. Recheck source row/code/instruction identities before using the derived state.

If the consumer cannot reproduce the same terminal/source state from the checkpoint record without historical replay, fail closed and return to ChatGPT with the exact missing prefix state. Do not broaden the checkpoint or guess state semantics silently.

## C70 continuation requirements

Use the accepted c69 record stream as the exact source domain.

Only a terminal record satisfying **all** of the following may be considered for continuation:

- terminal origin is edge-backed;
- the record is exact/current/authenticated;
- every architectural successor in that record is present;
- every architectural successor relation is `exact_unique_runtime_begin`;
- no source/target span overlap or target preflight mismatch exists;
- every target row begin/bounds/code identity is current and exact;
- successor state/path derivation passes the checkpoint/full-reference equivalence contract.

Expected owner-domain preflight counts before traversal are therefore:

```text
c69 terminal instances available = 191
all-successor-exact instances available = 177
exact successor edges in those instances = 191
interior-containing terminal instances refused before traversal = 13
transaction-only terminal instances refused before traversal = 1
```

If these counts do not match accepted c69 evidence, block.

All successors of one terminal are atomic: preflight every successor first, then admit the terminal only if the entire set passes. Never keep a provisional exact arm from a mixed/refused terminal.

After admission, continue only from those exact target entry instances using the derived authenticated state/path. Preserve exact root separation and existing bounded caps/dedup/cycle behavior.

Stop at the first new row-external boundary not already authorized by the c69 record. Recommended new fail-closed label:

```text
uncertified_post_c69_runtime_boundary
```

Do not classify or traverse that next boundary in c70.

## Output scope

Recommended result identity:

```text
repository_result POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c70 / schema 1
contract exact_c69_checkpoint_full_prefix_all_successor_continuation_v1
```

Report separately at least:

- checkpoint authentication status/identity;
- checkpoint fast path used vs fixture reference mode;
- c69 terminal instances available/considered/admitted/refused;
- c69 exact edges available/considered/admitted/refused;
- refusal counts for interior-containing and transaction-only c69 terminals;
- root transactions/continuation instances/rows/facts/caps under the c70 bounded domain;
- first new post-c69 boundary refusal count;
- candidate counts/records using the existing candidate vocabulary;
- all safety gates.

Any candidates remain OFFLINE static candidates only. Zero candidates never proves writer absence.

## Focused tests

Cover at least:

- accepted c69 result/checkpoint exact owner identities are pinned;
- checkpoint body/result/record-stream mutation fails closed;
- wrong executable/tool/schema/contract/limits fails closed;
- checkpoint consumer does not invoke full c56→c68 replay on the fast path;
- checkpoint-seeded path equals full-replay reference on owned fixtures;
- entry-seed state cannot be borrowed as source-instruction must-state;
- exact source-row replay derives the same successor state as full reference;
- exact one-successor terminal continues;
- exact two-successor terminal continues only after both arms preflight;
- one exact + one interior terminal refuses both arms atomically;
- interior-only terminal refuses;
- transaction-only terminal refuses and creates no synthetic instruction/edge;
- stale root/state/path/source/provenance record refuses;
- duplicate/missing/out-of-order record stream refuses;
- next unknown post-c69 boundary stops with no post-abort exploration;
- no unauthorized slot read/pointer chase/game/native probe;
- all safety gates remain false.

Then run the relevant scanner regression set and normal build/test/smoke/hooks/package validation required by the repository.

## Change scope

Allowed:

- new c70 OFFLINE tool/test files;
- minimal c69 checkpoint validation/consumer helper changes if strictly required;
- test registration;
- c70 analysis/result documentation.

Do not change accepted c56-c69 evidence semantics merely to make checkpoint reuse pass.

If the checkpoint proves insufficient, stop with the exact missing state/provenance requirement instead of replaying all history invisibly or weakening equality.

## Do not

- Do not continue the 13 interior-containing c69 terminals.
- Do not continue the transaction-only c69 terminal.
- Do not partially admit a mixed successor record.
- Do not invent a source instruction or edge for transaction terminals.
- Do not treat entry-seed state as instruction must-state.
- Do not silently full-replay c56-c68 on the owner checkpoint fast path.
- Do not infer writer absence from zero candidates.
- Do not read slot qwords or chase pointers.
- Do not traverse the deferred producer by assumption.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.
- Do not define c71 continuation behavior before the c70 owner artifact is analyzed.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed unless later independently proven and explicitly authorized.
