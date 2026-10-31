# Current motion/gravity contract report and future observation discriminator

2026-10-31 JST; task baseline `823e10c0ac4e03aa0a6d7d1a4edfd7b01de9528d`;
branch `codex/v01c20-edi-value-provenance`. No main merge.
**Accepted current-file identities; argument/object report ready, native report
contents pending owner-local EXE. No callable API/grounded capability claim.**

## Accepted facts / current contract boundary

Read [accepted owner resolution](MOTION_GRAVITY_OWNER_RESOLUTION_20261031.md),
[relocation review](MOTION_GRAVITY_RELOCATION_REVIEW.md),
[direct reference trace](SCRIPTED_FLIGHT_RESOLVER_MOTION_GRAVITY_REVIEW.md),
[downward design](DOWNWARD_INTENT_IMPLEMENTATION_DESIGN.md) and AGENTS required
docs/shared reference knowledge. Original EXE/current bodies are not available
in this cloud checkout; no new native instruction findings are invented.

| Current role | Accepted exact range | Native contract already established | Missing contract |
| --- | --- | --- | --- |
| UpdateMotion |21248208..21249549 |277 reachable instructions; exact SHA and reachable request CALL21249462 |incoming reads/writes, outgoing argument origins, real velocity/input provenance |
| RequestSimpleMotion |18488272..18488482 |53 reachable instructions, exact SHA,7-check set includes entry/motion/facing markers |which data are copied/read/stored/forwarded; downstream effects and pointer lifetime |
| GravityOrientationOwner |20246272..20246459 |40 reachable instructions, exact SHA; CALL20246375->setter |owner->subobject construction and vector source, frame and current state applicability |
| SetGravityVector |55653024..55653103 hypothesis extent |exact accepted79byte hash,19 decoded instructions, RET bound; **no .pdata** |function entry remains PARTIAL; field reads/stores don't prove allocation/lifetime/unit/call safety |

All4 were independently pattern-resolved, setter delta+9216 versus other3 -256.
Locations and seven markers/two relations are accepted: **no repeat discovery,
pattern/canonical/relocation/name scan**. Exact full hashes are pinned in new
report source. Reference hook preserves original second argument and substitutes
third/facing argument, but that alone does not establish current native RDX/R8
field semantics, contact/movement submission or ground Sprint. Current argument
contract is not upgraded from reference labels.

## Concrete local analysis in one operation

`motion_gravity_contract.py` reads only the fixed original EXE under the WS SHA,
validates each accepted complete function hash/range and exact reachable CALLs,
then computes bounded static **MAY** provenance across all4 known CFGs. It uses
only `resolution.Image` PE/CFG primitives: it never invokes resolve(), pattern
selection, canonical checking or a discovery profile. Windows staging deliberately
omits motion_gravity_patterns.json. Accepted hash/range/CFG drift refuses rather
than rediscovering or expanding the scope.

Output questions answered locally from actual decoded instructions:

* `incoming_register_uses`: physical register access widths and entry-value origins
  for RCX/RDX/R8/R9 and XMM0–3; distinct site counts/at most8 witness RVAs.
* `access_candidates`: read/write/RMW/unknown width and symbolic root+byte offset.
  Entry-register roots are untyped bit values until actual memory use; full64
  loads used as later bases become bounded opaque load64 chains, not allocated
  pointers. Image/stack roots separate. This exposes owner/subobject chain
  candidates without reading any live object or field value.
* Stores distinguish full64 incoming argument-value copies from values loaded
  through arguments. The former is a **pointer-retention candidate only**:
  integer interpretation, destination ownership and persistence remain UNKNOWN.
  Memory-load-to-store/SIMD lane links are copy candidates, not byte/vector equality.
* `call_candidates`: outgoing RCX/RDX/R8/R9, floating banks and known stack slots,
  direct target/current PE range/exact-entry metadata and accepted role if one of
  the4; indirect operand origin if present. No other callee body is followed.
  Unknown callee mutation/capture is explicit, so absence of a local pointer store
  NEVER proves no retention.
* `return_candidates`: conservative RAX/XMM0 origin sets. `unknown_barriers`
  identify unmodeled register transforms, SIMD lane uncertainty, unknown callees
  and unresolved scope edges. Branches are possible alternatives, not observed
  execution. Exact address expressions survive only equality at joins.

Windows x64 nonvolatile-register convention is used for call clobbers only,
not invented typed signatures. Calls invalidate volatile registers/spill facts;
unproved object writes invalidate possible stack aliases. Partial writes and
sign/zero extension discard exact pointer origin. SIMD lanes are not fully
modeled. Stack spills are modeled under bounded exact addresses, with overlap
invalidation. Memory alias/allocation/retention remains unproven. Register-only
leaf candidate requires no access/call/barrier and a conservative supported
register-op set; it too retains lifetime uncertainty. Capped origin TOP is
absorbing so loop joins cannot masquerade as renewed exact evidence.

