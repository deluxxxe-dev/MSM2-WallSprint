# V6 WallRun `owner+0x250` owner-relative provenance result — 2026-11-01

## Status

VERIFIED-STATIC for the bounded V6 scopes below. This is not a field-semantics or safe-write proof.

Owner result file:

`MSM2-WS-wallrun250-writers-20261101-132326.json`

V6 completed successfully with schema 6 and result:

`NO_OWNER_RELATIVE_0X250_ACCESS_IN_BOUNDED_DOWNSTREAM_TRACE`

Exact target image SHA and Capstone 5.0.6 matched the existing pinned contract.

## Path A — accepted bounded result

Root:

- `0x29C06F0`
- entry RCX = WallRun owner+`0x80`
- no covering `.pdata` runtime-function range
- V6 strict no-`.pdata` bounded scope `0x29C06F0..0x29C0760`
- reachable instructions: 30
- worklist visits: 30
- contained CFG cycle: yes

Result inside this bounded scope:

- owner-relative `+0x250` access: 0
- reachable direct calls: 0
- external tail jumps: 0

Therefore this bounded Path-A scope provides no current next producer lead. Do not reopen it without new evidence.

## Path B — accepted bounded result

Root:

- `0x29C18B0`
- entry RCX = WallRun owner+`0x58`
- PE runtime-function backed
- function range `0x29C18B0..0x29C1A6C`
- reachable instructions: 136
- worklist visits: 141

Result inside this bounded scope:

- owner-relative `+0x250` access: 0
- one reachable direct call preserves unique RCX owner provenance:
  - call site `0x29C18D4`
  - target `0x308A0F0`
  - pre-call `RCX = owner+0x58`
  - pre-call `RDI = owner+0x58`

This direct call is the only current bounded producer lead from V6.

## Combined V6 result

Summary:

- owner-relative `+0x250` accesses: 0
- reads: 0
- writes: 0
- access-unknown: 0

This does not mean `owner+0x250` is never written. It means neither accepted root directly resolves to that field within the exact V6 supported provenance model and bounded scope.

## Next step

Trace only `0x29C18D4 -> 0x308A0F0`, with callee-entry RCX authenticated as owner+`0x58`.

Do not:

- reopen Path A;
- return to global displacement scans;
- scan adjacent offsets;
- use runtime watchpoints/hooks;
- write the gameplay field.

V7 must re-authenticate the V6 Path-B call/RCX relation against the exact EXE before tracing that one callee.

Current runtime remains `v0.1c47 / 0.1.72`; no runtime rebuild or gameplay test is required for this static step.
