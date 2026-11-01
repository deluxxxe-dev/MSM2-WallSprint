# WallRun.this+0x250 V7 single-handoff result — 2026-11-01

Branch: `codex/v01c20-edi-value-provenance`.
Runtime remains `v0.1c47`, package `0.1.72`.

## Owner result

Owner returned:

`MSM2-WS-wallrun250-writers-20261101-133456.json`

Schema/result:

- schema 7;
- `NO_OWNER_RELATIVE_0X250_ACCESS_IN_V7_HANDOFF_CALLEE`.

The exact pinned EXE SHA, authenticated WallRun Update body/SHA, owner callee body/SHA and both V3 roots were revalidated.

## Re-authenticated V6 Path-B handoff

V7 reproduced the accepted V6 Path-B handoff before entering the deeper callee:

- source target `0x29C18B0`;
- call site `0x29C18D4`;
- call bytes `E8 17 88 6C 00`;
- target `0x308A0F0`;
- pre-call `RCX = owner+0x58`;
- pre-call `RDI = owner+0x58`.

This relation is VERIFIED-STATIC under the bounded owner-relative provenance model.

## Handoff callee result

Target `0x308A0F0`:

- no covering `.pdata` runtime-function range;
- accepted through the strict bounded no-`.pdata` V6 scope;
- bounded scope `0x308A0F0..0x308A120`;
- 8 reachable instructions / 8 worklist visits;
- no CFG cycle;
- no external tail jump;
- `owner+0x250` accesses: 0;
- reachable direct calls: 0.

V7 summary:

- owner-relative `+0x250` accesses: 0;
- reads: 0;
- writes: 0;
- access-unknown: 0.

## Decision

The authenticated RCX-owner callee chain selected from V6 is exhausted at this point:

- Path A had no owner+`0x250` access and no next direct call;
- Path B had one owner-preserving handoff;
- the V7 handoff callee has no owner+`0x250` access and no next direct call/tail lead.

Do not continue blind deeper producer traversal from this chain and do not return to global `+0x250` displacement scans.

The field producer remains UNKNOWN. This bounded no-access chain does not prove the field is never written.

The next useful bounded question is consumer-side: the authenticated WallRun Update directly reads `this+0x250` at `0x1424585` and `0x142460E`. Inspect the local Update control/data context of both reads, including RIP-relative comparison operands and immediate branches, to determine what condition the field participates in. This can narrow field meaning without a runtime write or another gameplay test.
