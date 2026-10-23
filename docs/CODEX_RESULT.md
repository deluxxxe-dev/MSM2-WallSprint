# Codex result — WallSprint v0.1c15

Branch: **codex/v01c15-writer-branch-contract**.
Implementation commit: **0efd13924426b1c96247ff35577f7f323c934b97**.
Pre-created c15 handoff/base: **7514c9c39b4c5fdfddbae35509ab80c225acc925**; accepted c14 implementation617d7b6 / delivery5fd2cdff9ae6732cf0eaa53fec02253c4d6ae25c preserved. CODEX_NEXT and compact human c14 evidence are byte-identical to the handoff; PROJECT_STATE explicitly updated as requested. No main rewrite/force push.

## Outcome / evidence boundary

**v0.1c15 / package0.1.33 implements bounded branch-aware writer RDX XYZ / independent W proof and fixes capture-summary finalization. Cloud build/tests PASS. Actual c15 writer proof or exact next local blocker remains PENDING human A/E.**

Accepted human c14: A4453ms/E4172ms, retained Up/verified Deinit/hard0/Position writes0/no stale/clean DR+entry-hook restoration;8 true entries/complete maps per session and correct final lineage summary. RDX=entryRSP+0x28, XYZ mask7/sourceW0/destW1 remain correlation. C14 fully decoded the183-byte writer but stopped at0x2CFC91F JNE. Parent anchored call0x351A01B->0x351EB00 is measured metadata, with caller ownership/provenance/lifetime/downstream/exclusive effects unproven.

The cloud has no complete private current writer body or running game. Owned183-byte functions are independently authored fixtures, not captures or reconstructions of the game. A real c15 diagnostic PASS A (writer XYZ+separate W proved) or B (precise narrower local blocker) is adjudicated only from the new actual runtime evidence. No equality is upgraded into actual causal proof here.

## What changed / files

- `source/include/NativeHooks/rdx_source_contract.hpp` / `source/src/NativeHooks/rdx_source_contract.cpp`: new `branchWriter()` shares the existing c14 instruction/provenance model, leaving the old straight-line `writer()`, caller/producer API and c12 analyzer unchanged. Limits are183bytes/block16/edge24/branch4/path16/state31/instruction visits4096. Full decode/no skipdata, direct forward instruction-aligned local branches only, both ordinary Jcc successors without guessing flags, no recursion/call expansion/fixpoint. Loop/backedge/foreign/indirect/malformed/cap failure emits its exact local barrier/edge.
- A reverse DAG pass identifies instructions that can reach one of the four named stores; separate bounded states enumerate all relevant entry paths. Unknown relevant instruction/memory effects refuse. Suffix effects unable to reach a target store are outside the proof, not certified safe or exclusive. Structural transfer validation still covers the decoded span. Each exact0x2CFC936/93c/942/945 store must target its entry-RCX component. Conflicting reaching provenance invalidates that component; paths bypassing a store supply no evidence, and each required store needs at least one reaching entry path. XYZ origin must remain entryRDX+0/+4/+8 on every relevant path. `all_relevant_store_paths_proven` explicitly covers XYZ; W is independent.
- W constant_one/independent_register/independent_memory requires consistent decoded provenance. Computed/unknown/conflicting W remains classified but unproven; destination1.0 alone never supplies provenance. homogeneous_coordinate_semantics stays unknown, writable_source_proven0.
- `writer_entry_snapshot.cpp`: production config selects the new exact-site proof under the unchanged exe/boundary/full-baseline-hash/current-code/direct-call gates and emits `MOVEMENT_RDX_BRANCH_CONTRACT` with static graph/path counts, component paths/mask, all caps and precise next barrier. Per-invocation XYZ/W additionally require the branch-bounded exact-site certificate and all existing true-entry/identity/ordered-hardware/guarded-stack/bit checks. The obsolete generic prefix-mismatch overwrite is removed so it cannot hide the next exact local blocker. No additional data-instruction semantics, memory dump or game ABI is introduced; branches explore both successors.
- Summary timing: the existing cancel operation additionally sets a reporting-only finalized flag after detaching the active scope. Worker drain waits for finalization and no callback; the existing hardware queue/inflight drain gate is unchanged. Inactive before activation is no longer mistaken for completion. Final evidence prints once with phase=finalized/summary_final=1; never-activated windows can honestly report zero on stop. Install/activate/freshness/8-entry/session/watch/hardware/raw-hook/cleanup policies are unchanged. The executable unhook/owned-code/foreign-patch block is unchanged.
- New `writer_branch_contract_smoke.cpp` / `tools/test_writer_branch.sh` provides owned183-byte offline fixtures and is included in the complete hooks suite. Existing entry fixture adds real preactivation/active-drain/stop-only summary tests and retains concurrent/nested hardware and ABI/cleanup regressions. Required analyses, version/mod/README_TEST/README/BUILD/WORKLOG, source copies, PROJECT_STATE and packaging metadata updated. No producer hook, producer traversal expansion, new dependency or vendor/reference MOD code.

## Validation

Complete suite ran serially with warnings-as-errors. No safety/timing/old assertion gate was weakened.

```text
Windows x64 build:                    PASS
Portable ASan/UBSan:                  13 PASS
DLL smoke:                            4 PASS
Full public-source hooks/native:     67 PASS / 5 private-input SKIP
Public-source total PASS records:    84
Entry/ABI/hardware/finalization:       8 PASS (included above)
C14 straight-line/storage/producer:    4 PASS (included above)
C15 branch contract:                  6 PASS (included above)
Separate accepted private-byte replay: 4 PASS records
```

