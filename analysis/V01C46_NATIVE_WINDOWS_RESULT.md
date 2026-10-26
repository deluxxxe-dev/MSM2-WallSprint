# c46 native Windows result

Date: 2026-10-27 (owner local time)
Branch: `codex/v01c20-edi-value-provenance`
Published c46 revision pulled locally through `27a7e44`.

Owner command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Result: **FAIL** at the intentionally unchanged c44 architectural comparator (`stability.canonicalAll`). This is not a production failure certificate; c46 was a test-only observation task.

## VERIFIED-RUNTIME — native Windows

Environment/identity:
- `environment=native_Windows`
- `cpu_vendor=GenuineIntel`
- mitigation tuple `[0,0,256]`, classifier `ADMIT`
- `contexts=3`
- `watch_context_writes_observed=1`
- `outer_hardware_DB_events=1`
- no game used

Final retirement bookkeeping remained healthy:
- pending before/after final poll `0/0`
- `canRetire()` true before final poll
- active leases false after final poll
- final poll returned false
- Dr0/Dr1/Dr2/Dr3 restored exactly, including synthetic foreign Dr2
- final Dr6 `0x00000000ffff0ff0`
- final Dr7 `0x0000000000000410`
- three final readbacks complete and raw-stable
- RAW_EXACT false
- c44 ARCHITECTURAL_CANONICAL false

## c46 pre-access boundary

The new `post_watch_resume_pre_access` observation was fully proven:
- gate reached and captured while worker suspended
- release still unsignaled at capture
- worker was resumed after watch installation
- active lease and owned slots still present
- `DB_events_at_arrival=0`
- `DB_events_before_suspend=0`
- `DB_events_at_capture=0`
- `DB_events_after_read=0`
- `machine_entries_at_capture=0`
- `tickets_at_capture=0`
- `pending_at_capture=0`
- one original watch `SetThreadContext` write and no intervening context write

Observed DR6 bit lifecycle:

```text
post_watch_set_readback_pre_resume   bit11=0 bit16=0 Dr6=0x0000000000000000
post_watch_resume_pre_access         bit11=1 bit16=1 Dr6=0x00000000ffff0ff0
outer_db_handler_entry               bit11=1 bit16=1 Dr6=0x00000000ffff0ff1
outer_db_handler_after_engine        bit11=1 bit16=1 Dr6=0x00000000ffff0ff0
post_return_pre_final_poll           bit11=1 bit16=1 Dr6=0x00000000ffff0ff0
after_final_poll                     bit11=1 bit16=1 Dr6=0x00000000ffff0ff0
verification_readback_1..3           bit11=1 bit16=1 Dr6=0x00000000ffff0ff0
```

Both bit11 and bit16 classify as `WATCH_ENABLED_RESUME_PRE_ACCESS`: the first observed 0->1 transition occurs after Resume but before the watched access, with zero #DB events and zero machine entries. Therefore the c45 label `HARDWARE_DB_ENTRY` was only a prior observation boundary and is superseded by this narrower native observation.

## Evidence boundary

VERIFIED-RUNTIME:
- bit11/16 do not require a watched access or #DB delivery to become 1 in this native owner fixture; they are already 1 in the watch-enabled post-Resume/pre-access suspended context.
- the production handler later clears owned B0 while bit11/16 remain 1.
- watch/lease/ticket retirement bookkeeping and Dr0-Dr3 restoration remain successful.

VERIFIED-DOCUMENTATION (separate from runtime):
- Intel defines DR6 as a debug status register and DR7 as the debug control register.
- Intel documents BLD11 and RTM16 as status fields with event-specific semantics.
- Microsoft documents that `SetThreadContext` may silently correct values that cannot be specified, including OS-controlled debug-register state.

UNKNOWN / NOT PROVEN:
- the exact kernel/processor micro-mechanism that changes DR6[11]/DR6[16] during the Resume-before-access interval.
- a production-wide retirement certificate for arbitrary threads/CPUs/configurations.
- production continuation-witness integration.

The c46 raw/c44 FAIL remains historical and auditable. Do not relabel it PASS. The result does, however, remove the owned #DB event itself as a necessary cause of the bit11/16 transition and permits a later task to evaluate a separately named, narrow configuration-retirement predicate.
