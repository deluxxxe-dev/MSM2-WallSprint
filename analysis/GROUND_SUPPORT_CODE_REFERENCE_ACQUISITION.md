# Ground locomotion names -> decoded reference / runtime identity acquisition

2026-10-31 JST. Task baseline `f95dce5` on
`codex/v01c20-edi-value-provenance`. No main merge.
**Next-stage offline acquisition implemented/tested. Actual native support/up/
gravity consumer remains UNKNOWN pending owner-local exact EXE.**

## Accepted input / actual source investigation

Read AGENTS required docs, current state/task,
[owner anchor summary](GROUND_SUPPORT_OWNER_ANCHORS_20261031.md), prior
[ground investigation](GROUND_SPRINT_SUPPORT_ANCHOR_INVESTIGATION.md) and
[completed intent/architecture design](DOWNWARD_INTENT_IMPLEMENTATION_DESIGN.md).
Owner metadata107 names/243 data refs, exact pinned EXE accepted. Do not rerun
that names-only acquisition. No full owner JSON or current EXE is assumed local.
Existing SDK/reference registry evidence is generic ComponentRegistry/component
lookup and WSU's `HeroStateWallRunMoveLocal`; neither authenticates ordinary
SprintMount or a ground-controller API. No fresh reference disassembly claimed.

Inspected previous lexical TERMS: mostly Sprint/Grounded compounds, missing
plain Run/Move/Locomotion. Owner's HeroStateSprintMount / Local / Remote /
MountToVault and nearby IssueDismountCollision are NOT continuous ground Sprint.
No RVA from those names is chosen as authoritative. Actor generic components,
WR scalar/capture paths, or Sprint animation/config names cannot replace ground
controller/field consumption evidence. Prior intent findings preserved, not
rediscovered as this task's deliverable.

### Narrow target family and deterministic ranking

New fixed family derives from actual `HeroStateWallRunMoveLocal` naming shape
(reference lead, not a game-type ABI), the task's ground-locomotion concept and
owner-observed secondary labels. Seek complete NUL-delimited ASCII/UTF16 names,
not arbitrary code/string fragments:

0. HeroState Ground/GroundRun/GroundMove/GroundMovement/GroundLocomotion/Grounded,
   Run/RunMove/Sprint/Move/Movement/Locomotion, optional Local, including syntactic
   MSVC RTTI wrappers. This is a search hypothesis, NOT verified exact symbols.
1. Exact observed AutoUpdateGroundState (diagnostic label, not a controller).
2. Exact MoveAccelSprint / StickConfigSprint (config/input leads, not state types).
3. Exact HeroSprintStartEvent / HeroSprintStopEvent (event leads).

Exclude Wall/Air/Swim/Mount/Vault/Bot/Vehicle/Combat/Attack/Anim/Remote forms before
ranking. Sort rank/name/RVA, select at most8 of at most64 relevant names; report
omitted count, never claim coverage or choose a controller from first match.
Speculative names with no actual EXE occurrence supply no addresses.

## Concrete code relationships obtained in ONE owner-local operation

`source/tools/ground_support_code_refs.py` rederives addresses under the existing
SHA pin, reuses validated PE/range/string helpers from discover_ground_support,
and uses pinned Capstone5.0.6. It does NOT call the previous metadata-only export.

1. Collect limited ranked targets and aligned read-only RVA32/VA64 words pointing
   to them (one metadata edge). Actual bytes validate the edge, not its registry/
   RTTI semantics; constants can coincide. No multi-hop pointer chase.
2. Targeted executable-byte prefilter only for REX.W RIP-relative LEA/MOV pointing
   to those exact names/words. Overlapping prefilter starts are considered.
   Decode each hit and require exact7byte LEA/MOV, RIP memory operand and target.
   This is not a generic all-game disassembly. Other addressing/forms intentionally
   outside scope; absence does not disprove ground controllers.
3. Validate declared exception directory, bounded ordered/nonoverlapping runtime
   entries, executable ranges and available unwind header extents. For each
   reference export containing begin/end/unwind RVA and original function SHA,
   not complete body. PE range identity is not semantic controller identity;
   unwind layout checks are not ownership or handler/chain semantics proof.
