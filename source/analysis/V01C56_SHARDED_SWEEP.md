# c56 deterministic sharded runtime-metadata coverage

2026-10-27 JST. Basec3013eae59803a280f158311758a2cb65ef23fe5,
branch codex/v01c20-edi-value-provenance. **SHARDED_SLOT_PUBLICATION_SCANNER_READY**.
Repository mechanism/owned tests only; current real-image c56 counts are UNKNOWN.
Runtime remainsv0.1c38/package0.1.56; production/gameplay/watch unchanged.

## Accepted c55 input / unchanged semantics

V01C55_OWNER_OFFLINE_RESULT.md (both supplied versions) is VERIFIED-STATIC:
exact image; c54 summaries1936/345/1591; c55 direct947/512/435; cache22246,
cycles57, work1228774/2M/no exhaustion;3286 CALL edges/3539 facts survive.
CALL-clobber324714->301194 (23520/7.24% fewer),0 local candidates, main5M
instruction cap after174498 of527161 rows,352663 unconsidered. This validates
bounded preservation on actual code, not writer absence or completeness.

All c38/c53/c54/c55 scanner sources, tests, accepted owner evidence and historical
statuses remain byte-identical. C56 imports their exact guard/PE parser/CFG scan/
summary engine; no new value/effect/ABI/entry semantics or relaxed cap. C52
production review FAIL and all historical counterexamples remain binding.

## Deterministic scope / finite work

Authenticate the exact frozen file SHA and pinned Capstone5.0.6 BEFORE metadata
or code analysis. Reuse the validated full runtime table in original row order;
each row identity hashes index/begin/end/unwind. The session manifest hashes the
full table plus image/decoder/tool/schema/contracts/profile and exact five scanner
source hashes. A changed file, cap, partition or contract invalidates resume.

Default shards are consecutive disjoint128-row intervals [start,end), the last
shortened.128 times every c53 local maximum fits its unchanged global maximum;
thus no process-wide5M instruction stop can omit later rows. A fresh unchanged
c55 engine exists per shard, sharing only frozen image/metadata with other shards.
Its c54/direct budgets, negative cache, depth and global decode partition operate
unchanged WITHIN that shard. No proof/cache/partition state is merged across shards.

Every metadata row is explicitly MODELED, REFUSED or EXCLUDED. Producer spans
are excluded before loading/decoding; unsafe section/backing/oversize rows refuse.
Strict c53 canonical scan/fixpoint/access classification stays intact. Any observed
summary cap, including a reused cached negative cap result, makes the ENTIRE row
REFUSED with no candidates. This extra conservative c56 exposure rule is stricter
than keeping an otherwise-modeled caller with clobbered origins. No partial-region
candidate survives. A summary budget exhausted earlier in a shard remains closed.

Per-shard c55 caps remain depth8 /2048 entries per engine /8MiB reads per engine /
1M work per engine, all existing per-summary instruction/state/edge/block limits.
Main row caps remain65536 bytes/1024 instructions and existing CFG limits. Shards
never widen these. Two engine maxima are explicit in the profile.128 rows/shard
and600000 metadata rows bound total shards/work. Memory holds one shard/engine,
aggregate counters, at most256 candidates and the bounded interval/hash list.

## Checkpoint / aggregate protocol

Default private cache: output filename with .shards instead of .json. It contains
manifest.json and exact shard-START-END.json files. Writes are compact UTF-8 JSON,
fsync then atomic replace through a uniquely named .c56-*.tmp. Crash leftovers
are ignored as uncommitted temporary files; existing final evidence is never
silently replaced. No source archives or active scripts are created.

Each shard seals its manifest ID, ordinal, interval, exact per-row identity/body
hash/outcome/work/reasons/counts, additive summary counters/report digest, and
bounded candidate records. No whole game function or slot value is stored.
Checkpoint JSON caps8MiB/file and rejects duplicate keys, nonfinite values,
symlinks, malformed schemas/counters, unknown files, stale/orphan evidence.

