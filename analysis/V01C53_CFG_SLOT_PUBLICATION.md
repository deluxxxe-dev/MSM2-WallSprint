# c53 offline CFG must-address scanner

2026-10-27 JST. Base7559677b4f440a24180e87582cdf38b4456e32b8,
branch codex/v01c20-edi-value-provenance.

Repository result: **CFG_SLOT_PUBLICATION_SCANNER_READY**.
This means bounded owned tooling/tests, not a real-image scan, a discovered
writer or publication proof. The private exact Spider-Man2.exe is unavailable
here. Real-image counts/candidates are UNKNOWN, not zero. C52 production review
FAIL remains unchanged; no VCH integration, hardware watch or gameplay work.
Runtime remains v0.1c38/package0.1.56; no new game release.

## Inputs, unchanged history and scope

Read AGENTS/manual/rules/workspace/state/reporting/task, c38 actual scan evidence,
slot_publication.py/tests, static_access_v2.py/tests, c52 evidence and Wisdoms
static-provenance/VCH references. Current branch fast-forwarded5882ac4->7559677;
no WS main merge/reset/force push. Original five untracked artifacts preserved.

C38 remains schema2/toolv0.1c38 with its unchanged linear/control-boundary reset
semantics and historical results. New source/tools/slot_publication_cfg.py has
separate schema1/toolv0.1c53. It imports unchanged pure c38 decoder/address/PE/
anchor helpers; no extraction or old assertion change. Both scanners require
Capstone5.0.6 and exact file SHA256
c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9.
There is no CLI hash override, synthetic mode, process-memory interface or
loaded-pointer rule. CLI file reads are bounded at512MiB+1; hash and parser work
on file bytes, but the target qword is never interpreted/read as a value or emitted.

The full sorted/nonoverlapping runtime table is validated by the existing
600000-entry/8MiB parser contract. Only runtime spans completely contained in
readable executable nonwritable file-backed sections are admitted. Entry facts
are EMPTY for all16 GPR parents. Image gaps/no-runtime functions, alternate
entries, unwind/EH edges and interprocedural effects are NOT covered by this
normal single-entry analysis. The known no-runtime c28 reader is kept external.

## Strict decoding, graph and convergence contract

1. Exclude the full runtime span containing0x351EB00 before code loading or
   decoding. If no such row exists, retain c38's whole containing executable
   section fallback. If exclusion is unavailable, image analysis refuses.
   Other runtime boundaries never share state; exclusion is explicit in JSON.
2. Establish a complete canonical nonoverlapping instruction partition within
   each bounded span, with no byte resynchronization. Any decode gap, even in
   unreachable bytes, rejects that region. This is deliberately stronger than
   recursively accepting an alternative overlapping instruction stream.
3. From the runtime begin, build reachable edges: ordinary fallthrough;
   direct conditional target AND fallthrough; direct JMP target only; direct
   CALL fallthrough with ALL address facts invalidated, including nonvolatile
   registers. CALL target must be a resolved image address and, in image mode,
   file-backed readable executable nonwritable storage. The target is not decoded
   or traversed; normal return remains conditional, not a callee/ABI summary.
4. RET terminates a path. Indirect CALL/JMP, INT/IRET/system/transactional/
   privileged transfers, unresolved/out-of-image target, crossing runtime span,
   middle-byte target, unavailable register effects or graph cap rejects the
   entire affected region. No earlier provisional candidate survives rejection.
5. Solve a bounded monotone must fixed point over16 facts. UNREACHED differs
   from reached UNKNOWN/empty. A fact is exact image RVA PLUS its ordered bounded
   derivation chain. At each join, retain it only if all reachable incoming
   states agree exactly. Missing/conflicting facts remove it, including a late
   predecessor after earlier propagation. Equal addresses with different copy/
   LEA derivations conservatively disagree. Loop revisits use the same join;
   a mutated/extended-chain loop loses the fact rather than endlessly unfolding.
6. Only after complete graph/fixpoint validation classify candidate operands.
   Ref/candidate cap during emission discards that region's candidates too.
   Earlier fully validated regions may remain reported, with completeness false.

Diagnostic reason labels (load/partial/call/merge/etc.) never introduce facts.
They form a bounded coarse merge view. Merge-discard counts are disagreeing
register-input observations at joins, not unique game locations or c38 totals.
The worklist cap bounds both fact and diagnostic-label propagation.

## Provenance language and access semantics

Only full64 RIP-LEA, full64 register copy, checked full64 base+constant LEA and
constant memory displacement from a proven origin are propagated. Pre-state
source operands are read before destination invalidation. Partial32/16/8 writes,
unsupported arithmetic/shift/immediate pointer materialization invalidate facts.
Memory loads can be candidate reads, but their destination NEVER becomes an
address origin. Indexed/segment/address-size/overflow/outside-image forms refuse.
Direct RIP/absolute instruction address forms remain distinct from register
facts; absolute immediate LEA does not create a register origin.

Each exact overlapping explicit operand reports site/instruction SHA256,
mnemonic/operands, operand and actual access widths, READ/WRITE/RMW/ADDRESS_ONLY,
decoder access metadata plus independent semantic proof, exact effective RVA,
overlap extent, bounded derivation chain, code/effective PE section, runtime
begin/end/unwind metadata, CFG entry/block/predecessors/graph hash and fixed-point
scope. No full body/raw byte dump or target qword is exported.