4. Decode from runtime entry toward reference only on straight-line prefix.
   If successful label `runtime_entry_straight_line_boundary`; this is static
   instruction-boundary proof from PE entry, not actual execution or argument
   semantics. If previous instruction overlaps the hit, discard byte coincidence.
   Prior CALL/branch/RET, decoder gap, no covering runtime, byte/instruction cap
   leave explicit **candidate** provenance. No CFG continuation/indirect targets.
5. Export reference plus up to4 following decoded instructions within48bytes,
   stopping at control boundary; annotate first local memory operand or direct
   call (including target's exact runtime-entry relationship if present). This
   locally inspects the first available follow-up rather than requesting adjacent
   bytes next. It does not relabel a name-reference call as a support/gravity
   consumer or assume an object register. Frame/field meaning stays UNKNOWN.
   Omit the last instruction record if excerpt would equal an entire tiny body.

`selected_controller=null`, `registry_authenticated=false`, ownership=false and
first_consumer_authenticated=false always. Decoded references/functions are
candidates for subsequent consumer review. No state change, game ABI, root field,
world-up or arbitrary-up conclusion from synthetic runs. A local follow-up
memory instruction is exact code evidence, not a validated actor/controller field.

### Fixed aggregate caps and failure behavior

Existing512MiB original-file SHA/strict PE/range/overlap checks; metadata192MiB,
relevant names64/selected8; data refs128; targeted executable scan192MiB and
2million prefilter hits; runtime600000 entries/8MiB; code refs32/functions16;
function hash bytes2MiB; each boundary prefix4096bytes/8192 aggregate instructions;
follow-ups <=5 instructions per reference,48bytes each; total exported bytes512;
final JSON128KiB. Strict caps refuse with compact reason, no partial authoritative
selection. Boundary budget caps downgrade to candidates explicitly. Ranking8 is
a disclosed subset with omitted count, not discovery completeness.

`CODE_REFERENCE_CANDIDATES` exit0 = evidence collection only;
`NO_CODE_RELATION_IN_FIXED_SCOPE` exit2 records exact scope limitation and name/
relationship counts. `REFUSED` exit2 supplies fixed reason. Never widen to broad
capture, fall back to WR scalar scans, select an ambiguous first match, or overwrite
prior output. Fragments remain private WS logs, never Git/release assets.

## One-shot Windows command

Existing WSL analysis-python with Capstone5.0.6 is required (already established
in owner setup). Wrapper stages BOTH dependency files from fetched commit,
checks interpreter/decoder and fails clearly if unavailable. No implicit install,
Python fallback, runtime update, game launch or full EXE upload. Execution policy
is bypassed for the child process only, as required by actual owner experience.

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
$body = @(& git -C $src show ($commit + ':source/tools/windows/Discover-Ground-Code.ps1'))
if ($LASTEXITCODE -ne 0) { throw 'Code discovery entry unavailable' }
$entry = Join-Path ([IO.Path]::GetTempPath()) ('WS-ground-code-entry-' + [guid]::NewGuid().ToString('N') + '.ps1')
try {
    [IO.File]::WriteAllText($entry, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $entry -SourceRoot $src -GameRoot $game
    if ($LASTEXITCODE -ne 0) { throw 'Offline acquisition stopped; return JSON if generated, otherwise the error' }
} finally {
    if (Test-Path -LiteralPath $entry) { Remove-Item -LiteralPath $entry -Force }
}
```

Return `Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-ground-code-refs-YYYYMMDD-HHMMSS.json`
(or collision suffix), including refusal/no-relation JSON if generated.
Do not return EXE/full raw functions or repeat previous names-only command.
Current runtime/config/packages remain r7; no Update-WS/Install mods/game test.

## Focused validation

See CODEX_RESULT for commands/results and publication. Synthetic decoder tests
cover positive references and PE runtime identity; literal byte coincidence inside
MOV-imm rejection; straight-line vs prior-control ambiguity; one metadata edge;
Mount/Bot/etc exclusion; deterministic ranking; exact hash/ranges/runtime/caps,
malformed files, exclusive output; first local memory/direct-call evidence and
no full tiny-body bytes. Existing PE helper regression rerun because imported.
PowerShell wrapper mock tests cover fresh dependency staging, Unicode/space paths,
collision preservation, unchanged worktree/EXE and unavailable WSL refusal.
These are tooling results, not real native support/gravity evidence.
