# c52 production-shape VCH witness integration review

2026-10-27 JST. Branch codex/v01c20-edi-value-provenance, base
e27fbaabafe05995f6f156657a430348cda2875d.

**PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL**. Completed review; no
production integration, watch/restore/ownership/handler/lease/cleanup change,
gameplay write or game release. Runtime v0.1c38/package0.1.56. No owner action.

## Evidence and first missing guarantee

**VERIFIED-RUNTIME, bounded owner native c51:** accepted
V01C51_NATIVE_WINDOWS_RESULT.md reports GenuineIntel/Position/close, actual
admitted policies [0,0,256], WS SEARCH -> foreign EXECUTION -> VCH1, exact final
context and unique worker obligation. Only Rip changed, exact token observed,
pending1->0 only afterward, cleanup plus stable3 readbacks without a second
access/resurrection; machine preservation and terminal-only storage free passed.
RAW_EXACT/c44 remained false, c51 same-event configuration true, c47 profile not
applied. This supersedes pending-native c51 summaries, NOT their historical Wine
UNAVAILABLE records/source/artifact manifests. C39/c40/c48 counterexamples remain.

**First exact missing design guarantee: a supported final-context boundary for
every admitted production saved-context obligation.** At production C7/T0
(orientation_trace.cpp:174-211), a mixed B0+B2 context can leave WS SEARCH with
owned configuration. C51 proves one downstream/VCH/final-Rip chain only. A
production design must authenticate what will ACTUALLY resume, including later
VCHs, before treating the witness as the context's last application. The audited
public API contracts do not grant WS a permanent terminal VCH position, freeze
other registrations, enumerate/authenticate the complete chain, or promise that
all relevant context-restoration paths visit WS VCH. Registering with First=0
is not an exclusive lifetime reservation of the terminal mutation boundary.

If another callback changes original target/DR state or skips/replays the stub,
it may prevent acknowledgment (honest permanent debt), or preserve the marker
route while retaining/reapplying another saved context. A marker alone cannot
certify that unknown additional application. This is a missing guarantee, NOT a
new measured native later-VCH attack/counterexample or a claim Windows always
omits VCH. Neither return constant is documented as a universal VCH-chain seal.

No independently observable supported admission rule for this open production
handler domain is established. A hypothetical cooperative host owning/fixing
every registration and continuation route could supply a different closed
domain, but that is not an authenticated game/loader contract. A one-time chain
probe or module list cannot prove future closure. Safe present admission is
**REFUSE BEFORE ARMING witness-dependent work**; do not pretend the legacy
production engine now implements that refusal. It is unchanged with its known
limitation. Failure is not merely absence of a new production ledger (c52 forbids
integrating one); the proposed positive design itself lacks this boundary.

## Complete current return-path/debt inventory

All anchors are in unchanged source/src/msm2/orientation_trace.cpp. The five
explicit return statements are at153,154(two),159,211. S0-S3/C0-C8 preserve every
c48 S/C row; T0 makes the shared terminal decision explicit. The structured
inventory/status pins source bytes and lexical spans. This is textual/source
coverage, not a guarantee WS sees every application of a saved CONTEXT.

Identity notation K: retained HANDLE plus creation epoch, tid, lease index,
window id/session/reset generation, context+exception identity and monotonic
context sequence; obligation set O contains EACH owned slot/lease incarnation.
Current production has tid/window/up and entry/reset metadata, but no stable
creation epoch/context sequence or continuation ledger. Equal addresses or
global usedLeases are not K. All potentially owned entries require reservation
before mutation/escape and before cleanup can observe zero, independent of the
droppable event ring. The same rule applies in reset and Position modes.

In the table, restore means the exact current73-78 helper: only owned Dr0/Dr1
addresses and their Dr7 fields, plus the existing snapshot/cause policy for
Dr6; foreign Dr2/Dr3/other controls are preserved. T0 additionally clears claimed
B0/B1 in Dr6. Even a C-row using one selected Lease pointer can represent several
matching lease incarnations; none proves O has cardinality1. All such rows need
the complete bounded obligation scan/set, not only the selected pointer.

