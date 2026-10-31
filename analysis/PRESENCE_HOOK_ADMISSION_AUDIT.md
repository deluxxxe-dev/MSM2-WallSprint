> Superseded current blocker: owner audit161445 supplied original unwind/prefix input.
> Actual MinHook/observer OWNED execution now exists; see PRESENCE_OWNED_NATIVE_FIXTURE.md.
> Native Windows is pending, not missing original metadata. Earlier audit below is historical.

# Presence diagnostic entry-hook admission audit — BLOCKED / not implemented

2026-10-31 JST; baseline `fc55924f01d730bddf0efb3346166befc6cda895`;
branch `codex/v01c20-edi-value-provenance`.
**RUNTIME_PRESENCE_OBSERVER_NOT_IMPLEMENTED. Admission audit partial; no new
version/package/arming option or tested in-game diagnostic is claimed.**

## Accepted inputs / first concrete missing guarantee

[Owner contract155119](MOTION_GRAVITY_OWNER_CONTRACT_20261031.md) accepted; no more
contract/MAY/identity/relocation acquisition. Request uses caller-stack argument
candidates, two12byte helper calls, final request object call; setter field stores
are not owner/lifetime permission. New observation would only count entries and
record opaque incoming bits/thread/time, NEVER dereference these arguments.

Task requires: “First perform hook-site/whole-instruction/unwind/trampoline/
continuation audit ... Identity hash/pattern alone is insufficient.”
Accepted function hashes/CFGs do not include actual native UNWIND_INFO or tests of
new-site relocated prefixes/native exception/continuation behavior. No current
EXE/raw code is available here. Existing WR hook tests do not automatically prove
these two new entry sites. **First unresolved guarantee: new entry-thunk/MinHook
relocated-prefix/native-unwind transparency for RequestSimpleMotion, followed by
separate optional GravityOrientationOwner.** No code installed pending that proof.

This is not a claim that MinHook or ground Sprint is impossible. The task's
explicit blocked-audit path is used; no gate is bypassed or diagnostic simulated.
New small offline hook-site audit collects the exact missing unwind declarations
and local prefix effects, not a generic contract/name/adjacent-byte scan.

## Concrete audit performed

| Item | Evidence / result |
| --- | --- |
| Exact candidate identities |Accepted Request18488272..18488482/SHA2d879249...; owner20246272..20246459/SHAbc9e1a75... . Both PE entries, unlike setter. Audit pins complete hashes; no target discovery |
| Minimum whole instruction prefix |Request MOV R11,RSP (3bytes) + original MOV [R11+8],RBX (4) =7; owner original MOV [RSP+8],RBX =5. Supported shapes do not change RSP, flags or nonvolatile register values; they reproduce original home-slot store. Whole-instruction lengths verified with real vendored HDE64 on authored fixture under Wine |
| Relocation shape |MinHook trampoline.c copies until>=5, then adds jump back; these MOV shapes have no RIP-relative operand/branch/call to relocate. Expected original continuation Request+7 / owner+5. Actual new-site MinHook relocation/execution not tested |
| Unwind |Existing wallrun_hooks.cpp::nativeSpan checks PE entry/range, not new-site unwind-code agreement. MinHook has no RtlAddFunctionTable registration in its trampoline source. This ALONE does not prove failure: an RSP-preserving prefix can be leaf-unwind compatible. Must validate actual native prologue/unwind declarations and new thunk/trampoline unwind paths rather than inventing metadata. Current declarations absent |
| Arguments/flags/XSTATE |Old hook_thunks.S fast path saves volatile GPRs/flags, untouched vector state; enabled global writer/lifecycle dispatch also enters C++. Cannot reuse macro unchanged for generic observer: its kind/dispatch would route into WR-specific callbacks. New capture-only thunk must separately prove full forwarding/return/flags/nonvolatile/XSTATE; not implemented |
| Return/control continuation |Intended tail jump, no synthetic return/VCH/DR/ticket. Current MinHook source maps suspended current PCs, not every saved context. Old WR module pin and retained trampolines (no Remove/Uninitialize) provide relevant code-residency policy, not an automatic new-observer cleanup proof |
| Disable/unload |wallrun_hooks.cpp::stop closes recording, restores only owned patches, retains module/trampolines. Future observer needs independent static storage/generation/window closure so delayed callbacks cannot publish as a new phase. Tests missing, no observer installed |
| Ground admission |Old drain rejects events without verified WR/snapshot and interprets kinds as WR lifecycle. Must not reuse it to arm ground presence. New truthful admission/window/zero-hit summaries would need a separate non-WR lease; none implemented |

