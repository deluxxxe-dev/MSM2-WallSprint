# c48 production continuation-witness integration review

2026-10-27 JST. Base95a32e4, branch codex/v01c20-edi-value-provenance.

**PRODUCTION_WITNESS_INTEGRATION_REVIEW_FAIL**. This is a completed fail-closed
review, not production integration. No production restore/ownership/install/
handler/lease/cleanup/equality change, new watch, gameplay write or game release.
Runtime remains v0.1c38/package0.1.56. No new native probe or owner action.

## Accepted c47 proof is bounded, preserved and not generalized

Owner-native schema7 c47 at da624eef72535b5d27751317b4b467e88d32a383 reported
OWNED_WITNESS_CONFIGURATION_PASS, native Windows/GenuineIntel/Position/multiple,
admitted actual[0,0,256], three acknowledged current-code tickets, pending0,
canRetire true, false poll/inactive leases, exact Dr0-Dr3/foreign Dr2 and stable
readbacks. Exactly two context writes (watch+original final restore), no extra/
unscoped/failed write. RAW_EXACT=false/c44 canonical=false/configuration=true
remain separate. Production_integrated/exact_slot_watch_enabled/game_used=false.
Both accepted V01C47_NATIVE_WINDOWS_RESULT.md copies have SHA256
960ea34e5d487bf192cb85ce309e4476de29cd6aed926eb0ea5bd04dfc5e51e9.

This proves only the c47-v1 activated-state fixture contract and known cooperative
worker/ticket/storage flow. It supplies neither all-path production registration
nor a generic DR6 ignore rule. c39/c40 production resurrection remains authoritative.

## First exact missing guarantee — modified CONTINUE_SEARCH contexts

Production orientation_trace.cpp:152-211 has four early SEARCH exits and a
shared terminal SEARCH/EXECUTION decision. At211 a real owned event can have B0
claimed/cleared while foreign B2 remains. The handler returns SEARCH with owned
Dr0/Dr1 configuration still present in its modified saved CONTEXT. A subsequent
owner can consume the remaining event and return it later, after coordinator
cleanup has restored CURRENT registers and inactive lease bookkeeping.

The c41-c47 test wrapper calls the engine first and registers tickets only if
its result is CONTINUE_EXECUTION. A SEARCH result explicitly poisons/refuses the fixture and returns without
publishing a ticket; it never certifies that path. Copying only its EXECUTION-only
ticket eligibility without the sticky refusal would drop this future obligation.
Keeping the refusal is honest fail-closed behavior, but does not supply a complete
positive production registration/lifetime contract. Every relevant saved context must be accounted for
BEFORE it can escape into another handler/return path, not just after WS decides
it fully consumed a hardware event. Inflight0/ring0/cleanup0 cannot substitute.

**OWNED-WINE execution, no production/CONTEXT edits by observers:** new
production_witness_review_smoke.cpp includes the unmodified production source.
A test observer preserves the engine's result, and a separately registered
following foreign owner holds its real hardware event before returning EXECUTION.
One aligned owned store overlaps an owned Dr0 watch and a synthetic foreign Dr2
watch: real B0+B2. Production claims only B0, leaves B2 and returns SEARCH. The
observer asserts that only the engine's existing B0 clearing changed the six DR
fields; observer/downstream code never writes CONTEXT/DR/RIP/stack or lease flags.

Across reset/Position x close/deadline/kill (six cases): engine SEARCH, inflight0,
one owned captured hit, saved context still owns both slots; current registers
restore exactly in Dr0-Dr3/Dr7 while return is held; lease bookkeeping becomes
inactive; then saved return re-enables owned Dr0/Dr1 with bookkeeping still
inactive. Synthetic foreign Dr2/control stays preserved. No second watched hit,
repair write or production ticket model is added. Worker termination is awaited
before freeing owned memory. This is a new SEARCH-path counterexample in addition
to, not a replacement for, the original c39/c40 EXECUTION-path counterexample.

Two controls trigger foreign B2 only. The engine's early mask0 SEARCH leaves the
context unchanged; on this Wine path current slots do NOT resurrect after return.
This narrower observed non-resurrection is retained explicitly, not relabeled
danger or a general Windows guarantee. An initial exploratory test incorrectly
expected resurrection on that unchanged path and failed; diagnostic values showed
own0. The final harness retains those two negative observations and tests the
separate modified/mixed SEARCH path. No context was edited to force the result.

No new OS probe is needed: the exact coverage defect is established from current
source plus owned tests and pre-existing c39/c40 proof. Native production
frequency and arbitrary OS/third-party dispatch behavior remain UNKNOWN.

