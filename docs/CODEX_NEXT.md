# Next task — c70 full-replay post-c69 continuation

## Status

Corrected c69 owner OFFLINE result is accepted as **VERIFIED-STATIC**.

Canonical results:

- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C70_CHECKPOINT_SUFFICIENCY_BLOCKED.md`

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

Accepted c69 checkpoint:

```text
MSM2-WS-v0.1c69-authenticated-prefix-checkpoint.json
physical SHA256 d7b8dae21e9e0478e8ea5d8136c7f19156cd5afca2ff80f167dce48a2e61b6b5
checkpoint identity b734f8ab2f0a24e002c8b5c2ed1dbbfe75c9411cc773ec1394a1cf330c9b3a28
record count 191
record-stream SHA256 20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed
```

Exact executable:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## Why c70 must full-replay

The accepted c69 checkpoint authenticates the exact diagnostic record stream, but it does not serialize the complete mutable per-root continuation prefix required by the existing transaction engine.

Missing exact-resume state includes historical contexts/path proofs, forward/reverse graph, already-consumed work/cap counters, provisional root candidates, continuation queue/processed state and mutable authorization state.

Do **not** reset these at c69 and call that equivalent continuation.

C70 therefore performs one explicit c56→c68 historical replay and uses the accepted c69 result/checkpoint only as the exact authorization source at the first post-c67 c68 terminal.

C70 output must state:

```text
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

Do not silently claim checkpoint acceleration.

## Accepted c69 continuation domain

```text
c69 terminal instances total = 191
edge-backed instances = 190
transaction-only instances = 1
architectural edges = 209
exact_unique_runtime_begin edges = 196
inside_unique_runtime_span_nonentry edges = 13
```

Per-terminal grouping:

```text
177 instances: every architectural successor is exact_unique_runtime_begin
191 exact edges: belong to those 177 complete instances
13 instances: at least one interior successor
1 instance: transaction-only, no source instruction/edge
```

The 13 non-exact edge-backed instances are:

```text
8 instances: one interior successor only
5 instances: one exact successor + one interior successor
```

The five mixed cases must remain wholly fail-closed. Never partially admit the exact arm.

The transaction-only terminal is root `164519`, current row `164524`, event `instance`, reason `transaction_instance_converted_uncertified_post_c65_runtime_boundary`. It must remain fail-closed and must not receive a synthetic instruction/edge.

## Purpose

Implement c70 as the smallest OFFLINE/STATIC full-root continuation stage that:

1. reconstructs the accepted c56-c68 historical transaction semantics unchanged;
2. authenticates the accepted c69 result/checkpoint exactly;
3. uses each c69 record as the only authorization for its first c68 `uncertified_post_c67_runtime_boundary` event;
4. continues only complete all-successor-exact c69 edge-backed terminals;
5. stops at the first new post-c69 unknown boundary.

Do not define behavior beyond that first new boundary.

## C69 artifact authentication

Before c69 authorization, require exact:

- executable SHA256;
- accepted c69 result physical SHA256;
- accepted c69 result semantic identity;
- c69 classifier identity/header;
- checkpoint physical SHA256;
- checkpoint identity/version/contract;
- checkpoint embedded c69 result identity;
- checkpoint record count and record-stream SHA256;
- accepted c68 identities embedded in the c69 evidence;
- c69 tool/schema/contract/source identities;
- existing limits/profile identities.

Any mismatch/corruption/staleness/missing/duplicate/out-of-order record must fail closed.

The c69 JSON and checkpoint must both be explicit owner inputs. Do not trust only the checkpoint's embedded result.

## First c69 terminal matching

On the replayed root's first c68 `uncertified_post_c67_runtime_boundary` event:

- locate the exact c69 record for that original root;
- reproduce the corrected c69 terminal record from the live replayed transaction;
- require byte/semantic equality with the accepted checkpoint record before considering continuation;
- preserve the corrected distinction between edge-backed and transaction-level terminal origin.

For a transaction-only c69 record:

- validate the current transaction terminal exactly;
- count it as considered/refused;
- do not synthesize an instruction or successor;
- abort that root with an explicit c69 transaction-terminal refusal.

## Edge-backed all-successor preflight

