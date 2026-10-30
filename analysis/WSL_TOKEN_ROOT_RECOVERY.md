# Confirmed environ EACCES — narrow read-only root token inspection

2026-10-31 JST. Baseline053f6a5 / codex/v01c20-edi-value-provenance.
Supplied fresh23525e8 DiagnoseWSL: expected/observed Ubuntu-24.04, observed UID1000,
legacy expected UIDnull; PIDs346/347 environ errno13, resultUNKNOWN/exit2. EACCES
is confirmed. Their identity/relevance and root inspection availability remain
UNKNOWN. No deployment/repair was performed by that diagnostic; do not repeat it.

## Selected minimum route, not a new process framework

Use the explicitly allowed read-only selected-distro root inspection. Existing
user probe binds distro and known build UID. Only QUIET or UNKNOWN caused solely
by environment access-denied (plus confirmed departed PID events) can proceed to
root. Active token, wrong distro/known UID, malformed identity/output, PID reuse
or other genuine unknowns are NOT washed by a privileged retry.

The only root command is:
`wsl.exe --distribution <recorded distro> --user root --exec python3 -c <authored readonly probe> <exact token> 0 <distro> 0 root_token`.
No shell/build/compiler/deployment runs as root. No sudo/security/sysctl/ACL/
dumpability changes, WSL shutdown, process killing or broad unreadable exemption.
Invocation is noninteractive, bounded25s; root unavailable/denied/timeouts/malformed
result or actual inspector UID!=0 fail closed with scope/distro/category/exit.

Root inspects ALL UIDs for exact inherited operation-token equality, not broad
argv or full environment dumps. It runs even after user QUIET so an escaped,
reparented or UID-changed token-bearing child cannot evade the user-only filter.
The stable inventory rechecks PID/start identities at the end; churn/new child
forces an immediate rescan, at most3 passes, then inventory_unstable UNKNOWN.
Kernel zombie/exited states are excluded consistently from both live-inventory snapshots after the existing exit/identity checks; a protected exited process cannot cause perpetual artificial churn.
No sleeps or top-level-exit shortcut. Existing pidfd/absence/PID-reuse safeguards
remain. Still-protected root reads remain UNKNOWN, not unrelated-by-assumption.

This is exact-token absence evidence under the existing inherited-token build
contract, NOT a universal supervisor/completion proof for descendants that erase
the token or arbitrary adversarial future processes. No new supervision framework
introduced. Build exit/code, manifest/hash/version/source-HEAD checks and OS-held
Windows/inherited-child exclusion remain separate and required. Existing pipeline
is never certified complete from its top-level exit alone. Inventory is not atomic;
identity/churn failures are explicit, not hidden behind an all-process certificate.

## Legacy authority / actual repair

Saved build_token must match its recorded operation token when present; a mismatched identity cannot obtain root recovery.
Known legacy UIDnull is NOT replaced with1000. User-observed UID and inspector0
are separate fields. The root all-UID exact-token scope can prove absence without
inventing the original UID. An empty/PID-only artifact with no exact token cannot
use this privileged path. Tokens/whole environ/argv are never printed.

WSL_TOKEN_CHECK logs TOKEN_ABSENT / root_token_all_uids /
authority=token_retirement_only_not_deployment. A read-only snapshot itself claims
no ownership or deployment. Only the existing held OS gate plus safe metadata /
identity/reparse checks can preserve/reclaim interrupted records; existing repair
then clean/ff-checks Git and refreshes owned KIT. Successful repair marker remains
TOOLING_REPAIR_COMPLETE runtime_unchanged=1 no_build=1 no_game_launch=1.
Denied root proof leaves previous locks/KIT/source HEAD/runtime intact in fixtures.

ONE owner action: fresh normal Repair-Mod-Tools.ps1 bootstrap from current Git,
WITHOUT DiagnoseWSL. This repairs before old checks, with root read-only evidence
when old unresolved token is encountered; no repeated full compile just to classify
processes. Dirty/divergent source still refuses. No Update-WS/game requested now.
Normal later Update-WS still rebuilds: existing staging has no authenticated exact
source/build-session/inactivity receipt. No cache/manual-deploy shortcut added.
R5 source/native/INI/binary/script/ZIP/manifests byte-identical to baseline.

## Evidence

Focused root models test protected unrelated absence vs protected matching/unknown,
known/null UID, unavailable/failed root, reused/malformed identity, mismatched distro,
root identity, escaped cross-UID token, bounded inventory churn. A separate owned
Linux subreaper harness actually lets a top-level parent exit while a reparented
new-session token child survives; probe finds ACTIVE; all test children reaped.
These are owned Linux/model tests, not actual WSL-root/owner acceptance. Existing
workspace/operation/environment/repair suites retain concurrency, failure rollback,
parent/child lifetime and clean/dirty/Git/refresh/data protections.