| Row / lines | Disposition; current saved-DR mutation | Obligations / earliest candidate reservation | Final VCH, target and outstanding lifetime |
| --- | --- | --- | --- |
| S0 /153 | SEARCH; no mutation, invalid pointers or non-single-step | Null has no usable context; valid other exception can carry owned config. Need domain registration before this guard, or proved exclusion | Unknown flags/foreign continuation; cannot assume VCH or prepared target. Retain unknown debt/residency; no invented DR reads |
| S1 /154 | SEARCH; missing debug flags, no mutation | Participation unknown, K/O not inspectable here; pre-admitted thread/domain credit or poison | No authenticated final DR snapshot; VCH observation cannot be presumed |
| S2 /154 | SEARCH; no owned cause B0/B1, no mutation | Config may still be owned; candidate O must be independent of cause bits and event emission | Two c48 B2-only Wine controls did not resurrect; not universal exclusion or VCH coverage |
| S3 /155-159 | SEARCH; no matching lease, no mutation | Could be genuinely foreign, recycled tid or unknown epoch. No identity guessed from tombstones; unknown-domain poison | Foreign path may resume elsewhere; no target/lifetime proof |
| C0 /161 | T0 SEARCH/EXECUTION; restore each owned matching lease and cause bits | Multiple up/lease identities, full O required, not first selected lease. Reserve before first restore | C51 one full lease does not cover this set. No general VCH/pool coverage; outstanding debt blocks retirement |
| C1 /162 | T0 both; restore owned subset for partial/kill/closed/clock/deadline | K plus partial O before restore, including stale/tombstone lease; foreign slots preserved | Timeout/kill does not acknowledge; missing final boundary retains debt |
| C2 /165 | T0 both; Position cap closes/restores | One or multiple compatible obligations; reserve before branch, event256 is not debt capacity | Closed capture does not imply OS return; target/pool unknown |
| C3 /166-168 | T0 both; budget multithread drop retains slots, otherwise restore | Reserve even without captured event; not maxEvents credit | Continuing armed drops still create possible return debt |
| C4 /169 | T0 both; eventLock failure closes/restores or drops/retains | Nested/reentrant entry needs independent pre-reserved credit before taking eventLock | Lock failure cannot lose debt; no user/event lock for registration |
| C5 /171-172 | T0 both; ring-full same restore/drop policy | Ring1024 is diagnostic only; separate K/O/credit | Ring drain cannot certify context retirement |
| C6 /196-204 | T0 both; unchanged dual-Up suppresses event, T0 still clears cause | Same O even when no hit/event; entry reservation still mandatory | No event means no evidence about saved-context escape |
| C7 /174-202 | T0 both; event capture/unwind/guarded reads, T0 cause clearing | Reserve before processing; guarded/nested faults need separate context sequence | Known hit RIP is not final downstream RIP; arbitrary/nested continuation not pre-served |
| C8 /203 | T0 both; reset candidate closes/restores | Reserve before saved restore, capture close must not release credit | No live cleanup while reserved/published debt exists |
| T0 /208-211 | residual B0-B3/BD/BS/BT => SEARCH, otherwise EXECUTION; clears claimed B0/B1 | Publish immutable K/O before either return; reserved-unpublished already counts | EXECUTION still precedes OS apply; SEARCH may be finalized elsewhere; both need final boundary and stub/readers lifetime |

C0's mixed flag detects different up values, not every possible distinct lease
incarnation. Reverse-order lookup and first lease selection do not establish a
unique obligation set; include same-address tombstone/generation alternatives.
C50's unique worker identity is useful only with a stable complete candidate
snapshot; unrelated global threads do not conflict, ambiguous local candidates do.

## Installation, cleanup, deinit and lifetime inventory

