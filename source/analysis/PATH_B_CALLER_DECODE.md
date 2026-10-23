# Path B caller: authenticated capture and decode status

## Measured / authenticated

Accepted Spider-Man2.exe 2.810.0.0 SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9. Exact caller unwind span 0x2CFC7D0..0x2CFC850 (128 bytes); direct E8 at 0x2CFC846 returns at 0x2CFC84B and targets 0x2CFC3C0. Startup requires this exact span/target, accepted executable and unchanged caller bytes. No guessed C++ ABI.

Runtime evidence establishes raw RDX=entry RSP+0x28 on A/E; this is a numerical observation, not instruction-level proof of how the caller constructs the local.

## Instruction-level status: UNKNOWN until original-byte capture

The supplied summary contains no original caller instructions. No instruction listing, nested pointer, temporary-object type, R8/R9 role or mixed-structure semantic is invented here. Full static mapping cannot be completed from addresses and a digest alone.

b9 logs `PATH_B_FUNCTION role=caller` and contiguous `PATH_B_CODE role=caller` before hook installation, under the accepted-build gate. `tools/review_path_b_layout.py` checks span/size/checksum, decodes every byte with Capstone (no skipping), checks the exact call return/target, and emits instruction RVAs plus must-analysis argument provenance at calls. It tracks RSP push/pop/add/sub and LEA/move aliases, intersects CFG predecessors, invalidates call/partial-register clobbers. Nested loads, spill reloads and unsupported computations remain UNKNOWN. Compare RDX provenance with current RSP at the call; callee entry RSP is caller RSP-8.

Next exact evidence: startup `PATH_B_FUNCTION`/`PATH_B_CODE` caller128 and writer444 plus bounded b9 entry/store events. Those private captures go to ChatGPT for derived instruction-level analysis; do not commit/package original game bytes. No undirected full-log re-analysis is required.
