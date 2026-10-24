# c20 gate

**ACTUAL-GAME DIAGNOSTIC PASS / LOCAL EDI PROVENANCE RESOLVED / WHOLE EFFECT STILL BLOCKED.**

Accepted actual-game evidence: `analysis/V01C20_RUNTIME_RESULT.md`.

## Accepted c20 value result

For the authenticated exact target build/callee, the c20 local EDI slice at:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
```

resolved to one exact local reaching definition:

```text
producer_rva=0x2B2356F
producer_kind=memory_load
origin_kind=memory_load
root=16 = analyzer globalRoot / exact RIP-relative executable-image address
offset=0x0C323788
reaching_definition_count=1
all_reaching_states_examined=1
all_reaching_states_same_value_origin=1
value_provenance_exact=1
```

This closes the c19 question "where does EDI come from locally?" for the modeled authenticated call input. It does not assign semantic meaning to the image-global value, EDI, or `Transform+0x10C`.

## Whole-effect gate remains closed

The EDI value result is not the writer contract:

```text
all_writer_call_states_examined=0
relevant_transform_effects_resolved=0
transform_position_protected_range_unmodified=0
source_span_no_write_proven=0
XYZ/W causal proof incomplete
```

Current exact blocker:

```text
0x2B23578
earlier_read_provenance_dependency
reason=exact_transform_write_may_change_a_decoded_read_carrier
```

This dependency site occurs before the exact write at `0x2B2358E`. The next task must classify the exact read address/value provenance and read-before-write/order/value-flow relationship. Do not simply declare it irrelevant due to ordering, and do not declare it causal merely because the conservative gate stops there.

## Runtime safety regression

Accepted c20 run retained the existing orientation/lifecycle safety baseline:

```text
session1: ~4.22 s, verified Deinit, retention pass, Up dot surface=1,
          one 103 ms stale suspend/recovery, hard0, Position writes0
session2: ~4.39 s, verified Deinit, retention pass, Up dot surface=1.00000012,
          stale0, hard0, Position writes0
```

Both sessions retained 8 true entry snapshots / 8 complete maps; Position watch cleanup restored with no pending cleanup/drops.

## Safety state

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Producer `0x351EB00` and later movement/adhesion/camera/animation/physics work remain deferred.

The next bounded task is in `docs/CODEX_NEXT.md`.