# Codex reporting requirements

This is a standing requirement for WallSprint Codex tasks.

## Repository record

Detailed validation belongs in `docs/CODEX_RESULT.md` (and task STATUS/analysis files when required), not in the user-facing Codex reply.

The repository record must include enough detail to reproduce and audit the task. At minimum record:

- the test or verification procedure performed;
- the exact command(s) executed;
- the expected result;
- the actual result;
- any test that was not performed and the exact reason it was not performed;
- branch and commit SHA;
- publication/push verification;
- whether owner-side action is required.

Do not report an unexecuted test as PASS. Distinguish owned fixture/static validation from actual game/runtime validation.

When `docs/CODEX_NEXT.md` defines stricter task-specific validation, satisfy it in the repository record.

## User-facing Codex reply — minimize tokens

Keep the final reply to the owner as short as possible. Do not paste the full test table, implementation narrative, hashes, changed-file list, analysis, or evidence already stored in GitHub.

Normally output only:

1. result/status in one line;
2. compact test summary totals plus any BLOCKED/important UNAVAILABLE item in one line;
3. branch + final commit SHA in one line;
4. owner action only when required.

If owner action is required, provide only the exact command/block to run and the exact output file/log to return. Do not repeat explanations already present in repository docs.

If there is an error or BLOCKED result, report only the shortest actionable reason plus the exact recovery/diagnostic command if one is required. Put detailed diagnostics in `docs/CODEX_RESULT.md` or analysis instead of the chat reply.

For PowerShell, shell, game-test, or log-return instructions, omit introductory prose when the command itself is sufficient. Never omit a safety-critical guard or command merely to save tokens.
