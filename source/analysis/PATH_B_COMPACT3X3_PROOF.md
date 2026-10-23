# Path B compact 3x3 proof

## Startup admission

Path B remains fail closed unless all of the following agree on the accepted executable before the hook is installed:

- writer function range `0x2CFC3C0..0x2CFC57C`, size 444, and SHA256 `ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196`;
- caller function unwind boundary `0x2CFC7D0..0x2CFC850`;
- the instruction ending at return RVA `0x2CFC84B` is a direct `E8` call to `0x2CFC3C0`;
- a bounded HDE64 decode derives exactly nine 32-bit direct-copy pairs from RDX offsets `0x00..0x20` to the expected padded RCX offsets;
- every required source offset occurs once and no instruction boundary, direction, alias provenance, or destination differs.

The proof follows register aliases only. Unknown operations cannot create a proof and invalidate their explicit register destination. The production log reports `PATH_B_COMPACT3X3_PROOF` with the caller, writer, mapping, and `source_bytes=0x24` fields.

## Runtime store boundary

The callback additionally requires the exact caller return, current Hero Transform target, current verified lifecycle/thread/session, a fresh coherent shared snapshot, and `RDX == entry RSP + 0x28` in writable current-thread stack memory. It validates the complete 64-byte local before committing, writes 36 bytes, and validates the complete 64 bytes afterward. Bytes at and after `RDX+0x24` must remain byte-identical. Row lengths must be finite and in `[0.5,2.0]`, and the normalized input and replacement must be proper rotations.

No captured original game bytes are distributed. The hash, RVAs, derived pairs, decoder, and independent synthetic fixtures are sufficient to reproduce the admission decision without publishing the private function capture.
