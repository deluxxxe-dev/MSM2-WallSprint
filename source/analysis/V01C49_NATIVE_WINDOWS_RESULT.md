# c49 native Windows result — VCH boundary observed, strict probe FAIL

Date: 2026-10-27 JST
Branch: `codex/v01c20-edi-value-provenance`
Probe source/artifact head before owner run: `6a6fae94a1a0c75f0ee7e8e63df6dc685e038a4b`
Command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

## Primary probe result

```text
result = VCH_SEARCH_CONTINUATION_BRIDGE_FEASIBILITY_FAIL
reason = WS_mixed_SEARCH_identity_or_cause_mismatch
environment = native_Windows
cpu_vendor = GenuineIntel
mode = Position
scenario = close
production_integrated = false
exact_slot_watch_enabled = false
game_used = false
```

The c42 mitigation tuple was known and admitted: `[0,0,0x100]`.

## VERIFIED-RUNTIME — native Windows boundary evidence

The native run exercised the boundary that Wine could not:

```text
WS_disposition          = CONTINUE_SEARCH
downstream_disposition  = CONTINUE_EXECUTION
WS_calls                = 1
downstream_calls        = 1
VCH_registered          = true
VCH_callback_count      = 1
ordered_final_context_observed = true
native_boundary_observed       = true
```

Observed order and debug state:

```text
order 1  WS_entry
  thread=8084
  context=0x00000003c35ff400
  exception=0x00000003c35ff8f0
  Rip=0x00007ff68301ce8c
  Dr0=0x0000010a320e0030
  Dr1=0x0000010a320e0038
  Dr2=0x0000010a320e0030
  Dr3=0
  Dr6=0x00000000ffff0ff5
  Dr7=0x000000000d990415

order 2  WS_after_engine
  same thread/context/exception/Rip/Dr0/Dr1/Dr2/Dr3/Dr7
  Dr6=0x00000000ffff0ff4

order 5  downstream_before
  same state as WS_after_engine

order 6  downstream_final
  same thread/context/exception/Rip/Dr0/Dr1/Dr2/Dr3/Dr7
  Dr6=0x00000000ffff0ff0

order 7  VCH
  same thread/context/exception/Rip/Dr0/Dr1/Dr2/Dr3/Dr6/Dr7 as downstream_final
```

Therefore, in this owned native run, WallSprint cleared B0 and returned SEARCH; the following foreign owner cleared B2 and returned EXECUTION; then a real VCH callback observed the downstream-finalized CONTEXT before worker completion. This is a user-mode Windows runtime observation. It does not by itself prove that VCH is the last possible third-party mutation point for arbitrary processes/registrations.

The reserved debt was published before SEARCH escape and remained pending:

```text
lease=1
generation=149
sequence=1
reserved_order=3
SEARCH_escape_order=4
pending_before_witness=1
pending_at_worker_exit=1
can_retire_before_worker_exit=false
VCH_hold_exercised=true
cleanup_requested_while_pending=true
live_final_cleanup_performed=false
cleanup_after_worker_exit=true
```

No VCH-to-witness redirection or acknowledgment was implemented in this c49 observation-only binary:

```text
VCH_to_witness_binding = null
witness_acknowledgment = null
pending_after_witness = null
resurrection_after_final_cleanup = null
bridge_implemented = false
observer_Rip_writes = 0
observer_DR_repairs = 0
```

## FAIL boundary / unresolved subpredicate

The run is still a valid c49 FAIL because the probe set sticky poison at the WallSprint-stage aggregate predicate:

```text
WS_mixed_SEARCH_identity_or_cause_mismatch
```

Static inspection of the c49 probe shows that this one reason combines multiple checks: disposition, Dr6 cause pattern/delta, RIP and Dr0/1/2/3/7 preservation, global `usedLeases==1`, exact ownership of `leases[0]`, window id and reset generation. The JSON exposes enough fields to show the expected SEARCH disposition, B0-only clear, preserved printed debug registers/RIP, downstream B2-only clear, and final VCH context match. It does **not** expose every internal lease/window subpredicate individually.

Therefore the exact failing subpredicate is **UNKNOWN** from this run and must not be guessed. In particular, this result must not be relabeled as VCH unavailable: native VCH dispatch/order was observed. It also must not be relabeled as bridge PASS because the strict precondition failed and stage3 witness redirection was not implemented.

## Consequence

The next bounded task should decompose the aggregate WallSprint identity/cause predicate into explicit reported fields and determine whether the failure is:

- a real mixed-event identity/ownership contradiction; or
- a test-fixture/global-count assumption that is stronger than the unique debt identity actually required.

No production integration, exact-slot watch, gameplay write, DR repair/masking, or repeated identical c49 run is authorized by this result.
