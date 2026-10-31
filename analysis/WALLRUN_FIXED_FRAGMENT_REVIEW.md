# Captured41-byte gate and fixed77-byte completion

2026-10-31 JST; baseline c224f17, branch codex/v01c20-edi-value-provenance.
Sources: WALLRUN_CONTINUATION_OWNER_RESULT_20261031.md and
FLAT_WALL_LATERAL_ROUTE_REVIEW.md. No original raw body/JSON is committed here.

## What was actually reviewed

Owner capture41 bytes0x14248f9..0x1424922, SHA
c5f5f80a08fe76ed7251ec5abc4be1ed6d3ce1479eafc2b5e32038437caed0b3,
seven operations and unwind chain are accepted supplied STATIC evidence. The full
original owner JSON is not locally present; do not reconstruct its raw bytes from
mnemonics or assert unreported RIP constant identities.
Existing private /tmp/ws-lateral-update-review/Update.bin WAS available:422 bytes
at0x1424530, independently rechecked SHA
e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f,
pinned Capstone5.0.6 decoded it for selected register-definition/branch review.
This is on-disk prior authenticated code evidence, NOT branch-execution evidence.

## Compact provenance / control interpretation

| Value/site | Established on the known prefix -> fragment routes | Still unknown |
|---|---|---|
| RSI | prefix0x142455a copies original RCX; retained owner alias along the four known direct incoming edges | C++ type/fields/extra incoming paths/global dominance |
| RDI / CL | entry RDI=RCX+0x110; later alternative RDI=RBX where RBX=RSI+0x110. At0x142464f..63 a qword through RDI feeds a dword load+0x7d0, shift8 and mask1. CL is bit8 of that loaded dword | pointed object type, lifetime, bit meaning; NOT measured requested direction/input |
| AL | supplied fragment scalar comparison sets AL via SETAE (CF==0). NaN/unordered compare does not set this condition. Earlier AL contents overwritten | constant identity/value, owner+0x40c meaning, eventual function return/consumer |
| EDX | fragment zero-extends byte at RSI+0x42c; this replaces prior EDX incl prior DL | byte's semantics and how remainder consumes it |
| RBP+0x118 | authenticated prologue RBP=entryRSP-0x108, hence this byte is entryRSP+0x10, caller home-stack area. Prefix alternative sets it1; fragment clears it0 | later use of home byte; not an Actor/Position store |
| CL -> JE | no intervening CL write/call in supplied seven operations. CL==0 branches back to0x14246d4; CL!=0 takes missing36byte fallthrough | actual branch outcome in owner run |
| 0x14246d4 | authenticated instruction is XOR AL,1; branch enters predicate inversion, NOT an authenticated RET. Prefix ends immediately afterward0x14246d6 | subsequent epilog/use/return, separate out-of-span fallthrough |

On the known prefix routes, the first incoming JNE0x142468f carries CL=1
(masked bit). Reaching any of the three later incoming branches requires that
first JNE was not taken, hence CL=0, with no intervening CL mutation. The captured
JE sends those later routes to AL inversion; the CL=1 route reaches missing36.
This is a path condition, not gameplay branch coverage or the bit's meaning.

Four earlier prefix branches to0x14248f9 involve this CL bit, byte comparison,
another predicate and scalar comparison. No scalar offset may be renamed down
speed/contact/input. Prefix helper calls remain untyped; no new signature or
invocation inferred. RSI-relative loads are reviewed statically, not read live.

Classification: the supplied41 bytes perform scalar/byte predicate preparation
and conditional control gating. They do not show a tangent movement request,
vector transform submission or verified input/motion API. This is a statement
about these operations ONLY, not proof that the entire Update or missing36 bytes
has no movement effect. Chain metadata now directly supports split layout;
previous "split plausible" wording is superseded for this supplied association.
No descending cause/fix follows. Full remainder is needed to resolve local AL/
EDX/control use instead of asking owner one branch/instruction at a time.

## Minimal existing-helper completion

Existing capture_wallrun_continuation.py gains --complete-fragment, a boolean
mode with NO configurable RVA/range/hash. Legacy first-control mode/tests remain.
Windows entrypoint now selects complete_fixed77 and outputs a new fragment77
filename, preserving the older41-byte JSON and source/local changes.
Fixed range0x14248f9..0x1424946 (77 bytes), of which36 are new. Require pinned EXE
SHA; exact covering runtime begin/end; UNW_FLAG_CHAININFO and exact parent tuple
(0x1424530,0x14246d6,0x94bdc14); section/file-backed coverage through fixed end;
first41 bytes SHA equal accepted owner capture. Mismatch refuses, never expands.

Only inside this exact span, decode through direct/conditional call/jump
boundaries and record edges without following them. First-control rule remains
unchanged in legacy mode/outside scope. RET refuses with an explicit reason, including at the fixed end. INT/IRET/syscall/HLT/transaction/other
unsupported controls or decode gaps/truncation refuse. End is a known metadata
fragment, not an entire logical function/complete CFG. No callee/branch-target/
chained-parent code is read. No movement API/certificate or gameplay writes.

No authenticated EXE, complete77/remainder capture or supplied JSON is present in
current /workspace or /tmp. Only known old prefix can be analyzed directly.
ONE fresh offline full77 action genuinely remains necessary; don't repeat old
first41 capture or unchanged r6 game test. Exact bootstrap in CODEX_RESULT.
Return only small private MSM2-WS-wallrun-fragment77 timestamped JSON from WS logs.
No runtime version/build/package/source-native/KIT/game changes in this task.
