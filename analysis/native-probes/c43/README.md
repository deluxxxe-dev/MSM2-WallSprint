# c43 standalone final-restoration diagnostic

This is an owned test process, not a game MOD. The owner native c42 run admitted
its mitigation policies, then returned FAIL at the final combined lease/DR check.
The original result and c41/c42 artifacts remain preserved. C43 diagnoses that
boundary; it does not fix production retirement or infer the original cause.

From this directory in native Windows PowerShell, run exactly one command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Return the compact JSON/output. No game, Overstrike, Update-WS, WallRun, offline
scan, game log, network access or Windows mitigation-setting change is needed.

Output schema3/taskc43/witness_taskc41 retains real raw mitigation flags and the
unchanged c42-v1 classifier. final_restoration reports c43-v1/EXACT_UNMASKED:

- active leases and pending contexts before/after the original final poll;
- pre-poll canRetire and the original poll return value;
- each of Dr0/Dr1/Dr2/Dr3/Dr6/Dr7: full64bit saved/before/after hex, exact equality
  and XOR, plus three bounded verification readbacks;
- actual worker thread, mode/scenario, lease/generation and context count.

Only owned debug-register state is included; no game/process memory dump or
unrelated registers are emitted. Missing observations are null/NOT_OBSERVED,
including early failures and cap cases that never attempt final poll. A final
classification distinguishes lease-only, DR-only, combined and all-restored
states; it never overrides other assertions or certifies overall success.

OWNED_WITNESS_PASS (exit0) requires every existing assertion, exact unmasked
restoration and inactive leases. REFUSED (exit77) means unsupported prerequisites/
policy. FAIL (exit3) retains available diagnostic fields and stderr assertion
details. Internal --cap cases retain FAIL_CLOSED_EXPECTED, not live restoration.
An early API failure may leave partial observations; do not treat null as zero.

The extra before-poll suspend/read/resume is a sequential observation and changes
scheduling. A different result does not reconstruct c42 or identify its cause.
Do not normalize/mask differences or change mitigations to obtain a PASS.
Owned Wine PASS is not native evidence or production-integration authorization.

The binary was copied only after all required owned validation passed and is
an exact copy of that tested executable. PROBE_MANIFEST records artifact/source
input hashes and source revision. Historical binaries are unchanged. GPL,
MinHook and Capstone notices accompany this artifact; complete corresponding
owned source and pinned dependency/build recipes remain in this repository.

Build/focused checks from repository root in configured Bash:

```bash
bash source/tools/build_windows.sh
bash source/tools/test_continuation_mitigations.sh
bash source/tools/test_continuation_diagnostics.sh
bash source/tools/test_continuation_witness.sh
```

Synthetic comparisons exist only in the separate portable diagnostic-test
program. This standalone executable has no state/policy injection option and
does not change production restoration semantics.
