# WallSprint Codex instructions

## Repository and development baseline

- Canonical remote: `https://github.com/deluxxxe-dev/MSM2-WallSprint.git`.
- Current implementation source of truth: `codex/v01c20-edi-value-provenance`.
- The owner confirmed the pushed/local baseline as `5192f05f32313919fdf4049bd734b971655d163f` on 2026-10-24. Continue from this commit and subsequent commits on that branch; do not reset later work to the baseline.
- `main` has diverged substantially. Do not merge `main` into the current development branch. Its independent documentation/analysis changes do not supersede this branch's implementation or task instructions.

Before starting any implementation task, read these files in order:

1. `docs/DEVELOPMENT_MANUAL.md`
2. `docs/DEV_RULES.md`
3. `docs/LOCAL_WORKSPACE.md`
4. `docs/PROJECT_STATE.md`
5. `docs/CODEX_NEXT.md`
6. Relevant files under `analysis/`
7. Relevant shared references in `deluxxxe-dev/MSM2-Wisdoms` when the environment can access that repository.

`docs/DEVELOPMENT_MANUAL.md` is the operational source of truth for branch policy, local workspace invariants, Git/fetch behavior, backup/recovery, failure triage, and GPT/Codex handoff. Read it before changing repository, workspace, or recovery behavior.

`docs/CODEX_NEXT.md` is the current highest-priority task. Do not continue to a later milestone unless it explicitly authorizes that step.

## Working rules

- Preserve exact-build guards and fail-closed behavior.
- Do not invent native function signatures or game ABI types without evidence.
- Prefer runtime evidence, exact RVA/AOB verification, and bounded diagnostics over blind patches.
- Do not add adhesion, movement, animation, camera, parkour, swing, or combat work unless `docs/CODEX_NEXT.md` explicitly requests it.
- Keep reference MODs as research hints only. Do not copy their code, binaries, assets, or proprietary data into production source.
- Keep WallSprint independently implemented and suitable for eventual Nexus Mods distribution.
- Do not add new vendored copies of `SM2ScriptTemplate` files. New helpers should live in WallSprint-owned code such as `GameInterop`, `NativeHooks`, or math utilities.
- Do not package raw user logs, full captured game functions, or unnecessary game binary fragments in release ZIPs.
- Treat each cloud task as isolated. Never assume an uncommitted working tree from a previous task still exists.

## Reusable MSM2 knowledge maintenance

Central shared knowledge repository:

`deluxxxe-dev/MSM2-Wisdoms`

`analysis/ENGINE_INTERNALS_REFERENCE.md` remains the WallSprint-local derived index/seed and project-local proof pointer. The organization-level reusable knowledge is now centralized in Wisdoms, especially:

- `docs/ENGINE_INTERNALS.md`
- `docs/REVERSE_ENGINEERING_GUIDE.md`
- `docs/WALLSPRINT.md`
- `docs/WALLSPRINT_INTERNALS.md`

Whenever a task establishes, narrows, or disproves a reusable engine fact, debugging method, tooling rule, or shared environment fact, preserve the detailed proof in WallSprint and update the relevant Wisdoms file in the same handoff when practical.

Preserve these evidence distinctions:

- `VERIFIED-RUNTIME`
- `VERIFIED-STATIC`
- `OBSERVED-CORRELATION`
- `HYPOTHESIS`
- `UNKNOWN`
- `REJECTED`

Never promote correlation to causality/ownership/writability without the required proof. If a prior shared fact is wrong, correct the canonical Wisdoms entry explicitly rather than leaving contradictory statements as equally current truth.

If Wisdoms is not accessible from the current Codex environment, do not block the bounded project task solely for that reason: record the reusable finding in WallSprint analysis/result docs and report that Wisdoms synchronization remains pending for ChatGPT/the next connected session.

Do not place raw user logs, complete proprietary native function bodies, credentials, personal paths, or unnecessary game binary fragments in the reusable knowledge base.

## Final local workspace invariant

The authoritative Windows development/test layout is `docs/LOCAL_WORKSPACE.md`.

