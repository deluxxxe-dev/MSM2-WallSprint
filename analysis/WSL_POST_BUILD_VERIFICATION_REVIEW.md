# r5 updater post-build WSL verification — tooling-only review

2026-10-31 JST, baseline5741e05, codex/v01c20-edi-value-provenance.
Owner result proves the WSL build/test/package step reported completion, then
WindowsSystem.build -> operation_lock.wsl_quiet failed BEFORE Manager.update
payload validation/deploy/checked manager start. Fixture v0.1c20 success lines
are not deployment evidence. Actual active version and process339 cause UNKNOWN.

## Exact producer and bounded correction

VERIFIED-STATIC: the old message is emitted only by PermissionError caught around
/proc/PID UID, environ, cmdline and cwd combined. This establishes Python's caught
exception class, NOT actual errno/stage, PID liveness or root cause on owner PC.
An exiting/zombie proc can make these reads unavailable; genuine denial is also
possible. Self PID was already excluded; self-probe causality is not established.

New structured schema1 carries expected/observed distro and UID, PID, read stage,
category/errno, exit status and bounded32 diagnostic entries plus category counts.
No full unrelated command line, environment values or raw stderr is returned.
QUIET requires successful schema/exit/identity checks. Malformed/incomplete output,
distro/known-UID mismatch, unreadable live process, PID reuse remain fail closed.

Exact-token mode reads environ and start identities only; unrelated cmdline/cwd
is unnecessary to disprove that exact token. Legacy mode still reads both.
Reads pin a Linux pidfd when available; after failure, only exact-process exit
poll or revalidated kernel signal0 ESRCH/absence can excuse a vanished PID.
PID reuse/new identity cannot be excused using an old exited handle. A live
matching token remains ACTIVE. pidfd is closed, no process is terminated.
Fallback signal0 is an existence query, not a kill; unsupported pidfd never
converts an unreadable live process to QUIET. Exact self PID excluded, not all
Python/probe/same-user processes. New builds bind distro and numeric Linux UID
before launching; post-check validates both. Historical records have no original
UID field (uid_expected=null); diagnostics expose this limitation rather than
inventing an old identity. Current same-user scan scope is not an all-UID proof.

OS gate/inherited-child ownership, unresolved build records, deploy validation,
commit/unlock/checked-launch ordering, previous artifacts and dirty Git rules are
unchanged. Probe fixes are real owned Linux /proc/pidfd tests plus process models;
not owner Windows/WSL acceptance and not a universal process-tree certificate.

## Staging / recovery decision

The external /tmp validation checkout is normally removed by workspace-build.sh
trap. Validated disabled payload/result/manifest can persist at WS/_archive/_build.
Current manifest hashes bind bytes/DLL/version, NOT an exact source-HEAD + successful
build-session + post-build inactivity receipt. Owner staging not inspected here.
Therefore no cache reuse/manual deployment shortcut is admitted. Existing retry
runs full build again; no claim that existing output was lost or r5 installed.
This tooling-only fix does not rebuild/change checked-in r5 artifacts/native code.

Supported fresh Repair-Mod-Tools.ps1 -DiagnoseWSL stages latest authored checker
before old failed check, then reads authenticated pending metadata and queries
WSL. It takes no ownership, edits no lock/KIT/runtime/worktree and does not build,
deploy or launch. Bootstrap fetch updates Git refs/objects and uses private temp
files only; process diagnostic itself is read-only. Dirty worktree is preserved.
No authenticated pending build record yields explicit uncertainty, not success.
Owner ONE next action is this diagnostic block in docs/mod-tools/README.md.
Normal repair remains separately supported (no DiagnoseWSL) but is not requested
until actual diagnostic evidence is reviewed; no bypass/delete/sleep/WSL shutdown.
