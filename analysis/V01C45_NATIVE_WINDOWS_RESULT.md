# c45 native-Windows DR6 lifecycle result

Date: 2026-10-27
Branch: `codex/v01c20-edi-value-provenance`
Tested published c45 tip: `d48b27acb71024409a55642fbe3226af7494aac6`

## Owner command

Executed from `analysis/native-probes/c45` in Windows PowerShell:

```powershell
.\continuation-witness.exe --position --multiple --json
```

## Exact outcome

The standalone owned process returned `FAIL` at:

```text
c41 owned assertion at line 296: stability.canonicalAll
```

Root result fields included:

```text
schema=5
task=c45
witness_task=c41
environment=native_Windows
mode=Position
scenario=multiple
result=FAIL
reason=owned_assertion_failed
cpu_vendor=GenuineIntel
mitigation_flags=[0,0,256]
watch_context_writes_observed=1
outer_hardware_DB_events=1
game_used=false
```

The c42 mitigation classifier admitted the actual native tuple `[0,0,0x100]`. The c44 raw and architectural comparators remained FAIL.

## VERIFIED-RUNTIME — native Windows owned process

### Final retirement bookkeeping

The run again established:

```text
contexts=3
pending before final poll=0
pending after final poll=0
canRetire before final poll=true
active leases after final poll=false
final poll returned=false
```

After final poll:

```text
Dr0 restored exactly
Dr1 restored exactly
Dr2 synthetic foreign state preserved exactly
Dr3 restored exactly
Dr6 = 0x00000000ffff0ff0
Dr7 = 0x0000000000000410
```

All three bounded post-poll readbacks were captured and raw-stable at the same values.

### DR6 lifecycle timeline

The ordered c45 observations were complete. Dr6 bit11 / bit16 were:

| Phase | bit11 | bit16 | Relevant DR6 |
| --- | ---: | ---: | --- |
| baseline_pre_foreign | 0 | 0 | `0x0` |
| post_foreign_set_readback | 0 | 0 | `0x0` |
| post_foreign_resume_saved | 0 | 0 | `0x0` |
| post_watch_set_readback_pre_resume | 0 | 0 | `0x0` |
| outer_db_handler_entry | 1 | 1 | `0xFFFF0FF1` |
| outer_db_handler_after_engine | 1 | 1 | `0xFFFF0FF0` |
| post_return_pre_final_poll | 1 | 1 | `0xFFFF0FF0` |
| after_final_poll | 1 | 1 | `0xFFFF0FF0` |
| verification_readback_1 | 1 | 1 | `0xFFFF0FF0` |
| verification_readback_2 | 1 | 1 | `0xFFFF0FF0` |
| verification_readback_3 | 1 | 1 | `0xFFFF0FF0` |

Both bit11 and bit16 therefore had exactly one observed transition, from 0 to 1, between `post_watch_set_readback_pre_resume` and `outer_db_handler_entry`.

The c45 classifier reported for both:

```text
classification=HARDWARE_DB_ENTRY
reason=unique_first_observed_boundary_not_underlying_causality
changes_observed=1
```

This is a first-observed boundary, not proof that the processor itself rather than Windows exception/context delivery caused the transition.

### Owned #DB cause handling

At `outer_db_handler_entry`, DR6 was `0xFFFF0FF1`, so owned B0 was present. Immediately after the existing engine handler, DR6 was `0xFFFF0FF0`: the owned B0 cause was cleared while bit11/bit16 remained 1.

No later owned B0/B1 cause was present in the final/readback snapshots.

## VERIFIED-STATIC — current production watch boundary

Current production ownership is still determined from owned Dr0/Dr1 addresses plus Dr7 control fields. The handler claims the hardware event from Dr6 B0/B1. Dr6 bit11/bit16 are not watch-enable fields and are not used to identify owned Dr0/Dr1 ownership.

No production restoration or ownership logic changed in c45.

## VERIFIED-DOCUMENTATION — current Intel/Windows leads

Current Intel SDM evidence classifies DR6 as a debug status register. RTM bit16 has debug-exception status semantics; BLD bit11 has bus-lock-detection status semantics. DR7, not DR6, contains the breakpoint enable/type/length controls.

Microsoft documents `GetThreadContext` / `SetThreadContext` as Windows thread-context interfaces and states that some non-specifiable state may be silently set to correct OS-controlled values. That documentation is not a blanket equivalence rule for DR6 status.

## Current conclusion

C45 remains **FAIL / necessary-conditions-only**. `RETIREMENT_STATUS_SAFE` was not added.

The native evidence substantially narrows the problem:

- the pre-event DR6 API snapshots were 0 for bit11/bit16;
- the existing watch Set/readback still showed 0 immediately before Resume;
- the first observed 1/1 state is the actual owned #DB exception context;
- the existing handler clears the owned B0 cause, not bit11/bit16;
- bit11/bit16 remain 1 through post-return, final poll and three stable readbacks;
- addresses, foreign Dr2, Dr7 meaningful ownership state, lease cleanup and continuation debt all restore/retire as expected.

The remaining evidence gap is the interval after the watch-enabled thread resumes but before the watched memory access/#DB is delivered. C45 does not distinguish a watch-enabled resume/context-load normalization from the debug-exception delivery boundary itself.

## Required next evidence

Use a test-only pre-access gate so the worker can Resume with the owned watch armed, then be suspended/read back **before it performs the watched access**. This must establish whether DR6 bit11/bit16 are already 1 before any #DB or only become observable as 1 at exception delivery.

Do not change production `restore()`, do not normalize DR6 broadly, and do not enable the deferred exact-slot watch or any gameplay write.
