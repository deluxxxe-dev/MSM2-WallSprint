# WallRun +0x250 V4 owner-provenance result — no `.pdata` for first downstream target

Date: 2026-11-01

## Owner operation

The owner fast-forwarded to V4 and ran the read-only offline provenance helper against the pinned Spider-Man2.exe 2.810.0.0 image.

Generated result:

`MSM2-WS-wallrun250-writers-20261101-130029.json`

The helper refused with:

`downstream_target_no_runtime_function_0x29c06f0`

Helper source commit reported by the owner operation:

`b0abfcac761fe5dda617a3df57317fba5327e2a7`

## Interpretation

This refusal is accepted as a structural result, not a WallRun-field result and not a missing-code claim.

`0x29C06F0` is the exact direct CALL target already authenticated from the V3 owner-relative path, but the PE runtime-function (`.pdata`) table does not contain a range covering that target. Therefore V4 could not use its normal runtime-function boundary/unwind proof and stopped fail-closed before tracing the function.

Do not interpret the refusal as:

- no function at `0x29C06F0`;
- no writer for `WallRun.this+0x250`;
- invalidity of the accepted `owner+0x80` call-entry relation.

## V5 correction

The next helper keeps the V4 authentication/provenance model and adds a fallback only for the two already-authenticated downstream roots when no runtime-function range covers the target.

Strict fallback contract:

- target must be in one unique executable PE section;
- never cross the next PE runtime-function start;
- maximum 128 bytes from the target;
- maximum 32 entry-reachable instructions;
- no CALL;
- no write to Win64 nonvolatile GPRs or RSP;
- no indirect jump;
- conditional branch target and fallthrough must stay inside the bounded scope;
- direct external JMP is recorded only as a tail exit;
- CFG cycles refuse;
- every reachable leaf must terminate by RET or an accepted direct external tail JMP.

Only after that structural proof may the existing owner-relative GPR provenance model inspect accesses resolving exactly to original owner+`0x250`.

No global executable scan, adjacent-offset scan, runtime hook/watchpoint, gameplay write, runtime/package bump, Update-WS, Overstrike, or game run is authorized by this result.

Current runtime remains `v0.1c47 / 0.1.72`.
