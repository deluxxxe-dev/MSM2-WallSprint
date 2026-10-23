# Path B orientation store map

Verified writer identity: RVA0x2CFC3C0..0x2CFC57C,444 original bytes, SHA256 ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196. b8 hardware observations place the reset in this function. Exact source-bearing store offsets and R8/R9/constant participation remain UNKNOWN because original instructions are absent from the supplied summary.

b9 captures the authenticated original writer before local hook patching. The private offline decoder emits every instruction, RDX-relative reads and RCX+[0,0x30) candidate stores with width, source fact and direct RDX offset only for tracked exact loads/copies. Computations/unknown dependencies are not mislabeled direct copies; unsupported dependencies remain unknown and require instruction review. No semantics are inferred from a hash.

For each hardware event, PATH_B_ENTRY_LINK uses Path B's own entry (never common-writer args), current-thread entry serial, session/generation/owner/target/caller and <=5ms QPC relation. New B callbacks invalidate old association even when capture fails. Offline PATH_B_STORE additionally requires complete original hash-verified writer, an instruction ending exactly at post-store RIP, effective destination matching entry RCX+mapped offset, overlap with watched Up, matching session/thread/entryQPC/target and read_valid. Wrong PC/address/thread/stale links are rejected.

Output: instruction_rva, destination_rcx_offset, source_kind, source_rdx_offset if direct, before and after. Before means previous watched observation, not atomic pre-instruction value. Sequential stores can expose partial intermediate Up; evaluate the complete sequence. Missing/unsupported mappings never establish absence or permit writes.

No actual-game PATH_B_STORE row is asserted in this checkout. Next input is the b9 original startup captures and RESET_COPY_LAYOUT_ENTRY / PATH_B_ENTRY_LINK / ORIENTATION_WRITER_EVENT / ORIENTATION_WRITER_REGS for A/E in elapsed0.60..1.50. Commit derived offset/instruction conclusions only, not full captured game functions.
