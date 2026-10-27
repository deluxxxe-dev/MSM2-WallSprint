# c54 offline direct-callee never-written preservation

2026-10-27 JST. Basee4ed9f2d3cc4616e00e10a7acc2d6a840382573e,
branch codex/v01c20-edi-value-provenance.

**CALLEE_PRESERVATION_SLOT_SCANNER_READY**. Repository tooling/test result only.
Actual c54 image scan is UNAVAILABLE here, not a zero-writer result. Runtime
remains v0.1c38/package0.1.56. No production/watch/VCH/retirement/gameplay change,
slot value/pointer chase, mitigation change or game release. C52 FAIL is binding.

## Accepted evidence / compatibility

Read AGENTS/manual/rules/workspace/state/reporting/NEXT, c38/c53 evidence and
tools/tests, static_access_v2, c52 gate and Wisdoms static-analysis/VCH references.
Continue only current branch337f953->e4ed9f2, no WS main merge/reset/force push.

V01C53_OWNER_OFFLINE_RESULT.md is accepted VERIFIED-STATIC, not runtime proof:
exact SHA authenticated,0 local candidates, separate accepted c28 reader,
527161 metadata functions,174498 considered/65455 modeled, instruction cap5M,
352663 functions unconsidered and unresolved memory560919. New dominant reason
call_clobber_all_address_origins324714 is dated c53 input, not a c54 measurement.
All writer/publication/ownership/base/lifetime/containment/continuation gates
remain false. Preserve both supplied owner-result copies and all old reports.

New slot_publication_callee.py is schema2/toolv0.1c54. The c38 tool is unchanged.
C53 gains only optional internal per-CALL summary/context callbacks; its CLI,
schema1/default all-clobber rule remain available. Fourteen owned pre-change c53
default result hashes, including branches/loops/CALL/loads/refusals, are pinned
in fixtures/c53_cfg_compatibility.json. Existing c38/c53 test files are unchanged;
the new tests additionally compare those14 snapshots and a CALL-bearing owned
PE's c53 default0 candidates against c54's safely derived reader/writer.
No historical c53 owner output is overwritten or relabeled.

## Summary admission / register contract

The image is frozen as bytes and accepted only after the same exact SHA and
Capstone5.0.6 guards. A target must equal ONE validated runtime entry, whose
complete span is within one readable executable nonwritable file-backed section.
No containing-prefix guess/no-unwind fallback or indirect target recovery.
The producer0x351EB00's full excluded runtime span (or existing whole executable
section fallback) is refused BEFORE loading/hashing/decoding any callee bytes.

For each admitted domain:

1. Strictly decode its entire bounded canonical byte partition without gaps or
   resynchronization, including unreachable bytes. Bind image identity, entry,
   end, complete code SHA256 and the normal_RET_never_written_per_GPR_v1 contract.
2. Enumerate all normal reachable direct branches/fallthroughs. CALL fallthrough
   requires a separately admitted exact child summary. RET must be a bare normal
   RET; stack-adjusting/far/system returns, indirect transfers, middle entries,
   unsupported/out-of-span edges or unknown effects refuse. Duplicate target/
   fallthrough edges are normalized before the DAG proof.
3. Reject EVERY reachable control-flow cycle using a topological/DAG check. No
   loop count or SCC preservation assumption. Every reachable sink must be normal
   RET and at least one RET must exist; therefore each modeled normal path is
   finite and terminates there. Recursion in the direct-call summary graph also
   refuses; partial active summaries are never accepted as a fixed point.
4. Inspect every reachable instruction's GPR write superset. Only explicitly
   admitted integer/selected vector/control effects are supported; other opcodes
   and failed register-effect queries refuse. Capstone writes are combined with
   known explicit destination/implicit RSP/RBP/LOOP and XCHG effects. Partial or
   conditional writes count. Indexed/segment/unsupported address-size forms in
   callees refuse rather than widening the provenance language.
5. Union writes across ALL reachable paths and child summaries. Only a non-RSP
   parent with NO write anywhere is PRESERVED. Any write, including write then
   restore, same-value overwrite or PUSH/POP restoration, refuses that register.
   Nested unknown/refused summaries conservatively refuse the parent domain;
   an admitted child that writes RCX kills RCX while other independently never-
   written registers can remain preserved. No Windows x64 callee-saved convention.

This is structural normal-call/normal-RET analysis, not proof of stack balance,
RET destination values, exception/fault freedom, EH/alternate entries or runtime
code integrity. No stack/memory/return values are interpreted. An instruction
may load or store memory while its GPR effects are audited, but no loaded value
is followed or converted into an address fact. Understanding arithmetic writes
is NOT an arithmetic/value-transform summary; written registers are killed.

## Cache, recursion and finite resources

Actual cache keys contain immutable image identity, exact entry/end, complete
callee code SHA256 and contract. The domain holds immutable file buffers and
metadata. No imported cache/CLI override exists. Bodies cannot change under a
cached proof; a new image/changed callee produces a different identity. Owned
fixtures copy their inputs so mutating a source dictionary cannot replace a body.

