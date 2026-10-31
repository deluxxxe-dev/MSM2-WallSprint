# Codex result — accepted77 / one shared-successor consumer

2026-10-31 JST; branch codex/v01c20-edi-value-provenance; baseline71acd91.
STATUS: gate relation/provenance verified locally; missing shared-consumer offline
mode ready. Actual successor acquisition UNAVAILABLE, no down fix/return claim.

## CHANGED / decision

See analysis/WALLRUN_SHARED_SUCCESSOR_REVIEW.md. Owner complete77 accepted, never
requested again. Known gate paths give AL=!P||(C&&R); authenticated prefix defines
R9B via owner byte and comparison R8B. All8 combinations checked. No global field/
dominance/typed function return claim. EDX from gate is preserved by77, but alternate
prefix route clears only DL, so shared state is path-dependent. Complete77 is
scalar/state gating, not motion vector/API; stop looking for movement inside it.

Existing helper adds only fixed --shared-successor0x14246d6/max128, authenticates
old prefix SHA/runtime tuple, actual successor runtime/unwind/file bounds; no
missing-entry guess. Captures a coherent local block through first control or cap,
reports register overlaps/edge without following or declaring value flow. Existing
legacy/fixed77 retained. Windows entrypoint selects new mode and new successor
JSON name; existing outputs and dirty local source remain intact. No runtime
version/build/package/native/INI/game/shared tools change.
Files: existing helper/test and PS entrypoint/mock test; new analysis; state/result.
No authenticated successor/EXE found among current supplied/local captures; prefix
ends at target and accepted77 does not cover it. ONE acquisition genuinely needed.
After this single local task stop the scalar route. Supported future alternative:
controlled A/B source-override ON/OFF frame-feedback comparison with existing r6
observations, under separate authorization; no such game test requested here.

## Exact validation / expected / actual

`/workspace/toolchains/analysis-python/bin/python3 source/tools/test_wallrun_continuation.py`
Expected prior12 cases plus successor consumers/control, absent prefix/covering
metadata, wrong prefix SHA/chain/unwind/raw extent, actual metadata cap,128 cap,
mode conflict and actual CLI output/EXE preservation. Actual17 PASS /0 SKIP /
0 UNAVAILABLE, exit0. Synthetic acceptance patches SHA ONLY in tests, not CLI.

```bash
XDG_CACHE_HOME=/tmp/ws-lock-xdg-cache XDG_CONFIG_HOME=/tmp/ws-lock-xdg-config XDG_DATA_HOME=/tmp/ws-lock-xdg-data DOTNET_CLI_HOME=/tmp/ws-lock-dotnet /tmp/ws-proxy-pwsh/pwsh -NoProfile -File source/tools/windows/test_capture_wallrun_continuation.ps1
```
Expected Unicode/space-derived paths/fresh Git helper/output collision preservation
and WSL failure refusal, now --shared-successor. Actual2 PASS groups /0 SKIP /
0 UNAVAILABLE, mock transport only. Actual Windows/WSL is NOT inferred PASS.

Private prefix422/89/SHA + R9B/DL/AL definitions and8 truth cases:1 PASS group.
Exact reproducible verification command follows. Raw input stays private.
Actual successor capture and owner Windows transport: UNAVAILABLE (2 evidence
groups), not PASS. BLOCKED0; SKIP0 focused. No test failure in this task.
Native/portable/smoke/hooks/build/release NOT RUN: offline-only diff, no runtime
bytes changed; task forbids rebuild/game test. No new package or version.

## NEED — ONE exact offline successor action, not complete77 again

The following fetches refs/stages current helpers only; source worktree/local
changes/runtime/KIT and existing reports stay intact. Uses existing default WSL
analysis Python with Capstone5.0.6. No install/repair/full test/build/game launch.
Return only new small successor JSON (or explicit REFUSED report).

```powershell
$game = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2"
$repo = Join-Path $game 'Mods Library\Mods-Developing\MSM2-WS\_source'
$branch = 'codex/v01c20-edi-value-provenance'
git -C $repo fetch origin $branch
if ($LASTEXITCODE -ne 0) { throw 'Fetch failed' }
$body = @(git -C $repo show ("origin/$branch" + ':source/tools/windows/Capture-WallRun-Continuation.ps1'))
if ($LASTEXITCODE -ne 0) { throw 'Offline entrypoint unavailable' }
$temp = Join-Path $env:TEMP ('WS-continuation-' + [guid]::NewGuid().ToString('N') + '.ps1')
[IO.File]::WriteAllText($temp, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
try { & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $temp -SourceRoot $repo -GameRoot $game }
finally { Remove-Item -LiteralPath $temp -Force }
```

Output: `Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-wallrun-successor-YYYYMMDD-HHMMSS.json`
(or -01..-99), NOT prior fragment77 JSON. No whole EXE/raw function upload.

## Static prefix verification / final checks

Exact private-input command (1 PASS group; no raw body output/commit):
```bash
/workspace/toolchains/analysis-python/bin/python3 - <<'PY'
from pathlib import Path
import capstone,hashlib
b=Path('/tmp/ws-lateral-update-review/Update.bin').read_bytes()
assert len(b)==422 and hashlib.sha256(b).hexdigest()=='e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f'
m=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64)
d={i.address:(i.mnemonic,i.op_str) for i in m.disasm(b,0x1424530)}
assert len(d)==89
for site,op in {0x142466e:('setb','r8b'),0x1424672:('cmp','byte ptr [rsi + 0x420], 0'),0x142467b:('test','r8b, r8b'),0x1424680:('mov','r9b, 1'),0x1424685:('xor','r9b, r9b'),0x14246c0:('xor','dl, dl'),0x14246d4:('xor','al, 1')}.items():assert d[site]==op
for P in [False,True]:
 for C in [False,True]:
  for R in [False,True]:
   tail=not P or R
   actual=tail if C else not P
   assert actual==(not P or (C and R))
print('PASS: prefix definitions and8 predicate combinations, not gameplay/return semantics')
PY
```
The private input is not part of Git. Compact accepted owner77 analysis plus this
prefix review establish local facts; no global dominance/branch execution proof.

`python3 -m py_compile source/tools/capture_wallrun_continuation.py source/tools/test_wallrun_continuation.py` expected/actual exit0.
`git diff --check` expected/actual exit0.
`git diff --exit-code 71acd91 -- source/include source/src source/mod environment BUILD_MANIFEST.json 'MSM2-WS-v0.1c38r6*'` expected/actual exit0: native/math/INI/shared tools/artifacts unchanged.
Wisdoms updated/pushed `15bd6349423eeeeef38edded4481a6d0470e1ec1`; remote main
SHA matches. Source worktree/runtime/KIT on owner not updated by publication.
