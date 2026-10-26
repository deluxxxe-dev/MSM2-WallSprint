# c42 standalone owned Windows probe

Refreshed test-only probe with a per-bit mitigation classifier. The original c41
artifact and its native REFUSED evidence remain in ../c41 and
../../V01C41_NATIVE_WINDOWS_RESULT.md. No game release or production integration.

From this directory in Windows PowerShell, run one command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

It uses only its own process/thread/code/data; no Spider-Man2, Overstrike,
Update-WS, game log/scan, settings change or network access is involved.

Expected for the previously observed known flags[0,0,256]: schema2/taskc42,
witness_task=c41 and mitigation_classifier.revision=c42-v1. The classifier should
admit CetDynamicApisOutOfProcOnly as governing2 unused APIs. The actual witness
must then execute to determine its OWNED_WITNESS_PASS, REFUSED or FAIL result.
Native witness success is not inferred from the synthetic classifier tests.

Return the compact JSON line. It includes real raw flags and each enabled bit's
name/class/reason plus per-policy query/decision/masks. Relevant, unknown or
reserved settings and unknown queries refuse. The known unused thread opt-out
permission alone is also irrelevant; the program never invokes opt-out or
changes any mitigation. No protection should be changed to obtain a PASS.

Exit0 indicates an owned witness result (internal --cap tests instead use
FAIL_CLOSED_EXPECTED). Exit77 is prerequisite/admission REFUSED, exit3 is a
failed owned assertion with line/rejection and classifier metadata. A PASS is
scoped to this candidate/configuration, never a production retirement certificate.
The raw query results can legitimately differ from the previous run.

The binary is refreshed only after required owned tests pass and is an exact
copy of the tested EXE. PROBE_MANIFEST.json records binary/source/input hashes.
GPL/MinHook/Capstone notices accompany it; complete corresponding project source
and pinned dependency/build recipes remain in this repository.

Build/test entrypoints (repository root, configured Bash):

```bash
bash source/tools/build_windows.sh
bash source/tools/test_continuation_mitigations.sh
bash source/tools/test_continuation_witness.sh
```

This executable has no flag-injection mode. Synthetic policy inputs exist only
in the separate portable classifier-test program, explicitly labeled synthetic.
