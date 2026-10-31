# Next task — prepare ON/OFF orientation-source comparison for down steering
Branch codex/v01c20-edi-value-provenance; no main merge.
Read AGENTS required docs, analysis/WALLRUN_SUCCESSOR_OWNER_RESULT_20261031.md, WALLRUN_SHARED_SUCCESSOR_REVIEW.md and r6 downward evidence.
20byte successor capture accepted; stop scalar route here. No more offline branch/callee capture tasks.

## Purpose
Distinguish original native down steering from interference caused by WS orientation override. No gameplay fix yet.
Existing source/mod/WallSprint.ini has WALLSPRINT_NATIVE_SOURCE_OVERRIDE=1 and F8-latches-OFF-until-restart comment. Verify actual lifecycle/semantics, do not assume config modification path.
Choose existing explicit source override ON/OFF control while keeping same WS script/read-only lifecycle diagnostics. Prefer restart/config boundary when F8 emergency latch alters other observations; no new toggle framework.
R6 motion windows currently only active-source; OFF comparison needs validated equivalent observations before asking owner for another run.

## Minimum implementation
Reuse existing authenticated worker snapshot position/full Actor axes/surface normal and verified WallRun lifecycle for bounded comparable ON/OFF windows even when publication disabled. Keep sampling eligibility independent of ability to write; OFF must never publish/commit orientation, and no new pointer reads/hooks.
Label treatment mode, actual admitted/override counts, session/gen/native update/monotonic time, world tangent delta/normal/Actor forward, reason for missing sample. Existing requested_heading means observer target, NOT input intent; OFF may have no valid target and must mark unavailable rather than invent one.
Do not fabricate WS orientation basis in OFF or silently use Actor Up=wall normal (vanilla actor up may differ). Use validated existing surface normal. Capture windows use same time/caps in both modes; source-off cannot lose all evidence due early source permission return.
If equivalent observation cannot be made using already authenticated paths, report exact blocker instead of adding unsafe reads or requesting inadequate comparison.
Record compact test specification for Chat: same flat wall/character/camera starting orientation, identical down/lower-diagonal holds, ON vs OFF in separate fresh attempts, no angle/speed change. Owner input report is not raw mapped intent proof.
Expose one documented safe method to select mode and restore original config via existing tooling; no hand-edit shared scripts.txt/archive or broad mod changes.

## Limits / completion
Diagnostics only, not downward patch. Preserve r6 heading math/all constants/normal gate/native Deinit/stale/A-B ownership/4096/C52. No motion/input/Position/velocity/gravity/animation writes or new input API, no SF/shared loader/profile/environment edits.
Allowed minimum native diagnostic sampling/fixture tests/version/package if changed, docs/state/result. Reuse code rather than parallel observer system.
Test OFF sampled but0publications/commits, ON unchanged gameplay, invalid/stale/identity/session clears, equal sampling/caps and disable/emergency behavior.
Use existing required tests for actual runtime diff; docs-only no rebuild. Commit/push verify. Reply STATUS/CHANGED/RESULT/NEED; explicitly state diagnostic-only version, exact selection mechanism and log name. Chat gives concise owner comparison after completion.
