# c57 exact first runtime-boundary refusal diagnostics

2026-10-27 JST. Base7d177ea7c0aec2405c7d8b7069ebf05be921edba,
branch codex/v01c20-edi-value-provenance. **RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY**.
Owned/static tooling proof only; actual c57 distribution is UNKNOWN here. Runtime
v0.1c38/package0.1.56 unchanged; C52 production review FAIL remains binding.

## Accepted c56 facts / immutable scope

V01C56_OWNER_OFFLINE_RESULT.md (both supplied copies) is VERIFIED-STATIC: exact
pinned image,527161/527161 metadata rows/0 unconsidered,4119 shards/no failures,
204770 modeled/322390 refused/1 excluded,16927172 main instructions/69007023
decoded bytes. Local candidates0 and both completeness flags false. Boundary
refusal162472 and indirect_call153992 explain98.16% of refused rows. C56's
932595 CALL-clobber count is shard-local and NOT comparable with c55's301194.

This closes metadata-row omission only, not gaps/refusals/alternate entries/EH/
indirect writers/deferred producer/publication/ownership/lifetime. Original
c38-c56 sources/tests/results and supplied owner evidence remain byte-identical.
No candidate admission, value/summary contract, c28 handling or exclusion changes.

## Baseline and accepted source set

New separate slot_publication_boundary.py, schema1/toolv0.1c57, requires the pinned
frozen image SHA/Capstone5.0.6. Construct the unchanged c56 session/profile. With
--c56-checkpoint-dir, validate its exact manifest/source/metadata/contracts/caps,
complete ordered shards and every row using the original c56 aggregate. The cache
is READ ONLY: missing/stale/corrupt evidence refuses, with no repair/fallback/write.
Without that option, execute unchanged c56 shards in memory; do not rewrite any
historical c56 aggregate/cache. External c28 remains a separate baseline anchor.

Collect exactly the rows whose c56 first/terminal refusal is the literal
unsupported_runtime_boundary. Bind each sorted unique source index to its metadata
row hash, full bounded body SHA and original graph-build work digest. The image
must have527161 metadata rows, exactly162472 boundary rows and accepted zero
candidate counts. No CLI image/count/profile override exists. Owned Python fixture
acceptance/hash seams never become owner admission controls.

The classifier header hashes image/decoder/tool/schema/contract, exact scanner
source hashes, c56 manifest and complete boundary-set identity. Count agreement
alone is insufficient: every supplied source must independently reproduce that
exact first refusal and original work. Each source record is replayed a second
time by the aggregate; wrong/missing/duplicate/order/stale/unknown records poison
it before output publication. Hash seals are local integrity, not third-party
signatures or a new production certificate.

## Exact recovery / edge semantics

For each source row, preflight the original readonly/file-backed/nondeferred
domain, rehash bytes, then repeat c53's full strict canonical partition, min-heap
CFG order, empty-state register-effect query and original successor oracle/caps.
Abort on ANY different earlier refusal, missing instruction boundary, cap, changed
work or missing boundary. Stop at the same first refusing instruction, not a later
exit or every hypothetical exit in the rest of that row. No post-refusal target
is enqueued, no cross-row state is carried, no target code is read.

Recover all modeled successors of that instruction, retaining only targets outside
the row's half-open span. Ordinary fallthrough, unconditional branch, conditional
taken, conditional fallthrough, CALL fallthrough and other supported modeled
successors are distinct. CALL's existing c53 CFG successor is its return site,
NOT its callee. Taken/fallthrough edges at the same RVA remain two semantic edges.

