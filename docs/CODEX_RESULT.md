# Codex result — WallSprint v0.1c14

Branch: **codex/v01c14-rdx-source-contract**.
Implementation commit: **617d7b6a84f752582aebd7be8a766c5ef02e476a**.
Pre-created c14 handoff/base: **4cbdc3ae0de317dfc958e5642fa62ce1b7a7d81e** (c13 implementation/delivery lineage preserved). No main rewrite or force push. CODEX_NEXT and the compact human c13 result are byte-identical to that handoff; PROJECT_STATE is explicitly updated as requested.

## Outcome / evidence boundary

**v0.1c14 / package0.1.32 implements read-only decoded RDX XYZ / independent W and bounded caller/producer diagnostics, plus corrected true-entry/XYZ aggregate accounting. Cloud build and regression tests PASS. Actual c14 diagnostic PASS remains PENDING human A/E.**

Accepted human c13 facts: A3875ms/E4281ms, verified Deinit/Up retention/hard0/Position writes0, no reported anomaly. Sixteen true entry snapshots each pair to all four stores. Pointer20 at RDX and stack23 at entryRSP+0x28 match executed XYZ bits (mask0x7); sourceW0/destinationW1. The cloud has no complete private current183-byte writer/30-byte caller body or running game. No actual decoded game XYZ/W/storage/producer outcome is invented from those equalities.

## What changed / files

- New owned `source/include/NativeHooks/rdx_source_contract.hpp`, `source/src/NativeHooks/rdx_source_contract.cpp`: separate bounded straight-line Capstone slice; c12 `movement_lineage.cpp` remains unchanged. Full contiguous decode/no skipdata, writer<=1024/caller<=8192, pointer aliases and stack width/overlap, scalar/packed SSE/AVX moves, double-lane loads, exact shuffle/unpack/blend lane rules, independent constant W and conservative arithmetic labels. Unknown/control/memory-effect/partial-width/address-size edges refuse proof with the first exact RVA/mnemonic. XYZ must be exactly entryRDX+0/+4/+8 bit loads to entryRCX+0x30/+0x34/+0x38. Production further requires the four exact known store sites0x2CFC936/93c/942/945. No expanded CFG project or typed game ABI.
- `writer_entry_snapshot.cpp/.hpp`: independently decoded startup proof uses the existing accepted exe, exact runtime boundaries/full c12 current startup baseline hashes/bytes and authenticated direct call. Existing raw hook, caps and cleanup remain. Per-invocation XYZ/W proof combines decoded path, exact complete ordered hardware store operands and explicit same-invocation identity with guarded equal RDX/stack snapshots and TEB bounds. An unreadable/unsupported linked event rejects the record; lost pairing provenance invalidates only its diagnostic token, preventing later stores from filling a lost entry path. Neither changes native orientation admission.
- Caller slice authenticates RDX assignment/precallRSP+0x20 and allocation range; caller-owned requires the whole16-byte inspected span above the32-byte outgoing home area and below the entry return slot. Merely inherited RDX stays owner-unproven even when its observed address matches. Lifetime labels describe the bounded allocation/current invocation, not exclusive engine use.
- The existing observed0x351A020 function is fully runtime-bounded/decoded and code-rechecked in the worker. Its anchored call is direct/indirect or an exact <=2-hop executable in-image E9/EB wrapper/tail chain; otherwise exact call target/next destination RVAs identify the blocker. Tiny-caller and parent pre-call stack-constructor candidates/barriers are separate; parent candidates are explicitly noncausal. No new producer hook, pointer chase, native call or return interception. Producer code scope is startup+worker, not an atomic instruction snapshot. Type/current Position/dt/direction/speed semantics remain unknown.
- `MOVEMENT_ENTRY_CAPTURE_SUMMARY` reports per-session actual records, complete maps, best XYZ origin/count/mask and source/destination observed W patterns. Pointer20 is preferred over its stack23 alias for ties. Candidates and maps now recognize repeated XYZ mask7 without requiring W equality. Stability is only the observed session-prefix XYZ bit-match pattern; a later XYZ mismatch clears it. Storage provenance/cross-surface/exclusive semantics stay unproven. Source W patterns cover XYZ-matched samples; destination W patterns cover complete maps.
- `source/src/msm2/orientation_trace.cpp`: passes the bounded parent startup snapshot and linked-event readability, and repairs `MOVEMENT_LINEAGE_SUMMARY` using real session entry counts/activation. The old static reconstruction readiness remains a separate field; c12 sourceMap/admission variables are not changed.
- New owned offline fixtures `rdx_contract_owned_test.S` / `rdx_source_contract_smoke.cpp`, `tools/test_rdx_contract.sh`; the existing concurrent/nested hardware entry fixture now explicitly uses sourceW0/destinationW1 and checks XYZ/stability/session/partial/duplicate/unreadable/zero-capture accounting. Build/test recipes link the new owned module. Required analyses, README_TEST/README/BUILD/WORKLOG, mod/version/manifest/release recipes updated and mirrored into the standalone source.

## Validation

The complete suite ran serially with warnings-as-errors. A final log-site clarification was additionally checked with Windows build, DLL smoke and all11 focused entry/contract tests; no admission or source policy changed. No safety, timing or existing assertion gate was relaxed.

```text
Windows x64 build:                    PASS
Portable ASan/UBSan:                  13 PASS
DLL smoke:                            4 PASS
Full public-source hooks/native:     60 PASS / 5 private-input SKIP
Public-source total PASS records:    77
Focused entry suite (included):        7 PASS
Focused contract suite (included):     4 PASS
Separate accepted private-byte replay: 4 PASS records
```

