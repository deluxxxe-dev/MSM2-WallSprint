# c33 gate — classify pinned MOVSXD; conditional full64-from-reg32 only

v0.1c33/package0.1.51. Accepted c32 advances to MOVSXD0x2CF7FF4, actual operands
UNKNOWN. Exact REX.W opcode63 register32/full64 form may be modeled after current
encoding/access/full-write/flags/SP checks. Source-parent tags replace prior
destination, opaque non-pointer origin, other GPR/vector/flags/memory unchanged.
Memory forms remain refused; no new memory rule or value observation.

All c30/c32 source/state policies and original both-path/RET/code/runtime/caps
remain; next unsupported op still refuses. No whole-writer/ownership/lifetime/
exclusivity/writable-source or gameplay-write permission. Minimum new MOVSXD
metadata capture needed for actual form/next barrier; no raw source/body requested.
See V01C33_MOVSXD.md / CODEX_RESULT.md.
