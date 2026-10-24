# c21 gate — read-dependency diagnostic only

Accepted input: V01C20_RUNTIME_RESULT.md, exact EDI producer0x2B2356F with
image-global origin root16+0x0C323788; earlier read blocker0x2B23578 before
native write0x2B2358E. The read opcode, address and value-flow result are UNKNOWN
until the c21 code snapshot is classified on the authenticated actual game.

The c21 sidecar inspects only that pinned read/write within the current exact
callee/hash/boundaries, separates forward local CFG normal-return ordering from
conservative possible flow, and reports returned GPR/XMM/flags/caller-stack,
later reload/control/memory escape and the first precise unsupported point.
Unsupported instructions/partial/indexed/conflicting addresses, CALLs, loops and
caps are not guessed. Both branch legs are retained; local ordering is not a
cross-invocation proof. Details/caps are in V01C21_READ_DEPENDENCY.md.

Outcome prepared is C: a narrower exact operand/order/value/control/feedback
boundary. Even if local read data is dead, the sidecar grants no certificate and
does not waive c19's current read-dependency refusal. Full protected effects,
source-no-write and XYZ/W stay separate and fail closed. This avoids treating
simple temporal ordering or taint presence as a causal conclusion.

No new hooks, live data reads, guessed calls/ABI types, PAGE_GUARD or data-page
mutation. Existing native orientation/lifecycle/ABI/hardware accounting/cleanup
are unchanged. writable_source_proven/caller_owned/ownership_exclusive/
source_lifetime_exclusive/downstream_conflicts_proven_absent0;
Position/source-position/input/movement writes0; producer0x351EB00 deferred.

Build/package: v0.1c21/package0.1.39, MSM2-WS-v0.1c21.zip and optional
MSM2-WS-v0.1c21-source.zip, with disabled build intermediates and versioned
MSM2-WS/logs path. Update-WS then shared Overstrike Install mods/Run modded game.
Shortest standard human test: A~4s -> fully end -> move/fast-travel -> E~4s;
send the one complete new versioned WS log. READ_DEPENDENCY/READ_FLOW are the
only new requested keys; existing c20/value/effect/safety summaries remain useful.
Actual c21 A/B/C result requires that log and is not inferred from owned tests.
