# v0.1b6 gate

| Gate | Status |
|---|---|
| Exact-build existing lifecycle and source route | Existing guards retained; source reaches Actor confirmed by v0.1b5 A/E log |
| Stable retained Actor surface Up | Not established; b5 returns toWorldUp |
| b6 reset writer/caller/thread/QPC/source | PENDING actual game test |
| RESET_WRITER_VERIFIED | **0** |
| Reset writer patch/skip/global override | Prohibited, absent |
| Direct Actor/Position/adhesion/movement/animation/camera/physics writes | Disabled, absent from new code |
| New game-call ABI / new native hook target | None |
| Next phase | Not authorized by synthetic tests or this package |

Completion requires at least E's actual surface-like→WorldUp write resolved to a native instruction, target, caller, thread, latency and readable correlated source. RESET_WRITER_VERIFIED=1 requires the same writer/caller repeated in explicitly confirmed A and E sessions, all offline gates passing. Before real logs this remains a diagnostic test build.

Cloud validation: portable ASan/UBSan and Windows warnings-as-errors build; existing ABI/MinHook/source lifecycle/stack/position/negative tests; b6 real hardware watch on owned memory, real MinHook raw-entry snapshots, independent second writer, asynchronous helper, timeout/cancel/F8/DR cleanup; offline anchored decode and negative gates. These exercise the implementation, **not Spider-Man2.exe**. Exact results are in release logs. Public source tests also run without private captured game functions (optional private replay tests explicitly skip).