| Stage / source lines | Existing behavior | Necessary candidate rule / unresolved part |
| --- | --- | --- |
| configureReset728-736; begin315-333 | build/debugger/cap gates, VEH registration and window publication | Actual process policies, stable handler domain, credits, pools/residency admitted BEFORE either install; none established |
| installSuspended246-261; legacy335-360 | saved registers; release-publish tid before Set/readback/Resume; rollback/suspension retained | Retained epoch handle and immutable O plus reserved credits before arming. TID/current readback alone insufficient |
| expand280-314 | snapshot128, coordinator excluded, no continuous new-thread coverage | Each admitted thread has independent credits/domain; snapshot is not nesting/lifetime proof |
| release220-238 /activeLeases239 | CURRENT DR restore, Resume, handle close; installed/suspended bookkeeping | Serialize registration vs close, join all old saved applications. Never infer debt0 from booleans |
| resume213-217 |3 retries; signaled handle means exit, otherwise retain/kill | WAIT_FAILED/invalid handle/timeout/resume failure is NOT exit; retained matching epoch only |
| tick638-645 /pollReset758-767 | lifecycle/deadline/kill release and false return | Close admission atomically, reserved/published/unknown debt survives; no rearm on false poll |
| kill637 /cancelReset769-770 /stop662 | stop capture, retry cleanup, resident tombstone intent | No token/quiescence proof from kill/deadline/deinit or retry count |
| actual thread exit216/225 | signaled owned HANDLE ends that thread's execution | Does not end other callback/coordinator readers. No tid reuse or fake invalid-handle exit |
| telemetry.cpp171-178/191-208 | telemetry join/reference released via FreeLibraryAndExitThread; FROM_ADDRESS ref, no PIN | Joining coordinator is not joining game threads/callbacks/stub PCs. Need explicit host residency or process-lifetime pin outside callbacks, not implemented |
| telemetry.cpp210-213 DllMain | no waits/work under loader lock | Preserve restriction; no unload wait/loader work inside handlers or DllMain |

## Bounded ledger contract — necessary design, not a working algorithm

States: FREE, RESERVED, PUBLISHED, FINAL_CONTEXT_BOUND, RIP_REDIRECTED,
DEBT_ACKNOWLEDGED, EXECUTION_QUIESCENT_OR_THREAD_EXIT, RECLAIMABLE, UNKNOWN_POISON.
DEBT_ACKNOWLEDGED and code quiescence are orthogonal facts; one linear enum alone
cannot encode readers, poison and thread-exit-before-ack. State transitions need
separate immutable execution/reader ownership and a sticky domain poison latch.

1. Before arming, admit finite thread count T, context credits C per thread,
   obligation slots O per context, immutable code slots S, and nonwrapping token/
   context-sequence capacity for the entire admitted lifetime. Retain handles,
   creation epochs, window/session/generation and full obligation sets. No numeric
   values are chosen here as a fake proof: windows16/threads128/leases2064,
   event256/ring1024 do not bound saved contexts or uncooperative nesting.
2. Registration must linearize before any escape AND against cleanup's close
   gate. FREE->RESERVED uses a preowned slot; RESERVED already counts even before
   PUBLISHED. Use bounded lock-free atomics with proven lock-free target types,
   independent nested slots, release publication/acquire readers. A bounded CAS
   retry failure goes to durable domain UNKNOWN_POISON, not a dropped record.
   No heap/loader/log/user-lock/eventLock/ring work in registration/binding.
3. K includes exact context/exception instance+sequence, not pointer alone
   (stack addresses can recur). O is a bounded immutable set of lease+slot+
   window+session+generation incarnations. Multiple contexts/threads stay
   separate; one context can bind several O members. Unrelated global leases do
   not force failure; multiple possible current matches do. A acknowledges only A.
4. PUBLISHED->FINAL_CONTEXT_BOUND requires authenticated final chain/domain,
   matching K/O and an already-published current immutable stub/target. Failure,
   cap, partial publication, changed/missing/duplicate/reordered VCH or an unknown
   later mutation poisons and retains debt. Do not release it because diagnostics
   dropped or because a handler returned. This binding step is currently unproved.
5. RIP_REDIRECTED requires only Rip changed and no foreign-state edit. Actual
   exact post-return marker+K/O/token may acknowledge only that context. Wrong,
   stale, duplicate, previous epoch/sequence or token wrap/ABA refuses. A failed
   validation can never clear another entry; poison remains sticky across close.
6. Close/deadline/kill/deinit forbids new admission/rearm; it is not evidence of
   acknowledgment. Configuration retirement requires complete closed registration
   domain, no RESERVED/PUBLISHED/unacknowledged or UNKNOWN obligations, plus
   preserved foreign registers and supported configuration comparisons.
7. Marker precedes JMP. Keep immutable RX code/RW marker allocated and unreused
   after acknowledgment until actual retained matching thread handle signals
   exit (the c51 strategy), or a separately proved code-exit protocol exists.
   An interrupted stub can have a nested saved Rip still referencing that code.
   Reader references/hazards must also be quiescent before RECLAIMABLE. Actual
   exit can terminate that thread's unacknowledged execution debt, but cannot
   acknowledge a token or free data with other live readers. UNKNOWN domain
   ambiguity is not cleared by one thread exiting.

