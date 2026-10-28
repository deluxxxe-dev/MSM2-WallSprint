# V01C68 owner OFFLINE result

2026-10-28 JST. Branch `codex/v01c20-edi-value-provenance`.

Evidence class: **VERIFIED-STATIC**.

## Artifact

```text
file: MSM2-WS-v0.1c68-post-c65-stitched.json
physical SHA256: 47fee13729d1b2d30b1c3cffc9f4bc19837fc18f580d0c0daac909ea7056a246
size: 13862 bytes
repository_result: POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool: v0.1c68
schema: 1
contract: exact_c67_full_prefix_terminal_reason_provenance_all_successor_continuation_v1
semantic result identity: dbfcdefeccbc7bcaba048829336d945707f0fc4fc76b2c17780f33413b48255a
```

Exact image remained authenticated:

```text
Spider-Man2.exe SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

Accepted c67 identities matched exactly:

```text
classifier_identity: a130b082512449cd008a912b94e2e3a2bcabf15140906c4780983bac07b99f20
record_set_identity: f0b2ce8c91868e572a83d59fc425c4e40d3fe4fe9faa7957e26a252b65e85ed6
result_identity: ac043225b8b4464e8fe7e52a2c44a9a81c72bf0c2fad6e30df173fdab83b113d
```

`c56_through_c67_evidence_read_only = true`, `live_qword_reads = 0`, and c67 certificate mismatch counts are empty.

## C67 continuation result

```text
c67 boundary instances available   = 458
considered                         = 458
admitted                           = 180
refused                            = 278

c67 architectural edges available = 507
c67 exact edges available          = 432
exact considered                   = 432
exact admitted                     = 221
exact refused                      = 211
```

All-successor preflight refused the known non-exact c67 edge classes without promotion:

```text
inside_unique_runtime_span_nonentry             = 74
executable_file_backed_outside_runtime_metadata = 1
```

No c67 record/provenance mismatch was observed.

## Preserved c67 phase/reason provenance

Considered phases:

```text
same_instance_after_c65_preflight_before_row_fixpoint = 348
post_c65_reachable_context                            = 34
post_c65_reachable_dedup_stored_earlier_path          = 3
prior_c65_authorization_converted_earlier_refusal     = 73
```

Admitted phases:

```text
same_instance_after_c65_preflight_before_row_fixpoint = 165
post_c65_reachable_context                            = 9
post_c65_reachable_dedup_stored_earlier_path          = 1
prior_c65_authorization_converted_earlier_refusal     = 5
```

Admitted reasons:

```text
used_c65_source_later_instruction          = 165
current_context_post_c65_reachable         = 10
prior_c65_authorization_converted_c64_post = 5
```

The converted-c64 cases remain a distinct historical authorization class and were not relabeled as current reachability.

## Root/continuation result

```text
root transactions attempted = 162472
completed                   = 105721
refused                     = 56751
rows reached                = 1750
address facts crossing rows = 3161
continuation instances created = 9189
deduplicated                   = 111
invalidated/refused             = 6516
cross-row cap refusals          = 46
```

Relative to accepted c66, c68 completed 180 additional roots, reached 67 additional rows, and carried 117 additional address facts.

The next newly exposed fail-closed wave is:

```text
uncertified_post_c67_runtime_boundary = 191
```

This is the exact next bounded unknown class for a diagnostic classifier. It is not proof of writer presence or absence.

## Candidates

```text
reference_candidate_count   = 0
new_cross_row_candidate_count = 0
reader_candidates            = 0
writer_candidates            = 0
rmw_candidates               = 0
address_only_candidates      = 0
```

Historical c66 candidate counts remain zero and its retained candidate output SHA256 remains:

`4f53cda18c2baa0c0354bb5f9a3ecbe5ed12ab4d8e11ba873c2f11161202b945`

Zero candidates does not prove writer absence.

## Safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/pointer/game gates remain false. No game was run, no slot qword was read, no pointer chase or gameplay/production write occurred.

## Performance observation

The owner run replayed c56 through c68 over the full 162472-root domain. This is correct but unnecessarily expensive when all accepted predecessor artifacts, executable identity and tool-source identities are unchanged.

The next tooling step should preserve fail-closed semantics while introducing a deterministic authenticated checkpoint/cache boundary: unchanged exact input hashes and accepted semantic identities may reuse the previously accepted prefix; any mismatch must fall back to/refuse without silently trusting stale data. This optimization must not alter evidence semantics or permit weaker provenance checks.

## Next

Define c69 as a diagnostic-only classifier for the exact 191 first `uncertified_post_c67_runtime_boundary` terminals, while also adding the authenticated-prefix checkpoint mechanism so future owner runs do not replay c56-c68 from scratch when the entire accepted prefix is byte/identity identical.
