# Next task — v0.1c20 EDI value provenance runtime capture

## Status
The real-PC local workspace/shared-Overstrike/logging acceptance gate has passed.

Accepted evidence is recorded in:
- `analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md`

The accepted run confirms the final WS workspace/log path, shared ordinary-mod + SF + WS recognition, modded-game launch, visible WS operation, and versioned WS/SF runtime logs. The prior infrastructure-only pause is therefore complete.

This file explicitly reauthorizes the prepared **v0.1c20 EDI value-provenance diagnostic work** on branch `codex/v01c20-edi-value-provenance`.

Do not merge `main` into this branch. This branch is the implementation source of truth.

## Required reading
Before changing code, read:
1. `AGENTS.md`
2. `docs/DEV_RULES.md`
3. `docs/LOCAL_WORKSPACE.md`
4. `docs/PROJECT_STATE.md`
5. `analysis/V01C19_RUNTIME_RESULT.md`
6. `analysis/V01C19_TRANSFORM_WRITE_CONTRACT.md`
7. `analysis/V01C20_EDI_VALUE_PROVENANCE.md`
8. `analysis/V01C20_GATE.md`
9. `analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md`

Preserve the exact-build/fail-closed behavior and all existing lifecycle/orientation/ownership gates.

## Accepted starting point
C19 actual runtime evidence remains authoritative:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX = Hero Transform
destination = Hero Transform +0x10C
```

The destination does not directly overlap Position `+0x30..+0x3F` or the Transform identity/pointer-carrier range.

The unresolved research question is the **value provenance of EDI at the exact target** and, only if that value contract succeeds, the next precise blocker in the writer effect/XYZ/W causal proof.

Current safety state remains:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Producer `0x351EB00` remains deferred.

## Current bounded Codex task
Use the c20 work already preserved in this branch (including the prepared EDI value-contract implementation/tests from commit `9057d24`) and move it from infrastructure-paused state to a **diagnostic-only production build suitable for the actual-game A/E capture**.

### 1. Revalidate the prepared implementation
- Inspect the current branch before editing; do not recreate work that is already present.
- Re-run the owned EDI value-contract tests and the existing c19/native/logger/workspace regression suite relevant to the changed code.
- Confirm the runtime path remains fail-closed on unsupported/partial/conflicting/capped/control effects.
- Preserve the distinction between raw low32 register-input identities and pointer/address provenance.
- Do not infer an ABI type or semantic meaning for EDI, Transform+0x10C, or any entry register without evidence.

### 2. Enable only the prepared c20 diagnostic path for the test package
Enable the already-reviewed EDI value-provenance diagnostics required to emit the c20 runtime contract described by `analysis/V01C20_EDI_VALUE_PROVENANCE.md` and `analysis/V01C20_GATE.md`.

Required runtime reporting includes the bounded value-contract result needed to distinguish:
- exact local producer/copy-chain/origin relation;
- all-local-path coverage versus conflict/refusal;
- first unresolved instruction/blocker when exactness fails;
- later relevant-read/dependency status;
- whole-writer-call/effect coverage kept separate from local EDI-value coverage.

Do not add a new observation mechanism, PAGE_GUARD, data-page mutation, guessed game call, or unrelated hook.

### 3. Keep gameplay writes disabled
This task is diagnostic only.

Do **not** add or enable:
- Position writes;
- source-position writes;
- input writes;
- movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- any claim that the writer source is writable/owned/exclusive.

Literal `transform_no_write_proven` must remain honest for the real Transform+0x10C write. A successful local EDI proof alone must not advance ownership or protected-effect gates.

### 4. Build/test/package
Produce the normal current WS test package through the current workspace/release tooling.

The package must preserve the accepted local structure:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v<version>.script
├─ Update-WS.cmd
├─ logs\
├─ _source\
└─ _archive\
```

Do not reintroduce the old `log_wallsprint` destination or old AppData/project-specific Overstrike behavior. Do not mutate `MSM2-SF` or ordinary mods.

### 5. Prepare the exact human runtime test
After build/test/package succeeds, the next human test is:

```text
A/cardinal surface: ~4 s
fully end WallRun
move or fast-travel
E/diagonal surface: ~4 s
```

The user should then return the newest complete file from:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs\
```

No video is required unless there is a visible regression.

## Acceptance criteria for this Codex task
Codex-side preparation is complete only when:
- prepared c20 EDI diagnostics are production-enabled for the test build;
- all relevant owned tests/regressions pass;
- no gameplay/Position/source/input write capability is introduced;
- the package uses the accepted shared-workspace/update/logging layout;
- `docs/CODEX_RESULT.md` records exact files changed, test results, package name, branch and commit SHA, and the required A/E test;
- the implementation is committed and pushed to `codex/v01c20-edi-value-provenance`.

Do not claim the c20 EDI provenance result from offline tests. Actual producer/origin/dependency/XYZ/W conclusions remain pending the real-game A/E log.

## Next step after the human log
ChatGPT will analyze the returned c20 runtime log against the exact VALUE_CONTRACT and writer-effect coverage rules.

Only that analysis may decide whether to:
- accept an exact EDI relation and continue the writer XYZ/W causal proof; or
- record the narrower precise blocker and design the next bounded diagnostic.

Do not advance to producer ownership or movement rewrite without a later explicit authorization.