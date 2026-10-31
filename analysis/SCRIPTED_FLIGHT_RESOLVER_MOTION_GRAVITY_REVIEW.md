# Direct Scripted Flight resolver / motion-gravity provenance review

2026-10-31 JST. Branch `codex/v01c20-edi-value-provenance`, baseline
`5685d9819ce5f8daebc73fc31a0359c2e5f3c097`. **REFERENCE_TRACE_COMPLETED;
CURRENT_GAME_RELATION_VERIFICATION_PENDING.** No execution/install/vendoring.
Ground-name/registration route stays STOPPED; no scanner/download rerun.

## Authenticated actual input and analysis

Owner ZIP actually opened recursively in memory:
ScriptedFlight_WSU_Compatible.modular -> scripts/ScriptedFlight.script ->
ScriptedFlight.dll. ZIP SHA256
`c1f43885f6138143ad54cf0d6d3be86b1e51b0abc27cc51155bb9de6e93794f7`;
DLL162816bytes SHA256
`7330c68fa87da58a50a6ee4c477d17339e5deb28a82cf77f7b16c3c2092b7662`.
Matches accepted reacquisition. Extracted DLL/disassembly only in private /tmp;
no raw/reference function bodies or binaries in Git/releases.

Used GNU objdump Intel and pinned Capstone5.0.6, PE runtime entries for exact
function ranges. [Nine function identities](SCRIPTED_FLIGHT_FUNCTION_IDENTITIES.json)
record ranges/hashes, not bodies. Relevant reference DLL functions:
D430..11D83 dependency collection/resolution;12FD0..135CC individual resolver;
137B0..155AA semantic/relationship verifier;16920..16CBD diagnostic formatter;
3C60..3D19 setter wrapper;3410..3590 refresh;6C70..6FD0 motion-request hook;
A380..A61D restoration;8770..8D41 record capture. **All these are DLL-relative RVAs, not game addresses.**

## Recovered resolver method — not just names/nearby CALLs

D430 validates loaded AMD64 PE32+ header/image bounds/executable sections through
D270 and its own section loop. It scans executable ranges using embedded
pattern alternatives through12720. Four relevant candidate vectors are associated
with labels and original/reference canonical game RVAs:

| Reference role | Candidate vector (startup RBP-relative) | Canonical game RVA hint | Resolver call (DLL RVA) |
| --- | --- | --- | --- |
| UpdateMotion | +930 | 0x14439D0 | 103E3 ->12FD0 |
| SetGravityVector | +7D8 | 0x3510EA0 |10597 ->12FD0 |
| GravityOrientationOwner | +7F0 |0x134F000 |10604 ->12FD0 |
| RequestSimpleMotion | +978 |0x11A1CD0 |10671 ->12FD0 |

These canonical game RVAs are recovered reference constants, **not authenticated
current WS game bindings**. Actual runtime address may depend on reference shift
resolution. Candidate vectors hold8byte addresses;12FD0 accepts exactly one,
refuses multiple with “ambiguous AOB cannot use shift fallback”. Zero candidates
may use canonicalRVA + loaded image base + signed shift only with the upstream
three-anchor consensus flag, overflow/bounds/executable-range checks, then append
one address. Relevant sites13062..130B0,1316F,13281,134CF..1355C.
D430 startup counts/sorts unique anchor deltas (canonical difference), establishes
consensus and reports selectedShift; later semantic verifier is still mandatory.
This is neither unchecked fixed offsets nor a proof that every build is supported.

At10F34..10FF0 resolved addresses are stored in the private context rooted at DLL
27588: +58 UpdateMotion, +78 SetGravityVector, +80 GravityOrientationOwner,
+88 RequestSimpleMotion. Four results passed separately into137B0 through112AC.
A result from individual resolver is not enough to pass this initializer.

### Build and integrity gate limits

