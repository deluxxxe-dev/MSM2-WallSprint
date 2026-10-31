# Native Windows owned fixture failure — 2026-10-31
Owner JSON62591b05... inspected. SchemaWS-owned-presence-fixture-v1; native Windows; commit7439b0471e958eb1a71e39ed826699fc42c60ab2; EXE SHA885cb79fb9e6fc0b4d9e7a8957c30f5679a07ede4bfe6dabf271598e3baa1d5e.
146PASS/55FAIL/0native-unavailable; production_admitted=false.
ALL55 failures are assertions containing StackWalk64 caller. Failures include UNHOOKED original+0/+3/+7 etc, not only observer/trampoline restore positions.
Other assertion groups pass, including actual MinHook create/enable/span/relay/continuation checks, GPR/flags/YMM/return/stack/nonvolatile preservation, function-table presence, RtlVirtualUnwind stack/nonvolatile checks, forwarding/disable, cap/race/close.
Therefore Windows fixture is genuinely FAIL; Wine201PASS cannot override. Pattern suggests shared StackWalk oracle/setup/native API frame iteration or callbacks problem; not proven root cause and not yet proof native hooks themselves broken.
Do not delete/skip failed55, equate VirtualUnwind with StackWalk, or admit production. Need investigate original-control StackWalk semantics/init/context/read/function-table/module callbacks and frame iteration, with observed vs expected diagnostics.
No game test/runtime update until corrected fixture evaluated.
