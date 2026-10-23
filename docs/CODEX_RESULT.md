# Codex result — WallSprint v0.1c19

Branch: **codex/v01c19-transform-write-contract**.
Implementation commit: **b3185cd240a16446a2424ecab9fa0c2605cb00e8**.
Handoff/base: **1754b97435c2feb54d496085c030b8e5251339ee**. C18 implementation19aa301/delivery3ffc19b retained; latest human evidence in V01C18_RUNTIME_RESULT.md is authoritative. AGENTS/DEV_RULES/CODEX_NEXT/LOGGING_CONVENTION unchanged; PROJECT_STATE updated as requested. No main rewrite/force push.

## Outcome / evidence

**v0.1c19 / package0.1.37 implements the exact MOV0x2B2358E destination/value/reaching-state contract and a one-site protected-effect model for exact Transform+0x10C dword writes. Cloud build/regressions PASS. Actual c19 instruction operands/value/all-reaching effects/next blocker/XYZ/W remain PENDING human A/E.**

Accepted c18 actual A2984ms/E4171ms: verified Deinit/retention/Up1 and1.00000012/stale0/hard0/Position0,8true entries/8complete maps each, XYZ mask7/sourceW0/destinationW1 and clean entry/watch restoration. Timestamped dedicated log_wallsprint/no shared fallback remains verified. Actual callee5branches/4Jcc/8blocks/10edges passed;3safe stack/home writes preceded exact width4 MOV0x2B2358E/root1+268/0x10C, previously possible Transform alias. Actual RCX/RDX arguments are Transform-derived; entry-RDX source is distinct, R8 constant/nonpointer, R9 unknown. Cloud has no current private137/183byte bodies, so fixtures do not establish actual c19 operand or value facts.

## What changed / files

- NativeHooks/rdx_source_contract.hpp/.cpp: opt-in exact site (production0x2B2358E) metadata copies authenticated destination base/index/scale/displacement, mnemonic/width and source operand/value origin. State root1 is traced from writer-entry RCX through existing exact pointer operations; not inferred from a number alone. Unknown/indexed/truncated/memory-loaded pointers cannot become Transform roots. Reaching destination/value conflicts refuse; all_reaching_states_same_* and all_reaching_states_examined only become1 after complete relevant writer traversal. Prefix counters explicitly remain prefix evidence.
- Only a scalar MOV dword at that site, exactly original writer RCX+0x10C, with same exact modeled value origin on reaching paths can receive the new effect. Protected Transform+[0,64), including Position+[0x30,0x40), is unchanged. Other offsets/sites/widths retain c18 refusal. Constant/exact entry-register/exact memory-load values are classified; opaque computed or unknown values refuse instead of guessed provenance. No semantic label for+0x10C is invented.
- A real Transform write keeps literal transform_no_write_proven=0. New transform_position_protected_range_unmodified and relevant_transform_effects_resolved must both pass with source_span_no_write_proven/all aliases before unchanged exact Position-store proof resumes. Existing call.noWrite means source no-write, not literal memory/Transform write-free. Telemetry reason/effect distinguish those notions.
- New bounded64unique decoded read-address states across relevant writer and callee paths conservatively exclude earlier/later overlapping or unknown read carriers, including read cap/out-of-bounded-stack refusal. This may refuse unused reads; it is not a minimal causal slicer. No memory-value symbol is reused across a potentially overlapping write. Explicit and implicit POP stack reads contribute to the existing full TEB footprint; image-relative reads require whole-span/image exclusion. Nested/foreign calls remain exact unexpanded blockers.
- writer_entry_snapshot.cpp: compact MOVEMENT_CALLEE_WRITE_CONTRACT, new narrow protection fields in ALIAS_CONTRACT and extended integrity gates. Runtime full Transform+[0,0x110) must exclude the TEB and, for image effects, the authenticated image; overflow/malformed/partial overlaps refuse. Existing pinned full current hash/bytes/boundary, direct-call, source=entryRSP+0x28, guarded source bytes, exact ordered store/invocation and W-independent gates retained. No new observation/hook/thunk/protection mutation.
- New transform_write_contract_smoke.cpp/tools/test_transform_write.sh7groups, full hooks integration; writer_entry_snapshot_smoke.cpp/tools/test_writer_entry.sh adds1extended-range group with its own output file so finalized regression logs remain intact. All c18 expectations retained. Required analyses/source mirrors, README_TEST/README, PROJECT_STATE, BUILD/WORKLOG, version/mod and package/release recipes updated. No vendor/dependency/reference MOD addition.

## Validation

Final Windows x64 warnings-as-errors build PASS; serial regressions:

