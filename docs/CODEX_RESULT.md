# Codex result — Steering Probe retirement

STATUS: success, scoped tooling implementation; real-PC duplicate acceptance unknown.
Branch codex/v01c20-edi-value-provenance, baseline9fd7c4a. No main merge.

Changed: source/tools/workspace.py, retired_probes.py, package.py,
windows/workspace-build.sh, test-workspace.py; DEV_RULES/LOCAL_WORKSPACE,
mod-tools OVERSTRIKE/MANUAL, PROJECT_STATE, this result and analysis/
STEERING_PROBE_RETIREMENT.md; matching r2 source/install ZIPs. No r2 runtime
label/version/DLL/script change (v0.1c38r2 /0.1.58).

Cause: archive-only sparse protection missed direct stage discovery. Overstrike
recursively discovers stage and archive members; new paths default checked.
Actual owner duplicate paths not established; no proof updater toggled profiles.
Retirement uses exact names plus metadata/member/payload original-SHA proof,
regular/no-reparse files, clean tracked Git, stopped processes and lock. Backups
are unique game-root/MSM2-WS-Retired-Probes/retired-* outside Mods Library, with
recovery manifest/hash. Profiles, SF, ordinary mods and unknown files untouched.

| Exact command | Expected | Actual |
| --- | --- | --- |
| `python3 source/tools/test-workspace.py` | original13 groups plus owned duplicate/idempotence/collision/identity/dirty/reparse/sparse/build contamination cases | exit0;18 PASS groups |
| `python3 environment/test_mod_environment.py` | shared backup/restore/launch isolation unchanged | exit0;28 tests OK |
| `python3 environment/test_start_overstrike_preflight.py` | existing preflight gates unchanged | exit0;PASS |
| `python source/tools/release.py` | package validates, both ZIP CRCs, retirement module included | exit0;PASS |
| `python3 /tmp/ws-retire-dist-check.py` | stale stage output refused; research stage not distributed | exit0;PASS |
| `git diff --check` | no whitespace errors | exit0 |

All executed with analysis-python on PATH. Separate byte comparison against
baseline r2 script equals; ZIP CRC/member scan verifies no nested stage/archive
member. Source builders retained. Detailed fixture/setup/intermediate correction
in analysis/STEERING_PROBE_RETIREMENT.md. PASS counts are different test units,
not summed assertions. SKIP0/BLOCKED0; owner Windows UI/duplicate-origin check
UNAVAILABLE. Native suites unperformed because task explicitly excludes unrelated
native reruns and runtime bytes are identical. No game/loader operation performed.

Owner: if old updater refuses before fetch, guarded source refresh is documented
in LOCAL_WORKSPACE. Outside-WS copies require tooltip path verification and manual
movement of only exact probe container; actual path unknown, never broad cleanup.
No mandatory game run. Queued r3 starts only after this implementation is pushed
and independently read back. Publication commit/head/blob comparison follows.
