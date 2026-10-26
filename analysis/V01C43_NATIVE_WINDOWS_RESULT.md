# c43 native-Windows final-restoration diagnostic result

Date: 2026-10-26
Branch: `codex/v01c20-edi-value-provenance`
Standalone artifact source base: `87cd2f0f18b15599353b689e2f69491c324386fb` (from `analysis/native-probes/c43/PROBE_MANIFEST.json`)

## Owner command

Executed from `analysis/native-probes/c43` in native Windows PowerShell:

```powershell
.\continuation-witness.exe --position --multiple --json
```

## Exact result

```text
c41 owned assertion at line 192: !*restoration.activeAfter&&same(after,saved)
```

```json
{"schema":3,"task":"c43","witness_task":"c41","result":"FAIL","reason":"owned_assertion_failed","rejection":"none","line":192,"environment":"native_Windows","mode":"Position","scenario":"multiple","thread":9544,"lease":1,"generation":101,"contexts":3,"mitigation_query_known":[true,true,true],"mitigation_flags":[0,0,256],"mitigation_classifier":{"revision":"c42-v1","admitted":true,"policies":[{"policy":"ProcessDynamicCodePolicy","query_known":true,"flags":0,"decision":"ADMIT","relevant_refuse_mask":0,"irrelevant_mask":0,"unknown_refuse_mask":0,"enabled_bits":[]},{"policy":"ProcessControlFlowGuardPolicy","query_known":true,"flags":0,"decision":"ADMIT","relevant_refuse_mask":0,"irrelevant_mask":0,"unknown_refuse_mask":0,"enabled_bits":[]},{"policy":"ProcessUserShadowStackPolicy","query_known":true,"flags":256,"decision":"ADMIT","relevant_refuse_mask":0,"irrelevant_mask":256,"unknown_refuse_mask":0,"enabled_bits":[{"mask":256,"name":"CetDynamicApisOutOfProcOnly","classification":"IRRELEVANT_TO_C41_CANDIDATE","reason":"candidate_uses_neither_restricted_CET_dynamic_API"}]}]},"final_restoration":{"revision":"c43-v1","comparison":"EXACT_UNMASKED","synthetic_inputs":false,"attempted":true,"capture_complete":true,"active_leases_before_final_poll":true,"active_leases_after_final_poll":false,"final_poll_returned":false,"pending_contexts_before_final_poll":0,"pending_contexts_after_final_poll":0,"can_retire_before_final_poll":true,"final_predicate":false,"classification":"DR_ONLY_FAILURE","registers":{"Dr0":{"saved":"0x0000000000000000","before":"0x000001c79c1f0030","after":"0x0000000000000000","before_equal":false,"after_equal":true,"after_xor":"0x0000000000000000"},"Dr1":{"saved":"0x0000000000000000","before":"0x000001c79c1f0038","after":"0x0000000000000000","before_equal":false,"after_equal":true,"after_xor":"0x0000000000000000"},"Dr2":{"saved":"0x00007ff671ee0000","before":"0x00007ff671ee0000","after":"0x00007ff671ee0000","before_equal":true,"after_equal":true,"after_xor":"0x0000000000000000"},"Dr3":{"saved":"0x0000000000000000","before":"0x0000000000000000","after":"0x0000000000000000","before_equal":true,"after_equal":true,"after_xor":"0x0000000000000000"},"Dr6":{"saved":"0x0000000000000000","before":"0x00000000ffff0ff0","after":"0x00000000ffff0ff0","before_equal":false,"after_equal":false,"after_xor":"0x00000000ffff0ff0"},"Dr7":{"saved":"0x0000000000000010","before":"0x0000000000990415","after":"0x0000000000000410","before_equal":false,"after_equal":false,"after_xor":"0x0000000000000400"}},"verification_readbacks":[{"captured":false,"same_saved":null,"registers":{"Dr0":null,"Dr1":null,"Dr2":null,"Dr3":null,"Dr6":null,"Dr7":null}},{"captured":false,"same_saved":null,"registers":{"Dr0":null,"Dr1":null,"Dr2":null,"Dr3":null,"Dr6":null,"Dr7":null}},{"captured":false,"same_saved":null,"registers":{"Dr0":null,"Dr1":null,"Dr2":null,"Dr3":null,"Dr6":null,"Dr7":null}}]},"game_used":false}
```

## Evidence classification

### VERIFIED-RUNTIME — native Windows owned standalone process

- c42 mitigation admission succeeded with actual policies `[0,0,0x100]`.
- Three continuation contexts were acknowledged; `pending_contexts_before_final_poll=0` and remained zero after final poll.
- `can_retire_before_final_poll=true`.
- `active_leases_before_final_poll=true` and `active_leases_after_final_poll=false`; therefore lease cleanup succeeded.
- Final poll returned false as expected by this fixture.
- Final classification is `DR_ONLY_FAILURE` under the unchanged exact-unmasked comparator.
- Dr0, Dr1, Dr2 and Dr3 exactly match the saved snapshot after final poll.
- The synthetic foreign Dr2 value is preserved exactly.
- Dr6 differs by exactly `0x00000000ffff0ff0`.
- Dr7 differs by exactly `0x0000000000000400`; all other Dr7 bits match the saved value.
- The c43 process exits before the three verification readbacks, so post-failure stability is not established by this run.
- No game was used and no Windows mitigation setting was changed.

### VERIFIED-STATIC — c43 comparator

The c43 probe uses an exact unmasked comparison over Dr0/Dr1/Dr2/Dr3/Dr6/Dr7. It intentionally did not normalize reserved/fixed architectural bits.

### DOCUMENTATION CANDIDATE — not yet an acceptance rule

Intel x86-64 documentation describes fixed/reserved values in DR6 and DR7, and Microsoft documents that `SetThreadContext` may silently set non-specifiable processor/debug states to the correct OS-controlled value. The observed DR6 `0xFFFF0FF0` and DR7 delta `0x400` are consistent with documented fixed-bit patterns. This is not yet permission to weaken restoration checks: c44 must establish the exact bit contract from canonical documentation and prove that every architecturally meaningful/owned bit remains equal.

## Current conclusion

C43 successfully localized the native divergence. The earlier opaque c42 failure is not a lease-cleanup failure and is not a Dr0-Dr3 foreign-breakpoint restoration failure. It is an exact-representation mismatch limited to Dr6 and Dr7 in this run.

Native witness acceptance remains **NOT ESTABLISHED** because the current acceptance rule is exact-unmasked and the stability readbacks did not execute. Production integration and the deferred exact-slot watch remain unauthorized.
