# c45 standalone DR6 lifecycle diagnostic

From this folder in native Windows PowerShell, run one game-free command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Return the compact output, including JSON even if the result is FAIL. No
Update-WS, game/Overstrike, A/E, WallRun, scan, game log, policy setting change
or network access is needed. Fetch this branch in your independent `_source`
clone with `git pull --ff-only` first if this c45 folder is absent.

Schema5/taskc45 reports actual CPUID/mitigation state, unchanged c43 raw and c44
architectural comparisons, leases/debt/canRetire/poll, all three stable readbacks
and eleven ordered phase observations. It records Dr0/1/2/3/6/7, bit11/bit16,
worker, sequence, API versus saved-exception origin, every transition and earliest
observed stage. It copies the existing watch-install readback before Resume and
original hardware exception context before/after the unchanged engine. One extra
foreign-stage GetThreadContext is added, with no extra Set, repair or watched hit.

Native c44 GenuineIntel RAW_EXACT+ARCHITECTURAL_CANONICAL FAIL is preserved.
RETIREMENT_STATUS_SAFE is NOT added; necessary_conditions_only is a diagnostic
model, never an acceptance certificate. Native still requires the c44 contract
and complete/unique phase evidence. A continued FAIL is legitimate and expected
when status differs; it still answers the native first-change question. Multiple,
missing or unisolated transitions refuse. Unknown/non-Intel/debugger/unsupported
mitigation prerequisites refuse before arming (exit77); owned assertion failures
emit FAIL (exit3). A successful owned candidate is not production integration.

Wine retains its original RAW_EXACT-only regression, explicitly not a native or
architectural certificate. All ten complete Wine scenarios first observe11/16
becoming1 at DB entry and reverting0 after final poll: two transitions, unresolved.
Do not promote those API/context views to a physical CPU cause or native history.

Current Intel proof lead is owner-retrieved Vol3B253669-090US/February2026/20.2.3;
independent cloud PDF/hash delivery was unavailable. The accepted extraction hash
in the manifest is not a PDF hash. Full semantics, static audit, exact boundaries,
tests and limitations: ../../V01C45_DR6_LIFECYCLE.md and ../../V01C45_STATUS.json.

PROBE_MANIFEST.json pins the tested PE and all249 source/tool inputs. Rebuild
from the implementation commit with its build commands; PE link timestamps may
change binary hashes. Existing c41-c44 artifacts/licenses remain unchanged.
Runtime remains v0.1c38/package0.1.56. No production restoration/slot/gameplay or
producer work is enabled by this diagnostic.