### C52 applicability — no invented blanket blocker

Read analysis/V01C52_GATE.md and existing sources. Its FAIL addresses production
VCH/saved-CONTEXT witness integration, finite obligations/final context/PC storage
lifetime/mitigations. This proposal uses no VCH, hardware watch, CONTEXT redirection
or witness integration. C52 therefore is NOT automatically a ban on every inline
entry observer, nor permission for one. No C52 predicate changes, shadow-stack
workaround, broadening source ownership or reuse of partial setter hook occurs.
Concrete new-site unwind/continuation/cleanup proof is still independently required.

## Focused audit tooling (NOT runtime implementation)

`audit_presence_hook_sites.py` accepts ONLY pinned WS EXE SHA and exact accepted
TWO full hashes/ranges. It decodes minimum>=5byte prefix with pinned Capstone and
recognizes only the supported MOV/no-RSP-adjust shapes; reports instruction
lengths/effects/hash, not bytes/body. Parses exact .pdata-selected UNWIND_INFO
version1/prologue/frame/code offsets, SAVE/ALLOC/frame operations and optional
handler/chained metadata/hash with range/slot validation. It does not follow a
handler/callee, snap an entry or hook the no-.pdata setter.

Bounded file512MiB,2 known functions, each prefix<=32bytes, unwind<=64 slots plus
small trailer, output<=32KiB; reused PE runtime-table cap600000/8MiB; malformed/
unsupported version/op/slot/prologue/hash/build/entry refuses. Output is always
`HOOK_ADMISSION_AUDIT_ONLY_BLOCKED`, hook_admitted=false/observer_implemented=false,
with actual_MinHook_trampoline=NOT_EXECUTED. Matching HDE prefix fixtures closes
only decoder-length subcheck; not actual forwarding or native Windows EH proof.
No live reads, arguments/pointees, code export, config changes or game launch.

`presence_hook_hde_fixture.cpp` uses only authored minimal MOV entry shapes and
the existing vendored HDE decoder. Cross-build + Wine test PASS request7/owner5.
No game/reference DLL executed. Wine emitted missing winedevice warnings; fixture
returned0 with explicit PASS. It does not test native Windows unwind or hooking.

## What remains BLOCKED / not falsely delivered

No phase ring/2second windows/16record budget/zerohit summary exists for this new
observer. No startup observer admission/version/path log. Buffer races, original
ABI forwarding, disable/unload and zero-effect fixtures for an actual observer
are unexecuted because it is not implemented/admitted. Full runtime build/package/
release tests are not required for offline-only audit changes, and no release is
created. Current r7 remains unchanged; do NOT run unchanged game test or tweak INI.

The minimal next evidence is actual native unwind/prefix metadata from these TWO
accepted entries in one read-only operation. Then prove new-site relocation and
unwind/forwarding on owned native fixtures before any runtime release. No generic
MAY report, extra signatures or new publisher/DR investigation is requested.

## ONE offline owner command

Existing WSL Capstone5.0.6; source fetch/temp staging only; child policy Bypass.
This produces an admission audit JSON, NOT ground/WR presence measurements.

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
$body = @(& git -C $src show ($commit + ':source/tools/windows/Audit-Presence-Hooks.ps1'))
if ($LASTEXITCODE -ne 0) { throw 'Hook audit entry unavailable' }
$entry = Join-Path ([IO.Path]::GetTempPath()) ('WS-presence-audit-' + [guid]::NewGuid().ToString('N') + '.ps1')
try {
    [IO.File]::WriteAllText($entry, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $entry -SourceRoot $src -GameRoot $game
    if ($LASTEXITCODE -ne 0) { throw 'Offline hook audit refused; return JSON if generated, otherwise the error' }
} finally {
    if (Test-Path -LiteralPath $entry) { Remove-Item -LiteralPath $entry -Force }
}
```

Return `MSM2-WS\logs\MSM2-WS-presence-hook-audit-YYYYMMDD-HHMMSS.json`
(optional suffix/refusal). No Update-WS/Install/game/ON-OFF/arming/restoration
commands: no new observer/version/config exists. This task has NOT delivered a
tested runtime diagnostic. Runtime admission remains fail closed.
