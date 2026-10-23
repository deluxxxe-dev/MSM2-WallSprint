# Codex result — WallSprint v0.1c13

Branch: **codex/v01c13-writer-entry-snapshot**.
Implementation commit: **eb257e340be347d6ac17358efeb61eaa6a83066e**.
Exact c12 delivery base: **c966d233787504dda88f65d1825c5c1c408a2d67** (implementation4c84d63). Pre-created c13 handoff: **6e475d10ef2c1c14a1e878e7e2bae689f0fe9dc9**. CODEX_NEXT is preserved byte-for-byte; PROJECT_STATE was explicitly updated as required. No main rewrite or force push.

## Outcome / evidence boundary

**v0.1c13 / package0.1.31 implements true writer-entry snapshots and invocation-linked Position diagnostics. Final cloud diagnostic implementation/tests PASS. Actual c13 A/E entry/source/hitch/cleanup verification remains PENDING.**

Accepted compact human c12 facts: A4266ms/E4297ms held Surface Up through verified Deinit, hard0/Position writes0; E's172ms native Update gap resumed after72ms. Writer0x2CFC900..0x2CFC9B7 (183 bytes), stores0x2CFC936/93c/942/945, caller0x351EEA7..0x351EEC5 and exact E8 at0x351EEAB returning0x351EEB0 are real. Observed caller2≈0x351A020 failed a direct static relation. First unsupported+0xA/source-prefix0/true-entry0 was the missing observation edge; no writable source was admitted.

C13 does not relax that c12 static proof. It independently observes the actual function entry through a narrow raw diagnostic detour and associates original hardware write events with that exact invocation. Owned fixtures prove true pre-body capture and exact written bits; they do not prove the actual game's upstream semantics. **Position/source/input/movement writes remain0; writable_source_proven=0.**

## Implementation and files

