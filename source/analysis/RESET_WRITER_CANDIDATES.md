# Reset writer candidates — before v0.1b6 game test

**RESET_WRITER_VERIFIED=0.** v0.1b5 identifies a successful source path and subsequent sampled WorldUp, not the intervening instruction.

| Candidate | Confirmed evidence | Missing evidence |
|---|---|---|
| Common writer RVA0x2CFC580, another caller | Original584byte body verified SHA3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6; source RDX→RSI, target RCX→RBX | Actual reset store/caller/source/time |
| Same writer/caller0x2CFC8F3, unmodified invocation | Existing caller-local source route works; admission can skip a call safely | An actual WorldUp-source reset invocation after tilted Up |
| A different native writer | Actor Up returns toWorldUp in actual snapshots | Actual watched store, AMD64 boundary, complete hash and minimal decode; upstream ABI still unknown |

Known common writer range0x2CFC580..0x2CFC7C8. Principal32byte basis store0x2CFC5E2 (`vmovups [rbx],ymm0`); hardware RIP-after0x2CFC5E6. Later scaled-basis stores must be decoded separately. No reset call is patched or suppressed. No candidate is promoted using a worker timestamp or synthetic fixture.

New logs should supply RESET_TRACE_EVENT / RESET_ENTRY_SOURCE / RESET_WRITER_CANDIDATE / RESET_WRITER_FUNCTION plus bounded private generic writer captures. Parser output is an analysis artifact; it grants no next-phase writes.
