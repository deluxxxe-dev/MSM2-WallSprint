# Codex result — WallSprint v0.1c12

Branch: **codex/v01c12-movement-source-lineage**.
Implementation commit: **4c84d634df4d629b237d4cc311d5e5cd6de4398c**.
Exact c11 delivery base: **9593f0230e220770f2b34faf512ca4108c1825cc** (implementation2129e69). Pre-created c12 task handoff: **fe557ec9b615f1a2d90af7814a8a3fe0e1317560**. PROJECT_STATE/CODEX_NEXT remain byte-identical to that handoff. No main rewrite or force push.

## Outcome

**v0.1c12 / package0.1.30: read-only movement writer/caller/source-lineage diagnostics. Cloud diagnostic implementation PASS; full actual-game c12 PASS remains PENDING.**

Accepted compact c11 real evidence: A/cardinal4125ms and E/diagonal4063ms retained Surface Up through verified Deinit, no hard kill/Position writes. Writer0x2CFC900 stores at0x2CFC936/93c/942/945 and observed return0x351EEB0/caller2≈0x351A020 correlate with translation. This does not supply a full function body or source ownership contract. Raw human logs/private game bodies are not committed.

C12 records the exact runtime writer boundary/hash/full decode/store expressions, anchored direct E8 callsite and caller/caller2 attribution, a conservative caller-side source slice, and bounded reconstruction/correlation from existing hardware-exception registers. It labels missing edges instead of guessing. **No true function-entry snapshot, typed upstream movement vector, W meaning or exclusive intended translation effect is authenticated in this cloud delivery. writable_source_proven=0; Position writes by mod=0.**

## Implementation and files

- `source/include/NativeHooks/movement_lineage.hpp` / `source/src/NativeHooks/movement_lineage.cpp`: independent bounded Capstone analyzer. Full contiguous decode/no skipdata; supported pointer/register/stack aliases, saved slots/overlap widths, scalar/packed moves and add/sub/multiply feed an expression DAG for all four Position components. Scalar SSE/VEX upper-lane rules and truncation ambiguities are handled conservatively. Calls/branches/unknown effects/partial or duplicate stores/decode or capacity failure deny complete proof. A separately labeled supported prefix may map all stores before a later unresolved effect; it never certifies the remainder or exclusive semantics. Limits:256 nodes,64 stack slots,8 store slots (valid maps exactly4), bounded depth/visits.
- `source/src/msm2/orientation_trace.cpp`: inside the existing exact-build configuration, RtlLookupFunctionEntry resolves writer0x2cfc900 and caller return-1 boundaries. Writer begin must equal0x2cfc900; writer bytes<=1024, caller/caller2<=8192. Oversize boundaries/reasons remain visible. MOVEMENT_WRITER_PROOF/SOURCE_COMPONENT and up to16 UNSUPPORTED offsets report derived facts only, never raw bodies/disassembly. Complete decoded E8 ending exactly0x351eeb0 must target0x2cfc900. Caller2 ending0x351a020 is independently attributed to the derived caller begin; observed caller2 and static direct verification are distinct facts.
- Caller substitution walks the verified caller to the exact direct call, marks unsupported/CFG barriers and describes payload roots/arithmetic shape. `construction_kind=unknown`, typed uses_* unknown, producer_rva0 persist without independent producer semantics. An add/multiply alone cannot establish current Position, delta, velocity/dt, speed/direction or root motion. No speculative input observation is enabled.
- The existing c11 Position exception path adds at most16 eligible reconstructions/window for the exact verified callsite, observed second return and watched target. It rechecks the entire cached writer and exact direct call bytes; proven caller2 direct bytes are rechecked too. Expressions may use only surviving register aliases and guarded trap-time reads,64 expression visits/attempt. New reads refuse uncommitted/guard/noaccess/cross-region pages. No allocator/decoder/hash/logger/SDK/game call or lock wait is added to the exception path. LastError preservation/debug cleanup remain.
- MOVEMENT_WRITER_SOURCE_MAP explicitly reports `entry_snapshot_captured=0`, `source_identity_stable=0`, recovered alias status, mask/readability/current-code/target/equality. MOVEMENT_SOURCE_CORRELATION reports payload/predicted delta, observed snapshots/error and SurfaceFrame normal/forward/right, with scalar component/final-store labels. Before is a previous serialized watch snapshot, after is post-instruction, not atomic pre-state or function return. Sources may have changed before a pointed read; equality is correlation, never ownership.
- Existing c11 Position raw256/identity16/printed4/thread snapshot128/no rescan/600ms/eight-window budgets and sole cleanup coordinator remain. New LINEAGE_SUMMARY discloses readiness/attempt cap/proof failures. Path A Position stays rejected for modification, with the existing post-row/downstream comparison retained. Finite-difference estimates do not become engine velocity.
- New independent `movement_lineage_smoke.cpp` and `tools/test_movement_lineage.sh`; existing real Position fixture adds four actual owned MOVSS hardware exceptions/source reconstruction, guard-page refusal and added-thread debug restore. Build/native recipes link the new owned analyzer; version/mod/package/release metadata, root/source README_TEST/README/BUILD/WORKLOG and analyses updated.
- Required analyses: `V01C11_RUNTIME_RESULT.md`, `MOVEMENT_WRITER_2CFC900.md`, `MOVEMENT_CALLER_351EEB0.md`, `MOVEMENT_SOURCE_LINEAGE.md`, `V01C12_GATE.md` (root/source mirrors). Historical c11 analyses now point to the subsequent accepted real result.

