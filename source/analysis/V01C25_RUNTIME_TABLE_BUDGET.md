# c25 bounded large-table validation / explicit coverage

## Accepted c24 evidence and interpretation correction

V01C24_RUNTIME_RESULT.md accepts code-region admission for target0x2CF7FE0,
PE runtime-table entries527161, table_valid0 and combined malformed_or_cap refusal.
The old budget262144 is smaller than that derived count. Thus neither corruption
nor absence of a containing runtime-function entry was established. No target
code/span/hash/effect was admitted; raw GPR0 retains selected Transform+0x10C tag1.

VERIFIED-STATIC correction to the supplied conclusion: c24's callback assignment
out.lookup=lookup(...) occurs only after the combined cap/structural guard and
complete entry loop. At this refusal it was not called. The logged found0 /
same_image1 / readable1 were default RuntimeRegion values, not an observed OS
no-entry result. Preserve the raw measured fields, but do not use them to infer
an absent entry. C25 adds explicit lookup_queried and cap-skipped coverage.

C24 session1 hit the unchanged5second diagnostic limit before verified Deinit;
session2 exited normally after about4.16s, with Position writes0 and retention
pass. The process-cumulative hard counter is not a second new hard-kill episode.
No timeout/assertion is weakened. C25's short capture uses about3s per A/E leg.

## Finite budget derived from directory bytes

The authenticated PE exception directory's RVA/byte size is the source. There
is no independently trusted entry-count field. Check RVA/size zero-pair validity,
in-image range using subtraction (no overflow) and exact12byte divisibility
before applying budgets. Count=size/12 is exact only when divisibility passes.

Hard ceilings:600000 entries and8MiB(8388608bytes). C24 reports the floor
count527161; an aligned directory would be6325932bytes, below both. Its actual
byte size/remainder is absent from compact evidence and must be logged, not assumed. Entry headroom is72839 (about13.8%); the byte
cap is a separate finite envelope. The entry ceiling implies at most7200000
actual table bytes/9375 guarded reads. Keep64entry/768byte stack chunks; no whole
table allocation, memory search, new body capture or repeated entry processing.
These ceilings are explicit, fixture-tested and do not automatically grow from
untrusted input. A size above either budget refuses, without declaring corruption.

Distinct outcomes:

- invalid RVA/size pair, out-of-image/overflow range or nondivisible size;
- metadata_byte_cap_exceeded or entry_cap_exceeded, entry validation skipped;
- exact guarded read failure, with bytes read and entries validated so far;
- unordered/overlapping begin/end, out-of-image range or invalid unwind RVA;
- full structural table validation, then actual OS lookup and comparison.

Every admitted-budget entry is checked with the unchanged c24 ordered/nonoverlap,
in-image range and bounded unwind-RVA rules, including entries after any target
match or far beyond the old cap. A tail defect still blocks lookup/admission.
tableReadComplete/tableValid require the entire loop to succeed. A malformed
prefix is not a claim about readability of unexamined suffix bytes. Structural
validation does not interpret unwind programs or establish game semantics.

## Target status / unchanged strict identity

Only after full table validation does lookup_queried become1 and the OS callback
run. A containing entry must agree with OS lookup and meet the ordinary c24
entry/boundary/snapshot rules. Missing entry in both fully checked sources is
the only route to the old single-target strict no-unwind proof. Defaults, skipped
validation, unreadable chunks and mismatches never become evidence of absence.

The c24 rule after lookup is unchanged: exact0x2CF7FE0, <=128bytes/32instructions,
guarded stable code, single-entry linear modeled register effects through normal
C3 RET, no memory/stack/branch/further CALL/partial effect or runtime-region
crossing. No first-convenient-RET finder, recursion or generic CPU extension.
Only admitted identity feeds c23. Existing c19/c20 read/effect refusal, XYZ/W,
cross-invocation, ownership/lifetime/exclusivity and write permissions stay separate.

## Compact log / proof boundary

MOVEMENT_CALLEE_READ_RUNTIME_TABLE reports directory RVA/bytes, entry width and
derived count, pair/range/divisibility flags, byte/entry budgets and excess flags,
budget admission, cap-skipped state, bytes read/entries validated, read failure,
complete-table validity and whether lookup actually ran. Existing TARGET_IDENTITY
also gains runtime_lookup_queried, and retains target identity/next exact reason.
No table bytes, full function bodies, raw live data or guessed ABI are logged.

Code-region/build/SHA/direct-call gates, c24 guarded snapshot/metadata rechecks,
c23 leaf/effect limits, c19/c20 certificates and native lifecycle/orientation/
thunk ABI/pairing/hardware cleanup/deadlines/assertions remain. No new hooks or
game calls, producer0x351EB00 traversal, Position/source-position/input/movement
writes, data-page mutation or PAGE_GUARD. All ownership/write gates remain0.

## Owned tests / actual next evidence

Six new groups retain the nine c24 and21 c21-c23 groups. Independently authored
sparse PE metadata covers262143,262145,527161 and600000 valid entries; finite
entry/byte excess; pair/divisibility/range wrap errors; late unordered/overlap/
unwind defects; guarded chunk failure; exact containing entry and lookup mismatch.
Each complete-table case checks every entry/count/read coverage, bounded chunks
and no metadata access outside the directory. Failed validation requires zero
lookup/code reads. Validated absence still exercises unchanged c24 strict proof.
These are owned descriptors, not a replay of the private game's actual entries.

Prepared Outcome C: actual table structure, lookup status and next identity/leaf
boundary remain UNKNOWN. The compact c24 count proves budget sufficiency but
cannot prove the527161 actual rows valid. A new RUNTIME_TABLE record is genuinely
needed; no raw table/function dump is requested. Update-WS, Install mods ->
Run modded game, A~3s -> fully end WallRun -> move/fast-travel -> E~3s (under the
unchanged5s cap). Return newest complete private
MSM2-WS/logs/MSM2-WS-v0.1c25-YYYYMMDD-HHMMSS.log. TARGET_IDENTITY/READ_CALL and
existing safety/cleanup in that same log decide any continuation. Final validation,
commits and hashes: CODEX_RESULT/V01C25_RELEASE.json.
