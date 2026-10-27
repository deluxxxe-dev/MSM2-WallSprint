# Codex result — c58 exact-runtime-begin continuation

2026-10-27 JST. Branch **codex/v01c20-edi-value-provenance**. Task base **9c66715e8b3b29a4a0f5d02df0b172b44ab005b2**.
Primary **EXACT_RUNTIME_BEGIN_STITCH_SCANNER_READY**; implementation/source commit **5897ef8d00ef5d830e242ed4bfb61c65e593b304** was pushed and
independently verified; final metadata HEAD is reverified and reported in chat. BLOCKED0. Real c58 private
image scan **UNAVAILABLE**, all new real metrics UNKNOWN/null, not zero.

## Result and changes

New separate bounded offline entry-instance scanner + owned focused tests. Exact
c56/c57 replay authenticates source rows/code/instruction/architectural edges and
target row/RX bytes before carrying unchanged c53 Facts. Different root/state
instances never contaminate independent EMPTY-root scans. Local exact must joins,
original fact/summary language and RSP kill remain. Separate boundary paths keep
address-hop semantics unchanged. Same-state cycles dedup; changed-state cycles,
uncertified later boundaries/targets, unknown semantics or caps abort entire root,
including earlier provisional candidates. Factless edges do not seed new roots.
Candidate counts are entry-instance observations only, not global unique sites.
Read-only existing checkpoint/JSON inputs must exactly match; no silent fallback.

Accepted c57 owner distribution is now historical:162472 rows/201788 architectural
edges/0 model-only,159734 exact beginnings/29560 interiors/12494 free RX,90430
adjacent+69304 nonadjacent exact. Prior c57 owner and all old scanner/test/evidence
bytes preserved. No production restoration/ownership/handler/lease/cleanup/VCH/
mitigation/watch/gameplay edit or new memory/value/pointer/producer semantics.
All completeness/publication/allocation/base/range/lifetime/ownership/CFG/EH gates
false; C52 FAIL binding. Runtimev0.1c38/package0.1.56 unchanged, no game release.

Files: new tools/slot_publication_stitched.py + test_slot_publication_stitched.py;
root/source V01C58_STITCHED_CONTINUATION.md, GATE.md, STATUS.json; existing hooks
adds mandatory focused test, package includes c57 accepted owner+c58 evidence and
manifest gate; PROJECT_STATE and CODEX_RESULT updated. No CODEX_NEXT overwrite.
Original five untracked ZIP/bundle artifacts remain unstaged. No WS main merge,
reset or force push. Detailed proof/count/cap contract is in mirrored analysis.

## Tests, exact commands, expected and actual

From repository root, existing pinned Capstone5.0.6 environment:

```bash
export PATH=/workspace/toolchains/analysis-python/bin:$PATH
```

| Exact command | Expected | Actual |
| --- | --- | --- |
| `python3 source/tools/test_slot_publication.py` | historical c38 semantics unchanged | 20 PASS; exit0 |
| `python3 source/tools/test_static_access_v2.py` | static access/AVX/refusal unchanged; optional private input SKIP | 1 PASS / 1 SKIP; exit0 |
| `python3 source/tools/test_slot_publication_cfg.py` | c53 normal CFG/exact must unchanged | 22 PASS; exit0 |
| `python3 source/tools/test_slot_publication_callee.py` | c54 authenticated summaries unchanged | 26 PASS; exit0 |
| `python3 source/tools/test_slot_publication_direct_entry.py` | c55 direct-entry rules unchanged | 32 PASS; exit0 |
| `python3 source/tools/test_slot_publication_sharded.py` | c56 complete shards/cache/counters unchanged | 25 PASS; exit0 |
| `python3 source/tools/test_slot_publication_boundary.py` | c57 first-refusal classification and readonly evidence unchanged | 27 PASS; exit0 |
| `python3 source/tools/test_slot_publication_stitched.py` | only exact architectural edges; separate states/must/cycles/caps/auth/refused roots cannot leak candidates | 32 PASS; exit0 |
| `bash source/tools/build_windows.sh` | Windows warnings-as-errors build succeeds | build success (case count not emitted); exit0 |
| `bash source/tools/test.sh` | portable ASan/UBSan regressions pass | 13 PASS; exit0 |
| `bash source/tools/smoke_windows.sh` | owned Windows smoke passes | 7 PASS; exit0 |
| `bash source/tools/test_hooks_windows.sh` | all historical hooks/ABI/lifetime/counterexamples plus mandatory c58; native VCH absence UNAVAILABLE | 574 PASS / 5 SKIP / 17 UNAVAILABLE; exit0 |
| `python3 source/tools/package.py` | disabled c38 script/PE/ZIP CRC and current tools/docs/evidence package valid | 1 PASS; exit0 |

Standalone scanner commands are repeated by hooks; do not double count them.
Unique regression groups: **PASS 594 / SKIP 5 / UNAVAILABLE 17 / BLOCKED 0**. Focused c58 32 PASS included in hooks; build success and package1 PASS separate.

