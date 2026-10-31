# Current-EXE motion/gravity relocation: reference mechanism and bounded verifier

2026-10-31 JST; baseline `ba979e1471cdc9eec42c9da52d4e6f48098efe30`;
branch `codex/v01c20-edi-value-provenance`. No main merge.
**RELOCATION_OPERATION_READY; current-file result pending.**

## Accepted mismatch and executed reference research

[Owner canonical mismatch](MOTION_GRAVITY_OWNER_MISMATCH_20261031.md) is accepted:
all7 markers and both CALLs fail at unshifted hints. No repeated canonical test.
It rejects zero shift, not the reference's full pattern/relocation mechanism.
Original [direct trace](SCRIPTED_FLIGHT_RESOLVER_MOTION_GRAVITY_REVIEW.md) still
establishes target use/restore; no more name/registration/callee slice acquisition.

Private ScriptedFlight DLL remains actually available; verified SHA
7330c68fa87da58a50a6ee4c477d17339e5deb28a82cf77f7b16c3c2092b7662.
Decoded reference DLL D430 startup,12720 alternative selector,12A40 masked matcher,
12FD0 role resolver,BE60 consensus helper and137B0 semantic verifier using
Capstone5.0.6 and prior objdump proof. No execution/install or reference vendoring.
Raw DLL/functions remain private /tmp; only minimal signature descriptors enter
owned offline tooling. No runtime/package/version/config changes.

### Actual entry/alternative/mask method (DLL-relative addresses)

12720 reads primary pattern descriptor from its incoming stack argument first,
calls12A40 and returns primary hits if nonempty. Only if empty does it scan the
second/interior descriptor. For those hits it reads the PE exception directory,
requires actual containment `[BeginAddress,EndAddress)`, maps to that exact Begin
and deduplicates. It does NOT choose the nearest preceding runtime start.
Our implementation additionally requires decoded entry-to-interior reachability;
a hit inside another instruction is insufficient.

| Role | Entry pattern string RVA / length | Interior string RVA / length | Interior wildcard byte positions |
| --- | --- | --- | --- |
| UpdateMotion |21085 /39 |204E4 /40 |3–6 |
| SetGravityVector |20846 /38 (wild8–11) |20978 /41 |none |
| GravityOrientationOwner |21274 /39 (wild19–22) |20340 /41 |33–36 |
| RequestSimpleMotion |1FBD9 /33 |1FF7F /38 |15–18,34–37 |

Lengths are matched bytes, not text-string lengths. Actual masks were recovered
from these exact strings, not guessed offsets. Fixed bytes and full-byte ??
wildcards are implemented as masked SHA256 plus an8byte fixed search seed and
explicit wildcard indices, preserving complete signature selectivity without
storing wholesale signature strings/function bodies. Source JSON contains8 such
descriptors and exact reference SHA; file itself is SHA-pinned in the tool.

12A40 parses spaced hex / ?? and searches the executable ranges.12720 primary
nonempty results retain ambiguity;12FD0 accepts exactly one address. >1 refuses
and cannot be repaired by shift fallback. Zero final hits can use canonicalRVA +
selected signed shift only if consensus is established and address is in executable
image bounds; later137B0 relationships/markers still mandatory.

BE60 (called with min3 at10107) sorts candidate deltas, counts equal values,
requires a unique best bin with>=3 votes, rejects ties/minimum failure. Startup
collects delta votes only from unique role vectors. Therefore similar near-entry
numbers do not constitute anchor proof. The full reference uses a larger role
set; our bounded subset uses only the four authenticated role/signature pairings,
three DISTINCT independently PE-entry/CFG-proven roles to infer a missing fourth.
It never claims to reproduce all reference fallback eligibility or semantics.

## Independent current-file operation

`resolve_motion_gravity.py` reads original EXE under fixed WS SHA
c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9.
It stages no DLL, runs no game APIs and accepts no arbitrary hash/RVA/shift option.

1. Validate PE32+/AMD64, nonoverlapping file/image ranges and bounded ordered
   runtime table. Scan only readable non-writable executable file ranges for
   the eight signature descriptors. Masked hashes validate entire patterns;
   seeds alone never select. Fixed8byte seeds total64bytes are the only signature
   byte seeds; no full functions/reference assets/code were copied.
