# Next task — repair c67 test path to unblock canonical Update-WS checks

Branch: codex/v01c20-edi-value-provenance. Do not merge main.
Read AGENTS and required docs, current PROJECT_STATE and CODEX_RESULT.
The v0.1c38r1 reversal implementation is already published and functionally checked. Do not redesign or modify it.

## Purpose / exact defect
Canonical test_hooks_windows.sh runs from source/. Existing tools/test_slot_publication_post_c67_boundary_fix.py uses runpy.run_path('source/tools/test_slot_publication_post_c67_boundary.py', ...), resolving to nonexistent source/source/tools.
Root invocation passes but source-directory invocation fails. This prevents canonical updater acceptance.

## Authorized change
Make this wrapper's referenced script path derive from its own resolved __file__ directory rather than cwd. Inspect only the directly dependent c67 wrapper chain for the same exact path defect; fix only independently reproduced instances needed by the canonical command. Preserve runpy globals/run_name and all assertions/test semantics.
No chdir workarounds, test omissions, failure masking, gate weakening, or updater bypass.
Allowed: those test-runner paths, narrow regression verification, CODEX_RESULT/PROJECT_STATE and necessary package documentation refresh.
Forbidden: runtime/reversal design/code/version changes, new gameplay behavior, native watch/VEH/VCH/producer work, c73, shared launcher/loader/config/SF changes, broad searches.

## Completion / verification
- Reproduce original failure and verify corrected wrapper from repo root and source/ (and an unrelated temporary cwd if supported).
- Run the unchanged canonical bash source/tools/test_hooks_windows.sh to completion; distinguish PASS/SKIP/UNAVAILABLE accurately.
- Inspect workspace updater's existing command list read-only; run any remaining required build/test checks not already valid for unchanged runtime bytes. Do not declare a local Update-WS execution that did not happen.
- Verify diff and normal package consistency if docs/artifacts are refreshed; keep v0.1c38r1 / package0.1.57 runtime unchanged.
- Record exact canonical results and remaining owner Windows/game validation in CODEX_RESULT/PROJECT_STATE, commit/push and verify remote.
If another unrelated blocker appears, report its exact failing command and cause; do not expand scope or deploy around it.
No owner game/test prose. ChatGPT will give update and runtime instructions after canonical checks pass.

Reply briefly:
STATUS: success / partial / blocked
CHANGED:
- paths
RESULT:
- canonical checks and published branch/commit; runtime remains v0.1c38r1
NEED:
- only exact remaining blocker
