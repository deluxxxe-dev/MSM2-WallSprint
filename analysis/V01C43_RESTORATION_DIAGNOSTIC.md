# c43 — exact final-restoration diagnostic for the owned standalone probe

Task base87cd2f0, branch codex/v01c20-edi-value-provenance. This is test-only
diagnostic work. Runtime remains v0.1c38/package0.1.56; no game release.

## Accepted evidence and unresolved cause

**VERIFIED-RUNTIME, owner native Windows:** the c42 artifact at70c55e3 admitted
actual policies[0,0,256] with classifier c42-v1, then returned FAIL at the final
combined `!activeLeases() && same(debug(),saved)` assertion. The preceding outer
acknowledgment, pending0, canRetire and false poll assertions had completed.
This is execution failure, not admission REFUSED, and is never reclassified as
PASS. Exact supplied evidence: V01C42_NATIVE_WINDOWS_RESULT.md, retained unchanged
with the historical c41/c42 artifacts/manifests.

**UNKNOWN:** retained leases, any individual DR mismatch, both, the first
divergence boundary and root cause. No Windows canonicalization, Wine behavior
or engine bug is inferred from a combined predicate. C42 classifier eligibility
is not final restoration, full state preservation or native witness acceptance.

## Diagnostic contract

New pure owned header continuation_diagnostics.hpp only compares/serializes
inputs. It is not linked into WallSprint.dll and never restores/masks/normalizes
registers. Original `same(CONTEXT,CONTEXT)` is byte-for-byte unchanged: Dr0,Dr1,
Dr2,Dr3,Dr6,Dr7 must all match exactly. The final lease requirement remains false.

All success/FAIL/REFUSED records now identify schema3/taskc43/witness_taskc41 and
carry the same final_restoration object, including when a snapshot could not be
obtained. Missing values are null/NOT_OBSERVED, never invented zero/success.
Mitigation raw flags, known-query status and unchanged c42-v1 interpretation
remain real API results, not synthetic inputs. Identity is fixed mode/scenario,
actual worker thread ID, lease1/generation101 and published context count.

For the non-cap path, after all required witnesses and pending0:

1. Retain the same saved exact snapshot established after test-owned foreign DR2
   setup. Sample the existing live worker's debug registers before final poll
   using the existing SuspendThread/GetThreadContext/ResumeThread helper.
2. Record active_leases_before_final_poll, pending_contexts_before_final_poll and
   can_retire_before_final_poll. The original canRetire assertion still gates poll.
3. Execute the original final poll exactly once; retain its boolean result.
4. Record active_leases_after_final_poll and pending_contexts_after_final_poll,
   then immediately obtain the after snapshot with the same helper.
5. Only after retaining both sides, assert the original false-poll requirement
   and `!active_after && same(after,saved)`. Retain each of the original three
   additional exact stability readbacks before its assertion.

No diagnostic formatting/allocation, register write, second watched access,
additional retry, larger deadline or cap is inserted between these samples and
poll. The bounded six-register snapshots use owned test memory. JSON gives each
register's full64bit saved/before/after hex, exact per-phase equality and after
XOR. Three subsequent snapshots are bounded and individually marked captured.
The four post-poll classifications are ALL_RESTORED, LEASE_ONLY_FAILURE,
DR_ONLY_FAILURE and LEASE_AND_DR_FAILURE. Classification does not override the
poll/state/ABI/ownership/witness assertions; a later readback mismatch still FAILs.

Cap cases deliberately never enter the final poll: diagnostics say attempted
false/incomplete/NOT_OBSERVED and keep final_predicate null. They remain expected
poisoned refusal, never claimed exact live-thread restoration. Early assertion
or API failure similarly retains partial/null fields before process exit3.

## Observation safety and limits

**VERIFIED-STATIC:** snapshots use the fixture's existing supported API helper,
after the outer context token is observed and while executable/data storage and
the owned worker remain alive. No CONTEXT values or production lease state are
forged. The worker has exactly one watched access; it remains in the existing
owned state-capture/finish protocol. Snapshots suspend/resume but never request
context mutation. Synthetic comparison inputs do not enter the real probe.

These are sequential observations, not an atomic all-thread snapshot. The extra
before sample changes scheduling and can expose or hide a timing-dependent
failure; it cannot prove why the original c42 run failed. active/pending and DR
samples also have small ordering intervals. If an API/assertion fails early,
the relevant snapshot stays absent and no final acceptance is certified. The
new native result must be interpreted as measured, even if it differs from c42.
No canonicalization hypothesis is promoted and no masking exception is added.

## Validation and handoff

Focused command: `bash source/tools/test_continuation_diagnostics.sh`. Sanitized
pure C++ tests cover all four outcomes, all six registers/high bits, missing
observations, independent before/after differences, immutable serialization,
three readbacks and full-width hex/XOR. Synthetic JSON is explicitly labeled.
The actual witness script runs all original12 processes and independently parses
their real output with `python3 tools/check_continuation_diagnostics.py
logs/windows_continuation_witness_tests.log` from source/. It verifies10 passing
exact restorations and2 unobserved cap refusals, identity and raw policy reporting.
Both focused helpers are mandatory in the full hooks chain. Exact final counts,
all required commands and limitations are in CODEX_RESULT and V01C43_STATUS.json.

After all required owned tests pass, publish a new artifact/manifest/licenses in
analysis/native-probes/c43 without replacing historical artifacts. One further
owner native run from that folder uses only:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Result may be OWNED_WITNESS_PASS, REFUSED or FAIL. The final diagnostic is emitted
on each path, even assertion failure, without manufacturing a native result.
No Update-WS, game/Overstrike/WallRun/scan/log or mitigation-setting change.
No production integration/restoration change, exact-slot watch/value read,
pointer chase, producer traversal or new gameplay permission is authorized.
All c39/c40 counterexamples, c41 ledger/state/foreign-DR/cap/lifetime checks and
c42 policy rules remain intact. Native cause and production retirement remain
unresolved; a diagnostic success is not a production fix.