- New `source/include/NativeHooks/writer_entry_snapshot.hpp`, `source/src/NativeHooks/writer_entry_snapshot.cpp`, `writer_entry_thunk.S`. Startup independently validates exact accepted exe/build input,183-byte writer and30-byte caller runtime boundaries, full c12 startup snapshot/digest equality, complete decode prerequisites and the exact E8 relation. Expected hash scope is the accepted-build current c12 startup baseline; the compact handoff did not supply a separately pinned historical writer digest. A baseline alone never establishes exclusive semantics or source-write permission.
- No startup patch. The existing helper installs the one exact writer hook only for an active fresh verified Position request after prior Up leases restore. Native source diagnosticIdentity and current existing SDK sample check session/generation/owner/Hero/Transform/native thread/init/permission/accepted epoch/fresh100ms/code guards. Full writer/caller integrity is rechecked. Capture remains disabled during process-thread watch setup and activates only after the Position watch is fully published, preventing setup from exhausting all snapshot slots.
- Raw entry thunk preserves all15 GPRs/RFLAGS, aligned FXSAVE64 x87/MXCSR/XMM and YMM0..15, original RSP/return/stack arguments and LastError; it restores state and tail-jumps to the retained original trampoline. No typed game signature, argument/source mutation or return interception. The actual original writer produces its own unchanged return value.
- Capture:8 eligible invocations/session, one scope per session,8 windows/process (64 immutable records),8 TLS nesting frames,16 raw GPRs/16 XMM groups,64 stack bytes at entry RSP+0x28 and three16-byte argument-pointer reads at RDX/R8/R9. Guard/noaccess/cross-region/unmapped/overflow/TEB checks reject reads without changing protection; no broad pointer chase or full body dump. Callback uses bounded owned record publication/coherent guards, no logger/decoder/hash or worker-lock wait. Generic target/wrong caller/cap-rejected entries invalidate recycled RSP tokens before returning.
- Explicit TLS record ID + original entry return-slot RSP pairs same session/generation/TID/Hero Transform/return to c11 hardware exceptions. The existing unwind records that return slot. Nested outer invocations remain distinct; wrong/lost/overflow/stale tokens fail closed. POSITION_WRITE_EVENT includes invocation_id. Every eligible event reaches c13 aggregation before the unchanged c11 identity/print cap; raw256/identity16/printed4/thread snapshot128/no rescan/600ms/eight Position windows and exact debug-slot restoration remain.
- Worker fully decodes saved original code and anchors MOV/MOVSS/VMOVSS ending at captured RIP. Exact Hero+0x30/34/38/3c store values come from captured unchanged source GPR/XMM or immediate bits; they are not inferred from a potentially overwritten target-memory read. Before/after target rows retain readability/validity and non-atomic observation labels. No return boundary is claimed. Duplicate/partial/unknown stores cannot yield a full invocation map. Incomplete record drain waits until queued/inflight hardware events settle.
- `MOVEMENT_WRITER_ENTRY_SNAPSHOT`, raw GPR records, `MOVEMENT_WRITER_INVOCATION_MAP`, fixed27 `MOVEMENT_ENTRY_SOURCE_CANDIDATE` origins and `MOVEMENT_CALLER_CHAIN_RUNTIME` provide bounded evidence. Component masks and a16-bit entry-lane→written-component equality matrix expose scalar/swizzled correlations without causal claims. Current/final Position and finite nonzero delta_like are separate labels, never velocity/displacement/root motion. Repeated origin matches are counted separately; stable_across_invocations/surfaces/source_stable/typed semantics remain unproven, write permission0.
- Caller2 is independently classified by complete bounded startup decode: exact E8 target=direct; register/memory CALL=indirect; an exact guarded E9 tail jump from the called target to the containing caller=wrapper; otherwise unresolved. Runtime unwind verification and cached static relation are separate; the cache applies only when the observed second return is the same static site. C12's direct_target_mismatch is not guessed into a wrapper.
- Deinit/F8/kill synchronously disables capture. Helper removes the owned writer entry only after hardware leases restore. DLL pin/trampoline/records remain for inflight tails; no MH_ALL_HOOKS or unsafe trampoline removal. Writer ownership is distinct from caller integrity, so caller changes cannot prevent removing our owned patch. A guarded exact own MinHook entry-branch proof supports cleanup after a full post-install read fails. Foreign patches remain untouched; cleanup failure prevents future capture.
- C12 original bytes are used for its inspector only while the live full writer equals our authenticated owned entry patch; foreign patches cannot masquerade as original. The native A/B admission, lifecycle/token/context, c7 recovery/c8 first cause/c9 secondary/c10 refresh and static source analyzers remain byte-identical to c12. Original Up hardware policy stays in its existing branches.
- New owned fixtures `writer_entry_snapshot_smoke.cpp`, `writer_entry_owned_test.S`, `tools/test_writer_entry.sh`; build/native recipes link new owned module/thunk. Existing isolated watch fixtures explicitly refuse the game gate. The existing orientation-integration fixture now persists diagnostic output before a failed assertion, without changing an assertion or policy.
- Root/source required analyses, README_TEST/README/BUILD/WORKLOG, mod/version/package/release metadata updated. PROJECT_STATE records implementation readiness and actual c13 evidence pending; CODEX_NEXT unchanged. No vendor/reference MOD copy or extra dependency.

## Validation

```text
Windows x64 warnings-as-errors build: PASS
Portable ASan/UBSan:                 13 PASS
DLL smoke:                           4 PASS
Full hooks/native/offline:          58 PASS / 4 historical private-input SKIP
Unique total:                       75 PASS / 4 SKIP
```

New c13 suite contributes5 PASS records: exact game gate/hash/identity/call refusal and guarded candidates/chain; actual raw ABI transparency; real concurrent+nested hardware-to-entry pairing;8-record cap/identity/stale/source metadata and unhook; synchronous cancellation/caller-integrity-loss cleanup/foreign-patch refusal with retained safe trampoline. An owned executable never passes the public exact game-RVA admission. Its private internal fixture setup tests mechanism only.

Two native threads (including reentry) produce **28 hardware store events paired to7 complete distinct true-entry invocations**. Every full map has all four component flags, exact source bits, and pointer candidate matches; one additional entry exercises the8-record cap/partial-no-event path. Scalar lane-to-component equality, partial/zero-observation/false-positive negatives, direct/indirect/E9 wrapper/unresolved chain and guard/noaccess/cross-page preservation pass. Source/Position metadata outside the native fixture's own stores is preserved.

Final composed production A/B orientation plus real Position hardware watch: **session1: 4116ms A177/B177, held2120ms/131updates; session2: 4101ms A178/B178, held2141ms/132updates**. Both normal verified Deinit/until1/retention1/hard0/clean debug restoration/Position writes0. This fixture intentionally does not impersonate the game's183-byte writer. Historical c7-c12 regression tests and private accepted original B444/caller128 proof/production installer mutation replay pass without executing/distributing private game instructions.

