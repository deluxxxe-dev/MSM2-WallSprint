# c6 planar adhesion — read-only diagnostic implementation

## Scope and controller

c5's real lifecycle-wide orientation PASS is retained. Safe source Position ownership is not yet independently proven (PATH_A_POSITION_PROOF.md), so c6 performs no Position correction. Both A/B store boundaries and the existing strict Position16 preservation helper remain unchanged. No direct Actor writer, new game call to set Position, page protection, or Path B Position path is added.

Read-only evaluation uses current HeroPosition, owner+0x338 P338 and owner+0x398 N398:

```text
distance = dot(HeroPosition-P338, normalized_N398)
error = 0.5-distance
requested = abs(error)<=0.01 ? 0 : clamp(error*0.25,-0.02,+0.02)
```

Requested is zero while t<1 or abs(error)>0.35. Raw normal must be finite, length.95..1.05, normalized abs(y)<=.20, and remain within the established owned planar normal tolerance (dot>=.995). P338 and HeroPosition must be finite/readable. There is no deadzone/range epsilon that silently widens the requested limits. Tangent motion of the vanilla game remains untouched. The candidate delta is requested*normalized_N; tangent checks use a basis projected onto that same current normal.

These are shadow requests only: correction_applied=0, position_source_write=0. A nonzero request is not an admission or a claimed native Position change. No artificial displacement is injected to force a visible effect.

## Session/identity safety and bounded records

Sampling is on the existing worker only, after verified active c5 ownership. It checks matching publication/session/generation/owner/Hero/Transform/close epoch before owner reads and checks native token owner/generation/thread/freshness and the open epoch afterward. Known Deinit/kill/identity loss prevents further owner-field reads; a close racing a read invalidates that observation. Guarded separate Hero/plane reads are not an atomic physics snapshot. The finite target and two finite plane fields are the only values logged; no pointers inside the object are followed.

At most64 attempts/session, at least100ms between samples, distinct native updates. Invalid/unreadable/changed planes and out-of-range errors add fail_closed_events and explicit reasons, without stopping otherwise valid c5 orientation. The correction remains disabled even on valid data.

ADHESION_SAMPLE reports validity, identity revalidation, Hero/P338/N398, distance/error, requested/applied, reason, candidate tangent delta and read-only mode. ADHESION_SUMMARY emits once at the first source end, with sample/attempt counts, max observed error, actual applied count/step/tangent/write counters all0, maximum shadow request/tangent and end reason. In read-only mode max_abs_error_after equals the SAME unmodified observation; it is explicitly labeled and is not a post-native or improved-error measurement. ORIENTATION_LIFECYCLE_SUMMARY retains c5 fields/semantics unchanged.

## Tests and gate

ASan/UBSan A/E plane evaluation covers signs, gain/deadzone/range/step, t<1, tangent translation invariance away from floating-point threshold ambiguity, candidate tangent projection and nonfinite/nonunit/steep/zero-normal negatives. A boundary-sensitive translation check initially crossed the exact0.35 float cutoff; the test now uses points on both sides away from that discontinuity. Production bounds were not widened.

Actual owned MinHook A/B tests retain full-WallRun orientation/Deinit while sampling valid planes and preserving source Position16. An unreadable P338 and out-of-range error produce refusal while orientation continues. In-range requests cap0.02; Deinit/kill cause no further plane reads. Startup inspection tests are separate from runtime game proof.

Human c6 minimum diagnostic gate: original identity/proof rejection reasons captured, valid actual plane/distance telemetry, Position writes0, c5 retained orientation through verified Deinit. Full adhesion control remains blocked until whole-path/W/exclusive-effect proof and a subsequent authorized implementation. No locomotion/input, animation, camera, gravity, traversal extension or later milestone is implemented.
