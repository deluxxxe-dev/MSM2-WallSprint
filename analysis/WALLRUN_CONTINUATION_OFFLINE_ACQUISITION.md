# Exact original WallRun continuation acquisition

2026-10-31 JST; branch codex/v01c20-edi-value-provenance, baseline e0b626b.
Task follows R6_OWNER_DOWNWARD_RESULT_20261031.md. R6 is diagnostic-only and owner
descent/down-diagonal failure is accepted; no tuning/new game test/runtime rebuild.

## Authentication / bounded scope

New capture_wallrun_continuation.py reuses slot_publication.parse_pe/file_range,
pinned Capstone5.0.6 decoder/is_boundary ONLY. Never calls analyze_image/scan_code.
EXE identity comes from the existing DEV_RULES/slot_publication exact SHA:
c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9 (2.810.0.0).
Known original Update422-byte SHA and four branch targets justify selecting
0x14248f9; full-file SHA authenticates the new original on-disk read. No live
process, pointer chase, callable API/signature or movement certificate is created.

Only target0x14248f9/cap128 accepted. Bound at executable section virtual end,
covering RUNTIME_FUNCTION end, or next actual runtime entry start (including when
no entry covers target). File-backed extent must cover the chosen range; virtual
zero-fill is refused. Decode linearly only until first CALL/JUMP/RET/INT/IRET or
existing syscall/UD2 boundary (also HLT/transaction boundary), including that
instruction; do not follow it. Invalid/partial decode refuses without expanding.
No covering runtime entry is reported explicitly, never replaced by a fictional
function span. This window does not resolve the separate0x14246d6 fallthrough.

Runtime table validation/caps are inherited (8MiB/600000 rows, ordered/nonoverlap);
read the table once, not once per row. Covering UNWIND_INFO v1 header/code-slot
extent and optional handler/chained RUNTIME_FUNCTION tail are file-backed checked.
Unknown version/flags/conflicts/truncation refuse. Chained entry/handler RVAs are
metadata only and are NOT followed. Raw bounded unwind metadata and SHA retained;
individual unwind operations are not certified/interpreted as movement semantics.

Compact JSON: EXE SHA, section virtual/raw/flags/file offset, exact captured range/
block SHA/max128 bytes hex, bounded disassembly, actual covering runtime/unwind
metadata or explicit absence, stop reason, no-live/no-recursion/no-write claims.
No EXE, complete original function or raw user log is committed/distributed. Keep
owner-generated JSON private; return only this small task-specific file to Chat.

## Windows action / preservation

Capture-WallRun-Continuation.ps1 derives _source from its own managed layout and
game root four ancestors above _source; explicit SourceRoot/GameRoot supported.
Fresh-bootstrap usage supplies both explicitly because script is staged in TEMP.
Verifies canonical origin/current branch, fetches intended branch, stages ONLY the
helper and existing PE module from that one remote commit into an owned temporary
directory, invokes existing default WSL's pinned analysis Python. No pull/reset/
checkout/sparse change/source overwrite, toolkit refresh, runtime build/package,
install, game launch, process inspection/kill or dependency install. Dirty local
files remain intact. Git refs/objects may update; source worktree/runtime do not.

Uses existing game EXE and existing WS logs directory. Output:
<game>/Mods Library/Mods-Developing/MSM2-WS/logs/
MSM2-WS-wallrun-continuation-YYYYMMDD-HHMMSS.json
Collision suffix -01..-99; Python exclusive create also refuses races/existing
files. Refusal produces small REFUSED JSON when output can be created; missing
WSL/decoder/path/output reports stage/error, never falls back to build/repair.
Only unique owned TEMP helper directory is removed in finally. No sibling cleanup.

Prerequisite: previously configured default Ubuntu WSL and
/workspace/toolchains/analysis-python/bin/python3 with Capstone5.0.6. Actual Windows/
WSL transport is not proven by the Linux PowerShell mock. No authenticated EXE or
successor capture found under current /workspace or /tmp; actual acquisition is
UNAVAILABLE pending ONE owner action. No request for whole EXE upload.

Exact one-action fresh-bootstrap block and tests/results are in CODEX_RESULT.
Chat supplies owner instructions; no unchanged runtime comparison requested here.
