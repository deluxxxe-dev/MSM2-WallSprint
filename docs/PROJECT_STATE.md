# WallSprint project state

## Current stage — c68 owner OFFLINE result accepted; c69 diagnostic/checkpoint work next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, and c68 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical detail:

- `analysis/V01C66_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C67_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C67_CORRECTED_OWNER_RUN.md`
- `analysis/V01C67_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C68_TOOL_READY.md`
- `analysis/V01C68_OWNER_OFFLINE_RESULT.md`

## Accepted c68 artifact

```text
file: MSM2-WS-v0.1c68-post-c65-stitched.json
physical SHA256: 47fee13729d1b2d30b1c3cffc9f4bc19837fc18f580d0c0daac909ea7056a246
size: 13862 bytes
repository_result: POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool: v0.1c68 / schema 1
contract: exact_c67_full_prefix_terminal_reason_provenance_all_successor_continuation_v1
semantic result identity: dbfcdefeccbc7bcaba048829336d945707f0fc4fc76b2c17780f33413b48255a
```

Exact image remains authenticated:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

Accepted c67 identities remained exact and c67 certificate mismatch counts were empty.

## C68 measured continuation

```text
c67 instances available/considered = 458 / 458
admitted/refused                    = 180 / 278
c67 exact edges available/considered = 432 / 432
exact admitted/refused                = 221 / 211
```

Known non-exact c67 successor relations stayed fail-closed:

```text
inside_unique_runtime_span_nonentry             = 74
executable_file_backed_outside_runtime_metadata = 1
```

Root totals:

```text
attempted = 162472
completed = 105721
refused   = 56751
rows reached = 1750
address facts crossing rows = 3161
```

Relative to accepted c66, c68 completed 180 additional roots, reached 67 additional rows, and carried 117 additional address facts.

The exact next new fail-closed wave is:

```text
uncertified_post_c67_runtime_boundary = 191
```

Candidates remain zero:

```text
reference = 0
READ = 0
WRITE = 0
RMW = 0
ADDRESS_ONLY = 0
```

Zero candidates does not prove writer absence.

## Performance constraint discovered during owner run

The c68 owner command replayed c56 through c68 over the full 162472-root domain. This is valid but too expensive to repeat mechanically for every later stage.

The next tooling must add a deterministic authenticated-prefix checkpoint/cache path. Reuse is allowed only when the executable hash, accepted artifact physical/semantic identities, tool-source identities, schema/contracts and limits are exactly unchanged. Any mismatch must fail closed or require full replay; no weaker provenance acceptance is permitted.

This is a tooling optimization only. It does not change evidence class, admit additional roots, or weaken c56-c68 proofs.

## Current bounded action

C69 is diagnostic-only and must:

- classify exactly the 191 first `uncertified_post_c67_runtime_boundary` terminals exposed by accepted c68;
- preserve exact current root/state/path/source/full-prefix and all c67/c68 provenance;
- use the established boundary-classifier relation vocabulary;
- perform no target traversal, no new Fact propagation and no candidate admission;
- keep all existing safety gates closed;
- add/test authenticated-prefix checkpoint reuse for unchanged accepted c56-c68 inputs so future owner runs need not replay the full historical chain.

Do not define c70 continuation behavior until the c69 owner artifact is analyzed.

## Safety

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

Runtime remains `v0.1c38`; c53-c69 are OFFLINE/STATIC analysis tooling unless separately stated. Do not modify SocialFeed or other MOD repositories.
