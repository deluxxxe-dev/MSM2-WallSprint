> Accepted c26 actual outcome: CMP r8d,0x3FFF flags-only/tag0 crossed; JA0x2CF7FE7 condition tag0 but numerical outcome unknown is the next barrier. See V01C26_RUNTIME_RESULT.md and successor V01C27_JA_FANOUT.md. Pending-form/linear-only statements below are preparation history.

# C26 diagnostic gate

Accepted c25: full527161-entry/6325932-byte table valid, actual lookup queried,
0x2CF7FE0 truly no-entry; stable snapshot blocked at exact first CMP.
Prepared c26: strict decoded full-GPR flags-only CMP with untainted immediates,
compact operands/tags and next branch condition/flag dependency. Partial/memory/
unknown forms refuse; all branches refuse. Complete stable linear RET under128/32
and coverage recheck required before target hash/c23 effects. No scope widening.

Outcome C: actual CMP operands/width/next boundary remain PENDING new metadata.
No actual leaf/effect/XYZ/W/ownership/writability claim. All gameplay/source-position/
Position/input/movement writes0; producer0x351EB00 deferred.

Detailed proof/tests: V01C26_CMP_FLAGS.md. Main build/full pipeline and separately
extracted source reproduction results are recorded in docs/CODEX_RESULT.md and
analysis/V01C26_RELEASE.json. Owned synthetic PASS is never actual game evidence.
Human: close game/Overstrike; Update-WS.cmd; Install mods -> Run modded game;
A ~3s -> fully end WallRun -> move/fast-travel -> E ~3s, each below5s;
newest MSM2-WS/logs/MSM2-WS-v0.1c26-YYYYMMDD-HHMMSS.log. New metadata only,
no private target body/memory values or routine raw-data capture extension.
