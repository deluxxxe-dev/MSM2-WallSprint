# Codex result — direct Scripted Flight resolver/motion/gravity trace

2026-10-31 JST; branch `codex/v01c20-edi-value-provenance`.
Baseline `5685d9819ce5f8daebc73fc31a0359c2e5f3c097`, fetched/fast-forwarded from
d0f03cc. No main merge. Five pre-existing untracked old archives/bundle preserved.

## STATUS / findings

**REFERENCE_TRACE_COMPLETED; CURRENT_GAME_RELATION_VERIFICATION_PENDING.**
Actually opened attached nested ZIP and authenticated DLL SHA7330c68f...b7662.
No reference locator/name scan repeated; ground-name route remains STOPPED.
Direct analysis recovered individual resolver uniqueness/three-anchor shift
fallback, verifier's two signed-rel32 CALL comparisons/semantic checks, stored
result slots, actual native-target/trampoline uses and vector/mode/scalar capture/
restore.16920 is diagnostic formatter, not request or setter. Motion hook forwards
original second argument and supplies cached third/facing argument; gravity wrapper
performs real indirect target call. Actual facts/limits/locations are in analysis.
No claim of an independently authenticated current native ABI or grounded contact.

One new tightly grounded read-only current-EXE verifier checks FOUR recovered
canonical hints, TWO CALL relations and SEVEN marker hashes, exact WS EXE SHA.
No scanning/shift fallback/raw code export. Explicit zero-shift subset of reference
method, not a copy of its resolver or a production certificate. Native callable
ABI/lifetime/contact/production_permission all remain false even on match.

## CHANGED / outputs

* `analysis/SCRIPTED_FLIGHT_RESOLVER_MOTION_GRAVITY_REVIEW.md`: direct actual trace,
  resolver/argument/compare/store/use/save/restore evidence and exact blockers;
  one owner bootstrap. `analysis/SCRIPTED_FLIGHT_FUNCTION_IDENTITIES.json`:
 9 function ranges/hashes only, no bodies.
* `source/tools/verify_motion_gravity.py`, `test_verify_motion_gravity.py`:
  bounded exact-EXE metadata/hash/CALL checks,9 positive/negative fixture tests.
* Windows `Verify-Motion-Gravity.ps1` / `test_verify_motion_gravity.ps1`:
  existing fresh dependency staging/WSL decoder checks, exclusive timestamp JSON,
 3 mock groups, child powershell Bypass owner command.
* PROJECT_STATE prepended accepted attachment/direct-trace status; this result.
  CODEX_NEXT unchanged. All production/runtime/version/config/packages/C52/shared
  loader/environment/SF untouched. No live reads/hooks/calls/writes, Position/
  gravity/animation/state changes. Runtime remains r7/package0.1.63, no new package.

Output `MSM2-WS-motion-gravity-verification-YYYYMMDD-HHMMSS.json` under WS logs,
optional suffix. It contains metadata/marker booleans/hashes, not code fragments.
DLL/disassembly/raw function bodies stayed private /tmp, not Git or release inputs.

## Executed static analysis / expected / actual

1. Python zipfile in-memory nested inspection; `sha256sum` original attached ZIP.
   Expected accepted identities; actual archive
   c1f43885f6138143ad54cf0d6d3be86b1e51b0abc27cc51155bb9de6e93794f7;
   DLL162816bytes SHA7330c68fa87da58a50a6ee4c477d17339e5deb28a82cf77f7b16c3c2092b7662.
   PASS. No DLL executed, extracted only to task-private /tmp for static tools.
2. `objdump -h /tmp/ws-flight-review/ScriptedFlight.dll`;
   `objdump -d -M intel --start-address=0x1800137b0 --stop-address=0x1800155aa /tmp/ws-flight-review/ScriptedFlight.dll`;
   private xref index `objdump -d -M intel /tmp/ws-flight-review/ScriptedFlight.dll`;
   `objdump -p /tmp/ws-flight-review/ScriptedFlight.dll` imports.
   Expected actual verifier/call/data flow; actual reviewed initializer and
   relevant startup/resolver/formatter/hooks/restore paths, reference text~110KiB
   indexed privately to locate exact target-slot uses. Not an all-game scan.
3. Pinned Capstone5.0.6 PE-runtime-range inspection of D430,12FD0,137B0,16920,
   3C60,3410,6C70,A380,8770 using existing PE helper; private
   `/workspace/toolchains/analysis-python/bin/python3 /tmp/ws-flight-review/check_findings.py`.
   Expected exact range/hash and instruction checks; actual PASS9 identities,
   4 resolver calls,2 formatter calls, real signed-CALL decoders, motion/gravity
   use and restore edges. Derived range/hash-only JSON committed. Two disassemblers
   corroborate instructions; no arbitrary mid-instruction decode promoted to fact.
   Initial scratch utility was named inspect.py and shadowed Python inspect;
   renamed flight_view.py before successful Capstone inspection. No source change
   or failed preliminary decode used as evidence.