Credit exhaustion may honestly retain permanent poison/storage until process
exit; it cannot both reclaim and keep the same lifetime promise. With arbitrary
live game threads, bounded no-reuse slots eventually exhaust and nesting is
unbounded by the available evidence. Refusal before arming is safe; a design
with guaranteed useful bounded recycling/retirement is not established. These
are necessary invariants and proposed refusal semantics, NOT a verified
concurrent production ledger, a selected memory-order implementation, or a
proof that all early/nested registration sites are reachable without faults.

## VCH ordering, coverage and callback removal

Official MicrosoftDocs sources were retrieved 2026-10-27; exact URLs, raw-file
SHA256, sizes and compact contract excerpts are in V01C52_STATUS.json. Learn
delivery itself returned proxy403 for11 pages; official GitHub sources for all11
were available. Initial two lowercase Debug paths returned404; corrected official
paths were read. No unavailable document is presented as retrieved or runtime proof.

- AddVectoredContinueHandler: First nonzero selects first, zero selects last;
  later first registration can replace first. It specifies registration order,
  not permanent exclusive last position, a whole-chain snapshot/lock, downstream
  context ownership or OS post-return acknowledgment. A later tail registration
  is not ruled out by this contract. One c51 VCH1 measurement cannot close it.
- Shared PVECTORED_EXCEPTION_HANDLER docs define EXECUTION/SEARCH values for
  the VEH callback and warn against synchronization/allocation. AddVCH refers
  to that callback type, but those pages do not independently specify a universal
  VCH terminal-mutation/remaining-callback suppression contract for either value.
  Do not infer a supported VCH chain-sealing technique from the shared return
  description or from an undocumented dispatcher implementation.
- Vectored Exception Handling describes debugger first-chance and pre-unwind
  ordering. This is not all restore-route VCH dispatch or final-CONTEXT ownership.
  Earlier foreign VEH/SEH/unwind/context-transfer routes need explicit coverage;
  copying the c51 exact downstream callback is not a production assumption.
- RemoveVectoredContinueHandler documents unregistration and success status,
  NOT an outstanding-callback join/readers grace period. AddVCH/AddVEH warn that
  unloading a DLL leaves its registered handler behind. Removing registrations
  does not discharge saved return debt or release stub PCs. A process-lifetime
  host/module residency policy is a necessary candidate, not present proof.

The absence of a guarantee in these audited contracts is sufficient to refuse
this positive design. It is not a universal theorem that no future supported
Windows mechanism/host contract can supply one. Another bounded native example
would add an observation but would not prove universal chain closure, so no new
owner probe is requested and no later-VCH runtime experiment is claimed.

## Stub availability without a production callback hold

C51 observeEngine/foreign owner/VCH are owned fixture gates. The coordinator
freezes leases, holds real downstream/VCH with2000ms waits, then invokes reviewed
c41 prepare(). Its256 near VirtualAlloc attempts, RW->RX transition, Capstone,
target/page/current-code reads and FlushInstructionCache are outside the callback
but occur while it waits. That hold is NOT a production preparation strategy.

A candidate pool must be allocated/decoded/sealed/published before arming,
outside callback/loader critical paths. For every admitted exact final target R,
choose immutable code C and marker M satisfying checked signed32 displacements
M-(C+10) and R-(C+15), with the old overflow/alignment/byte/page/target guards.
Proximity of M does not prove reachability of R. RX code and separate RW marker
pages must be immutable per outstanding debt, validated and cache-flushed before
release publication; no allocate/Capstone/VirtualProtect/Flush on the binding path.

The final R can change in downstream handlers/VCHs, be in another DLL/JIT, or be
a nested/foreign continuation not known at the watched instruction. A finite
predecoded exact-target pool is not proved to cover that set. Generic per-RIP
patching of live RX slots, W^X toggles in VCH or waiting for coordinator work is
not an acceptable substitute. Bind-time current-code/page checks also require
proof of bounded supported fault-free access and lifetime; c41 current()'s API
reads are not automatically safe production callback machinery.

Candidate admission is ONLY a finite authenticated target/module set with
already-prepared exact-token slots and sufficient credits, stable target/code
residency and guaranteed final-context ownership. No such closed target domain
is established for the game. If final target or capacity changes after debt
reservation, retain sticky poison/debt; never drop it, allocate later inside
VCH, or reuse an acknowledged but live-thread stub. Near allocation succeeding
once in c51 does not establish arbitrary production address-space availability.

