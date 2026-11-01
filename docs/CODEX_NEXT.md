# Next task — run V8 bounded Update-consumer trace for c47 `WallRun.this+0x250`

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/C47_WALLRUN250_RUNTIME_RESULT_20261101.md`, `analysis/WALLRUN250_V6_OWNER_RESULT_20261101.md`, `analysis/WALLRUN250_V7_HANDOFF_RESULT_20261101.md`, and current `docs/CODEX_RESULT.md`.

## Accepted evidence

Do not repeat c47 gameplay, RequestSimpleMotion, GravityOrientationOwner, V1/V2 global scans, or V3-V7 producer-path traversal.

Accepted runtime:

- `WallRun.this+0x250` changes during verified WallRun;
- changes correlate with ordinary reversals and down-hold left/right oscillation;
- direct signed left/right-input semantics are unsupported;
- field meaning/units/producer/writability remain UNKNOWN.

Accepted producer-path result:

- Path A `0x29C06F0`: no `owner+0x250` access and no next direct call/tail lead;
- Path B `0x29C18B0`: one owner-preserving call `0x29C18D4 -> 0x308A0F0`;
- V7 target `0x308A0F0`: 8 reachable instructions, no `owner+0x250` access, no direct call/tail lead;
- the selected authenticated RCX-owner chain is exhausted.

Do not interpret this as proof that the field has no writer.

## Current V8 implementation

V8 changes question from producer to consumer.

Run:

`source/tools/windows/Inspect-WallRun250-Consumers.ps1`

It stages:

- `source/tools/wallrun250_consumer_v8.py`;
- `source/tools/wallrun_scalar_writer_refs.py` only for exact-image/Update authentication;
- `source/tools/discover_ground_support.py` PE helper.

V8 inspects exactly the two authenticated Update read sites:

- `0x1424585`;
- `0x142460E`.

For each site it records:

- exact read instruction;
- six preceding Update instructions;
- max 32 reachable Update instructions from the read;
- register read/write metadata;
- memory operand width/access metadata;
- direct branch/call targets without callee traversal;
- file-backed RIP-relative operand initial bytes and 32/64-bit numeric/float decode when available.

Important: on-disk RIP-relative values are initial file bytes only, not verified live runtime values.

Expected schema: 8.

Output:

`MSM2-WS-wallrun250-consumers-YYYYMMDD-HHMMSS.json`

## Owner action

No Update-WS, build, deploy, Overstrike or game run is required.

Fast-forward the branch and run only `Inspect-WallRun250-Consumers.ps1`, then return the new consumer JSON.

## Result handling

Use V8 to determine only what conditions/control flow the two authenticated reads participate in.

- If both reads feed the same kind of threshold/control decision, classify that structural consumer relation as VERIFIED-STATIC and then decide whether one minimal runtime discriminator is justified.
- If they feed distinct consumers, keep both interpretations separate.
- Do not infer a writer from consumer use.
- Do not authorize a game write from V8 alone.
- Do not return to global writer scanning merely because the producer remains unknown.

Current runtime remains `v0.1c47 / 0.1.72`; no runtime rebuild or gameplay test is part of this static step.
