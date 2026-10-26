# c45 native-Windows DR6 lifecycle result

Date: 2026-10-27
Branch: `codex/v01c20-edi-value-provenance`
Tested published c45 tip: `d48b27acb71024409a55642fbe3226af7494aac6`

## Owner command

Executed from `analysis/native-probes/c45` in Windows PowerShell:

```powershell
.\continuation-witness.exe --position --multiple --json
```

## Outcome

The standalone owned process returned `FAIL` at `stability.canonicalAll`.

Native identity/result:

```text
environment=native_Windows
mode=Position
scenario=multiple
cpu_vendor=GenuineIntel
mitigation_flags=[0,0,256]
contexts=3
watch_context_writes_observed=1
outer_hardware_DB_events=1
game_used=false
```

Final bookkeeping again showed pending0, canRetire true before final poll, inactive leases after final poll, and final poll false. Dr0/Dr1/Dr3 restored exactly; the synthetic foreign Dr2 was preserved exactly. Final Dr6 was `0x00000000ffff0ff0` and Dr7 was `0x0000000000000410`; all three final readbacks were raw-stable.

## DR6 lifecycle

Dr6 bit11 / bit16 stayed `0 / 0` through:

- baseline before foreign state;
- foreign Set/readback;
- post-foreign Resume saved snapshot;
- owned watch Set/readback while still suspended before Resume.

At the first owned hardware #DB exception context they were `1 / 1` with DR6 `0xFFFF0FF1`. After the existing engine handler, B0 was cleared and DR6 became `0xFFFF0FF0`; bit11/bit16 remained `1 / 1` through post-return, final poll and all three stability readbacks.

C45 therefore reports the first-observed boundary as `HARDWARE_DB_ENTRY` for both bits, with the explicit qualification `unique_first_observed_boundary_not_underlying_causality`.

C45 remains FAIL / necessary-conditions-only and adds no `RETIREMENT_STATUS_SAFE` certificate.

The remaining gap is the watch-enabled Resume-to-first-watched-access interval. A next test-only probe should gate the worker after Resume but before the watched access, capture `CONTEXT_DEBUG_REGISTERS`, and determine whether bit11/bit16 become 1 before any #DB or only at exception delivery.

No production restoration/ownership change, exact-slot watch, game use or gameplay write is authorized by this result.
