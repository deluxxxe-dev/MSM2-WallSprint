# c36 — exact store escape summary; destination containment unproven

## Accepted evidence and bounded answer

Basecad6bd8, codex/v01c20-edi-value-provenance; no WS main merge.
VERIFIED-RUNTIME V01C35_RUNTIME_RESULT.md: VMOVUPS0x2CF7FFB is6byte VEX3.256
opcode11, m256[R8+RAX] <-YMM0, scale1/disp0,32byte write/no read/RMW, no GPR/
vector/SP/flags writes. R8tags0/RAXtags7, sourceYMM0 low/high7/above256zero,
flags7. Address indexed/unproven, alias may0x7F. Input method unspecified; do
not infer device equivalence. Store is existing game code, not a MOD write.

VERIFIED-STATIC conclusion from the existing abstraction: current evidence
cannot exclude code/control-stack/critical-storage alias. RAX comes from opaque
writable .data qword, explicitly NOT a pointer; MOVSXD/SHL output R8 has unknown
non-pointer origin even with tags0. Dependency tags prove neither numerical
address/offset bounds, object identity, allocation, lifetime, ownership nor
non-overlap. Current code snapshots/PE/runtime checks do not certify a future
unknown destination; successful sessions/clean second JA leg cannot either.

**Do not continue CFG.** Even a static fallthrough or RET may use code/control
storage affected by that unresolved game-side store. No pending-write model can
restore control soundness without a destination containment proof. Memory escape
must be explicit despite target admission refusal; c35 escape0 was not no-escape
proof. This negative result, not an invented next opcode, is the authorized task.

## Exact current-code guard / summary

Only this site's full VEX3.256 opcode11 form,6bytes, exact memory32 address
baseR8/indexRAX/scale1/disp0/no segment/addr64, sourceYMM0 width32, encoding-
resolved memory WRITE/source READ, no read/RMW/modifiers/masks/other register/
vector/SP/flags writes and complete decoder no-register-write effect matches.
Keep the Capstone store-direction conflict diagnostic; never require its known
incorrect memory READ metadata to mean a real load. No different site/form
receives this summary. Original c35 conditional load and all c19-c35 rules remain.

Current tags are converted from entry-carrier masks before summary, without
mutating architectural state. Summary is of a POTENTIAL reachable game store,
not observed execution or numerical taint:

```text
memory_write_seen=1
memory_write_width=32
memory_address_tags=7
memory_value_low128_tags=7
memory_value_high128_tags=7
memory_alias_may_mask=127
memory_escape=1
first_escape_rva=0x2CF7FFB
pending_unknown_write=1
store_destination_contained=0
CFG_blocked=1
continuation_allowed=0
later_memory_facts_usable=0
memory_bytes_available=0
```

Critical alias mask1023 is a separate may-upper-bound:
code1/runtime-control2/return-control-stack4/caller-home8/private-stack16/Hero32/
field10C64/writer-source128/Position-related256/other-later-memory512. These
possible categories do not assert actual overlap with any/all of them.
Tag0 on R8 does not narrow them. Value low/high tags are independent from address
tags; no source data, addresses or vector values are read/logged or pointer-chased.

Before parent propagation require current stable authenticated code plus known
current tags. Existing c30 source/target recheck suffices when present; otherwise
one bounded current-code equality recheck is required. Changed snapshot/source
metadata or absent tags cannot validate the summary. Diagnostic presence, valid
proof, pending memory effect and actual executed writes remain distinct.

## Parent propagation / merges / no memory model

New STORE_EFFECT record plus parent READ/CALL memory_escape1/first_escape while
identity and flow stay refused. A callee's early identity failure cannot hide a
known potential store merely because modeled_states remains0. Parent first-
escape uses exact recorded site rather than a later barrier placeholder.

mergeReadStoreEffect is used by current-parent replay: union seen/escape/pending/
critical and alias masks/address/value tags/path masks, preserve first escape,
intersect proof/containment/continuation facts when both summaries contain a
write. Empty/clean state is the union identity, never an eraser. Thus a clean
branch cannot certify away the store. Both c27 JA outcomes remain mandatory;
one already-unsafe fallthrough suffices for refusal, without pretending the
unvisited clean taken leg proves a complete2path normal-RET result.

The summary changes no GPR/vector/flag/stack/memory architecture. RAX7/R8zero,
YMM0 halves7/abovezero and flags7 remain. It stores no bytes into Slice/ReadFlow
memory slots and grants no later read/load semantics. All later potentially
aliasing memory accesses, familiar Hero reads, apparently disjoint static forms
or even clean RET remain unreachable because control containment already fails.
No positive disjoint case is fabricated: no nonempty range can currently be
proved disjoint against this all-alias destination. If future evidence removes
control alias, a separately authorized memory state/disjointness rule would be
needed before continuation; this task adds none.

Original128byte/32unique/64visit/1JA/2path and4096cumulative/current-code/runtime/
all-return caps remain. No new game call, hook, raw ABI, native assertion/deadline
relaxation, generic store/AVX/memory semantics, or producer0x351EB00 traversal.
All writable-source/caller-owned/lifetime/exclusivity/conflict and Position/source/
input/movement/adhesion/camera/animation/physics write gates remain0. SF/ordinary
mods/shared environment and updater behavior unchanged.

## Validation and minimum next evidence

8 new groups: identity104/read-call21. Exact form/half tags/component provenance,
valid parent escape, unchanged architecture, clean/store union both orderings,
no continuation to later reads/writes/RET, wrong site/form/width/direction/source/
index/scale/mode refusal, changed code/no tags/full527161 table and c19-c35/native
regressions. Full build/regression, independent extracted source build/focused
suite/package checks/hashes in CODEX_RESULT and V01C36_RELEASE.json.

No extra raw log/function/values are needed to derive the current hard blocker.
Minimum NEW proof for future containment: authenticated non-value provenance/
publication and bounded region/offset/lifetime evidence for the opaque RAX slot
and derived R8 offset, excluding current code/runtime/control stack and all
tracked Hero/source/Position/other-memory ranges. A readable/writable page or
stable numeric pointer alone is insufficient; do not guess ABI/ownership or
traverse the deferred producer. Designing that new observation/provenance class
requires a future bounded task, not an ad-hoc live-value read in this one.

A shortest new A~3s/end/move/E~3s log can verify c36 summary propagation and
unchanged safety only. Repeating the same capture/tag upper bounds does NOT solve
containment or justify advancing. Return STORE_EFFECT/parent escape/state/CFG/
safety records if validating the new build; no video/raw values required.
