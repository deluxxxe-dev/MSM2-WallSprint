# c44 — conservative vendor-specific debug-register comparison

Base7336ef4, branch codex/v01c20-edi-value-provenance. Test-only comparison;
runtime remains v0.1c38/package0.1.56. No production restoration or watch change.

## Accepted native evidence

V01C43_NATIVE_WINDOWS_RESULT.md is retained byte-for-byte. C43 native execution
admitted actual policies[0,0,256], acknowledged3 contexts, pending0 before/after,
canRetire true, poll false and inactive leases afterward. Dr0-Dr3 restored
exactly; raw Dr6 XOR=0xFFFF0FF0 and Dr7 XOR=0x400. The exact-unmasked result
remains FAIL/DR_ONLY_FAILURE. The original assertion prevented all3 subsequent
readbacks. CPU vendor and native post-poll stability were NOT supplied.

## Documentation inspected, provenance and limits

Direct current Intel/AMD publication endpoints returned proxy403, including
Intel's SDM landing page/cdrdv2 and AMD docs/content/system-file endpoints. Do
not claim the latest vendor edition was obtained. Actual vendor-authored primary
manual PDFs were instead read through a pinned third-party distribution mirror:
tpn/pdfs at a0eca7a2d92cd21a5fc8e8dcdb7653153e6c55a8. These are primary manuals,
not mirror-authored interpretations; delivery provenance and edition are distinct.
No PDF/manual body is committed or redistributed with the executable.

- Intel, Intel64/IA-32 SDM combined325462-075US, June2021, Vol3B17.2,
  Fig17-1/17-2,17.2.3 DR6,17.2.4 DR7,17.2.6 64bit registers,17.3.3 RTM.
  Mirror filename: `Intel 64 and IA-32 Architectures Software Developer's Manual -
  Combined Volumes 1-4 - June 2021 (325462-sdm-vol-1-2abcd-3abcd).pdf`.
  PDF SHA256 ef129f8b18963a55deb190e2fbff40f7e3bacbee4afc248c9c9087093d08be48.
- AMD, AMD64 APM24593 Rev3.30, September2018,13.1.1.3/Fig13-2 pp352-353 and
  13.1.1.4/Fig13-3 pp353-355. Mirror filename: `AMD - AMD64 Programmer's Manual -
  Volume 2 - System Programming - Rev 3.30 - Sept 2018 (24593).pdf`.
  PDF SHA256 fec654e554b241f82d2b2197e03c46934efdf1ec0a987019a849acf975421446.
  The prose/diagram explicitly specify31:16 read/set-one,11:4 read-one and12
  read-zero; the small table's63:16 MBZ range conflicts with that prose/diagram.
  This is not silently used as a zero mask. The table/prose discrepancy and
  inability to check the newest edition remain documentation limitations.
- Current MicrosoftDocs/sdk-api docs branch: processthreadsapi/
  nf-processthreadsapi-getthreadcontext.md and -setthreadcontext.md. Actual
  content retrieved from raw.githubusercontent.com. Get requires a suspended
  target/selective ContextFlags; Set may silently correct non-specifiable
  privileged/global-debug state. This is not a blanket equivalence guarantee.
  Get file SHA25682be7f41ca82e777e3a731bb9cb5169816bf86d4e6a4fd2bc70b48e3842b9597
  (metadata date2018-12-05); Set file
  SHA256af689d9e657208757e6d1c8b74e7b757c711122c321fe6a3a5a6191619fd1618
  (metadata date2025-06-13).
- Checked Microsoft WinSDK winnt.h from microsoft/win32metadata main,
  SHA256 d66df443218279ed2ffe7718b5347e64bf712dbfa5d25a3b0692787169405c69.
  CONTEXT_DEBUG_REGISTERS selects exactly Dr0-Dr3/Dr6/Dr7; CONTEXT_AMD64 names
  the ISA/layout, NOT the physical CPU vendor. No SDK definition proves fixed
  register semantics or overrides the vendor contract.
- Current supplemental Linux upstream documentation/checked definitions:
  Documentation/arch/x86/buslock.rst and arch/x86/include/uapi/asm/debugreg.h,
  retrieved from torvalds/linux master. SHA256 respectively
  7881aab499c76ffeccdad2fa577d8fbac808102c0b355ea53c3815f729bcde4e and
  0a24618ea3c5d8ccd80fca8ebfa41ed9453741843d7db9c48c708952ce0a988e.
  These are supplementary implementation documentation, NOT vendor normative
  proof for accepting a bit. They warn that BLD11 and RTM16 can be active-low
  status, and that some AMD processors support bus-lock detection. This is a
  reason to PRESERVE/refuse differences, never to authorize normalization.

