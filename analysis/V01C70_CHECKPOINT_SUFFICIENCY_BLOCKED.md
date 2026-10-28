# V01C70 checkpoint continuation sufficiency — BLOCKED

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Status

**VERIFIED-STATIC tooling conclusion:** the accepted c69 checkpoint is sufficient to authenticate the exact 191 c69 diagnostic terminal records, but it is **not sufficient to resume the existing full-root continuation transaction with equivalent semantics**.

This does not invalidate the accepted c69 result or checkpoint identities. It narrows what the checkpoint can safely be used for.

## Why the direct c70 fast path is blocked

The existing continuation engine (`slot_publication_stitched.py` and descendants) carries mutable per-root state across the full historical replay. The acceptance/refusal and candidate output can depend on that state.

The transaction contains, among other things:

- all entry-instance contexts and their full path proof objects;
- forward/reverse context graph used for cycle checks;
- work/cap counters already consumed by the historical prefix;
- provisional reference candidates that are committed only if the whole root completes;
- prior crossings and created/deduplicated/cycle accounting;
- the current processed/todo continuation position;
- mutable boundary-authorization state used by the later c59/c61/c63/c65/c67/c68 layers.

The accepted c69 checkpoint contains the authenticated c69 record stream and each terminal's entry seed/state/path **summary identities**, but it does not serialize that full mutable root transaction prefix.

In particular, `checkpoint_incoming_state` is explicitly an entry seed, not the source-instruction must-state. Re-evaluating only the c69 source row with fresh counters/graph would not be equivalent to continuing the original root transaction.

## Unsafe shortcuts rejected

Do not:

- reset transaction work/cap counters at c69;
- drop the historical forward/reverse graph and cycle state;
- treat the entry seed as the source-instruction state;
- omit provisional pre-c69 candidates from a root that later completes;
- replace full boundary path proofs with only their digest and then claim byte/semantic equivalence;
- silently fall back to full replay while reporting the checkpoint fast path as used.

Any of these would change the evidence scope or make the continuation less conservative.

## Consequence

The c69 checkpoint remains accepted for:

- exact c69 result/record authentication;
- deterministic record-stream identity;
- terminal-domain selection and preflight counts;
- future tooling that explicitly defines a narrower independent evidence scope.

It is not yet an exact continuation-resume snapshot for the existing transaction engine.

## Correct next move

C70 should therefore perform one more explicit full historical replay while using the accepted c69 result/checkpoint as the exact authorization source for the c69 boundary. This keeps all existing transaction/cap/cycle/candidate semantics intact.

C70 may continue only the 177 c69 terminal instances whose complete architectural successor set is exact runtime begin. The 13 interior-containing records and one transaction-only record remain fail-closed.

The c70 implementation should be structured so the owner command clearly reports `full_historical_replay_used=true`; do not claim checkpoint acceleration.

A later performance checkpoint must be designed around the actual mutable transaction prefix needed for resumption, or a separately specified narrower continuation semantics must be proven. Do not retrofit weaker semantics under the existing full-root contract.
