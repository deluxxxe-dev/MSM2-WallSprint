# c44 native-Windows standalone probe result

Date: 2026-10-26
Branch: `codex/v01c20-edi-value-provenance`

Owner command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Observed result: native Windows `FAIL` at `stability.canonicalAll` after the c44 architecture-aware comparison.

Key runtime facts:

```text
cpu_vendor=GenuineIntel
mitigation_flags=[0,0,256]
mitigation_classifier.admitted=true
contexts=3
active_leases_after_final_poll=false
pending_contexts_after_final_poll=0
can_retire_before_final_poll=true
Dr0-Dr3 exact=true
Dr6 saved=0x0 after=0x00000000ffff0ff0 xor=0x00000000ffff0ff0
Dr7 saved=0x10 after=0x410 xor=0x400
readbacks_complete=true
raw_readbacks_stable=true
architectural_results_stable=true
RAW_EXACT=false
ARCHITECTURAL_CANONICAL=false
```

The c44 comparator accepts the Dr7 `0x400` fixed-one representation difference but rejects the Dr6 `0x10800` portion (bits 11 and 16) as meaningful/status mismatch. The three post-poll readbacks are complete and stable. This is not a native witness PASS and not a production retirement certificate.

Current Intel SDM 253669-090US (February 2026), section 20.2.3 identifies DR6 as a debug status register. BLD bit 11 and RTM bit 16 have active-low debug-event semantics; ordinary debug handlers are instructed to leave/set the corresponding no-cause state to 1. The remaining question is the exact Windows lifecycle/normalization boundary for the pre-event saved zero values versus post-#DB returned values on this owned path.

See the canonical detailed result at `analysis/V01C44_NATIVE_WINDOWS_RESULT.md`. Production integration, exact-slot watch, slot reads, producer traversal and gameplay writes remain unauthorized.