The5 raw public-suite SKIPs include the optional accepted writer444/caller128 replay; that replay was then run separately with the authorized private input and passed published-c1 reproduction, corrected complete copy/caller proof and negatives, and production installation/owned restoration on a nonexecuted owned image. Its fourth PASS record repeats the baseline reset-copy fixture, so separate totals are not summed as unique tests. The other4 historical unavailable-private-input checks remain skipped. Private game code was never executed or packaged.

Owned new fixtures prove scalar/AVX packed/shuffle at+0xA/double-lane XYZ copies separately from W=1, source/control/unknown-memory/source-stack/arithmetic/truncated/W refusal, caller-owned versus inherited/overlapping/guarded/overflow storage, and anchored direct/wrapper/two-hop tail/indirect/unresolved producer edges. These instructions are independently written, not reconstructions or captures of the game body.

Real owned hardware: **28 paired store events,7 complete concurrent/nested invocations,8 captured entries including cap/partial case**, exact XYZ mask7 on pointer20 in all7, sourceW0 preserved/destinationW1. Worker-only fixtures isolate two sessions, pointer20/stack23 aliases, later mismatches, duplicate/unreadable/partial maps, idempotent drain and zero capture. Raw ABI/flags/all GPR/YMM/XMM/x87/MXCSR/LastError/return and clean owned-hook/foreign-patch behavior still pass.

Composed unchanged production orientation with real Position watch: session1/A4102ms (A176/B176, held2105ms/130updates); session2/E4100ms (A179/B179, held2133ms/133updates). Both verified Deinit/retention/Up-dot1/hard0,12 Position watch events, no drops, exact DR restoration and Position writes0. This synthetic host deliberately cannot pass the public game entry gate.

Byte comparison with the handoff confirms unchanged native source/reset source/lifecycle/orientation ownership/WallRun/c12 analyzer/raw thunk. The complete cancel/stop/original-owned cleanup block is unchanged. CODEX_NEXT and accepted human evidence preserved.

Independently extracted standalone source ZIP built the Windows DLL with correct c14/package version, PE32+/exports/import checks and all11 focused entry/contract PASS records. Final archive CRC/current-source/docs/license/nested-script/hash/private-full-body exclusion checks are in `analysis/V01C14_RELEASE.json`. Only selected independent cloud stdout logs are packaged; raw synthetic/user logs and complete game bodies are excluded. The first extracted source built successfully, but direct focused-test execution stopped because the existing entry test assumed logs/ already existed. Both focused scripts now create their output directories; this test-setup fix changes no production code or assertions. The corrected source and all final source inputs were independently extracted/compared, rebuilt and tested successfully.

## Safety / actual remaining edges

Exact Spider-Man2.exe2.810.0.0 SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9 remains mandatory. Full original writer/caller baseline, exact E8 and runtime identity/Hero/Transform/session/generation/thread/epoch/freshness gates remain. Existing c7 recovery/c8 first cause/c9 secondary admission/c10 refresh are unchanged. Existing8 entries/session,8 windows, bounded TLS/stack/pointers, watch600ms/raw256/printed4/thread128/no rescans and retained trampoline/foreign-code refusal remain.

No Position/RDX/source/Path A/input/movement/gravity/adhesion/camera/animation write, guessed native ABI/game call, new hook target, data-page protection/PAGE_GUARD mutation, raw user log/full private body distribution or vendor/reference MOD copy. Only the previously authorized c13 diagnostic entry executable patch is installed/restored through the same existing mechanism.

`writable_source_proven=0` is unconditional. Actual c14 logs must establish the XYZ path and W behavior, narrow ownership and the actual producer edge. Prefix success does not certify a full CFG/return, engine type, mutation lifetime, downstream overwrite absence or exclusive intended Hero translation. Source reads/post-store rows are separate observations, not an atomic engine state. Caps/incomplete coverage cannot prove absence of conflicts. If the prefix is refused, use the precise barrier RVA/mnemonic; if RDX is inherited, use the anchored parent call target/next destination and local-construction barrier. Do not turn arithmetic resemblance or repeated equality into velocity/displacement/root-motion/direction semantics. No automatic movement rewrite.

## Outputs / persistence / next human test

- `WallSprint_v0.1c14_rdx_source_contract.zip`
- `WallSprint_v0.1c14_source.zip`
- root `WallSprint.script` / `BUILD_MANIFEST.json`
- `analysis/V01C14_RELEASE.json` with exact sizes/SHA256 and validation

Implementation is in the SHA above; result/ZIPs/script/manifest/release record form a delivery commit on the same authorized task branch. The final delivery message supplies its exact fixed GitHub SHA after fresh isolated remote clone and ZIP byte/size/hash/document comparison. No main rewrite or force push; private-repository browser access is not independently claimed.

Human test: ordinary A/cardinal~4s, fully end WallRun, move/fast-travel to E, E/diagonal~4s; send one complete private WallSprint log and note visible anomaly/hitch/freeze/crash. Video only for an anomaly or later targeted reproduction. Require unchanged lifecycle/Up/hard0/Position0/DR+entry-hook cleanup plus STATIC_CONTRACT / RDX_XYZ_PROOF / POSITION_W_PROOF / RDX_STORAGE_PROOF / RDX_PRODUCER_PROOF / ENTRY_CAPTURE_SUMMARY. Refusals/partial/unresolved are exact blockers, not permission to loosen gates.
