# v0.1c2 gate

Package 0.1.20: `WallSprint_v0.1c2_real_runtime_proof_fix.zip`.

## Implementation / actual-byte proof: PASS

The SHA-authenticated real writer444/caller128 inputs reproduce the old c1 failure and pass the c2 proof: writer_verified=1, caller_verified=1, rdx_to_rcx_map_verified=1, pairs=9. Full decode counts are 444 and 128; no undecodable byte is skipped. Original-byte mutation, missing/duplicate/outside-offset, wrong-return/target, midstream/truncated decode and stack-relation negatives fail closed. A nonexecuted registered-image test also passes the production installer and patch restoration.

c1 lock-free shared snapshot, every-call Path A override, compact36-only Path B override with row-scale/metadata preservation, lifecycle/identity/thread/freshness/kill gates and bounded hardware watch are retained. No direct Actor/Position or later-milestone writes are enabled.

## Actual c2 runtime / retention: PENDING

First confirm startup `PATH_B_COMPACT3X3_PROOF admitted=1` with caller_verified=1, writer_verified=1, rdx_to_rcx_map_verified=1, decoded_prefix_bytes=444, caller_decoded_bytes=128, pairs=9. Then run A ~1.5 s and E ~2 s.

Runtime success requires both override counts >0 and `ORIENTATION_RETENTION_PASS=1` in the same active session, with no crash or transform/Position corruption. The retention threshold remains dot>=0.98 for >=300 ms or >=20 distinct native updates. Further resets are diagnostic only. No adhesion in this package even if retention succeeds.
