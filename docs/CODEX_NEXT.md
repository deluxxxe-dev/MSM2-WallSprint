# Next task — inspect current-build vanilla WallRun steering configs before any c73 direct-CFG continuation

## Status

Corrected c72 owner OFFLINE result is accepted as **VERIFIED-STATIC**.

Accepted c72 artifact:

```text
MSM2-WS-v0.1c72-post-c71-stitched.json
SHA256 077955562f1cdd9ba966fd1104ecd2b3b258f3aa25384418032fa5099149d32d
repository_result POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
```

Key result:

```text
c71 records = 102/102
all-successor-exact = 95/95
exact edges = 99/99/99
post_c71_uncertified_boundary_refusals = 54
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
live_qword_reads = 0
```

Do **not** automatically define c73 as another direct-CFG continuation. The direct route is paused pending a materially different investigation.

Canonical detail:

- `analysis/V01C72_OWNER_OFFLINE_RESULT.md`
- `analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md`
- `docs/PROJECT_STATE.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Purpose

Determine whether the authenticated current game build exposes useful WallRun steering behavior through vanilla config assets before spending another version on the remaining 54 direct-CFG frontier observations.

This is a **read-only/static asset investigation first**.

## Primary current-build assets

Obtain or extract these exact vanilla resources from the current game build:

```text
configs/hero/hero_WallRunSteeringSetup_Standard.config
  asset id 948FA063808F7E18

configs/hero/hero_WallRunSteeringSetup_Indoor.config
  asset id 83DF4507531061B1
```

Secondary comparison assets, only after the steering assets are available:

```text
configs/hero/hero_WallRunConfig.config
  asset id 86DF98B17F4D766D

configs/hero/hero_WallRunSpeedSetup_Standard.config
  asset id 80D25C57AC09BE31

configs/hero/hero_WallRunJumpSetup_Standard.config
  asset id B78A8FA3857CFB17
```

The asset IDs are locator aids derived from public DAT1 path hashing and reference-MOD resource names. Current-build asset presence/content must be independently established.

## Task

1. Locate/extract the current-build vanilla Standard and Indoor WallRun steering configs.
2. Record their exact file SHA256 identities and extraction source/build context.
3. Decode them using the already documented DAT1 config format or an equivalent independently verifiable decoder.
4. Record the decoded config type and a compact inventory of field/group names and values relevant to direction, steering, surface-relative movement, momentum, turn rates, thresholds or wall orientation.
5. Compare Standard vs Indoor structurally. Do not assign semantics beyond names/data without evidence.
6. Only if useful, decode the current-build WallRunConfig/SpeedSetup/JumpSetup resources to establish references/defaults and vanilla-vs-reference differences.
7. Decide whether one **minimal asset-only experiment** could test a single steering quantity. Do not perform that experiment unless separately authorized after review.
8. Preserve reusable engine/config facts in `MSM2-Wisdoms` when independently established from current-build vanilla evidence.

## Allowed changes

- WallSprint analysis/documentation.
- Small WallSprint-owned read-only extraction/decoding helper if genuinely required.
- Static tests for such a helper.
- Wisdoms documentation for reusable facts after current-build verification.

## Do not

- Do not define or implement c73 direct-CFG continuation yet.
- Do not edit/deploy vanilla config assets in this task.
- Do not copy third-party MOD config values into production as truth.
- Do not import third-party binary code, offsets, signatures or ABI declarations.
- Do not add native hooks, qword reads, pointer chase, game probes or gameplay writes.
- Do not weaken C52 or any existing safety/admission gate.
- Do not modify SocialFeed or any other MOD repository.

## Completion conditions

Report only after establishing one of the following:

### Success

- current-build Standard steering config obtained and SHA256 recorded;
- preferably Indoor steering config also obtained and SHA256 recorded;
- decoded type/fields recorded with evidence distinction;
- a clear conclusion whether a minimal asset-only steering experiment is worth attempting next;
- detailed evidence persisted in WallSprint and reusable current-build facts synchronized to Wisdoms when applicable.

### Blocked

If the environment cannot access the current-build game assets, stop without substituting third-party files. Report the exact owner-supplied files needed:

```text
hero_WallRunSteeringSetup_Standard.config
hero_WallRunSteeringSetup_Indoor.config
```

Optionally request the three secondary vanilla configs only after steering inspection requires them.

## Binding safety

Runtime remains `v0.1c38`. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.
