# Shared workspace runtime acceptance — 2026-10-24 18:02:29 local

## Scope and input

User-supplied MSM2-WS-v0.1c20-20261024-180229.log and
MSM2-SF-v0.1.3-20261024-180229.log; the user confirms game launch and visible WS
operation. Only derived records are published. Raw logs, full native functions,
HeroSystem byte windows and process addresses remain private.

The prior actual helper runs completed WS139/5 optional private SKIPs and
SF54+40/actual CMD15/workflow87/Core11. Shared screenshots showed ordinary mods,
SF and WS once each and checked. The subsequent game logs prove both installed
modules actually ran in the game, not merely appeared in the manager.

## WS VERIFIED in this capture

- Target2.810.0.0, expected executable SHA256 accepted; four lifecycle hooks and
  authenticated orientation/reset-copy hooks installed.
- New versioned log uses game-root Mods Library/Mods-Developing/MSM2-WS/logs,
  filename clock local and line clock UTC, shared_logs_fallback=0.
- Session1 duration1594ms: A47/B46, verified native Deinit, source active until
  Deinit, retention_pass1, Up dot surface1, stale suspend/resume0, hard kills0.
- Session2 duration2234ms: A79/B80; the same retention/Deinit/Up/stale/kill gates
  pass. These are short sessions; do not relabel them as the historical4-second
  A/E regression or assume named surfaces without markers.
- Both Position-watch summaries report Position_writes_by_mod0,
  cleanup_restored1, cleanup_pending0 and dropped0. Threads are snapshot-only
  coverage (128 seen/126 installed); session2 reaches its256-event cap.
- Session2 finalizes8 true entry snapshots/8 complete invocation maps, XYZ mask7,
  source W0/destination W1; writable_source_proven remains0. Session1 entry
  diagnostic was not ready; no complete capture is inferred for that session.
- One NATIVE_SOURCE_LOG_DROPS count1 record reports writes_unaffected1. It is a
  diagnostic record omission, not evidence of a hard kill or game-write drop.
- EDI tracing remains disabled. Position/source-position/input/movement writes
  are not authorized by these observations; producer0x351EB00 stays deferred.
- Later Hero availability loss resets caches without retaining a stale pointer.
  Neither file has an explicit process-exit/unload record; do not claim that
  their tails prove clean whole-process shutdown. Per-WallRun Deinit is verified.

## SF VERIFIED, separate from player-signal acceptance

Bootstrap enters script_enable, pins module, creates/enters worker and executes
runtime diagnosis. Signatures hero/actor are unique; expected image fingerprint
matches the concurrent WS process. Versioned new SF logs use MSM2-SF/logs.
The log spans09:02:29Z..09:05:39Z.20 candidate summaries include18 ready and2
actor-table-unavailable records. All ready summaries examine32 aligned words
but range_pass0/valid0/statusnone; no unique_stable/player position is established.
Transform gate stays closed. No bootstrap failure or DISABLED record appears.
Current SF is a read-only probe; absence of a visible feed is expected at this
phase and is distinct from the unresolved player-handle lookup.

## Interpretation and remaining scope

Installation/launch, shared ordinary+SF+WS recognition and new versioned logs
are accepted. The WS infrastructure gate is satisfied; no EDI/gameplay research
is reauthorized by this log-review request. Historical c19 long A/E evidence
remains authoritative; current short runs provide an additional smoke check.
SF player/handle evidence remains unresolved and does not accept Phase II.2b.
SF research notes separately record the root-address versus pointer-read
hypothesis; no guessed offset, runtime patch or code change is introduced here.
Old source trees are preserved; this review does not order cleanup/deletion.