Reference startup3E60 computes a file SHA256 using CryptCreateHash CALG0x800C,
ReadFile64KiB chunks/CryptHashData and retrieves32bytes; reviewed paths also check
loaded PE layout, executable ranges, page properties, candidate ambiguity and
semantic instructions. Merely calculating a hash is NOT an established fixed
SHA allowlist: a comparison against WS's exact EXE digest was not established
in the reviewed reference gate. Do not claim its shifted fallback authenticates
our build. It retains entry-prefix snapshots and later memcmp checks before use;
these are bounded reference safeguards, not WS ownership/lifetime certificates.

## R8/R9 provenance and real comparison logic

16920 is a stream-based diagnostic formatter, confirmed independently. It receives
label and two mismatching candidate values on failure branches, **not motion or
gravity calls**. Critical successful-path computations before those diagnostics:

* DLL14732..1475E: read owner candidate, require CALL E8 at owner+67; signed rel32
  decode gives owner+6C+disp32. Store computed target in verifier local+2E0.
  DLL14BD2..14BE0 compares it against separately resolved SetGravityVector.
  Failure14C70..14C8C formats computed R8 vs resolved R9. The zero at149AF is
  the failure alternative; it is NOT always-zero on the successful path.
* DLL149B8..149E9: require E8 at UpdateMotion+4E6; signed rel32 gives
  UpdateMotion+4EB+disp32. DLL14BE6 compares it with separately resolved
  RequestSimpleMotion. Failure14C96..14CAE is diagnostic-only.
* DLL14EFF..14F7C checks RequestSimpleMotion entry and two local argument-marker
  instruction regions (+74,+A2), with distinct entry/motion/facing error labels.
* DLL14F81..14FDF checks setter load/store markers at entry,+C,+17 and RET+4E.
  Other verifier checks relate pointer-cache/velocity/input/anim fields and count
  expected calls; cannot discard them when considering production use.

This establishes actual **resolved candidate -> decoded call target equality ->
semantic markers -> stored result** in this reference. It does not establish
current game call arguments or lifetimes by labels alone.

## Actual use and lifecycle / restoration

Request target (context+88 / DLL27610) is installed through the reference hook
helper at4FC8..4FFC with hook entry6C70 and original trampoline DLL27990.
6C70 checks trampoline, TLS-associated owner, active mode2/failure flag, and
protected cached owner records. It saves incoming RCX/RDX/R8 as owner/unchanged
second argument/original third argument. A valid cached12byte record is checked
through31E0;3410 refresh must succeed. At6F38..6F45 it passes RCX=original owner,
RDX=unchanged original second argument, R8=temporary cached12byte record to the
**original trampoline**, not16920. On failure it records a failure state and
forwards original RDX/R8 at6FAD..6FB6. Reference treats third argument as facing;
this is observed reference call usage, not a ready WS Win64 typed declaration.
It does not demonstrate an independent new collision/movement request replacing
native locomotion: the request itself already exists, and motion's second
argument is preserved by this hook.

Setter target (context+78 / DLL27600) is loaded at3C7A.3C60 tests non-null object/
target, checks each of three float components via imported classification helper,
compares current first16 target bytes against its saved prefix (3CF2), and at3D12
actually calls that target with RCX=object and RDX=3-component source. Wrapper's
AL success is its own result, not evidence of native return type.

Refresh3410 obtains an active record keyed to owner; follows cached object chain,
requires terminal object equals saved record+58, adds reference gravity subobject
offset, and passes record+30 vector to3C60 at3548. It also changes two adjacent
mode/scalar fields and marks dirty+74. These are reference-specific writes;
vector setter alone is not its entire motion/gravity behavior.
Record capture8770..8D41 reads the owner chain/vector at8A10..8A70 and stores
the terminal object at record+58, original12byte vector at+60/+68, and mode/scalar
values at+6C/+70 (8C9E..8CB5). RestorationA380 uses record+60 saved vector, revalidates the same chain/object,
invokes3C60 atA51F and restores saved record+6C/+70 fields. Calls from6B62
(reference deinit hook) and7B72 (transition/stop flow) establish cleanup usage.
Identity/setter failure can return without clearing/restoring; no unconditional
restoration or cross-thread lifetime guarantee inferred. Hook entry counters,
original forwarding, integrity rechecks and module pinning are present, but no
WS continuation/ownership certificate follows from them. WS C52 remains intact.