## Production install / publication / identity inventory

All references below are source-relative line anchors in the audited base. Source
SHA256 values are pinned in V01C48_STATUS.json; no native game bytes are embedded.

| Stage | Current ordering / identity | Review implication |
| --- | --- | --- |
| Window admission315-333 | capacity/active leases/debugger gates; immutable window id/state/session/reset generation; publish count, capture/reset index | No reservation of future context credit or executable continuation space before arming |
| Multithread install246-278 | validate PID/handle, Suspend, Get; capture old slots; release-publish tid BEFORE Set, Get/readback, ownership check, Resume | Useful immutable lease publication; incomplete rollback/resume retains lease; not saved-context accounting |
| Legacy install335-360 | Open/PID/Suspend/Get; append lease/window/up/old registers; publish tid before Set/readback; retain failed suspension | Same missing return-debt reservation; installation readback is current state only |
| Expansion280-314 /765 | bounded Toolhelp snapshot128, excludes coordinator, filtered PID and per-thread installs | Snapshot-only coverage, no guarantee for all later threads/interleavings |
| Lease key42-44 /158 | numeric tid + append-only lease index/window id/up; handle retained only while current install/suspension active | No context sequence or stable thread creation epoch; old tombstones retain tid after handle close |
| Configure728-736 | exact-build gate, debugger gate, VEH registration, capacities | No actual-process mitigation admission or per-context witness pool |

Leases/windows are append-only for this module lifetime (16 windows,128 thread
snapshot cap,2064 lease slots). These are not bounds on exception nesting or total
returns from one armed live thread. TID recycling cannot be solved by a numeric
thread id alone. A mixed context can represent two different lease obligations.

## Complete textual production VEH return-path inventory

This table covers every explicit return and every branch reaching211 in this
handler. It does NOT assert coverage of earlier third-party handlers that consume
an event without invoking WS, kernel-only context applications or arbitrary OS
exception dispatch. Those are additional admission/coverage unknowns.

At211 claimed B0/B1 are cleared, inflight decremented; residual B0-B3/BD/BS/BT
select SEARCH, otherwise EXECUTION. Every C-row below has both possible terminal
outcomes subject to its residual cause. A diagnostic drop must never be a future
debt drop. No current production row creates any continuation ticket.

| ID / source | Path and mutation | Identity available / required registration boundary |
| --- | --- | --- |
| S0 153 | null record/context or non-SINGLE_STEP ->SEARCH | Null has no usable context; valid unrelated exceptions may still carry owned DR config. Exclusion/coverage guarantee missing before this guard |
| S1 154 | debug context flags missing ->SEARCH | tid/context may exist but DR participation unknown; cannot manufacture registers or zero debt |
| S2 154 | Dr6&B0/B1=0 ->SEARCH | can still have owned Dr0/1 plus foreign/other cause. No lease lookup/inflight. Two Wine B2-only controls do not prove universal safety |
| S3 159 | no owned lease matched ->SEARCH | no unique owned obligation; preserve foreign state, but stale epoch/unknown identity cannot silently discharge a previously registered domain |
| C0 161 | mixed up/lease identities; close/kill, restore each matching lease | tid plus all matched obligations, not just selected first lease. One immutable context identity must bind the full set BEFORE restore/pass-through |
| C1 162 | partial ownership, killed, closed, bad clock, deadline; restore owned subset | selected lease/window available; immutable ticket/unknown poison needed even on closed/tombstone hits |
| C2 165 | Position cap256; close/restore | same lease/window; event cap is not context debt capacity |
| C3 166-168 | event budget: multithread drops but stays armed; other modes close/restore | valid lease while diagnostic suppressed; debt still required for each return |
| C4 169 | eventLock contention: Position/nonmultithread close/restore; multithread drop/retain | nested/reentrant handler can land here; registration cannot depend on acquiring eventLock |
| C5 172 | full diagnostic ring1024: same restore-or-retain policy | ring reservation/drop cannot serve as no-drop debt ledger |
| C6 196-204 | unchanged verified-dual Up write suppressed | no event/hit but valid context can retain owned slots; must register independently of event emission |
| C7 174-202 | normal captured event/read/unwind/lineage path | lease/window known; nested guarded memory fault or another exception can occur while inflight/eventLock held |
| C8 203 | reset candidate closes capture/restores in saved context | restore subset/foreign preservation must remain; publication must precede context mutation/final branch |

A complete design would need registration at an earlier authenticated context-
entry boundary and reconciliation with every SEARCH-chain continuation, including
multi-obligation identities. No supported no-drop boundary covering all such
production contexts is established in this review. This is not proof that every
SEARCH context resurrects, or that all possible future designs are impossible.