Steady-state WallSprint root:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v<version>.script
├─ Update-WS.cmd
├─ logs\
├─ _source\
└─ _archive\
```

A one-time `Migrate-WS.cmd` may exist during migration.

Important invariants:

- `_source` is the independent authoritative Git clone; no worktree/junction/symlink/reparse-point dependency on the old D-drive checkout.
- Exactly one active WallSprint `.script` may exist recursively beneath `MSM2-WS`, at workspace root only.
- No active `.script` may remain in `_source`, `logs`, or `_archive`.
- Overstrike also recursively scans ZIP/RAR/7z contents. `_source` uses an independent Git clone with the managed sparse checkout excluding archive extensions while retaining all source code and archive blobs/history in Git. Never disable that policy inside Mods Library.
- No visible `.zip`, `.rar` or `.7z` may remain anywhere under `MSM2-WS`. Store build ZIPs only as `.zip.disabled`; the updater may disable only positively identified legacy WS build outputs. Public ZIP downloads stay outside Mods Library.
- Shared Overstrike is only `<game>\Overstrike\Overstrike.exe`, launched with WorkingDirectory=`<game>`.
- Do not create WS-specific/AppData Overstrike copies or a separate WS Mods Library/profile.
- Never mutate `Mods-Developing\MSM2-SF` or ordinary existing mods from WallSprint tooling.
- Explicit user-authorized shared `environment/` Mod-Tools are a separate owner-operated utility: the current request authorizes whole-environment snapshots/restores including SF and ordinary mods. This exception applies only to an explicitly invoked shared backup/restore, never to WS updater cleanup, SF development, or publication of private backups. Install only the toolkit/shortcuts; restore only the documented mod roots with verified staging and preserved previous data.
- Reusable scripts must not hard-code the user's personal C-/D-drive paths and must support spaces/Unicode.
- The old D-drive WallSprint environment must never be automatically deleted.

The real-PC workspace/shared-Overstrike/log acceptance gate has passed. The prepared c20 EDI diagnostic work is reauthorized only to the scope explicitly defined by the current `docs/CODEX_NEXT.md`; gameplay writes and producer traversal remain unauthorized.

## Runtime-log handoff

The user normally sends game runtime logs to ChatGPT, not directly to Codex.

ChatGPT will analyze those logs and write implementation-relevant evidence into repository analysis files. `docs/CODEX_NEXT.md` will reference the latest relevant runtime-result file.

When starting a task:

1. read the runtime-result files referenced by `docs/CODEX_NEXT.md`;
2. treat their measured RVAs, timings, register values, admission failures, A/E comparisons and reproduced behaviors as the runtime evidence supplied for that task;
3. preserve the distinction between measured facts, static-analysis conclusions and hypotheses;
4. do not require the full raw `.log` file unless the task explicitly says a missing detail cannot be resolved from the extracted evidence;
5. if more raw evidence is required, state the exact event, field, timestamp window or log key needed instead of asking for an undirected full re-analysis.

Do not commit full raw runtime logs to GitHub by default. Keep the repository clean for eventual public/Nexus distribution. Compact, task-specific runtime excerpts and derived analysis belong under `analysis/`.

Current local runtime logs, after the workspace migration is implemented, belong under:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs
```

with the versioned timestamp naming documented in `docs/LOGGING_CONVENTION.md`.

## Persistence is mandatory

A task is not complete merely because a ZIP/artifact was generated.

Before reporting completion:

1. ensure all intended source, analysis, test and documentation changes are present in the repository working tree;
2. update `docs/CODEX_RESULT.md`;
3. commit the implementation changes to the task's Git branch;
4. push/publish that branch to GitHub when the environment supports it;
5. if direct push is unavailable, clearly report the exact branch/commit/diff that the user must preserve and do not claim the implementation is safely handed off.

Prefer the existing dedicated task branch over leaving changes only in an ephemeral cloud checkout. Do not force-push or rewrite `main` history.

The next isolated Codex task must be able to reproduce the current implementation by checking out GitHub alone.

## After completing a task

The owner's latest handoff preference supersedes the earlier download-link rule:
after committing/pushing and verifying publication, tell the user to run the
installed `MSM2-WS/Update-WS.cmd`, then give a brief task-specific human test and
the exact log to return when needed. Do not send download, repository or test-page
links unless the user explicitly asks. Keep artifacts, detailed tests, hashes and
results in the repository for future agents. Do not claim publication before a
successful push. Distinguish Update-WS (fetch/build/test/deploy/open manager) from
the desktop Mod-Tools launchers (launch only), and leave Install mods / Run modded
game as the human Overstrike steps. Documentation-only changes do not require a
new game test or runtime rebuild. If a download is explicitly requested later,
verify its actual access/CRC/hash and keep archives outside Mods Library.

Update `docs/CODEX_RESULT.md` with:

- what changed;
- files changed;
- build/test results;
- safety/admission gates;
- what still requires real in-game/Windows verification;
- exact package/output name;
- branch name and commit SHA containing the implementation;
- next recommended human action/test.

Do not overwrite `docs/PROJECT_STATE.md` or `docs/CODEX_NEXT.md` unless the task explicitly asks you to. The current workspace migration task explicitly authorizes the documentation updates listed in `docs/CODEX_NEXT.md`.

