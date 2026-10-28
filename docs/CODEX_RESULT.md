# Codex result — c66 c65-authenticated post-c63 continuation

2026-10-28 JST. Branch **codex/v01c20-edi-value-provenance**.
Primary result: **POST_C63_RUNTIME_BEGIN_STITCH_SCANNER_READY**, tool `v0.1c66` / schema `1`.

This c66 implementation was completed by ChatGPT while Codex usage was unavailable. Repository changes and owner-run automated validation are canonical; no game/runtime/private-image c66 result exists yet.

## Changes

- Added `source/tools/slot_publication_post_c63_stitched.py`.
- Added `source/tools/test_slot_publication_post_c63_stitched.py`.
- Registered the c66 focused suite in `source/tools/test_hooks_windows.sh`.
- Fixed `source/tools/compat/Windows.h` so the canonical `_source` clone on a case-insensitive `/mnt/c` WSL mount resolves the real system `<windows.h>` rather than recursively resolving the compatibility shim.
- Added `analysis/V01C66_TOOL_READY.md`.

Historical c56-c65 tools/evidence remain read-only. No other MOD repository was changed.

## C66 proof scope

C66 reconstructs/authenticates the complete accepted c56→c65 chain. It may use a c65 terminal boundary only for the exact current root/state/path/source/full-prefix/phase/reason/provenance record. The compact c65 JSON is not a row-global certificate pool.

Every required architectural successor of the exact c65 boundary instruction must independently be classified `exact_unique_runtime_begin`, map to an exact unique runtime row/begin/end, and pass the unchanged immutable/file-backed executable preflight. Any bad, stale, interior, metadata-free executable, model-only, unsupported or mismatched successor aborts the whole root transaction before provisional candidates commit.

The next unapproved row-external boundary remains fail-closed as:

`uncertified_post_c65_runtime_boundary`

Accepted c65 input remains 998 boundary instances / 1080 architectural edges / 1021 exact unique-runtime-begin edges / 51 runtime-span interior edges / 8 metadata-free executable edges / 0 candidates. The 1021 value is an edge count, not a safe-root/instance count.

All historical Fact/must/CALL/RSP/cap/root-atomic semantics and all closed safety gates remain unchanged. C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding.

## Owner automated validation

Focused c66 command:

```bash
/workspace/toolchains/analysis-python/bin/python3 source/tools/test_slot_publication_post_c63_stitched.py
```

Actual: **13 c66 PASS groups, exit 0**.

Owner then ran the historical/static scanner regression set through c66. Actual: all 16 commands PASS, including:

```text
source/tools/test_slot_publication.py
source/tools/test_static_access_v2.py
source/tools/test_slot_publication_cfg.py
source/tools/test_slot_publication_callee.py
source/tools/test_slot_publication_direct_entry.py
source/tools/test_slot_publication_sharded.py
source/tools/test_slot_publication_boundary.py
source/tools/test_slot_publication_stitched.py
source/tools/test_slot_publication_later_boundary.py
source/tools/test_slot_publication_later_stitched.py
source/tools/test_slot_publication_post_c59_boundary.py
source/tools/test_slot_publication_post_c59_stitched.py
source/tools/test_slot_publication_post_c61_boundary.py
source/tools/test_slot_publication_post_c61_stitched.py
source/tools/test_slot_publication_post_c63_boundary.py
source/tools/test_slot_publication_post_c63_stitched.py
```

Integration/build/package owner results after the final compatibility fix:

```text
bash source/tools/build_windows.sh          PASS / exit 0
bash source/tools/test.sh                   PASS / exit 0
bash source/tools/smoke_windows.sh          PASS / exit 0
bash source/tools/test_hooks_windows.sh     PASS / exit 0
python3 source/tools/package.py             PASS / exit 0
```

Package validation produced the existing disabled validation artifact `source/output/MSM2-WS-v0.1c38/MSM2-WS-v0.1c38.script.disabled`; runtime/package version remains unchanged.

### First owner build failure and fix

The first `build_windows.sh` run from the canonical Windows `_source` clone failed in MinHook with missing Windows types. This was not a c66 scanner failure and not vendor-source corruption. On the case-insensitive `/mnt/c` mount, WallSprint's `tools/compat/Windows.h` collided with MinHook's lowercase `<windows.h>` include and recursively resolved the shim instead of the real toolchain header.

The WallSprint-owned shim was changed to include the next real system Windows header. No MinHook vendor source, production hook behavior or gameplay behavior was modified. The owner repeated the build and all remaining validation successfully.

## Safety / unrun domains

No c66 game test, Overstrike use, Update-WS, native probe, slot qword read, pointer chase, producer traversal or gameplay/production write was run or authorized. These are outside the c66 OFFLINE/STATIC tool-validation task.

Actual private-image c66 distribution metrics remain **UNKNOWN** until the owner runs the one offline scan below. Zero candidates must not be interpreted as writer absence.

## Owner next action

Run only this OFFLINE command. Reuse existing c56 checkpoint evidence and accepted c57-c65 JSONs read-only. Return only:

`logs/MSM2-WS-v0.1c66-post-c63-stitched.json`

```powershell
$gameWsl = "/mnt/c/SteamLibrary/steamapps/common/Marvel's Spider-Man 2"
$wsWsl = "$gameWsl/Mods Library/Mods-Developing/MSM2-WS"
$wsRoot = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\Mods Library\Mods-Developing\MSM2-WS"
if ((git -C "$wsRoot\_source" branch --show-current) -ne "codex/v01c20-edi-value-provenance") { throw "Unexpected branch; stop here" }
git -C "$wsRoot\_source" pull --ff-only origin codex/v01c20-edi-value-provenance
if ($LASTEXITCODE -ne 0) { throw "WS source update failed; stop here" }
wsl -d Ubuntu-24.04 --exec /workspace/toolchains/analysis-python/bin/python3 "$wsWsl/_source/source/tools/slot_publication_post_c63_stitched.py" "$gameWsl/Spider-Man2.exe" --c56-cache "$wsWsl/logs/MSM2-WS-v0.1c56-slot-publication-sharded.shards" --c57-json "$wsWsl/logs/MSM2-WS-v0.1c57-runtime-boundary.json" --c58-json "$wsWsl/logs/MSM2-WS-v0.1c58-slot-publication-stitched.json" --c59-json "$wsWsl/logs/MSM2-WS-v0.1c59-later-runtime-boundary.json" --c60-json "$wsWsl/logs/MSM2-WS-v0.1c60-slot-publication-later-stitched.json" --c61-json "$wsWsl/logs/MSM2-WS-v0.1c61-post-c59-runtime-boundary.json" --c62-json "$wsWsl/logs/MSM2-WS-v0.1c62-post-c59-stitched.json" --c63-json "$wsWsl/logs/MSM2-WS-v0.1c63-post-c61-runtime-boundary.json" --c64-json "$wsWsl/logs/MSM2-WS-v0.1c64-post-c61-stitched.json" --c65-json "$wsWsl/logs/MSM2-WS-v0.1c65-post-c63-runtime-boundary.json" --output "$wsWsl/logs/MSM2-WS-v0.1c66-post-c63-stitched.json"
if ($LASTEXITCODE -ne 0) { throw "c66 offline continuation refused; preserve existing evidence" }
```

No updater/game/runtime/native follow-up should be performed unless this command reports a specific stale/missing input.