## Cleanup / lifetime / scheduling inventory

| Path | Current behavior | Missing witness/accounting guarantee |
| --- | --- | --- |
| release220-238 | stop capture; resume retained suspension, inspect signaled handle; suspend live foreign thread, Get/restore/Set, Resume, close handle on success | current DR restoration and handle close do not join saved returns; first demonstrated blocker |
| resume213-217 | retry3; actual signaled thread ends suspension debt; otherwise kill/retain handle | invalid handle is not exit proof. No speculative ticket retirement on error |
| activeLeases239 / begin317 | installed/suspended booleans only; next begin refuses while true | false ignores saved-context/stub-PC debt; c39/c40 and new SEARCH test demonstrate |
| tick638-645 | closed/lifecycle/deadline/kill releases; may later admit another window | no atomic context-registration-versus-cleanup handoff |
| pollReset758-767 | invalid/kill/clock/deadline closes; release/current=-1 and false return | false return is not saved-CONTEXT retirement |
| cancelReset769-770 / kill636 | close capture/cancel writers, no exception-return join | timeout/kill never acknowledges a context |
| thread exit225 /216 | actual SYNCHRONIZE handle signaled can terminate that thread's future execution | new ledger must retain stable handle/epoch and account other readers/obligations; invalid handle/timeout is not signaled exit |
| mixed ownership161 /229 | restore only still-owned slots, preserve foreign configuration | unresolved/stale/multiple identity must poison or bind full obligation set, never silently drop |
| Deinit/lifecycle / telemetry171-178 | stop components,3 cleanup passes, release worker module reference with FreeLibraryAndExitThread | no ticket/stub/VEH quiescence join or process-lifetime module pin |
| script_disable204-208 | kill; wait telemetry worker | joining telemetry worker does not join all watched threads/exception callbacks |
| DllMain210-213 | no waiting/work under loader lock | correct restriction; it does not solve unresolved external callback/code lifetime |

MS documents that a VEH whose DLL unloads remains registered and may cause errors.
The source does not remove this VEH or use GET_MODULE_HANDLE_EX_FLAG_PIN; the
FROM_ADDRESS reference at telemetry196 is released by its worker at178. The log
"VEH_tombstones_resident_until_process_exit" is an intention, not a demonstrated
host-loader lifetime contract. Whether the actual loader always keeps another
reference is UNKNOWN. No real unload crash experiment or unload-policy change
is performed or claimed.

## Minimum candidate contract — required, not established or integrated

A future proposal must define a closed, bounded domain before allowing the first
watch and satisfy ALL requirements, not just add a counter at handler exit:

1. Stable thread handle/creation epoch, immutable lease/window/session/reset
   generation, per-context monotonic sequence without wrap/reuse, and explicit
   multi-lease obligation set. Context A acknowledgment cannot clear B/new epoch.
2. Pre-reserved storage/identity admission before arming. Reserve debt before any
   relevant saved-context mutation/return and before cleanup can observe zero.
   Reserved-but-not-published slots count as debt; atomic release/acquire state
   transitions must cover nested/reentrant producers and cleanup races.
3. A finite per-thread/process context-credit proof, not event/ring limits. On
   cap/publication/identity failure, a durable sticky unknown-debt poison blocks
   certification/re-arm and retains reachable storage. It cannot be logged/dropped,
   hidden as0 or discharged on timeout/kill. This is an honest necessary fallback,
   not a demonstrated bounded positive retirement mechanism.
4. No heap allocation, loader interaction, formatting/logging, user locks or
   unbounded work in exception registration. Do not use the droppable eventLock/
   ring as the ledger. Every reachable early/terminal SEARCH and EXECUTION path,
   mixed obligation and nested fault needs a proved coverage/admission rule.
5. Publish immutable prepared code/target/token binding before redirect. Keep
   pending until the specific post-return marker is observed; duplicate/stale/
   wrong-thread/lease/generation/token observations cannot discharge another.
6. **Marker acknowledgment is not code-exit/reclamation.** The15byte stub stores
   marker THEN jumps. A thread may be paused between those instructions; its PC
   and later nested saved contexts can still reference code/data. Existing
   freeStorage() requires actual worker termination, not just canRetire(). Any
   live-thread reuse scheme needs an additional proof, not marker0 reset/ABA.
7. True thread exit may end that thread's execution debt with its retained kernel
   handle, but metadata/code readers and any other obligations still need joins.
   Unresolved live debt through kill/deadline/deinit must block re-arm and release;
   module-unload policy/host residency must be explicit and independently proved.