## Conservative per-bit contract c44-v1

Every Dr0-Dr3 bit remains MEANINGFUL_EXACT, full64bit. For Dr6/Dr7 the following
groups partition each full64bit word without overlap. Numeric masks are emitted
in JSON, so every bit is independently classifiable from the report.

| Vendor | Register/bits | Class | Evidence/boundary |
| --- | --- | --- | --- |
| Intel | Dr6 0-3,13-15 | MEANINGFUL_EXACT | B0-B3,BD,BS,BT status; never discarded |
| Intel | Dr6 11,16 | MEANINGFUL_EXACT | Potential BLD and RTM status; preserve even when a feature-specific fixed rule might be possible |
| Intel | Dr6 4-10,17-31 | FIXED_ONE | SDM Fig17-1/17-2 explicitly fixed/set-one representation |
| Intel | Dr6 12 | UNKNOWN_REFUSE | Conservative reserved-field boundary; no generic reserved don't-care |
| Intel | Dr6 32-63 | FIXED_ZERO | 64bit reserved upper bits; writing1 faults |
| Intel | Dr7 0,2,4,6,8-9,11,13,16-31 | MEANINGFUL_EXACT | Local enable,LE/GE,RTM enable,GD,RW/LEN; even ignored/unsupported controls are not normalized |
| Intel | Dr7 1,3,5,7 | OS_CONTROLLED_EXACT_OR_REFUSE | Global enables; Microsoft caveat does not permit masking them |
| Intel | Dr7 10 | FIXED_ONE | SDM debug-register diagram fixed1 |
| Intel | Dr7 12,14-15 | UNKNOWN_REFUSE | Conservative low reserved-field read-contract boundary |
| Intel | Dr7 32-63 | FIXED_ZERO | Reserved upper bits must0; writing1 faults |
| AMD | Dr6 0-3,13-15 | MEANINGFUL_EXACT | AMD APM B0-B3,BD,BS,BT |
| AMD | Dr6 11,16 | OS_CONTROLLED_EXACT_OR_REFUSE | Deliberately stricter than old reserved-one layout; current extension/feature contract not certified, so require exact equality |
| AMD | Dr6 4-10,17-31 | FIXED_ONE | APM adjacent prose/diagram explicit read/set-one; conflicting table range noted above |
| AMD | Dr6 12,32-63 | FIXED_ZERO | APM read-zero/upper MBZ, not generic reserved masking |
| AMD | Dr7 0,2,4,6,8-9,13,16-31 | MEANINGFUL_EXACT | Local enables,legacy LE/GE retained,GD,RW/LEN |
| AMD | Dr7 1,3,5,7 | OS_CONTROLLED_EXACT_OR_REFUSE | Global enables remain exact |
| AMD | Dr7 10 | FIXED_ONE | APM explicit read-as1 |
| AMD | Dr7 11-12,14-15,32-63 | FIXED_ZERO | APM explicit RAZ/upper MBZ |
| Unknown vendor/register/bit | all | UNKNOWN_REFUSE | No Intel rule applied to another vendor |

Intel Dr6 masks meaningful=0x1E80F,one=0xFFFE07F0,zero=0xFFFFFFFF00000000,
OS=0,unknown=0x1000. Intel Dr7 masks meaningful=0xFFFF2B55,one=0x400,
zero=0xFFFFFFFF00000000,OS=0xAA,unknown=0xD000.
AMD Dr6 meaningful=0xE00F,one=0xFFFE07F0,zero=0xFFFFFFFF00001000,
OS=0x10800,unknown=0. AMD Dr7 meaningful=0xFFFF2355,one=0x400,
zero=0xFFFFFFFF0000D800,OS=0xAA,unknown=0.

