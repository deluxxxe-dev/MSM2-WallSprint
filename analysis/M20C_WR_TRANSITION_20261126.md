# M20c result: who calls the WR state transition (2026-11-26, build d5e0c19, log MSM2-WS-v0.1c48-20261126-131326)

Inspected in full: the log (7558 lines). No video.

## Scan (VERIFIED-RUNTIME + VERIFIED-STATIC)
- `WS_WR_TRANSITION_SCAN direct_sites=2`: both direct calls to the transition helper 0x367E5B0 are in one function,
  0x36825E0-0x3682AC0 (pdata), at 0x36827AC and 0x3682A56.
- Disassembly of the captured bytes (rbx = state machine object, rsi = requested state, rdi = context):
  - before site 1: r14b = "may transition"; it comes from the machine's vtable +0x68 check (only when a timer
    difference exceeds [rbx+0xE18]) and then, if a function pointer is stored at [rbx+0xE30], from that callback:
    `call [rbx+0xE30](owner=[rbx+0x10], current=[rbx+0x70], requested=rsi, context=rdi, r14b)` -> r14b = result.
    Then vtable +0x30; if that returns true and r14b == 0, the code jumps to 0x3682AA3 and does NOT call the
    transition; with r14b != 0 it calls 0x367E5B0 (site 1).
  - site 2 (0x3682A56) is reached on another path (vtable +0x30 false), after state-table checks (0x367C750 result 2/3
    skips it); on that path r14b == 0 does not prevent the call.
- So 0x36825E0 is a generic "request a state change" routine of a state-machine framework (used for all traversal
  states, not only WR). The per-machine callback at [rbx+0xE30] is a veto point on path 1 only.
- Unknown: which state value/object means WR, whether the hero's traversal machine has a callback at +0xE30, which path
  (1 or 2) the WR start on the face takes, and who requested the WR state (one more level up).

## This run
- One WR session of 11 ms (WallRunSide) at 04:18:06.019, ~1.2 s after a stop-hop landing; no `WS_FACE_WR_START` line (the
  hero was not in an eligible pinned frame at that moment). No up-sprint WR start was reproduced.

## Next evidence (proposal)
A bounded read-only detour on 0x36825E0 (exact bytes/pdata check, default OFF, F8), logging only while a hold is active:
machine pointer, requested state (rsi), current state ([rbx+0x70]), [rbx+0xE30] callback pointer, and which site was
taken; matched in time with the WR Init event. That identifies the WR state value and the refusal path. A veto (phase B)
is only built after that evidence.

## M20d (implemented by Claude Code, 2026-11-26)
- MinHook detour on 0x36825E0, installed only when the first 64 bytes match sha256
  5597730185c403a601f72a3aa2a9b8f50132054c0b41f6ac78c43905d4a47ec4 (captured from the M20c dump) and pdata begins there;
  INI `WALLSPRINT_WS_WR_REQUEST_DIAG` (code default 0, shipped 1); F8 makes it a pure pass-through.
- The original runs first and unchanged (all four argument registers, return value passed back). Afterwards, if a WR Init
  hook event was recorded on the same thread during the call (ws_hook_head advanced, slot kind 0, same thread id), one
  `WS_WR_REQUEST` line is logged (<= 60): machine, machine vtable RVA, owner [+0x10], current state before/after [+0x70]
  and their vtable RVAs, Init self, Entry seen, callback [+0xE30] and its RVA, return value, hold/face state, stick, RT.
- Reads only fields the original itself uses ([+0x10], [+0x70], [+0xE30]) plus the state objects' vtables via guarded reads;
  no writes.
- Checks: build PASS, test.sh PASS (85, incl. a static read-only/original-first/exact-byte/INI contract test), package PASS
  (script.disabled sha256 435ef1dbc8b5ea9976ec844ff7e1ac8fb15c34741260fa8f7639684921f39801, DLL
  0d42f38c2b7e87074653a240d4bb80b12e856adddb7cb4436b39ec245d00977a, packaged INI has the switch = 1); smoke/hooks
  UNAVAILABLE (wine). Tree 27ed18882779bb875a2f9de6bf717d0a234fb4b4.
