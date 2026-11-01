# Result — c47 WallRun scalar runtime accepted; V4 owner-relative provenance ready

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C47 SCALAR OBSERVATION VERIFIED-RUNTIME / `WallRun.this+0x250` SELECTED AS CORRELATED DYNAMIC CANDIDATE / V1+V2 GLOBAL SEARCHES RETIRED / V3 AUTHENTICATED OWNER-CALLEE SCAN ACCEPTED WITH NO DIRECT +0x250 WRITER / V4 TWO-ROOT OWNER-RELATIVE CFG PROVENANCE IMPLEMENTED / OWNER OFFLINE JSON PENDING / NO WRITE AUTHORIZED.**

## Accepted evidence

C47 verified that `WallRun.this+0x250` changes during actual WallRun and is temporally associated with both ordinary reversals and the later down-hold left/right oscillation. Both reversal directions use negative pulses, so direct signed left/right-input semantics remain unsupported. Field meaning, units, producer and writability are still UNKNOWN.

V3 owner JSON `MSM2-WS-wallrun250-writers-20261101-092733.json` successfully revalidated the exact target image and found no direct encoded `+0x250` writer in WallRun Update, `0x367DD10`, or `0x29BD5F0`.

The accepted transformed owner roots are:

- Path A: `0x367DD2D sub rcx,-0x80`; `0x367DD31 -> 0x29C06F0`; downstream entry RCX = owner+`0x80`; original owner+`0x250` corresponds to relative `+0x1D0` before further transforms.
- Path B: `0x367DD38 add rcx,0x58`; `0x367DD3C -> 0x29C18B0`; downstream entry RCX = owner+`0x58`; original owner+`0x250` corresponds to relative `+0x1F8` before further transforms.

Detailed runtime/V3 evidence remains in:

- `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`
- `analysis/WALLRUN250_V3_OWNER_CALLEE_RESULT_20261101.md`

## V4 implementation

`source/tools/wallrun_scalar_writer_refs.py` is now schema 4 and performs only bounded static owner-relative GPR provenance from the two accepted downstream roots.

Admission/identity checks retained or added:

- exact Spider-Man2.exe SHA guard;
- Capstone 5.0.6 guard;
- exact authenticated WallRun Update range/SHA and known `+0x250` reads;
- exact `0x367DD10..0x367DD5F` SHA from accepted V3;
- exact Path A/B adjust bytes, call bytes and decoded call targets;
- arithmetic invariant `0x250-entry_owner_offset == 0x1D0/0x1F8`;
- downstream target must lie in a PE runtime function and be an exact decoded instruction boundary;
- containing runtime function must decode contiguously entry-to-end.

Provenance model:

- start A with `RCX = owner+0x80` at `0x29C06F0`;
- start B with `RCX = owner+0x58` at `0x29C18B0`;
- track only full-64-bit register MOV;
- track full-64-bit LEA of known base + constant with no index;
- track full-64-bit ADD/SUB immediate;
- kill destination provenance on partial/unsupported writes or memory-loaded destinations;
- CFG merge is exact intersection: a register survives only when all incoming paths prove the same owner-relative offset;
- every CALL kills Win64 volatile GPR provenance (`RAX/RCX/RDX/R8-R11`) on return;
- reachable indirect JMP refuses rather than silently dropping a possible path;
- deeper callees are not traversed.

Output reports:

- only entry-reachable instructions from each downstream root;
- memory operands whose current proven base plus encoded displacement resolves exactly to original owner+`0x250`;
- read/write/access-unknown metadata;
- reachable direct calls with zero-or-one uniquely proven RCX owner offset at the call site;
- containing runtime function range/unwind/SHA and trace counts.

## Files changed for V4

- `source/tools/wallrun_scalar_writer_refs.py`
- `source/tools/windows/Inspect-WallRun250-Writers.ps1`
- `source/tests/wallrun_scalar_writer_tool_static.py`
- `docs/CODEX_RESULT.md`
- `docs/CODEX_NEXT.md`

The existing `source/tools/test.sh` already invokes `wallrun_scalar_writer_tool_static.py`.

## Validation performed here

Before publication, the V4 Python helper passed `py_compile`/AST syntax checks. An equivalent local static-contract check passed for the two roots, entry offsets, effective offsets, conservative CFG merge, volatile-call invalidation, no global scan, and unchanged c47 version/package.

The Windows wrapper also performs a pinned Capstone version check and V4 `py_compile` preflight before reading the EXE.

Actual execution against the owner's pinned Spider-Man2.exe is still required to obtain the V4 result. This is an offline read-only operation, not a game test.

## Safety / active runtime

Current runtime remains **v0.1c47 / package 0.1.72**.

- runtime/gameplay code unchanged;
- all gameplay writes remain OFF;
- no new hook/watchpoint/page guard;
- no global executable writer scan;
- no adjacent-field scan;
- no memory-load pointer inference;
- no SocialFeed or other MOD change;
- no Update-WS/build/deploy/game action required for V4 acquisition.