2. Prefer entry hits; multiple hits are ambiguous. A primary hit inside an
   existing runtime function is rejected WITHOUT snapping. For interior-only
   hits, use the exact containing runtime range and prove the matched address
   is an instruction boundary reachable from its entry by bounded static CFG;
   deduplicate only independently proved same-entry results.
3. Decode direct branches on both conditional paths, calls with possible static
   fallthrough, stop indirect/unknown edges; reject overlapping decoded starts.
   No actual call-return, all-path effect or runtime reachability claim. Require
   the two selected CALL sites and seven marker starts to be reached instruction
   boundaries too. Unresolved edges remain reported; no gameplay certificate.
4. Only distinct pattern-resolved roles vote on delta from reference canonical
   role hints. All4 may relocate non-uniformly; do not force a common shift when
   independently resolved. Missing roles may use unique>=3-vote consensus;
   ambiguity/boundary failure is never repaired by votes. Range/overflow checks
   still apply, then unchanged marker/CALL crosschecks at the selected addresses.
5. Reuse `verify_motion_gravity.verify` with resolved targets (NOT a rerun of its
   old unshifted CLI). All7 marker hashes and both decoded CALL equalities remain
   required. Never relax original offset/semantic checks if code has changed.
   Output per-role candidates/proof/selection, deltas/consensus, identity hashes,
   call and marker boundaries, mismatch reasons; no raw code or bodies.

No runtime entry for setter may have a narrowly bounded reference-RET leaf
candidate: cap at+4F, require reachable RET+4E and no calls. It stays explicitly
`PARTIAL_reference_bounded_leaf_candidate`, not authenticated function ownership.
Leaf candidates never vote in shift consensus and may not cross any later PE
entry. Other missing runtime entries refuse. Fields not covered by the seven markers,
ABI, object lifetime, support/contact and arbitrary-up grounded Sprint are UNKNOWN.
Every output keeps production_permission=false even if static relations match.

### Caps / precise stopping rule

File512MiB, readonly executable scan192MiB (8 fixed pattern passes; <=1.5GiB
aggregate byte-search work), seeds100000, full matches128, patterns8,
runtime600000 entries/8MiB, function16 ranges each<=64KiB, decoder30000 aggregate
instructions, no recursive callee following, output64KiB. Malformed descriptors/
PE/ranges/profile/overlap/caps refuse explicitly. Exclusive JSON creation protects
previous evidence/EXE. Profile SHA
158f45f34cf25590cd18d586ff4b3e3194bf2a2653b47fa942e6d3d43d510c6d
is checked, including exact UTF8/LF staged bytes.

`RELOCATED_REFERENCE_RELATIONS_VERIFIED_READONLY` means static relationships only.
`UNRESOLVED_STOP_REFERENCE_RELOCATION_ROUTE`,
`SEMANTIC_OR_CALL_BOUNDARY_MISMATCH_STOP` or `REFUSED_STOP` gives the exact missing/
ambiguous/boundary/profile/cap/semantic limitation. **If this concrete method
cannot resolve the current image, stop this reference relocation route**, not
another shifted-pointer/adjacent-byte cycle or speculative production patch.
Ground Sprint remains unproven, not impossible. Current cloud has no original
EXE, so no current-image success/failure is invented from fixtures.

## ONE fresh owner operation — PowerShell

Requires existing WSL analysis-python/Capstone5.0.6. Fetches source objects and
stages3 Python files + pinned JSON profile; no source checkout/index/config edits,
no build/deploy/update/Install/game launch. Child-only policy Bypass.

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
$body = @(& git -C $src show ($commit + ':source/tools/windows/Resolve-Motion-Gravity.ps1'))
if ($LASTEXITCODE -ne 0) { throw 'Relocation entry unavailable' }
$entry = Join-Path ([IO.Path]::GetTempPath()) ('WS-motion-resolution-' + [guid]::NewGuid().ToString('N') + '.ps1')
try {
    [IO.File]::WriteAllText($entry, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $entry -SourceRoot $src -GameRoot $game
    if ($LASTEXITCODE -ne 0) { throw 'Offline resolution stopped/refused; return JSON if generated, otherwise the error' }
} finally {
    if (Test-Path -LiteralPath $entry) { Remove-Item -LiteralPath $entry -Force }
}
```

Return only `MSM2-WS\logs\MSM2-WS-motion-gravity-resolution-YYYYMMDD-HHMMSS.json`
(optional collision suffix), including stop/refusal. No EXE/full function upload,
no reference locator or unchanged canonical test, no Update-WS/Install/game.
