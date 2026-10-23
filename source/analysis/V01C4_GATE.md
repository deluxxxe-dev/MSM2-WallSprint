# v0.1c4 gate — real thread enumeration fix

Cloud regression PASS is separate from **PENDING REAL-GAME COVERAGE**.

Minimum human PASS on accepted Spider-Man2.exe2.810.0.0 / documented full SHA:

```text
PATH_B_COMPACT3X3_PROOF admitted=1 ... decoded_prefix_bytes=444 caller_decoded_bytes=128 pairs=9
DUAL_SOURCE_ADMISSION admitted=1
PATH_A_OVERRIDE_COUNT > 0
PATH_B_OVERRIDE_COUNT > 0
MULTITHREAD_EXPAND_ATTEMPT ... trigger_seen=1
MULTITHREAD_ENUM_RESULT ... entries_total>1 owner_pid_matches>1
MULTITHREAD_UP_WATCH ... expand_attempted=1 process_threads_seen>1 eligible>0 installed>1
MULTITHREAD_UP_WATCH_RESTORE ... cleanup_pending=0
```

Strong success additionally captures UNKNOWN_WRITER / THIRD_WRITER_EVENT and THIRD_WRITER_IDENTITY; these are not required to prove enumeration itself. No event does not prove absence, especially with foreign slots, failed access, cap, drops or later-created threads. complete_thread_coverage concerns eligible snapshot threads only.

If no attempt: read MULTITHREAD_EXPAND_SCHEDULED and final MULTITHREAD_EXPAND_STATE stop_reason with requested/attempted/completed/failed. API failures require MULTITHREAD_ENUM_RESULT or bounded MULTITHREAD_UP_WATCH_SKIP stage and win32_error_code. Never infer Toolhelp failure from zero counters.

First test: same wider near-A planar wall, one left-to-right WallRun approximately1.5–2s. It measured N398≈(-0.113317,0,+0.993559). Report flicker/crash/position behavior and return the complete log privately. E is needed only when coverage is established but the unknown writer remains uncaptured on the control wall.

Still blocked: third-path modification, adhesion, movement, animation, camera and later milestones. Retention still requires dot(ActorUp,N398)>=.98 for>=300ms OR>=20 distinct native updates after both A/B commits. c4 does not claim to fix visible flicker or retained ownership.