No unique all-path registration boundary, bounded total lifetime/credit proof or
safe normal live-thread stub reuse is established. Therefore no unrelated abstract
positive queue model is added merely to produce green model tests. This review
specifies necessary states (Free ->Reserved ->Published ->WitnessObserved ->
ExecutionQuiescent/ThreadExit ->Reclaimable plus sticky Unknown) and invariants,
not a verified deployable accounting algorithm.

## Executable continuation feasibility — still not production-ready

Current c41 prepare() runs on a coordinator while the cooperative owned handler
waits. It tries256 nearby VirtualAlloc locations,8192bytes RW, places the code
page RX/marker page RW, FlushInstructionCache, verifies exact MOV+[RIP] token +
direct rel32 JMP15bytes/target code/current page identity, then releases the
handler. It is not allocation-free ready-to-run production machinery.

Both marker displacement and target JMP require signed32bit reach from the exact
instruction end with checked overflow. The known fixture has one module RIP;
arbitrary production image/DLL/JIT return RIPs and available nearby address space
are not proven covered. A refusal after context escape cannot simply discard
its debt. Preallocated immutable near pools, capacities, target/current-code
stability and publication outside VEH need a new supported design.

MOV/JMP uses no GPR/RFLAGS/RSP/stack/vector state in the stub; old fixture verifies
GPR/RFLAGS/RSP/active stack/YMM0-15/FXSAVE. This is not arbitrary production
XSTATE/AVX512/AMX/unwind/CET/EH-continuation proof. Interruption/nesting inside
stub and its code-exit lifetime remain additional obligations. W^X/cache flush
and immutable RX publication are necessary, not sufficient.

A future admission design must query the ACTUAL game process through known
GetProcessMitigationPolicy queries for DynamicCode/CFG/UserShadowStack and any
additional relevant EH-continuation/context-restoration policy. Reuse unchanged
c42 unknown/reserved/relevant-bit refusal only within its verified API scope,
require supported CPU/XSTATE/OS and code/target/lifetime proofs BEFORE arming.
Query failure/unknown/new protection/unsupported feature refuses; no policy
setter/disabling or assumed equality with standalone[0,0,256]. The exact c47
pre-access/tuple predicate is not transferred to uncooperative game threads.
The current game process was not queried/launched and no admission is claimed.

## Production-shape coverage matrix / why no review PASS

| Shape | Evidence here | Remaining production requirement |
| --- | --- | --- |
| one context/one thread | c39/c40 normal/held returns and new mixed/foreign actual events | complete registration before escape |
| same-thread multiple/nested | c41-c47 controlled1..3 tickets, identity/cap negatives | arbitrary reentrancy/nesting bound and all SEARCH paths |
| independent multithreads | existing watch/expansion regressions; immutable lease table audit | no-drop per-context ledger across threads, not supplied by event counters |
| close/deadline/kill delayed return | old EXECUTION counterexample plus six new SEARCH reproductions | never certify while registered OR unknown debt exists |
| stale gen/cap/publication/token | c41/c47 fixture negatives; proposed invariants above | production thread epoch/mixed obligations/credit/publication design |
| thread exit0/unresolved debt | current signaled-handle branch, c41 terminal-only storage release | general exit/readers lifetime proof; no forced game-thread termination test |
| module/deinit outstanding debt | source/module API audit | host residency, callback quiescence and immutable stub lifetime proof |
| all debt discharged | owned native c47 PASS | all-domain registration, not just known tickets |

This is review FAIL despite successful counterexample/regression observations.
Full positive production-shape acceptance tests are NOT IMPLEMENTED because a
coherent complete contract is not established. The new harness exercises exact
unmodified production branches and reports its limitations explicitly; it is not
an unrelated abstract model or a production integration certificate.

## Validation / handoff

New focused command: bash source/tools/test_production_witness_review.sh.
Eight actual owned Wine processes plus strict JSON validation yield9 PASS
observations: six mixed SEARCH resurrection cases and two foreign-only controls.
Required c39-c47 regressions, initial exploratory failures, counts and commands
are in CODEX_RESULT/status. Original tests/assertions/caps/deadlines remain intact.
New foreign handler/observer code exists only in the test executable, never DLL.
No analysis/native-probes/c48 is created: another owner game-free run would not
supply the missing production registration/credit/lifetime design. No owner game,
Update-WS, log, slot scan or standalone command is requested. Future work requires
an explicitly bounded design addressing the first blocker; do not implement
production/slot changes merely because the review itself finished successfully.
