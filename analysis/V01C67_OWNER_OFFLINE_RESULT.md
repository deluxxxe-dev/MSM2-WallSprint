# V01C67 OWNER OFFLINE RESULT

Date: 2026-10-28 JST  
Branch: `codex/v01c20-edi-value-provenance`  
Evidence class: **VERIFIED-STATIC** owner OFFLINE result.

## Artifact

Owner result file:

`MSM2-WS-v0.1c67-post-c65-runtime-boundary.json`

Physical file SHA256:

`277cb83c3fbe36715876133079c99a1d5151634442ed842960a917b140ab8aee`

Size: `600196` bytes.

Canonical compact result identity (SHA256 of sorted compact JSON encoding):

`ac043225b8b4464e8fe7e52a2c44a9a81c72bf0c2fad6e30df173fdab83b113d`

Repository result:

`POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY`

Tool/schema/contract:

- tool `v0.1c67`
- schema `1`
- contract `exact_c66_first_terminal_post_c65_boundary_full_prefix_provenance_diagnostic_v1`

Exact image authentication passed for:

`c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`

Classifier identity was independently recomputed from `classifier_header` and matches:

`a130b082512449cd008a912b94e2e3a2bcabf15140906c4780983bac07b99f20`

Authenticated record-set SHA256:

`f0b2ce8c91868e572a83d59fc425c4e40d3fe4fe9faa7957e26a252b65e85ed6`

The corrected c67 tool source identity includes the c67-only lower-capture fix. The accepted c56-c66 chain remained read-only.

## Accepted chain authentication

Accepted c66 identities in the c67 artifact:

- c66 authentication identity: `9b333e3e4b881f427be02fb7dfcea2223b27d28798b37d362df0a38dccb9d021`
- c66 result identity: `444152457993414e32b0050da58e9bc40ee6672ce5d67398b4ba7360791c2eea`

Accepted c65 identities remain:

- classifier: `e213e4e137efb57f36a3a716280ed9c83f9641e599fed26daeb2bedcc758bb99`
- record set: `dd2964a012eff48f01dfd2f3d49b6acb70e7e40ba44cf2cda3534f1f9e2eff22`
- result: `dd4ff3451faa9735f88756e16222a36fc80cafbf51f113bc12f6f06b6dc62780`

All earlier accepted c57-c64 identities are present and consistent with the accepted chain.

## Classification completeness

Exact c66 terminal domain:

```text
expected roots   = 458
classified roots = 458
instances        = 458
unique states    = 458
unique source rows = 445
```

`classification_complete=true`.

Boundary edge totals:

```text
post-c65 boundary edges = 507
architectural edges     = 507
model-only edges        = 0
```

Edge kinds:

```text
ordinary fallthrough          = 141
conditional fallthrough       = 51
direct conditional taken      = 211
direct unconditional branch   = 102
call fallthrough              = 2
other supported direct        = 0
```

## Target relation distribution

```text
exact_unique_runtime_begin                      = 432
inside_unique_runtime_span_nonentry             = 74
executable_file_backed_outside_runtime_metadata = 1
all other relation classes                      = 0
```

Exact-edge kinds:

```text
ordinary fallthrough          = 141
conditional fallthrough       = 51
direct conditional taken      = 197
direct unconditional branch   = 41
call fallthrough              = 2
```

Important: `432`, `74`, and `1` are **edge counts**, not unique-root counts. C68 must retain all-successor fail-closed semantics; the number of roots that can complete cannot be inferred by subtracting these edge totals.

## Terminal provenance

C67 terminal phase counts:

```text
same_instance_after_c65_preflight_before_row_fixpoint = 348
post_c65_reachable_context                            = 34
post_c65_reachable_dedup_stored_earlier_path          = 3
prior_c65_authorization_converted_earlier_refusal     = 73
```

Terminal reason counts:

```text
used_c65_source_later_instruction          = 348
current_context_post_c65_reachable          = 37
prior_c65_authorization_converted_c64_post = 73
```

Underlying refusal counts:

```text
uncertified_post_c63_runtime_boundary = 73
```

Those 73 converted cases are an authenticated provenance class. They must not be relabeled as natural post-c65 reachability or borrow a different lower authorization.

Nested c65 authorization phase counts:

```text
same_instance_after_c63_preflight_before_row_fixpoint = 364
post_c63_reachable_context                            = 31
post_c63_reachable_dedup_stored_earlier_path          = 1
prior_c63_authorization_converted_earlier_refusal     = 62
```

Nested c65 authorization reasons:

```text
used_c63_source_later_instruction          = 364
current_context_post_c63_reachable          = 32
prior_c63_authorization_converted_c62_post = 62
```

## Candidate and safety result

C67 is diagnostic only:

```text
new_post_c65_boundaries_admitted = 0
new_cross_row_candidate_count    = 0
cross_runtime_fact_propagation_unchanged = true
c66_candidate_output_unchanged = true
```

Historical c66 candidates remain all zero and the candidate output SHA256 remains:

`4f53cda18c2baa0c0354bb5f9a3ecbe5ed12ab4d8e11ba873c2f11161202b945`

No live qword reads occurred. `c56_through_c66_evidence_read_only=true`.

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

Zero candidates does not prove writer absence because candidate sweep and writer enumeration remain incomplete.

## Interpretation / next bounded task

C67 is accepted as **VERIFIED-STATIC**. The prior c67 implementation blocker was a diagnostic observation bug and is resolved by the c67-only correction; accepted c66 evidence was not invalidated.

The smallest next task is c68: replay/authenticate the exact accepted c67 records and allow continuation only through complete architectural successor sets whose every edge is `exact_unique_runtime_begin`, with exact source/target/provenance checks and root-level rollback on any bad arm. The 74 interior and 1 metadata-free-RX edges remain fail-closed diagnostics.

C68 must not infer an admitted-root count from aggregate edge counts. The actual next unknown wave and any new candidate observations must be measured by the c68 owner OFFLINE run.
