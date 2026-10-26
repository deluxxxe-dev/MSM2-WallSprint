# c35 gate — classify second VMOVUPS independently

v0.1c35/package0.1.53. Accepted c34 SHL r8,6 retains flags top7 and reaches
VMOVUPS0x2CF7FFB. Actual form/direction/address UNKNOWN. Separate VECTOR2
classification; only canonical pinned full VEX2 XMM/YMM load from STILL-proven
current Hero RDX+0 range may reuse c32 low/high state. VEX128 high zero and
VEX256 high top7 are exact destination effects; other parents/RAX/R8/flags7
preserved. No values/pointer chase. Stores/RMW/dynamic/masked/EVEX/wider/other
sources refuse. All-path/current-code/runtime/cap/RET and ownership/write gates
remain. No generic AVX or next opcode rule. Minimum compact actual VECTOR2
capture needed; see V01C35_SECOND_VMOVUPS.md / CODEX_RESULT.md.
