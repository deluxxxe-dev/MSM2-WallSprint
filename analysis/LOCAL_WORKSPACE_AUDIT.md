# Local workspace migration audit

Branch audited: `codex/v01c20-edi-value-provenance`.

Purpose: capture the repository facts that matter for the final `MSM2-WS` local workspace migration. This is an infrastructure audit, not new gameplay/reverse-engineering evidence.

## Current repository facts

The current checkout contains the core source under `source/`, including:

```text
source/CMakeLists.txt
source/BUILD.md
source/include/...
source/src/...
source/vendor/...
source/tools/...
```

Existing build/test/release helpers include at least:

```text
source/tools/build_windows.sh
source/tools/test.sh
source/tools/smoke_windows.sh
source/tools/test_hooks_windows.sh
source/tools/package.py
source/tools/release.py
```

`source/BUILD.md` documents the current serial validation flow around those scripts. The migration must preserve those tests and adapt/wrap them for the new local Windows workflow rather than silently skipping them.

## Critical conflict with recursive Overstrike scanning

The repository currently tracks a root-level live artifact:

```text
WallSprint.script
```

and many historical ZIP artifacts.

An independent clone placed directly at:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\_source
```

would therefore place a valid `.script` beneath recursively scanned `Mods Library`. That violates the final rule that exactly one active WallSprint `.script` exists and that it lives only at:

```text
MSM2-WS\MSM2-WS-v<version>.script
```

The migration implementation must remove the checkout's dependency on a tracked/generated live `.script` and verify `git ls-files "*.script"` / filesystem scans are compatible with the new invariant. Git history need not be rewritten.

Historical ZIP files do not by themselves count as active `.script` files, but migration/package tooling must never unpack a live `.script` into `_source`, `logs`, or `_archive`.

## Current logging conflict

The current implementation/documentation writes runtime logs under the game-root `log_wallsprint` directory with names like:

```text
WallSprint_YYYYMMDD-HHMMSS.log
```

The final workspace specification supersedes this with:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

The logging implementation and its filesystem smoke tests therefore require a narrow infrastructure update while preserving the existing collision/open-failure/no-silent-fallback behavior.

## Current research state to preserve

C19 actual-game evidence remains accepted. The exact current research blocker is the EDI value provenance at the exact Transform+0x10C write. C20 was prepared to study that value provenance, but the workspace migration is now higher priority.

Keep unchanged during infrastructure migration:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Do not traverse producer `0x351EB00` and do not implement movement, adhesion, camera, animation, or traversal as part of this migration.

## Required migration result

After Codex implementation and later local execution/validation, the authoritative local shape is documented in `docs/LOCAL_WORKSPACE.md`. The repository must contain reusable migration/update helpers and tests for that shape, while actual user-PC migration is performed locally. The old D-drive environment must never be auto-deleted.