# Ground-Sprint native consumer investigation / metadata acquisition

2026-10-31 JST; branch `codex/v01c20-edi-value-provenance`;
start `f07e4c077b9f02c70a75be322a774e63fe7d2f59`.
**Executed source/input inventory; offline metadata acquisition READY. Native
controller identity / first support-Up/gravity consumer UNAVAILABLE.**
No new architecture comparison is substituted for the requested investigation.
Prior design and completed mapped-intent work remain valid inputs.

## Actual investigation and first missing identity

Read AGENTS required operational docs, current PROJECT_STATE/CODEX_NEXT,
DOWNWARD_INTENT_IMPLEMENTATION_DESIGN, INITIAL_WALL_AS_GROUND_REVIEW,
V01C38R7_OWNER_COMPARISON and WSU_REFERENCE_REVIEW. Fetched current Wisdoms main
and read relevant engine/reference facts. Historical posture-only tests did not
exercise ground-controller transplantation, so do not reject that route.

Executed source searches and inspected actual `sdk_bridge.cpp::sampleCallback`,
vendor `game/Actor.h`, `Transform.h`, `Native.h` and current heading/native-source
path. The SDK exposes generic components/Transform, not a typed native ground
Sprint/support/gravity API. Generic GetComponent and fixture `wallrun=false`/
`ground` tests cannot identify an actual ground-state owner or support consumer.
Existing authenticated WR callsites/captures are WR/scalar-specific; they do not
supply a ground-controller entry. No stopped WR scalar path was reopened.

`rg --files /workspace /tmp -g 'Spider-Man2.exe' -g '*SwingCore*' -g '*wallslide*'`
identified no required executable/reference DLL here (one unrelated temp subtree
was unreadable). Prior bounded102-archive inventory is recorded in the completed
design, not claimed as fresh disassembly. No game binary or native consumer was
available for direct analysis. WSU report's `HeroStateWallRunMoveLocal` and
SwingCore function0x36770 refer to WR/reference-context work, not a ground-Sprint
entry. Shared Scripted Flight labels RequestSimpleMotion/SetGravityVector are
only leads; no reference binary code was available to follow them now.

**First exact blocker:** no authenticated current-build ground state/controller
identity or code anchor. Therefore owner-relative support/up versus world vertical
cannot yet be classified. No exact Sprint RVA/function signature is invented.
No consumer, writable ownership, gravity mutability or arbitrary-up feasibility
is marked PASS. A name match would not resolve the remaining semantics by itself.

## Concrete authorized completion path B: metadata-only discovery

New independent stdlib helper `source/tools/discover_ground_support.py` and
Windows entry `source/tools/windows/Discover-Ground-Support.ps1` reuse existing
fresh-Git-stage/temp/log/output-collision conventions. Uses already installed
Windows `py -3`; does not require WSL/Capstone, build or runtime deployment.

Grounded discovery scope is the ground/Sprint **type/state-name family**, sought
in actual read-only metadata, rather than a guessed code RVA. TERMS is fixed:
Sprint, GroundLocomotion, GroundMovement, GroundMove, GroundRun, Grounded,
GroundState. These terms are discovery vocabulary from the requested controller
concept, **not verified exact game symbols**. Match complete bounded NUL-delimited
ASCII/UTF16 strings. A syntactically MSVC `.?AV/.?AU...@@` name with preceding
16byte header (in-image VA, zero spare) gets a TypeDescriptor-layout candidate,
not an authenticated RTTI/type identity. Other names remain lexical leads.

Return only exact observed bounded names, name hashes/RVAs, candidate descriptor
RVAs and actual aligned read-only data-word references (RVA32/VA64) to those
addresses. Such words can be incidental constants, relative RTTI fields or registry
links: they are **not proven registries, instruction xrefs or callable entries**.
No instruction sweep/decoding, raw bytes, functions, data values or pointer chase
are exported. No broad code dump. This single acquisition replaces unknown
names/addresses with compact exact-image candidate identities/links for the next
bounded static review; it does NOT identify the first gravity consumer yet.

### Identity, caps and fail-closed semantics

