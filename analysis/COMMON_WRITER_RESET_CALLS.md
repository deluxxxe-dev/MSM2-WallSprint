# Common writer reset calls — v0.1b7

CANDIDATE known writer RVA 0x2CFC580..0x2CFC7C8, original 584-byte SHA256 `3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6`. Existing authenticated WallRun caller return is 0x2CFC8F3; it alone may receive the established source override. Other callers are read-only diagnostics, never skipped or patched.

During the admitted prearmed window, COMMON_WRITER_TRACE records each current-Hero Transform invocation on any calling thread: raw RCX/RDX/R8/R9, caller RVA, thread, session/elapsed/QPC/entry id, validated source Right/Up/Forward, Up dot World/N398, read-only Actor Up before original, normal and this-entry override flag. Read only 64 bytes at RDX with overflow/readability/affine basis/finite bounds checks. Invalid pointers get source_valid=0; no secondary pointers followed. Bounded recording can fail; drops close capture rather than imply complete coverage.

Hardware is primary-thread-only. For its latest known entry, correlation requires same thread, generation/owner/native update, entry time <= event, exact Transform, unwound caller equality, post-store RSI == raw entry RDX and resolved runtime function RVA 0x2CFC580. Source validity is separately required for RESET_PATH_CORRELATED=1. A stale entry from another function cannot establish an unknown ABI. common_writer_reset=0 preserves another function as a candidate, not proof its RDX is a transform.

UNKNOWN before real A/E logs: whether the reset is the same caller, a second common-writer caller, or another native writer. No second-caller claim has been inferred from synthetic tests. Extract reset_source_up only with valid entry correlation; use the complete private anchored decode for actual store address/hash and sanitize results before sharing.
