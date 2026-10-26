# c42 native-Windows standalone probe result

Date: 2026-10-26
Branch: `codex/v01c20-edi-value-provenance`
Source/handoff commit tested: `70c55e3caec4ca10d0e995b8de4ba13b1b089aa2`

## Owner command

Executed from `analysis/native-probes/c42` in Windows PowerShell:

```powershell
.\continuation-witness.exe --position --multiple --json
```

## Exact result

```text
c41 owned assertion at line 166: !activeLeases()&&same(debug(),saved)
```

```json
{"schema":2,"task":"c42","witness_task":"c41","result":"FAIL","reason":"owned_assertion_failed","rejection":"none","line":166,"mitigation_classifier":{"revision":"c42-v1","admitted":true,"policies":[{"policy":"ProcessDynamicCodePolicy","query_known":true,"flags":0,"decision":"ADMIT","relevant_refuse_mask":0,"irrelevant_mask":0,"unknown_refuse_mask":0,"enabled_bits":[]},{"policy":"ProcessControlFlowGuardPolicy","query_known":true,"flags":0,"decision":"ADMIT","relevant_refuse_mask":0,"irrelevant_mask":0,"unknown_refuse_mask":0,"enabled_bits":[]},{"policy":"ProcessUserShadowStackPolicy","query_known":true,"flags":256,"decision":"ADMIT","relevant_refuse_mask":0,"irrelevant_mask":256,"unknown_refuse_mask":0,"enabled_bits":[{"mask":256,"name":"CetDynamicApisOutOfProcOnly","classification":"IRRELEVANT_TO_C41_CANDIDATE","reason":"candidate_uses_neither_restricted_CET_dynamic_API"}]}]},"game_used":false}
```

## Evidence classification

### VERIFIED-RUNTIME — native Windows standalone owned process

- All three mitigation queries succeeded with raw flags `[0,0,256]`.
- The c42 per-bit classifier admitted the observed `0x100 CetDynamicApisOutOfProcOnly` configuration.
- Therefore the continuation-witness path executed; this run was not an environment-boundary REFUSED.
- The `--position --multiple` path reached the final post-witness restoration check and failed at the combined assertion `!activeLeases() && same(debug(), saved)`.
- The preceding source assertions necessarily completed before this line: the outer witness was acknowledged, pending context debt reached zero, `canRetire()` was true, and `pollReset(false, "owned_c41_post_return_witness")` returned false as expected by the fixture.
- No Spider-Man 2 process or game data was used.

### VERIFIED-STATIC — source at tested commit

The failing assertion combines two independent requirements:

1. `activeLeases()` must be false after final `pollReset`.
2. `debug()` must exactly equal the saved debug-register snapshot under `same()`, which compares `Dr0`, `Dr1`, `Dr2`, `Dr3`, `Dr6`, and `Dr7`.

The current failure JSON does not identify which side failed and does not identify an individual debug-register mismatch.

### UNKNOWN

- Whether the native failure is retained lease bookkeeping, debug-register restoration mismatch, or both.
- If debug state differs, which of `Dr0/Dr1/Dr2/Dr3/Dr6/Dr7` differs.
- At what point between acknowledged witness, final `pollReset`, and the post-poll readback the divergence first appears.
- Whether any difference is an owned-engine bug, native-Windows debug-register canonicalization/semantics, or another mechanism. Do not infer a cause from this combined assertion.

## Required next evidence

Do not weaken the exact restoration requirement and do not change Windows mitigation settings.

The next bounded task should keep the same owned native scenario but replace the opaque combined failure with test-only diagnostics that separately report:

- active-lease state before and after final poll;
- final `pollReset` result;
- saved/current per-register equality for `Dr0/Dr1/Dr2/Dr3/Dr6/Dr7` before and after final poll;
- the existing thread/lease/generation/context witness state.

The diagnostic must preserve the current c39-c42 counterexamples and fail closed. It must not integrate the continuation witness into production or enable the deferred exact-slot watch.
