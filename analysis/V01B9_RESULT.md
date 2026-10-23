# v0.1b9 result used by v0.1c1

## VERIFIED runtime facts

The checked-in v0.1b9 result establishes the exact target as `Spider-Man2.exe 2.810.0.0`, SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.

Path A is the padded orientation-source writer at RVA `0x2CFC580`, called from return RVA `0x2CFC8F3`. Path B is the writer at `0x2CFC3C0..0x2CFC57C`, SHA256 `ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196`, called from return RVA `0x2CFC84B` in `0x2CFC7D0..0x2CFC850`.

At exact Path B entries on both tested A and E walls, `RDX == entry RSP + 0x28`, the source and span are in the current thread stack, and the source is readable. Its first nine floats at `[RDX,RDX+0x24)` map row-wise into RCX padded orientation rows at offsets `0x00/04/08`, `0x10/14/18`, and `0x20/24/28`. Both walls reproduced the same semantics.

v0.1b9 intentionally used `path_b_override=0`, so no visual rotation or retention PASS was expected. It also measured frequent Path A `state_lock_contention` rejects. These facts authorize the narrow c1 compact-source experiment and the removal of mutex admission from both hot hooks. They do not authorize adhesion or any direct Actor write.

## UNKNOWN until the human c1 run

Actual A/E orientation retention after both overrides, visible sideways rotation, and any later reset writer remain unverified. Synthetic tests are implementation validation only.
