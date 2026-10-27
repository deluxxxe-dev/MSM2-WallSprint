# c57 owner offline runtime-boundary result

2026-10-27 JST. Branch `codex/v01c20-edi-value-provenance`, implementation/publication HEAD before owner run `b9c185960cd562f2711e3a41d9816c1297aaf91c`.

Evidence class: **VERIFIED-STATIC**. Repository result: **RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY**.

Owner supplied `MSM2-WS-v0.1c57-runtime-boundary.json` from the pinned private image. Supplied JSON SHA256: `dc8ae1deaf71ede331ee3d26604bd6e0738bd6fdf89a4aaa92e41c572ddd680a` (51,750 bytes). The JSON is derived diagnostic evidence only; the private EXE is not stored.

## Identity / completeness

- image authenticated: true
- image SHA256: `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- decoder: Capstone 5.0.6
- c56 manifest: `a93a176c58b53e0e5b16c74114ca2ab3956eeacc1d6649c7b3cac4dacc84046f`
- expected boundary-refused rows: 162,472
- classified boundary-refused rows: 162,472
- classification_complete: true
- boundary edges: 201,788 architectural / 201,788 total
- model-only boundary edges: 0
- runtime rows remain 527,161/527,161 considered
- c56 candidate counts unchanged: true
- cross_runtime_fact_propagation: false

This closes the c57 diagnostic domain only. It does not change c56 candidate admission or establish writer/publication completeness.

## Exact target distribution

Across 201,788 row-external boundary edges:

| target relation | count | share |
|---|---:|---:|
| exact unique runtime begin | 159,734 | 79.1593% |
| inside unique runtime span, non-entry | 29,560 | 14.6490% |
| executable file-backed outside runtime metadata | 12,494 | 6.1916% |
| overlap / unbacked / nonexec-or-writable / outside-image / deferred | 0 | 0% |

The exact-runtime-begin class is the dominant safely-identifiable next boundary class, but c57 itself grants no continuation permission.

Exact adjacency totals 90,430 edges (44.8144% of all boundary edges):

- ordinary fallthrough: 40,926
- conditional fallthrough: 39,335
- direct conditional taken: 10,087
- direct unconditional branch: 81
- call fallthrough: 1

The remaining exact-runtime-begin edges are 69,304 non-adjacent direct-control transfers. Adjacency is diagnostic only; direct edge identity, not proximity, is the relevant control-flow fact.

## Edge-kind distribution

- direct conditional taken: 81,015 (40.1486%)
- ordinary fallthrough: 41,084 (20.3600%)
- direct unconditional branch: 40,353 (19.9977%)
- conditional fallthrough: 39,335 (19.4932%)
- call fallthrough: 1
- other supported direct successor: 0

Within the exact-runtime-begin class:

- direct conditional taken: 54,773
- ordinary fallthrough: 40,926
- conditional fallthrough: 39,335
- direct unconditional branch: 24,699
- call fallthrough: 1

Interior targets are entirely direct branches in this result: 26,242 conditional-taken and 3,318 unconditional. Metadata-free RX targets are 12,336 unconditional branches plus 158 ordinary fallthroughs.

## Safety interpretation

Runtime fact: none; this is an offline exact-image static classification.

Static fact: c57 authenticated and completely classified the accepted c56 `unsupported_runtime_boundary` set. A large majority of boundary edges land exactly at one unique authenticated runtime-row begin. No ambiguous-overlap, outside-image, unbacked, nonexec/writable, or deferred target occurred in this measured boundary set.

Not proven:

- an exact runtime begin is not automatically a safe all-entry invariant or one logical function;
- adjacency is not ownership or same-function proof;
- no register/address fact may cross a row solely because c57 classified the edge;
- interior and metadata-free RX targets remain refused;
- indirect calls/writers, EH/alternate entries, executable gaps, unsupported semantics, deferred producer, allocation/base/range/lifetime/ownership/destination/publication remain open;
- writer_candidates remains 0 but candidate_sweep_complete=false and writer_enumeration_complete=false, so this is not a no-writer proof;
- c52 production VCH review FAIL remains binding; no production/gameplay permission follows.

## Next bounded direction

c58 should test a separate **exact-runtime-begin cross-row address-provenance continuation** contract. It may carry only already-proven c53-style GPR image-address facts across an authenticated architectural edge whose c57 target relation is exactly `exact_unique_runtime_begin`. All other c57 target classes remain refused.

The continuation must remain path/entry-instance scoped: an incoming predecessor state must not be silently merged with or promoted to the target row's independent empty-root analysis. Within each continuation instance, retain c53 must-join/fail-closed semantics. No memory-loaded value, pointer chase, ABI assumption, stack/local/callee-return value, or publication inference may be introduced.

A successful c58 tool still must keep `candidate_sweep_complete=false` and `writer_enumeration_complete=false`; it only tests whether the dominant exact-entry boundary class can safely extend existing static address provenance.