The exact observed c43 pair is **NOT architectural equivalence under either
conservative profile**: its delta includes11 and16, which are never normalized.
Intel documents RTM16 as always1 only when RTM is unsupported; c44 deliberately
does not use that conditional relaxation. The owner vendor/feature state was
missing, so the old all-one pattern is not a proof of harmless representation.
Unknown fields present in either saved/current snapshot refuse even if equal.
Unknown zero fields do not participate; no unknown difference is ignored.
For AMD11/16, OS_CONTROLLED_EXACT_OR_REFUSE denotes the conservative rule for
these OS-returned context fields with an uncertified newer-feature contract;
it is NOT a new verified claim that Windows assigns a specific semantic role to
either field. No AMD RTM capability is invented; the unsupported feature guard
and exact comparison avoid needing that claim.
An additional conservative candidate boundary requires CURRENT Dr6[11,16]=1.
This is NOT a fixed-bit classification or normalization: both fields must STILL
match saved exactly. Current active-low0 states refuse even when raw-equal,
because their feature/OS-state contract is not certified here. This also avoids
accepting a current0 where a checked no-RTM/fixed-one contract would require1.
It intentionally refuses some otherwise plausible exact states; it never treats
a meaningful saved0/current1 transition as equivalent.

Comparison may be equivalent only when addresses, meaningful and OS-exact fields
match, unknown fields are absent, and EVERY current fixed-one/fixed-zero bit has
its required value. Saved/current differences must consequently lie solely in
documented fixed groups. Raw exact is reported independently, including raw true
with architectural false when a current fixed value is wrong. No actual register
is normalized or written by this helper. New CPUID leaf0 vendor bytes, signature
and leaf7 words are reported without injection/overrides. Unknown vendors refuse
before arming. Old/new undocumented extensions receive no blanket support claim.
The checked AMD DR7 profile additionally refuses if leaf7 is unknown or reports
RTM support (EBX11): that feature combination is not certified by the checked AMD
edition. Intel never uses feature absence to discard BLD/RTM status differences.

## Fixture acceptance and three readbacks

The original saved/before/after JSON and exact comparator remain unchanged.
Root schema4/taskc44 adds debug_register_comparison, documentation revision,
vendor masks, RAW_EXACT and ARCHITECTURAL_CANONICAL for each snapshot. All3
existing post-poll suspended-thread readbacks now execute BEFORE final result
assertions, without intervening repair/write/retry or an extra watched access.
Each raw six-register snapshot, both comparisons, capture-complete state,
raw stability, all-equivalent and comparison-result stability are retained.
API failure retains partial data and exits FAIL; no absent capture becomes PASS.

Native acceptance uses the explicitly named ARCHITECTURAL_CANONICAL comparison:
all4 after/readback comparisons must pass, raw current snapshots must be stable,
leases inactive, pending0 and the original canRetire/false poll requirements hold.
No setting/MSR read or mitigation mutation is added. A later exact predicate or
machine-state/identity/lifetime assertion can still fail the overall probe.

Wine currently returns exact-restored API contexts with Dr6=0/Dr7=0x10, which do
NOT satisfy current hardware fixed-one values. Preserve the old owned Wine
RAW_EXACT regression semantics, explicitly labeled RAW_EXACT/owned_Wine_only_not_
architectural_certificate; never report its architectural comparison as true.
Even this raw branch requires known vendor, meaningful/global fields exact,
unknown fields absent, all3 raw readbacks exact/stable and all old witness/state
checks. It is not a native acceptance bypass or canonicalization runtime proof.

## Tests, handoff and unresolved work

`bash source/tools/test_debug_register_comparison.sh` runs owned sanitized pure
classification/comparison/stability tests plus explicit synthetic JSON parsing.
It tests each mask partition, meaningful/control/global/address/unknown/fixed
violations, the actual c43 VALUES as synthetic input (refused), canonical true
for a fixed-only difference, all3 readbacks, drift and missing observations.
Existing c43 pure diagnostics and c42 mitigation tests remain unchanged. Real
Wine output is independently parsed by check_continuation_diagnostics.py.

Counts, all required exact commands and unperformed checks are in CODEX_RESULT
and V01C44_STATUS.json. After tests pass, publish only the c44 standalone artifact
and provenance/licenses, preserving all historical native evidence/binaries.
One owner game-free native command from analysis/native-probes/c44 remains:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Native CPU identity,3-readback stability and both results remain unmeasured until
that run. A continued FAIL is legitimate: the observed status-bit differences
have not been justified as fixed representation. Latest official vendor-edition
delivery remains unavailable, not silently certified by the pinned mirrors.
No production integration/restoration change, exact-slot watch/value read,
producer traversal, game/Update-WS/log/scan or gameplay write is authorized.
