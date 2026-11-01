# WallRun `this+0x250` V3 authenticated owner-callee result — 2026-11-01

Branch: `codex/v01c20-edi-value-provenance`.

Owner result file: `MSM2-WS-wallrun250-writers-20261101-092733.json`.

## Accepted result

The V3 offline scan completed successfully on the exact pinned Spider-Man2.exe 2.810.0.0 image:

- EXE SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- Capstone `5.0.6`;
- authenticated WallRun Update `0x1424530..0x14246D6` SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`;
- known Update `this+0x250` reads rediscovered at `0x1424585` and `0x142460E`;
- direct owner-RCX calls revalidated:
  - `0x14245C3 -> 0x367DD10`;
  - `0x1424630 -> 0x29BD5F0`.

Result: `NO_DIRECT_DISP250_WRITER_IN_AUTHENTICATED_OWNER_CALLEES`.

There is no direct `+0x250` writer in:

- authenticated WallRun Update;
- `0x367DD10`;
- `0x29BD5F0`.

This does not prove the field is not written. It proves only that these three bounded functions contain no direct memory write operand whose encoded displacement is exactly `+0x250`.

## Useful provenance from `0x367DD10`

`0x367DD10` receives the authenticated WallRun owner in RCX at entry.

Two entry-reachable branches transform that owner-derived pointer before making direct calls:

### Path A

At `0x367DD2D`:

- `sub rcx, -0x80` => RCX becomes `WallRun owner + 0x80`;
- `0x367DD31 -> 0x29C06F0`.

Therefore original `WallRun owner + 0x250` corresponds to downstream address:

`entry RCX + 0x1D0`.

This arithmetic relation is static and exact for that call entry. It does not yet prove how the callee propagates RCX internally.

### Path B

At `0x367DD38`:

- `add rcx, 0x58` => RCX becomes `WallRun owner + 0x58`;
- `0x367DD3C -> 0x29C18B0`.

Therefore original `WallRun owner + 0x250` corresponds to downstream address:

`entry RCX + 0x1F8`.

Again, this is exact at call entry only; intra-callee provenance remains unproven.

## `0x29BD5F0` branch

V3 found no direct `+0x250` reference or writer in `0x29BD5F0`.

Its linearly decoded later direct calls are not suitable as owner-provenance leads from the current evidence: the reported local context includes a `ret` before the first such call region and then replaces RCX with a RIP-relative address. Do not follow those calls as WallRun-owner propagation without an independent entry-reachability proof.

## Interpretation

Accepted evidence classes:

- c47 `WallRun.this+0x250` changes during actual WallRun: `VERIFIED-RUNTIME`;
- temporal association with direction reversals/down-hold oscillation: `OBSERVED-CORRELATION`;
- no direct `+0x250` writer in the three V3 functions: `VERIFIED-STATIC` for the fixed V3 scope;
- Path A call-entry RCX = owner+0x80: `VERIFIED-STATIC`;
- Path B call-entry RCX = owner+0x58: `VERIFIED-STATIC`;
- exact field meaning/units/producer/writability: `UNKNOWN`.

## Next minimum proof

Do not resume global displacement scanning and do not add a runtime write/watchpoint yet.

Analyze only:

- call target `0x29C06F0` with entry RCX provenance `owner+0x80`; target effective owner field is entry-relative `+0x1D0`;
- call target `0x29C18B0` with entry RCX provenance `owner+0x58`; target effective owner field is entry-relative `+0x1F8`.

Inside each target, use bounded control-flow-aware GPR provenance. Support only clearly provable pointer-preserving transforms such as full-64-bit register copy, base+constant LEA, and 64-bit ADD/SUB immediate. Kill provenance on unsupported/partial/memory-loaded pointer writes and conservatively intersect at CFG merges. Calls clobber volatile GPR provenance unless separately proven.

Report only accesses that are proven on a reachable path to resolve to original `WallRun owner+0x250`, plus direct calls where RCX still has a unique owner-relative provenance. No game write or runtime package change is authorized.