```text
Portable ASan/UBSan                       13 PASS
DLL smoke + isolated logging              7 PASS
Full public hooks/native                 97 PASS /5 private-input SKIP
Total public PASS records                117
Entry/ABI/hardware/cleanup/range           11 PASS (included)
C14 straight-line/storage                  4 PASS (included)
C15 writer branch                         6 PASS (included)
C16 exact call/effect                      7 PASS (included)
C17 alias                                 7 PASS (included)
C18 local callee CFG                      6 PASS (included)
C19 exact write/dependency                 7 PASS (included)
Logging                                   3 PASS (included in DLL smoke)
Separate accepted old private replay       4 PASS records
```

The historical replay includes1duplicate baseline and resolves1optional public SKIP separately; other4 unavailable historical private inputs stay skipped. Those older444/128byte reset-copy functions are not the current137/183byte callee/writer. No captured body is executed/published/packaged.

Owned tests cover exact+0x10C value/destination/full-path proof and literal Transform-no-write0; protected identity/XYZ/W overlaps/neighboring offset/no opt-in/auth/site boundary refusal; differing roots/displacements/value origins; constant/register/memory/computed/unknown classification; earlier/later/unknown writer read carriers, callee read-after-write, out-of-bounded-stack reads and64read cap; later safe home writes with successful XYZ/W and precise later writer BSWAP/callee CALL. Full extended272byte Transform TEB/image tests cover touching bounds/partial overlap/malformed ranges/overflow. Existing c18 branch budgets, conflicts, control and private-stack/global/source/saved-carrier tests pass.

Unchanged owned entry ABI preserves GPR/YMM/XMM/x87/MXCSR/flags/stack/nonvolatiles/LastError/returns; concurrent+nested8captures/7complete maps/28paired stores/two threads, XYZ7/sourceW0/destW1 and finalized cleanup. Owned production A/E orientation/watch retains >=4s verified Deinit/retention/Up1/hard0/Position0/12events and clean DR restoration; exact durations/counters recorded in V01C19_RELEASE.json. Fixtures cannot pass actual-game admission.

Standalone source ZIP: **PASS: fresh source ZIP rebuilt Windows DLL;48 focused plus7 DLL/logger records, no prior build/logs/private inputs.** Final archive CRC/current complete sources/docs/licenses/nested script/DLL hash/private-body exclusion recorded in V01C19_RELEASE.json. No browser/anonymous HTTP download check is claimed.

## Safety / limitations

Exact exe2.810.0.0/SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9, pinned callee0x2B23560..0x2B235E9/hash2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d and full current code/boundary remain mandatory. C18 CFG/byte/branch/path/state/visit/write caps and c15 writer graph unchanged. Legacy State/Slice/straight-line writer/caller/producer/c16 classifier, native lifecycle/c12 analyzer, raw entry thunk/start/activate/cancel/stop/pair/drain/finalization and dedicated logger unchanged. No test assertion/timing/admission gate is weakened.

Only new read exclusion/narrow call gates are inserted in writer traversal. Unknown/conflicting/computed values, unsupported effects and conservatively possible read dependence remain exact diagnostic blockers; no claim that actual+0x10C is harmless is made before the game log. Per-invocation region conditions remain mandatory; code snapshots are not atomic execution attestation and equality never excludes transient effects. Metadata with incomplete coverage grants no all-path certificate. Existing incomplete hardware coverage/close overlap/bounded refresh limits remain.

writable_source_proven=0; caller_owned=0; ownership_exclusive=0; source_lifetime_exclusive=0; downstream_conflicts_proven_absent=0. Position/source/input/movement writes=0. Producer0x351EB00 deferred; no new runtime observation, game call, PAGE_GUARD/data-page mutation, movement/velocity/gravity/adhesion/camera/animation/traversal.

## Outputs / next human test

- WallSprint_v0.1c19_transform_write_contract.zip
- WallSprint_v0.1c19_source.zip
- WallSprint.script / BUILD_MANIFEST.json / analysis/V01C19_RELEASE.json

Use fixed GitHub direct ZIP and GitHub README_TEST links. Replace old WallSprint; confirm v0.1c19/package0.1.37/exact build and a new log_wallsprint timestamped run file. Normal A/cardinal~4s -> fully end -> move/fast-travel -> E/diagonal~4s. Privately send newest complete run log to ChatGPT; video only for visible anomaly. Inspect exact MOV operands/value/root/offset/reaching coverage/dependency site, literal Transform-no-write versus narrow protection/effects, next precise blocker or XYZ/W,8finalizedentries/maps, Deinit/retention/Up and watch restoration. No automatic producer/movement follow-up authorization.
