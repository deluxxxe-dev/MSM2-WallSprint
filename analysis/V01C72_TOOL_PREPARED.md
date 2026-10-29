# V01C72 tool prepared — focused validation pending

## Status

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

C72 implementation is prepared but is **not yet TOOL READY** until the focused synthetic suite passes.

## Accepted basis

C71 remains accepted **VERIFIED-STATIC**:

```text
checkpoint physical SHA256 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
checkpoint identity 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
c71 result identity 18d61be59032514a4c74c9b015f422c379e2c05050e2d5819c1025048a2276d7
c71 classifier identity 18cba768c4261886b70496e7f73abe3fe5276b44aecadb0d929802ae3dae9fd5
record stream 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
records 102
```

Continuation domain:

```text
all-successor-exact c71 records = 95
exact architectural successor edges = 99
interior-containing records = 6
other non-continuable relation records = 1
```

## Implementation

Added:

- `source/tools/slot_publication_post_c71_stitched.py`
- `source/tools/test_slot_publication_post_c71_stitched.py`

C72 uses full historical replay and does not treat the c71 checkpoint as a mutable resume snapshot.

At each live c70 post-c69 terminal it:

1. reproduces the c71 terminal record using the unchanged c71 observer;
2. requires byte/semantic equality with the accepted checkpoint record for that exact root;
3. preflights the complete architectural successor set;
4. continues only if every successor is `exact_unique_runtime_begin`;
5. treats later distinct c70 POST events after c71 authorization as `uncertified_post_c71_runtime_boundary`;
6. commits c71 admission only if the whole root transaction completes.

The six interior records and one executable-file-backed-outside-runtime-metadata record remain refused.

## Additional lifecycle guard

During implementation a potential relabeling hazard was identified: after an edge-backed c71 terminal is already authenticated, a later transaction-level c70 refusal must be treated as the next post-c71 unknown, not re-authenticated as a second c71 terminal.

C72 therefore distinguishes:

- first transaction-origin c71 terminal: authenticate and refuse as non-continuable;
- any later c70 transaction refusal after an authenticated edge-backed c71 terminal: map to `uncertified_post_c71_runtime_boundary`.

## Safety

No c56-c71 evidence semantics were changed.

C72 adds no slot qword read, pointer chase, native/game probe, gameplay write or production integration. All existing safety gates remain closed.

## Required next check

Run only:

```text
source/tools/test_slot_publication_post_c71_stitched.py
```

Do not run the owner c72 replay until the focused suite passes.
