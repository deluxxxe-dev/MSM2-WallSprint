# Mod-Tools 1.2.0 — tooling-only validation

2026-10-31 JST. Owned temporary fixtures only. Expected all admitted operations
pass, unsafe/concurrent/unknown cases refuse without losing runtime/user bytes.
Actual final results:

| Exact command | Expected / actual | PASS | SKIP | UNAVAILABLE |
|---|---|---:|---:|---:|
| `python3 environment/test_operation_lock.py` | cross-process lifetime, legacy/identity/race/replacement refusal; matches | 14 | 1 | 0 |
| `python3 source/tools/test-workspace.py` | update failure preserves old runtime; launch failure retains committed new runtime; concurrent update denied; matches | 20 groups | 0 | 0 |
| `python3 environment/test_mod_environment.py` | backup/restore, refresh/settings/data safety; matches | 28 | 0 | 0 |
| `python3 environment/test_tooling_repair.py` | real owned Git ff/old checker repair/idempotence/dirty/divergent/live blocks; matches | 3 | 0 | 0 |
| `python3 environment/test_start_overstrike_preflight.py` | installer proxy accepted, misplaced loader refused unchanged; matches | 1 group | 0 | 0 |
| `bash environment/test_operation_lock_windows.sh` | Win32 sharing/crash/child lifetime under Wine; matches | 4 groups | 0 | 1 |
| PowerShell command below | mocked Windows launch contract; matches | 16 checks | 0 | 0 |

```
XDG_CACHE_HOME=/tmp/ws-lock-xdg-cache XDG_CONFIG_HOME=/tmp/ws-lock-xdg-config XDG_DATA_HOME=/tmp/ws-lock-xdg-data DOTNET_CLI_HOME=/tmp/ws-lock-dotnet /tmp/ws-proxy-pwsh/pwsh -NoLogo -NoProfile -File environment/test_windows_contract.ps1
```

SKIP: full sandbox /proc environment inventory cannot inspect process1; actual
probe refused. Isolated real owned process token inventory passed, not WSL proof.
UNAVAILABLE: Wine reparse API did not materialize a reparse node; Linux symlink
refusal passed. Actual owner Windows CPython/CIM/WSL/shortcuts/deployment are
separately UNAVAILABLE (one acceptance group); no actual-PC PASS claim.
PowerShell Core on Linux uses owned mocks, not actual COM shortcuts. Native API
fixture is Win32 C++ under Wine, not the Python ctypes integration on Windows.

Initial diagnostic runs encountered read-only default PowerShell cache (fixed by
private XDG dirs), Wine reparse nonmaterialization (reported UNAVAILABLE), and an
aborted fixture path collision (unique tick suffix fixes fixture). Official Windows
embeddable Python download returned403; no Windows Python integration claimed.

`python3 environment/build_distribution.py`: toolkit-only ZIP CRC/member bytes/
SHA checks; no game, external binary, user backup or r4 rebuild. Existing r4 runtime
and packages must match baseline6eeef17. Native/game suites intentionally NOT RUN:
CODEX_NEXT forbids native suite rerun for tooling-only work. No gameplay changes.
