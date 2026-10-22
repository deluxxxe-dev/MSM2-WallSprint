# v0.1b7 result — reset paths identified

Ground truth supplied by `analysis/V01B7_RUNTIME_RESULT.md` / `docs/PROJECT_STATE.md`: accepted Spider-Man2.exe 2.810.0.0, A and E primary-thread prearm succeeded. The immediate reset writer is RVA **0x2CFC3C0..0x2CFC57C**, 444 bytes, captured SHA256 **ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196**, caller return **0x2CFC84B**, caller2 **0x35184A5**. It directly copies source basis dwords into Hero, including Up at +0x10/+14/+18, typically ~0.8–1.0 ms after the first source commit. E showed sequential component changes toward World Up.

A later common-writer WorldUp path also exists: 0x2CFC580 / return 0x2CFC8F3 with source_overridden_this_entry=0, ~14.9 ms after a surface commit in the strong A sample. These findings supersede historical b7 pre-test gates. The new task treats these checked-in runtime findings as ground truth; the original private log/video/full functions are not in this checkout and were not independently replayed here.

Source injection remains valid. b8 preserves it and supplies the same orientation to the second exact caller-local source. The old common-hook RESET_ENTRY_SOURCE with entry_correlated=0 is NOT evidence for the second function's entry RDX. b8 must capture its own entry and check source sanity/provenance. Post-prologue RDX≈RSP+0x60 is not an authenticated entry-stack offset.