### Identity/size/fail-closed rules

EXE512MiB; exact four accepted ranges and SHA in source are authoritative,
bounded by four<=64KiB ranges.
Existing CFG decoder30000 aggregate instructions/16 ranges; dataflow8192 state
evaluations per role. Per role:512 access variants,128 stack entries,128 register-
use groups,64 call variants/64 return variants,8 origins/3 load-chain depth.
Final JSON192KiB. No raw instruction listing/bytes/full bodies/EXE upload.
Exclusive output preserves earlier JSON/EXE. Unsupported hash/changed function/
range/CFG/required call/caps refuses. Setter must retain no covering .pdata, no
intervening entry, bounded RET/no-call shape; it stays **PARTIAL** regardless of
complete local memory/read/write descriptions. No object certificate follows.

## Minimal future observation-only discriminator — DESIGN ONLY

No new hook/version/read is implemented or requested now. Static report first
selects actual field spans and call arguments; owner/thread/lifetime evidence
and hook/continuation audit must separately admit ANY later live observation.
Do not hook the no-.pdata setter solely on its bounded leaf status.

One later bounded observation, using only admitted PE-entry/call sites:

1. Fixed window<=2seconds/<=16 records per manually marked phase: ordinary ground
   Sprint, ground neutral/reinput, WR on one wall. Repeat camera heading once
   only if needed to distinguish frame. Manual phase is an owner label, not
   automatic proof of native ground state or contact.
2. First collect entry/thread/native-call counters and opaque argument identities,
   pairing to existing guarded Hero/Transform snapshot and session generation.
   Determine whether RequestSimpleMotion and gravity owner actually occur in
   each phase, whether owner identity changes, and whether chain/slot exists.
   No fallback dereference when an object is missing or unproved.
3. Only spans justified by the current static report AND separately proven
   borrowed-argument/object lifetime may be copied as bounded finite vectors
   during that native call. Compare motion-candidate versus facing-candidate
   vectors with existing world displacement and, in WR only, validated wall normal.
   Relate held/reversed/neutral intent to owner marks; no raw WASD/XInput substitute.
   Camera-local/actor-local/world frame is a hypothesis until these comparisons
   discriminate it; rates/scales/units cannot be guessed from names or widths.
4. Release, state/owner/thread change, Deinit, stale generation or unreadable span
   closes observation and emits unavailable reason. No object caching beyond its
   proven lease, no state/velocity/gravity writes, no suppression of original
   requests. Contact presence must come from a proved contact signal; normal or
   continued motion alone is not a grounded flag. If such signal is missing,
   contact lifecycle stays UNKNOWN rather than adding another field guess.

Decision: entry/call presence and verified spans in both states can justify
subsequent ABI/lifetime research; WR-only owner/chain means no ground-state reuse
assumption. Even matching vector direction does not prove native collision/
adhesion/grounded support under arbitrary gravity. This design does not authorize
that observation. C52 and existing restoration/ownership remain unchanged.
A later actual behavior patch must assign its exact new version/log and specify
Update-WS -> Install mods -> correct ON/default -> Run; test lateral/repeated
reversal, down/lower diagonals held3seconds, upper regression and RT-neutral/
reinput. No unchanged r7 test requested for this offline report.

## ONE current owner operation — PowerShell (offline only)

Existing WSL Capstone5.0.6 required; fresh4 Python dependencies from fetched commit,
no discovery JSON. Child-only execution policy bypass, no checkout/config edits.

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
$body = @(& git -C $src show ($commit + ':source/tools/windows/Report-Motion-Gravity-Contract.ps1'))
if ($LASTEXITCODE -ne 0) { throw 'Contract report entry unavailable' }
$entry = Join-Path ([IO.Path]::GetTempPath()) ('WS-motion-contract-' + [guid]::NewGuid().ToString('N') + '.ps1')
try {
    [IO.File]::WriteAllText($entry, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $entry -SourceRoot $src -GameRoot $game
    if ($LASTEXITCODE -ne 0) { throw 'Offline contract report refused; return JSON if generated, otherwise the error' }
} finally {
    if (Test-Path -LiteralPath $entry) { Remove-Item -LiteralPath $entry -Force }
}
```

Return only `MSM2-WS\logs\MSM2-WS-motion-gravity-contract-YYYYMMDD-HHMMSS.json`
(optional collision suffix), including refusal. No Update-WS/Install/game launch,
no unchanged resolution rerun, EXE/body upload or serial next-byte capture.