LEA has access_width0: address-only overlap is the addressed point (extent1),
not a fabricated8byte read. Known scalar and selected legacy/VEX vector moves
use encoded opcode direction to keep stores WRITE even if Capstone says READ.
Whitelisted explicit RMW and read-only CMP/TEST/BT remain separate. Unknown
access semantics, including unadmitted vector maps/forms, are counted as possible
writes and withheld; they are NEVER downgraded to READ from decoder flags alone.
Implicit stack/string/other accesses and unknown writers remain outside complete
enumeration; JSON explicitly keeps implicit_memory_operands_enumerated=false.

## Deterministic caps and accounting

| Limit | Whole scan | Per runtime region |
| --- | ---: | ---: |
| Code file bytes loaded |64MiB|65536|
| Decoded instructions |5000000|1024|
| CFG edges |10000000|2048|
| Reachable states |5000000|1024|
| Worklist iterations |20000000|8192|
| Blocks |2000000|512|
| Runtime regions considered |600000|1 entry|
| Candidate records including external anchor |256|remaining global credits|
| Register provenance steps |8|8|

Code-byte budget charges full loaded spans (including unreachable bytes), not
only successfully decoded bytes. Decoded bytes/instructions and reached states
are separate. The external anchor's7byte/1instruction/record work is charged
against the same aggregate caps; it NEVER seeds a CFG state. No cap is a command
line knob. Owned pure tests may supply smaller caps to exercise refusal.
The explicit anchor-budget flag records its one validation-attempt credit even
if its current form does not match; that conservative charge is not instruction
coverage or execution evidence. A matched external record also has its own
instruction hash/access/overlap/PE/runtime-or-null metadata and non-CFG scope.

JSON separately reports table functions, considered/modeled/refused/excluded/
unconsidered regions, executable file bytes vs runtime metadata bytes, entries,
blocks, all edge types, joins/merge observations/discards, state/loop revisits,
decode totals, unresolved explicit operand categories, unknown possible writers
and stop/cap reasons. Stopped-path counters are the FIRST refusal per affected
region/global stop, not an enumeration of every possible path or hidden opcode.
All candidates of a refused region are withheld; unscanned code stays unproven.

Comparison preserves accepted c38 values control_flow_boundary_loss822382,
runtime_region_boundary_loss208548 and memory_loaded_base85282 as dated inputs.
New previously_control_lost_edges_safely_modeled counts direct CFG edges whose
final successor retains at least one exact agreed origin. Separate counters
cover edges without agreed facts and refused control regions. These are different
units/scopes from c38 operand/reset totals: no subtraction or claimed percentage
improvement is made, and c38 is not silently rerun/reinterpreted. Shared operand
reason categories are comparable only where actually exercised. Real-image new
counts remain UNKNOWN until the owner offline run.

## Evidence / test boundaries

Focused command: python3 source/tools/test_slot_publication_cfg.py. It is also
required inside hooks using the existing pinned analysis virtual environment.
Owned tests cover agreeing/mutating conditional diamonds (including late input),
different provenance at equal addresses, JMP-only flow, stable/mutating/chain-
changing loops, CALL-all-clobber and target-section refusal, indirect/system/
middle/gap/boundary refusal, every resource cap and hop cap, reader/writer/RMW/
address-only/overlap semantics, memory-loaded/partial/unsupported/index/segment
origins, wrong store metadata, unsupported semantics, independent external
anchor, predecode producer exclusion, whole-section fallback, authenticated owned
PE integration/caps/determinism and exact guard/JSON CLI with spaced paths.
Changing only a synthetic slot qword changes only the whole-file hash, not any
scan fact. Unit hash substitution is confined to owned Python fixtures; no CLI
override or owned image is published.

Initial focused execution found a diagnostics counter bug: when the first join
input was unknown and a later input carried a fact, the fact correctly remained
unknown but the discard counter omitted that disagreement. Corrected the counter
to count both input orders and extended the test to both schedules. No candidate
soundness assertion, old test, production code or cap was weakened.
After the first full green chain, external-anchor record metadata was completed
and its7byte exclusion overlap check tightened. All eight Required validation
commands were rerun on that final source; preliminary repeated totals are not
added to final case counts.

Required commands and final observed counts are in CODEX_RESULT/status. Synthetic
success is not real-image evidence. C38 code/tests/historical evidence, C36 hard
store/CFG barrier, C37 offset proof, C39-C52 retirement predicates/counterexamples
and all ownership/gameplay gates remain unchanged. No production source/restore/
handler/lease/cleanup or watch is modified. No image/whole game-function/private
log is added to Git; root game release ZIPs remain unchanged.

## Owner offline handoff after verified publication

Use the existing independent _source checkout and pinned WSL analysis environment.
Fetch latest intended branch first (ordinary git pull --ff-only, NOT Update-WS).
Run exactly one offline command, with the actual game root substituted for the
documented placeholder:

```bash
/workspace/toolchains/analysis-python/bin/python3 "<game-in-WSL>/Mods Library/Mods-Developing/MSM2-WS/_source/source/tools/slot_publication_cfg.py" "<game-in-WSL>/Spider-Man2.exe" --output "<game-in-WSL>/Mods Library/Mods-Developing/MSM2-WS/logs/MSM2-WS-v0.1c53-slot-publication-cfg.json"
```

The CLI writes and prints ONE compact JSON record. Return only that JSON/file;
no EXE upload, game launch, Overstrike, new game log, hardware watch or repeated
movement test. Wrong hash/decoder/input refuses before writing a report.
READY is the repository tool result; writer/publication_source remain unproven
regardless of zero/one/multiple candidates. All writer-enumeration/ownership/
allocation/base/lifetime/containment/CFG-continuation certificates remain false.
