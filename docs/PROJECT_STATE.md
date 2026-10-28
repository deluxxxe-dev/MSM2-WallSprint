# WallSprint project state

## Current stage — corrected c69 owner OFFLINE result accepted; c70 checkpoint-consuming continuation next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, c68, and corrected c69 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical detail:

- `analysis/V01C66_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C67_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C67_CORRECTED_OWNER_RUN.md`
- `analysis/V01C67_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C68_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C69_TOOL_READY.md`
- `analysis/V01C69_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C69_OWNER_CORRECTION_READY.md`
- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`

## Accepted c69 artifacts

Result:

```text
file: MSM2-WS-v0.1c69-post-c67-runtime-boundary.json
physical SHA256: e57b7f39ea476e356397f944d8aadb14841b27874736a60c1a82b982eb86537f
size: 683026 bytes
repository_result: POST_C67_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool: v0.1c69 / schema 1
contract: exact_c68_first_terminal_post_c67_boundary_full_prefix_provenance_diagnostic_v1
classifier identity: 4f42b0d04346eb5c7b10c8fe0592edd3a3f19fcb463d66a0adf40be01fd25049
semantic result identity: 08002e4c3aef2c7cfa037040b6f09f7d1b6e247d0b446daea1ffdb2569aa01fb
authenticated record set: 20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed
```

Checkpoint:

```text
file: MSM2-WS-v0.1c69-authenticated-prefix-checkpoint.json
physical SHA256: d7b8dae21e9e0478e8ea5d8136c7f19156cd5afca2ff80f167dce48a2e61b6b5
size: 3506330 bytes
checkpoint contract: authenticated_c68_prefix_first_post_c67_records_v1
checkpoint identity: b734f8ab2f0a24e002c8b5c2ed1dbbfe75c9411cc773ec1394a1cf330c9b3a28
record count: 191
record stream: 20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed
```

Exact image remains authenticated:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

Accepted c68 identities remain exact.

## C69 measured terminal classification

The accepted c68 next unknown wave was exactly 191 `uncertified_post_c67_runtime_boundary` roots. C69 classified all 191 without admission or traversal.

Owner data disproved the original assumption that every c68 POST is raised from an instruction-edge callback. The corrected classifier distinguishes edge-backed and transaction-level terminal origins without inventing missing instruction/edge evidence.

```text
expected/classified terminal instances = 191 / 191
unique entry-instance states           = 191
unique terminal context/source rows    = 186
edge-backed terminal instances         = 190
transaction terminal instances         = 1
classification complete                = true
terminal-origin classification complete = true
```

The one transaction terminal is:

```text
root = 164519
current row = 164524
event = instance
reason = transaction_instance_converted_uncertified_post_c65_runtime_boundary
source instruction = none
successor edges = none
```

It remains fail-closed.

## C69 edge relations

The 190 edge-backed terminals expose 209 architectural edges:

```text
exact_unique_runtime_begin          = 196
inside_unique_runtime_span_nonentry = 13
all other relation classes          = 0
```

Per-terminal complete-successor grouping from the accepted record stream:

```text
177 terminal instances have every architectural successor at exact runtime begin
191 exact edges belong to those 177 complete instances
13 edge-backed instances contain at least one interior successor
1 transaction-only terminal has no edge to continue
```

The 13 non-exact instances are:

```text
8: one interior successor only
5: one exact successor + one interior successor
```

The five mixed instances are not partially admissible. The whole terminal remains fail-closed when any successor is non-exact.

## Candidates and safety

C69 is diagnostic-only:

```text
new post-c67 boundaries admitted = 0
new candidates = 0
c68 candidate output unchanged = true
c56-through-c68 evidence read-only = true
live qword reads = 0
```

Historical c68 candidate counts remain zero. Zero candidates does not prove writer absence.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

The following remain false/unproven:

```text
candidate_sweep_complete
writer_enumeration_complete
allocation_proven
base_proven
range_proven
lifetime_proven
ownership_exclusive
destination_contained
CFG_continuation
alternate_entries_EH_indirect_writers_closed
production_integrated
hardware_watch_enabled
pointer_chase
game_used
```

Runtime remains `v0.1c38`; c53-c70 work is OFFLINE/STATIC unless separately stated. Do not modify SocialFeed or other MOD repositories.

## Performance/checkpoint status

The c69 checkpoint is accepted and internally authenticated. However, only checkpoint **creation/validation** is owner-proven so far. A later stage has not yet demonstrated consuming it instead of replaying c56-c68.

Do not claim historical replay has already been eliminated.

## Current bounded action

C70 is the smallest checkpoint-consuming continuation stage and must:

- authenticate exact executable + accepted c69 result/checkpoint + tool/schema/contract/limits identities;
- prove checkpoint-seeded behavior equals the corresponding full-replay reference on owned fixtures before owner use;
- consider only the 177 c69 terminal instances whose entire architectural successor set is `exact_unique_runtime_begin`;
- preflight all successors of one terminal atomically; no partial admission of mixed-successor terminals;
- keep all 13 interior-containing terminals and the one transaction terminal fail-closed;
- continue from exact authenticated successor states/paths only;
- stop at the first new post-c69 unknown boundary;
- preserve all earlier provenance and all safety gates.

Do not define behavior beyond that first new boundary until the c70 owner result is analyzed.
