# v0.1c3 bounded multithread Up trace

## Scope

Keep c2 exact-build, Capstone/hash/caller proof, lock-free shared A/B snapshots, source boundaries, scale/metadata/Position preservation and retention unchanged. No third-path patch or direct Actor write is added.

The existing helper owns setup/cleanup. It secures the primary thread at about elapsed0.60, then performs ONE Toolhelp process-thread snapshot at elapsed>=0.85, with an absolute elapsed1.50 end. At most128 process-thread entries are considered. The helper itself is excluded because it never writes Hero memory and cannot suspend itself. New threads after this snapshot are not covered; rescan_count=0 is explicit.

## Installation and restoration

Every opened handle is checked against the current process. Active foreign DR0/DR1 or GD causes a skip; free DR0/DR1 may coexist with foreign DR2/DR3. The Up12-byte range is watched as8+4 bytes. Each suspension has a retained lease before context operations, protected setup, readback and balanced Resume attempts. Already-suspended threads are skipped after balancing only our own increment. Failed/partial setup rolls back or retains the lease for cleanup. No allocation/log/SDK/user lock is taken while another thread is suspended.

Cleanup restores saved DR0/DR1 controls/address/status; if both slots remain owned and unrelated status is unchanged, complete saved DR6 is restored too. Concurrent foreign slot takeover is preserved and reported. Lifecycle/identity/guard/kill/window/event budgets close capture; the helper resumes and restores all retained leases. Immutable ownership tombstones and the pinned VEH remain for delayed exceptions. Existing page protection and Actor/Position are untouched.

MULTITHREAD_UP_WATCH reports seen/eligible/installed/foreign/failed/cap/drops and bounded first16 skipped TIDs/reasons. Complete coverage refers only to the eligible snapshot, never the lifetime set of process threads. MULTITHREAD_UP_WATCH_RESTORE reports pending cleanup explicitly.

## Hit evidence

Every retained hit is classified by actual function identity: KNOWN_PATH_A at0x2CFC580, KNOWN_PATH_B at0x2CFC3C0, otherwise UNKNOWN_WRITER. Read failure and unchanged values are explicit; unchanged dual-watch records are suppressed. Separate atomic last-A/last-B commit times are snapshotted with each event. Diagnostic lock contention drops a record and marks incomplete evidence, while preserving watch operation and A/B commits.

Callers are accepted only after current-thread stack bounds/readability, bounded guarded unwind/leaf-return reads, monotonic stack advance<=64KiB and executable return-page checks. Missing/foreign-module game RVAs remain0 with raw addresses retained, rather than guessed callers.

THIRD_WRITER_EVENT is emitted once per session after both commits when before dotSurface>=.90, after dotSurface<=.50 and dotWorld increases. Sequential component stores may use a Surface anchor from<=5ms earlier with explicit non-atomic before_semantics. A known writer with another caller/thread can be an unexplained path and is not suppressed solely by its function name.

THIRD_WRITER_IDENTITY logs module/RIP/function bounds/size/SHA and callers. At most16KiB of the one selected function is captured into the LOCAL private log, each THIRD_WRITER_CODE chunk marked private_do_not_distribute=1. Missing unwind/oversize/unreadable identity stays unavailable. Runtime hashes may include an existing owned entry hook; they are not labeled original-before-hook. No raw game input is committed or packaged.

## Verification and limits

Owned native-thread hardware fixtures cover a second-thread unknown writer, actual function hash/caller evidence, once/session reporting, sequential resets after the old1.10 deadline, foreign DR0 refusal, coexistence/restoration of foreign DR2, diagnostic contention, invalid stack refusal, timeout/Deinit/identity/F8/shutdown cleanup and unchanged Position/page protection. Existing c2 ABI/source/proof/reset regressions remain required.

Actual third-writer identity is still UNKNOWN until the human c3 run. Snapshot gaps, foreign slots, caps, read/drop/setup failures and no event never prove absence. Retention remains dot>=.98 for300ms or20 distinct native updates; no later milestone is authorized here.
