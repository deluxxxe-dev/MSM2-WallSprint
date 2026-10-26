# c44 standalone vendor-specific debug-register comparison

The original c43 native run remains RAW exact FAIL/DR_ONLY_FAILURE. It established
lease cleanup and exact Dr0-Dr3, but did not report CPU vendor or reach3 subsequent
readbacks. Historical evidence and c41-c43 artifacts remain unchanged.

From this folder in native Windows PowerShell, run one game-free command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Return the compact JSON/output only. No Update-WS, game/Overstrike, A/E, WallRun,
offline scan, game log, mitigation change or network access is needed.

Schema4/taskc44 identifies actual CPUID vendor/signature/leaf7, real mitigation
flags and unchanged classifier, lease/pending/canRetire/poll, raw saved/before/
after six-register state and3 verification readbacks. All3 now run BEFORE final
acceptance assertions, without post-poll repair or retry. An API failure emits
partial/null evidence and FAIL; missing data never certifies restoration.

debug_register_comparison reports comparator c44-v1, source-edition identifier,
per-vendor full64bit masks, separate RAW_EXACT and ARCHITECTURAL_CANONICAL for
each snapshot, capture completeness and stability. final_restoration retains
the original c43-v1 raw comparisons/values, even when canonical comparison differs.

Dr0-Dr3, meaningful/global control/status and potential BLD11/RTM16 differences
must remain exact. Only documented fixed groups may differ, and CURRENT fixed
bits must have their required values. Unknown vendor/participating unknown bits
refuse; unsupported AMD feature profiles refuse before arming. No Intel-only
rule is applied to an unknown vendor. The observed c43 pair still fails both
conservative profiles; no blanket0xFFFF0FF0 mask is accepted.
The candidate also refuses CURRENT Dr6[11,16]=0 even if raw-equal: those active-low
states are outside its certified boundary. This is a stricter state restriction,
not a fixed-bit mask; saved/current equality for these fields is still required.

Source scope is explicit: latest official Intel/AMD delivery was unavailable
(proxy403). Vendor-authored Intel SDM075/June2021 and AMD APM24593 Rev3.30/
September2018 were inspected through a pinned distribution mirror. Exact sections,
hashes, reserved-field conservatism and an AMD table/prose discrepancy are recorded
in ../../V01C44_DR_CANONICALIZATION.md. This is not an asserted latest-edition or
arbitrary future-extension proof. Windows SetThreadContext corrections do not
authorize ignoring meaningful fields.

Native success (exit0/OWNED_WITNESS_PASS) requires the explicit
ARCHITECTURAL_CANONICAL comparator on after+all3 readbacks, stable raw current
representations and every original lease/debt/state/identity/lifetime assertion.
Raw equality may remain false and must stay reported false. REFUSED (exit77)
means unsupported prerequisites/vendor/feature/policy; FAIL (exit3) retains the
comparison/diagnostic and assertion details. Internal cap cases remain
FAIL_CLOSED_EXPECTED, never a restoration certificate.

Owned Wine regression still uses the original RAW_EXACT acceptance, explicitly
labeled owned_Wine_only_not_architectural_certificate. Wine currently returns
non-hardware fixed values, so its architectural result is FALSE even when its
raw restore passes. This cannot establish native architectural compatibility.
Synthetic fixtures exist only in the separate unit-test executable, never as
runtime CPUID/register/mitigation overrides in this standalone probe.

Sequential observations perturb scheduling and are not an atomic execution
history. Even a successful candidate run is not production retirement proof.
No production debug restoration/watch integration, slot value or gameplay write.

The binary was copied only after all required owned tests passed. PROBE_MANIFEST
records its exact tested bytes, source revision and246 code/tool input hashes.
GPL/MinHook/Capstone notices accompany it; corresponding owned source and pinned
build recipes remain in this repository. Focused build/checks from repository root:

```bash
bash source/tools/build_windows.sh
bash source/tools/test_continuation_mitigations.sh
bash source/tools/test_continuation_diagnostics.sh
bash source/tools/test_debug_register_comparison.sh
bash source/tools/test_continuation_witness.sh
```
