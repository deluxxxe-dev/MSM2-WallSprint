# Codex result — canonical c67 runner path repair

2026-10-30 JST. Branch `codex/v01c20-edi-value-provenance`.
Baseline: c51aa453ad8b6c53f5911d8820bc1904cbeab175.

## STATUS

**success** — bounded path repair verified from three working directories;
unchanged canonical hooks command completes. No runtime/reversal/version change.
Actual owner Windows updater/game validation remains unperformed.

## CHANGED

- `source/tools/test_slot_publication_post_c67_boundary_fix.py`: import Path;
  derive child path using `Path(__file__).resolve().with_name(...)`.
- `docs/CODEX_RESULT.md`, `docs/PROJECT_STATE.md`.
- Necessary package documentation/source refresh: r1 install/source ZIPs and any
  generated manifest metadata. Runtime stays v0.1c38r1 / package0.1.57; script/DLL
  content remains byte-identical. No new package version or gameplay change.

## RESULT / narrow scope

Original command `(cd source && python3 tools/test_slot_publication_post_c67_boundary_fix.py)`
reproduced exit1/FileNotFoundError at `source/source/tools/test_slot_publication_post_c67_boundary.py`.
Only this wrapper's child path changes. All assertions, patch relationships,
runpy globals/defaults and `run_name='__main__'` are preserved. No chdir, failure
masking, omission, gate relaxation or updater bypass.

The directly related full `test_slot_publication_post_c67_boundary_fix2.py` also
contains a cwd-relative runpy path, but is NOT on the canonical command list.
Canonical uses `test_slot_publication_post_c67_boundary_fix2_quick.py`, which
contains no such runpy path. No unrelated/non-required wrapper was changed;
do not claim every optional wrapper was made cwd-independent.

## Exact execution / expected and actual outcomes

Environment: `export PATH=/workspace/toolchains/analysis-python/bin:$PATH`.
Counts are emitted PASS records per invocation, not unique summed cases.
The same14 wrapper groups run repeatedly to check cwd independence.

| Command | cwd | Expected | Actual |
| --- | --- | --- | --- |
| `python3 source/tools/test_slot_publication_post_c67_boundary_fix.py` | `/workspace/WallSprint` | exit0; complete existing assertions | exit0; PASS 14, SKIP 0, UNAVAILABLE 0 |
| `python3 tools/test_slot_publication_post_c67_boundary_fix.py` | `/workspace/WallSprint/source` | exit0; complete existing assertions | exit0; PASS 14, SKIP 0, UNAVAILABLE 0 |
| `python3 /workspace/WallSprint/source/tools/test_slot_publication_post_c67_boundary_fix.py` | `/tmp` | exit0; complete existing assertions | exit0; PASS 14, SKIP 0, UNAVAILABLE 0 |
| `bash source/tools/test_hooks_windows.sh` | `/workspace/WallSprint` | exit0; complete existing assertions | exit0; PASS 895, SKIP 5, UNAVAILABLE 17 |

Original failure reproduction: exit1 (before repair), retained in cloud scratch;
corrected root/source/temp invocations and canonical chain exit0. No runtime
code was changed to obtain these results.

## Existing updater command list / reused valid checks

Read-only inspection of `source/tools/workspace.py` and
`source/tools/windows/workspace-build.sh:31-35` confirms the required serial list:

```bash
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python source/tools/release.py
```

The first three were already valid for the unchanged final r1 code commit
`985c4bccf8aa4a7c21df66fe19c63f3367366059`, recorded in previous CODEX_RESULT:
Windows build exit0; portable23 PASS (including focused10); smoke7 PASS. Existing
native source command `bash source/tools/test_native_source.sh` also passed10.
These are **reused prior results, not newly executed tests in this task**.
No C++/headers/version/config/build/updater/canonical script diff exists from
this task baseline; DLL, disabled script and version-header hashes were recorded
before execution and checked unchanged. Rebuilding/repeating unaffected checks
would add no new evidence. Canonical hooks is newly executed in full; release/
package is newly executed for changed source/docs, with CRC/content validation.
No actual Windows Update-WS, WSL staging entrypoint or local deployment claimed.

## Remaining evidence / counts

Five optional private replay SKIPs are retained: original-byte positive/native
body/installer/writer-normalization/accepted-build capture. Owned negatives and
synthetic/native fixtures still run. The17 Wine UNAVAILABLE outcomes remain:
c40 VCH2, c49 bridge6, c50 identity7, c51 post-return2. They are not native PASS.
Owner Windows updater and r1 gameplay/facing outcome remain unverified, and no
new game test/capture or human instructions are supplied by this task.
BLOCKED0 for this bounded repair; no other canonical blocker observed.
C52 and all existing proof/write gates remain binding.

## Package / scope validation

`python source/tools/release.py`: expected exit0, unchanged r1 runtime version/
script/DLL content, changed wrapper packaged correctly, ZIP/script CRC pass.
Actual: exit0, CRC/content checks pass; packaged wrapper equals the corrected
checkout; DLL, disabled script and header SHA256 exactly match task-start values. `git diff --check` and
`git diff --cached --check`: expected exit0, actual exit0. Diff restricted to
wrapper path, task/state report and necessary generated package documentation.
Five pre-existing untracked archives/bundle excluded and preserved.
No runtime/design/watch/VEH/VCH/producer/c73/shared launcher/loader/config/SF or
other repository changes. No main merge, reset or force push.

Runtime byte hashes (unchanged from task start):
- `source/build/windows/WallSprint.dll` SHA256 `88272f32adcfab0afde51f8949faed0cef8fb190a18becfb59befa2f9fa12ece`
- `MSM2-WS-v0.1c38r1.script.disabled` SHA256 `eaaf55b9017680bb47c3e4c07249964424c84723edf5c068072548f6b77f68db`
- `source/include/version.hpp` SHA256 `febfcbb198c98f52a02e3ddaedfaa69b366206078133cb91f803d0863189cea5`

## Branch / publication

Branch `codex/v01c20-edi-value-provenance`. Implementation SHA and independent
GitHub readback will be finalized after normal commit/push, before owner handoff.

## NEED

No remaining canonical blocker. Only owner Windows/updater/game evidence remains
unverified; ChatGPT supplies subsequent instructions, not this task.
