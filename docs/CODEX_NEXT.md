# Priority — new owner update stops mid-suite with ACTIVE token descendants
Branch codex/v01c20-edi-value-provenance; no main merge. Read AGENTS docs/current result and existing WSL ownership analyses. Previous root recovery succeeded on owner PC; do NOT repeat old EACCES-only repair task.

## New supplied owner evidence
Owner fresh745fcbf normal repair produced TOKEN_ABSENT(root/allUID,3 checks) and TOOLING_REPAIR_COMPLETE runtime_unchanged1.
Subsequent Update-WS output (420 lines) ends during c48 native fixture JSON, NOT after full WSL completion/package marker.
Final error user inspection: ACTIVE, counts access_denied2/active_build6:
unreadable environ PIDs338,339; matching-token PIDs2411,2417,2420,2424,2431,2451; UID1000 expected/observed; Ubuntu-24.04 matches.
Exact process identities/roles and why main runner returned UNKNOWN. Matching token proves operation association, not necessarily an active compiler; distinguish lingering Wine fixture services/test children. No final update success, no r5 owner deployment proof.
Do not declare this only unreadable unrelated processes, ignore active matches, or repeatedly ask same repair/rebuild.

## Concrete source issue / scope
workspace.py WindowsSystem.build:
p=subprocess.run(...workspace-build.sh...)
wsl_quiet(record)
phase(validated...)
require(p.returncode==0,...)
Thus quiet exception masks original build exit status. Fix reporting/order without permitting deploy on nonzero or live children. Record primary returncode and failing stage before secondary completion-check errors. Failed/nonquiet build cannot enter validated phase.
Trace only workspace-build.sh and invoked c48/current terminal fixture/test runner plus its child cleanup. Determine original termination reason and token-bearing descendants. Add compact operation-scoped PID/PPID/start/state/executable NAME/stage diagnostics (not full private argv/env); retain exact identity validation and root read-only restrictions.
Identify if Wine/test services inherit token and outlive harness, shell pipeline returns early, external interruption or genuine test failure. Do not assert a cause without evidence.
Fix deterministic owned test/build child teardown and wait at responsible scope. No killall/wsl shutdown/user-wide process killing, no root build, token erasure as a bypass, arbitrary sleeps, clearing pending ownership despite active children or broad executable whitelist. If graceful bounded teardown needed, restrict to authenticated owned process identity/tree and preserve unrelated Wine/user work.
For real native test failure, preserve assertions and fix actual failing implementation/fixture contract; no skipping tests to ship.
Keep r5 gameplay/binary semantics unchanged; if native fixture-only changes required report scope and run affected tests.

## Recovery / validation
Primary original exit remains visible even when quiet also fails. Test main nonzero+remaining child, success+remaining child, owned service cleanup, unrelated service untouched, stale PID reuse, interruption, access denied/root recovery and concurrent owner exclusions.
Use focused workspace/ownership/environment plus relevant failing fixture; full build only once corrected targeted checks pass.
Provide ONE read-only owner diagnostic through fresh bootstrap if exact PID roles/last build exit cannot be inferred. It must work with old checker pending ACTIVE record; no full rebuild to gather that.
Existing validated artifacts cannot be assumed present (this run ends mid-suite). Do not ask manual script copying or claim deployment.
Allowed minimum tooling/tests/analysis/manual/PROJECT_STATE/CODEX_RESULT. No WS movement/facing change, SF/profile/loaders/ordinary mods, broad process framework.
Commit/push verify and give exact recovery/bootstrap action, distinguishing source update vs owner installed KIT. Reply STATUS/CHANGED/RESULT/NEED.
