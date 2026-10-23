# Codex result — WallSprint v0.1c16

Branch: **codex/v01c16-writer-call-effect**.
Implementation commit: **0a00bdeee05ffdae593d6963abd5c24424a2d8ef**.
Pre-created handoff/base: **9eab1422f8c4e0c939d34f5aeb9492a188360127**.
Accepted c15 implementation0efd139 / delivery8e500b2 retained. CODEX_NEXT/AGENTS/DEV_RULES preserved; PROJECT_STATE updated as explicitly requested. No main rewrite or force push.

## Outcome / evidence boundary

**v0.1c16 / package0.1.34 implements exact writer-local call authentication and bounded source-effect diagnostics. Cloud build and regressions PASS. Actual c16 target/form/boundary/hash/effect or narrower callee/writer blocker remains PENDING human A/E.**

Accepted human c15: A3860ms/E4297ms, verified Deinit/retention/Surface Up, hard0/stale0/Position writes0,8 complete maps per session and finalized summaries. C15 crossed JNE0x2CFC91F and stopped exactly at CALL0x2CFC927. RDX=entryRSP+0x28, XYZ mask7/sourceW0/destinationW1 remain corroboration. The cloud has neither the current private183-byte writer nor the callee, so owned fixture success is not an actual game proof. No full raw log or private function was requested or distributed.

## Changes / files

- `source/include/NativeHooks/rdx_source_contract.hpp`, `source/src/NativeHooks/rdx_source_contract.cpp`: exact single-call API anchors writer+0x27/return+0x2c by complete decode. Direct target must be in image and outside writer. Indirect register/memory forms refuse without pointer chasing. Runtime supplies only one exact target; no recursion, branch/call expansion or fixpoint. Target <=1024bytes, complete decode, linear leaf through plain RET; unknown effects report exact callee RVA/mnemonic/class. A first entry JMP is labeled thunk/wrapper and stopped.
- Source-no-write effect language accepts register/flags operations and only explicitly proven private stack writes. PUSH/POP, allocation, GPR/XMM saves/restores are tracked with the unchanged independent provenance engine. Private writes must remain within the currently allocated negative stack span, at most4096bytes. Return/home/unallocated/unknown-pointer writes, unbalanced/out-of-range SP and unsupported/implicit effects refuse. Nonvolatile preservation is decoded from saves/restores; modified/unresolved carriers are invalidated. All Win64 volatile GPR and XMM0..5 lanes are invalidated unconditionally, so XYZ must reload through preserved aliases after the call.
- Optional call summary resumes only the exact c15 call/return/target. Every c15 relevant path and four exact Position stores remain required; mismatched provenance or a later unsupported instruction stops precisely. Caller saved slots must not overlap the possible callee-private/return/shadow footprint. The lowest write/return offset is carried to the writer certificate. Per invocation, source==entryRSP+0x28 with guarded equal TEB samples, nonstack Transform, complete ordered exact-site maps and a call footprint inside captured TEB bounds are mandatory. Call-dependent W requires these alias checks too and remains independent of XYZ.
- `writer_entry_snapshot.cpp`: exact-build/full writer/current owned-code/direct caller guards precede target lookup. Target must have a same-image runtime-function entry, end in image, <=1024bytes, no writer/deferred-producer overlap. Guarded full target bytes/SHA and a second-byte comparison establish startup current identity. Worker rechecks complete target/boundary/hash per sampled invocation before causal flags. `MOVEMENT_WRITER_CALL_CONTRACT` reports only compact metadata and exact effect blocker; `MOVEMENT_WRITER_CALL_INTEGRITY` reports code/stack-footprint checks. No new runtime observation hook, return interception, data-page protection change or game invocation.
- New owned `writer_call_effect_smoke.cpp` / `tools/test_writer_call.sh`, included in full hooks suite. Existing entry fixture gains an isolated real owned unwind-table/hash/suffix/boundary/TEB-footprint refusal mode. Old ABI/pairing/cleanup/finalization assertions are retained.
- Required c15/c16/RDX analyses, README_TEST/README and source mirrors, BUILD/WORKLOG, PROJECT_STATE, version/mod and package/release recipes updated. Selected independent cloud stdout logs only; raw synthetic/private logs are not packaged. Existing vendor/dependency sources are unchanged.

## Validation

Final production Windows x64 build passed with warnings-as-errors. Final full hooks suite ran serially; no assertion/timing/admission gate was weakened.

```text
Portable ASan/UBSan:                  13 PASS
DLL smoke:                            4 PASS
Full public hooks/native:            75 PASS / 5 private-input SKIP
Total public PASS records:           92
Entry/ABI/hardware/finalization:       9 PASS (included)
C14 straight-line/storage/producer:    4 PASS (included)
C15 branch contract:                  6 PASS (included)
C16 exact call/effect:                 7 PASS (included)
Separate accepted private replay:     4 PASS records
```