An intermediate repeated composed Position-watch run missed its required capture marker while independent native validation was also running. Its exact cause was not established; isolated integration and the **final complete serial suite** passed. The fixture now saves diagnostics on failure; no timing/safety/freshness/assertion gate was weakened. This remains distinct from actual game hitch/crash verification, which requires human A/E evidence.

Fresh extracted standalone source ZIP rebuilt the Windows DLL successfully with private game captures absent from the extracted source. Final CRC/current-source/docs/license/nested-script/hash/private-body-exclusion checks: PASS. Final evidence/hashes/sizes are recorded in `analysis/V01C13_RELEASE.json`. Only selected cloud stdout test logs are packaged; raw synthetic/user logs and complete game bodies are excluded.

## Safety / limitations / exact next edge

No Actor/Transform Position or Path A Position store, writer argument/source change, input/controller hook/rewrite, custom movement/velocity/gravity/adhesion, animation/camera/traversal feature or full function/raw user log distribution. The explicitly requested diagnostic MinHook entry install/restore is the narrow executable-code-hook exception; no data-page protection or PAGE_GUARD change. Existing known guarded SDK sampling is reused; no unknown native ABI/call is invented.

True entry values are before body mutation, but the several guarded reads are not an atomic engine state. The four anchored source-bit observations identify executed store operands; post-store target snapshots can contain concurrent effects. A full four-store map is not a function return, producer lifetime or downstream/exclusive effect certificate. Short/incomplete hardware coverage/caps/reentry limits can leave partial/no-event maps. Fixed candidate origins can miss computed or differently placed inputs. Existing c12 admitted-orientation-store/close overlap and bounded refresh limits remain.

**Actual c13 game installation, true entry/caller/store/candidate association, A/E stability, no introduced hitch/crash/hard cause and cleanup are pending.** No actual upstream source has been authenticated or made writable. If one origin correlates on A/E, the next exact edge is its construction/storage lifetime in caller0x351EEA7..0x351EEC5 and the observed caller2 producer path, including XYZ/W causal effects, downstream conflict and exclusive intended translation. If no origin matches, use partial masks/raw entry lanes and anchored store operands to identify the missing computation around the known unsupported+0xA edge. Do not infer engine velocity/direction/root motion or start a rewrite automatically.

## Outputs / human test / persistence

- `WallSprint_v0.1c13_writer_entry_snapshot.zip`
- `WallSprint_v0.1c13_source.zip`
- root `WallSprint.script` / `BUILD_MANIFEST.json`
- `analysis/V01C13_RELEASE.json`

GitHub README_TEST: ordinary~4s A/cardinal then~4s E/diagonal planar WallRun. No visible movement change is expected. Return complete private startup-through-cleanup log plus posture/hitch/freeze/crash observations; video optional. Require old exact build/dual-source/orientation/Deinit/hard0/Position0/DR cleanup and new ENTRY_GATE / START / ACTIVE / ENTRY_SNAPSHOT / INVOCATION_MAP / ENTRY_SOURCE_CANDIDATE / CALLER_CHAIN_RUNTIME / STOP records. Install/capture refusal/partial/unresolved records are exact blockers, not reasons to loosen gates. No private body/raw log needs public GitHub publication.

Implementation is committed at the SHA above. Result/ZIPs/script/manifest/release JSON form a separate delivery commit on the same authorized branch. Push and authenticated Git fetch/show hash/byte verification complete the handoff; no main rewrite/force push. Delivery message uses fixed GitHub raw ZIP and blob test/result URLs; anonymous/browser private-repository access is not claimed.

### GitHub publication recovered

After the user reconnected GitHub, push succeeded without force on **codex/v01c13-writer-entry-snapshot**. Published delivery **77c8a072c5401ced0dd427e764c142ab0780bf76** contains implementation **eb257e340be347d6ac17358efeb61eaa6a83066e**. A fresh isolated HTTPS Git clone from GitHub confirmed the branch SHA and retrieved both ZIPs; their complete bytes, sizes and SHA256 matched the local release record. The earlier credential failure is resolved for Git publication. GitHub REST API access remained unavailable, so artifact retrieval was verified through the fresh Git clone.

This publication-status refresh changes documentation and repackages the same validated c13 implementation. Code and existing **75 PASS / 4 SKIP** results remain unchanged; actual game A/E validation remains pending. Both refreshed ZIPs receive updated hashes in `analysis/V01C13_RELEASE.json`. The final metadata delivery is pushed and independently cloned again before reporting its fixed GitHub raw download and GitHub README_TEST links. The delivery message supplies that final SHA; private-repository browser access is not independently claimed.
