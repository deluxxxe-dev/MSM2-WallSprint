# Priority — confirmed WSL environ EACCES blocks updater
Branch codex/v01c20-edi-value-provenance; no main merge. Read AGENTS docs, CODEX_RESULT and analysis/WSL_POST_BUILD_VERIFICATION_REVIEW.md.
Prior diagnostic task complete. Do not ask owner to rerun identical DiagnoseWSL.

## Fresh owner evidence
Fresh fetched23525e8 Repair-Mod-Tools.ps1 -DiagnoseWSL:
read_only=1 ownership_or_deployment_certificate=0
exit2 JSON schema1/resultUNKNOWN
distro_expected=Ubuntu-24.04; distro_observed=Ubuntu-24.04;
uid_expected=null; uid_observed=1000;
counts.access_denied2:
pid346/stageenviron/errno13;pid347/stageenviron/errno13.
Thus EACCES at process environment read is confirmed, not disappeared PID/distro mismatch. Exact identity/relevance of processes and why permission denied UNKNOWN. Null expected UID is legacy metadata, not proved identity mismatch.
No deployment/refresh executed by diagnostic. r5 runtime retained, owner r5 deployment still unverified.

## Objective / bounded repair
Current whole-user environment inventory can be blocked by same-user non-dumpable/protected processes. Same UID alone does not guarantee readable /proc/PID/environ. Do not label every such process a WS build, or ignore all unreadable processes.
Inspect current probe/ownership and design smallest reliable way to establish actual WS build completion independently of unrelated process-env readability.
For new builds prefer owned supervised build/descendant lifetime and authenticated per-operation marker/OS handles/process identity; test escape/remaining descendants. Do not invent completion certification from top-level exit alone.
For existing legacy unresolved record, preserve conservative matching and provide supported narrow recovery that proves token absence; if required use bounded READ-ONLY elevated WSL probe (e.g. explicit selected distro root user) with same distro/identity validation. Never expose full environment/argv, mutate WSL permissions/sysctl/security settings, stop all WSL, kill processes or execute build/deployment as root. Privileged inspection availability/ptrace restrictions not assumed. If access still denied remain specific UNKNOWN.
Select minimal evidence-backed route; do not expand into new process framework or repeat generic diagnostic-only cycle. Existing OS lock/ownership race protection remains.
Known legacy uid_expectednull must be handled explicitly without manufacturing original UID.
If only a new read-only probe is possible, it must answer whether the exact operation token is present, not dump unrelated processes; exact recovery authority must be distinguished from diagnostic output.

## Checks / owner recovery
Tests: same-user protected unrelated process present; protected matching/unknown relevance stays blocked absent independent evidence; live token build child refuses; authentic completed build can recover; malformed/reused identity, wrong distro/UID, remaining/escaped child, failed root probe and no elevation availability fail correctly.
Use existing focused ownership/WSL/repair/workspace/environment tests. Native r5 bytes/packages unchanged; no native rebuild.
One owner repair/bootstrap action usable before old checks; no repeated full compile merely to classify blocked process. If new update rebuild unavoidable state it.
Allow only minimum tooling/tests/docs/manifests for tooling. No SF/other mods/profile/loader/WS gameplay edits, no checker bypass or broad same-user exemption.
Update PROJECT_STATE/CODEX_RESULT with concrete change and bootstrap, commit/push and verify. Reply STATUS/CHANGED/RESULT/NEED.
