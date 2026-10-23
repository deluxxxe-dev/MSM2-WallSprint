# Codex result — WallSprint v0.1c17

Branch: **codex/v01c17-callee-alias-contract**.
Implementation commit: **be59ac2004f6810e5ea362e0de28a02f9c4cf8f4**.
Pre-created handoff/base: **74a4c49487f9363792b9924b3634ca6eefd57f42**.
C16 implementation0a00bde / delivery8f66b15 retained. AGENTS/DEV_RULES/CODEX_NEXT unchanged; PROJECT_STATE updated as explicitly requested. No main rewrite/force push.

## Outcome / evidence boundary

**v0.1c17 / package0.1.35 implements pinned137byte callee write-address alias diagnostics using exact writer pre-call provenance. Cloud build/tests PASS. Actual c17 alias result, XYZ/W advancement or narrower exact callee blocker remains PENDING human A/E.**

Human c16 A3875ms/E4203ms is accepted: verified Deinit/retention/Surface Up, hard0/stale0/Position writes0,8 complete entry maps each, source XYZ mask7/sourceW0/destinationW1 and clean diagnostic/watch restoration. A naturally admitted one bounded publication refresh. Exact call0x2CFC927 ->0x2B23560, return0x2CFC92C, boundary0x2B23560..0x2B235E9 and hash2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d are verified c16 metadata. C16 first MOV alias effect remained unknown.

Cloud has neither the current private writer nor callee body; it does not invent the actual first MOV operand, argument origin or next instruction. Owned fixtures establish the mechanism only. No raw log/full private body was requested or distributed.

## Changes / files

- `source/include/NativeHooks/rdx_source_contract.hpp`, `source/src/NativeHooks/rdx_source_contract.cpp`: separate bounded alias model and optional c17 branch-proof context. Legacy State/Slice/straight-line writer/c16 classifier/caller/producer and c15 graph construction stay unchanged. Exact writer pre-call GPR/vector/stack state maps into callee entry. RCX/RDX/R8/R9 labels/offsets/masks distinguish entry-RDX source from repurposed call-time RDX; unknown argument roots never imply distinct safe objects.
- Tracks exact pointer copies, simple LEA/add/sub, caller stack arguments and in-image RIP-relative globals. Source normalization to writer-entry RSP+0x28 is conditional on existing per-invocation storage admission. Loaded pointer values/indexed/segmented/truncated addresses remain unknown. Every reachable write gets site/width/root/offset/class; source/Transform/unknown/conflicting writes refuse. Private frame/home writes require known bounds, protected return/source exclusions and no overlap with writer saved carriers. Actual stack/register saves/restores determine preserved nonvolatiles; volatile GPR/XMM0..5 are always invalidated. No arguments/return/flags/native behavior are altered.
- Callee CFG is forward/local/instruction-aligned; both Jcc successors are enumerated without choosing flags. Caps137bytes/4 branches/16 pending paths/31 states per call,16 writer call states,64 total return paths,4096 total instruction visits,24 unique write sites. No recursion/nested-call expansion/indirect transfer/backedge/fixpoint. Missing/unbalanced return, unsupported effect and cap failure print exact blocker. Unique-site counts describe analyzed prefixes, not CPU execution counts. Different reaching addresses at one common store refuse even if individually safe.
- `writer_entry_snapshot.cpp`: pin measured callee target/boundary/full digest before alias proof, retaining all exe/writer/caller/current-byte gates. Whole target identity is rechecked per sampled invocation. Entire stack write/return footprint must fit captured TEB lower and upper bounds, source must equal entryRSP+0x28 with guarded bytes, and Transform must be nonstack. Image-global separation additionally requires complete TEB stack and protected Transform range to exclude the image; unequal pointer values alone are insufficient. Only all-resolved plus both protected no-write certificates can resume existing exact XYZ/W stores. Unsupported later writer effects remain exact. Writable source never becomes positive.
- Added `MOVEMENT_WRITER_CALL_ARGUMENT_CONTRACT`, `MOVEMENT_CALLEE_ALIAS_CONTRACT`; updated existing call/branch/XYZ/W/integrity records with compact metadata and exact address ambiguity. No full bytes, new observation hook, return interception, data-page protection change, game invocation or producer traversal.
- New `callee_alias_contract_smoke.cpp` / `tools/test_callee_alias.sh`; existing entry fixture gains pinned-identity/upper-footprint/image-region refusal mode. Included in full hooks suite. Required analyses/source mirrors, README_TEST/README, BUILD/WORKLOG, version/mod, PROJECT_STATE and release recipes updated. No new dependency/vendor/reference MOD source.

## Validation

Final Windows x64 build passed with warnings-as-errors. Full regressions ran serially. No assertion/timing/admission gate was weakened. Expanded boundary/cap fixtures were recompiled and passed in the focused alias run after the full suite; production code did not change.

