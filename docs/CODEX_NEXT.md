# Next task — fix native Windows StackWalk fixture failure before observer integration
Branch codex/v01c20-edi-value-provenance; no main merge. Read AGENTS required docs, analysis/PRESENCE_OWNED_WINDOWS_FAIL_20261031.md, fixture audit/design and current source.
Owner native fixture returned146PASS/55FAIL; ALL55 StackWalk64 caller checks fail INCLUDING unhooked original controls. VirtualUnwind/function-table/ABI/real tramp/race groups pass. Production false; no game observer yet. Do not repeat unchanged test or reinterpret Wine PASS as native PASS.

## Execute
Inspect exact fixture.cpp StackWalk64 usage and independently documented native Windows contract: initial STACKFRAME64/CONTEXT addresses/modes, stack bounds/backing synthetic memory, function-table/module/read callbacks for owned dynamic tables, first-call/current-frame vs next-caller iteration and errors. Establish whether existing oracle is flawed or actual metadata/continuation fails. Original unhooked controls must be first discriminator.
Use primary Windows API documentation if needed. Add compact per-failure diagnostics: initial PC/SP/FP, expected caller, each returned PC/SP/frame, BOOL/GetLastError with appropriate stale-error handling, metadata lookup/callback events and environment/DbgHelp version. Do not dump arbitrary process memory. Compare direct RtlVirtualUnwind independently; don't replace/remove StackWalk requirements.
Fix demonstrated fixture/API misuse with proof and regression coverage, or fix real metadata issue without weakening assertions. Preserve legacy32-failure counterexample and its ability to detect unsafe restore metadata. No blanket passing/skipping/relabeling native checks.
If cannot reproduce native in cloud, deliver corrected or instrumented OWNED executable, exact SHA/source manifest and ONE standalone runner invocation. No game attachment/build/deploy required. Keep old evidence and report Wine vs native distinctly.
After true native success, resume Request-only runtime observer task; no production integration before evidence. Do not perform more game EXE/scanner acquisitions.

## Scope/delivery
Owned fixture/helpers/docs only until native review; production runtime/config/packages/shared environment/SF/C52 unchanged. No gameplay/unknown pointee reads or writes.
Run focused owned fixed/legacy/runner checks appropriate to changes; no unchanged whole-runtime suites just for fixture diff. Update analysis/PROJECT_STATE/CODEX_RESULT, commit/push/readback. Final concrete cause if proven and exact owner runner/output; no offer to prepare later.
