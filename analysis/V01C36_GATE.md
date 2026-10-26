# c36 gate — exact potential store escape, no CFG continuation

v0.1c36/package0.1.54. Accepted exact VEX3.256 m256[R8+RAX]<-YMM0 at0x2CF7FFB
has32byte unknown destination. Opaque RAX/non-pointer R8 plus tags0/7 cannot
exclude code/control-stack or tracked storage. Record valid current-code store
seen/address/value halves/alias masks/escape1/first_escape/pending, propagate to
parent before identity refusal, preserve architectural state and stop at store.
No bytes/memory map/later reads/RET, MOD writes or containment permission.
Clean paths do not erase escape. Both JA/all current-code/runtime/caps and
ownership/write gates remain. New non-value provenance/range/lifetime/control
exclusion proof is needed; same A/E tags cannot supply it. Runtime c36 capture
only verifies new summary/safety. See V01C36_STORE_CONTAINMENT.md.
