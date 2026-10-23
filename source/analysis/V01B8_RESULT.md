# v0.1b8 measured result

Source: supplied `V01B8_RUNTIME_RESULT.md` (2026-10-23), not a reconstructed log.
Accepted executable/build and second hook authentication passed on A and E. Path A briefly changed Hero Up. Path B made zero overrides; retained orientation failed (held_updates=0, held_ms=0). Later common calls remained World-Up after the b8 session abort.

The old `source_is_stack_local=0` conflated provenance and matrix validation. Measured A RDX=0x433812D300, RSP=0x433812D2D8, bounds=[0x4338107000,0x4338130000): RDX-RSP=40 (0x28), numerically inside the stack. E reproduced that relation. Readable64 did not validate as an ordinary affine matrix. This does not prove the representation, writable field offsets, or pointer semantics.

b9 therefore removes Path B matrix mutation and session-abort coupling, preserves Path A reference interpolation, and separates stack/location/layout diagnostics. Orientation ownership and adhesion remain unverified/disabled.