SKIP5: optional private original-byte positive replay, private native replay,
private game-body installer positive, private584-byte normalization and accepted-
build capture replay inputs absent. Static-access standalone's1 SKIP is the same
optional input counted by hooks; not another unique skip. UNAVAILABLE17 in Wine:
c40 actual-VCH/order2, c49 mixed-dispatch6, c50 identity7, c51 post-return2. Zero
callbacks/registration success is not native dispatch evidence. All historical
counterexamples remain. Additional UNAVAILABLE1: actual pinned-image c58 scan,
private EXE unavailable. No requested validation command omitted; no blocked
subtask. Native game/new probe unrun because c58 is offline only, not required.

Development focused command also ran before Required validation: first test had
an incorrect zero-reader expectation for the load that legitimately reads the
slot before invalidating its loaded base; corrected the fixture to preserve that
reader and refuse the subsequent loaded-value dereference. Final32 groups pass;
no value semantic was broadened. No production source changed to obtain PASS.

## Artifact, integrity, publication and owner next action

Validation-only artifact:
`source/output/MSM2-WS-v0.1c38/MSM2-WS-v0.1c38.script.disabled` plus current tools/
docs/compact evidence. No new runtime release/native probe/private data packaging.
Final metadata/package mirror checks and GitHub checkout comparison are recorded
below and in STATUS. Documentation/publication-only edits after code validation
require package/integrity recheck, not repeating unchanged compiled regressions.

After publication, run ONLY the following PowerShell block, then return the new
compact c58 JSON from logs. Existing c56 checkpoints and c57 JSON are read-only;
if moved, point the two flags at their real locations. No Update-WS, game,
Overstrike, EXE upload, runtime log or standalone Windows probe is requested.

```powershell
$gameWsl = "/mnt/c/SteamLibrary/steamapps/common/Marvel's Spider-Man 2"
$wsWsl = "$gameWsl/Mods Library/Mods-Developing/MSM2-WS"
$wsRoot = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\Mods Library\Mods-Developing\MSM2-WS"
if ((git -C "$wsRoot\_source" branch --show-current) -ne "codex/v01c20-edi-value-provenance") { throw "Unexpected branch; stop here" }
git -C "$wsRoot\_source" pull --ff-only origin codex/v01c20-edi-value-provenance
if ($LASTEXITCODE -ne 0) { throw "WS source update failed; stop here" }
wsl -d Ubuntu-24.04 --exec /workspace/toolchains/analysis-python/bin/python3 "$wsWsl/_source/source/tools/slot_publication_stitched.py" "$gameWsl/Spider-Man2.exe" --c56-cache "$wsWsl/logs/MSM2-WS-v0.1c56-slot-publication-sharded.shards" --c57-json "$wsWsl/logs/MSM2-WS-v0.1c57-runtime-boundary.json" --output "$wsWsl/logs/MSM2-WS-v0.1c58-slot-publication-stitched.json"
if ($LASTEXITCODE -ne 0) { throw "c58 offline scan refused; preserve existing evidence" }
```


## Final verification procedures

- `python3 /workspace/scratch/ws-c58/verify.py`: expected no changes to1053
  protected baseline files,289 tested code/tool hashes match current bytes and
  package mirrors, root/source/out evidence and docs agree, C52 FAIL/c58 actual
  UNKNOWN/false gates retained. Actual PASS1. This local checker consumes the
  committed STATUS hash inventory; no proprietary inputs are involved.
- `git diff --check`: expected no whitespace errors; actual exit0.
- `python3 source/tools/package.py` repeated after metadata-only edits: expected
  current complete tools/docs/evidence with disabled unchanged runtime; actual
  PASS. Compiled tests were not repeated because no executable source changed
  after the successful full suite; exact289-input digest is checked instead.
- Wisdoms `docs/REVERSE_ENGINEERING_GUIDE.md` updated with accepted c57 owner
  distribution and reusable entry-instance/cycle/root-transaction method.
  Commit **b74c32c2792a1991a61b3da33e55f8ae4185e99b**, pushed main.
  `git -C /workspace/scratch/msm2-wisdoms-c51 ls-remote origin refs/heads/main`:
  expected pushed SHA; actual exact match (PASS1).


## Publication accepted

Implementation/source commit **5897ef8d00ef5d830e242ed4bfb61c65e593b304** is published on
**codex/v01c20-edi-value-provenance**. `git push origin codex/v01c20-edi-value-provenance`
succeeded (normal push). `git ls-remote origin refs/heads/codex/v01c20-edi-value-provenance`
matched local HEAD exactly. `python3 /workspace/scratch/ws-c58/check_publication.py`
fetched the intended branch in an independent GitHub checkout: expected identical
12 changed files, all289 tested code/tool inputs and protected evidence; actual
**320 distinct files byte/hash equal, PASS1**. Tested input digest:
`91a7f2e66f551a172e88644d4ec79a07ee54edb39985942fd28ed9fb296da770`.
C58 actual counts UNKNOWN and C52 FAIL retained in fetched evidence. Final
publication-record-only commit follows this pinned implementation; its HEAD is
again remote/checkout verified and returned in chat (avoids self-referential SHA).
No compiled regression rerun needed: all289 executable inputs still match the
successfully tested digest. Final package/mirror/integrity checks are repeated.