For an edge-backed c69 record, classify/preflight the complete stored architectural successor set before returning any c69 continuation proof.

A terminal is eligible only if every successor is:

- architectural;
- `exact_unique_runtime_begin`;
- non-overlapping source/target span;
- current exact runtime row begin/bounds;
- current exact target row/code under the authenticated executable.

Expected accepted c69 preflight domain:

```text
all-successor-exact terminal instances = 177
continuable exact edges = 191
interior-containing terminal instances refused = 13
transaction-only terminal instances refused = 1
```

If these accepted source-domain counts do not match, block.

All successors of one c69 terminal are atomic. Preflight the entire set before marking that terminal eligible. A mixed exact+interior record refuses the whole terminal and commits no exact arm.

## C69 continuation proof

For each admitted exact c69 successor, create a bounded proof that binds at least:

- exact image identity;
- accepted c69 result/checkpoint identities;
- c69 record identity;
- c69 classifier/result/record-set identity;
- original root identity;
- current entry-instance state identity;
- c69 source row/code/instruction identity;
- c69 boundary path/full-prefix identity;
- c69 terminal phase/reason/provenance;
- nested accepted c67 authorization/provenance;
- edge kind/target/runtime-row identity;
- current target row/code identity.

Use a distinct `proof_kind`, e.g. `c69_post_boundary`.

Do not relabel historical c67/c69 authorization as current reachability.

## Post-c69 continuation

Once a c69 proof creates an exact target entry instance:

- preserve the existing transaction graph/caps/cycles/dedup/candidate semantics from the full replay;
- mark the resulting context as post-c69;
- propagate that mark only through the existing transaction continuation operations;
- any later row-external boundary not authorized by that same first c69 record is the next unknown and must abort the root.

Recommended next fail-closed label:

```text
uncertified_post_c69_runtime_boundary
```

Do not classify/traverse that next boundary in c70.

## C70 output

Recommended identity:

```text
repository_result POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c70 / schema 1
contract exact_c69_full_replay_all_successor_continuation_v1
```

Report separately at least:

- accepted c69 result/checkpoint identities;
- `full_historical_replay_used=true`;
- `checkpoint_accelerated_resume_used=false`;
- c69 terminal instances available/considered/preflight-eligible/admitted/refused;
- transaction-terminal refusal count;
- interior-containing terminal refusal count;
- c69 continuable exact edges available/considered/admitted/refused;
- next `uncertified_post_c69_runtime_boundary` refusal count;
- root transactions completed/refused;
- continuation instances/rows/facts/caps;
- candidate counts/records;
- all safety gates.

Any candidates remain OFFLINE static candidates only. Zero candidates never proves writer absence.

## Focused tests

Cover at least:

- exact accepted c69 result/checkpoint owner identities pinned;
- c69 JSON/checkpoint physical/semantic/stream mutation fails closed;
- full replay reconstructs the same c69 first-terminal record on fixtures;
- exact one-successor c69 terminal continues;
- exact two-successor c69 terminal continues only after both arms preflight;
- one exact + one interior terminal refuses both arms atomically;
- interior-only terminal refuses;
- transaction-only terminal validates then refuses without synthetic edge;
- c69 root/state/path/source/phase/reason/provenance mutation refuses;
- duplicate/missing/out-of-order c69 record stream refuses;
- later boundary after c69 continuation becomes `uncertified_post_c69_runtime_boundary`;
- no post-abort provisional candidate/admission commits;
- existing full-root caps/cycles remain active across the c69 continuation;
- no slot qword read/pointer chase/native/game probe;
- all safety gates remain false.

Then run the relevant scanner regression set and normal build/test/smoke/hooks/package validation.

## Change scope

Allowed:

- new c70 OFFLINE tool/test files;
- minimal c69 helper exposure if strictly required without changing accepted c69 semantics;
- test registration;
- c70 analysis/result docs.

Do not change accepted c56-c69 evidence semantics to make c70 pass.

## Do not

- Do not use the current c69 checkpoint as an exact transaction-resume snapshot.
- Do not reset historical caps/graph/candidate state at c69.
- Do not continue the 13 interior-containing c69 terminals.
- Do not continue the transaction-only c69 terminal.
- Do not partially admit mixed successors.
- Do not invent transaction-terminal instructions/edges.
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