## Capability established / exact remaining blocker

**VERIFIED-STATIC, reference only:** real resolved motion-request hook with facing
substitution and original request forwarding; a real vector-setter indirect call;
owner-associated vector/mode/scalar save/restore; decoded-call/independent-target
crosschecks. Stronger than string discovery; it is actual resolver/use data flow.

**UNKNOWN:** current exact game four identities and call relations, actual native
vector frame/units/magnitude, argument ownership, input-to-velocity producer,
collision sweep/support/grounded behavior, stable object lifetime and compatibility
with WR/ground Sprint, whether arbitrary gravity reaches contact queries. Flight
control and gravity direction writes do not prove grounded arbitrary-up Sprint,
wall adhesion, safe Position replacement or grounded motion submission.

## First independent current-build verification — one bounded operation

New `verify_motion_gravity.py` checks original owner EXE under WS's fixed SHA
`c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.
This is a conservative **zero-shift canonical subset** of recovered reference
validation, not a port of its AOB/consensus algorithm. Only the four constants
above, two recovered CALL offsets and7 minimal marker hashes are inspected.
No arbitrary scan/RVA/shift CLI, adjacent-byte follow-up or live process.

It validates PE/file ranges/readonly executable sections, bounded ordered runtime
table (<=600000 entries/8MiB), exact/partial runtime entry status, decoded CALL
relation including available owner range, and marker hash equality. No raw code
is exported. Leaf/no-runtime entry remains boundary partial, not an invented
function certificate. Does not prove CFG reachability of call at offset.
Fullfile<=512MiB, four targets/two calls/seven markers,16byte identity hashes,
JSON<=32KiB, exclusive output; unsupported hash/ranges/caps refuse. Missing marker
or relation produces MISMATCH without searching for shifted substitutes. Even
REFERENCE_RELATIONS_MATCH retains ABI/lifetime/contact/production_permission=false.
A match is the first current-file anchor evidence, not permission to call/write.

### ONE PowerShell command (no Update-WS / Install / game)

```powershell
$game = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2"
$src = Join-Path $game 'Mods Library\Mods-Developing\MSM2-WS\_source'
$branch = 'codex/v01c20-edi-value-provenance'
$origin = & git -C $src remote get-url origin
if ($LASTEXITCODE -ne 0 -or $origin -ne 'https://github.com/deluxxxe-dev/MSM2-WallSprint.git') { throw 'Wrong WS origin' }
$current = & git -C $src branch --show-current
if ($LASTEXITCODE -ne 0 -or $current -ne $branch) { throw 'Wrong WS branch' }
& git -C $src fetch origin $branch
if ($LASTEXITCODE -ne 0) { throw 'Fetch failed' }
$commit = & git -C $src rev-parse "origin/$branch"
if ($LASTEXITCODE -ne 0 -or $commit -notmatch '^[0-9a-f]{40}$') { throw 'Remote head unavailable' }
$body = @(& git -C $src show ($commit + ':source/tools/windows/Verify-Motion-Gravity.ps1'))
if ($LASTEXITCODE -ne 0) { throw 'Verifier entry unavailable' }
$entry = Join-Path ([IO.Path]::GetTempPath()) ('WS-motion-gravity-' + [guid]::NewGuid().ToString('N') + '.ps1')
try {
    [IO.File]::WriteAllText($entry, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $entry -SourceRoot $src -GameRoot $game
    if ($LASTEXITCODE -ne 0) { throw 'Read-only verification mismatch/refused; return JSON if generated, otherwise the error' }
} finally {
    if (Test-Path -LiteralPath $entry) { Remove-Item -LiteralPath $entry -Force }
}
```

Return `MSM2-WS\logs\MSM2-WS-motion-gravity-verification-YYYYMMDD-HHMMSS.json`
(optional suffix), including mismatch/refusal. No EXE/code upload, reference
locator, DLL install, unchanged r7 test or package update. Dependencies staged
from fetched commit; requires existing WSL Capstone5.0.6 or explicitly refuses.