## Validation

```text
Windows x64 warnings-as-errors build: PASS
Portable ASan/UBSan:                 13 PASS
DLL smoke:                           4 PASS
Full hooks/native/offline:          53 PASS / 4 historical private-input SKIP
Total unique suite records:         70 PASS / 4 SKIP
```

Full hooks passed after final analyzer capacity/upper-lane/typed-ambiguity refinements. Accepted original B writer444/caller128 proof and production installer/mutation replay ran privately without executing game code or distributing its bytes. The four historical optional private skips are not reported as passes. No private 0x2CFC900/0x351EEB0 body was available or required for synthetic tests.

New tests cover exact return/direct target/embedded-E8/truncated decode; complete scalar/pointer/register/stack/prologue maps; supported computed/target+delta/displacement/velocity-times-dt-shaped untyped arithmetic; caller stack substitution; partial/multiple/branch/call/effect failures; W/downstream overwrite/invalid target/source/context and metadata preservation; VEX scalar upper-lane semantics and bounded DAG exhaustion; A/E SurfaceFrame correlation. Real owned scalar MOVSS exceptions verify actual captured GPR/XMM payload reconstruction, partial scalar progress/full-row match, new guard reads preserve PAGE_GUARD, owned modules cannot impersonate the game, and exact final debug state including the added thread. Existing c7/c8/c9/c10 production/MinHook/ABI/lifecycle and c11 watch/identity/stale/Deinit regressions pass.

Final production-orientation-with-real-Position-watch fixture: **session1: 4111ms A178/B178, held2146ms/133updates; session2: 4153ms A165/B165, held1734ms/104updates**. Both source_active_until_deinit1, verified_deinit1, retention1, hard kill0. Each watch restores cleanly with pending0 and Position writes0. These are owned fixtures, not actual c12 game source-lineage evidence.

Fresh extracted standalone source ZIP rebuilt the Windows DLL successfully with private game captures absent from the extracted source. Final CRC/current-source/doc/license/nested script/hash/private-body-exclusion validation: PASS. The final added-thread hardware debug-restoration confirmation also passed (2 existing suite records, not added to the unique total). Final completion record: `analysis/V01C12_RELEASE.json` (ZIP hashes/sizes, compact synthetic evidence, implementation SHA and proof limits). Only selected cloud stdout validation logs are packaged; raw user and raw synthetic event logs are excluded.

## Safety and exact blocker

Byte-identical to c11: native_source_override.cpp (all A/B hot admission, token/context, c7 recovery/c8 first cause/c9 secondary/c10 refresh), reset_copy_source.cpp, path_a_position_proof.cpp, source_lifecycle.cpp, lifecycle/raw thunks, original source helpers/math/observation/hook guards. New diagnostic failure cannot grant write permission or kill otherwise valid orientation. Exact exe2.810.0.0/SHA gate, freshness100ms, fixed native ownership, A48/B36 byte source limits and metadata/Position preservation remain.

No direct Actor/Transform or Path A Position store, writer/caller source write, input rewrite/hook, custom movement/velocity/gravity/adhesion, PAGE_GUARD/protection mutation, new game hook/ABI/call/executable breakpoint, animation/camera/traversal feature, reference mod code/assets or private full-body distribution. Deferred root lean/foot floating is unchanged. Existing incomplete watch coverage/short-window/c10 admitted-store-close overlap/single-refresh limits remain documented.

**Exact remaining proof:** the game must supply c12 boundary/decode/source-component/direct-call/caller-slice records for the measured chain; unsupported instruction/CFG/producer edges may remain. Existing post-store sampling cannot recover an overwritten entry value or prove a true entry snapshot. Caller typed Position/delta/dt/direction/speed/root-motion semantics, W behavior, source lifetime and downstream/exclusive effects are not established. No safe future write contract can yet be admitted. This is the diagnostic-PASS path explicitly allowed by CODEX_NEXT, not a claim of full runtime lineage/locomotion ownership.

## Outputs and human test

- `WallSprint_v0.1c12_movement_source_lineage.zip`
- `WallSprint_v0.1c12_source.zip`
- root `WallSprint.script` / `BUILD_MANIFEST.json`
- `analysis/V01C12_RELEASE.json`

Install using GitHub README_TEST: continuous~4s A/cardinal, then~4s E/diagonal, ordinary input. Report posture/orientation hold, hitch/freeze/crash and complete private startup-through-cleanup log. No visible movement change expected. Require old verified Deinit/until1/retention1/no hard cause/Position0/cleanup pending0, plus startup MOVEMENT_WRITER_PROOF/SOURCE_COMPONENT/UNSUPPORTED/CALLSITE_PROOF/CALLER_SOURCE_PROOF/COMPONENT and per-session SOURCE_MAP/CORRELATION/LINEAGE_SUMMARY. If proof/arming/reconstruction fails, preserve its exact reason; do not loosen a gate or add guessed input. No raw game body/log/video needs public GitHub publication.

## Persistence

Implementation committed at the exact SHA above. Result/ZIPs/script/manifest/release JSON are a separate delivery commit on the same authorized branch. Push and authenticated Git fetch/show validation complete the handoff; no main rewrite/force push. Delivery message uses fixed GitHub raw ZIP download and GitHub blob test/result links. Private-repo anonymous/browser access is not claimed.