```text
Portable ASan/UBSan:                    13 PASS
DLL smoke:                              4 PASS
Full public hooks/native:              83 PASS / 5 private-input SKIP
Total public PASS records:             100
Entry/ABI/hardware/finalization/gates:  10 PASS (included)
C14 straight-line/storage/producer:      4 PASS (included)
C15 branch contract:                    6 PASS (included)
C16 exact call/effect:                   7 PASS (included)
C17 alias contract:                     7 PASS (included)
Separate accepted private replay:       4 PASS records
```

One public SKIP is optional historical authenticated writer444/caller128 replay, separately passed using authorized private input. Its4 records include a duplicate baseline and are not summed as unique public tests; the other4 unavailable historical private-input checks remain skipped. Those older reset-copy functions are not the current Position writer/callee. Captured instructions are not executed or packaged.

New owned nonexecuted137/183byte fixtures cover private-stack/home/source16/Transform/return writes, MOV/LEA alias chains, repurposed call-time RDX, exact direct/global-argument RIP addresses and foreign global refusal, same-origin branch reconvergence versus conflicting addresses, all64 bounded return paths/fifth-branch refusal, unknown/index-width/control/nested-call blockers, caller saved-slot overlap, nonvolatile source-carrier clobber, exact XYZ/W continuation and later writer BSWAP barrier. Runtime helper tests cover fixed target/begin/end/digest mismatches, TEB footprint both bounds and complete image/source-stack/Transform range overlap/overflow refusals.

Unchanged raw entry tests preserve GPR/YMM/XMM/x87/MXCSR/flags/stack/nonvolatiles/LastError/returns. Concurrent+nested owned hardware still yields8 captures/7 complete maps/28 paired stores/two threads, XYZ mask7 with sourceW0/destinationW1. Finalized summaries remain stop-only/idempotent; never-activated session stays0. Cleanup/foreign patches/retained trampolines and all c7-c16 policies pass.

Composed production orientation/watch fixture: A4113ms (A176/B176, held2130ms/132 updates), E4106ms (A178/B178, held2136ms/132 updates). Both verified Deinit/retention/Up-dot1/hard0/no stale,12 Position events/no drops, clean DR restoration and Position writes0. This owned host cannot pass actual-game entry admission.

Standalone source ZIP: **PASS**. A fresh extraction without build/logs/private captures rebuilt the Windows DLL and passed all34 focused entry/contract/branch/call/alias records, with correct c17/package version, PE32+/exports/system imports and no WriteProcessMemory. Final archive/current-source/docs/license/hash/private-body exclusion checks are recorded in analysis/V01C17_RELEASE.json.

## Safety / limitations

Exact Spider-Man2.exe2.810.0.0/SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9 and measured callee hash are mandatory. Existing Hero/Transform/session/generation/thread/epoch/freshness/source/owned-code gates,8 entries/session and8 windows/600ms watch/raw256/printed4/thread128/no rescans remain. Native source/lifecycle/c12 analyzer, c13 raw thunk/start/activate/cancel/stop/pair/drain and c14/c15 final accounting/finalization remain unchanged.

Static no-write certificates are conditional on per-invocation alias/region admission. Code snapshots do not attest atomic execution; before/after equality cannot exclude transient writes. Unknown/conflicting address/unsupported effect supplies an exact diagnostic blocker instead of assumed safety. Static result A or narrower B needs no new runtime detour. Actual c17 target write map/proof/next blocker and visible A/E regression still require human evidence.

writable_source_proven=0 remains unconditional. No Position/RDX/stack-source/input/movement/velocity/gravity/adhesion/camera/animation writes. Producer0x351EB00 is deferred; caller_owned/ownership_exclusive/source_lifetime_exclusive/downstream_conflicts_proven_absent remain unpromoted0. Full writer proof would not grant mutation permission. Existing incomplete hardware coverage, admitted-store/close overlap and bounded refresh limits remain.

## Outputs / next human test

- WallSprint_v0.1c17_callee_alias_contract.zip
- WallSprint_v0.1c17_source.zip
- root WallSprint.script, BUILD_MANIFEST.json, analysis/V01C17_RELEASE.json

Install c17 in place of older WallSprint. Ordinary A/cardinal~4s -> fully end WallRun -> move/fast-travel -> E/diagonal~4s -> send one complete private WallSprint.log to ChatGPT. No special input; video only for visible anomaly. Open README_TEST on GitHub using fixed delivery link. Confirm c17/package0.1.35/exact build, pinned call identity, argument origins, ALIAS_CONTRACT write classes/conditional no-write/precise next effect, BRANCH_CONTRACT XYZ/next writer blocker, independent W, finalized summaries and clean orientation/watch restoration. No producer/movement follow-up automatically authorized.