**Owned decoder finding:** Capstone5.0.6 decorates a prefixed unconditional JMP as
"bnd jmp" while preserving the JMP instruction ID. Original c53 tests mnemonic
name=="jmp", so it can add a nonarchitectural synthetic fallthrough. C57 DOES NOT
fix or reclassify the old row outcome. It uses the decoder ID for the genuine
unconditional target and labels that old synthetic successor
other_supported_direct_successor / architectural_successor=false with an explicit
diagnostic note. Separate architectural_boundary_edges_total and
model_only_boundary_edges_total prevent this artifact becoming an architectural
continuation claim. Required boundary_edges_total counts original c53 successors
with that scope explicitly stated. Real-image presence/frequency is UNKNOWN.

Every record binds source row/begin/end/code/instruction RVA/size/SHA/mnemonic,
first refusal/recovery work and exact target metadata. No whole function bytes or
slot qword values are exported. At most two row-external successors exist under
the unchanged oracle. No proximity, nearest-entry or ABI inference is used.

## Target relation / counts / non-admission

Map the unchanged c55 metadata relation to eight explicit classes: exact unique
runtime begin, unique-span interior, multiple-span overlap, metadata-free RX backing,
unbacked executable, nonexec/writable/unmapped, outside image, deferred producer.
Exact begin includes target index/begin/end and exact span overlap/adjacency.
Other relations carry no guessed target row. Point-unique metadata does not imply
nonoverlapping spans; the helper explicitly handles both properties. Malformed
overlapping PE metadata is still rejected by original admission before real scans.

Counters include every row/edge, all six kinds/eight relations, exact-begin/
interior/free-RX kind matrices, invalid target categories and exact adjacency.
Examples cap8 per kind and8 per relation (at most112); zero/example limits do not
affect counts. Generation and independent validation use the same bounded replay;
reported diagnostic work covers VALIDATION only, separate from generation and
historical c56 counters. No single-process c55 count comparison is inferred.

classification_complete=true requires the full exact accepted refusal set and
all count partitions. It means DIAGNOSTIC domain completeness only. Existing c56
candidate outputs/counts/anchor and all false publication/allocation/base/range/
lifetime/ownership/destination/CFG/EH/production/watch/game gates are copied or
kept false. cross_runtime_fact_propagation=false. Ordinary adjacent fallthrough,
interior, free-RX, overlap or even exact entry is never an admitted continuation.

## Tests / owner handoff

Focused command: python3 source/tools/test_slot_publication_boundary.py, also
mandatory in hooks. Owned tests cover every required edge/target class, duplicate
conditional successors, decorated JMP artifact, exact first-node heap order,
identity/work/byte/set/schema/count failures, poisoned aggregation, example caps,
unchanged candidate/gates, readonly cache vs fresh equality and CLI refusal.
Existing scanner/test files remain untouched. Required command results and source
hashes are in V01C57_STATUS.json and CODEX_RESULT.

No legitimate private EXE exists in cloud, so actual c57 counts/distributions are
UNAVAILABLE/null. After verified push, fetch independent _source intended branch,
then run ONE offline classifier command, preferably reusing the owner's existing
c56 .shards directory read-only; return only compact c57 JSON:

```bash
/workspace/toolchains/analysis-python/bin/python3 "<game-in-WSL>/Mods Library/Mods-Developing/MSM2-WS/_source/source/tools/slot_publication_boundary.py" "<game-in-WSL>/Spider-Man2.exe" --c56-checkpoint-dir "<game-in-WSL>/Mods Library/Mods-Developing/MSM2-WS/logs/MSM2-WS-v0.1c56-slot-publication-sharded.shards" --output "<game-in-WSL>/Mods Library/Mods-Developing/MSM2-WS/logs/MSM2-WS-v0.1c57-runtime-boundary.json"
```

Progress is stderr; compact aggregate is one stdout line plus atomic .json output.
Failure exits2/BLOCKED with no publication and preserves old output/cache. Output
cannot be input or lie inside readonly c56 checkpoints. No Update-WS/game/Overstrike,
hardware watch/PAGE_GUARD/new native probe, EXE upload or new runtime log. No
production restore/ownership/handler/lease/cleanup/VCH, mitigation or gameplay edit.
