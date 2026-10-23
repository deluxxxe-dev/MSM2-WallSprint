# Codex result — WallSprint v0.1c2

Branch: **codex/v01c2-proof-fix**. Implementation commit: **2d26710034dcf695fa7bb2da763f117faf6171db**.
Base: published c1 implementation/handoff at 71437fac6c0640af01725a46e1b49c5627c739bb / 7c485461a8755387cd37167626bd6f420a83ba1b, with only the three latest main runtime/state/task commits cherry-picked. Main history was not rewritten.

## Outcome

The actual accepted-build writer444 and caller128 supplied privately by the user reproduce the exact c1 failure under the published old decoder and pass c2's complete proof. The old decoder rejects the first 5-byte stack save; HDE also cannot decode the following VEX instruction stream. c2 independently recovers all nine copies and verifies caller construction, stack relation, direct call and full instruction boundaries.

Version **v0.1c2 / package 0.1.20**.
Distribution: **WallSprint_v0.1c2_real_runtime_proof_fix.zip**.
Complete WallSprint source/build recipe: **WallSprint_v0.1c2_source.zip**.

## Implementation and files

- `compact_map_proof.*`: full x86-64 Capstone 5.0.6 startup decode with skipdata OFF, explicit/implicit register-write invalidation, RCX/RDX aliases, nine unique entry-block dword copies, scalar-product/local-store provenance, caller RSP alias/frame tracking and exact E8 return/target. Every writer444/caller128 byte must decode. Capstone's erroneous VMOVSS memory access flag is handled by the decoded instruction's known store direction.
- `reset_copy_source.*`: retains exact exe prerequisite, writer SHA/unwind/direct-call identity and current owned-patch integrity; adds the supplied exact caller128 SHA. Bounded first-failure diagnostics identify role/function/offset/absolute RVA/remaining bytes/backend/status/reason with at most 16 diagnostic bytes. Success logs the nine recovered offset pairs once. Full private function logging is removed.
- `telemetry.cpp`, `immediate_reset_trace.cpp`, native-source configure logging: exe identity, each writer identity, caller/map proof, and dual admission are separate. A later proof failure no longer labels the known exe as exact_build=0.
- `setup_runtime_decoder.sh`, build/test scripts, `vendor/capstone/`, notices: statically link the full x86 decoder from checksum-pinned upstream source; BSD/LLVM notices included; no external decoder DLL and no new SDK vendor file.
- `compact_proof_replay.cpp`, `replay_compact_proof.py`, `test_compact_proof.sh`, retained c1 test fixture: replay private actual inputs against both implementations, reject mutations/invalid dataflow, and test production installer authentication/installation/restoration on an owned registered image without executing game code.
- Required c1 result, decoder fix, retention and c2 gate analyses are mirrored under root/source. README_TEST, BUILD, WORKLOG, version, manifest and release recipe are updated.

c1 lock-free A/B snapshot, every-call exact Path A override, compact36-only Path B override, row-scale/metadata preservation, lifecycle/thread/Hero/target/stack/freshness/kill gates, hardware watch and retention logic are preserved. Direct Actor/Position and all later-milestone writes remain disabled.

## Validation

Completed with exit 0:

- Windows x64 build: WallSprint warnings as errors; static decoder built from verified upstream source.
- Portable ASan/UBSan: **11 PASS**.
- Windows DLL smoke: **4 PASS**, including separated admission diagnostics under a rejected host build.
- Full hook/offline/actual-byte replay suite: **31 PASS**, **4 explicit optional historical private-fixture SKIPs**. The c2 actual-byte regression itself ran.
- Exact old c1 replay on the real inputs: writer admitted=0, decoded_prefix_bytes=5, pairs=0, caller verified=0.
- c2 actual replay: **writer 444/444 bytes, 109 instructions; caller 128/128 bytes, 28 instructions; pairs=9; both admitted**.
- Production SHA/unwind proof + MinHook installation/owned restoration passes in the nonexecuted registered image using these actual inputs. Production authentication refuses writer and caller byte mutations.
- Actual-input negative tests reject missing/duplicate/outside-offset pairs, wrong return/target, truncated and midstream invalid x64 instructions, a VMOVSS read replacing a store, and stack relation mismatch. The A/B fixture refuses an attempted metadata alteration and verifies hooks still work while the worker state lock is deliberately held.
- Release checks cover version/default INI, PE32+, expected exports/system imports, no WriteProcessMemory, archive CRC/hash, licenses/source/build recipes, and private-input exclusion.
- Standalone source ZIP extracted into a fresh temporary directory: Windows DLL and pinned static decoder rebuilt successfully with private captures absent.

The selected `v01c2_proof.log` and hook log contain hashes/counts/derived offsets only. No raw private user log, captured full game function or game binary input is committed/distributed.

## Actual game status and next human test

**Startup proof implementation gate PASS; real c2 retention PENDING.** Static/installer replay never executes the captured game code and does not prove visible rotation or game retention.

First confirm startup:

```text
PATH_B_COMPACT3X3_PROOF admitted=1 caller_verified=1 writer_verified=1 rdx_to_rcx_map_verified=1 source_bytes=0x24 decoded_prefix_bytes=444 caller_decoded_bytes=128 pairs=9
```

Then run A Control left-to-right about 1.5 s and the same E diagonal left-to-right about 2 s. Expected visible success is sideways Actor rotation while the camera stays world-up. Require both override counts >0 and ORIENTATION_RETENTION_PASS=1 in the same active session, with no crash/transform/Position corruption. The unchanged threshold is dot(ActorUp,N398)>=0.98 for 300 ms or 20 distinct native updates. If proof fails, return its first-failure and admission lines before spending time on A/E. If another writer resets Up, use NEW_RESET_AFTER_DUAL_OVERRIDE for further investigation. No adhesion in this package.

## Persistence

Root WallSprint.script and BUILD_MANIFEST.json, both c2 ZIPs and this result are saved in the delivery commit following the implementation commit. The dedicated branch is pushed and its remote SHA checked at delivery. PROJECT_STATE and CODEX_NEXT remain the latest handoff content; they were not rewritten for the implementation.
