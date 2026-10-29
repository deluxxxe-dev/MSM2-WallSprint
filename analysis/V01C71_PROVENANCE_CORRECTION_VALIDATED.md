# V01C71 provenance correction — quick validation passed

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Status

The owner ran the dedicated regression for the provenance form that blocked the first c71 owner replay.

Observed:

```text
PASS: c71 owner-observed retired c69 observer distinct edge is recorded without relabeling current reachability
```

This validates the c71 correction for the exact implementation gap observed in the first owner attempt:

- c69 first-terminal observer already authenticated/retired;
- later distinct edge routed by corrected c70 to the next `uncertified_post_c69_runtime_boundary`;
- current key need not be in `tx.post_c69` or `view.c69_source_keys`;
- c71 records the form explicitly as `retired_c69_first_terminal_observer_distinct_edge`;
- c71 does not relabel it as current post-c69 reachability;
- no target traversal/admission/candidate/value read is added.

This is a focused tooling validation only. It is not an accepted c71 owner result.

## Next

Run exactly one corrected c71 owner OFFLINE classifier against the accepted c56-c70 evidence. If another unknown provenance form or evidence mismatch appears, stop fail-closed and analyze that observed fact before widening anything.
