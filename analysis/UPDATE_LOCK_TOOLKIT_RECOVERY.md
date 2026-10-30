# Update ownership / stale KIT repair — tooling-only review

2026-10-31 JST. VERIFIED-STATIC plus owned fixtures; owner PC remains UNKNOWN.
Baseline 6eeef17 on codex/v01c20-edi-value-provenance; no main merge.

## Actual old chain / root cause

Update-WS.cmd -> workspace.py Manager.update -> WindowsSystem.build -> validate /
deploy -> WindowsSystem.launch (raw Popen Overstrike.exe), then sentinel unlink
in finally. The old launch did NOT invoke the environment checker. Empty O_EXCL
sentinel descriptor was closed immediately, so process death left existence-only
state. Source Start checked ws_launch_ready BEFORE installed KIT refresh. Old
installed preflight also performed DLL/attribute/PID cleanup, now removed.

## New contract

One stable game-root .MSM2-Operations.guard uses Windows CreateFile read/write
ownership with FILE_SHARE_READ only (no write/delete sharing), checked opened-file
reparse attributes. Shared tools/update/repair acquire this same OS gate.
Compatibility .MSM2-Mod-Tools.lock and Update-WS.lock retain versioned metadata:
PID plus process creation identity, token, workspace, action/phase/state. Released
records remain intentionally: obsolete presence-only updaters cannot bypass new
ownership. Never unlink a guard or release a replaced/foreign-token record.

Git, PowerShell and WSL build children inherit explicit handles. Parent crash
cannot release a surviving child's ownership. Building phase additionally records
exact distro/token; reclaim checks remaining same-user Linux token processes.
Legacy empty/PID-only artifacts require read-only Windows process inventory and
all running WSL distro inventories; unreadable/ambiguous evidence refuses. Byte
copies are preserved under .MSM2-lock-recovery before known inactive rewrite.
No age-based ownership inference, blind index.lock deletion or automatic kills.

Validated runtime deployment commits and releases update ownership BEFORE current
checked launcher invocation. Launch failure reports separately and retains good
new deployment. Existing build/hash/test failures preserve previous runtime.
KIT refresh is verified/idempotent, copies only owned files, preserves previous
KIT, settings/profile/backup path. Restore overlays current KIT before switch.
Regular installer scripts_proxy.dll is accepted; misplaced manager winmm refused
without DLL relocation/deletion. Current Windows launch still uses game-root CWD.

## Bootstrap / deployment

Repair-Mod-Tools.ps1 fetches canonical branch sources into private temporary
staging, then repair_tools.py holds fresh ownership before clean Git/ancestor/
ff-only validation and current KIT refresh. Old installed checker is not executed.
Independent exact workspace layout and origin/branch are mandatory. No reset,
clean, rebuild or runtime deployment. See single owner block in mod-tools README.
Future bootstrap/source packaging includes shared toolkit/ownership dependency;
existing r4 output is deliberately not regenerated.

## Evidence limits

Portable cross-process tests cover races/crash/inheritance/replacement/identity;
Win32 API executable under Wine covers deny-delete/share and inherited/crashed
parent handles. Wine reparse materialization unavailable. This is NOT an actual
owner Windows Python/CIM/WSL/shortcut acceptance test. Full cloud /proc scanning
refuses inaccessible process metadata (SKIP); isolated owned process-token read
passes. No production safety check weakened to accommodate that restriction.
Wisdoms synchronization pending; detailed reusable proof retained locally.
