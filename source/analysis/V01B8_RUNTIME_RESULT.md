# v0.1b8 runtime result — 2026-10-23

## Summary

User reported no visible orientation change. Runtime log agrees: v0.1b8 did not achieve retained surface-relative orientation.

Accepted build gate passed for Spider-Man2.exe 2.810.0.0 / SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9.

The second reset-copy hook was installed and authenticated:

- writer RVA 0x2CFC3C0
- caller return RVA 0x2CFC84B
- dual-source admission = 1

However, the second source override never executed. On both A and E, RESET_COPY_SOURCE_ENTRY failed closed with:

- source_readable=1
- source_is_stack_local=0
- layout_valid=0
- override=0
- reason=caller_local_provenance_or_source_sanity_failed

Representative A:

- RDX = 0x433812D300
- RSP = 0x433812D2D8
- stack bounds = 0x4338107000 .. 0x4338130000

Representative E used the same relation and failed the same way.

Important: although the RDX address numerically lies inside the thread stack and is RSP+0x28 at the captured entry, its first 0x40 bytes do not satisfy the assumed ordinary affine 4x4 transform layout. Therefore the previous assumption that path B uses the same RDX matrix layout as 0x2CFC580 is not proven and should be dropped.

The existing common-writer path still works briefly: 0x2CFC8F3 / 0x2CFC580 source overrides produce interpolated Surface-relative Up values. But the 0x2CFC84B / 0x2CFC3C0 path then writes the Hero Up back toward World Up. Retention stayed at held_updates=0 / held_ms=0 / stability_observed=0.

Later 0x2CFC8F3 calls also still appear with source_overridden_this_entry=0 and World-Up source after the dual-source attempt aborts.

## Conclusion

v0.1b8 is a useful fail-closed result, not an ownership success.

The immediate blocker is now source-layout/provenance characterization for the exact 0x2CFC84B -> 0x2CFC3C0 call. Do not write to its RDX block until its semantics are statically and dynamically established.

Next milestone should be diagnostic-first v0.1b9: decode the caller and writer, map which exact source fields feed RCX orientation, and determine whether RDX is a packed transform-like structure, mixed structure, pointer-bearing local, or another representation. Keep direct Actor writes, adhesion, movement, animation, camera and physics disabled.