One of the5 public SKIPs is the optional authenticated historical writer444/caller128 replay, subsequently run with authorized private input. Its4 records include a duplicate baseline reset-copy PASS and are not summed as unique public tests. The other4 historical private-input checks remain unavailable/skipped. Captured game instructions are not executed or distributed; these are the older reset-copy functions, not the current183-byte Position writer.

Owned c16 fixtures exercise nested local reconvergence, source-safe direct call continuation, volatile GPR/XMM loss versus reload, explicit nonvolatile clobbers and decoded stack restoration, source XYZ/unknown/implicit/return/home/unallocated writes, private-stack caps/caller-spill overlap, indirect/out-of-image/wrong-site/return/target/boundary, malformed decode/no-return, callee branch/thunk/nested call, later precise writer BSWAP barrier and independent W constant/register/memory/computed/unknown.

Real owned runtime-function metadata and complete target identity tests reject changed suffix bytes even beyond the analyzed RET, wrong boundary/digest, removed unwind table and insufficient captured TEB stack headroom. This target is not executed or hooked. Existing raw-entry tests preserve GPR/YMM/XMM/x87/MXCSR/flags/stack/nonvolatiles/LastError/return; concurrent+nested hardware pairing remains8 captures/7 complete maps/28 stores/two threads, XYZ mask7 with sourceW0/destinationW1. Finalization still refuses preactivation/active drain and prints once after stop; never-activated session honestly stays0.

Unchanged composed production orientation/watch fixture: A4100ms (A174/B174, held2115ms/131 updates), E4106ms (A178/B178, held2140ms/132 updates). Both verified Deinit/retention/Up-dot1/hard0/no stale,12 Position events/no drops, clean DR restoration and Position writes0. This owned host cannot pass the actual-game public entry admission.

Byte comparisons preserve native orientation/reset/lifecycle/c7-c12 sources, c12 analyzer, raw entry thunk, shared legacy State/Slice/straight-line writer, caller/producer API, c15 graph construction, start/activate/cancel/stop/pair/drain/finalization and executable-code ownership/restoration. No producer traversal expanded.

Standalone source ZIP: **PASS**. A fresh extraction without builds/logs/private captures rebuilt the Windows DLL and passed all26 focused entry/contract/branch/call records, with correct c16/package version, PE32+/exports/system imports and no WriteProcessMemory. Final archive/source/license/private-body checks and hashes are recorded in `analysis/V01C16_RELEASE.json`.

## Safety / limitations

Exact Spider-Man2.exe2.810.0.0 SHA `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9` remains mandatory. Existing Hero/Transform/session/generation/thread/epoch/freshness/stack/owned-code gates,8 entries per session/8 windows,600ms watch/raw256/printed4/thread128/no rescans, c7 recovery/c8 immutable hard cause/c9 secondary admission/c10 bounded refresh/c11 DR cleanup/c12 exact call/c13 pairing/c14 aggregate/c15 finalized summary remain.

Startup/current code snapshots are accepted-build runtime identity, not an independently supplied historical callee digest or atomic execution attestation. No before/after equality is promoted over an unresolved effect; transient writes are not ruled out by equality. Unsupported callee effects remain an exact diagnostic B blocker. Extra runtime detours are unnecessary for that narrower static result, so no observation ABI is introduced. Actual c16 A/E, visible stability, target/effect and next exact blocker still require the human log.

`writable_source_proven=0` remains unconditional. No Position/RDX/stack-source/input/movement/velocity/gravity/adhesion/camera/animation writes. Producer0x351EB00 remains deferred; caller_owned/ownership_exclusive/source_lifetime_exclusive/downstream_conflicts_proven_absent remain unpromoted0. A writer certificate grants no upstream mutation permission. Prior incomplete watch coverage, admitted-store/close overlap and bounded refresh limits remain.

## Outputs / next human test

- `WallSprint_v0.1c16_writer_call_effect.zip`
- `WallSprint_v0.1c16_source.zip`
- root `WallSprint.script`, `BUILD_MANIFEST.json`, `analysis/V01C16_RELEASE.json`

After installing c16 in place of the older WallSprint: ordinary A/cardinal~4s -> fully end WallRun -> move/fast-travel to E -> E/diagonal~4s -> one complete private WallSprint.log to ChatGPT. No special input; video only for visible anomaly. Read README_TEST on GitHub using the fixed delivery link. Confirm startup c16/package0.1.34 and exact build accepted, then CALL_CONTRACT target/boundary/hash/effect/callee blocker, BRANCH_CONTRACT next writer blocker/XYZ, independent W, finalized capture/lineage summaries and clean A/E orientation/watch cleanup. No producer/movement follow-up is automatically authorized.
