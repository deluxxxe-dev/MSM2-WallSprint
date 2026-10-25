# c34 gate — exact SHL only, statically known count and conservative flags

v0.1c34/package0.1.52. Accepted c33 MOVSXD r8,r8d advances to SHL0x2CF7FF7.
Actual operands/count/memory use UNKNOWN. Classify first. Only canonical pinned
register32/64 constant count can continue; width-derived mask63/31, correct
count0 operand/flags and32bit parent extension, conservative RMW destination
tags, undefined AF/OF -> aggregate may-top7 without value reads. Partial/CL/SP/
memory forms refuse. All c30-c33/both JA paths/RET/code/runtime/cap and safety
gates preserved; next unsupported instruction stops. Minimum new SHL metadata
A/E capture required; no raw values or full function/body. No producer traversal,
whole-writer certificate or gameplay write permission. See V01C34_SHL_FLAGS.md.
