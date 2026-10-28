# V01C69 owner correction ready

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Status

The first c69 owner OFFLINE attempt blocked with:

```text
POST_C67_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED: terminal c68 POST without source edge callback; diagnostic source unavailable
```

This was an implementation-assumption failure in c69, not corruption of accepted c68 evidence. Accepted c68 remains VERIFIED-STATIC and its exact `uncertified_post_c67_runtime_boundary = 191` domain is unchanged.

## Root cause

C68 can surface the same fail-closed `uncertified_post_c67_runtime_boundary` result from two observable origin classes:

1. an instruction-edge callback; or
2. a transaction-level entry/propagation guard (`mark_post*`, `instance`, or `connect`) that converts/propagates the lower refusal before a new source-edge callback exists.

The original c69 observer assumed every terminal POST was class (1), so it blocked on a real class-(2) owner case.

## Correction

`source/tools/slot_publication_post_c67_boundary_fix2.py` observes both origin classes without changing c68 continuation semantics.

- Edge-backed terminals keep the existing c69 instruction/successor classifier.
- Transaction-backed terminals bind the exact root/current context/state/path, c67 authorization, current c68 prefix/provenance, event kind and underlying refusal.
- Transaction-backed terminals do **not** invent a source instruction or successor edge.
- No target traversal, admission, new Fact propagation, candidate creation, qword read, pointer chase, native/game probe, gameplay write, or production integration is added.

The prior checkpoint correction remains active underneath fix2.

## Focused correction validation

A lightweight owned smoke specifically exercised transaction-origin capture/record creation and passed:

```text
PASS: c69 quick transaction-origin capture/record smoke
```

This quick validation intentionally avoids replaying the full historical synthetic scanner suite again. The previous c69 aggregate full validation remains evidence for unchanged lower behavior; fix2 is a narrow observer/capture correction and the owner retry is the next discriminating test.

## Next

Retry the exact c69 owner OFFLINE run using `slot_publication_post_c67_boundary_fix2.py` and emit both the c69 result and authenticated-prefix checkpoint. Analyze those outputs before defining c70.
