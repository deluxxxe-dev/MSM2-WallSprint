# c50 native Windows result — unique matching debt identity confirmed

Date: 2026-10-27 JST
Branch: `codex/v01c20-edi-value-provenance`
Probe source/artifact head before owner run: `c2cbc4cc82e0f48e58dfdeaf2c34fc6717a0043c`
Owner command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

## Primary result

```text
result = MIXED_SEARCH_IDENTITY_DIAGNOSTIC_PASS
reason = native_only_legacy_global_count_failed_unique_matching_debt_identity_observed
environment = native_Windows
cpu_vendor = GenuineIntel
mode = Position
scenario = close
production_integrated = false
exact_slot_watch_enabled = false
game_used = false
```

The c42 mitigation tuple was known and admitted: `[0,0,0x100]`.

## VERIFIED-RUNTIME — native Windows

The native mixed event and continuation boundary were reproduced:

```text
WS_disposition                  = CONTINUE_SEARCH
downstream_disposition          = CONTINUE_EXECUTION
WS_calls                        = 1
downstream_calls                = 1
VCH_registered                  = true
VCH_callback_count              = 1
ordered_final_context_observed  = true
native_boundary_observed        = true
```

Observed order remained:

```text
1 WS_entry
2 WS_after_engine
3 reserved debt
4 SEARCH escape
5 downstream_before
6 downstream_final
7 VCH
```

The downstream-final and VCH snapshots were identical for thread/context/exception/RIP/EFlags/ContextFlags and Dr0/Dr1/Dr2/Dr3/Dr6/Dr7. WallSprint cleared only B0 (`Dr6 0xFFFF0FF5 -> 0xFFFF0FF4`), the downstream owner cleared only B2 (`0xFFFF0FF4 -> 0xFFFF0FF0`), and the VCH saw that finalized state.

The c50 decomposition identified the exact old c49 failing conjunct:

```text
used_leases_total                    = 4
global_used_leases_is_one            = false
legacy_c49_aggregate                 = false
only_global_usedLeases_conjunct_failed = true
legacy_failed_conjuncts              = [global_used_leases_is_one]
```

Every other old aggregate condition passed:

```text
ws_search_ok                    = true
entry cause                     = 5 expected 5
post-engine cause               = 4 expected 4
cause delta                     = 1 expected 1
RIP preserved                   = true
Dr0/Dr1/Dr2/Dr3/Dr7 preserved  = true
legacy first lease own mask     = 3
legacy window0 id               = 1 expected 1
legacy window0 generation       = 149 expected 149
```

The worker-specific identity was unambiguous despite four global lease records:

```text
matching_worker_lease_count          = 1
matching_owned_context_lease_count   = 1
selected_lease_index                 = 0
selected_window_id                   = 1
selected_window_generation           = 149
selected_own_mask                    = 3
UNIQUE_MATCHING_DEBT_IDENTITY        = true
c50_diagnostic_admissible            = true
```

All four published leases had the same window/generation/up and `own(context, lease)==3`, but had different thread IDs. This confirms the static c50 distinction: `own()` recognizes address/Dr7 ownership and is not itself a thread-identity predicate; the current saved-context obligation is selected by the matching worker thread plus lease/window/generation identity. The global `usedLeases==1` fixture condition was therefore over-strong for this native multithread-expanded run.

## Debt/lifetime boundary remains open

This PASS is diagnostic identity evidence only. No VCH-to-c41 bridge or post-return acknowledgment was implemented:

```text
VCH_to_witness_binding          = null
witness_acknowledgment          = null
pending_before_witness          = 1
pending_after_witness           = null
pending_at_worker_exit          = 1
can_retire_before_worker_exit   = false
cleanup_requested_while_pending = true
live_final_cleanup_performed    = false
cleanup_after_worker_exit       = true
bridge_implemented              = false
observer_Rip_writes             = 0
observer_DR_repairs             = 0
```

Therefore c50 does **not** prove saved-context retirement, production integration, code/storage reclamation, exact-slot safety, or gameplay writability. The c39/c40/c48 counterexamples remain authoritative until a later bridge/integration task closes those gaps.

## Consequence

C50 satisfies the explicit PASS condition from `docs/CODEX_NEXT.md`: the exact historical c49 failing conjunct is identified as the global lease-count assumption, and native Windows demonstrates a separately named unique matching debt identity without weakening production ownership semantics.

This authorizes only the next explicitly bounded **test-only VCH-to-c41 post-return witness bridge feasibility** task. It does not authorize production integration or the deferred exact-slot watch.
