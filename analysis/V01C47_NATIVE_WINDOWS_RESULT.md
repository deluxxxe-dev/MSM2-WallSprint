# c47 native Windows result — bounded configuration retirement PASS

Date: 2026-10-27 JST
Branch: `codex/v01c20-edi-value-provenance`
Published probe source head before this owner run: `da624eef72535b5d27751317b4b467e88d32a383`

## Owner command

```powershell
.\continuation-witness.exe --position --multiple --json
```

No game, Overstrike, Update-WS, slot scan, A/E capture or mitigation-setting change was used.

## VERIFIED-RUNTIME — native Windows

The owner supplied schema7/taskc47 output with:

```text
result                         = OWNED_WITNESS_CONFIGURATION_PASS
environment                    = native_Windows
mode / scenario                = Position / multiple
cpu_vendor                     = GenuineIntel
mitigation_flags               = [0,0,256]
mitigation_classifier          = ADMIT
contexts                       = 3
watch_context_writes_observed  = 1
outer_hardware_DB_events       = 1
RAW_EXACT                      = false
ARCHITECTURAL_CANONICAL        = false
RETIREMENT_CONFIGURATION_SAFE  = true
acceptance_comparison          = RETIREMENT_CONFIGURATION_SAFE
exact_slot_watch_enabled       = false
production_integrated          = false
game_used                      = false
native_candidate_probe_passed  = true
retirement_accepted            = true
```

The new c47 predicate reported:

```text
revision                       = c47-v1
profile                        = Intel_SDM253669_090US_DR6_owner_c46_Position_multiple_v1
synthetic_inputs               = false
native_Windows                 = true
prerequisites_known            = true
cpuid_known / leaf7_known      = true / true
debugger_absent_at_admission   = true
Position_multiple_profile      = true
RETIREMENT_CONFIGURATION_SAFE  = true
reason                         = exact_owner_c46_configuration_retired_status_representation_contract
ticket_capture_complete        = true
ticket_count                   = 3
total_context_write_attempts   = 2
final_poll_context_write_attempts = 1
unscoped_context_write_attempts   = 0
failed_context_write_attempts     = 0
```

All three continuation tickets were on the same worker/lease/generation, were acknowledged, and had `current_code=true`.

Final-poll retirement state remained:

```text
active leases before final poll = true
active leases after final poll  = false
pending before/after final poll = 0 / 0
canRetire before final poll     = true
final poll returned             = false
```

Dr0/Dr1/Dr3 returned to saved values and the synthetic foreign Dr2 was preserved exactly. All three final readbacks were complete and stable. Historical raw comparison remained false because Dr6 was `0x00000000ffff0ff0` versus saved `0`, and historical c44 architectural comparison also remained false. Dr7 final was the already-characterized `0x0000000000000410` versus saved `0x10`.

The c46 lifecycle evidence was reproduced in the c47 output: before Resume after watch installation Dr6[11,16] were `0,0`; after Resume but before watched access / machine entry / ticket / pending debt / owned #DB, both were already `1,1`. The exact full c47 profile matched this measured history; no generic `0x10800` mask was introduced.

## Interpretation

### VERIFIED-RUNTIME

For this exact owned fixture/profile, configuration retirement is accepted by the separate `RETIREMENT_CONFIGURATION_SAFE/c47-v1` predicate even though the older raw and c44 canonical comparisons remain false.

The run confirms the bounded candidate can distinguish breakpoint configuration retirement from the measured DR6 status representation without weakening historical evidence. It also confirms three acknowledged current-code continuation tickets, zero pending debt, inactive leases after the final poll, exact Dr0-Dr3/foreign-Dr2 preservation, and no unscoped/failed context writes.

### VERIFIED-STATIC / task boundary

The c47 implementation is test-only. It explicitly reports `production_integrated=false` and `exact_slot_watch_enabled=false`. The c47 analysis states that a native PASS permits only a later separately authorized production-integration review.

### UNKNOWN / NOT PROVEN

This run does **not** prove:

- process-wide coverage of arbitrary game threads or arbitrary exception interleavings;
- that every production saved exception CONTEXT is registered into the continuation-ticket mechanism;
- production-safe behavior across abrupt thread exit, module teardown/unload, kill/timeout races or all nested/recursive exception patterns;
- a general Windows/Intel DR6 normalization rule outside this exact measured profile;
- safe production integration of the continuation witness;
- permission to enable the deferred exact-slot watch at `module_base + 0xC2D2580`;
- any slot value read, pointer chase, producer traversal or gameplay write.

The original c39/c40 delayed-saved-CONTEXT resurrection counterexample remains authoritative for production until a production-shape integration proof closes those gaps.

## Next boundary

Advance only to a bounded **production continuation-witness integration review**. Do not implement or enable the exact-slot watch yet. The review must prove or fail closed on registration completeness, per-thread/context identity, allocation/handler safety, teardown/lifetime, nested/multiple contexts, thread exit and the original c39/c40 delayed-return counterexamples before any production integration task can be authorized.
