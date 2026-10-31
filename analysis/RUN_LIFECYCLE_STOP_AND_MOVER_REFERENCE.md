# Run name -> lifecycle stopping rule; alternate mover dependency evidence

2026-10-31 JST. Baseline `9ed44919e78623b633051c9ac3da2f722d735834`;
branch `codex/v01c20-edi-value-provenance`. No main merge.

## Actual findings / explicit stopping-rule outcome

Read current AGENTS operational docs/state/task, accepted
[owner code result](GROUND_CODE_OWNER_RESULT_20261031.md), existing ground
investigation/design and shared reference/engine documentation.
The accepted Run name reference and CALL0x3081DB0 remain name/argument evidence,
not a movement or registry identity. Event name-return leaves remain non-motion.
Do not repeat names, reference windows, next instructions or follow that callee.

**VERIFIED-STATIC:** `source/src/msm2/sdk_bridge.cpp::initialize` lines74–94
collects four independent SwingCore-derived AOB candidates for WR Init/Entry/
Update/Deinit. It does NOT resolve state names through a registry/type/vtable.
`source/src/msm2/wallrun_hooks.cpp::start` lines129–158 requires a unique candidate,
original prefix and nativeSpan/function boundary, exact build, complete hooks,
current patch ownership; semantic acceptance also has existing measured lifecycle
proof. These checks authenticate individually discovered methods; they cannot
synthesize Run method identities from an unrelated name initializer.
Vendor Actor/Component declarations expose generic component lookup, not a
state-name registration schema or Run lifecycle vtable slot contract.
WSU report establishes ComponentRegistry/HeroCache labels and a state query in
its reference controller, but supplies no authenticated Run registration record,
name-hash algorithm, vtable relationship or offsets. DLL-relative locations are
not game offsets. No structurally applicable name->state->methods resolver was
found to transfer. Reusing WR AOBs/slots for Run would invent the missing edge.

**STOPPING_RULE_OUTCOME = GROUND_NAME_REGISTRATION_ROUTE_STOPPED_NO_DEFENSIBLE_MECHANISM.**
Unresolved edge is Run name initializer/unknown helper -> actual registered type/
state instance/method table. No new exploratory Run scanner or serial CALL/byte
acquisition implemented. This is not proof that ground Sprint is impossible or
world-up-bound. Ground support/gravity remains UNKNOWN, production unchanged.

## Concrete alternate investigation executed/prepared

Selected native mover/request/gravity interface lead from Scripted Flight,
not another architecture comparison. Local input inventory found no reference
DLL; actual reference acquisition was run against /workspace/attachments and
/workspace/WallSprint with the new bounded helper:0 named packages,0 DLLs,
`REFERENCE_UNAVAILABLE_OR_DIFFERENT_SHA`. No disassembly of absent inputs claimed.

Grounded alternate anchors come from
`analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md` Finding3 and Wisdoms:
reference `ScriptedFlight.dll` SHA256
`7330c68fa87da58a50a6ee4c477d17339e5deb28a82cf77f7b16c3c2092b7662`,
with exact recorded verification labels
`UpdateMotion.call->RequestSimpleMotion` and
`GravityOrientationOwner.call->SetGravityVector`.
The reference strings assert relationships; they do NOT prove native game API,
callable ABI, gravity ownership or grounded contact behavior. WSU report's unknown
state writes are not a substitute or permission to copy offsets.

New `reference_motion_evidence.py` performs ONE private owner-local operation:
locate this exact SHA in named Scripted Flight packages/DLL, then obtain decoded
references to the TWO dependency-verification labels, containing PE function
identity, and first available local memory/direct-call evidence. This enables
review of the reference's dependency verifier/resolution path, rather than
mislabeling a generic game-name initializer as a movement method. A nearby call
may simply be a logger: no native method is certified. The result's domain is
explicitly **reference DLL RVAs, NOT game RVAs**, native_binding_proven=false.

Reuses existing bounded decoder/PE/evidence engine with explicit internal
needle/ranker arguments. Default ground helper CLI/selection/hash remain unchanged;
no new CLI build bypass, Run family scan or recursive call traversal. No reference
code copied into production. Entire binaries never extracted/copied/executed.
No current EXE upload or game data read is needed for this alternate operation.

### Acquisition bounds and limits

Windows wrapper inspects user Downloads and game Mods Library read-only. Only
exact ScriptedFlight.dll or named Scripted+Flight ZIP/script/modular packages
are opened; nested ZIP/script/modular may contain that DLL. No all-mod DLL scan.
Other names/renamed archives/RAR/7z are outside this operation; no-match is explicit,
not permission for another broad scan or automatic download request.
At most100000 filesystem entries,64 named packages/64 archives,16384 members per
archive,256MiB container,512MiB aggregate data,3 archive nesting levels, no symlink
root/traversal or filesystem extraction. Duplicate SHA-identical copies deduplicate;
unknown DLL SHA is not parsed. Malformed/encrypted/oversize/caps refuse. Output
exclusive, bounded128KiB, no binary/full-function dump (existing tiny-body omission).
Inherited code caps:8 targets/64 relevant labels,128 metadata refs,32 code refs/
16 functions,512 exported code bytes; bounded straight-line decoding only. References
with unproven boundary stay candidates, metadata words never become registries.

Output `REFERENCE_UNAVAILABLE_OR_DIFFERENT_SHA` is useful acquisition status, not
reference semantics FAIL or gameplay proof. Return that JSON as well. Exact
reference missing means reference verifier analysis unavailable; native mover/
contact/request ABI still requires independent current-build evidence later.
This task authorizes no fourth Run exploration or runtime changes.

## ONE new owner command — PowerShell

Fetches only source objects/refs, stages3 owned helper dependencies from one commit,
checks existing WSL analysis-python/Capstone5.0.6, no package/runtime update.
Child process bypasses script policy only; no global policy changes.

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
$body = @(& git -C $src show ($commit + ':source/tools/windows/Inspect-Reference-Motion.ps1'))
if ($LASTEXITCODE -ne 0) { throw 'Reference evidence entry unavailable' }
$entry = Join-Path ([IO.Path]::GetTempPath()) ('WS-reference-entry-' + [guid]::NewGuid().ToString('N') + '.ps1')
try {
    [IO.File]::WriteAllText($entry, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $entry -SourceRoot $src -GameRoot $game
    if ($LASTEXITCODE -ne 0) { throw 'Reference evidence stopped/unavailable; return JSON if generated, otherwise the error' }
} finally {
    if (Test-Path -LiteralPath $entry) { Remove-Item -LiteralPath $entry -Force }
}
```

Return only `MSM2-WS\logs\MSM2-WS-reference-motion-refs-YYYYMMDD-HHMMSS.json`
(optional collision suffix). Private fragments stay out of Git/release artifacts.
No Update-WS / Install mods / game launch / EXE upload. If the reference isn't
present or is a different version, return the JSON; no download/copy/install is
requested. This is alternate evidence collection, not gameplay testing.
