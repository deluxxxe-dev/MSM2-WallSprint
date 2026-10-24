# c22 gate — pinned read-only TEST diagnostic

**ACTUAL C22 ACCEPTED:** V01C22_RUNTIME_RESULT.md supersedes the original
preparation/pending notes below: TEST r8b/r8b -> JE has no selected tags; the
next exact barrier is CALL0x2B235D4. No complete writer/ownership proof follows.
C23 preparation: V01C23_READ_CALL_FLOW.md.

Accepted c21 exact same-field pre-write read and possible local control:
V01C21_RUNTIME_RESULT.md. Current exact stop0x2B235BE TEST operands UNKNOWN.

Prepared outcome C: report exact operands, model only verified register-only
TEST forms, and continue conservative local possible-value/flag flow until the
next exact boundary or normal-return completion. Read-only low-byte TEST keeps
whole-parent tags; high-byte/16bit/partial-write/unsupported forms still refuse.
No numerical/cross-invocation/XYZ/W causal conclusion is inferred.

No effect certificate or c19/c20 dependency waiver, no new hooks/live data reads,
producer traversal or guessed ABI. Exact identity/CFG/limits, old pointer/value
models, native lifecycle/orientation/ABI/pairing/cleanup remain. Safety:
writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement
writes0; producer0x351EB00 deferred.

Actual c22 classification PENDING. v0.1c22/package0.1.40, MSM2-WS-v0.1c22.zip
and MSM2-WS-v0.1c22-source.zip, disabled intermediates and exactly one active
root script remain. Full tests and metadata: CODEX_RESULT/V01C22_RELEASE.json.
Human: Update-WS, shared Overstrike Install mods -> Run modded game,
A~4s -> fully end -> move/fast-travel -> E~4s; newest complete versioned WS log.
