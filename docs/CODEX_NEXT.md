# Next task — trace transformed owner provenance for c47 `WallRun.this+0x250`

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`, `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`, and current `docs/CODEX_RESULT.md`.

## Accepted evidence

Do not repeat c47 gameplay, RequestSimpleMotion, GravityOrientationOwner, or V1/V2 global `+0x250` scans.

Accepted runtime:

- `WallRun.this+0x250` changes during verified WallRun;
- changes correlate with ordinary reversals and down-hold left/right oscillation;
- direct signed left/right-input semantics are not supported;
- field meaning/units/producer/writability remain UNKNOWN.

Accepted V3 static result from owner JSON `MSM2-WS-wallrun250-writers-20261101-092733.json`:

- exact EXE/Update identities passed;
- known Update `+0x250` reads rediscovered;
- V3 result `NO_DIRECT_DISP250_WRITER_IN_AUTHENTICATED_OWNER_CALLEES`;
- no direct encoded `+0x250` writer in Update, `0x367DD10`, or `0x29BD5F0`.

This is only a bounded no-direct-writer result.

## Exact next provenance roots

`0x367DD10` receives authenticated WallRun owner in RCX at entry.

Path A:

- at `0x367DD2D`, `sub rcx,-0x80` => RCX = owner+`0x80`;
- call `0x367DD31 -> 0x29C06F0`;
- original owner+`0x250` corresponds to target-entry-relative `+0x1D0` if the same provenance is preserved.

Path B:

- at `0x367DD38`, `add rcx,0x58` => RCX = owner+`0x58`;
- call `0x367DD3C -> 0x29C18B0`;
- original owner+`0x250` corresponds to target-entry-relative `+0x1F8` if provenance is preserved.

Do not pursue `0x29BD5F0`'s later linear call list as an owner path without new reachability evidence; the V3 context crosses a `ret` and then replaces RCX with RIP-relative state.

## Implementation task

Extend the existing offline helper `source/tools/wallrun_scalar_writer_refs.py` only enough to perform control-flow-aware owner-relative GPR provenance in the two roots above.

Required behavior:

1. retain exact Spider-Man2.exe SHA guard and Capstone 5.0.6 guard;
2. retain authentication of WallRun Update and `0x367DD10` source relation;
3. pin/verify the exact Path A and Path B instruction bytes/call targets from the accepted V3 result before tracing downstream;
4. locate the PE runtime function containing each downstream call target; target may be an exact runtime entry or an interior instruction boundary;
5. require complete contiguous decode of each containing runtime function and require each call target to be an exact decoded instruction boundary;
6. start trace A with `RCX = owner+0x80` at `0x29C06F0`;
7. start trace B with `RCX = owner+0x58` at `0x29C18B0`;
8. follow only entry-reachable CFG instructions within the containing runtime function;
9. support provenance only for:
   - full-64-bit register-to-register MOV;
   - full-64-bit LEA of `known_base + constant` with no dynamic index;
   - full-64-bit ADD/SUB immediate;
10. kill destination provenance on partial-width writes, memory loads, unsupported transforms, or other writes;
11. merge CFG states by intersection: retain a register only when all merged incoming states prove the same owner-relative offset;
12. before a CALL, report current RCX owner-relative offset if uniquely proven; after CALL, invalidate volatile Win64 GPR provenance (`RAX/RCX/RDX/R8-R11`) unless separately proved;
13. report a memory access only when current path provenance proves `base_owner_offset + displacement == 0x250` with no dynamic index;
14. distinguish read/write/access-unknown from decoder metadata;
15. report direct calls reachable from each root with the set of proven RCX owner offsets seen at that call site, so a no-writer result still provides the next bounded lead.

## Safety / scope

Do not:

- scan the whole executable for offsets/writers;
- scan adjacent WallRun fields;
- infer pointer provenance through memory loads;
- treat indexed/computed addresses as proven;
- follow deeper callees in the same task;
- add hooks/watchpoints/page guards;
- write `+0x250` or any gameplay value;
- bump runtime/package version;
- modify runtime c47 code/config;
- modify SocialFeed or any other MOD.

The Windows bootstrap may remain `Inspect-WallRun250-Writers.ps1`; update its preflight text/tests only if necessary. Keep Python syntax preflight before EXE analysis.

## Automated validation

Update `source/tests/wallrun_scalar_writer_tool_static.py` to pin:

- both downstream roots;
- entry owner offsets `0x80` and `0x58`;
- corresponding effective direct offsets `0x1D0` and `0x1F8`;
- conservative CFG merge/invalidation behavior being present in source;
- volatile-call clobber behavior;
- no global scan / no process write APIs / no runtime package change.

Run the normal portable/static automated checks available to Codex. No owner game test is part of this task.

## Completion

Return the normal short Codex result only. If implementation/build/static checks succeed, Chat will give one offline owner command to produce a new `MSM2-WS-wallrun250-writers-*.json`.

No Update-WS, Overstrike, install, or game run is required for this implementation task. Current runtime remains `v0.1c47 / 0.1.72`.
