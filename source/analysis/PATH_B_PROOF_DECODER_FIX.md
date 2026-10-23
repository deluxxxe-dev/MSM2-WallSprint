# c2 compact proof repair

## VERIFIED cause and repair

The original writer's first 5-byte instruction saves RBX to [RSP+8]. The c1 proof had no RSP fact, returned immediately from its memory resolver, and reported 5 decoded bytes despite recovering no pair. The original caller contains VEX VMOVSS/VMULSS instructions unsupported by HDE. The unchanged c1 source was replayed with both actual authenticated inputs and reproduced the exact runtime failure.

c2 uses the full Capstone 5.0.6 x86-64 decoder at startup only, statically linked from checksum-pinned upstream source. Skipdata is OFF. Every instruction in writer444 and caller128 must decode contiguously with details; a partial/invalid sequence fails closed. Explicit/implicit register writes invalidate unsupported dataflow facts. Ordinary prologue spills, RIP-relative unrelated reads and VEX arithmetic cannot be mistaken for orientation copies.

The writer verifier tracks RCX/RDX pointer aliases and exact 32-bit value copies. All nine pairs must be unique and recovered in the entry block before a call/branch/return; all remaining bytes are still decoded. The caller verifier tracks entry RSP aliases, sub/add/push/pop, scalar products, all nine VMOVSS local stores, exact LEA-derived RDX, exact direct E8 return/target, and callee-entry RDX=RSP+0x28. Unexpected pre-call control flow, missing initialization, changed offsets and unknown facts reject.

Capstone 5.0.6 marks VMOVSS memory destination access as READ. c2 determines its store direction from the decoded instruction ID and operand position, while retaining register-write effects from the decoder. Regression on the actual caller covers this decoder metadata issue.

## Recovered actual mapping

| Source offset | Destination offset | Store RVA |
|---|---|---|
| 0x00 | 0x00 | 0x2CFC3EE |
| 0x04 | 0x04 | 0x2CFC3F3 |
| 0x08 | 0x08 | 0x2CFC3F9 |
| 0x0C | 0x10 | 0x2CFC3FF |
| 0x10 | 0x14 | 0x2CFC405 |
| 0x14 | 0x18 | 0x2CFC40B |
| 0x18 | 0x20 | 0x2CFC411 |
| 0x1C | 0x24 | 0x2CFC417 |
| 0x20 | 0x28 | 0x2CFC41D |

Actual replay: writer 444/444 bytes, 109 instructions; caller 128/128 bytes, 28 instructions; pairs=9, both proofs admitted. Production original SHA/unwind/direct-call authentication, proof, MinHook installation and owned restoration also passed on these exact bytes in a registered owned image. The fixture never executes the game code.

## Admission, diagnostics and negative cases

The accepted exe version/hash gate is unchanged. The writer SHA and exact unwind range are unchanged. c2 additionally pins the supplied caller128 SHA256 `1665a7a329a983101673f05ccd95bbf3d738c39099469b4acfd14344ec98a79d`.

Startup logs the proof's complete byte counts and nine recovered offsets once. Failure reports role, function RVA, first failure offset/absolute RVA, remaining bytes, at most 16 diagnostic bytes, decoder backend/status and reason. Full private function capture logging is removed. `ORIENTATION_ADMISSION` reports exe identity, each writer identity, caller/map proof and final dual admission separately. The old composite reset-trace prerequisite field is no longer labeled exact_build.

Actual-byte negatives reject writer/caller hash mutations, wrong return/target, missing/duplicate pair, source/destination outside the orientation payload, truncated writer/caller, and stack relation mismatch. The native-source fixture additionally rejects an attempted metadata alteration and verifies the lock-contended worker does not reject the hot hooks.

Private inputs are not committed or distributed. `tools/test_compact_proof.sh` replays a locally supplied private capture and compares the retained c1 test implementation against c2. Only hashes, counters, derived RVAs and results are distributable.

## Still UNKNOWN

Real c2 A/E source writes, visible rotation, and retained Actor Up require the human run. Static proof replay and fixture retention are not game retention evidence.
