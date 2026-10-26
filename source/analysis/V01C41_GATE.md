# c41 gate — owned Wine witness, no production authorization

The15byte owned MOV [RIP+marker],imm32 / JMP rel32 stub establishes a bounded
post-OS-return context witness in owned Wine tests: per-thread/lease/generation/
context tokens, held close/deadline/kill, nested/multiple debt, no second watched
hit, exact measured register/vector/active-frame/DR restoration and lifetime
through thread exit. Context cap poisons retirement instead of falsely completing.
Production engine and c39/c40 counterexamples remain unchanged. No general
retirement fix, c39 slot mode, native-Windows/mitigation-enabled compatibility
or full-XSTATE runtime preservation is claimed. Overall handoff classification:
PARTIAL_SUCCESS (owned Wine result established; native Windows pending).
Runtime v0.1c38/package0.1.56;
no game release. Native question is restricted to the standalone owned JSON
probe, not a game test. See V01C41_CONTINUATION_WITNESS.md / V01C41_WITNESS_STATUS.json,
native-probes/c41 and docs/CODEX_RESULT.md. All prior proof/write gates remain.
