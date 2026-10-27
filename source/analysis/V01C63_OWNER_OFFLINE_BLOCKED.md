# V01C63 owner offline result — BLOCKED observer provenance gap

2026-10-28 JST. Owner ran the published c63 offline classifier against the accepted private-image c56→c62 evidence chain.

## Classification

**BLOCKED / tool-observer gap**. This is not a c62 regression and not a new gameplay/runtime failure.

Observed owner output progressed through:

```text
c63 unchanged c62 terminal observer replay: 20480/162472
POST_C61_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED: terminal POST not justified by original c62 state
```

No c63 result JSON is accepted. Existing c56-c62 evidence remains immutable and accepted.

## Static diagnosis

`source/tools/slot_publication_post_c61_boundary.py::capture_for()` accepts an edge-callback c62 `POST` only when the current key is already in `tx.post_c61` or `view.c61_source_keys`.

However unchanged c62 `View.edge()` has another legitimate fail-closed route to the same `uncertified_post_c61_runtime_boundary` result: after a prior c61 authorization exists in the root, a later underlying c60 POST / c61-record lookup-or-full-prefix mismatch can be converted to c62 `POST` even when the current key is neither `post_c61` nor the original c61 source key.

Therefore the owner failure is consistent with a real c62 terminal edge callback that c63's observer provenance model does not represent. The exact subtype (unrecorded-root vs context/full-prefix mismatch or another edge-sourced conversion) is UNKNOWN until the observer records the original c62 conversion reason in an owned fixture/replay. Do not infer a PC for invalidation-only non-edge terminals.

Evidence classes:

- owner failure string/progress: **VERIFIED-STATIC owner OFFLINE observation**;
- existence of edge-sourced c62 POST conversion paths outside `post_c61|c61_source_keys`: **VERIFIED-STATIC** from c62/c63 source;
- exact private root and exact conversion subtype: **UNKNOWN**.

## Required repair

Repair c63 only. Do not change c62 admission, facts, candidates, caps, root outcomes, or accepted c62 counts.

The observer must classify the exact original c62 terminal reason/path without pretending every POST is post-c61-reachable state. For edge-sourced converted POST, bind the current source/root/state/path and the exact c62 conversion provenance. For an invalidation-only terminal with no source edge callback, remain BLOCKED rather than inventing an instruction.

Add focused counterexamples for at least:

1. c62 POST from `key in post_c61`;
2. c62 POST from a used c61 source key on a later scan/instruction;
3. c62 POST converted after prior c61 authorization from an underlying c60 POST / lookup or full-prefix mismatch with current key outside both sets;
4. invalidation-only non-edge POST still blocks;
5. unchanged c62 result/candidate bytes and all safety gates.

Owner rerun should reuse the same accepted c56-c62 evidence read-only. No game, Update-WS, Overstrike, native probe, qword read, pointer chase, or evidence editing.
