# c45 — owned DR6 status lifecycle, no new retirement acceptance

2026-10-27 JST. Base192ff50, branch codex/v01c20-edi-value-provenance.
Runtime remains v0.1c38/package0.1.56; test-only observations, no game release.

## Accepted c44 native result and current primary documentation

The owner supplied GenuineIntel native execution with admitted[0,0,256],3
acknowledged contexts/pending0/canRetire true, inactive leases after false poll,
Dr0-Dr3 exact, Dr6 saved0/current0xFFFF0FF0 and Dr7 saved0x10/current0x410.
All3 final readbacks were stable. RAW_EXACT and c44 ARCHITECTURAL_CANONICAL
both failed; remaining nonfixed Dr6 delta0x10800 is11/16. Preserve this FAIL.

Canonical accepted extraction: analysis/V01C44_NATIVE_WINDOWS_RESULT.md, SHA256
52ef030b7b73050003c4ddb7d61b6f781efadbd35ae6574c8a3d67ccecbf441d.
The source/analysis file is an owner-authored compact summary, not an identical
mirror; it is also preserved unchanged, SHA256
10b7b40e6949c63c7de3775f4456700b9a9ff66384fcdf1cb22ebcb2c209e028.

**Current primary reference accepted through the owner handoff:** Intel SDM
Vol3B,253669-090US, February2026,20.2.3. The owner independently retrieved the
official Intel-hosted manual after c44 publication and supplied these semantics:

- DR6 reports status sampled at the last debug exception, not breakpoint enables.
- BLD11 is active-low for OS bus-lock-detection #DB; other debug exceptions do
  not modify it. Intel recommends handlers return with11 set to avoid stale cause.
- RTM16 is active-low only for advanced RTM debugging; other debug exceptions
  set it. Handlers should leave/set16 on return.

This is the best available CURRENT Intel evidence, superseding c44's older
mirror-based Intel lead. It is not replaced by the old June2021 edition here.
Cloud independent attempts at the Intel SDM landing page and candidate Intel
CDN253669 paths still returned proxy403. PDF bytes/hash were NOT obtained in this
environment; manual PDF SHA256 is UNAVAILABLE, not invented. The extraction hash
above identifies the accepted repository text, NOT the official PDF. Exact
independent PDF delivery/authentication remains unperformed; no private vendor
PDF is copied into the repository or artifact.

The previously checked vendor definition of advanced RTM debug requires both
Dr7.RTM and IA32_DEBUGCTL.RTM. For this fixture every measured Dr7.RTM is checked
as0, so the conjunction cannot be enabled regardless of the unmeasured MSR. No
privileged MSR read or setting change is introduced. A new native timeline is
still needed to apply the current status contract to the exact Windows views.

## Unchanged production ownership/restore audit

**VERIFIED-STATIC, current source/src/msm2/orientation_trace.cpp:**

- own() checks exact Dr0/Dr1 target addresses and their Dr7 enable/type/length
  fields. Dr6[11,16] does not enable a watch or establish slot ownership.
- The hardware handler derives the owned claim from Dr6 B0/B1, preserves
  unrelated causes and clears only claimed owned bits at its end.
- restore() restores owned address/Dr7 fields. Its existing snapshot path may
  assign old6 only when both slots are owned and foreign cause bits match;
  otherwise it restores/clears only the owned B0/B1 portion. No c45 edit changes it.
- Consequently configuration restoration and status equality are distinct
  questions, but this static fact alone is not a new retirement certificate.

The unchanged owned assembly executes one aligned plain32bit MOV store, no LOCK
or XBEGIN/XEND, then the exact exported post-store label. A real SINGLE_STEP
exception at that label, on the bound worker with exactly one matching owned
lease, B0/B1 claimed and no B2/B3/BD/BS/BT, is recorded as the ordinary owned
data-DB observation. c41's current-code/context/flags/ticket checks remain.
This is not a game event or a deduction that a pre-event API zero means a prior
hardware bus-lock cause actually occurred.

## Bounded phase observations c45-v1

Eleven immutable slots contain only Dr0/1/2/3/6/7, observed worker ID, sequence,
origin and explicit11/16 values. Complete evidence requires all slots, strictly
increasing sequences, the same worker, correct API/exception origins and no
duplicate/observer failure. No unknown slot is filled with a fabricated value.

| Ordered phase | Exact observation point |
| --- | --- |
| baseline_pre_foreign | Existing suspended-worker GetThreadContext before foreign Dr2 setup |
| post_foreign_set_readback | One extra GetThreadContext immediately after existing foreign Set, still suspended/before Resume |
| post_foreign_resume_saved | Original subsequent saved-snapshot readback; isolates a possible Resume/API-view gap |
| post_watch_set_readback_pre_resume | COPY the existing successful production watch-install GetThreadContext readback, before its Resume |
| outer_db_handler_entry | COPY supplied exception CONTEXT before the original production handler |
| outer_db_handler_after_engine | COPY the same CONTEXT immediately after that handler, before redirect/nested test exceptions |
| post_return_pre_final_poll | Original suspended-worker sample after exact outer witness acknowledgment/pending0 |
| after_final_poll | Existing post-poll suspended-worker readback |
| verification_readback_1/2/3 | Existing three suspended-worker stability samples |

