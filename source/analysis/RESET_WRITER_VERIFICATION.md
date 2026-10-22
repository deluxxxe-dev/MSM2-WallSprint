# Reset writer verification

**RESET_WRITER_VERIFIED=0. Real v0.1b7 A/E evidence is pending.**

Minimum useful session: all RESET_TRACE_SELF_CHECK fields 1, PREARM_STATUS first_override=1 with trace_already_armed_before_override=1, >=1 actual hardware RESET_TRACE_EVENT, valid unchanged Hero identity, no dropped evidence. Startup admission alone proves no session watch installation.

Primary success: anchored actual surface-like -> WorldUp store, or a current-Hero WorldUp-source common-writer call immediately after the surface-source entry, with thread/target/caller/time/source evidence. A common call alone does not prove its original function completed or wrote Up. Strong verification needs explicitly confirmed distinct Control A and Face E sessions with the same reset writer/caller, valid normal axes, anchored function SHA/store/effective target and source association. Control A's normal must be measured, not hardcoded to +Z. No video/session mapping is invented.

Offline: `/workspace/toolchains/analysis-python/bin/python tools/review_reset_writers.py /path/to/WallSprint.log --a-session <A> --e-session <E> --output /path/to/RESET_REVIEW.json` from source/. The parser preserves legacy b6 review but b7 additionally requires successful self-check and first-override prearm evidence. Unknown writer functions remain candidates; common-writer verification needs the known normalized complete SHA. The owned hook prefix is restored only when the complete original SHA matches; partial/invented bytes never qualify. Synthetic fixtures never set game verification.

No reset patch, caller skip, direct Actor write or next-phase permission follows automatically from any parser result. Review the real private log and recording, including no-crash/Position checks. Package only sanitized findings, never raw user logs or full game functions.
