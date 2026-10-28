# Next task — c71 diagnostic classification of c70 post-c69 boundary

## Status

Corrected c70 owner OFFLINE result is accepted as **VERIFIED-STATIC**.

Canonical result:

- `analysis/V01C70_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c70 result:

```text
MSM2-WS-v0.1c70-post-c69-stitched.json
physical SHA256 2780ca89a98ad22d12e2252f53cd202830bc8228edd67d0b0c7fb8eccf7f15ac
size 15044 bytes
semantic result identity 9b213db28b05f9d1fc8baefa37f981aa521cd7ee45110a7cfe6b122567f0b9e1
repository_result POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c70 / schema 1
contract exact_c69_full_replay_all_successor_continuation_v1
```

Exact executable:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

Accepted c69 identities remain pinned by c70 and must remain exact.

## Measured c70 domain

```text
c69 boundary instances available/considered = 191 / 191
all-successor-exact c69 instances = 177
c69 instances admitted = 70
c69 instances refused = 121
c69 exact edges admitted = 84
c69 exact edges refused = 107
c69 record mismatch counts = {}

post_c69_uncertified_boundary_refusals = 102
```

The exact next diagnostic domain is **102**, not 107. Five other preflight-eligible roots were refused by existing fail-closed conditions after c69 preflight and are not c71 boundary records.

## Purpose

Implement c71 as the smallest OFFLINE/STATIC diagnostic stage that observes exactly the accepted c70 first terminal `uncertified_post_c69_runtime_boundary` event for each of those 102 roots, records exact full-prefix provenance, classifies any architectural successors, and rethrows/refuses the same terminal.

C71 is diagnostic only. It must not traverse or admit a c71 target.

Recommended identity:

```text
repository_result POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool v0.1c71 / schema 1
contract exact_c70_first_terminal_post_c69_boundary_full_prefix_provenance_diagnostic_v1
```

## Required replay model

C71 must use the same full historical replay semantics as accepted c70.

Do not claim checkpoint acceleration. If a new diagnostic checkpoint is emitted, it is evidence serialization only unless a later task separately proves exact mutable resume sufficiency.

Require:

```text
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

## C70 artifact authentication

Before observing c71 terminals, require exact:

- executable SHA256;
- accepted c70 physical SHA256;
- accepted c70 semantic result identity;
- c70 repository result/tool/schema/contract;
- c70 authentication header and limits;
- accepted c69 result/checkpoint identities embedded in c70;
- c70 tool-source identities;
- all c70 safety gates still closed.

Any mismatch, mutation, stale file, wrong schema/contract/limits, or changed accepted c69 identity fails closed.

## Exact c71 terminal domain

The c70 owner result measured exactly:

```text
uncertified_post_c69_runtime_boundary = 102 roots
```

C71 must reproduce exactly 102 terminal records under unchanged replay.

Do not create records for:

- the 13 c69 non-exact preflight refusals;
- the one c69 transaction-only refusal;
- the five other post-c69-preflight roots that later refused for a reason other than `uncertified_post_c69_runtime_boundary`;
- any historical c68/c69 boundary that was already authorized or refused at an earlier phase.

Missing, extra, duplicated, stale, reordered, or borrowed provenance fails closed.

## Terminal-origin distinction

Do not assume all 102 terminals are instruction-edge callbacks.

C70 can produce `uncertified_post_c69_runtime_boundary` through edge-backed and transaction-level continuation paths. C71 must preserve the exact origin class.

For an edge-backed terminal:

- capture exact current root/state/path/source row/source instruction;
- bind the accepted c69 authorization record(s) on the path;
- bind current c70 post-c69 origin/proof context;
- classify the complete architectural successor set using the existing relation vocabulary;
- do not traverse any successor.

For a transaction-level terminal:

- capture the exact transaction event kind and current root/state/path/context;
- capture the underlying refusal/conversion cause;
- bind the exact accepted c69/c70 provenance available at that event;
- do not invent a source instruction or successor edge;
- emit an empty edge list.

Report edge-backed and transaction-level terminal counts separately.

