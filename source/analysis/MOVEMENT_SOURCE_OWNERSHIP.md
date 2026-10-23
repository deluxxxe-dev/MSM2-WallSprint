# c11 movement source ownership — no writable source authenticated

## Evidence and candidates

C10 is accepted for planar orientation through verified Deinit on cardinal/diagonal walls. That does not prove a translation source. The checked-in accepted-build evidence supplies lifecycle identity and surface reference fields, not a complete displacement/velocity/input data-flow proof. No new typed native/game call or input hook is introduced.

| Candidate | Verified identity/provenance | Current translation evidence | Permission / blocker |
| --- | --- | --- | --- |
| Path A caller-local source+0x30..0x3f | Authenticated writer0x2CFC580/caller return0x2CFC8F3 and existing current-stack admission | Candidate source row; c11 adds linked post-instruction observations | NO: caller construction, full CFG/downstream effects, W/exclusive effects missing |
| Actual Position writer/caller from c11 watch | Exact watched target, bounded unwind PC/caller and dynamic hash/derived store | Native write observed if captured; correlation only | NO until actual A/E log and source lineage/ownership proof |
| WallRun Entry0x14235C0 / Update0x1424530 | Existing lifecycle originals/patches/native owner verified | No authenticated upstream tangent-motion vector supplied in current extracted evidence | NO: need writer/caller chain and exact producer/consumer lineage |
| Owner+0x320 / +0x338 | Measured hero-side/plane reference points | Their separation/standoff is approximately0.5*N; not a measured per-time movement vector | NO: do not rename them velocity/displacement/input |
| Position finite differences | Read-only current Hero samples with existing lifecycle/identity admission | delta/dt normal/forward/right and speed for correlation | NO: diagnostic estimate, never an engine velocity source |

MOVEMENT_SOURCE_CANDIDATE emits the known Path A row once at startup and at most one derived candidate per observed unique writer. source_kind is position only for a linked known stack source; other sources remain unknown. writable_source_proven=0 always. The candidate table does not invent velocity/root-motion semantics from register shape, equality or a field name.

## SurfaceFrame sampling

After full interpolation, on distinct native updates and at least100ms between attempts, worker samples are capped40/session. XYZ delta=Pnow-Pprev; components use the existing full published SurfaceFrame normal/forward/right, finite-difference world velocity=delta*1000/dt_ms, speed=length. Finite proper unit frame/finite inputs and0<dt<=500ms are required. W at Transform+0x3c is a separate guarded read with raw-bit output; its meaning stays unknown. The stale transition clears the finite-difference baseline so it never bridges a suspended lease. No callback/world/physics write is involved; observations are separately guarded snapshots, not atomic physics state or post-native intent.

## Next decision

No safe upstream movement vector is authenticated in this cloud task. Exact blocker: actual c11 writer/caller identities and corresponding bounded producer/source lineage have not yet been measured; complete Position W/control-flow/downstream effects also remain unresolved. Human c11 A/E logs locate that slice without direct Actor Position spam. C12 must authenticate an upstream displacement/velocity/direction/root-motion source or explicitly reject it before input is mapped into SurfaceFrame. C11 authorizes no minimal proof write, input rewrite, adhesion, gravity, animation or camera. Body lean/foot floating remain a later visual/contact pass.
