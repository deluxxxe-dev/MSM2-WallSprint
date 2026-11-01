# WallRun +0x250 V5 no-.pdata CFG-cycle result — 2026-11-01

## Owner-local result

The owner ran the pinned offline V5 helper against the exact supported Spider-Man2.exe.

Result:

`REFUSED: no_pdata_fallback_cfg_cycle_0x29c06f0`

Output path reported by the wrapper:

`MSM2-WS-wallrun250-writers-20261101-131833.json`

## Interpretation

This is a structural refusal from the V5 helper, not an engine writer/no-writer result.

Accepted facts:

- Path A target `0x29C06F0` is still the authenticated downstream root receiving RCX = WallRun owner+`0x80` from `0x367DD10`.
- V4 already established that this target is not covered by PE runtime-function (`.pdata`) metadata.
- V5 successfully entered its strict no-.pdata bounded CFG fallback far enough to discover a cycle reachable from `0x29C06F0`.
- V5 then refused solely because its initial fallback policy prohibited every CFG cycle.

Not established:

- no writer;
- field semantics;
- safe writability;
- function-wide boundary/lifetime properties.

## V6 correction

V6 changes only the demonstrated cycle rule. It preserves the V5 no-.pdata bounds and refusal gates:

- max 128-byte window;
- max 32 entry-reachable instructions;
- no CALL;
- no nonvolatile-GPR or RSP write;
- no indirect JMP;
- conditional branch targets/fallthrough remain inside the bounded scope;
- unresolved successors/exits still refuse;
- normal PE runtime-function path remains unchanged.

A contained CFG cycle is now reported rather than rejected. Owner-relative provenance still uses the existing V4 monotone merge-by-intersection worklist. Since per-instruction register provenance can only remain equal or lose entries at merges, the dataflow converges; V6 also applies a separate 2048-worklist-visit cap for no-.pdata scopes.

Current runtime remains `v0.1c47 / 0.1.72`. No gameplay/runtime code changed and no game test is authorized by this result.
