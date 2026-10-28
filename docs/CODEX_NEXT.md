# Next task — c69 post-c67 boundary classifier plus authenticated-prefix checkpoint

## Status

C68 owner OFFLINE result is accepted as **VERIFIED-STATIC**.

Canonical result:

- `analysis/V01C68_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c68 artifact:

```text
MSM2-WS-v0.1c68-post-c65-stitched.json
physical SHA256 47fee13729d1b2d30b1c3cffc9f4bc19837fc18f580d0c0daac909ea7056a246
repository_result POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c68 / schema 1
contract exact_c67_full_prefix_terminal_reason_provenance_all_successor_continuation_v1
semantic result identity dbfcdefeccbc7bcaba048829336d945707f0fc4fc76b2c17780f33413b48255a
```

Measured c68 result:

```text
c67 instances considered/admitted/refused = 458 / 180 / 278
c67 exact edges considered/admitted/refused = 432 / 221 / 211
root transactions completed/refused = 105721 / 56751
rows reached = 1750
address facts crossing rows = 3161
new candidates = 0
```

Exact next unknown wave:

```text
uncertified_post_c67_runtime_boundary = 191
```

## Purpose

Implement c69 as the smallest diagnostic-only classifier for the exact 191 first post-c67 terminal boundaries exposed by accepted c68.

At the same time, remove the avoidable full c56→c68 replay cost from future owner runs by introducing an authenticated-prefix checkpoint/cache path. This is a performance optimization only; it must not weaken provenance or acceptance semantics.

## C69 boundary-classifier requirements

Use the established c57/c59/c61/c63/c65/c67 boundary-classifier vocabulary and patterns.

Required semantics:

- exact accepted c68 result is the source domain;
- classify exactly 191 first `uncertified_post_c67_runtime_boundary` terminals or block;
- bind exact original root, entry-instance state, boundary path, source row/instruction, full-prefix continuation identity and inherited c59/c61/c63/c65/c67 provenance;
- preserve actual terminal phase/reason instead of relabeling earlier authorization as current reachability;
- classify every architectural/model successor with the existing target-relation vocabulary;
- no target traversal;
- no admission through c69;
- no new Fact propagation;
- no candidate creation;
- historical c68 candidate output must remain byte/identity unchanged;
- malformed, stale, duplicate, missing, out-of-order or provenance-mismatching records fail closed;
- all safety gates remain false.

Recommended result:

```text
repository_result POST_C67_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool v0.1c69 / schema 1
contract exact_c68_first_terminal_post_c67_boundary_full_prefix_provenance_diagnostic_v1
```

## Authenticated-prefix checkpoint requirements

The owner observed that c68 replays c56 through c68 over the complete 162472-root domain. Future stages must not do this when the entire predecessor prefix is unchanged.

Implement a deterministic local checkpoint that contains enough accepted prefix state for the next stage to start from the c68 boundary without recomputing c56-c68.

The checkpoint is reusable only when all required identities match exactly, including at least:

- exact executable SHA256;
- accepted c56 manifest identity;
- accepted physical/semantic identities for each required predecessor artifact through c68;
- relevant tool-source SHA256 identities;
- schema versions/contracts;
- analysis limits/profile identities;
- accepted c68 result identity and any record/index pool identities needed for exact replay.

Rules:

- checkpoint bytes must have their own deterministic SHA256/identity;
- no trust by filename, timestamp or path;
- any mismatch/corruption/staleness must fail closed or force explicit full replay; never silently continue;
- checkpoint reuse must produce the same c69 classifier records/result as full replay for the owned test fixtures;
- no weakening of record/provenance checks;
- do not widen caps;
- do not make the checkpoint a writer/completeness certificate;
- keep it local analysis evidence, not production/game state.

Prefer a compact serialized accepted-prefix object rather than saving arbitrary Python objects.

## Focused tests

Cover at least:

- exact 191-domain count contract on owner data path logic;
- ordinary/direct/conditional/CALL-fallthrough classification;
- multi-successor remains diagnostic only;
- interior/free-RX/invalid relation classification without promotion;
- exact c68 root/state/path/source/provenance mutation poison cases;
- converted historical authorization provenance remains distinct;
- duplicate/missing/out-of-order records fail closed;
- no admission/Fact/candidate changes;
- all safety gates false;
- checkpoint create/load deterministic identity;
- unchanged checkpoint reuse equals full replay result on owned fixtures;
- executable/artifact/tool/schema/limits/checkpoint mutation causes refusal or explicit full-replay requirement;
- checkpoint cannot bypass c68/c69 exact provenance checks.

Then run the existing scanner regression set, Windows build/test/smoke/hooks/package validation.

## Do not

- Do not classify anything beyond the exact first 191 post-c67 terminals.
- Do not infer writer absence from zero candidates.
- Do not traverse target rows in c69.
- Do not promote interior or metadata-free RX targets.
- Do not change accepted c56-c68 evidence.
- Do not read slot qwords or chase pointers.
- Do not traverse the deferred producer by assumption.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.
- Do not define c70 continuation behavior before the c69 owner artifact is analyzed.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed. Zero candidates never proves writer absence.
