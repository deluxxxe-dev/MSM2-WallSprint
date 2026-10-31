# Native Windows v2 diagnostic result — 2026-10-31
Owner JSON27f66500...; actual native Windows; revision2; commit e402c7c3135fe6c7ce3520866e3fc4df4b4ac1f1; EXE SHAc6fc4fff0473c76bf353cbbaa3df40e66d076920871cb05bb7b03e08289e8850.
Primary remains146PASS/55FAIL, production=false. Do NOT call diagnostic instrumentation a successful correction.
Inspected ALL220 StackWalk traces grouped into4variants:
baseline_zero_context_frame_rsp0/55 pass;
captured_context_frame_rsp0/55;
captured_context_frame_rbp0/55;
baseline_context_genuine_call_return55/55.
In ALL55baseline traces, second returned frame.pc=expected_pc-1, SP=expected SP (first specimen and aggregation confirmed PC delta1 for55). Independent VirtualUnwind succeeds for original/observer ranges; missing table relay/tramp remains explicit leaf handling.
Original unhooked request+0: baseline initial RIP140695336487808, expected PC140695336489000, returned frame PC140695336488999 with context RIP140695336489000 at next-caller iteration. Genuine return expected/returned PC140695336488723, matching SP. Captured segments/frame changes alone do not fix it. DbgHelp6.2.26100.9549 API11.0.0.
This strongly localizes failure to synthetic entry-address return sentinel/oracle boundary behavior rather than a universal MinHook unwind failure. Exact documented native DbgHelp reason for -1 not yet independently established; don't claim Windows bug. Real-CALL-return variant proves an owned valid return address avoids all55mismatches.
Next: replace artificial primary caller setup with genuinely authored CALL-return control across identical primary positions, preserving exact PC/SP/nonvolatile and legacy-bad-metadata rejection. Keep invalid-sentinel case as explicitly separately named diagnostic, never accept PC±1 tolerance or OR alternatives into passing old primary.
No production/game diagnostic admitted from this result alone.
