# v0.1c55 owner offline result

Evidence class: **VERIFIED-STATIC**. Exact authenticated on-disk `Spider-Man2.exe`; no game/runtime execution.

## Accepted result

Input artifact: `MSM2-WS-v0.1c55-slot-publication-direct-entry.json`.

- tool/schema: `v0.1c55` / `3`
- repository result: `DIRECT_CALL_ENTRY_SLOT_SCANNER_READY`
- exact image authenticated: true
- image SHA256: `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- c54 runtime-entry summaries: 1936 attempted / 345 admitted / 1591 refused
- c55 metadata-free direct-entry summaries: 947 attempted / 512 admitted / 435 refused
- direct-entry admission rate on considered unique metadata-free targets: 54.1%
- metadata relation requests: 22273 exact runtime entry; 23193 executable file-backed outside runtime metadata; all other reported relation classes 0
- direct-entry cache hits: 22246
- direct-entry cycles refused: 57
- combined summary work: 1,228,774 / 2,000,000; global summary budget not exhausted
- CALL edges with at least one preserved exact origin: 3286
- preserved origin register facts across CALLs: 3539
- c53 CALL-clobber baseline: 324714
- remaining CALL-clobber unresolved: 301194
- improvement: 23520 fewer, about 7.24%
- local exact-slot candidates/readers/writers/RMW/address-only: all 0

Direct-entry refusal reasons remain fail-closed: direct edge leaves metadata-free RX domain 128, control cycle 57, indirect jump 31, system/unsupported transfer 1, unsupported memory addressing 55, unsupported RET adjustment 111, unsupported instruction effect 52.

## Main-scan coverage remains incomplete

The main CFG sweep still reaches the 5,000,000 instruction global cap. Of 527161 runtime functions, only 174498 are considered and 352663 remain unconsidered. `writer_enumeration_complete=false`, `candidate_sweep_complete=false`, and all publication/allocation/base/lifetime/range/ownership/destination/CFG-continuation/EH-indirect-writer gates remain false.

No local candidate is therefore only an incomplete zero-observation, not evidence that no writer exists.

Current unresolved memory reasons remain dominated by:

- `call_clobber_all_address_origins`: 301194
- `register_base_without_exact_origin`: 140619
- `unsupported_register_mutation`: 62848
- `memory_loaded_base`: 20289
- `cfg_merge_unknown`: 17025
- `indexed_address`: 14775

## Interpretation

C55 validates the metadata-free direct-CALL entry contract on the real pinned image: unlike c54's earlier owner run, real summaries are now admitted and exact caller origins survive selected CALLs without ABI assumptions or inferred function boundaries.

The next largest bounded blocker is no longer merely callee-entry authentication. The scanner still stops after the first 174498 runtime rows because of one global instruction budget, leaving 352663 metadata rows never considered. A deterministic sharded/resumable offline sweep should be evaluated next so every runtime metadata row can be considered under unchanged per-region fail-closed rules, without broadening value semantics, hardware watch, producer traversal, slot-value reads, production integration, or gameplay.

C52 production VCH integration FAIL remains binding. Runtime/package stay unchanged.
