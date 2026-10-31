# Codex result — interpret gate / complete fixed77-byte fragment

2026-10-31 JST. Branch codex/v01c20-edi-value-provenance; baseline c224f17.
STATUS: static interpretation and minimal offline completion mode ready; actual
remaining36-byte acquisition unavailable here. No descending fix claimed.

## CHANGED / findings

See analysis/WALLRUN_FIXED_FRAGMENT_REVIEW.md. Rechecked prior private422-byte
prefix SHA/size with pinned Capstone. RSI=original RCX owner on known routes;
CL=one masked bit of a pointer-derived dword; fragment EDX=zero-extended byte,
AL=comparison predicate. Frame-relative byte maps to entryRSP+0x10. JE branches
back to XOR AL,1 at0x14246d4, NOT a verified RET. These are scalar/state decision
gates in this41-byte span, not evidence of a motion/input API. Unknown field names,
untyped calls and eventual AL/EDX use remain unknown. Supplied explicit unwind
chain establishes split relation, superseding earlier plausibility-only status.

Existing helper gets --complete-fragment; existing Windows wrapper selects it.
Exactly77 bytes0x14248f9..0x1424946 in one output, first41 SHA+runtime+chain identity
mandatory. Decode records but never follows branches, no arbitrary range or image
scan. RET/unsupported control/decode/raw/metadata mismatch refuses. Legacy
first-control behavior retained. New fragment77 filenames preserve old41 output.
Files: existing Python helper/test and PowerShell entrypoint/mock test;
new analysis, PROJECT_STATE and this result. Runtime r6 and packaged bytes unchanged.

## Exact tests / expected / actual

`/workspace/toolchains/analysis-python/bin/python3 source/tools/test_wallrun_continuation.py`
Expected prior8 cases intact plus fixed77 completion across JE, exact metadata/
chain/first41 hash guards, RET/unsupported instruction and tail decode/raw
refusals. Actual12 PASS /0 SKIP /0 UNAVAILABLE, exit0.
New synthetic fixture initially used too-small SizeOfImage for the accepted parent
unwind RVA; guard correctly refused it. Corrected ONLY synthetic image extent,
not production guard/assertions; rerun all12 passed.

```bash
XDG_CACHE_HOME=/tmp/ws-lock-xdg-cache XDG_CONFIG_HOME=/tmp/ws-lock-xdg-config XDG_DATA_HOME=/tmp/ws-lock-xdg-data DOTNET_CLI_HOME=/tmp/ws-lock-dotnet /tmp/ws-proxy-pwsh/pwsh -NoProfile -File source/tools/windows/test_capture_wallrun_continuation.ps1
```
Expected same Unicode/space layout + preservation/failure contract, now explicit
--complete-fragment and fragment77 output. Actual2 PASS groups /0 SKIP /0 UNAVAILABLE,
MOCK WSL/Git transport only. Real Windows owner transport not inferred.

Prefix check command: pinned analysis Python reads ONLY private Update.bin,
asserts422 bytes/SHA above, then Capstone disasm at0x1424530; selects definition/
branch sites. PASS, no raw body committed. Missing target filenames/EXE search
found none; owner41 facts accepted from compact supplied analysis, not fabricated.

UNAVAILABLE: actual complete77/remaining36 EXE capture and owner Windows execution.
BLOCKED0. Native/build/portable/smoke/hooks/release NOT RUN because only offline
helper/docs changed and task forbids runtime rebuild/game test. No new package.
Additional syntax/unchanged-byte/publication checks recorded below.

## NEED — ONE exact fresh offline action, NOT old41/game test

Existing default WSL analysis Python5.0.6 prerequisite remains. Fetch/stage current
helpers without pulling/resetting source/local edits or installed KIT/runtime.
Return only the newly printed small fragment77 JSON (REFUSED report also useful).
The same77 bytes include41-byte consistency authentication and all remaining36;
no serial single-instruction request, target-follow or whole EXE upload.

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

Output: `Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-wallrun-fragment77-YYYYMMDD-HHMMSS.json`
(or -01..-99). No Update-WS/build/package/game launch. Do not resend old41 JSON.

## Final checks / shared synchronization

Known-route refinement: first prefix incoming JNE carries masked CL=1, while
three later incoming edges require CL=0; captured JE routes the latter to AL
inversion, leaving CL=1's36-byte continuation unresolved. Not runtime outcome.

Exact private-prefix verification used (1 PASS group):
```bash
/workspace/toolchains/analysis-python/bin/python3 - <<'PY'
from pathlib import Path
import hashlib,capstone
b=Path('/tmp/ws-lateral-update-review/Update.bin').read_bytes()
assert len(b)==422 and hashlib.sha256(b).hexdigest()=='e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f'
m=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64)
d={i.address:(i.mnemonic,i.op_str) for i in m.disasm(b,0x1424530)}
assert len(d)==89 and d[0x142455a]==('mov','rsi, rcx') and d[0x14246d4]==('xor','al, 1')
assert d[0x1424660]==('shr','ecx, 8') and d[0x1424663]==('and','cl, 1')
print('PASS: authenticated prefix definitions; not runtime semantics')
PY
```
Private input is not distributed. Future sessions without it must use compact
analysis rather than invent bytes. No full body/executable requested/committed.

`python3 -m py_compile source/tools/capture_wallrun_continuation.py source/tools/test_wallrun_continuation.py` PASS exit0.
`git diff --check` PASS exit0.
`git diff --exit-code c224f17 -- source/include source/src source/mod environment BUILD_MANIFEST.json 'MSM2-WS-v0.1c38r6*'` PASS exit0.
Wisdoms updated/pushed `1f298026f2caeb464853306763e5805704df0bd6`; remote main
SHA matches. Confirmed split association/static gating is not motion semantics.
