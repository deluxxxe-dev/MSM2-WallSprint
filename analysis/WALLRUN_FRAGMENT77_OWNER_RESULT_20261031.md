# Complete77 owner result — 2026-10-31 JST
Supplied MSM2-WS-wallrun-fragment77-20261031-123525.json successfully read.
CAPTURED_ORIGINAL_OFFLINE_WINDOW/complete_fixed77, same pinned EXE and chain.
77bytes0x14248f9..0x1424946,17instructions; recomputed length/hash and sum instruction sizes match. SHA0a40476fcf82c4462b95a41e724df1d53e040d0e4fda6a911f238e2b03ce2738. previous_window_sha_verifiedtrue.
No raw binary/body is committed.

New36bytes:
TEST AL; JE0x1424938; TEST R9B; JNE0x1424938.
Otherwise XOR AL,AL; write AL(0) to[RBP+0x118]; JMP0x14246d6.
0x1424938 sets AL1; writes0 to[RBP+0x118]; JMP0x14246d6.
No new EDX read/write in tail. Every fragment path clears this stack byte; no direction-vector or movement submission in these77 bytes.
Define comparison predicate P=SETAE outcome already established, C=CL!=0, R=R9B!=0.
Cfalse -> old0x14246d4 XOR AL1 -> AL=!P.
Ctrue -> AL=(!P || R) ->0x14246d6.
These are scalar low-byte facts, NOT inferred bool function return or named gameplay flags. EDX preserves zero-extended owner byte across tail, later use UNKNOWN.
Shared successor0x14246d6 remains outside earlier422byte prefix and outside77 fragment. No complete Update/return/input semantics follows. No descending root cause established.
Do not repeat77 acquisition. Next review actual prefix reaching R9B definition and AL/EDX consumers using existing captures; missing successor code can be acquired only by one scoped offline follow-up if necessary, not recursive search or game mutation.
