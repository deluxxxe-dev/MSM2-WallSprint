# Codex result — c67 first terminal post-c65 boundary diagnostic

2026-10-28 JST. Branch **codex/v01c20-edi-value-provenance**.
Primary result: **POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY**, tool `v0.1c67` / schema `1`.

This c67 implementation was completed by ChatGPT while Codex usage was unavailable. Repository changes and owner-run automated validation are canonical. The owner/private-image c67 classification artifact has not yet been produced or accepted.

## Changes

- Added `source/tools/slot_publication_post_c65_boundary.py`.
- Added and then expanded `source/tools/test_slot_publication_post_c65_boundary.py` to cover the complete required c67 focused contract.
- Registered c67 in `source/tools/test_hooks_windows.sh` without removing historical tests.
- Added `analysis/V01C67_TOOL_READY.md`.
- Updated project state for c67 TOOL READY.

Historical c56-c66 tools/evidence remain read-only. No other MOD repository was changed.

## C67 proof scope

C67 reconstructs/authenticates the accepted c56→c66 chain and observes only the first actual `uncertified_post_c65_runtime_boundary` terminal event for each exact c66 refused root. The accepted domain is 458 roots.

It preserves exact root/state/path/source/current-c66-prefix and c65 provenance, classifies supported row-external successors with the unchanged c57 relation vocabulary, and fails closed on stale/missing/duplicate/out-of-order/mismatching evidence.

C67 is diagnostic only. It does not traverse classified targets, admit edges, propagate Facts, create candidates, infer nearest entries, issue continuation certificates, read slot qwords, chase pointers, run native/game probes, or enable production/gameplay writes.

C52 remains binding:

```text
PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL
```

## Owner automated validation

Final expanded focused command:

```bash
/workspace/toolchains/analysis-python/bin/python3 source/tools/test_slot_publication_post_c65_boundary.py
```

Actual: **13 c67 PASS groups, exit 0**.

Coverage includes:

- exact first post-c65 terminal source/path/provenance binding;
- direct/conditional/CALL-fallthrough edge vocabulary;
- multi-successor architectural classification without traversal;
- interior and metadata-free RX preservation;
- exact root/state/path/source/c66-prefix/c65 provenance binding;
- stale phase/reason/origin/provenance rejection;
- stored earlier-path vs actual post-c65 arrival separation;
- converted c65 authorization provenance preservation;
- missing/duplicate/out-of-order record failure;
- physical and semantic c66 evidence rejection;
- invalidation-only terminal blocking;
- unchanged c66 candidate bytes/counts;
- zero edge/Fact/candidate admission and all safety gates false.

Before the final focused-test expansion, the owner ran the complete slot-publication regression set through c67. All 17 scanner commands passed, including the new c67 focused suite. Integration validation also passed:

```text
bash source/tools/build_windows.sh          PASS / exit 0
bash source/tools/test.sh                   PASS / exit 0
bash source/tools/smoke_windows.sh          PASS / exit 0
bash source/tools/test_hooks_windows.sh     PASS / exit 0
python3 source/tools/package.py             PASS / exit 0
```

After the focused test was expanded, the owner reran:

```text
bash source/tools/test_hooks_windows.sh     PASS / exit 0
python3 source/tools/package.py             PASS / exit 0
```

The hooks suite prints repeated Wine `winedevice.exe: c0000135` diagnostics in this environment, but completed successfully; the subsequent PowerShell `$LASTEXITCODE` guard did not throw. Package validation produced the existing disabled validation artifact under `source/output/MSM2-WS-v0.1c38/`; runtime/package release version remains unchanged.

## Safety / unrun domains

No c67 game test, Overstrike use, Update-WS, native probe, slot qword read, pointer chase, producer traversal or gameplay/production write was run or authorized.

Zero candidates remains incomplete evidence and never proves writer absence.

## Owner next action

Run only this OFFLINE command. Reuse the existing c56 checkpoint evidence and accepted c57-c66 JSONs read-only. Return only:

```text
logs/MSM2-WS-v0.1c67-post-c65-runtime-boundary.json
```

```powershell
$gameWsl = "/mnt/c/SteamLibrary/steamapps/common/Marvel's Spider-Man 2"
$wsWsl = "$gameWsl/Mods Library/Mods-Developing/MSM2-WS"
$wsRoot = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\Mods Library\Mods-Developing\MSM2-WS"

if ((git -C "$wsRoot\_source" branch --show-current) -ne "codex/v01c20-edi-value-provenance") { throw "Unexpected branch; stop here" }
git -C "$wsRoot\_source" pull --ff-only origin codex/v01c20-edi-value-provenance
if ($LASTEXITCODE -ne 0) { throw "WS source update failed; stop here" }

wsl -d Ubuntu-24.04 --exec /workspace/toolchains/analysis-python/bin/python3 `
"$wsWsl/_source/source/tools/slot_publication_post_c65_boundary.py" `
"$gameWsl/Spider-Man2.exe" `
--c56-cache "$wsWsl/logs/MSM2-WS-v0.1c56-slot-publication-sharded.shards" `
--c57-json "$wsWsl/logs/MSM2-WS-v0.1c57-runtime-boundary.json" `
--c58-json "$wsWsl/logs/MSM2-WS-v0.1c58-slot-publication-stitched.json" `
--c59-json "$wsWsl/logs/MSM2-WS-v0.1c59-later-runtime-boundary.json" `
--c60-json "$wsWsl/logs/MSM2-WS-v0.1c60-slot-publication-later-stitched.json" `
--c61-json "$wsWsl/logs/MSM2-WS-v0.1c61-post-c59-runtime-boundary.json" `
--c62-json "$wsWsl/logs/MSM2-WS-v0.1c62-post-c59-stitched.json" `
--c63-json "$wsWsl/logs/MSM2-WS-v0.1c63-post-c61-runtime-boundary.json" `
--c64-json "$wsWsl/logs/MSM2-WS-v0.1c64-post-c61-stitched.json" `
--c65-json "$wsWsl/logs/MSM2-WS-v0.1c65-post-c63-runtime-boundary.json" `
--c66-json "$wsWsl/logs/MSM2-WS-v0.1c66-post-c63-stitched.json" `
--output "$wsWsl/logs/MSM2-WS-v0.1c67-post-c65-runtime-boundary.json"

if ($LASTEXITCODE -ne 0) { throw "c67 offline classifier refused; preserve existing evidence" }
```

Do not run game/Overstrike/native follow-up. Analyze the resulting c67 JSON before deciding c68.