SetThreadContext/GetThreadContext wrappers exist only while compiling the owned
probe translation unit that includes unchanged production source. They call the
real API exactly once, preserve its result/LastError and merely observe the
selected worker's one watch-install write/readback. No extra Set or extra watch
readback is inserted. Data capture uses fixed storage and lock-free atomics;
there is no formatting/allocation/user lock in the suspended/handler observation.
Actual observation counts and errors are emitted. Failure/duplicate poisons
evidence instead of changing a production result or repairing state.

The only additional ordinary-path context read is the foreign-set sample. The
cap negative also receives a post-return observation but still never final-polls
or certifies live-thread restoration. It retains partial7-stage evidence and
terminal-thread-only teardown. c41 deadlines/caps/lease/ABI/foreign-DR/lifetime
checks, the production handler and raw/c44 comparison helpers remain unchanged.

## Transition classification and limits

For each bit independently, emit every observed adjacent transition and its
before/after value, earliest changed stage and count. A complete ordered single
transition may classify as SETTHREADCONTEXT_ROUNDTRIP (foreign/watch sample),
HARDWARE_DB_ENTRY, HANDLER_EDIT or OS_RETURN_POST_CONTEXT. Fully unchanged data
is explicitly UNCHANGED. Missing/misordered/wrong-thread/wrong-origin observations,
multiple changes or an unisolated foreign-Resume/final-poll interval classify
UNRESOLVED_OR_MULTIPLE. The earliest observed stage remains visible even when
the whole history cannot support a unique transition.

These are first OBSERVED boundaries between Windows API snapshots and saved
exception contexts. They are not direct hardware-register sampling, atomic
history or proof that a specific API/CPU internally caused the change. The extra
observation changes timing; output retains origin and gaps rather than guessing.

**OWNED-WINE measured trace, not native Windows:** all10 non-cap runs observed
Dr6=0 through baseline/foreign/resume/watch samples; DB entry=0xFFFF0FF1;
after-engine/post-return=0xFFFF0FF0; final-poll/three readbacks=0. Thus both11/16
first differ at outer_db_handler_entry, then change back at after_final_poll.
The classifier correctly reports2 transitions and UNRESOLVED_OR_MULTIPLE.
For Position/multiple, Dr7 was0 ->0x10 ->0x10 ->0x990015 during the watch/context/
post-return ->0x10 after poll/readbacks; foreign Dr2 remained preserved.
This confirms observation placement and the unchanged production snapshot path
on Wine. It does NOT establish when native Windows changes or canonicalizes DR6.
Native c44 ended at1/1, unlike Wine's final0/0; no parity is invented.

## No RETIREMENT_STATUS_SAFE acceptance added

RAW_EXACT and c44 ARCHITECTURAL_CANONICAL, their values/revisions and acceptance
semantics are retained. The c45 native path additionally requires complete
ordered evidence and unique per-bit transition classification before the old
c44 acceptance; unresolved native observations fail closed. Wine keeps the
original scoped RAW_EXACT regression, never an architectural/native certificate.

The pure lifecycle helper audits NECESSARY conditions for a future proof:
Intel/admitted policy, ordinary owned DB/engine claim, Dr7.RTM0, acknowledged
debt/poll/lease/canRetire gates, exact foreign snapshot identity/addresses,
c44 Dr7 contract, zero owned/foreign causes after poll, required fixed/unknown
groups, post-event BLD/RTM1, unique supported transition and3 raw-stable readbacks.
Its positive synthetic model is explicitly only necessary_conditions_only;
RETIREMENT_STATUS_SAFE_added is always false and the audit cannot grant acceptance.

Why stop there: actual Wine has multiple transitions and ends with status0, while
native phase data is missing. Current Intel documentation plus supplied c44
stable endpoints does not yet identify Windows's exact API/context-view lifecycle.
Enabling a status-based alternative now would promote a model to native proof.
No such comparator, blanket status mask, production restore/ownership change,
slot watch, producer traversal or gameplay write is added.

## Validation / publication

Focused command: `bash source/tools/test_dr6_lifecycle.sh` (sanitized pure
classification/necessary-condition negatives and strict clearly synthetic JSON).
All task-required commands, actual counts, initial parser schema synchronization
and unperformed native/PDF checks are in CODEX_RESULT/V01C45_STATUS.json.
The real-output validator independently checks all phase values/order/origins,
per-bit transitions and raw/c44/readback consistency for12 actual processes.
Unknown/non-Intel vendors refuse before arming; no AMD certificate is created.

After all owned tests pass, publish only analysis/native-probes/c45 with exact
tested binary/source hashes, README and licenses. One owner native command from
that folder remains:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Return compact output only. A continued native FAIL is expected if c44 still
rejects status changes; it nevertheless exposes the native first-change phases
needed for the next review. No Update-WS/game/Overstrike/A-E/WallRun/scan/log or
mitigation-setting change is needed. Historical c41-c44 artifacts/results remain.
