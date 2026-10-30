# Priority — r5 updater WSL post-build verification failure
Branch codex/v01c20-edi-value-provenance; no main merge. Read AGENTS required docs, current CODEX_RESULT, analysis/UPDATE_LOCK_TOOLKIT_RECOVERY.md and relevant updater/environment manuals.
r5 heading implementation and validation already published; no further gameplay change.

## Supplied owner result
Update terminal ends:
PASS: required full WSL build/test/package completed; disabled artifact staged for transactional WS-only deployment
WS ERROR: WSL build not proved inactive: Cannot verify same-user Linux process 339
No final real r5 update success. Earlier v0.1c20 update complete lines inside workspace tests are synthetic fixture output, not owner deployment evidence.
r5 package output exists in temporary validation checkout; owner installed state not independently checked. Do not claim r5 deployed or previous runtime lost.
Actual process339 identity/liveness and failure reason UNKNOWN; do not assume orphan or permission failure.

## Scope
Find exact error producer in existing ownership/process checker and its post-build caller. Distinguish disappeared /proc PID race, access denied, malformed/incomplete output, still-active descendant, unrelated same-user process, self-inspection probe, distro/UID identity and genuine unknown.
Record minimum cause-specific diagnostics (distro, pid, stage/error category, exit status; avoid full unrelated command lines/private env).
Fix evidence-backed false positive minimally. Disappeared PID must be revalidated using reliable process identity/absence evidence, not treated as arbitrary unreadable active process. Still-active build children retain update ownership; permissions/uncertainty must not silently pass. No age-only/PID-only stale decisions, arbitrary sleep or broad same-user exemption. Do not terminate all WSL or unrelated processes.
Review whether self-created probe process can be misclassified, but do not assert this is actual cause without evidence.
Preserve OS exclusion, build descendants, commit/unlock/launch ordering and previous artifact protections.
If private owner state is necessary, provide ONE compact read-only diagnostic command via supported tooling with exact fields and runnable from current installation despite failure.

## Recovery / checks
Allow updater/ownership/WSL checker and focused test/manual/result/state changes only; preserve r5 binary/package and native code byte-for-byte.
Test PID exits between enumerate/read, permission denial, live build child, unrelated process identity, PID reuse, self probe, distro mismatch and concurrent update; run existing focused workspace/environment checks. Label Windows/WSL checks unavailable if not actually run.
No full native rebuild/tests for tooling-only fix. No SF/profile/ordinary MOD/loader mutation; no checker bypass.
Provide concrete bootstrap refresh if old installed updater cannot fetch before its failing check. Prefer documented supported refresh, preserve dirty clone.
Assess whether existing validated staging can safely be reused with exact source/build manifest identity; do not add unsafe cache shortcut or require owner to manually deploy unverified script. If retry must rebuild, state honestly.
Commit/push and verify. CODEX_RESULT must distinguish validated build from deployment and identify ONE owner next action. Reply STATUS/CHANGED/RESULT/NEED only; Chat gives game-test instructions after deployment succeeds.
