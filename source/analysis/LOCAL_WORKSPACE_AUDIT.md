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

The initial audit counted physical `.script` files only. Real-PC UI acceptance disproved the assumption that historical ZIPs are harmless: Overstrike recursively opens ZIP/RAR/7z and detects contained scripts without extraction. The44 tracked ZIPs include22 install packages with WallSprint scripts; one build-cache install ZIP plus the root active script predicts24 entries, matching the screenshot. Physical-script checks alone are insufficient. The correction uses managed Git sparse checkout for source archives, disabled build ZIP suffixes and an archive-aware strict layout guard; see OVERSTRIKE_ARCHIVE_DISCOVERY.md.

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

## Implementation audit (cloud, real PC pending)

The preserved current chain is MinGW GCC14/Clang19 SEH and hash-pinned full Capstone5.0.6 through build_windows.sh, portable ASan/UBSan test.sh, actual DLL/logger smoke_windows.sh and full native/hooks test_hooks_windows.sh, then package.py/release.py PE/export/import/INI/source/license/hash/CRC checks. Native Windows MSVC/CMake is not an equivalent supported shortcut: CMakeLists explicitly directs to the cross-build. The Windows-local wrapper uses Windows Python3.11+/Git/PowerShell plus configured WSL2 x86-64 Debian/Ubuntu with the same /workspace/toolchains and analysis/Wine environment. Missing prerequisites fail; no build/test step is skipped.

workspace-build.sh copies sources/docs into a temporary WSL build tree outside the scanned authoritative clone, runs the full chain and returns only validated disabled artifacts/manifests/ZIPs to WS _archive/_build. package/release now materialize .script.disabled, virtualize the install ZIP's one versioned .script member, and never unpack a live script into _source. The tracked root WallSprint.script is removed without history rewrite. Existing generated cloud caches were renamed to .script.disabled preserving bytes; historical ZIPs remain. Git ignore/output rules and filesystem tests enforce zero live scripts in a normal checkout.

Migrate-WS/Update-WS use only exact WS-owned paths, independent no-reference/no-local clone, intended branch/upstream and ff-only pull, full required validation before transactional deployment, root-only versioned active script and disabled archives. SF/ordinary/legacy fixtures retain byte hashes and mtimes. Unicode/space layout, worktree/reparse/alternates/dirty/detached/diverged-source refusals, shared Overstrike/proxy checks and rollback are exercised with owned fixtures. Real process-stop/WSL installation/Overstrike UI and real-PC migration remain pending user execution.

Runtime logger appends exact Mods Library/Mods-Developing/MSM2-WS/logs under its executable root, uses authoritative version/local time/atomic numeric suffixes, rejects blocked or reparse directories and never falls back. Actual owned Windows filesystem and DLL integration validate the path, including Japanese/space paths. Telemetry UTC payload/lifecycle/gameplay code remains unchanged except new log opening destination/name.

Prepared EDI implementation9057d24 is preserved and production-disabled. The research blocker remains the accepted c19 EDI provenance at0x2B2358E; no producer/movement work is authorized by migration.
