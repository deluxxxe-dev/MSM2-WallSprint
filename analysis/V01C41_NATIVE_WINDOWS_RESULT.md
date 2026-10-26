# c41 native-Windows standalone probe result

Date: 2026-10-26
Branch: `codex/v01c20-edi-value-provenance`
Source/handoff commit tested: `ded0ce052d2c39e5640d1ac5b79be0b85046ccac`

## Owner command

Executed from `analysis/native-probes/c41` in Windows PowerShell:

```powershell
.\continuation-witness.exe --position --multiple --json
```

## Exact result

```json
{"schema":1,"task":"c41","result":"REFUSED","reason":"mitigation_active_outside_supported_boundary","mitigation_query_known":[true,true,true],"mitigation_flags":[0,0,256],"game_used":false}
```

## Evidence classification

### VERIFIED-RUNTIME — native Windows standalone owned process

- All three mitigation queries used by the c41 probe returned known results.
- Reported flags were `[0,0,256]`.
- The probe returned `REFUSED` before running the continuation-witness acceptance path.
- No game was used.
- Therefore this run is **not** a native-Windows witness PASS or FAIL; it is a conservative environment-boundary refusal.

### VERIFIED-STATIC — current c41 probe source

`continuation_witness_smoke.cpp` queries policies in this exact order:

1. `ProcessDynamicCodePolicy`
2. `ProcessControlFlowGuardPolicy`
3. `ProcessUserShadowStackPolicy`

The current probe refuses when any queried policy DWORD is nonzero. Thus the observed third value `256` is the direct reason the native run refused.

### VERIFIED-DOCUMENTATION — Microsoft process mitigation structure

For `PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY`, bit `0x100` (256) is `CetDynamicApisOutOfProcOnly`. Microsoft documents this bit as restricting certain hardware-enforced-stack-protection dynamic-code APIs (`SetProcessDynamicEHContinuationTargets` and `SetProcessDynamicEnforcedCetCompatibleRanges`) to out-of-process callers.

The c41 candidate does not currently call either API. This documentation fact does **not** by itself prove that the continuation witness is compatible with the observed native-Windows configuration.

### UNKNOWN

- Whether the c41 continuation witness succeeds on native Windows while `CetDynamicApisOutOfProcOnly` is set and the other two queried policy DWORDs are zero.
- Whether any other user-shadow-stack-policy bit materially affects this exact context-redirection/direct-jump candidate.
- Whether the current all-nonzero refusal is unnecessarily broad for this candidate.

## Required next evidence

Do not disable or change Windows mitigations. Do not launch Spider-Man 2.

The next bounded task should review the exact per-bit mitigation contract and, only if supported by documentation plus owned tests, narrow the test-only admission classifier so irrelevant bits do not cause a false refusal. Unknown/reserved or materially relevant protection bits must continue to fail closed.

After any justified classifier refinement, rebuild/retest the owned suite and produce a refreshed standalone Windows probe for one more native run. Production watch integration and the deferred exact-slot watch remain unauthorized.