Reproducible method: authenticate DLL, parse .pdata, decode each recorded exact
runtime range, use RIP targets to map label/context-slot edges and direct calls
between recorded functions. Native current binding is not certified by any of
these reference checks. Marker hashes in new verifier are independently derived
from this initializer's minimal comparison constants; no binary fragments stored.

## Focused tests / exact commands / expected / actual

* `/workspace/toolchains/analysis-python/bin/python3 source/tools/test_verify_motion_gravity.py`
  Expected positive recovered-relation fixture, hash/marker/opcode/target mismatch,
  partial boundary, call outside owner runtime range, malformed PE/runtime/range/
  caps, writable code/decoder refusal, Unicode/space and exclusive CLI output,
  no shift override. Actual **9 PASS**, synthetic marker hashes use authored NOP
  fixtures, not copied native bytes; no real-game result inferred.
* `python3 source/tools/test_discover_ground_support.py`
  Expected reused PE/hash/range/string/output helper stable; actual **18 PASS**.
* `XDG_CACHE_HOME=/tmp/ws-ground-pwsh/cache XDG_CONFIG_HOME=/tmp/ws-ground-pwsh/config XDG_DATA_HOME=/tmp/ws-ground-pwsh/data /tmp/ws-proxy-pwsh/pwsh -NoProfile -File source/tools/windows/test_verify_motion_gravity.ps1`
  Expected fresh2 dependencies, Japanese/space paths, collision output/dirty file/
  EXE preserved, unavailable WSL/decoder refusal. Actual **3 PASS mock groups**,
  Git/WSL mocked, not real Windows acceptance.
* `git diff --check`: expected clean; actual PASS.
* `python3 /tmp/ws-flight-scope.py`: expected only8 intended offline helper/test/
  analysis/docs paths, task/runtime/packages preserved, links/9 identity records
  valid, Bypass command present; actual PASS; extracts owner command for parser.
* `XDG_CACHE_HOME=/tmp/ws-ground-pwsh/cache XDG_CONFIG_HOME=/tmp/ws-ground-pwsh/config XDG_DATA_HOME=/tmp/ws-ground-pwsh/data /tmp/ws-proxy-pwsh/pwsh -NoProfile -Command '$t=$null; $e=$null; [void][System.Management.Automation.Language.Parser]::ParseFile("/tmp/ws-flight-owner.ps1",[ref]$t,[ref]$e); if($e.Count){throw ($e|Out-String)}; "PASS verifier bootstrap syntax"'`
  Expected owner block valid; actual PASS parser, not actual Windows acquisition.

**Focused tests PASS30 / SKIP0 / UNAVAILABLE1 / BLOCKED1.** Static/scope/parser/
publication checks counted separately, not inflated into fixture PASS counts.
UNAVAILABLE1 = current original EXE verification, no game EXE in cloud.
BLOCKED1 = native callable ABI/lifetime/grounded-contact authorization; not proved
by static reference trace or even an eventual canonical-relation match.
Not run: runtime build/native/Wine/smoke/hooks/package/game tests, task is static/
offline only and production untouched. Not equivalent to gameplay improvement.

## Shared knowledge / publication / NEED

Wisdoms direct static resolver/use/restore facts and certificate limits pushed
`96ff78fb5691ac89610770ec466b6d802c1a263c`; ls-remote agrees. No raw reference code
or personal paths committed. WS implementation/publication readback follows.

ONE runnable owner command in analysis/final: Verify-Motion-Gravity via child
powershell.exe -NoProfile -ExecutionPolicy Bypass -File, no global policy edit.
Return new motion-gravity-verification JSON including mismatch/refusal. No new
EXE slice chain, reference scan/download, EXE upload, Update-WS, Install mods or
game launch. Original EXE read-only; source worktree/config/KIT unchanged, Git
fetch updates refs/objects and own temporary/output files only. Matching checks
still do not authorize gameplay integration; partial/no runtime entry explicit.

### Verified publication

Implementation `8e6ecc40928a406945ad22fbc2369a3028fc8711` pushed on task branch.
`python3 /tmp/ws-flight-publication.py`: expected independent fetched FETCH_HEAD,
local HEAD and GitHub ls-remote agreement plus exact bytes of8 changed files
against5685d98; actual PASS at8e6ecc4. Record-only commit follows and same readback
repeated. Unchanged30 focused tests need no rerun for this documentation-only
publication record; publication checks do not inflate fixture counts. Final
branch/record SHA in owner handoff. No runtime release produced.
