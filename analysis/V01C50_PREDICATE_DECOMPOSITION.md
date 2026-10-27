# c50 — mixed SEARCH conjunct and unique-debt identity diagnostics

2026-10-27 JST; base9a24784b71fb3438446d9632f35402ac90b27c02;
branch codex/v01c20-edi-value-provenance. Test-only.

Primary: **MIXED_SEARCH_IDENTITY_DIAGNOSTIC_UNAVAILABLE**. Repository diagnostics,
model and owned Wine coverage are complete; the exact failing conjunct of the
accepted c49 native run is still UNKNOWN until the decomposed native probe runs.
BLOCKED0: no requested bounded diagnostics/test subtask was security-blocked.
No production source/behavior/ownership/restore/handler/lease/cleanup changes,
VCH-to-c41 bridge, RIP redirection, DR repair/normalization, exact-slot or gameplay
write. Runtime stays v0.1c38/package0.1.56; no game release.

## Accepted native input — do not relabel the result

Both supplied V01C49_NATIVE_WINDOWS_RESULT.md copies remain byte-identical,
SHA25630031c6c243c09e5ef85b2dd603222ccd50da9ad588025d76e97ea96c3ee240b.
Native GenuineIntel/Position/close with actual[0,0,256] admitted produced
VCH_SEARCH_CONTINUATION_BRIDGE_FEASIBILITY_FAIL, reason
WS_mixed_SEARCH_identity_or_cause_mismatch. The same run actually observed
WS SEARCH -> foreign EXECUTION -> VCH1 at ordered stages1/2/3/4/5/6/7,
exact downstream-final/VCH CONTEXT, honest pending1 until actual worker exit,
no bridge/RIP/repair and terminal cleanup. Native VCH is therefore observed,
not UNAVAILABLE in that historical run. It does not make the aggregate FAIL a
PASS or prove a witness. Every old native artifact/source/result stays unchanged.

Old JSON did not expose all lease/window conjuncts. A larger global usedLeases
is a **HYPOTHESIS** for that native failure, not a measured answer. This task
does not infer the missing number from other flags, replace the old FAIL, or
request an identical c49 rerun.

## Static audit — global slots versus a matching obligation

VERIFIED-STATIC, unchanged orientation_trace.cpp:

- Lease records contain atomic published tid, immutable window/up/old-state
  metadata and coordinator-owned installed/suspended/handle fields.
- installProcessThread262-280 increments usedLeases after Open/Suspend and before
  the installation outcome. Global slots can include unrelated workers, failed/
  unpublished attempts or retained records; this is not a per-context debt count.
- installSuspended246-261 publishes tid with release before arming/resume. Handler
  own()70-74 checks Dr0/Dr1 addresses and slot-specific Dr7 fields; own() alone
  does not test thread identity. The real handler separately checks tid before
  selecting a matching slot/lease. Same-address watches on different threads
  are not the same saved-context obligation.
- begin Position state5 calls expandProcessWatch synchronously at362. It first
  secures the primary worker, then snapshots/installs eligible other process
  threads while excluding the coordinator. Thus extra global slots are possible
  before the owned worker is released. This makes the hypothesis plausible but
  does not measure the accepted native failing run's counts.
- The old14 conjuncts use own(after,leases[0]) but **windows[0]** id/generation,
  not necessarily the selected lease's window. C50 serializes both separately;
  no hidden substitution of selected-window fields rewrites old semantics.

A context obligation must bind this worker/retained creation epoch/context/
exception/sequence and an unambiguous matching lease/window/generation/up. All
worker leases with any owned bit count as candidates, including partial matches
and tombstones. Two candidates refuse even if one has a full own-mask3. An
unrelated tid does not become this worker's obligation just because own() sees
the same addresses in the worker's supplied CONTEXT. This is a separately named
test predicate, not a production ownership or retirement-rule change.

## Capture and conditional diagnostic policy

New test-only vch_search_identity_smoke.cpp preserves the c49 stage/foreign/VCH
observation shape. After unchanged production begin/expansion finishes, before
go, the coordinator freezes its counts and waits without further lease install/
release until capture. The handler then captures all old conjunction inputs and
every used lease slot into a fixed2064-record global buffer, before any sticky
diagnostic decision. This is the existing table bound, not a bound on saved
contexts/nesting. tid is acquired; immutable window/up and original own(context)
are read only after bounds checks. installed/suspended are read only in this
explicitly serialized owned-coordinator interval; unknown/unstable status refuses.
No production data race/lifetime guarantee is inferred for an arbitrary handler.
No allocation, formatting or diagnostic-ring lock is added to capture.

The pure helper reports:

1. every old14 boolean conjunct and exact values, original aggregate and ordered
   failed-conjunct names, without changing the old c49 implementation;
2. global used count, all captured slots, matching-worker count, matching-worker
   leases with nonzero own count, and all-thread nonzero-own count separately;
3. selected index only if precisely one worker-owned candidate exists; actual
   window/index/generation/up/status and expected tuple are compared explicitly;
