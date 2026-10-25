# v0.1c31 gate — exact VMOVUPS diagnostic only, actual form pending

v0.1c31/package0.1.49. C30 exact qword unknown-top7/no value reads accepted;
actual barrier VMOVUPS0x2CF7FF0. Classify only pinned encoding/width/operands/
access/direction/current parent aliases and upper-state effects. Decoder may
misreport VMOVUPS store access; encoding-verified stores/RMW refuse immediately.
No vector load semantics: XMM-only state lacks complete YMM/ZMM/upper-lane
representation, actual form UNKNOWN, masks/merge/modifiers refused. No vector
value observation/tag0/pointer chase/generic memory rule.

C30 top7, c29 tag0 refusal, original both-path/all-RET/code/runtime/cap gates
and all ownership/lifetime/exclusive/writable-source/game-write gates remain.
Minimum new VECTOR/OPERAND/ADDRESS/ARCH A/E metadata required; actual target
identity/effects or vector architectural form not inferred from fixtures.
See V01C31_VMOVUPS_CLASSIFICATION.md / CODEX_RESULT.md.
