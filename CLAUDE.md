@AGENTS.md

# Claude Code: WallSprint (WS) handoff

This file governs WallSprint changes in `deluxxxe-dev/MSM2-WallSprint`. The canonical implementation branch is `codex/v01c20-edi-value-provenance`, **not** `main`. Do not merge `main` or reset a newer branch to a historical snapshot.

## Multi-repository sessions (deluxxxe-dev)
- Claude Code cloud sessions may include `MSM2-WallSprint`, `MSM2-Wisdoms`, `MSM2-SocialFeed`, and other explicitly selected `deluxxxe-dev` repositories. Read across them when relevant, using each repository's current branch and `AGENTS.md` as its own source of truth.
- For WS tasks, consult `MSM2-Wisdoms` for reusable engine findings and shared development procedures. If the task explicitly authorizes preserving newly verified shared knowledge, update Wisdoms on its own appropriate branch, commit/publish there independently, and report its GitHub handoff separately.
- Reading other repositories does **not** authorize editing them. Cross-repository edits require an explicit task scope; do not silently modify SF, CI, shared workspace, or other mods.
- `Update-WS.cmd` fetches/deploys **WallSprint only**; it does not synchronize Wisdoms, SocialFeed, or other repositories. Never claim cross-repo changes deployed via Update-WS.


## Before any work
1. Follow `AGENTS.md` and the documents it requires, especially `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_NEXT.md`, and referenced `analysis/` evidence.
2. Read `docs/CODEX_RESULT.md` and check the actual current branch/head. The repository files outrank any earlier conversation or memorized status.
3. Treat instructions in `AGENTS.md` to the implementation agent (“Codex”) as applying to Claude Code. Never assume a cloud VM can access the user's Windows game or run real-game tests.
4. Keep verified runtime evidence, verified static analysis and hypotheses separate. Do not invent offsets, APIs, game behavior, test passes or a later milestone.

## Attached evidence (owner rule 2026-11-09)
- Inspect EVERY attached file before analysing or answering: every video from start to end (sample frames across the
  whole duration of each video, not only some of them) and every log. Never report or decide from a subset, and say
  explicitly which files were inspected.

## Codex handoff (owner rule 2026-11-11)
- When implementation is handed to Codex, put the complete instructions in the repository (`docs/CODEX_NEXT.md` plus the
  referenced `analysis/` files), commit and push them, and give the owner only a minimal paste message for Codex:
  repository, branch, and "follow docs/CODEX_NEXT.md". Do not repeat task details in the chat message.
- Every finish response still lists the owner's concrete test steps and the log/videos to return in NEED (also when
  the implementation is handed to Codex: state which test is for now and which is after the Codex push).

## Development division
- ChatGPT decides technical next steps and interprets real-game logs. Claude Code implements the *currently authorized* bounded task, performs available non-game checks, and records results.
- `docs/CODEX_NEXT.md` is the task boundary. If it currently describes only a Windows owner action or a pending real-game test, **do not create new implementation work**. Report that the next action is for the user.
- Do not edit SocialFeed, unrelated MODs, shared Overstrike infrastructure, the user's game, or other repositories unless the task explicitly authorizes it.
- Keep explanations short and understandable without assuming reverse-engineering expertise.

## Cloud-to-Windows handoff
The user's normal local workflow is **run the existing `MSM2-WS/Update-WS.cmd`**, which fast-forward pulls the tracked implementation branch and then performs the local build, validation, WS-only deployment and shared Overstrike launch. Do not replace it with ZIP downloads, ad hoc file copies, a new launcher or Remote Control.

The updater cannot see a cloud task's unpublished commits. Therefore:
1. Implement and verify only what the task authorizes. Update `docs/CODEX_RESULT.md`, commit all relevant source/analysis/docs to the task branch.
2. Ensure those commits are published on the **tracked implementation branch** before saying the updater can fetch them. A normal fast-forward push to that branch is acceptable only if permitted and if no remote changes would be overwritten. Never force-push.
3. Cloud sessions may publish to a **new feature branch** instead. In that case create/request a PR targeting `codex/v01c20-edi-value-provenance`, clearly state that merging that PR is still required, and **do not** say `Update-WS.cmd` can fetch the changes yet.
4. After verifying the implementation branch actually contains the intended commit, tell the user to run `Update-WS.cmd`; then give only the necessary in-game verification and log name. No download link unless asked.
5. The local updater refuses a dirty `_source` checkout. Do not instruct the user to erase or reset local changes merely to make a pull work. Ask for a narrow preservation/diagnosis if it reports dirty worktree.

## Finish response
```text
STATUS: success / partial / blocked
CHANGED:
- files changed
RESULT:
- implementation, checks, published branch/commit
NEED:
- only action required from ChatGPT or the user
```
