# V0.1c70 first owner OFFLINE run — BLOCKED

Date: 2026-10-29 JST
Branch: `codex/v01c20-edi-value-provenance`

## Status

The first c70 owner OFFLINE run is **BLOCKED**. No c70 owner JSON is accepted.

The run reached:

```text
c70 full-replay c69 continuation: 26624/162472
POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: multiple distinct first terminal c68 source events
```

This occurred after the focused c70 fixtures had passed.

## Static root cause

**VERIFIED-STATIC:** c70 inherits c69's `View.edge()` observer. That observer was designed for c69's diagnostic-only contract: observe the first terminal c68 POST, retain one `post67_capture`, and then let the root abort. It intentionally rejects a later distinct c68 POST capture as `multiple distinct first terminal c68 source events`.

C70 changes exactly one thing: after the accepted c69 record is reproduced and all of its architectural successors preflight exactly, c70 returns an authenticated `c69_post_boundary` continuation proof instead of letting that first c68 POST abort the root.

Therefore a later row-external edge can legitimately appear in the same root. At that point it is no longer a second candidate for c69's *first* terminal. It is the next post-c69 unknown and must be classified by c70 as:

```text
uncertified_post_c69_runtime_boundary
```

The first c70 owner run failed because the inherited c69 observer remained active after the first accepted c69 terminal had already been authenticated. A later distinct c68 edge reached the c69 observer and was incorrectly compared against the retained first-terminal capture.

This is a c70 observer-lifecycle bug, not evidence that accepted c68/c69 artifacts are inconsistent.

## Required correction

Do not change accepted c56-c69 evidence or c69 record identities.

After c70 has set an exact accepted `c69_record` for the current root:

- callbacks for the same authenticated c69 source instruction/scan context must continue to use the existing exact sibling proof matching;
- any other later edge callback must bypass the inherited c69 first-terminal observer and fail closed directly as `uncertified_post_c69_runtime_boundary`;
- unknown extra siblings at the same authenticated source instruction must still fail as evidence mismatch, not be relabeled as a later boundary;
- transaction-only c69 terminals remain fail-closed;
- no candidate/admission may survive a later post-c69 abort;
- all safety gates remain false.

Add a focused regression with an accepted exact c69 continuation followed by a second row-external boundary in the same root. It must produce c70 POST rather than `multiple distinct first terminal c68 source events`.

## Evidence classification

- Accepted c69 owner result/checkpoint: unchanged **VERIFIED-STATIC**.
- c70 focused fixtures before this owner run: passed, but incomplete for this owner-discovered lifecycle case.
- c70 first owner run: **BLOCKED**.
- c70 owner result: not accepted.
- Runtime/game evidence: none; this remains OFFLINE/STATIC tooling.