Implementation note: current Windows entry points are `Migrate-WS.cmd` / `Update-WS.cmd`; the reviewed engine is `source/tools/workspace.py`. Build intermediates use `.script.disabled` and only the install ZIP virtualizes its versioned active script member. C20 local EDI provenance is accepted. The current c21 diagnostic sidecar classifies only the pinned read/order/possible value flow, cannot issue certificates, and keeps the existing read/effect refusal. C21 actual same-field read/control is accepted. C22 adds only the pinned read-only TEST operand/flag diagnostic, with conservative parent tags for low-byte reads and unchanged partial-write/effect refusal. C22 actual TEST r8b/r8b -> JE is accepted as disjoint from the selected read. C23 classifies only pinned CALL0x2B235D4 and at most one current-authenticated register-only leaf; it cannot issue effect certificates. C23 actual direct target0x2CF7FE0/carrier is accepted; target boundary/current identity remains the blocker. C24 permits only that target to attempt a strict128byte/32instruction no-entry linear leaf proof after PE/runtime/code checks; no certificate relaxation. C24 actual527161-entry table exceeded old262144 cap before lookup. C25 separates structural/cap/read reasons and validates at most600000 entries/8MiB in64-entry chunks before any lookup; the strict c24 rule is unchanged. C25 actual full527161-entry table and genuine target no-entry are accepted; strict fallback stops at first CMP. C26 classifies this exact CMP and conditionally models only full32/64-bit GPR flags-only reg/reg or reg/imm forms, with compact operand/tags/next branch metadata. Partial/memory/unknown forms and every branch still refuse; complete128/32 linear RET and coverage recheck gate target identity. C26 actual CMP r8d,0x3FFF flags-only/tag0 is accepted; next JA0x2CF7FE7 has tag0 and unknown outcome. C27 adds exact edge classification and only current-state tag-disjoint1-JA/2-path proof under128bytes/32 instructions/64 visits; every leg must safely RET and pass final current-code/runtime checks, with conservative path effects and per-state tag recheck. C27 actual short JA edges/fan-out are accepted; fallthrough MOV0x2CF7FE9 memory effect is the blocker. C28 adds only exact MOV operands/access/writes/flags/address provenance and parent-state alias/tag diagnostics. No load semantics or immutable-image rule; write/RMW and unproven loaded values refuse. Actual c28 full64 RAX <- [RCX+0x100] and same image-root16 source+204285312/tag0 are accepted. C29 permits only that load after readable non-writable PE section, current committed same-image readonly page, no checked direct loader overlap, two equal guarded8byte reads and unchanged metadata/page/target code. Only full64 RAX tags can be replaced; no numeric interpretation/raw value/pointer chase/generic memory rule or global lifetime claim. Actual c29 .data/PAGE_READWRITE source is accepted; tag0 refused before values. C30 models only this exact load as unknown may-top7 with an opaque non-pointer origin, no qword read/numeric interpretation. Current instruction/parent/PE/page/code checks remain. Entry-carrier masks are converted to actual category tags before intrinsic top injection; selected dependence remains possible. All c27 all-path/RET/cap/runtime and ownership/gameplay-write gates remain. Actual c30 next barrier VMOVUPS0x2CF7FF0/path0 with RAX7 is accepted. C31 classifies only its exact encoding/width/operands/access/current-parent aliases/upper-lane effects. Encoding-verified stores refuse even if decoder access says READ. No vector load semantics or whole-vector abstraction: XMM-only state lacks unrepresented YMM/ZMM/upper-state proof. Actual c31 VEX2.256 YMM0 <-m256[RDX]/Hero root1+0/tag0 is accepted. C32 adds only low/high128 dependency state for16 parents and models that exact load with both halves top7 and above256 zero knowledge. Legacy XMM preserves high; every other new vector form and ZMM/EVEX use refuses. Current source/state and all-path return/code/runtime/cap gates remain, no data read or new gameplay permission. Actual c32 state/continuation is accepted; next blocker MOVSXD0x2CF7FF4, operands unknown. C33 classifies it and conditionally models only exact current REX.W reg32-to-full64 sign extension at this site: source-parent tags replace destination, unknown non-pointer origin, other state preserved. Memory sources refuse pending exact-source evidence. Actual c33 MOVSXD r8,r8d is accepted; next SHL0x2CF7FF7 operands/count unknown. C34 classifies exact encoding/width/count/access/flags and conditionally models only pinned canonical register32/64 constant counts with width-derived masks and undefined flags aggregate top7. Count0 preserves operand/flags with32bit parent zero-extension. CL/partial/memory/SP refuse. Actual c34 SHL r8,6 is accepted with flags top7; next separate VMOVUPS0x2CF7FFB has unknown form/direction/address. C35 classifies it independently, conditionally modeling only canonical VEX2 full XMM/YMM load from still-proven current Hero RDX+0. Recheck source origin after prior GPR writes. VEX128 high zero vs256 high top7, other parents/flags preserved. Stores/RMW/dynamic/EVEX/masked/wider/other forms refuse. All prior all-path/code/runtime/cap/ownership/write gates and producer deferral remain; actual second form/next barrier pending.

## Shared Mod-Tools manual

For shared launch/backup/recovery changes, read `docs/mod-tools/README.md` and the
relevant `OVERSTRIKE.md`, `PLAY_MODDED.md` or `BACKUP_MODS.md` in that directory.
They map the three shortcuts to entrypoints, settings, functions, safety limits
and regression checks. `docs/mod-tools/MANUAL.md` combines all four for browsing.
Update the individual specification and combined manual together when behavior
changes. Keep runtime evidence distinct from authored specifications; a manual
does not authorize gameplay/SF changes or claim an unperformed real-PC test.
