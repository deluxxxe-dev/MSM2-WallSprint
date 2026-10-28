# Codex result — c68 owner OFFLINE result accepted

2026-10-28 JST. Branch **codex/v01c20-edi-value-provenance**.

## Status

C68 owner OFFLINE result is accepted as **VERIFIED-STATIC**.

Canonical detail:

- `analysis/V01C68_TOOL_READY.md`
- `analysis/V01C68_OWNER_OFFLINE_RESULT.md`

Accepted artifact:

```text
MSM2-WS-v0.1c68-post-c65-stitched.json
physical SHA256 47fee13729d1b2d30b1c3cffc9f4bc19837fc18f580d0c0daac909ea7056a246
size 13862 bytes
repository_result POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c68 / schema 1
contract exact_c67_full_prefix_terminal_reason_provenance_all_successor_continuation_v1
semantic result identity dbfcdefeccbc7bcaba048829336d945707f0fc4fc76b2c17780f33413b48255a
```

The owner log completed with both `POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY` and `C68_OWNER_OFFLINE_PASS`.

## Result

C68 authenticated the exact accepted c67 stream and measured actual all-successor completion rather than inferring roots from edge totals:

```text
c67 instances considered/admitted/refused = 458 / 180 / 278
c67 exact edges considered/admitted/refused = 432 / 221 / 211
```

Known non-exact c67 successors remained refused:

```text
inside_unique_runtime_span_nonentry = 74
executable_file_backed_outside_runtime_metadata = 1
```

No c67 certificate mismatch was observed.

Root/continuation totals:

```text
attempted = 162472
completed = 105721
refused = 56751
rows reached = 1750
address facts crossing rows = 3161
```

The newly exposed bounded unknown wave is:

```text
uncertified_post_c67_runtime_boundary = 191
```

## Candidate/safety result

```text
reference_candidate_count = 0
new_cross_row_candidate_count = 0
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
live_qword_reads = 0
c56_through_c67_evidence_read_only = true
```

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/pointer/game gates remain false. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

Zero candidates never proves writer absence.

## Performance finding

The owner run replayed the whole c56-c68 prefix over 162472 roots. This preserved exact provenance but is unnecessarily expensive when the executable, accepted artifacts, tool sources, schemas/contracts and limits are byte/identity unchanged.

Future tooling should support authenticated-prefix checkpoint reuse. Any mismatch must fail closed or require full replay; the optimization must not weaken evidence semantics.

## Next

`docs/CODEX_NEXT.md` defines c69 as:

1. diagnostic-only classification of the exact 191 first `uncertified_post_c67_runtime_boundary` terminals; and
2. a deterministic authenticated-prefix checkpoint/cache mechanism for unchanged accepted c56-c68 inputs.

Do not define c70 continuation behavior until the c69 owner artifact is analyzed.
