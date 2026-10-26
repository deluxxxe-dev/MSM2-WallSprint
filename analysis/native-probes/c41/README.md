# c41 standalone owned Windows probe

This executable tests only its own process, worker, generated code and allocated
memory. It does not start Spider-Man2/Overstrike, inspect another process, install
a MOD, update a workspace, access the network or enable the deferred slot watch.
Production integration is outside c41. Runtime/game packages stay unchanged.

The specific question is whether actual Windows resumes a redirected owned saved
exception context through the15byte continuation while preserving the measured
machine/DR state, with per-context acknowledgment under the reported mitigation
boundary. Wine execution is already established; it is not native evidence.

From this directory in **Windows PowerShell**, one standalone command:

```powershell
& ".\continuation-witness.exe" --position --multiple --json
```

Expected: one JSON line, environment=native_Windows, result=OWNED_WITNESS_PASS,
contexts=3, retirement_accepted=true, native_unmitigated_probe_passed=true,
production_integrated=false. It exercises one real hardware event and nested
owned contexts; no later watched hit is used to repair restoration.

REFUSED (exit77) means debugger/AVX/mitigation prerequisites were not established
or lie outside the supported boundary. FAIL (exit3) includes an assertion/source
line and rejection reason. Return that JSON as evidence; protection settings
are not changed. Enabled CFG/ACG/CET and unknown native policies are not supported
by this feasibility candidate. A PASS is scoped to this probe/configuration,
not a production retirement or universal Windows compatibility certificate.

The binary is statically linked to the owned test, Capstone5.0.6 and MinHook;
standard Windows system DLLs remain required. PROBE_MANIFEST.json records the
exact SHA256/source commit/build inputs. Corresponding source and build recipe:
source/src/NativeHooks/continuation_witness_{smoke.cpp,harness.S},
source/tools/test_continuation_witness.sh and the existing normal cross-build.
Complete project/source/dependencies are in the same repository. Licenses and
third-party notices accompany this diagnostic; it is not a game release.

```bash
bash source/tools/build_windows.sh
bash source/tools/test_continuation_witness.sh
```

The script builds the standalone EXE and runs12 Wine cases. The repository binary
is an exact copy of that tested EXE, not an independent prebuilt dependency used
by the game updater. No private game body/log is included.
