# v0.1c1 gate

Package `0.1.19` implements the compact Path B override and lock-free Path A admission for the exact accepted build.

## Implementation gate — PASS in cloud validation

- anchored caller/writer/map proof implemented and negative-tested;
- Path A and Path B consume one coherent atomic publication without the worker state lock;
- Path A preserves Position and Path B writes exactly 0x24 bytes while preserving later metadata and row magnitudes;
- lifecycle, identity, caller, stack, freshness, scale, integrity, write-budget, and kill gates fail closed;
- bounded counter/event logging and hardware-watch continuation remain enabled;
- direct Actor and all later-milestone writes remain disabled.

## Runtime gate — PENDING

The package requires a human A Control run and E diagonal run. Runtime success is `ORIENTATION_RETENTION_PASS=1` on at least one wall with no crash or transform/position corruption. Strong success is PASS on both walls with no `NEW_RESET_AFTER_DUAL_OVERRIDE`.

This pending runtime gate does not authorize adhesion in the c1 package. The next milestone is considered only after the checked-in result contains accepted-build retention evidence.
