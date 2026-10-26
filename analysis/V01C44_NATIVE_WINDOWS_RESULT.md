# c44 native-Windows standalone probe result

Date: 2026-10-26
Branch: `codex/v01c20-edi-value-provenance`
Latest repository head inspected before handoff: `8a2815ea517fc2bc4b9a95212b844ac6ffe849d4`

## Owner command

Executed from `analysis/native-probes/c44` in Windows PowerShell:

```powershell
.\continuation-witness.exe --position --multiple --json
```

## Exact observed result boundary

The native probe reached the c44 final stability assertion and failed closed:

```text
c41 owned assertion at line 218: stability.canonicalAll
```

Root result: `schema=4`, `task=c44`, `witness_task=c41`, `result=FAIL`, `reason=owned_assertion_failed`, `line=218`, `environment=native_Windows`, `mode=Position`, `scenario=multiple`, `contexts=3`, `game_used=false`.

Observed native identity/policy:

```text
cpu_vendor=GenuineIntel
cpuid_known=true
mitigation_flags=[0,0,256]
mitigation_classifier.admitted=true
```

Final retirement bookkeeping completed as expected:

```text
active_leases_before_final_poll=true
active_leases_after_final_poll=false
final_poll_returned=false
pending_contexts_before_final_poll=0
pending_contexts_after_final_poll=0
can_retire_before_final_poll=true
```

Final raw debug-register state:

```text
Dr0 saved=0x0000000000000000 after=0x0000000000000000 exact=true
Dr1 saved=0x0000000000000000 after=0x0000000000000000 exact=true
Dr2 saved=0x00007ff665300000 after=0x00007ff665300000 exact=true
Dr3 saved=0x0000000000000000 after=0x0000000000000000 exact=true
Dr6 saved=0x0000000000000000 after=0x00000000ffff0ff0 exact=false xor=0x00000000ffff0ff0
Dr7 saved=0x0000000000000010 after=0x0000000000000410 exact=false xor=0x0000000000000400
```

All three post-poll verification readbacks were captured and were byte-for-byte stable with the `after` snapshot. Thus the native post-poll representation is stable across the bounded observations.

The c44 comparator classified Dr7 bit `0x400` as a documented fixed-one representation difference, but still rejected the Dr6 state because the delta contains `0x10800` (bits 11 and 16) inside the current conservative meaningful/status mask. Every after/readback comparison returned:

```text
RAW_EXACT=false
ARCHITECTURAL_CANONICAL=false
Dr0_Dr3_exact=true
meaningful_exact=false
OS_controlled_exact=true
unknown_bits_clear=true
current_fixed_bits_valid=true
status_boundary_valid=true
reason=meaningful_status_or_control_not_exact
```

Aggregate stability:

```text
readbacks_complete=true
raw_all_exact=false
architectural_all_equivalent=false
raw_readbacks_stable=true
architectural_results_stable=true
```

## Evidence classification

### VERIFIED-RUNTIME — native Windows owned standalone process

- CPU vendor is `GenuineIntel`.
- The c42 mitigation classifier admitted the observed `[0,0,0x100]` policy tuple.
- Three continuation contexts were acknowledged; pending debt reached zero.
- Final poll removed the active lease and returned the expected false fixture result.
- Dr0-Dr3 restored exactly, including the synthetic foreign Dr2 state.
- Dr7 differs only by `0x400`; c44 classifies this as a fixed-one representation bit.
- Dr6 differs by `0xFFFF0FF0`; the only c44-nonfixed portion of that observed delta is `0x10800` (bits 11 and 16).
- Three post-poll readbacks are complete and stable.
- The c44 conservative architectural comparator therefore failed closed. This is not an owned witness PASS and not a production retirement certificate.

### VERIFIED-STATIC — current production/test implementation boundary

The current production watch engine records `old6`, uses DR6 B0/B1 to identify the owned breakpoint cause, and its ownership test is based on Dr0/Dr1 plus owned Dr7 fields. The production `restore()` routine does not use DR6 bits 11/16 to decide watch ownership. This static observation does not by itself authorize relaxing the c44 test comparator.

### VERIFIED-DOCUMENTATION — current Intel primary manual lead

Intel SDM Volume 3B, order 253669-090US, February 2026, section 20.2.3 states that DR6 is a debug *status* register reporting conditions sampled at the last debug exception. It states:

- DR6.BLD bit 11 is active-low for an OS bus-lock-detection #DB; other debug exceptions do not modify it, and handlers should set it to 1 before returning.
- DR6.RTM bit 16 is active-low for the special advanced-RTM-debug case and is set for any other debug exception; handlers should leave/set bit 16 to 1 before returning.

This is a stronger current primary-source lead than the older c44 pinned-manual limitation, but it still does not prove how Windows `GetThreadContext`/`SetThreadContext` maps the pre-event saved `0` values versus post-#DB returned values on this exact native sequence.

### UNKNOWN / next blocker

- At which exact boundary DR6 bits 11/16 change on this Windows path: `SetThreadContext` round-trip, #DB entry, the test-owned `restore()` edit, OS exception return, or another documented Windows normalization step.
- Whether the pre-watch `saved=0` representation is a stable Windows API baseline that must be re-established, or merely a pre-event/status representation that is legitimately superseded by the owned #DB lifecycle.
- Whether a narrowly defined test-only retirement-safety comparator may validate BLD/RTM by documented post-event status semantics rather than equality to the pre-event snapshot.

Production integration, exact-slot watch, slot reads, producer traversal and gameplay writes remain unauthorized.
