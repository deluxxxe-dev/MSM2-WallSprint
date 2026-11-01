# Next task — resolve direct writer provenance for c47 `WallRun.this+0x250`

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`, and current `docs/CODEX_RESULT.md`.

## Accepted runtime results

Do not repeat the same c47 game sequence merely to obtain more scalar samples or a summary.

Accepted c47 facts:

- exact target build and authenticated WallRun lifecycle/Update accepted;
- scalar session actually armed with `surface_telemetry=0`;
- all five direct fields were readable at first sample;
- `+0x208` changed once from `0.25` to about `1e30`;
- `+0x220`, `+0x238`, `+0x40C` emitted no retained change before the change-log cap;
- `+0x250` repeatedly produced bounded negative pulses temporally associated with right/left reversals and the owner's down-hold left/right oscillation;
- no retained scalar sample exactly matched `0.4/12/75/100/50`;
- direct signed left/right-input semantics are not supported because both reversal directions produced negative `+0x250` pulses;
- field meaning, units, producer/writer, owner provenance and writability remain UNKNOWN.

The missing `wallrun_update_scalar_summary` is explained by the existing emergency Update-timeout lifecycle close with `Deinit=0`. Do not rerun c47 solely for summary output.

Do not revisit c44 RequestSimpleMotion or c45 GravityOrientationOwner presence for the same ordinary locomotion/WallRun state.

## Static facts already accepted

Authenticated WallRun Update:

- RVA `0x1424530..0x14246D6`;
- SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`;
- direct `this+0x250` reads at `0x1424585` and `0x142460E`;
- no direct `+0x250` write in this exact Update body.

Two exact Update call sites pass the WallRun owner into entry RCX:

- `0x14245C3 -> 0x367DD10`;
- `0x1424630 -> 0x29BD5F0`.

That proves only call-entry RCX relation. It does not prove owner provenance at a later callee instruction.

## Owner V1 offline result

The first owner-local direct-writer scan refused with:

`decoded_candidate_start_cap`

Do not interpret this as no writer. The V1 byte prefilter guessed up to 14 instruction starts around every raw literal `0x250` occurrence and exhausted its false-candidate cap before the fixed analysis completed.

The Capstone bootstrap quoting issue was also corrected earlier; current wrapper uses integer tuple version comparison and is not blocked on that issue.

## Current V2 task

Fast-forward and rerun only:

- `source/tools/wallrun_scalar_writer_refs.py`;
- Windows bootstrap `source/tools/windows/Inspect-WallRun250-Writers.ps1`.

Expected owner result file:

`MSM2-WS-wallrun250-writers-YYYYMMDD-HHMMSS.json`

No Update-WS, build, deploy, Overstrike or game run is needed.

## V2 fixed scan contract

The scan:

1. requires exact Spider-Man2.exe SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
2. requires pinned Capstone 5.0.6;
3. re-authenticates exact Update SHA and the two owner-RCX direct-call prefixes;
4. scans executable file bytes only for raw literal `0x00000250` as a **function prefilter**;
5. maps each raw hit to its containing PE runtime-function range; no nearby instruction-start guessing is allowed;
6. decodes each selected runtime function exactly once and accepts references only from a complete contiguous entry-to-end linear decode;
7. requires the two known Update `+0x250` sites as an internal self-check;
8. counts all confirmed direct displacement references but retains output only for direct writers, the two known Update references, and references inside the two authenticated owner-RCX callee entries;
9. reports direct writer candidates, containing runtime function identity/hash, memory base/index/width/access, and owner-callee entry relation without promoting it to owner-at-writer proof.

Fail-closed bounds include:

- executable prefilter bytes <= 192 MiB;
- raw displacement hits <= 200,000;
- candidate runtime functions <= 16,384;
- individual function <= 256 KiB;
- aggregate decoded candidate-function bytes <= 96 MiB;
- retained references <= 1,024;
- direct writers <= 512;
- output <= 512 KiB.

Explicitly outside scope:

- computed-address writers without literal `+0x250` displacement;
- bulk-copy/memcpy effects that do not directly encode the field displacement;
- nested-pointer fields;
- runtime value capture;
- hooks/watchpoints/page guards/debug registers;
- scalar writes or A/B modifications;
- adjacent-field scans.

## Acceptance / stop rules

If a direct writer is found inside one of the authenticated owner-RCX callee runtime functions:

- inspect its intra-function provenance only far enough to decide whether the writer's base register is still proven to derive from entry RCX on that path;
- do not assume owner merely because the memory base register is named RCX;
- do not write the field yet.

If writers are found only in unrelated functions:

- retain them as displacement-only candidates;
- do not equate them with WallRun owner fields;
- return to Chat for a smaller relation/provenance discriminator.

If no direct writer is found:

- accept only that this fixed direct-displacement scan did not identify the producer;
- stop rather than broad-scanning adjacent offsets or immediately adding a live watchpoint;
- Chat chooses whether computed-address/bulk-copy provenance or another authenticated consumer path is the next minimum proof.

No runtime/package version bump is authorized for this offline step. Current runtime remains `v0.1c47 / 0.1.72`.