4. **UNIQUE_MATCHING_DEBT_IDENTITY**: complete stable capture, unique worker
   candidate, full own3, expected window/index/generation/up, installed/not
   suspended. Unknown/partial/multiple/mismatch refuses;
5. c50_diagnostic_admissible requires that property AND every old conjunct other
   than the global count. The only permitted old failure is exactly the global
   usedLeases==1 conjunct. Additional legacy/ownership/cause conflicts stay FAIL.

The actual native primary may be diagnostic PASS only when THIS new native run
observes the VCH boundary, identifies exactly that sole old global-count failure,
and satisfies the separate named identity property with all other checks intact.
A native run with no reproduced old failure is UNAVAILABLE for identifying that
prior failure. Genuine cause/register/window/generation/ambiguity remains FAIL.
Wine and synthetic truth cannot select native PASS. Even diagnostic PASS is not
a bridge, post-return acknowledgment, retirement or production certificate.

## All decomposed fields — real owned Wine observations

Normal=all6 reset/Position x close/deadline/kill runs; extra=one real Position/close
run adding an explicitly test-owned idle thread before unchanged expansion. That
thread never accesses watched memory. Production creates its unrelated lease;
no lease metadata/flags or reported API result are forged. The normal artifact
owner command does not add this extra thread.

| Field | Normal | Extra owned idle worker |
| --- | --- | --- |
| ws_search_ok | true | true |
| entry_cause_mask / entry_cause_expected |5 /true|5 /true|
| post_engine_cause_mask / post_engine_cause_expected |4 /true|4 /true|
| cause_delta / cause_delta_expected |1 /true|1 /true|
| rip_preserved |true|true|
| Dr0_preserved /Dr1_preserved /Dr2_preserved /Dr3_preserved /Dr7_preserved |all true|all true|
| used_leases_total /records_observed |1 /1|2 /2|
| matching_worker_lease_count |1|1|
| matching_owned_context_lease_count (worker AND nonzero own) |1|1|
| context_owning_lease_count_all_threads |1|2|
| selected_lease_index (zero based) |0|0|
| selected_window_id /expected_window_id |1 /1|1 /1|
| selected_window_generation /expected_generation |149 /149|149 /149|
| selected_own_mask |3|3|
| selected up /expected_up |exact owned allocation offset|exact owned allocation+0x30|
| selected installed /suspended /status known |true /false /true|true /false /true|
| legacy first own mask3 /windows[0] id1 /generation149 |all true|all true|
| global_used_leases_is_one |true|false|
| legacy_c49_aggregate |true|false|
| legacy_failed_conjuncts |empty|global_used_leases_is_one only|
| UNIQUE_MATCHING_DEBT_IDENTITY /c50_diagnostic_admissible |true /true|true /true|
| VCH registered /callbacks |true /0|true /0|
| pending at worker exit |1|1|
| c50 primary |UNAVAILABLE|UNAVAILABLE|

All slot summaries include index/tid/published/window index/id/generation/up/own,
metadata/status availability and installed/suspended flags. In the extra case,
both threads share window/up/own3, but only one is the captured worker. Exact
field/JSON checks are performed by check_vch_search_identity.py; model JSON is
explicitly synthetic/native_execution=false. These observations demonstrate the
distinction in owned Wine, not the missing native subpredicate.

## Debt, lifetime, historical regression and native handoff

Reservation still precedes SEARCH escape. A bad/ambiguous identity reserves
unknown pending debt and poisons; it does not drop the obligation. Exact
downstream-final/VCH CONTEXT bytes/order remain required. No RIP redirect or
post-return witness is implemented. Close/deadline/kill do not clear pending1
or permit legacy live final polling. Actual retained-handle worker termination
(and optional idle-worker normal termination) precedes terminal cleanup/free.
No repair/mask/retry, extra watched access or forced thread termination.

test_vch_search_identity.sh runs12 ASan/UBSan model PASS records,7 actual owned
Wine UNAVAILABLE observations and1 strict JSON PASS. Models cover unrelated vs
unique, duplicate/partial matches, wrong tid/window/generation/up/own, all cause/
preserved-register violations, incomplete/status/cap data and original aggregate
serialization. c49/c48 run separately unchanged, including every historical
counterexample, original aggregate condition and c40 callback0 UNAVAILABLE.

Publish the exact tested observation-only PE/licenses/manifest under native-
probes/c50 after required validation. One owner native command from that folder:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

Return compact JSON including identity_diagnostics and all ordered stages.
Schema2/taskc50 emits every conjunct/lease summary; it does not repeat the opaque
c49 test. No Update-WS/game/Overstrike/scan/game log/mitigation change. Unknown
prerequisites exit77; diagnostic conflict exit3. Exit0 can still be UNAVAILABLE,
not native PASS. All three primary names are MIXED_SEARCH_IDENTITY_DIAGNOSTIC_
PASS/FAIL/UNAVAILABLE. The exact native old failing conjunct remains unmeasured
until that new result is supplied. No c51 bridge work is performed or authorized.