## Actual game-process architectural/mitigation/lifetime admission

No game is launched/queried. Standalone [0,0,256] is not a game-process policy
sample. Before any future arm a coherent design would need:

- actual process DynamicCode/CFG/UserShadowStack queries, known supported masks
  and relevant EH/context-IP validation/strict/audit modes, including
  SetContextIpValidation, non-EHCONT/CET binary restrictions and dynamic-target
  API scope; GetProcessMitigationPolicy supported-option mask/OS version and
  DEP/ASLR/image-load/signature/extension/strict-handle policies as applicable
  to the specific pool/loader/handle design. No policy setter or relaxation.
  Unknown/failure/relevant unsupported flags refuse; unchanged c42 classifier
  only proves its three-query standalone scope, not this broader audit.
- exact build/code/module identity and a preserved immutable continuation target;
  safe residency against unload/reload/patching. Debugger attached/foreign DR
  conflicts/partial-ambiguous ownership refuse rather than stealing registers.
- supported CPU/OS/CPUID/OSXSAVE/XCR0, enabled and per-context XSTATE masks/layout/
  feature sizes. The Microsoft XState API distinguishes extended state from a
  fixed CONTEXT copy. C51 GPR/RFLAGS/RSP/active-stack/YMM0-15/FXSAVE checks do not
  prove AVX512/AMX/PKRU/CET or arbitrary future enabled-state preservation.
  A memory MOV/direct JMP touches no GPR/flags/stack/vector by instruction
  semantics, but interruption/fault/unwind/full OS restoration and handler
  instrumentation still need a complete supported preservation proof.
- process-lifetime residency for callback/ledger/stub/marker/target and stable
  thread handles, or an independently proved join/removal/reclamation protocol.
  GetModuleHandleEx(PIN) documents process-lifetime pinning; current code uses
  FROM_ADDRESS and releases that ref. A pin outside callback is a possible
  future host policy only, not c52 code or proof the host accepts it. No waiting
  in DllMain, no deinit-success claim while readers/debt/PCs remain.

## Production-shape matrix and harness decision

The necessary contract is not coherent enough for a positive production-shape
ledger/bridge harness. Per task section6, NO new executable/model bridge is
invented to manufacture positive evidence. A focused static integrity command
checks actual source inventory/proof provenance/historical bytes and packaging;
its PASS means those audit artifacts agree, not design acceptance.
It runs on the full repository checkout with historical native artifacts; the
disabled runtime package carries compact evidence, not all old standalone EXEs.

| Requested shape | Existing evidence / c52 conclusion |
| --- | --- |
| one EXECUTION and mixed SEARCH debt | c41-c47 controlled EXECUTION, c51 native one mixed PASS; all-path VCH boundary unresolved |
| multiple/out-of-order same-thread; multiple threads; nested registration | controlled old fixture negatives/credits; no production concurrent no-drop algorithm or proven bounds |
| multiple lease obligations; unrelated global leases | inventory C0/partial/tombstones; c50 unique current worker semantics retained, full O binding unresolved |
| close/deadline/kill; cap/publication uncertainty | original counterexamples rerun; proposed poison retains debt, positive production discharge not supplied |
| missing/duplicate/reordered VCH/final mismatch/later mutation | c49-c51 bounded negatives remain; later mutation is a specification-level refusal, not a new native test |
| wrong/stale/duplicate token; A must not clear B | old controlled identity tests preserved; no production accounting generalization |
| marker while PC still in stub; exit with/without debt | c51 terminal-only retention accepted; arbitrary readers/nested PC lifetime unresolved |
| deinit/module lifetime and all-ack+quiescence | necessary separate reader/PC joins above, not established by telemetry join |
| bridge absent/disabled | unchanged c39/c40 and c48 six SEARCH counterexamples plus two foreign-only controls rerun |

No c52 native artifact, owner command, game/update/scan/log request. Native c51
PASS remains scoped; review FAIL leaves later production/exact-slot work closed.
Required command results/counts, unperformed reasons, hashes and publication are
in V01C52_STATUS.json and docs/CODEX_RESULT.md. CODEX_NEXT and old c39-c51
evidence/source/tests/artifacts are preserved byte-for-byte.
