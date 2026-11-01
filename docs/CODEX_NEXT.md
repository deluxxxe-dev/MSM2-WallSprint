# Next task — resolve c47 `WallRun.this+0x250` writer provenance in authenticated direct callees

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`, and current `docs/CODEX_RESULT.md`.

## Accepted runtime facts

Do not repeat the c47 game sequence merely for more scalar samples or a summary.

- exact target build and authenticated WallRun lifecycle/Update accepted;
- scalar session actually armed with legacy surface telemetry disabled;
- `+0x250` repeatedly changes during verified WallRun and is temporally associated with both ordinary direction reversals and the later down-hold left/right oscillation;
- both reversal directions use negative pulses, so direct signed left/right-input semantics are not supported;
- exact meaning, units, producer, writer, owner-at-writer provenance and writability remain UNKNOWN.

Do not revisit c44 RequestSimpleMotion or c45 GravityOrientationOwner for the same ordinary locomotion/WallRun state.

## Accepted static facts

Authenticated WallRun Update:

- RVA `0x1424530..0x14246D6`;
- SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`;
- direct `this+0x250` reads at `0x1424585` and `0x142460E`;
- no accepted direct `+0x250` write in Update.

Two exact direct calls pass the authenticated WallRun owner in entry RCX:

- `0x14245C3 -> 0x367DD10`;
- `0x1424630 -> 0x29BD5F0`.

Entry RCX does not by itself prove owner provenance at later instructions.

## Retired V1 / V2 global scans

V1 owner result: `decoded_candidate_start_cap` from byte-local false instruction-start explosion.

V2 owner result: `writer_cap` from whole-image direct `+0x250` writes. This establishes only that displacement `0x250` is too common for an ownership-agnostic global writer search. Do not increase the writer cap or continue whole-EXE/adjacent-offset enumeration.

Neither refusal is an engine no-writer result.

## Current V3 task

Fast-forward and run only:

- `source/tools/wallrun_scalar_writer_refs.py`;
- Windows bootstrap `source/tools/windows/Inspect-WallRun250-Writers.ps1`.

Expected result:

`MSM2-WS-wallrun250-writers-YYYYMMDD-HHMMSS.json`

No Update-WS, build, deploy, Overstrike or game run is required.

## V3 fixed scope

Decode exactly these three PE runtime-function entries:

1. `0x1424530` — authenticated WallRun Update;
2. `0x367DD10` — authenticated Update direct callee receiving WallRun owner in RCX at entry;
3. `0x29BD5F0` — authenticated Update direct callee receiving WallRun owner in RCX at entry.

Required checks:

- exact Spider-Man2.exe SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- Capstone 5.0.6;
- exact Update SHA/range;
- exact two direct-call prefixes and decoded targets;
- both callee targets must be exact PE runtime-function entries;
- complete contiguous entry-to-end linear decode for all three functions;
- both known Update `+0x250` sites must be rediscovered;
- unexpected direct Update `+0x250` writer must refuse.

For each of the two callees, report:

- function range/unwind/SHA;
- direct memory operands with displacement `+0x250`;
- any direct writer candidates and bounded local context;
- direct call sites/targets from the callee, so a no-direct-writer result still supplies bounded next provenance leads.

Do not promote entry-RCX relation into owner-at-writer proof.

## Explicitly outside V3

- whole-EXE direct-writer enumeration;
- adjacent-field scanning;
- computed-address writers without direct `+0x250` displacement;
- bulk-copy/memcpy effects;
- nested-pointer fields;
- deeper call traversal;
- runtime watchpoints/hooks/page guards;
- any scalar write/A-B modification.

## Acceptance / stop rules

If a direct `+0x250` writer exists in either authenticated owner-entry callee:

- inspect only enough intra-function provenance to determine whether its memory base is still definitely derived from entry RCX on the writer path;
- do not write the field yet.

If neither callee directly writes `+0x250`:

- use only the returned bounded direct-call list to select the next smallest producer/provenance step;
- do not return to global displacement scanning.

If V3 refuses on exact identity/runtime/decode checks, fix only that concrete failure. Do not broaden scope.

Current runtime remains `v0.1c47 / 0.1.72`.
