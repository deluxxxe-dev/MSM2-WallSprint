# Next task — stop retired Steering Probes returning/enabling during WS update

Branch codex/v01c20-edi-value-provenance. Do not merge main.
Read AGENTS required docs, current PROJECT_STATE/CODEX_RESULT and relevant shared Overstrike/layout manuals.
r2 implementation is complete; runtime response pending. Do not modify it.

## Owner request / evidence
2026-10-31 screenshot: TWO checked identical "WS Steering Probe - TurnSpeedSlow 5" entries beside checked WallSprint. Owner reports probes reappear/re-enable during Update-WS and authorizes retirement/removal of these WS test probes. They are no longer required for the current experiment.
Do not assume updater explicitly toggles them: distinguish archive discovery, duplicate paths, profile new-entry defaults and actual selection mutation.

## Purpose / bounded change
Inspect only WS updater/package/discovery/sparse-checkout paths and exact probe names. Determine how stage files or archive members enter recursively scanned Mods Library, including _source/_archive and duplicate copies.
Normal Update-WS must deploy current WS runtime only and must not copy, unpack, regenerate, auto-enable or reintroduce retired Steering Probes.
Preserve source builders and research history. Retain useful probe artifacts outside scanner-visible paths or in genuinely ignored disabled form; verify scanner behavior rather than assuming an archive disabled extension hides a nested active stage.
For recognized retired probes already inside MSM2-WS, updater may preserve/move them to a dedicated location OUTSIDE Mods Library after validating identity (exact known probe names plus package metadata/payload as needed), regular files/no reparse, no overwrite and reversible archival. This is explicit authority ONLY for these WS probe artifacts, not general cleanup.
If copies live elsewhere in shared Mods Library, do not blindly remove/move broad matches. Report exact verified origin/path and an existing user-operated removal method, or build a separately invoked narrowly scoped retire-probes action if needed. Never scan unrelated file contents unnecessarily.
Do not edit Overstrike vendor code or broadly rewrite profiles. Preserve user's enabled/disabled choices and ordering for every unrelated MOD and existing WS runtime. No enable-all/reset profile/defaults.
No automatic game install/reinstall, loader mutation or local-PC claim.

## Allowed / forbidden
Allowed: minimum WS update/distribution/sparse policy fix, focused regression tests, matching local manuals and combined manual if affected, CODEX_RESULT/PROJECT_STATE. Keep existing archive visibility protection and Git history.
Forbidden: r2 DLL/gameplay/version changes; probe field changes; SF/IDLE/ordinary MOD mutations; destructive library-wide cleanup; symlinks; disabling sparse protections; reset/clean/main merge; hook/watch/c73 work.

## Completion / checks
Use existing required workspace/environment test commands. Test:
- normal update never introduces probe stage/ZIP/archive members into scanner-visible workspace;
- repeated updates are idempotent;
- known duplicate retired WS probes can be reversibly retired only within explicitly owned scope;
- unrelated stage/script/archive files and profile choices untouched;
- spaces/Unicode, collisions, dirty Git and reparse safeguards retained.
If owner manually removes a probe, next update must not restore it.
Keep v0.1c38r2/package0.1.58 runtime bytes unchanged; no unrelated native-suite rerun needed for tooling-only change.
Record verified cause, precise scope, commands and remaining owner acceptance; commit/push and verify remote.
If current installed updater needs a bootstrap before fetch, use documented guarded refresh and report it, not a validation bypass.

Reply briefly:
STATUS: success / partial / blocked
CHANGED:
- paths
RESULT:
- cause, regression checks, publication
NEED:
- exact owner action for existing duplicates only if needed
