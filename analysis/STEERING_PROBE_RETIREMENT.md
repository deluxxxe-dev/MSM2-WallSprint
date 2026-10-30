# Steering Probe retirement — 2026-10-31 JST

Task baseline9fd7c4a, branch codex/v01c20-edi-value-provenance. Tooling only;
v0.1c38r2/package0.1.58 runtime script and DLL byte-unchanged.

## Cause and evidence limits

Reviewed public Overstrike source at443ec468b5a7a922771dce6bfa96396f56896dcb:
Detectors/ModsDetection.cs searches each registered extension recursively; its
archive detector recursively dispatches matching members. StageModDetector.cs
registers stage, ArchiveDetector registers zip/rar/7z. Thus a loose stage and its
ZIP member can produce TWO entries without unpacking. `.zip.disabled` does not
match archive extensions, but an adjacent active stage still does. Cached results
are only consulted for discovered files, not a reason to delete library cache.
MainWindow.MakeModsItems retains prior path selections, while newly found paths
inherit ModEntry.Install=true. This explains a possible checked duplicate without
WS modifying profile choices. No WS profile toggle or probe build/copy operation
found in normal workspace-build/update. Owner's exact two paths are UNKNOWN;
we do not claim which copies were installed or that a profile flag changed.

Existing sparse rule excluded archives but not loose stages. No tracked stage
currently exists at baseline. Ignored/untracked stages can remain under _source
or _archive, or a manual ZIP/stage elsewhere can remain discoverable. Those are
supported causes, not authenticated owner-path identification.

## Bounded resolution

workspace.py upgrades only recognized old managed sparse policy to also exclude
stages (clean tracked bytes stay in Git). Exact named Fast10/Slow5 stages and
single-stage wrappers inside WS may be retired after full member/metadata and
reverse-one-byte original544byte SHA verification. Slow5 original byte is75,
Fast10 original100. Name-only/malformed/foreign data does not authorize movement.
Untracked known probes may be moved only after tracked-clean Git/branch checks,
whole-workspace no-reparse validation and process stop/update lock. Manifest,
original relative paths/hash and collision-free backup under game-root
MSM2-WS-Retired-Probes/retired-* make retirement reversible outside scanned roots.
Unknown stages/archives refuse. Non-WS paths/profile/order/SF/ordinary MODs untouched.

Source package copies omit stage/archive inputs, stale scanner-visible output
refuses, external build checks stages too, bootstrap includes retired_probes.py.
Builders/research preserved; no asset field/runtime/loader/vendor change.

## Validation

`python3 source/tools/test-workspace.py`:18 PASS groups (old13 plus5 retirement
regressions): repeated update, duplicate stage/ZIP retirement/manifest byte
roundtrip, collision backups, sibling/profile isolation, forged/unknown refusal,
dirty/reparse refusal, old sparse upgrade with tracked history, injected build
stage refusal. Synthetic asset expected hash is substituted only in the owned
fixture module; production pin unchanged. Initial added fixture created a sibling
inside an already snapshotted ordinary directory and selected wrong tuple index;
corrected fixture setup (separate manual-copy root/upstream index2), no safety
assertions removed. Final full18 groups exit0.
`python3 environment/test_mod_environment.py`:28 tests OK, exit0.
`python3 environment/test_start_overstrike_preflight.py`:PASS exit0.
`python source/tools/release.py`:exit0, CRC/pass and r2 runtime-byte equality.
`python3 /tmp/ws-retire-dist-check.py`:PASS, stale output synthetic stage refuses;
research-stage injection excluded from both release ZIPs, then owned files removed.
This script creates exclusive mkstemp .stage sentinels in package output/tools,
executes package.py expecting contamination assertion, removes output sentinel,
executes release.py and checks CRC/no .stage/.zip/.rar/.7z members, then removes
tools sentinel. No real game asset/PC process is involved.

SKIP0/BLOCKED0 in this tooling run; actual owner Windows UI/duplicate paths
UNAVAILABLE (cloud-only). Native suites not rerun: explicitly tooling-only task,
unchanged runtime hash verified. Exact publication record in CODEX_RESULT;
manual guard refresh/restore/external-copy handling in LOCAL_WORKSPACE and
mod-tools OVERSTRIKE/MANUAL. No owner automatic install/reinstall.

Publication verified before any r3 runtime edit: implementation faace6f,
normal same-branch merge preserving concurrent planning docs, final retirement
head b0683a5af79a522d596937c842321ea420f1b8ac. Normal push succeeded;
`python3 /tmp/ws-r2-publication.py` independently fetched GitHub into existing
/tmp/ws-steering-docs-publication and checked FETCH_HEAD/ls-remote/local head
and all27 changed blobs against3ba11ef baseline (equal).