An active-target set rejects direct/self/mutual recursion. A cached positive
summary carries its full transitive height; reuse at depth d is allowed only
when d+height-1 fits the limit. A shallow cache cannot bypass a deeper callsite's
depth cap. Refused entries remain refused; no optimistic recursive component.

| Summary resource | Limit |
| --- | ---: |
| Active transitive depth |8|
| Distinct loaded functions/cache bodies |2048|
| Total callee body bytes |8MiB|
| Total summary work credits |1000000|
| Bytes per summary |16384|
| Canonical instructions / reachable states per summary |512 /512|
| Blocks / unique edges per summary |256 /1024|
| Bounded refusal / caller-site examples |64 /128|

Work credits charge every cache/metadata lookup request, body-byte load/hash work BEFORE loading and each decode,
graph node/edge, DAG node and effect inspection. No failed cap can load an
unbounded body. Per-summary/depth failure refuses the affected proof. A global
work/byte/function cap sets sticky exhausted state; subsequent requests, even
cached requests, refuse. Already completed earlier proofs are not retroactively
claimed invalid, but no new request can use the exhausted budget as permission.
Evidence/cache/diagnostic storage is bounded separately; no function bytes are
exported. Summary requests are bounded by the caller CFG work and summary work.

Main c53 scan caps remain separate and unchanged (including5M main instructions).
Output reports summary work plus combined main+summary instruction/code-byte
totals, so extra decoding is not hidden under the old5M counter.

## Caller integration / output evidence

Summary resolution is lazy: request only when a reachable CALL has an existing
exact non-RSP incoming fact. A reached must state can only lose facts; querying
does not create one. The per-call proof mask is fixed after its first resolution.
Only incoming facts explicitly covered by the ADMITTED mask survive. RSP always
dies, even if present in a malformed mask. No callee-local/return-register address,
constant, stack value or loaded pointer is imported. Origin/derivation chains
are unchanged by preservation; separate per-region call-proof digests bind why
the same fact may cross CALL. Caller merges remain must intersections.

The standalone emits one compact JSON record and a compact output file. It keeps
all c53 candidate access/overlap/section/runtime/CFG fields and false completeness
gates, adding:

- callee_summaries_attempted/admitted/refused and refusal reasons;
- nested_summaries_used, summary_cycles_refused, cache/work/byte counters/caps;
- call_edges_with_any_preserved_origin and preserved_origin_register_facts_across_calls;
- call_clobber_all_address_origins_remaining and dated c53 baseline324714;
- per-summary image/entry/span/code SHA/identity, preserved-register results,
  child identities, structural counts/height or nulls when unproved, refusal reason;
- bounded caller-site and unread-refusal examples (no fake code hash when unread).

Attempted counts non-cache requests, including refused requests; admitted/refused
counts partition them. Nested use counts successfully admitted child-call proofs;
caller transfer counts are recorded only after complete caller fixed points.
Summary proof attempts in a later-refused caller region are not caller success.
Coverage order/work may differ from c53; the same operand-reason baseline is a
comparison input, not permission to subtract counts into a completeness claim.
No current c54 real-image impact is asserted here.

## Owned validation / limits / owner action

Focused command: python3 source/tools/test_slot_publication_callee.py, also
mandatory in hooks. Tests cover leaf/branch/multiple-RET effects, caller must
joins, nested child preservation/writes, both recursion kinds/control cycles,
indirect/system/gap/middle/span refusal, every per-summary/global cap, sticky
budget/depth-cache guard, RSP and write/restore refusal, precise reader/writer/
overlap, loaded/local-return origin refusal, producer exclusion before body load,
separate anchor/no-origin lazy path, immutable cache, unsupported effects/address
forms, duplicate edges,14 c53 snapshots, owned PE and exact-guard compact CLI.
Synthetic hash substitution is only in owned Python tests, never a CLI mode.

Real exact-image c54 scan is UNAVAILABLE: no private executable in this workspace.
After the first full green chain, work metering was extended to cache/metadata
requests too, and XADD's two explicit destinations were conservatively included.
The final complete Required validation chain was rerun; preliminary repeats are
not added to final case counts. No old assertion/guard/cap was relaxed.
Accepted c53 owner evidence is sufficient to choose this task, not to invent
new counts. After tested commit/push verification, fetch the intended _source
branch normally, then run exactly ONE offline command with the existing pinned
WSL environment (replace the documented placeholder with the actual game root):

```bash
/workspace/toolchains/analysis-python/bin/python3 "<game-in-WSL>/Mods Library/Mods-Developing/MSM2-WS/_source/source/tools/slot_publication_callee.py" "<game-in-WSL>/Spider-Man2.exe" --output "<game-in-WSL>/Mods Library/Mods-Developing/MSM2-WS/logs/MSM2-WS-v0.1c54-slot-publication-callee.json"
```

Return only compact JSON/file, no EXE upload. No Update-WS/game/Overstrike launch,
new runtime log, hardware watch or mitigation change. All c38/c53 historical
results, c39-c52 retirement evidence and deferred gameplay/ownership/write gates
remain unchanged. Required command results, hashes and publication are in c54
status and CODEX_RESULT; no new game ZIP/script/native probe is published.