Aggregation accepts only the exact expected ordinal/interval and manifest ID.
It verifies every row identity/body hash against the authenticated frozen image,
counter caps/partitions and zero candidate/call-success counts on refused rows.
Candidate metadata binds shard/row/runtime span, source instruction hash/decoded
operands/access/width/overlap, exact CFG/must scope/hash and bounded origin chain.
Bad evidence permanently poisons that aggregate object; catching an error cannot
continue with partially added counters. Missing/duplicate/overlap/out-of-order/
out-of-range/wrong-image/source/schema/contract evidence refuses before publication.

Cache seals are integrity checks for the owner's LOCAL checkpoint files, not
cryptographic signatures or independent re-proofs of a third party's arbitrary
forged analysis. Completed local evidence is checked but not reanalysed; corrupt
or stale finals refuse rather than being silently recomputed. Use a distinct
explicit checkpoint directory for an intentionally new incompatible run. A
failed run preserves the previous aggregate and rejected evidence and prints
BLOCKED/aggregate_published=false, exiting2. Success writes one compact aggregate.
Output must be .json outside the checkpoint directory, never the input image.

## Exact sums / independent scopes / false completeness

Accepted row work/reasons/access counts and per-shard summary attempt/admit/refuse/
cache/work counts are additive. Candidate counts include all successfully modeled
rows, even when stored records reach128/shard or256/aggregate. Retained records
are deterministic row/site/operand prefixes and carry shard/row identity. Output
states exact counts and records_not_retained; storage limits never stop row work.

Independent shard caches can repeat the same callee and their global budgets/
partitions reset between invocations. Therefore summary counts are ATTEMPTS in
independent bounded domains, not global unique functions. Whole-row cap refusal
and different cache state can change counts relative to c55's single capped run.
Do not subtract them into a comparable coverage/completeness claim or merge
separate proofs into one production/global certificate.

Main/shard-creation work is separate from current aggregate validation body-hash
reads and candidate decoding. C55 CALL revalidation counts are included explicitly
in main+summary decode totals. C28 anchor validation happens once per session,
outside all shard/local counts. Its attempted/accepted decoder work is separate;
unmatched decode count is UNKNOWN rather than a fabricated zero. External parent
origin never seeds a row or summary. Cached and fresh aggregates are byte-identical.

Only successful full exact coverage reports runtime_rows_covered=N and
runtime_functions_not_considered=0. This is metadata-row consideration, not complete
body/writer analysis: executable gaps, excluded producer, refused regions,
unsupported effects, alternate entries/EH/indirect targets/writers remain open.
candidate_sweep_complete/writer_enumeration_complete and every publication/base/
allocation/range/lifetime/ownership/destination/CFG/EH gate remain false.

## Validation / handoff

Focused: python3 source/tools/test_slot_publication_sharded.py, also mandatory in
hooks. Owned PE tests cover partition/full coverage, malformed/mixed/stale inputs,
all interval failures, exact additive accounting, cap-refused rows, cached negative
caps, bounded records, original c55 behavior, separate anchor, unread slot/producer,
poisoned aggregation, full/partial/crash resume, source/profile identity and CLI.
Initial focused full-coverage test caught an interval cursor shadowed by a code
end RVA; renamed the interval stop independently. No test/cap/guard was relaxed.

Real c56 scan is UNAVAILABLE here: no legitimately supplied exact executable.
After verified commit/push, fetch the independent _source intended branch normally
and run ONE offline command. It performs/resumes all shards with stderr progress,
then returns only the compact JSON/file (keep checkpoints private/local):

```bash
/workspace/toolchains/analysis-python/bin/python3 "<game-in-WSL>/Mods Library/Mods-Developing/MSM2-WS/_source/source/tools/slot_publication_sharded.py" "<game-in-WSL>/Spider-Man2.exe" --output "<game-in-WSL>/Mods Library/Mods-Developing/MSM2-WS/logs/MSM2-WS-v0.1c56-slot-publication-sharded.json"
```

No Update-WS/game/Overstrike/new runtime capture, EXE upload or native probe.
Production restore/ownership/handler/lease/cleanup/VCH, mitigation, hardware watch/
PAGE_GUARD, pointer chase/slot qword/producer traversal and gameplay unchanged.
Required validation/hash/publication evidence is V01C56_STATUS.json/CODEX_RESULT.
