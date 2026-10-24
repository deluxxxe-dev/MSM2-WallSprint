# c21 accepted same-field read / local control result

Evidence: owner-supplied private `MSM2-WS-v0.1c21-20261025-013952.log`,
as accepted in PROJECT_STATE/CODEX_NEXT at a180b6b. No raw log or complete
private function body is added. This records the supplied compact findings;
missing counts, timings and instruction operands are not reconstructed.

Scope: Spider-Man2.exe2.810.0.0, accepted exact SHA256
c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9,
current authenticated callee0x2B23560..0x2B235E9/hash
2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d.

## VERIFIED-STATIC under actual-game authenticated identity

Selected read0x2B23578 is MOV, a4-byte memory read from exact writer-entry
Hero Transform+0x10C, baseRCX/no index, carrierEAX. The address agrees for
the analyzed states. It reads the same field as the later native write
0x2B2358E: MOV dword[RCX+0x10C],EDI. Same-field order includes read-before-write.

First possible use0x2B23581; possible local control dependence at0x2B23583;
control_may_depend1 and memory_escape0 in the analyzed prefix. This is bounded
possible dependence, not a downstream numerical/XYZ/W causal certificate.
Memory_escape0 is not a proof about the unanalyzed suffix or other invocations.

Next exact flow stop0x2B235BE, mnemonicTEST, reason
read_flow_partial_or_unmodeled_register. Its exact operands/width are absent
from the supplied compact evidence and remain UNKNOWN until c22 reports them.
Do not assume TEST AL/AL, DIL/DIL or any other register from the error text.

C20 EDI producer0x2B2356F/image-global root16+0x0C323788 remains accepted.
Root16 is the analyzer's dedicated image-global root, not a game type/GPR16.
The shared log phrase about raw entry-register bits does not change that
implementation-defined root classification. Semantic field/value meaning UNKNOWN.

## Accepted safety / remaining proof

The supplied acceptance records two WallRun sessions, authenticated target,
normal verified teardown, existing orientation operation, no added Position
writes and no writable-source authorization. No unprovided duration/count is
claimed here. Full local flow, XYZ/W relevance and cross-invocation feedback
remain UNKNOWN; effect_certificate0 and existing read/effect refusal remain.

writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement
writes0. Producer0x351EB00 and movement/adhesion/camera/animation/physics deferred.

Next bounded implementation: classify only the pinned TEST and flag consumers,
continue supported local flow or report the exact next boundary. See
V01C22_READ_TEST_FLOW.md and docs/CODEX_RESULT.md for prepared c22 work;
owned fixture completion is not an actual c22 result.
