# c51 native Windows result — one-debt VCH post-return witness bridge PASS

Date: 2026-10-27 JST
Branch: `codex/v01c20-edi-value-provenance`
Probe source/artifact head before owner run: `1aad6e4d6d3d976203781f57778d49f52bc8be55`
Owner command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

## Primary result

```text
result = VCH_POST_RETURN_WITNESS_BRIDGE_FEASIBILITY_PASS
reason = native_exact_one_debt_post_return_token_then_configuration_cleanup
environment = native_Windows
cpu_vendor = GenuineIntel
mode/scenario = Position/close
production_integrated = false
exact_slot_watch_enabled = false
game_used = false
```

The live c42 mitigation tuple was known and admitted: `[0,0,0x100]`. CPUID/leaf7 and AVX/OSXSAVE/XCR0 prerequisites were known/admitted.

## VERIFIED-RUNTIME — native Windows

The native mixed SEARCH continuation chain completed in the required order:

```text
WS SEARCH -> downstream EXECUTION -> VCH -> RIP-only continuation stub
-> exact token acknowledgment -> pending debt 0 -> final cleanup
```

Observed facts:

- VCH registered and callback count = 1.
- downstream-final and pre-redirection VCH context matched exactly.
- c50 identity diagnostics remained admissible: global leases 4, matching worker lease 1, matching owned context lease 1, selected index 0/window 1/generation 149/own mask 3.
- debt identity was thread + creation epoch + context + exception + lease + window + generation + sequence + original RIP.
- reservation preceded SEARCH escape.
- coordinator prepared and published the unchanged reviewed c41 continuation form outside the VCH.
- VCH changed only `Rip`; `VCH_extra_changed_byte_offsets=[]` and no debug-register repair occurred.
- expected witness token `1090519041` was observed for the exact same debt identity.
- `DEBT_ACKNOWLEDGED=true`; pending debt changed 1 -> 0 only after that token.
- cleanup had already been requested while debt was pending; live final cleanup occurred only after acknowledgment.
- no second watched access was required (`second_watched_accesses=0`).
- no owned-slot resurrection was observed after final cleanup.
- machine state preservation passed.

The bridge redirected from original RIP `0x00007ff680b564a9` to test-owned stub RIP `0x00007ff680b30000`. This address is run-specific runtime evidence, not a reusable game RVA.

## Debug configuration result

The original c43 RAW_EXACT and c44 ARCHITECTURAL_CANONICAL comparisons remain false and are not relabeled. The c47 profile was not applied to this different mixed-event shape.

The separately scoped c51 same-event configuration contract passed:

```text
same_event_configuration_restored = true
```

After final poll:

- Dr0 restored exactly to saved 0.
- Dr1 restored exactly to saved 0.
- synthetic foreign Dr2 remained exactly preserved.
- Dr3 remained exact.
- final/readback Dr6 was stable at `0x00000000ffff0ff0`.
- final/readback Dr7 was stable at `0x000000000d000410` while the saved form was `0x000000000d000010`; the known fixed-one representation difference remains separate from raw equality.
- all three verification readbacks were captured and stable.

No DR6 masking/normalization or post-cleanup repair write was added.

## Lifetime boundary

Marker/token acknowledgment and executable-storage lifetime remain distinct.

The test retained stub/marker storage until actual owned worker exit. Only after the retained worker handle signaled exit did the fixture mark execution quiescent/storage reclaimable and free the test-owned storage:

```text
storage_retained_until_worker_exit = true
STUB_EXECUTION_QUIESCENT = true
STORAGE_RECLAIMABLE = true
storage_freed_after_exit = true
```

This does not establish safe live-thread stub reuse/reclamation in production.

## Consequence / limits

C51 closes the bounded native Position/close one-debt feasibility question opened by c48-c50: a pre-reserved mixed-SEARCH debt can be authenticated in the downstream-final VCH, redirected through the reviewed c41 stub, acknowledged after OS continuation, and followed by cleanup without a second watched access or observed resurrection.

It does **not** prove:

- all production return paths are registered without drops;
- nested/multiple debts or arbitrary game threads are covered;
- a bounded process-wide credit/accounting design;
- safe live-thread stub reuse;
- production module/VEH/code lifetime and unload quiescence;
- actual game-process mitigation admission;
- full arbitrary production XSTATE preservation;
- production integration or exact-slot-watch safety.

The c39/c40/c48 counterexamples remain authoritative outside this bounded successful bridge shape. The next task must be a separate production-shape integration review before any production implementation or exact-slot work.