One of the5 public-suite SKIPs is the optional accepted writer444/caller128 replay, subsequently run separately with authorized private input: published-c1 refusal reproduction, corrected copy/caller proof and negatives, production installation/owned restoration on a nonexecuted owned image passed. Its fourth PASS repeats the baseline reset-copy fixture, so separate records are not summed as unique tests. The other4 historical unavailable-private-input checks remain skipped. Private game instructions are not executed or packaged.

New branch fixtures prove both JNE successors reconverge on identical RDX XYZ provenance with separate W1. One-leg X/Y/Z changes reject exactly that component; selective bypass only contributes to stores actually reached, while unreachable stores cannot be proved. Nested4 branches and all16 paths/31 states pass under caps;5th branch/oversize/loops/backedges/indirect/call/outside/mid-instruction/unsupported/memory/wrong-site/wrong-target/W conflict negatives refuse. Next unsupported examples emit exact owned local+0x24 RVA/mnemonic, instead of the old JNE barrier+0x1F. This demonstrates mechanism, not the actual game's next instruction.

Real owned entry hook: preactivation drains never emit/finalize zero; active capture is not drained early; stop prints actual one-entry partial count once; another never-activated session prints honest zero at stop. Existing actual hardware test retains28 paired store events/seven complete concurrent+nested invocations/eight captured records, exact XYZ mask7 with sourceW0 unchanged/destW1. ABI/flags/all GPR/YMM/XMM/x87/MXCSR/LastError/return, guards/identity/hash/caller/freshness and foreign-patch cleanup remain PASS.

Composed unchanged production A/B orientation with real Position watch: session1/A4100ms (A175/B175, held2118ms/131updates), session2/E4110ms (A177/B177, held2110ms/131updates). Both verified Deinit/retention/Up-dot1/hard0/no stale,12 watch events/no drops, clean DR restoration and Position writes0. This owned host cannot pass the exact public game-entry admission.

Byte comparison confirms unchanged native orientation/reset/lifecycle/c12 analyzer/raw thunk/orientation watch/c14 final lineage accounting/caller and producer metadata code. Cancel adds only a reporting flag; the executable stop/unhook block remains identical. CODEX_NEXT and compact human c14 result preserved.

Fresh extracted standalone source ZIP rebuilt the Windows DLL with correct c15/package version, PE32+/exports/import checks and all18 focused entry/contract/branch PASS records. No private captures are in the extracted source. Final CRC/current-source/docs/license/nested-script/hash/private-full-body exclusion checks are in `analysis/V01C15_RELEASE.json`. Only selected independent cloud stdout logs are packaged; raw synthetic/user logs and full private game bodies are excluded.

## Safety / remaining gate

Exact Spider-Man2.exe2.810.0.0 SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9, full current writer/caller baseline, exact direct E8, current owned-code and live Hero/Transform/session/generation/thread/epoch/freshness gates remain. C7 recovery/c8 immutable cause/c9 secondary admission/c10 bounded refresh, c11 DR watch/cleanup, c12 direct-call proof, c13 entry/pairing and c14 final accounting remain. Max8 entry records/session,8 windows,600ms watch/raw256/printed4/thread128/no rescans, bounded stack/pointers/TLS and retained safe trampoline remain.

No Position/RDX/stack-source/Path A/input/movement/velocity/gravity/adhesion/camera/animation write; no producer hook, new producer read/traversal expansion, game call/guessed ABI, PAGE_GUARD/data-page mutation, raw user log/full private body or copied mod/vendor assets. Only the already-authorized exact c13 diagnostic executable entry patch is installed/restored through its unchanged mechanism.

`writable_source_proven=0` unconditionally. Historical producer0x351EB00, caller_owned0/ownership_exclusive0/source_lifetime_exclusive0/downstream_conflicts_proven_absent0 remain unpromoted. A local writer certificate is not source lifetime/type/whole-function return/downstream/exclusive Hero translation or mutation permission. Counts are static proof exploration, not measured CPU branch decisions. Guarded entry/post-store observations are not atomic engine state; incomplete hardware coverage cannot prove absence of conflicts.

Actual c15 will either prove all relevant exact XYZ paths and independently classify W, or report one precise narrower local writer barrier. No producer investigation or automatic movement rewrite is part of this task. Only after writer evidence is adjudicated should a separate task decide whether to inspect0x351EB00.

## Outputs / persistence / next human test

- `WallSprint_v0.1c15_writer_branch_contract.zip`
- `WallSprint_v0.1c15_source.zip`
- root `WallSprint.script` / `BUILD_MANIFEST.json`
- `analysis/V01C15_RELEASE.json` with exact sizes/hashes/validation

Implementation is at the SHA above; result/script/manifest/ZIPs/release record form a delivery commit on the same authorized branch. The final message supplies a fixed GitHub SHA after fresh remote clone and exact ZIP byte/size/SHA256/CRC/document comparison. No main rewrite/force push. Private-repository browser access is not independently claimed.

Human test: ordinary A/cardinal~4s, fully end WallRun, move/fast-travel to E, E/diagonal~4s; send one complete private WallSprint log and note visible anomaly/hitch/freeze/crash. Video only for a visible anomaly. Require unchanged lifecycle/Up/hard0/Position0/DR+entry-hook cleanup, BRANCH_CONTRACT / XYZ_PROOF / POSITION_W_PROOF and final capture summary. Unknown/partial/refused records preserve their exact reasons; do not loosen gates to make counts positive.
