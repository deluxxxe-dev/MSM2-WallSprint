# Next task — isolate downward WS motion / orientation interaction
Branch codex/v01c20-edi-value-provenance; no main merge.
Read AGENTS docs, analysis/R5_NUMBERED_OWNER_RUNTIME_20261031.md, r5 heading implementation analysis and current source paths. Relevant Wisdoms references only.
Owner r5 actual game run supplied. Stop old updater/WSL diagnostics; do not repeat completed r5 general-plane implementation.

## Objective / decision
Fix descending WS without periodic horizontal flips. First isolate why measured motion becomes horizontal during owner down-input report. Observer cannot face requested down if sampled translation becomes horizontal; blindly forcing down or reducing thresholds is not supported.
Current up/general-plane headings execute. Preserve existing working left/right/up and r5 timing.
Upper diagonal weak lateral movement, direct vertical entry and RT-held stationary WS are explicit later backlog, NOT solved or silently changed here.

## Narrow code/evidence task
Trace ONLY existing authenticated native WallRun motion/steering and A/B original source vs orientation override interaction; use current known captures and review FLAT_WALL_LATERAL_ROUTE_REVIEW if needed. Do not reopen large c-series search or repeat input inventory.
Check whether heading/source basis participates in original movement-coordinate calculation, whether original game down steering/contact behavior naturally produces horizontal oscillation, and which existing evidence can distinguish those hypotheses. Names/static interpretation not runtime causality.
Use supplied concrete session4 delta/target evidence; do not blame low_speed gate alone or claim accepted switches were tiny lateral noise.
If source shows a specific reproducible pure-math/adapter defect, fix ONLY that defect in separate minimal change with targeted replay tests.
If original motion semantics unproven, implement only necessary bounded diagnostics on ALREADY authenticated values at existing hook/caller locations: coherent full tangent displacement, current surface normal, requested/published heading, actual actor basis and native lifecycle/update/monotonic clock, plus original incoming A/B axes only if existing safe reads cover them. Original incoming axes may be prior WS output; do not treat them as independent input intent. Reuse existing logs when sufficient.
Ensure down/diagonal/low_speed rejection samples remain observable beyond first8 hold events: bounded summaries/ring windows with explicit drops, NOT unlimited logging. Preserve no new pointers/hooks/API and no generic telemetry framework.
If exact current-build original continuation is required but absent, report ONE precise offline capture/helper action; do not fabricate function signatures or broad scan.

## Strict change boundaries
No arbitrary Position/velocity/gravity/input/animation writes; no forcing target down from unmeasured stick, screen axes or sign heuristics; no native Deinit suppression.
No threshold/count/interval/duration changes as an unsubstantiated fix. Speed2/33ms/two samples/120ms/12deg/8deg stay unless a specific verified bug mandates change and explained.
No simultaneous descending-speed/animation-playback/diagonal-gain/idle/camera/contact overhaul.
Existing owner/build/lifetime/stale/A-B proper basis/4096/C52 gates remain. No SF/shared tools/profile/DLL mutation.
Allowed minimum source/math fix IF evidenced, or scoped diagnostics; analysis/PROJECT_STATE/CODEX_RESULT and corresponding tests/version/package only if runtime bytes change.

## Completion
One causal explanation limited to established evidence, actual minimal patch or exact remaining missing fact. Synthetic fixture tests must not substitute for native game ownership proof.
Run appropriate existing required checks for actual diff; no full native suite for docs-only. Runtime patch uses standard build/focused/portable/smoke/native/hooks/release validation and exact version/log naming.
Commit/push verify; report STATUS/CHANGED/RESULT/NEED only. Chat supplies numbered owner comparison test after implementation. No request to repeat unchanged test now.