## Provenance requirements

Each c71 record must bind enough information to prevent another root/state/path/source from borrowing the authorization. At minimum include or identity-bind:

- exact image identity;
- accepted c70 physical/semantic identity;
- c70 tool/schema/contract/limits/source identities;
- original root identity;
- current entry-instance row/state identity;
- exact boundary path/full-prefix identity;
- current post-c69 provenance;
- accepted c69 result/checkpoint/classifier/record-set identities;
- c69 record identity/identities present on the current path;
- nested accepted c67 provenance where already carried;
- terminal phase/reason/origin kind;
- source row/code/instruction identity when edge-backed;
- transaction event/context/cause when transaction-level;
- complete successor relation classification when edge-backed.

Do not relabel historical c69 authorization as current c71 reachability.

## Classification vocabulary

For edge-backed terminals, reuse the existing relation labels, including at least:

```text
exact_unique_runtime_begin
inside_unique_runtime_span_nonentry
executable_file_backed_outside_runtime_metadata
executable_not_fully_file_backed
non_executable_or_writable_section
outside_image
overlapping_multiple_runtime_spans
deferred_producer_excluded
```

Record architectural edge kind and target identity. No edge is authorization to continue in c71.

## Diagnostic-only contract

C71 must add:

```text
new_post_c69_boundaries_admitted = 0
new_cross_row_candidate_count = 0
live_qword_reads = 0
```

Historical c70 candidate output must remain byte/semantic equivalent for candidate accounting.

No new Fact, candidate, target entry instance, pointer/value read, native probe, or game interaction may be introduced by c71.

All safety gates remain false.

Zero candidates does not prove writer absence.

## Output

Report at least:

- accepted c70 physical and semantic identities;
- expected/classified c71 terminal count = 102/102;
- unique root/state/terminal-context counts;
- edge-backed terminal count;
- transaction terminal count;
- terminal event-kind counts;
- terminal phase/reason counts;
- architectural/model-only edge counts;
- relation counts;
- complete per-terminal successor grouping where edge-backed;
- authenticated record-set identity;
- historical c70 candidate counts unchanged;
- read-only/live-qword/safety gates.

A deterministic diagnostic checkpoint may be produced for the exact 102 record stream, with its own digest and accepted c70 identity. Do not claim it is a mutable transaction resume snapshot.

## Focused tests

Cover at least:

- accepted c70 physical/semantic identity pinned;
- exact 102 production-domain constant pinned;
- exact edge-backed terminal reproduction;
- direct/conditional/CALL-fallthrough successor classification without traversal;
- multi-successor classification without partial admission;
- interior and metadata-free RX remain diagnostic only;
- transaction-origin terminal is represented without synthetic instruction/edge;
- c70 root/state/path/c69-record/source/phase/reason/origin mutation fails closed;
- duplicate/missing/out-of-order record evidence fails closed;
- historical c70 accounting/candidate bytes remain unchanged;
- no c71 admission/Fact/candidate;
- no slot qword read/pointer chase/native/game probe;
- all safety gates remain false.

Use small fixtures first. Do not force a production full replay merely to debug fixture/test mistakes.

After focused tests pass, run the owner OFFLINE classifier once against the exact accepted c56-c70 evidence.

## Change scope

Allowed:

- new c71 OFFLINE diagnostic tool/test files;
- minimal c70 helper exposure/correction layer if strictly required without changing accepted c70 semantics;
- test registration;
- c71 analysis/result docs.

Do not change accepted c56-c70 evidence semantics to make c71 pass.

## Do not

- Do not traverse a c71 target.
- Do not define c72 continuation behavior before c71 owner classification is analyzed.
- Do not turn the 102 diagnostic records into admission proof in c71.
- Do not infer 107 records from c69 exact-edge refusal accounting.
- Do not invent transaction-terminal instructions or successors.
- Do not infer writer absence from zero candidates.
- Do not read slot qwords or chase pointers.
- Do not traverse the deferred producer by assumption.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed unless later independently proven and explicitly authorized.
