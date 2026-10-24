# WallSprint v0.1c20 actual-game result

Verdict: **Diagnostic PASS / EDI local value provenance resolved; next exact read-dependency blocker identified.**

Actual log: `MSM2-WS-v0.1c20-20261025-002043.log` (private user log; do not commit raw log).

## Build / admission
- WallSprint v0.1c20 / package 0.1.38
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- target build gate accepted
- canonical workspace log path under `Mods Library/Mods-Developing/MSM2-WS/logs`
- shared log fallback disabled

## A/E runtime safety regression

```text
session1: WallRun 4219 ms (verified hook duration 4.214959 s)
  verified Deinit=1
  source active until Deinit=1
  retention pass=1
  final actor Up dot surface=1
  stale suspend/resume=1/1, max stale 103 ms, recovered
  hard kills=0
  Position writes by MOD=0
  Position watch cleanup restored=1, pending=0, dropped=0

session2: WallRun 4390 ms (verified hook duration 4.392723 s)
  verified Deinit=1
  source active until Deinit=1
  retention pass=1
  final actor Up dot surface=1.00000012
  stale suspend/resume=0/0
  hard kills=0
  Position writes by MOD=0
  Position watch cleanup restored=1, pending=0, dropped=0
```

Both sessions finalized 8 true writer-entry snapshots and 8 complete invocation maps. No gameplay Position/source/input/movement write permission is implied by this regression.

## c20 EDI result

The c19 blocker at:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
```

now has an exact bounded local EDI reaching-definition result for the analyzed authenticated call input:

```text
producer_rva=0x2B2356F
producer_kind=memory_load
origin_kind=memory_load
root=16
image-relative offset=204617608 decimal = 0x0C323788
reaching_definition_count=1
all_reaching_states_examined=1
all_reaching_states_same_value_origin=1
value_provenance_exact=1
local_paths=1
instruction_visits=15
```

In the c20 analyzer, root `16` is the dedicated `globalRoot`: an exact RIP-relative address inside the authenticated executable image. It is not GPR16 and must not be assigned a game semantic name merely from this result.

Therefore the prior statement "EDI value provenance unknown" is superseded for the local c20 slice. The exact semantic meaning of the image-global value and of `Transform+0x10C` remains **UNKNOWN**.

## Writer-effect result / next blocker

Local EDI value proof did **not** complete the whole callee/writer-effect proof:

```text
all_writer_call_states_examined=0
relevant_transform_effects_resolved=0
transform_position_protected_range_unmodified=0
source_span_no_write_proven=0
XYZ/W writer proof=not complete
```

The exact next dependency site is:

```text
0x2B23578
```

Current analyzer classification:

```text
earlier_read_provenance_dependency
reason=exact_transform_write_may_change_a_decoded_read_carrier
```

The dependency is encountered before the exact write at `0x2B2358E`. The current conservative model therefore needs a narrower proof of that read's exact address/value provenance and its same-invocation ordering/value-flow relationship before deciding whether the native `Transform+0x10C` write is actually relevant to downstream XYZ/W effects.

Do not simply mark the write irrelevant because the read occurs earlier; cross-invocation/state persistence and returned carrier effects have not been excluded. Conversely, do not claim the read is causal merely because the current conservative gate stops there.

## Safety / ownership gate remains closed

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Producer `0x351EB00` remains deferred.

## Exact next edge

Next bounded diagnostic should classify only the dependency at `0x2B23578` and the relevant value/order flow around the authenticated `0x2B2358E` write. It should distinguish:

1. exact read address/root/offset/width;
2. whether the read aliases `Transform+0x10C` or another carrier;
3. read-before-write ordering on every relevant local path;
4. whether the pre-write read can influence post-callee writer XYZ/W state;
5. whether a prior invocation's `+0x10C` write can feed the next invocation's read, if that distinction is necessary;
6. the next precise blocker or completion of protected-effect/XYZ/W proof.

No producer traversal and no movement implementation are authorized by c20.