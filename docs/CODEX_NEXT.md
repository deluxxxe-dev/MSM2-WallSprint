# Priority — eliminate recurring update-lock / stale launcher dead ends
Branch codex/v01c20-edi-value-provenance; no main merge.
Pause gameplay/movement tasks. Read AGENTS required docs, docs/mod-tools/{README,OVERSTRIKE,PLAY_MODDED,BACKUP_MODS,MANUAL}.md, environment/README.md and relevant shared installer analysis.

## Owner authorization / evidence
Owner cannot use shortcuts:
1) old installed checker said Remove misplaced Overstrike/scripts_proxy.dll; current source permits regular non-reparse installer DLL.
2) source environment/Start-Overstrike.cmd now stops: WS update lock is present; let Update-WS finish first.
Owner explicitly requests durable correction rather than repeated manual file deletion/renaming. Actual active-versus-orphan lock on owner PC is UNKNOWN.
VERIFIED-STATIC current workspace.py update creates EMPTY _archive/Update-WS.lock via O_EXCL, closes descriptor, unlinks only in finally. self.system.launch(game) occurs inside transaction BEFORE final lock removal. environment ws_launch_ready rejects lock existence alone. Source start_manager checks readiness BEFORE copying fresh KIT into installed Mod-Tools. Do not claim launch method invokes checker until actual call chain reviewed.
No owner-PC access; no deployment claims.

## Implement one coherent tooling fix
1. Identify actual Update-WS -> process/build -> launch and shortcut refresh call chain. Fix ordering: finish/validate deployment and release update ownership BEFORE invoking checked shared launcher. Launcher failure after committed deployment must report launch failure separately, not falsely roll back good WS or leave lock stuck.
2. Replace presence-only sentinel with robust OS-backed mutual exclusion/process-lifetime ownership suitable for Windows updater and all shared entrypoints. Prefer OS-released lock over stale PID-only guesses; retain audit metadata (owner/process-start identity/workspace/token) where useful. Handle PID reuse, concurrent claim/reclaim, WSL/build children, exceptions/interruption, and never release another owner's lock. Cross-process Windows semantics must be tested, not assumed from Linux unlink.
3. Distinguish active update vs abandoned artifact. Auto-recover only provably inactive owned state with path/reparse/identity/race checks; preserve legacy lock artifact reversibly if safe. Legacy empty lock cannot prove live/dead by age alone. Provide a narrowly scoped recovery/bootstrap for pre-fix orphan empty lock with verified process/build inactivity; ambiguity must explain exact owner/evidence rather than endlessly generic retry. Do not kill processes automatically or blindly delete lock/index.lock.
4. Treat shared .MSM2-Mod-Tools.lock and updater lock consistently where entrypoints interact; no separate broad rewrite. Prevent simultaneous launch/install/deploy; no check-then-act race. Preserve backup/restore consistency and live-operation protection.
5. Fix installed/source tooling refresh deadlock: updater/repair can stage verified current owned KIT safely without first requiring the old failing checker. Automatic refresh on successful normal update must preserve mod-tools settings/backup location/profile and shortcuts; idempotent, no vendor DLL relocation or manager recreation. Source/installed paths supported. Installer source Overstrike/scripts_proxy.dll permitted; Overstrike/winmm.dll remains distinct. Do not bypass all environment validation.
6. Supply one concrete copy-paste/bootstrap action for owner that can work BEFORE old launcher/updater checks. Retain clean/dirty Git/branch protections and existing previous runtime; do not require full WS rebuild just to update tooling.

## Scope
Allowed: WS updater/source Windows entrypoints, shared environment launcher/checker/install files strictly for above, focused tests, matching individual/combined manuals, CODEX_RESULT/PROJECT_STATE.
Keep WS r4/runtime/package bytes unchanged; no native test suite rerun for tooling only.
No SF source/runtime or ordinary mods/selection mutations, shared loader deletion/moves, profiles reset, broad cleanup, old checkout deletion, git reset/clean/main merge.
Existing lateral-route research complete; keep results, no gameplay work in this task.

## Required meaningful regressions
- Successful update -> checked manager start with no live update lock; committed artifact remains if launch fails.
- Two concurrent updates cannot both deploy; active update blocks Play/Start/backup safely.
- Crash/exception and interrupted parent/remaining build child; safe eventual recovery only after ownership ends.
- Stale owner/PID reuse, empty legacy lock, malformed metadata, reparse path and replacement/claim races.
- All shortcuts from old installed KIT refresh through supported bootstrap; repeat refresh preserves DLLs, profile/settings and unrelated files.
- Update/test/build failure preserves old runtime; dirty Git and divergent branch remain protected.
Use existing workspace/environment/distribution checks and real Windows-capable fixture where available; clearly label unavailable checks. No simulated proof promoted to real PC acceptance.
Update result with exact root cause, tested cases, publication and ONE owner repair action. Commit/push, verify. Reply STATUS/CHANGED/RESULT/NEED only.