* CLI pins EXE SHA256
  `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
  (existing accepted build2.810 identity). No CLI digest override; fixture digest
  injection is a Python test API only. Parsing operates on the hashed immutable
  file snapshot, never a live process. No full EXE upload.
* Maximum512MiB file; AMD64 PE32+, validated headers/table/image base/ranges,
  <=96 sections, no overlapping raw/virtual sections or truncated raw input.
* Scan only file-backed readable non-writable/non-executable sections,
  <=192MiB total; <=192 characters/name, <=128 names, <=512 reference records,
  <=128KiB final pretty-printed JSON. Any candidate/reference/scan cap refuses
  without exporting a partial candidate selection. No code bytes are read out
  for export, and no caller-defined range/RVA is supported.
* `CANDIDATE_ONLY` / `AMBIGUOUS_CANDIDATES_ONLY`: acquisition exit0, always
  selected_consumer=null, unique_consumer_authenticated=false, ownership=false.
  Multiple matches refuse selection; they are not success proof of a consumer.
* `NO_ANCHOR_MATCH`: exit2, empty candidates, no fallback scan. `REFUSED`: exit2,
  compact fixed reason (bad hash/PE/range/cap/input). Return this JSON too.
  Unknown build does not trigger fallback or broader capture.
* Output exclusively created, existing output/EXE cannot be overwritten. Wrapper
  picks a timestamp/collision suffix under existing WS logs, stages only the
  helper from one fetched commit and cleans its own temp. Git refs/objects may
  update; working tree/index, installed scripts/INI/KIT/profile/SF do not change.

## Exact one-shot owner invocation (PowerShell)

Paste the entire block. No Update-WS / Install mods / Run / EXE upload needed.
Only Git fetch, temporary script staging and local read-only EXE metadata analysis.
The initial Windows Git/Python dependencies are already installed per owner setup.

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
$body = @(& git -C $src show ($commit + ':source/tools/windows/Discover-Ground-Support.ps1'))
if ($LASTEXITCODE -ne 0) { throw 'Discovery entry unavailable' }
$entry = Join-Path ([IO.Path]::GetTempPath()) ('WS-ground-entry-' + [guid]::NewGuid().ToString('N') + '.ps1')
try {
    [IO.File]::WriteAllText($entry, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    & $entry -SourceRoot $src -GameRoot $game
} finally {
    if (Test-Path -LiteralPath $entry) { Remove-Item -LiteralPath $entry -Force }
}
```

Return only the generated JSON at
`Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-ground-support-anchors-YYYYMMDD-HHMMSS.json`
(or collision-suffixed filename), including NO_ANCHOR_MATCH/REFUSED if generated.
No need to close/start the game for this original-file read; no runtime inspection.
If local execution policy blocks the temporary entry, preserve the error rather
than changing machine policy. This is evidence collection, not a gameplay test.

## Executed focused validation

`python3 source/tools/test_discover_ground_support.py`: expected correct synthetic
PE candidate/RTTI-layout/aligned reference discovery, ambiguity/no-match refusal
of selection, ASCII/UTF16, executable/writable exclusion, unsupported hash,
malformed/truncated/overlap/range errors, input/scan/name/ref/output caps,
nonterminated text rejection, CLI no hash bypass, Unicode/space paths and
exclusive output/EXE preservation. Actual **18 PASS**,0 SKIP. Synthetic SHA
fixtures prove tool behavior only, not native game state/consumer semantics.

`XDG_CACHE_HOME=/tmp/ws-ground-pwsh/cache XDG_CONFIG_HOME=/tmp/ws-ground-pwsh/config XDG_DATA_HOME=/tmp/ws-ground-pwsh/data /tmp/ws-proxy-pwsh/pwsh -NoProfile -File source/tools/windows/test_discover_ground_support.ps1`:
expected fresh staged helper, derived Unicode/space game layout, pinned Git
reads, collision output preservation, local changes/EXE preserved and Python
failure refusal. Actual **2 PASS groups**, Git/Python mocked; not real Windows
Python/Git acceptance. First attempt without writable XDG cache failed before
test execution on read-only /home cache; rerun used task-specific /tmp dirs,
no permissions weakened or HOME reassigned.

Native Windows EXE acquisition **UNAVAILABLE1**, owner has the pinned private file.
Actual first native support/Up/gravity consumer analysis **BLOCKED1**, awaiting
candidate identity evidence; not a failed ground-Sprint architecture experiment.
Runtime suites/build/package intentionally not run: no runtime/helper dependency
changes to r7 and task requires only relevant offline helper tests. Exact final
publication/check commands and outcomes in CODEX_RESULT.
