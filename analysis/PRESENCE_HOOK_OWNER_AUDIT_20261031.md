# Owner presence-hook audit161445 — 2026-10-31 JST
HOOK_ADMISSION_AUDIT_ONLY_BLOCKED; actual original EXE inspected; both accepted hashes unchanged.
Request runtime18488272..18488482; unwind155621344 v1flags0 prologue23 frame0. Codes: offset23 SAVE_NONVOL RSI1152,RBP1144,RBX1136, ALLOC_LARGE1120; offset16 PUSH_NONVOL RDI. Metadata SHA9f25979a133ba75c02e2da0f96631b692a6445a12456f96c4b477fac8fd00ce5.
Prefix7bytes: MOV R11,RSP (3), MOV saved RBX home entry RSP+8 (4); no RSP adjustment. Prefix SHA4409ca3689d39165729542bf9fc76b56ffe59d6e9b79d2ff1082a81be2ba2c37.
Gravity owner20246272..20246459; unwind155851960 v1flags0 prologue15 frame0. Codes offset15 SAVE_NONVOL RSI72,RBX64, ALLOC_SMALL48; offset11 PUSH_NONVOL RDI. Metadata SHAd95dec7a506aa265e67c8078b348991ba97d4271c5c73fa68982ef362b5dd60d.
Prefix5bytes original RBX home store RSP+8; SHA0666a96c5b537b54fb43e8d983f54c516a379f17f302825c7f047c7e9ff4cab0. Both HDE agreement, no early native unwind operations. Neither handler nor chain.
Actual MinHook trampoline NOT_EXECUTED. Both hook_admitted=false; observer false; no live reads. Missing native unwind input now RESOLVED. Remaining work is OWNED FIXTURE construction/execution for real relocated prefix, unwind/continuation/forwarding and phase/ring lifetime.
Do not request same audit/prologue/contract/relocation again or describe .xdata as still absent. RSP-preserving prefix alone is not sufficient or proof of failure; interpret native prologue offsets and home saves accurately. C52 not blanket inline ban.
