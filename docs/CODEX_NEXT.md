# Next task — c68 owner OFFLINE scan

## Status

C68 implementation and automated validation are **TOOL READY**.

Canonical detail:

- `analysis/V01C67_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C68_TOOL_READY.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c67 input:

```text
MSM2-WS-v0.1c67-post-c65-runtime-boundary.json
SHA256 277cb83c3fbe36715876133079c99a1d5151634442ed842960a917b140ab8aee
classifier_identity a130b082512449cd008a912b94e2e3a2bcabf15140906c4780983bac07b99f20
authenticated_record_set_sha256 f0b2ce8c91868e572a83d59fc425c4e40d3fe4fe9faa7957e26a252b65e85ed6
compact result identity ac043225b8b4464e8fe7e52a2c44a9a81c72bf0c2fad6e30df173fdab83b113d
```

C68 contract:

```text
repository_result POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c68 / schema 1
contract exact_c67_full_prefix_terminal_reason_provenance_all_successor_continuation_v1
next unknown uncertified_post_c67_runtime_boundary
```

## Owner action

Run exactly one OFFLINE c68 scan against the exact current executable and accepted c56-c67 artifacts.

Expected output:

```text
logs/MSM2-WS-v0.1c68-post-c65-stitched.json
```

No real game launch is required.

The owner command is maintained in `docs/CODEX_RESULT.md`/ChatGPT handoff. The scan must provide all of these inputs unchanged:

```text
c56 cache
c57 JSON
c58 JSON
c59 JSON
c60 JSON
c61 JSON
c62 JSON
c63 JSON
c64 JSON
c65 JSON
c66 JSON
c67 JSON
Spider-Man2.exe
```

## Acceptance after run

ChatGPT must inspect the exact resulting JSON before any c69 behavior is defined.

At minimum verify:

- `repository_result = POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY`;
- tool `v0.1c68`, schema 1, exact contract;
- exact executable authentication;
- exact accepted c67 physical/semantic authentication;
- c67 instances considered/admitted/refused;
- c67 exact edges considered/admitted/refused;
- phase/reason preservation including converted-c64 provenance;
- all-successor preflight refusal counts;
- completed/refused root totals;
- next `uncertified_post_c67_runtime_boundary` wave;
- rows/Facts/candidate results;
- all safety/completeness/publication/ownership/production/watch/game gates remain closed.

Do not infer any root total from the c67 aggregate 432 exact-edge count.

## Do not

- Do not define or implement c69 before the c68 owner JSON is analyzed.
- Do not widen caps.
- Do not promote interior or metadata-free-RX targets.
- Do not alter accepted c56-c67 evidence.
- Do not read slot qwords or chase pointers.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or another MOD repository.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

Zero candidates never proves writer absence.
