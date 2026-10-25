# Codex result — c32 exact YMM0 load with bounded low/high128 dependencies

Branch **codex/v01c20-edi-value-provenance**; accepted task base **b12d7fe**.
Implementation **e13d4a0b11c1ed32d004d697f5099377aa62778f**. Version **v0.1c32 / package0.1.50**.
Outputs **MSM2-WS-v0.1c32.zip**, **MSM2-WS-v0.1c32-source.zip**,
**MSM2-WS-v0.1c32.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C32_RELEASE.json** (final hashes/sizes/source-snapshot commit).

## Accepted input / bounded result

Read AGENTS and previously read unchanged operational manual/rules/workspace/
logging, latest state/NEXT/compact c31 result, relevant code and refreshed Wisdoms
instructions/manual/WS/engine/method references. Fast-forwarded intended WS branch
only; no WS main merge/reset/force push,5 original untracked artifacts preserved.

VERIFIED-RUNTIME c31: VMOVUPS0x2CF7FF0 is4byte VEX2.256 YMM0 <-m256[RDX], current
same Hero root1+0/address tags0/Hero-only alias, no store/RMW/GPR/SP/flags effect.
C31 correctly refused missing upper-state representation. C30 RAX unknown/top7/
no qword reads, c29 tag0 refusal and c25-c28 prerequisite chain remain accepted.

**Implemented only the requested representation and exact-load continuation.**
Low/high128 dependencies are represented, both become may-top7 without vector
reads, above256 becomes known zero. Every future ZMM/EVEX/>256 use still refuses;
no generic AVX emulator. Exact actual next instruction/full-target completion
remain UNKNOWN: compact c31 evidence stops at this load and cloud has no private
current target body. Minimum new vector-state/next-barrier capture is needed.

## Implementation / changed files

- New owned vector_dependency.hpp:16 low128 XMM alias banks,16 high128 may-tag
  banks (unknown upper input starts top7) and per-parent above256-known-zero
  markers. No physical bits, generic lanes, semantic types or pointer values.
  XMM reads low; YMM dependency reads union halves. Legacy XMM writes preserve
  upper state; only pinnedYmm0Unknown replaces both halves/top7 and zero marker.
- read_target_identity.hpp / rdx_source_contract.hpp: current Hero-source proof,
  upper entry tags/zero facts, pre-barrier low/high state and distinct returned
  high-category summaries/path outcomes. Explicit optional vector256 mode is
  enabled by production; original diagnostic-only mode retained for regression.
- rdx_source_contract.cpp: pin exact site/4bytes/VEX2/VL256/YMM0/m256[RDX], no
  index/displacement/segment/prefix/modifier, pure read, no flags/GPR/SP effects.
  Identity entry evidence requires sampled same-origin Hero root1/offset0/tags0;
  actual c23 walk repeats source check per state. RDX numeric bits are never
  read/interpreted; no data reader observes the32 source bytes.
- Only this load sets both halves7 and opaque unknown low Slice origins, records
  above256 zero, preserves other vector parents/GPR (including RAX7)/flags/SP/
  memory. Known-zero marker does not permit ZMM/EVEX. Other sites, widths,
  destinations, VEX128/YMM forms, stores/RMW/masks/merge/modifiers remain refused.
- Clone both halves per path, retain separate returned low/high category masks,
  union tags and intersect above256 zero knowledge. Legacy low-only zero never
  kills possible selected dependence in high. Symbolic entry-carrier masks are
  converted once to actual categories; upper category state stays a separate
  domain. Current caller conflicts/missing tags refuse, never infer observed taint.
- writer_entry_snapshot.cpp: VECTOR256 / relevant VECTOR_STATE / VECTOR_RETURN
  alongside existing rich classification, UNKNOWN_LOAD, exact barrier/path/tag/
  CFG/safety records. Report modeled effect only after current code/source gates;
  no raw vector/qword data. High and low state immediately before the next
  unsupported instruction are distinct, with path-dependent return masks.
- read_target_identity_smoke.cpp:8 new groups,68 identity total/read-call21;
  original c19-c31/native regressions retained. Updated version/info/README/TEST/
  BUILD/package metadata, accepted c31 mirror, c32 analysis/gate/index and AGENTS/
  state. **docs/CODEX_NEXT.md preserved byte-for-byte**.

Only the exact VMOVUPS is new semantics. Existing proven legacy register rules
continue; no next new instruction is opportunistically modeled. Both JA outcomes,
all modeled normal RETs and final current code/runtime/source checks still gate
target begin/end/hash. Caps unchanged1JA/2paths/128bytes/32unique/64visits, caller/
replay4096; one safe leg does not certify the other or whole writer effects.

## Validation

- Windows x64 full warnings-as-errors build PASS.
- **13 portable +7 DLL/logger +208 hooks/native/contracts/workspace =228 public
  PASS records /5 optional private-input SKIP**, all commands exit0 on first full
  attempt. No native assertion/deadline/cleanup guard relaxed.
- New groups cover full YMM overwrite/low alias/high preservation/parent isolation;
  current-derived exact Hero load/top7/no reads; flags/GPR/SP/memory preservation;
  legacy XMM zero/copy vs unsupported VEX128; wrong source/width/parent/encoding/
  direction/modifier refusal; current caller/conflicting evidence; above256 zero
  with ZMM refusal; exact next barrier/unsafe leg/code mismatch/full527161 table;
  distinct low/high returns and conservative path merge/zero knowledge.
  Existing c31 diagnostic refusal, c30 top/c29 readonly negatives retained.
- Actual provisional source ZIP extracted fresh; independent full build PASS;
  **143 focused entry/c14-c32 +7 DLL/logger +13 workspace =163 separate PASS
  records**, no private skips; standalone package verifier PASS separately. Do not
  add duplicate163 to228. Final report/package-metadata refresh retains identical
  tested native inputs and separately rechecks the current standalone packager.
- Static byte checks preserve NEXT, native lifecycle/orientation/raw ABI/assertions,
  workspace/updater/shared launchers, entire c25 table/lookup/snapshot block,
  c29/c30 source metadata policies and c31 vector classifier. ReadFlow gains only
  the required category state/alias handling; no game-hook ABI/layout changes.
- Final archives verified for CRC/hash/version/new DLL markers, complete current
  sources/docs/evidence/helpers/licenses, one virtual active install script,
  no active source script, compiled/private body or raw runtime/vector payload.
- Known cloud Wine winedevice.exe startup diagnostics occur; final commands exit0.
  Owned fixtures/cloud tests are not actual-game full-target or gameplay PASS.

Wisdoms **3a07fb54092ac5066011af63295b420aaed3492d** pushed: accepted exact c31
form, minimal c32 halves/aliasing/upper unknown initialization, return union vs
zero-knowledge intersection, unsupported wider forms and safety limits. Concurrent
SF updates preserved; append conflict resolved keeping both independent sections.
No private logs, complete proprietary function bodies or vector values published.

## Safety / minimum human evidence

writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement
writes0; producer0x351EB00 deferred. No new hook/game function, PAGE_GUARD/data-page
mutation, ABI guess or gameplay/adhesion/camera/animation/physics change. Existing
orientation source writes remain under unchanged native guards, separate from
denied translation/source-position writes.

May-top7 does not observe taint, kill selected dependence, prove a vector type,
semantic matrix meaning, pointer/constant/value identity or ownership. Even a
complete bounded target is not whole writer XYZ/W/feedback/storage/lifetime/
exclusive-conflict/writable movement-source proof.

After verified push: close game/shared Overstrike, run installed
**MSM2-WS\Update-WS.cmd**; shared UI **Install mods -> Run modded game**.
A/cardinal~3sec -> fully end WallRun -> move/fast-travel -> E/diagonal~3sec,
below unchanged5sec limit. Return newest private
**MSM2-WS\logs\MSM2-WS-v0.1c32-YYYYMMDD-HHMMSS.log**. Minimum new records:
MOVEMENT_CALLEE_READ_VECTOR256 / VECTOR_STATE / VECTOR_RETURN plus existing
next barrier/path/tag/CFG/current identity/safety/cleanup. No vector bytes,
qword/body dump or public raw log requested. Desktop launchers launch only.

---

# Codex result — c31 exact VMOVUPS classification, diagnostic-only Outcome C

Branch **codex/v01c20-edi-value-provenance**; accepted task base **b78cd42**.
Implementation **6f78587134986bacde4683908b7cc57135bda7e7**. Version **v0.1c31 / package0.1.49**.
Outputs **MSM2-WS-v0.1c31.zip**, **MSM2-WS-v0.1c31-source.zip**,
**MSM2-WS-v0.1c31.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C31_RELEASE.json** (final hashes/sizes/source-snapshot commit).

## Accepted input / decision

Read AGENTS/manual/rules/workspace/state/NEXT, referenced compact runtime and
relevant code/contracts, refreshed Wisdoms instructions/common manual/WS/engine/
method references. Fast-forwarded intended WS branch only, no WS main merge/
reset/force push. Existing5 untracked legacy artifacts retained.

VERIFIED-RUNTIME c30: exact qword0x2CF7FE9/source0xC2D2580 abstract top7 effect
works with zero qword reads; RAX origin unknown/non-pointer, selected dependence
still possible. New barrier0x2CF7FF0 VMOVUPS/path0 from JA0x2CF7FE7 ->0x2CF7FE9,
RAX7/other GPR/XMM/flags0. Target4instructions/4visits/0returns/identity/effects.
C29 writable-source tag0 refusal and c25-c28 prerequisite chain remain accepted.

**Outcome C: classify this exact instruction; no new vector semantics.** Actual
VMOVUPS operand/width/direction/address/upper effects are absent from compact
runtime evidence and the repository contains no current private target snapshot.
Do not guess a form from a mnemonic or fixture. Current Slice is four32bit lanes
per XMM, ReadFlow16 XMM tag banks; complete YMM/ZMM/upper state is unrepresented.
VEX128 also clears upper state, VEX256/EVEX can exceed128 or add mask/merge/fault
behavior. Without complete representation, no full architectural vector-load
rule can be admitted. Actual metadata requires only one shortest new A/E capture.

## Implementation / changed files

- read_target_identity.hpp: pinned vector0x2CF7FF0 descriptor, up to8 operands,
  VEX2/3/EVEX/width/parent/access/modifier/upper metadata; lower represented state
  explicitly separate from unrepresented upper tags. No new vector state/model.
- rdx_source_contract.cpp: classify only reachable exact VMOVUPS before existing
  memory refusal. Decode exact XMM/YMM/ZMM aliases/parents/access, memory width/
  direction, base/index/scale/signed displacement/segment/address width, current
  conditional Hero/field10C/source/home/below-SP/image/unknown aliases/tags.
  Refuse store/RMW immediately and unsupported architecture/forms distinctly.
- Important owned-fixture finding: Capstone5.0.6 reports some VMOVUPS opcode11
  memory destinations as READ, with no register writes. Preserve decoded access
  but resolve direction only against exact VMOVUPS encoding/map/pp/VL/opcode and
  operand layout; opcode11 stores refuse. Masked vector merge(read+write) is not
  memory RMW. No decoder-wide patch or inferred permission for unrelated forms.
- For owned current-parent address/tag refinement, replay existing bounded saved
  code with natural CALL SP delta(-8), no live stack/data/vector read. Current
  actual branch/c30 source proof must still pass; vector always stops. Replay
  cumulative cap4096, per-walk original64visits/128bytes/32unique/1JA/2paths. Lose
  exactness/union aliases/tags for conflicts, refused replay or budget exhaustion.
  Below-SP is an unowned candidate, not private allocation. Unknown RAX never
  supplies pointer provenance or a second dereference.
- writer_entry_snapshot.cpp: compact VECTOR / VECTOR_OPERAND / VECTOR_ADDRESS /
  VECTOR_ARCH records include current-byte stability, decoder vs encoding access,
  direction, register/flags/SP/memory effects, parent aliases/tags, upper zero/
  preservation/mask/merge, representation refusal and no-value-observation flags.
  Existing UNKNOWN_LOAD/barrier/path/CFG/native safety records retained.
- read_target_identity_smoke.cpp:8 new groups, identity60 total; read/call21 and
  all c19-c30/native regressions retained. Updated version/info/README/TEST/BUILD,
  package metadata, accepted c30 mirror, c31 design/gate/index and AGENTS/state.
  **docs/CODEX_NEXT.md preserved byte-for-byte**.

The descriptor supplies no vector-load semantics, value/numeric observation,
tag0, state updates or next-op lookahead. Unknown vector loaded tags remain
may-top7 metadata only; preexisting vector tags are untouched. All actual forms
remain to be measured. No full-XMM load-admission test applies because no actual
complete represented architectural form is available; owned cases assert refusal.

C30 qword top7/source/domain conversion/no value reads are unchanged. Original
both-path/all-RET/final code/runtime/cap gates remain required for target span/
hash/effects; one safe taken leg cannot authenticate a refused fallthrough.
No local/vector/target diagnostic proves writer XYZ/W/feedback or ownership.

## Validation

- Windows x64 full warnings-as-errors build PASS.
- **13 portable +7 DLL/logger +200 hooks/native/contracts/workspace =220 public
  PASS records /5 optional private-input SKIP**, all final commands exit0 on first
  full attempt; no assertion/deadline/native cleanup rule relaxed.
- New cases cover exact VEX2/VEX3/EVEX operands/width/vector parents/access;
  encoded store refusal despite decoder READ; indexed/segmented/address32/loaded
  unknown RAX provenance; Hero/field10C/source/home/below-SP/image/conflicting
  aliases; XMM low128 vs upper-zero/YMM256/ZMM/EVEX mask/merge/zero refusal;
  wrong site/snapshot/one-safe-other-leg refusal; exact barrier/pre-state and
  unchanged vector/GPR/flags/SP, c30 top7/no data reads and full527161 table.
- Final test-only supplement adds invalid EVEX broadcast, LOCK, map and adjacent
  VMOVUPD negatives; focused identity60 rerun PASS. Production code unchanged.
  Source ZIP refreshed before compiling fixtures; production build inputs stayed
  byte-identical across this test supplement.
- Provisional source ZIP extracted fresh; independent full build PASS; **135
  focused entry/c14-c31 +7 DLL/logger +13 workspace =155 separate PASS records**,
  no private skips; standalone package verifier PASS separately. Do not add155
  duplicates to220. Final report-only refresh retains identical native/tool
  inputs and rechecks standalone package.
- Static byte checks preserve original scalar/CMP/JA/MOV-descriptor helpers,
  c29 readonly/value and c30 source metadata policy, complete c25 table/lookup/
  snapshot block, native orientation/lifecycle/raw ABI/assertions, workspace/
  updater/shared launchers and NEXT. No SF/ordinary mod/environment code changed.
- Final archives checked for CRC/hash/version/new DLL markers, current source/
  docs/evidence/helpers/licenses, one virtual active install script, no active
  source script, compiled/private game-body payload or raw runtime/vector data.
- Known cloud Wine winedevice.exe startup diagnostics occur; final commands
  nevertheless exit0. Owned fixtures/cloud checks are not actual-game PASS.

Wisdoms **dd5f55ceffeb885109396bf344849b3c18117b7f** pushed: accepted c30 exact
VMOVUPS barrier, missing actual form, decoder store-access caveat, vector upper-
representation/refusal and bounded parent diagnostics. Concurrent SF additions
were fetched and preserved; append conflict resolved by retaining both sections.
No raw private log/body/vector/qword published.

## Safety / minimum human evidence

writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement
writes0; producer0x351EB00 deferred. No new hook/game call, PAGE_GUARD/data-page
mutation, guessed ABI, gameplay/adhesion/camera/animation/physics change. Existing
orientation source writes remain under unchanged native gates, separate from
denied new translation/source-position writes.

After verified push, close game/shared Overstrike, run installed
**MSM2-WS\Update-WS.cmd**. Shared UI **Install mods -> Run modded game**.
A/cardinal~3sec -> fully end WallRun -> move/fast-travel -> E/diagonal~3sec,
below unchanged5sec limit. Return newest private
**MSM2-WS\logs\MSM2-WS-v0.1c31-YYYYMMDD-HHMMSS.log**. Minimum new evidence:
MOVEMENT_CALLEE_READ_VECTOR / VECTOR_OPERAND / VECTOR_ADDRESS / VECTOR_ARCH
plus existing UNKNOWN_LOAD/barrier/path/tag/CFG/safety/cleanup. No raw vector/
qword, full body or public runtime log requested. Desktop launchers launch only.

---

# Codex result — c30 pinned writable-image load with unknown may-top7

Branch **codex/v01c20-edi-value-provenance**; accepted task base **bd120c5**.
Implementation **f97d38f95d0123541e0abf0cf0654dffa24430f9**. Version **v0.1c30 / package0.1.48**.
Outputs **MSM2-WS-v0.1c30.zip**, **MSM2-WS-v0.1c30-source.zip**,
**MSM2-WS-v0.1c30.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C30_RELEASE.json** (final hashes/sizes/source-snapshot commit).

## Accepted input / decision

Read AGENTS/manual/rules/workspace/state/NEXT, referenced compact runtime and
relevant source/contracts; refreshed Wisdoms and read instructions/common manual/
WS/engine/method references. Fetched/fast-forwarded intended WS branch only,
no WS main merge/reset/force push; original5 untracked legacy artifacts retained.

VERIFIED-RUNTIME c29: exact source0xC2D2580 is readable/writable/nonexec .data
0x9B24000..0xC565A28/0xC0000040 on current committed same-image MEM_IMAGE/
PAGE_READWRITE. C29 tag0 rule correctly refused BEFORE value reads (0), not
proof of observed taint. C28 exact7byte full64 RAX <- [RCX+0x100], current same
image-root16 RCX source/tags0; prior RAX selected1. C25 full527161-entry table/
queried no-entry, c26 CMP tag0 and c27 short JA both forward edges remain accepted.

**Implemented only exact-load abstract top continuation; actual next boundary
pending.** Loaded dependence is unknown upper bound7, never measured1/2/4,
not tag0, constant/pointer/type/lifetime proof or killed selected dependence.
No qword read/value sample/log/hash/numerical interpretation/chase is used.
The accepted compact evidence stops at this MOV; next actual opcode/bytes are
absent from the repository. A minimum guarded next-boundary capture is needed;
no undirected raw body/qword request or habitual repetition of c29 tag0 test.

## Implementation / files

- read_target_identity.hpp: distinct ReadUnknownLoad and pre-stop path/edge/
  actual category-tag boundary records; explicit top=1|2|4=7; optional pinned
  unknown policy is explicitly selected by production, not a generic load grant.
- rdx_source_contract.cpp: only existing exact MOV/source form/current parent
  origin/tags can enter the rule. Before and after bounded walk, authenticated
  PE source section/range/flags must match accepted readable/writable/nonexec
  .data exactly, unique full8byte containment/no overlap, explicitly checked
  direct loader directories disjoint. Current whole8byte mapping must still
  be committed same-image MEM_IMAGE/PAGE_READWRITE with valid bounds. Exact
  PE/page equality and target-code recheck required, even on a later barrier.
- Existing c29 readonly/value policy remains byte-for-byte unchanged and records
  its structural readonly refusal separately. C30 uses its bounded PE/page
  classifier solely for metadata, never begin/finish value reads. Actual page
  outside this narrow accepted policy refuses; no permission broadening.
- Only RAX abstract contents become opaque unknown(globalRoot16,source), tags7.
  Unknown kind is not pointer/constant. All other GPR/XMM/flags/SP/memory remain.
  Selected bit1 is still possible; no claim that selected dependence is killed.
- Identity carrier masks and actual three-category tags are different domains.
  Convert a symbolic path through current entry tags exactly once BEFORE injecting
  intrinsic top7. Already-actual caller walks do not remap; other JA leg normalizes
  before merging. This prevents all-entry-tags0 from erasing unknown dependence.
  Existing register-only propagation/zero precision and JA tag checks retained.
- Exact first unsupported effect retains RVA/mnemonic/reason/path/incoming JA
  edge and GPR/XMM/flags state immediately before that instruction. Both legs
  required; current source disagreement, next memory/stack/CALL/indirect/partial/
  branch/crossing/cap/code/metadata/runtime failures still refuse whole target.
- writer_entry_snapshot.cpp: compact UNKNOWN_LOAD / LEAF_BARRIER / BARRIER_TAGS
  telemetry plus existing CFG/identity/safety. Source-value reads explicitly0;
  upper-bound tags distinct from precise provenance or observed taint. Existing
  two parent passes/reset seeds/code equality and current caller checks retained.
- read_target_identity_smoke.cpp:8 new groups,52 identity total; read/call21 and
  all c19-c29/native regressions retained. Updated version/info/README/TEST/BUILD,
  package metadata, accepted c29 source mirror, c30 design/gate/index, AGENTS/
  PROJECT_STATE. **CODEX_NEXT preserved byte-for-byte**.

All target begin/end/hash checks still require both modeled normal RETs under
unchanged target0x2CF7FE0/1JA/2paths/128bytes/32unique instructions/64visits,
plus final code/runtime coverage. Returned category summaries union both paths,
keeping differences explicit. No whole-writer/effect certificate or write gate
is granted by either local load success or owned-fixture target completion.

## Validation

- Windows x64 full warnings-as-errors build PASS.
- **13 portable +7 DLL/logger +192 hooks/native/contracts/workspace =212 public
  PASS records /5 optional private-input SKIP**, all commands exit0 on first
  full attempt. No native assertion/deadline/cleanup rule relaxed.
- New fixtures: exact source writable top-only/no value reads vs unchanged c29
  tag0 refusal; RAX incoming0..7 and all-entry-tags0 top preservation/copy/later
  zero; GPR/XMM/flags/SP preservation and existing propagation; wrong source/
  destination/width/store/RMW/partial/XMM/index/segment/address32/chase refusal;
  missing/conflicting/tainted parent evidence and actual caller disagreement;
  PE/page/code/runtime change refusal; full527161-entry table; exact next barrier/
  path/tag-state and unsafe second leg; union of different both-return tags.
  Source qword callbacks deliberately fail, proving no hidden read is required.
- Provisional source ZIP extracted fresh, independent full build PASS; **127
  focused entry/c14-c30 +7 DLL/logger +13 workspace =147 separate PASS records**,
  no private skips; standalone package verifier PASS separately. Do not add147
  duplicates to212. Final report-only refresh retains identical tested native/
  tool inputs and separately rechecks standalone package.
- Static byte checks preserve original scalar/CMP/JA/MOV-descriptor helpers,
  entire c29 readonly/value policy, complete c25 table/lookup/snapshot block,
  native orientation/lifecycle/ABI/assertions, workspace/updater/shared launchers
  and NEXT. No environment/SF/ordinary MOD implementation changed.
- Final archive checks: CRC/hash/version/new DLL diagnostic/refusal markers,
  all current sources/docs/analysis/helpers/licenses, one virtual active install
  script, no active source script or compiled/private game-body payload.
- Known cloud Wine winedevice.exe startup diagnostics occur; final commands
  nevertheless exit0. Synthetic fixtures and cloud checks are not real-game PASS.

Wisdoms **d5a7dda31816d5750d4a5f0f19902f9838051fbe** pushed: accepted c29 writable
source refusal, exact c30 abstract-top scope, carrier/category domain separation,
upper-bound vs observed-taint distinction and current proof/ownership limits.
Existing SF shared knowledge retained. No raw private log/body/value published.

## Safety / minimum human action

writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement
writes0; producer0x351EB00 deferred. No game ABI, new hook, PAGE_GUARD/data-page
mutation or gameplay/adhesion/camera/animation/physics change. Existing orientation
source writes remain governed by unchanged native gates; separate from denied
new translation/source-position writes.

Top7 means every tracked subset remains possible, not precise value provenance,
semantic type/pointer identity/constantness/immutability or observed dependence.
A completed target would not prove writer XYZ/W causality, cross-invocation
feedback, storage ownership, lifetime/exclusivity/conflict absence or writability.

After verified push: close game/shared Overstrike, run installed
**MSM2-WS\Update-WS.cmd**. Shared UI **Install mods -> Run modded game**.
A/cardinal~3sec -> fully end WallRun -> move/fast-travel -> E/diagonal~3sec,
both below unchanged5sec limit. Return newest private
**MSM2-WS\logs\MSM2-WS-v0.1c30-YYYYMMDD-HHMMSS.log**. Minimum new keys:
MOVEMENT_CALLEE_READ_UNKNOWN_LOAD / LEAF_BARRIER / LEAF_BARRIER_TAGS / LEAF_CFG
plus existing JA/current identity/safety/cleanup. No raw qword/full body/public
runtime log needed. Desktop launchers launch only, not update/build.

---

# Codex result — c29 exact image qword current-proof rule, actual mapping pending

Branch **codex/v01c20-edi-value-provenance**; accepted task base **822993e**.
Implementation **4bba3623ab325ca7d9971f9648d6b1321a210ea1**. Version **v0.1c29 / package0.1.47**.
Outputs **MSM2-WS-v0.1c29.zip**, **MSM2-WS-v0.1c29-source.zip**,
**MSM2-WS-v0.1c29.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C29_RELEASE.json** (final hashes/sizes/source-snapshot commit).

## Accepted input / result

Read AGENTS/manual/rules/workspace/state/NEXT and referenced compact runtime,
source/contracts and Wisdoms instructions/manual/engine/method/WS references.
Fetched/fast-forwarded only intended WS branch; no main merge/reset/force push.
Accepted c28 actual MOV0x2CF7FE9:7byte full64 RAX <- [RCX+0x100], flags preserved,
no other GPR/XMM/SP/memory write. Current parent RCX image-root16+204285056/tag0,
source8bytes+204285312, outside target code section. Incoming RAX tag1; loaded
value UNKNOWN/may7. C25 full527161 table/queried no-entry, c26 CMP tag0 and
c27 exact short JA both forward edges remain accepted.

**Bounded implementation complete; actual section/page/value proof pending.**
C29 does not claim actual-game immutable source or next target barrier. This
compact evidence has no source PE section/current VirtualQuery/stability record;
only the shortest A/E capture with the new keys can resolve those live facts.
No fresh raw target body or qword is requested. If actual source is writable,
the intended result is refusal at MOV before any value read, not a broader rule.

## Implementation / changed files

- read_target_identity.hpp: exact source pin, current-origin entry evidence,
  compact PE/page/value-window proof. Private metadata max4168bytes and private
  qword buffer are never logged or numerically interpreted.
- rdx_source_contract.hpp/.cpp: currentReadEntryEvidence requires sampled same
  affine RCX root16/exact offset/tags0. Pin only full64 RAX,[RCX+0x100], width8,
  exact site/shape. Before values, guarded authenticated PE64 section/range/flags
  must prove all8bytes in one readable non-writable section. Check direct import,
  TLS, loadconfig, bound-import, IAT, delay-import directory pairs/ranges/overlap.
  No descriptor traversal or universal loader-subrange exclusion claim.
- Current page must cover whole8bytes inside admitted image, same allocation,
  MEM_IMAGE/MEM_COMMIT, exactly READONLY or EXECUTE_READ. Reject writable/COW/
  guard/noaccess/unsupported, wrong allocation/type/state/range/boundary/overflow.
  AllocationProtect is compared metadata; current Protect gates non-writability.
- Two guarded8byte attempts maximum across bounded walk; require exact private
  byte equality, exact before/after PE metadata and page metadata/protection.
  Recheck target code also on a later-barrier stop. Failed source/code/reproof/
  runtime coverage invalidates proof; instability stops at exact MOV, may7.
- Only after all checks, full64 RAX tags replace prior tag1 with0. Opaque
  memoryWide(image-root16,source) is never pointer provenance. Preserve all other
  registers/flags/SP/memory. Actual per-caller RCX/tags must still agree; union
  returned tags and origins across both JA paths, never prefer a safe leg.
- writer_entry_snapshot.cpp: existing two parent passes/reset seeds/identical
  code rule retained. Add VirtualQuery metadata and compact IMAGE_SOURCE/PAGE/
  VALUE records. Report only structural flags/equality/attempt counts/reasons,
  no raw qword, numeric interpretation, second dereference or new hook/game call.
- read_target_identity_smoke.cpp:8 new owned groups, identity44 total; existing
  read/call21 retained. Version/info/README/README_TEST/BUILD, accepted c28 mirror,
  c29 analysis/gate/engine index, AGENTS/PROJECT_STATE/package manifest updated.
  **docs/CODEX_NEXT.md preserved byte-for-byte**.

Unchanged target0x2CF7FE0, one JA/two paths/128bytes/32unique instructions/64visits,
all-RET requirement and final code/runtime coverage before target span/hash/effects.
No generic memory semantics, another branch, stack/nested CALL/indirect/loop/
partial-write relaxation. Next unsupported effect remains exact barrier.

## Validation

- Windows x64 full warnings-as-errors build PASS.
- **13 portable +7 DLL/logger +184 hooks/native/contracts/workspace =204 public
  PASS records /5 optional private-input SKIP**, all final commands exit0.
  Full run passed on first attempt; no native test deadlines/assertions relaxed.
- Identity44 includes nonwritable/readable PE/RX, writable PE/page and COW/guard/
  noaccess/unsupported refusal, wrong allocation/type/state/mapping/PE boundaries,
  overflow/outside pin, six direct loader overlaps/malformed ranges, first/final
  read failure/change, metadata/page/code change, two-read budget, caller origin/
  tags/width mismatch, full64 RAX kill and other register/flag preservation,
  differing both-path returned effects/unsafe leg and exact next barrier.
  Existing c19-c28/no-unwind/full527161 metadata regressions retained.
- Actual provisional source ZIP extracted into a fresh directory, independent
  build PASS; **119 focused entry/c14-c29 +7 DLL/logger +13 workspace =139 separate
  PASS records**, no private skips; standalone package verifier PASS separately.
  Do not add duplicate139 to204. Final report-only refresh retains identical
  native/tool inputs and rechecks standalone package.
- Static byte checks preserve NEXT, c25 complete table/lookup/snapshot block,
  native lifecycle/orientation/raw ABI/override tests, workspace/updater/shared
  recovery launchers. No environment or SF/ordinary MOD implementation changed.
  Final archive checks cover CRC/hash/version/DLL markers, all current source/
  docs/analysis/helpers/licenses, one virtual active install script, no active
  source script, compiled/private-body exclusions and retained5 legacy artifacts.
- Known Wine winedevice.exe startup diagnostics occur in this cloud toolchain;
  every executed final command nevertheless exited0. No actual-game claim.

Shared Wisdoms **bf48f3a3e303bd89689bf14275fdbd8929cd3dc7** pushed: accepted c28
exact image load/source vs UNKNOWN value, c29 bounded policy/method and explicit
loader coverage/equality/lifetime limits. Existing SF knowledge retained.

## Safety and minimum human evidence

writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent remain0. Position/source-position/input/
movement writes0. Producer0x351EB00 deferred. No new PAGE_GUARD/data protection,
game ABI or gameplay/adhesion/camera/animation/physics behavior. Existing admitted
orientation source writes remain governed by unchanged original gates; they are
separate from the denied new translation/source-position writes.

Two equal reads plus current readonly mappings prove only this bounded analysis
window, not atomicity, no intervening mutation, future stability, image-global
lifetime or exclusive ownership. Even a successful local load is not a complete
target certificate; even an all-path target is not whole-writer XYZ/W causality,
feedback, storage/lifetime/exclusivity or writable movement-source permission.

After verified push: close game/shared Overstrike, double-click installed
**MSM2-WS\Update-WS.cmd**. In shared UI **Install mods -> Run modded game**.
A/cardinal ~3sec -> fully end WallRun -> move/fast-travel -> E/diagonal ~3sec,
each below unchanged5sec limit. Return newest private
**MSM2-WS\logs\MSM2-WS-v0.1c29-YYYYMMDD-HHMMSS.log**. Minimum new evidence:
MOVEMENT_CALLEE_READ_IMAGE_SOURCE / IMAGE_PAGE / IMAGE_VALUE and existing exact
next barrier/current MOV/JA/identity/safety/cleanup records. Do not publish raw
qword/code/full runtime log. Desktop launchers launch only, not update/build.

---

# Codex result — c28 exact MOV operands/address provenance, diagnostic-only Outcome C

Branch **codex/v01c20-edi-value-provenance**; accepted task base **e420e61**.
Implementation **95a2aec20e53913b64c245691a081658862c7d29**. Version **v0.1c28 / package0.1.46**.
Outputs **MSM2-WS-v0.1c28.zip**, **MSM2-WS-v0.1c28-source.zip**,
**MSM2-WS-v0.1c28.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C28_RELEASE.json** (final hashes/sizes/source-snapshot commit).

## Accepted input / decision

Read AGENTS/manual/rules/workspace/state/NEXT, relevant source/contracts, accepted
compact runtime and accessible Wisdoms instructions/manual/engine/method/project
references. Fetched/fast-forwarded intended WS branch only, no main merge/reset.
Preserved incoming Overstrike updater/WINMM recovery helpers and ordinary updater.

C27 actual evidence accepts short JA0x2CF7FE7 disp42 -> fall0x2CF7FE9/taken0x2CF8013,
both forward/in-window/no runtime crossing, current tag-disjoint fan-out with no
numerical outcome choice. First fallthrough MOV memory operand refuses:3 unique
instructions/3 visits/0 returns/identity/hash/effects. C25 full527161-entry table/
queried no-entry and c26 CMP r8d,0x3FFF/tag0 remain accepted. Native Deinit/
retention clean, hard kills/Position/source-position/corrections0.

**Outcome C, diagnostic implementation complete.** Exact actual MOV form/access/
address/value provenance is not in supplied compact evidence. Add classification
first; no new memory-load/immutable-image/partial/XMM destination semantics.
Address provenance or a read-only-looking image name cannot supply the missing
loaded-value proof. All memory writes/RMW and unproven reads remain refusals.
No actual MOV or downstream target/effect success claimed from owned fixtures.

## Implementation / files

- read_target_identity.hpp: pinned MOV0x2CF7FE9 compact operand/access/width/alias,
  flags/decoded write and conditional address-provenance records. Alias masks
  distinguish Hero1/field10C2/source4/caller-home8/private-candidate16/image32/
  unknown64. Unknown addresses retain all possible bits; below-SP isn't allocation.
- rdx_source_contract.cpp: classify only this reachable instruction from the
  stable c27 snapshot, before the existing memory refusal. Describe exact GPR/
  XMM/immediate/memory classes, full/partial aliases/parents, base/index/scale/
  signed displacement/segment/address width, decoded read/write/RMW and complete
  register-write/flag metadata. Pure-load FORM is distinct from load permission.
  Unexpected/unmodeled memory forms refuse; store/RMW stops immediately.
- Initial Slice entry roots remain untyped tokens. At the already-analyzed parent
  CALL, re-decode only the saved pinned instruction and refine using owned Slice/
  ReadFlow state with existing natural CALL delta(-8), no stack/value read. Known
  affine roots may report conditional Hero/field/source/stack/image address ranges;
  loaded raw bits, other tokens, index/segment/address32/range ambiguities stay
  unproven. RIP image/code membership grants no loaded-value rule. Across parent
  states union possible aliases/tags and lose exactness on conflicting addresses.
  Origins explicitly describe first sampled state, not universal typed arguments.
- Register/base/index input tags are separate from loaded-value tags. Loaded
  values remain UNKNOWN with may-tag upper bound7, never untainted0 or a state
  update; samples0 means no current-parent refinement. No memory contents or
  physical operand values are observed. Only instruction constants/displacements
  and abstract roots/offsets are emitted. No second dereference, reader or hook.
- writer_entry_snapshot.cpp: compact READ_MOV / READ_MOV_OPERAND / READ_MOV_ADDRESS
  records alongside unchanged next-boundary/JA/table/native safety records.
  Decoded writes are explicitly not executed writes. No raw code/value dump.
- Eight new owned groups in read_target_identity_smoke.cpp (36 identity groups),
  existing21 call/read and c19-c27/native regressions retained. Version/info,
  package metadata, README/README_TEST/BUILD, accepted c27 source mirror, c28 design/
  gate, AGENTS/PROJECT_STATE and engine index updated. **CODEX_NEXT unchanged**.
  Detailed method/limitations: analysis/V01C28_MOV_CLASSIFICATION.md.

No change to c27 one-JA/two-path/128byte/32unique-instruction/64visit all-RET proof,
c25 table/lookup budgets, c26 CMP/JA disjointness, code/coverage rechecks, native
lifecycle/orientation/ABI/assertions/deadlines/cleanup, or workspace isolation.
A failed MOV refuses the whole target; one safe other leg grants no identity.

## Validation

- Final Windows x64 warnings-as-errors build PASS.
- Full chain **13 portable + 7 DLL/logger + 176 hooks/native/contracts/workspace
  =196 public PASS /5 optional private-input SKIP**, all final commands exit0.
- New cases cover exact operand/access/width/writes/flag preservation; parent vs
  entry-token/loaded-raw-bit address provenance; selected1/postwrite2/other4 input
  tags vs unknown loaded7; conflicting caller addresses; indexed/segmented/32-bit
  metadata; Hero/field/source/home/below-SP alias refusal and correct CALL delta;
  store/RMW/immediate and partial/full GPR/XMM metadata; image/code read refusal;
  pin/stability/branch-current-tag gates and unsafe-leg whole-target refusal.
  No load rule is implemented, so destination zero/upper-lane semantic admission
  tests are inapplicable; these widths only receive metadata and refusal tests.
- A new negative test initially assumed no record after an earlier pinned register
  MOV, though a later non-pinned memory instruction failed. Corrected it to require
  that earlier pinned no-memory record plus the exact later refusal. No guard or
  test assertion weakened to admit memory.
- Initial full run hit an unchanged native_source_override_smoke.cpp reject-reason
  assertion (exit3). The isolated native fixture then passed all its7 groups and
  lifecycle passed; a60s whole-script diagnostic wrapper ended during the later
  integration executable (124), not on another assertion. Final normal full hooks
  run passed both native/integration and all remaining suites. No native source,
  rejection criterion, freshness limit or assertion changed. Cause of the initial
  unexpected reason is not established; don't represent it as a real-game result.
- Actual provisional source ZIP extracted fresh: build PASS; **111 focused
  entry/c14-c28 +7 DLL/logger +13 workspace =131 separate PASS**, no private skips;
  package verifier PASS separately. Don't add duplicate131 to196.
- Static byte checks preserve pre-existing scalar/CMP/JA-edge helpers, complete
  table/lookup/snapshot guard segment, native lifecycle/orientation/ABI code,
  updater/recovery helpers and CODEX_NEXT. Final reporting refresh retains identical
  tested native/tool inputs and rechecks package, ZIP CRC/hash/version/DLL MOV/
  refusal markers, complete current source/docs/analysis/helpers/licenses, single
  active virtual install member and private/compiled exclusions.

Shared Wisdoms **4e98687** accepts c27 exact geometry/MOV boundary and records
address-vs-value/unknown-tag/conditional alias method as VERIFIED-STATIC owned
fixture scope, actual c28 UNKNOWN. Concurrent SF knowledge changes retained.
No raw private log, proprietary target body or arbitrary memory value published.

## Safety / next human evidence

All writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement0.
Producer0x351EB00 deferred. No gameplay/adhesion/camera/animation/physics, guessed
ABI, data-page/PAGE_GUARD, new hook or SF/ordinary mod changes. Local address or
all-path diagnostics never prove whole writer XYZ/W, feedback, storage/lifetime/
exclusive ownership or writability. No effect certificate issued.

Exact MOV metadata is genuinely absent from compact c27; one short live capture
is needed. Minimum new fields: READ_MOV / READ_MOV_OPERAND / READ_MOV_ADDRESS and
existing TARGET_IDENTITY/READ_CALL next barrier plus native safety. No body,
memory-value dump or habitual repeat once current evidence is sufficient.
Close game/Overstrike; installed **MSM2-WS/Update-WS.cmd**; shared Overstrike
**Install mods -> Run modded game**. **A ~3s -> fully end WallRun -> move/fast-travel
-> E ~3s**, each below unchanged5s. Return newest private complete
**MSM2-WS/logs/MSM2-WS-v0.1c28-YYYYMMDD-HHMMSS.log**. No manual ZIP placement or
links required. Actual MOV classification/next justified model remains pending.

---

# Codex result — c27 exact JA edges / bounded all-path target proof

Branch **codex/v01c20-edi-value-provenance**; accepted task base **7f0c4d9**.
Implementation **0305d77ff17b25fe7169420415e968670b976286**. Version **v0.1c27 / package0.1.45**.
Outputs **MSM2-WS-v0.1c27.zip**, **MSM2-WS-v0.1c27-source.zip**,
**MSM2-WS-v0.1c27.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C27_RELEASE.json** (final sizes/hashes/source-snapshot commit).

## Accepted input / outcome

Read AGENTS, development manual/rules/workspace/state/NEXT, referenced compact
runtime results, current code and relevant accessible Wisdoms instructions/manual/
engine/method/project references. Fetched/fast-forwarded only the intended WS
branch; no main merge/reset. Incoming Overstrike updater/WINMM recovery helpers
remain byte-identical to task base. No SF/ordinary-mod implementation changes.

Accepted c26 actual diagnostic: exact CMP r8d,0x3FFF at0x2CF7FE0, flags-only,
R8/flags tags0; next JA0x2CF7FE7 uses CF/ZF from CMP, condition tags0 but numerical
outcome UNKNOWN. C25 full527161-entry/6325932-byte table/actual queried no-entry
and next runtime0x2CF8020 remain accepted. C26 native Deinit/retention passed,
hard kills/Position/source-position/corrections0. Compact evidence contains no
branch displacement, taken/fallthrough RVAs or subsequent path bytes.

**Outcome C / prepared conditional proof:** classify exact current edges and
attempt only an explicitly authorized bounded all-path extension. Actual c27
edges/next barrier/identity/effects remain PENDING minimum live metadata. Owned
fixture success does not supply the missing actual bytes/structure. Do not infer
branch direction, function semantics, ABI, XYZ/W relevance or writable ownership.

## Implementation / files

- read_target_identity.hpp / rdx_source_contract.hpp: compact exact edge/range/
  runtime/encoding and CFG fields, current entry tags and per-return summaries.
- rdx_source_contract.cpp: only pinned JA0x2CF7FE7, short77rel8 or near0F87rel32,
  exact decoded signed displacement/edge agreement, RVA under/overflow/code/
  allowance/runtime bounds and forward checks. Require its exact accepted CMP
  producer, complete decoded flag access and no relevant1/2/4 condition tags.
  No historical tag0 assumption or numerical-direction choice.
- One conditional split, at most2 paths,128 target bytes,32 unique reachable
  instructions,64 visits. Clone state before both legs; use byte ownership to
  reject overlapping instruction decodes. Decode reachable instructions only;
  unused hash-span gaps are not modeled instructions. Any unsupported effect,
  memory/stack/nested-call/partial/indirect/other branch/backedge/cap or missing
  normal RET refuses the whole candidate, including after one safe RET.
- Every path must safely end at plain C3 RET before span admission. Re-read the
  whole bounded snapshot after the walk and recheck runtime coverage before
  identity/hash. c23 verifies the same pinned proof/snapshot, repeats branch
  tags per analyzed state and models all reachable returns; legacy return-slot/
  stack alias/SP bookkeeping stays. Union GPR/XMM/flags/clobber effects; differing
  origins become unknown, with explicit per-path/path-dependent return records.
  Caller state is untouched if any path fails. No recursive CFG emulator added.
- writer_entry_snapshot.cpp: at most2 proof/parent-analysis passes. First proof
  records edges and stops for missing current tags; parent sidecar supplies tag
  unions. Reprove current metadata/code, compare to prepass snapshot, reset both
  analysis seeds, then rerun. Any incomplete parent proof remains incomplete.
  New TARGET_PASSES/LEAF_CFG/LEAF_RETURN and extended LEAF_BRANCH report exact
  outcomes/next failure. No raw body, memory value or numerical outcome emitted.
- Seven owned groups added to read_target_identity_smoke.cpp (28 total), retaining
  c19-c26/21 read-call groups. Version/info, package manifest, README/README_TEST/
  BUILD, accepted c26 source mirror, c27 design/gate, engine index, AGENTS and
  PROJECT_STATE updated. **CODEX_NEXT preserved**. See V01C27_JA_FANOUT.md.

## Validation

- Final Windows x64 warnings-as-errors build PASS.
- Final full pipeline **13 portable + 7 DLL/logger + 168 hooks/native/contracts/
  workspace = 188 public PASS / 5 optional private-input SKIP**, all exit0.
- New owned cases cover exact short/near edges and last allowed byte, signed
  overflow/range/backedge/runtime crossing, missing/dependent current tags and
  per-state recheck, two identical/different live-killed GPR/XMM returns in both
  orders, decode overlap, second branch/path cap, forbidden effect on one leg,
  instruction/byte/normal-return limits, initial/final snapshot instability,
  proof-versus-effect snapshot mismatch, exact CMP producer, full527161-entry
  table and existing c19-c26 guards. No private target body replay.
- Actual provisional source ZIP extracted independently: fresh build PASS;
  **103 focused entry/c14-c27 + 7 DLL/logger + 13 workspace = 123 separate PASS**,
  no private skips; package verifier PASS separately. Don't add duplicates to188.
- Initial full run stalled in the unchanged hardware orientation-watch fixture;
  stopped that owned process (exit143). One bounded isolated retry timed out124.
  Prior c26 and current fixtures then passed independently, including current
  normal WINEDEBUG=-all. Final full normal pipeline passed including that fixture.
  No fixture assertion, timeout or native production gate was relaxed. This is a
  Wine-fixture intermittent stall, not a claimed real-game result or hidden PASS.
- Static byte checks preserve c19-c26 scalar models before the new edge helper,
  complete c25 table/lookup/snapshot guards, native lifecycle/orientation/ABI/
  deadline/cleanup sources, updater/WINMM helper baseline and CODEX_NEXT.
- Final reporting refresh retains byte-identical tested native/tool inputs,
  rechecks standalone package, final ZIP CRC/hash/version/DLL diagnostic markers,
  complete current source/docs/analysis/helpers/licenses, one active virtual
  install member and private/compiled exclusions. Hashes are in release metadata.

Shared Wisdoms **650e384** supersedes stale c26 UNKNOWN form with accepted CMP/JA
facts and records the all-path/current-state/conservative-merge method as
VERIFIED-STATIC/owned-fixture scope, actual c27 UNKNOWN. Concurrent SF knowledge
changes retained; no raw private logs/bodies/personal data added.

## Safety / human evidence

writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement0.
Producer0x351EB00 deferred; no gameplay/adhesion/camera/animation/physics, guessed
ABI/PAGE_GUARD/data-page changes/new hook or SF/ordinary mod mutation. Current
code snapshots are not atomic execution attestation or future ownership proof.
Local all-path completion alone does not resolve whole writer XYZ/W, feedback,
caller storage, lifetime/exclusivity or writability. No effect certificate issued.

Exact actual edges/path bytes are absent from compact c26, so one short capture
is genuinely required. Minimum fields: LEAF_BRANCH encoding/displacement/both
edges and limits, TARGET_PASSES/LEAF_CFG/next TARGET_IDENTITY/READ_CALL barrier;
LEAF_RETURN only if all outcomes pass; existing native/Position/cleanup records.
Close game/Overstrike; installed **MSM2-WS/Update-WS.cmd**; shared Overstrike
**Install mods -> Run modded game**. **A ~3s -> fully end WallRun -> move/fast-travel
-> E ~3s**, each below unchanged5s. Return newest private complete
**MSM2-WS/logs/MSM2-WS-v0.1c27-YYYYMMDD-HHMMSS.log**. No function/memory dump,
manual ZIP placement, routine repeat run or download link required.

---

# Codex result — c26 exact pinned CMP / strict linear next boundary

Branch: **codex/v01c20-edi-value-provenance**; accepted task base **4b075c6**.
Implementation: **53530d9db2098c34dfa6b709398d3f3f79355957**. Version **v0.1c26 / package0.1.44**.
Outputs: **MSM2-WS-v0.1c26.zip**, **MSM2-WS-v0.1c26-source.zip**,
**MSM2-WS-v0.1c26.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C26_RELEASE.json** (final sizes/hashes and source-snapshot commit).

## Accepted evidence / outcome

Read AGENTS/manual/rules/workspace/state/NEXT, referenced compact runtime evidence,
current source/contracts and accessible Wisdoms shared references. Fetched only
this WS development line; no main merge. Base4b075c6 accepts c25 actual evidence:
complete directory0xC566000/6325932bytes/527161 entries structurally valid under
unchanged600000/8MiB limits; actual OS lookup queried and agrees no containing
entry for0x2CF7FE0, next runtime0x2CF8020. Stable64-byte snapshot stops immediately
at CMP. C25 A/E clean native Deinit/retention, no hard kills or Position writes.

**Outcome C / conditional diagnostic implementation.** Compact c25 provides no
CMP operands/width/immediate or next instruction. Do not guess from mnemonic,
reason or raw register index. C26 classifies exact guarded code first and may
advance only a proven form. Owned tests establish the implementation, not actual
CMP form, target RET/hash/effect, downstream XYZ/W or writable ownership.

## Implementation / files

- read_target_identity.hpp and rdx_source_contract.cpp: bounded at-most2 operand
  descriptors for pinned0x2CF7FE0 CMP. Report class/width/access/exact alias/parent,
  instruction immediate and memory address metadata without reading its value.
  Check complete decoder register reads/writes, every explicit input, no hidden
  inputs, exact CF/PF/AF/ZF/SF/OF modified set and no legacy prefixes.
- Only full32/64-bit GPR reg/reg or reg/immediate flags-only forms are admitted at
  this exact site. Existing full reg/reg leaf forms remain. New reg/immediate
  semantics are not added to generic Slice/parent CFG or arbitrary sites.
  Operands/GPR/XMM/stack remain unchanged; immediates contribute tag0. Flags carry
  possible input-tag union; same exact aliases yield constant flags without
  killing register provenance. No numerical branch direction is inferred.
- Low/high8/16 aliases record conservative parent metadata but refuse semantics;
  memory value tags remain UNKNOWN and memory forms refuse. Unknown widths,
  prefixes/effects/aliases refuse at the exact instruction. No partial write,
  memory observation, guessed ABI or recursive emulator added.
- Stable-prefix symbolic GPR/XMM masks track only admitted operations and flag
  overwrites. Map to already-analyzed parent CALL tag1/2/4 states without live
  game calls or accepting a target span. A next Jcc reports exact RVA/mnemonic/
  condition/read flags/producer/dependency; incoming unknown flags and samples0
  stay unknown. All branches refuse, including tag0/constant conditions; no
  outcome is narrowed or explored. Prefix facts never become identity/effects.
- Unchanged c25 full-table/read/lookup and c24 exact code/snapshot/runtime-region
  guards precede classification. The128byte/32instruction complete linear normal
  C3 RET/coverage-recheck contract is retained. Only a fully admitted span is
  hashed and passed to c23. No first-RET trap, branch, memory/stack/nested-call or
  unsupported consumer can be used to force target completion.
- writer_entry_snapshot.cpp adds compact READ_CMP / READ_CMP_OPERAND /
  READ_LEAF_BRANCH records. No raw bytes or live CMP operand values are emitted.
  Existing TARGET_IDENTITY/READ_CALL/runtime-table and all lifecycle logs remain.
- Six new owned test groups in read_target_identity_smoke.cpp (21 identity groups,
  retaining15 c24/c25); existing21 read/call groups retained. Version/info,
  package metadata, README/README_TEST/BUILD, c25 accepted source mirror, c26 design/
  gate, engine index and PROJECT_STATE updated. **CODEX_NEXT unchanged**.

## Validation

- Windows x64 warnings-as-errors build PASS.
- Full pipeline: **13 portable + 7 DLL/logger + 161 hooks/native/contracts/workspace
  = 181 public PASS / 5 optional private-input SKIP**, every command exit0.
- New tests cover32/64 reg/immediate and full reg pairs, signed constant,
  selected1/postwrite2/other-prewrite4/disjoint/self-comparison tags, flags-only
  effect and operand preservation, low/high8/16 refusal, memory refusal/unknown,
  exact JE/JNE/JB/JG refusal with dependency, overwritten flags/GPR-XMM copies,
  unsupported flag consumers, stable RET, pin/stability/runtime crossing, complete
 527161-entry table and late-invalid-tail refusal. No private actual-body fixture.
- Actual provisional source ZIP extracted fresh: build PASS; **96 focused
  entry/c14-c26 + 7 DLL/logger + 13 workspace = 116 separate PASS records**, no
  private skips; package verifier PASS separately. Do not add duplicate116 to181.
- Static checks confirm c19-c22 models before the new helper, complete c25 table/
  lookup/snapshot guard segment, native lifecycle/orientation/ABI/cleanup/timeouts,
  updater and CODEX_NEXT unchanged. No main merge or SF/ordinary-mod mutation.
- Final source reporting refresh retains byte-identical tested native/tool files,
  rechecks its package, and checks final ZIP CRC/hash/version/DLL diagnostic markers,
  complete source/docs/analysis/licenses/helpers, one virtual active install script,
  no compiled/private source-archive payload and matching release metadata.

Shared Wisdoms ENGINE_INTERNALS now supersedes stale prepared c25 UNKNOWN status
with accepted complete-table/queried absence evidence; RE guide/WS internals
record bounded CMP metadata and refused-prefix method as VERIFIED-STATIC/owned
fixtures, actual c26 UNKNOWN. Synced commit **373516a**. Concurrent SF shared
knowledge updates were retained during the documentation fast-forward.

## Safety / next human step

All writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent remain0; Position/source-position/input/movement
writes0. Producer0x351EB00 remains deferred. No new hook/data-page/PAGE_GUARD,
adhesion/camera/animation/physics advancement, raw private log/body publication,
old-workspace deletion or changes to native assertions/deadlines/teardown.

Current compact evidence genuinely lacks exact CMP metadata; there is no sufficient
actual static/current-body evidence to skip the new capture. Minimum needed:
READ_CMP/READ_CMP_OPERAND and next LEAF_BRANCH/TARGET_IDENTITY/READ_CALL, alongside
existing safety records. No full function/memory dump or private byte copy requested.
Close game/Overstrike, run installed **MSM2-WS/Update-WS.cmd**; shared Overstrike
**Install mods -> Run modded game**. Test **A ~3s -> fully end WallRun -> move/
fast-travel -> E ~3s**, each below unchanged5s limit. Return newest private complete
**MSM2-WS/logs/MSM2-WS-v0.1c26-YYYYMMDD-HHMMSS.log**. No manual ZIP placement or
links needed. If this supplies sufficient exact-form/next-boundary evidence,
continue from it without a habitual repeat test.

---

# Codex result — c25 finite large-table validation / lookup coverage (2026-10-25)

Branch: **codex/v01c20-edi-value-provenance**; accepted task base **4e4d900**.
Implementation: **eee37b68095fd5ab8f09339982020c58cbb0eee2**.
Version: **v0.1c25 / package0.1.43**. Outputs:
**MSM2-WS-v0.1c25.zip**, **MSM2-WS-v0.1c25-source.zip**,
**MSM2-WS-v0.1c25.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C25_RELEASE.json**. Exact final hashes are in release metadata.

## Accepted input / interpretation

Read AGENTS/manual/rules/workspace/state/NEXT, accepted c24 runtime, c24 identity
contract and relevant code/tests, plus accessible Wisdoms references. C24 reported
527161 derived entries at the combined malformed_or_cap refusal before validation.
The old262144 limit was smaller. This proves neither corruption nor absent entry,
leaf identity or effects. Raw GPR0 still carries selected Transform+0x10C tag1.
C24 session1 reached the existing5second diagnostic limit before verified Deinit;
session2 exited normally around4.16s. Position writes0; process-cumulative hard
counter does not mean a second new episode. No limit/assertion was relaxed.

VERIFIED-STATIC correction: in c24 source, OS lookup assignment followed both
the cap guard and the complete table loop. At this refusal the callback did not
run. The found0/same_image1/readable1 fields were defaults, not measured OS absence.
The observed count is floor(size/12);6325932bytes is the aligned size implied by
that count, not a supplied measurement of directory bytes/divisibility.

**Outcome C diagnostic preparation:** validate actual metadata under justified
finite budgets, then and only then determine target status/identity/next barrier.
The game's actual rows/byte size are unavailable in compact cloud evidence;
**actual c25 structure/lookup/identity/effects remain PENDING the new log**.
No proprietary table/body is reconstructed/requested/distributed. All downstream
XYZ/W/cross-invocation/ownership/write questions remain separate and unproven.

## Implementation / files

- read_target_identity.hpp + rdx_source_contract.cpp: explicit600000-entry and
  8MiB(8388608byte) hard ceilings cover the reported527161 floor count if aligned,
  with72839 entry headroom. At most7200000 actual table bytes/9375 guarded reads,
  same64entry/768byte stack chunk and no table-sized allocation or memory search.
- Derive count from the admitted directory's byte size; distinguish invalid RVA/
  size pair, out-of-image/overflow range, nondivisible size, byte-cap and entry-cap
  refusal. Structural checks precede budgets. Cap refusal explicitly marks skipped
  entry validation and never implies malformed metadata or a missing-unwind leaf.
- Preserve every c24 ordered/nonoverlap/begin-end/image/unwind-RVA check and scan
  every entry, including tails after target matches. Report bytes read/entries
  validated, exact chunk read failure and full-table completion. No target lookup,
  code snapshot or identity can occur on incomplete validation. New lookupQueried
  records actual callback execution, not initialized result values.
- The entire c24 post-lookup rule is byte-for-byte unchanged from task base:
  exact target0x2CF7FE0, PE/OS agreement, ordinary runtime entry or strict no-entry
  128byte/32instruction linear register-only RET exit set, stable guarded code,
  no memory/stack/branch/further CALL/partial effect or runtime-region crossing.
  Only admitted identity feeds c23; no c19/c20 read/effect waiver/certificate.
- writer_entry_snapshot.cpp adds compact READ_RUNTIME_TABLE and queried flag in
  TARGET_IDENTITY. No directory bytes or raw live values/full code are logged.
  Warnings-as-errors format checks pass; records fit4096byte formatter.
- read_target_identity_smoke.cpp adds6 independently authored large-table groups,
  retaining9 c24 groups and21 c21-c23 read/call groups. Version/info, README/
  README_TEST/BUILD, package metadata, accepted c24 source mirror, c25 analysis/
  gate, PROJECT_STATE and derived engine index updated. CODEX_NEXT preserved.

Exact build/current code/image/parent-call gates, c19-c24 models, native lifecycle/
orientation/ABI/pairing/hardware cleanup/deadlines/assertions and ordinary update/
archive/shared-Overstrike/log contracts remain. No new hook or game call, producer
0x351EB00 body traversal, PAGE_GUARD/data-page mutation, guessed ABI, SF/ordinary
mod implementation, main merge or old-workspace deletion.
writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement
writes0. Movement/adhesion/camera/animation/physics remain deferred.

## Validation

- Warnings-as-errors Windows x64 build PASS.
- Full chain **13 portable + 7 DLL/logger + 155 hooks/native/contracts/workspace
  PASS records = 175 public PASS / 5 optional private-input SKIP**, all exit0.
- Six new groups cover262143/262145/527161/600000 valid owned entries, finite entry/
  byte excess, nondivisibility/pair/image/overflow errors, late unordered/overlap/
  zero/reversed/out-of-image ranges and invalid unwind RVAs, guarded chunk failure,
  containing-entry identity/lookup disagreement and validated absence. Tests check
  full scan/count/byte coverage, bounded chunks and zero lookup/code reads on refusal.
- Existing c24 first-RET trap/branch/call/memory/stack/crossing/unstable/pin/128-32
  rules and c23/c22/c21/c20/c19 regressions remain. No private game-table replay;
  matching527161 is a generated fixture shape, not proof about actual rows.
- Actual source ZIP extracted fresh: build PASS; **90 focused entry/c14-c25 +
  7 DLL/logger + 13 workspace = 110 separate PASS records**, all exit0/no private
  inputs. Package verifier PASS separately; don't add duplicates to175.
- Final archives check CRC, payload/DLL/version/table-marker hashes, complete
  current source/docs/analysis/licenses/helpers/owned logs, one virtual active
  install script, private/compiled exclusions and unchanged task/native safety
  files. Final report-doc refresh uses identical tested native/tool sources and
  a standalone package recheck. No semantic implementation change after validation.

Wisdoms ENGINE_INTERNALS/RE guide/WS internals preserve the c24 cap/default-field
clarification and finite-budget/full-coverage method as VERIFIED-STATIC source/
owned-fixture scope only; actual c25 remains UNKNOWN. Detailed proof stays here.
No raw private logs, table dump, full game body or personal data published.

## Next human step / specific missing evidence

Compact c24 has only a floor count and combined refusal, not actual directory
bytes/rows or an executed target lookup. A new RUNTIME_TABLE record is genuinely
needed to adjudicate actual structure/lookup/next identity boundary; no habitual
rerun or full raw table/body request. Close game/Overstrike, run installed
**MSM2-WS/Update-WS.cmd**, then shared Overstrike **Install mods -> Run modded game**.
Test **A/cardinal ~3s -> fully end WallRun -> move/fast-travel -> E/diagonal ~3s**;
finish each below the unchanged5s diagnostic limit. Return newest complete private
**MSM2-WS/logs/MSM2-WS-v0.1c25-YYYYMMDD-HHMMSS.log**.
Only RUNTIME_TABLE and lookup_queried are new; existing TARGET_IDENTITY/CALL/READ/
TEST/VALUE/WRITE/ALIAS/WRITER and safety/cleanup remain in the same log.
No body dump/video unless visible anomaly; no manual ZIP placement or link needed.

---

# Codex result — c24 single-target boundary / no-unwind identity (2026-10-25)

Branch: **codex/v01c20-edi-value-provenance**; accepted task base **432f8a1**.
Implementation: **d9df7a4a4ec68cabdf9bbe239c9d36fbb20e3654**.
Version: **v0.1c24 / package0.1.42**. Outputs:
**MSM2-WS-v0.1c24.zip**, **MSM2-WS-v0.1c24-source.zip**,
**MSM2-WS-v0.1c24.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C24_RELEASE.json**. Exact final hashes are in release metadata.

## Accepted input / bounded result

Read AGENTS/manual/rules/workspace/state/NEXT, accepted c23/c22/c21/c20 runtime,
c23 call design/RDX and relevant code/tests, plus accessible Wisdoms references.
C23 accepts direct0x2B235D4 ->0x2CF7FE0, raw GPR0 tag1/mem32 Transform+0x10C;
target begin/end/hash unavailable, modeled_states0. The supplied acceptance does
not establish why lookup failed. Its partial EAX return mask1 and prefix escape0
are not complete CALL-path results. C23 A/E4078/4156ms retained verified Deinit,
retention, Position writes0 and hard kills0; c22 TEST/JE disjointness remains.

**Outcome C diagnostic preparation:** now classify the exact boundary failure,
and only if a strict single-target proof succeeds, feed admitted bytes to the
existing c23 effect sidecar. Actual c24 metadata/reason/span/hash/next barrier
remain **PENDING the new compact game log**. Cloud has no authenticated target
body/runtime table; nothing is reconstructed from hashes or proprietary bodies.
Local identity/RET completion does not settle XYZ/W, cross-invocation, ownership
or writability. No existing read/effect refusal is waived.

## Files / implementation

- read_target_identity.hpp + rdx_source_contract.cpp: guarded RVA/OS-lookup
  callbacks, exact build/direct-call/target pin, PE64/AMD64/image and unique
  readable executable nonwritable code section checks. Structural exception
  directory validation is bounded to262144 ordered/nonoverlapping entries in
  64-entry chunks, not a generic unwind interpreter. Compare containing PE and
  OS lookup entries; wrong-image/unreadable/interior/malformed/disagreement/cap
  refusals remain distinct. Existing exact runtime entry can retain c23 identity.
- Only target0x2CF7FE0 with no containing entry in both sources may attempt a
  no-unwind span. Bound by code/next static or dynamic runtime region and original
  128byte/32instruction caps. Double guarded read/equality; decode from the exact
  entry without skipping. Every preceding instruction must have one fallthrough
  edge and a supported c23 register-only effect, ending in plain C3 RET. Branch,
  indirect/nested CALL, memory/stack/partial/unknown effect, malformed bytes,
  missing RET or cap refuse exactly. A branch-around-first-RET trap is rejected
  at the branch. This is a modeled single-entry exit set, not symbol recovery.
- Runtime coverage is rechecked over the selected no-entry span (or matching
  normal entry). Hash only an admitted stable prefix; digest failure revokes
  admission. This is startup current snapshot identity, not historical-hash or
  subsequent per-invocation consistency/ownership proof. Candidate bytes cannot
  cross another authenticated runtime-function region; no other body is followed.
- writer_entry_snapshot.cpp: replace generic unavailable-boundary result with
  compact MOVEMENT_CALLEE_READ_TARGET_IDENTITY and guarded lookup/header readers.
  Existing READ_CALL reports admitted begin/end/size/hash; only admitted identity
  enters c23. Failure propagates its exact reason/RVA/mnemonic without consuming
  a nested effect state. No raw bytes/live data dump/game call/new hook/write.
- read_target_identity_smoke.cpp/test_read_target_identity.sh:9 owned sparse PE/
  code groups, full hooks integration. Version/info, README/README_TEST/BUILD,
  package/release metadata and selected identity log, accepted c23 source mirror,
  c24 analysis/gate, PROJECT_STATE and derived engine index updated. CODEX_NEXT
  preserved; normal updater/shared Overstrike/archive/log invariants unchanged.

Legacy State/Slice/c19-c23 value/alias/effect gates, native source lifecycle/
orientation, raw thunk/ABI/pairing/hardware cleanup/deadlines/assertions remain.
writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement
writes0. Producer0x351EB00 deferred. No movement/adhesion/camera/animation/physics,
SF/ordinary-mod implementation, guessed ABI, PAGE_GUARD, main merge or old deletion.

## Validation

- Warnings-as-errors Windows x64 build PASS.
- Full chain **13 portable + 7 DLL/logger + 149 hooks/native/contracts/workspace
  PASS records = 169 public PASS / 5 optional private-input SKIP**, all exit0.
- Nine new groups: strict no-entry completion/pin controls; first-RET/branch/CALL
  traps; memory/stack/partial refusal; static/dynamic crossings and overlaps;
  unavailable/unstable snapshot/coverage change; runtime entry start/interior/
  lookup mismatch/cap; invalid image/code/PE/table; byte/instruction/RET bounds;
  end-to-end identity -> c23 selected live/killed returns and failure propagation.
  All21 c21-c23 read/call groups, c20 EDI and c19 Position gates remain.
- Actual source ZIP fresh extraction: rebuild PASS; **84 focused entry/c14-c24 +
  7 DLL/logger + 13 workspace = 104 separate PASS records**, all exit0/no private
  inputs. Package verifier PASS separately; do not add duplicates to169.
- Final reporting-only label clarifies that the identity scope depends on method
  (normal runtime entry versus strict no-entry exit set). Main and extracted-source
  rebuilds PASS; extracted DLL/logger7 PASS recheck. No semantic change after
  full169/core104. Final native/tool sources match that rebuilt snapshot.
- Final ZIP CRC, payload/DLL/version/identity-marker hashes, full current sources/
  docs/analysis/licenses/helpers, exactly one virtual active install script,
  source private/compiled exclusions and unchanged task/native safety files pass.
  Final report docs are refreshed with a standalone package recheck.

Wisdoms ENGINE_INTERNALS records accepted c23 target/carrier and unresolved
identity; RE guide/WS internals preserve the strict no-entry method as
VERIFIED-STATIC implementation/owned-fixture scope only, actual c24 UNKNOWN.
Concurrent SF documentation was preserved without SF code changes. No private
raw logs, full target body, personal data or recovered game ABI is published.

## Next human step / specific missing evidence

Compact c23 evidence lacks code-region/runtime metadata, target bytes and the
actual failure stage. One new TARGET_IDENTITY record is necessary to distinguish
those causes; this is not a habitual rerun of completed target decoding.
Close game/Overstrike, run installed **MSM2-WS/Update-WS.cmd**, then shared
Overstrike **Install mods -> Run modded game**. Test **A/cardinal ~4s -> fully end
WallRun -> move/fast-travel -> E/diagonal ~4s**. Return newest complete private
**MSM2-WS/logs/MSM2-WS-v0.1c24-YYYYMMDD-HHMMSS.log**.
Only TARGET_IDENTITY is new; old READ/CALL/TEST/VALUE/WRITE/ALIAS/WRITER and
safety/finalization/cleanup stay in the same log. No body dump/video unless a
visible anomaly; no manual ZIP placement or unsolicited link needed.

---

# Codex result — c23 exact CALL / bounded register-only leaf (2026-10-25)

Branch: **codex/v01c20-edi-value-provenance**; accepted task base **16dc676**.
Implementation: **d33cccfffe38bc72e58b3fa32429c6953630f204**.
Version: **v0.1c23 / package0.1.41**. Outputs:
**MSM2-WS-v0.1c23.zip**, **MSM2-WS-v0.1c23-source.zip**,
**MSM2-WS-v0.1c23.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C23_RELEASE.json**. Final hashes are in release metadata.

## Accepted input / bounded result

Read AGENTS/manual/rules/workspace/state/NEXT, accepted c22/c21/c20 runtime,
c22 TEST and RDX contracts, relevant implementation/tests and Wisdoms references.
C22 accepts TEST r8b/r8b0x2B235BE -> JE0x2B235C1 with parent/flags/consumer tags0.
That resolves the pair only. The selected-read suffix stops at CALL0x2B235D4;
its target, call-state arguments and target body are absent from compact evidence.
Do not upgrade the other completed path's EAX return mask1 or prefix escape0
into complete CALL-path coverage. No private body is reconstructed/requested.

**Outcome C diagnostic preparation:** exact pinned call form/target and raw
carriers are now classified on the existing authenticated parent code snapshot.
If an exact target has a safe runtime-function boundary/current code hash and
recheck, a single128byte/32instruction register-only leaf can be evaluated.
Otherwise the exact call or nested instruction/alias/cap is reported. Actual
c23 target/effect/next barrier remain **PENDING the new compact game log**.
A local normal return does not settle downstream XYZ/W/cross-invocation or
ownership. Existing c19/c20 read/effect refusal remains unconditionally separate.

## Files / implementation

- rdx_source_contract.hpp/.cpp: ReadCallContract, exact callsite/return/target
  re-decode and per-state all-GPR/XMM/flags/visible-stack tag/provenance union.
  Conflicting origins are marked; displayed origin is the last state. Tags1/2/4
  still distinguish selected/post-write/other pre-write field values. No guessed
  typed game arguments. Unknown raw/loaded bits never become disjoint aliases.
- Optional leaf uses existing Slice register semantics plus separate tags, no
  generic CPU model. Full decode128bytes/32instructions,4096 cumulative nested
  visits, depth1. MOV/register vector copies, full self-XOR,32/64bit CMP/TEST,
  NOP/ENDBR/RET only. Memory, branches, further CALL, partial/unknown effects,
  stack changes and missing return stop precisely; failure discards temporary
  state. Explicit return/clobber masks replace no ABI assumptions. Unknown calls
  never clear volatile tags and continue. Modified masks cover analyzed prefixes;
  return masks cover modeled normal returns only.
- CALL return-address slot is an owned abstract-stack operation, not process
  memory mutation: bounded exact SP, protected return/source exclusion, temporary
  untainted return slot and restored SP. Raw GPR/XMM/flags survive according to
  decoded effects. Caller/home/Transform/source/unknown memory aliases refuse,
  with possible selected-value escape labeled conservatively.
- writer_entry_snapshot.cpp: pinned0x2B235D4 opts in only after old parent identity.
  At configure, one exact target may obtain unwind boundary and guarded code/SHA/
  byte recheck within128bytes. Parent/writer overlaps and functions containing
  deferred0x351EB00 are excluded before body reads; exact0x351EB00 before lookup.
  Identity means startup current snapshot, not a historical digest or subsequent
  per-invocation effect certificate. No new hook/live-data observation/game call.
- READ_CALL / READ_CALL_CARRIER (at most16) / READ_CALL_RETURN report compact
  metadata only, never full bytes/live register payloads. Existing READ_FLOW and
  c19/c20/TEST records remain. Formatter/type checks passed with4096byte limit.
- read_dependency_smoke.cpp:5 new groups retain16 c21/c22 groups. Version/info,
  README/README_TEST/BUILD, package metadata, accepted c22 source mirror, new c23
  analysis/gate, PROJECT_STATE and derived engine-reference checkpoint updated.
  CODEX_NEXT preserved. Normal Update-WS/shared manager/archive/log contracts stay.

Legacy State/Slice/readMayDepend/recordRead and c19/c20 certificates remain;
existing outer4096visit/9pending/8branch/16writer-input limits, lifecycle/
orientation/ABI/pairing/hardware cleanup/deadlines/assertions unchanged.
writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement
writes0. No producer traversal or movement/adhesion/camera/animation/physics,
SF/ordinary-mod code, guessed ABI, PAGE_GUARD or old-workspace deletion.

## Validation / persistence

- Warnings-as-errors Windows x64 build PASS.
- Full chain **13 portable + 7 DLL/logger + 140 hooks/native/contracts/workspace
  PASS records = 160 public PASS / 5 optional private-input SKIP**, all exit0.
- Five new groups cover direct selected/disjoint inputs, returned GPR/XMM/flags,
  exact self-zero clobbers, caller/home escape, Transform/source aliases,
  indirect/missing identity/partial/depth-two CALL, byte cap/no RET/indexed alias.
  Independently authored fake-address fixtures, not game code reconstruction.
- Final log field labels clarify analyzed-prefix modified masks. Rebuild and
  DLL/logger7 PASS recheck follow that reporting-only change; no new behavioral
  changes or weakened tests. Fixture variable-name collisions were corrected
  during development before validation; no production gate was changed for them.
- Actual source ZIP extracted fresh: build PASS; **75 focused entry/c14-c23 +
  7 DLL/logger + 13 workspace = 95 separate PASS records**, all exit0/no private
  inputs. Package verifier PASS separately. Do not add duplicates to160.
- Final archives verified for CRC, payload/DLL hashes/version/new CALL markers,
  current full source/docs/analysis/licenses/helpers, one virtual active script,
  source private/compiled exclusions, and exact unchanged task/native safety files.
  Final report/roadmap refresh uses identical tested native/tool inputs and a
  standalone package recheck; source95 covers final implementation.

An exec-server disconnect interrupted provisional packaging after the code/test
commit; reconnect verified the commit and completed test records remained intact.
No completion/publication was claimed during the interruption. A concurrent
same-branch6a6df0d extended only deferred roadmap docs; preserved with a normal
same-branch merge. No main merge or scope expansion; final source includes it.

Wisdoms ENGINE_INTERNALS accepts the supplied c22 TEST/JE result and next CALL;
RE guide/WS internals preserve the narrow-call method as VERIFIED-STATIC
implementation/owned-fixture scope only, actual c23 UNKNOWN. Concurrent SF docs
were preserved; no SF implementation or raw logs/private bodies copied.

## Next human step / why a capture is required

Compact current evidence lacks this exact target's bytes/boundary/hash and
call-state carriers; cloud has no authenticated game snapshot. New READ_CALL
metadata is therefore necessary, not a habitual rerun of resolved TEST work.
Close game/Overstrike, run installed **MSM2-WS/Update-WS.cmd**, then shared
Overstrike **Install mods -> Run modded game**. Test **A/cardinal ~4s -> fully end
WallRun -> move/fast-travel -> E/diagonal ~4s**. Return newest complete private
**MSM2-WS/logs/MSM2-WS-v0.1c23-YYYYMMDD-HHMMSS.log**.
Only the3 READ_CALL families are new requested fields; existing READ/TEST/VALUE/
WRITE/ALIAS/WRITER and safety/cleanup remain in the same log. No body dump or
video unless a visible anomaly; no manual ZIP placement or link needed.

---

# Codex result — c22 pinned TEST operand / flag flow (2026-10-25)

Branch: **codex/v01c20-edi-value-provenance**; accepted task base **a180b6b**.
Implementation: **14c0027b4d3c96c1ab9ea3dfae452b61650eedbd**.
Version: **v0.1c22 / package0.1.40**. Outputs:
**MSM2-WS-v0.1c22.zip**, **MSM2-WS-v0.1c22-source.zip**,
**MSM2-WS-v0.1c22.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C22_RELEASE.json**. Final hashes are in the release metadata.

## Accepted input / bounded result

Read AGENTS/manual/rules/workspace/state/NEXT, c21 read design and accepted c20
runtime/value/RDX evidence, relevant source/tests and accessible Wisdoms technical
references. Consolidated the owner's c21 compact acceptance from task/state
into V01C21_RUNTIME_RESULT.md: exact MOV4 Transform+0x10C -> EAX at0x2B23578,
possible use0x2B23581/control0x2B23583, prefix memory_escape0, next TEST0x2B235BE.
Do not extend prefix observations over the unanalyzed suffix. C20 EDI image-global
origin remains accepted; no semantic field name or pointer type is assigned.

**Outcome C diagnostic preparation:** compact c21 evidence omits TEST operands.
No game body is reconstructed/requested. The new authenticated snapshot diagnostic
reports exact operand names/classes/widths and parent tags, modeled flag effects,
local consumers, normal-return-flow coverage or exact next unsupported point.
Actual c22 operands/completion/next barrier remain **PENDING a new game log**.
Local possible flow never becomes a numeric downstream XYZ/W or cross-invocation
proof. The existing c19/c20 read/effect refusal is retained, even in dead-value
fixtures; no ownership/write permission is granted.

## Implementation / files

- rdx_source_contract.hpp/.cpp: one pinned ReadTestContract; same-width register
  TEST32/64 and opted-in read-only low8bit aliases, after existing identity/CFG/
  EDI gates. Decoder must confirm operand reads/EFLAGS-only write. Low-byte reads
  retain conservative whole-parent tags; they do not obtain exact byte provenance
  or general partial-register write semantics. High8/16bit/immediate/memory/indexed
  forms, unknown effects, partial writes and non-Jcc flag consumers still refuse.
- TEST does not read old flags, write operands or kill self-test dependence.
  ZF/SF/PF may depend on either AND operand; CF/OF0, AF undefined. Track through
  local Jcc, separate CF/OF-only conditions, kill provenance on later flag writes,
  retain both CFG legs. Selected/new/other field origins remain masks1/2/4.
- writer_entry_snapshot.cpp: current authenticated opt-in0x2B235BE and compact
  READ_TEST / READ_TEST_OPERAND / READ_TEST_FLAGS logs; existing READ_FLOW still
  reports return carriers/caller homes/control/escape/precise barrier. At most
  two operand records; no full instruction bytes or new observation hooks.
- read_dependency_smoke.cpp:8 new fixture groups plus8 existing c21 groups.
  Version/info, README/README_TEST/BUILD, package/release metadata, c21 acceptance,
  new c22 design/gate and source analysis mirrors synchronized. PROJECT_STATE
  records prepared c22; CODEX_NEXT preserved. Stale seed c21-pending statements
  explicitly superseded by actual acceptance; shared root16 clarification kept.

Legacy State/Slice/address and EDI models, readMayDepend/recordRead, c19/c20
certificates, native source lifecycle/orientation, ABI/entry pairing, deadlines/
assertions, hardware accounting and cleanup remain unchanged.4096visits/pass,
9pending,128stack,8branch/16writer-call bounds and no-call-expansion remain.
writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement
writes0. No producer0x351EB00, movement/adhesion/camera/animation/physics work,
SF/ordinary mod mutation, main merge or old-environment deletion.

## Validation

- Warnings-as-errors Windows x64 build PASS.
- Full required chain: portable **13 PASS**, DLL/logger **7 PASS**, hooks/native/
  c19/c20/c21/c22/workspace **135 PASS / 5 optional private-input SKIP**.
  **155 public PASS records / 5 SKIP**, every final suite exit0.
- 16 read groups include self-test/both operands, killed/dead/unrelated values,
  low8/DIL/64bit, all16 Jcc/CF-OF constants, overwritten flags, separate post-write/
  other-field tags, both reaching paths, partial-write/high8/16bit/immediate/
  indexed/non-TEST refusal, unsupported flag use/CALL and auth/disable/boundary.
- During fixture construction, an indexed negative placed before EDI's definition
  correctly stopped at the old EDI gate before TEST metadata. Place that owned
  negative after the native write to test the new sidecar, preserving gate order.
  A CALL case's reaching-state expectation was corrected to both CFG legs.
  No old assertion/deadline/production gate was weakened. Final full chain passed.
- Actual source ZIP extracted into a separate directory: fresh build PASS;
  **70 focused entry/c14-c22**, **7 DLL/logger**, **13 workspace** = **90 separate
  source-reproduction records**; all exit0/no private inputs. Package verifier
  also PASS; don't add these duplicate records to155.
- Final archive validation checks CRC/source completeness/analysis/manuals/licenses,
  exact payload/DLL/version/marker hashes, one virtual active install script,
  source exclusions and unchanged old task/lifecycle/ABI/source assertions.
  Final reporting/roadmap-only packaging changes use the same tested native sources.

Wisdoms RE guide/WS internals preserve the TEST/flag/partial-register method as
VERIFIED-STATIC implementation/owned-fixture scope; actual c22 remains UNKNOWN.
ENGINE_INTERNALS clarifies root16 is the dedicated executable-image globalRoot,
while raw entry-register tokens and loaded game semantic types stay separate.
No raw log/private body/personal data is published.

Publication fetch found same-branch concurrent5faacc4 (deferred controls/settings
roadmap only). Preserved it by a normal same-branch merge; FEATURE_ROADMAP is
included in the final source archive. No main merge or implementation change;
this roadmap explicitly grants no current gameplay scope.

## Next human step

Close game/Overstrike, run installed **MSM2-WS/Update-WS.cmd**, then shared
Overstrike **Install mods -> Run modded game**. Test **A/cardinal ~4s -> fully end
WallRun -> move/fast-travel -> E/diagonal ~4s**. Return newest complete private
**MSM2-WS/logs/MSM2-WS-v0.1c22-YYYYMMDD-HHMMSS.log**.
The three READ_TEST families are the only new fields needed; existing READ_FLOW,
VALUE/WRITE/ALIAS/WRITER and safety/finalization/cleanup stay in the same log.
No body dump/video unless a visible anomaly, no manual ZIP placement or link needed.

---

# Codex result — c21 bounded read-dependency diagnostic (2026-10-25)

Branch: **codex/v01c20-edi-value-provenance**; base **897381a**.
Implementation: **9b3b7b0ab9c1bfbd932693fceb60530fca77fe34**.
Outputs: **MSM2-WS-v0.1c21.zip**, **MSM2-WS-v0.1c21-source.zip**,
**MSM2-WS-v0.1c21.script.disabled**, **BUILD_MANIFEST.json**,
**analysis/V01C21_RELEASE.json**. Version **v0.1c21 / package0.1.39**.
Exact final archive/script/DLL hashes are recorded in the release metadata.

## Result / evidence scope

Read the required project manual/rules/state/task and c20/runtime/value/write/RDX
evidence, plus the accessible Wisdoms operational/engine/RE guides and WS references.
Accepted c20 EDI origin remains0x2B2356F -> image-global root16+0x0C323788, with
semantic meaning UNKNOWN. The current read0x2B23578 opcode/address/carrier are
not present in compact c20 evidence and are not guessed or reconstructed here.

Added a small diagnostic sidecar within the already authenticated137-byte callee
snapshot. Production opts in only for exact read0x2B23578 under calleeCurrent.
It reports operand/mnemonic/width/base/index/scale/displacement, modeled address
root/offset/class and overlap, destination carrier, both-path structural ordering,
and separate conservative possible-value-flow metadata. GPR/XMM/flags/caller
stack, later same-field reloads, possible first/control/escape uses and exact
unsupported/CALL/cap blockers are distinguished. Structural CALL fallthrough is
explicitly conditional on normal return; it is not a call-effect certificate.

**Bounded Outcome C diagnostic preparation:** existing earlier-read/effect
refusal is retained even if a local fixture value is dead. The sidecar cannot
grant/relax any alias, source-no-write, Position/effect, XYZ/W, ownership or write
certificate. Downstream relevance and cross-invocation feedback stay unknown.
Actual narrower game blocker/classification requires the new compact READ log
records. No private game body or new live observation/call/hook is added.

## Files / safety

- rdx_source_contract.hpp/.cpp: diagnostic fields and bounded private sidecar,
  called after existing identity/CFG/EDI checks. Legacy State/Slice, readMayDepend,
  recordRead, c19/c20 certificates and writer/caller/producer proof logic unchanged.
- writer_entry_snapshot.cpp: pinned opt-in and READ_DEPENDENCY/READ_FLOW reporting.
- read_dependency_smoke.cpp/test_read_dependency.sh plus full hooks integration:
  eight independently authored fixture groups; no captured function bytes.
- version.hpp/info.json, package/release metadata, new c21 analysis/gate, accepted
  c20 source mirrors, README/README_TEST/BUILD, AGENTS, PROJECT_STATE and derived
  engine-reference checkpoint synchronized. CODEX_NEXT is preserved.
- Logger smoke now derives its expected version from version.hpp while retaining
  independent path/time/suffix/filesystem assertions. Production logger unchanged.

Native lifecycle/orientation, raw entry/hardware/ABI/pairing/accounting/cleanup,
exact-build/current-code/region admission and all deadlines/assertions remain.
writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/
downstream_conflicts_proven_absent0; Position/source-position/input/movement
writes0. No producer0x351EB00, movement/adhesion/camera/animation/physics work.
No SF/ordinary mod/toolkit change, no main merge, no old-workspace deletion.

## Validation and a non-reproduced existing fixture failure

- Warnings-as-errors Windows x64 build PASS.
- Final required chain: portable **13 PASS**, DLL/logger **7 PASS**, full
  hooks/native/c19/c20/c21/workspace **127 PASS / 5 private-input SKIP**;
  **147 public PASS / 5 optional SKIP**, all final suites exit0.
- Eight c21 fixture groups cover dead and surviving pre-write values/protected
  writer carrier, caller homes/partial overwrites, post-write/new-value reloads,
  mixed branch presence/field order, disjoint source/stack/image, unknown/indexed/
  conflicting addresses, flags/control, partial registers, calls, boundaries,
  loops, cap and disabled/auth-negative controls. Existing c20/c19 checks retained.
- An earlier final-chain attempt stopped at unchanged
  native_source_override_smoke.cpp:376 (exact stale suspend/resume/write counters).
  The unchanged focused native chain then passed9 records, and unchanged full
  rerun passed127/5. **Root cause UNKNOWN/not separately measured**; do not claim
  it was fixed, lower a deadline or weaken the assertion. This fixture/source
  path was not changed. Record the transient rather than hiding a retry.
- Actual source-ZIP fresh extraction: rebuild PASS; **62 focused entry/c14-c21**,
  **7 DLL/logger**, **13 workspace** = **82 separate source-reproduction records**,
  all exit0/no private inputs. Do not add these duplicates to147.
- Final packaging checks CRC, complete current source/docs/analysis/licenses,
  manifest/script/DLL hashes, new compiled READ markers, single virtual active
  install script, no live/compiled/private body members in source and correct
  Organization helper/log/version contracts. Final native sources match that
  tested snapshot; final reporting/package-check text changes are revalidated.

Wisdoms RE guide and WS internals checkpoint are synchronized with the reusable
method (order/possible flow/flags/caller stack/certificate separation), explicitly
implementation/owned-fixture scope only. Actual c21 engine read result remains
UNKNOWN. Detailed proof stays in this project; no raw logs/personal data copied.

## Next human step

Run installed **MSM2-WS/Update-WS.cmd**, then shared Overstrike **Install mods ->
Run modded game**. Test **A/cardinal ~4s -> fully end WallRun -> move/fast-travel
-> E/diagonal ~4s**. Return the newest complete
**MSM2-WS/logs/MSM2-WS-v0.1c21-YYYYMMDD-HHMMSS.log** privately.
Only READ_DEPENDENCY/READ_FLOW are new requested keys; existing VALUE/WRITE/ALIAS/
WRITER and Deinit/retention/Up/stale recovery/hard0/Position0/entry-map-finalization/
cleanup records remain part of the same log. Video only for a visible anomaly.
No link or manual ZIP placement is needed for normal updater-based handoff.

---

# Codex result — updater-first handoff, no unsolicited links (2026-10-25)

Branch: **codex/v01c20-edi-value-provenance**; policy base **c30acd3**.
The owner replaced the previous direct-download preference: after verified
commit/push, prompt Update-WS.cmd and give a short task-specific test/log request.
Do not send download/repository/test links unless explicitly requested.
AGENTS and DEVELOPMENT_MANUAL are synchronized; historical entries below retain
the prior preference as history. Detailed artifacts/tests/hashes stay in GitHub.

Documentation-only change: whitespace and policy consistency reviewed; no native
or tool source, version, package, CODEX_NEXT or PROJECT_STATE changes, and no
runtime suite rerun. Current diagnostic implementation/delivery remains cad74ab;
actual EDI A/E is still pending. When that build is tested, use the accepted
Update-WS -> Install mods -> Run modded game flow, A~4s -> fully end -> move -> E~4s,
and the newest complete versioned MSM2-WS/logs file. This policy-only update does
not itself require another build/game test.

---

# Codex result — direct-download handoff preference (2026-10-25)

Branch: **codex/v01c20-edi-value-provenance**; policy base **0c1b942**.
Existing diagnostic implementation/package publication remains **cad74ab**.
AGENTS and DEVELOPMENT_MANUAL now require direct archive-download links as the
preferred release handoff, separate GitHub test links, and real GET/CRC/hash
validation instead of treating a blob preview or authenticated Git read as a
browser-download test. Failed direct access must be disclosed; the existing
updater/authenticated Git route remains available without changing visibility.

Current cad74ab install ZIP was fetched via its pinned raw.githubusercontent URL:
HTTP200/application-zip, **3460868 bytes**, ZIP CRC PASS, SHA256
`f64bab71482c74d47cfbb09ce0e9b02fa8602ed5461ceac9834232bc2b15007e`.
The github.com/raw form returned404 in this environment and is not advertised
as verified. These are current cloud observations, not proof of the user's
browser permissions. No native/source/tool/version/package/metadata change and
no runtime suite rerun; output is documentation only. The next user action is
unchanged: keep public/extracted ZIPs outside Mods Library, use installed
Update-WS.cmd for deployment, then the documented A/E capture.

---

# Codex result — c20 authenticated EDI diagnostics enabled for A/E (2026-10-24)

Branch: **codex/v01c20-edi-value-provenance**.
Base: **b49425e** (same branch only; no main merge).
Implementation: **5e894c1a99932554d73f95954ae1a1becfbf6ad1**.
Outputs: **MSM2-WS-v0.1c20.zip**, **MSM2-WS-v0.1c20-source.zip**,
**MSM2-WS-v0.1c20.script.disabled**, **BUILD_MANIFEST.json** and
**analysis/V01C20_RELEASE.json**. Version/package remain **v0.1c20 / 0.1.38**;
these are new diagnostic payloads replacing the previously paused c20 artifacts.
Exact final archive/DLL/script hashes are recorded in the release metadata.

## Result and change scope

Read AGENTS/DEV_RULES/LOCAL_WORKSPACE/PROJECT_STATE/CODEX_NEXT and the required
c19 write/runtime, prepared c20 and accepted shared-runtime evidence. The current
task explicitly reauthorizes the existing9057d24 bounded local EDI pass after
real-PC workspace acceptance. The algorithm was already present and is retained.

writer_entry_snapshot.cpp now sets writerAlias.edi.enabled to **calleeCurrent**,
the existing complete current-code/boundary/hash authentication result, instead
of the infrastructure pause's false. VALUE_CONTRACT reports enabled explicitly.
All existing exact exe/callsite/callee/current-code checks still apply; local EDI
coverage and whole-writer call/effect coverage remain separate. No native ABI
guess, new observation, function-body publication, hook or game call was added.

package.py now describes diagnostic activation, accepted workspace state, raw-bit
versus pointer identity, exact-affine policy and pending actual-game conclusions
honestly. It includes the current validation and derived acceptance notes.
README/README_TEST/BUILD, the c20 analysis/gate (and source mirrors), info.json
and AGENTS activation note are synchronized. Current public suite logs replace
the prior selected logs. EDI implementation/legacy State/Slice, c19 read/region
checks, native lifecycle/orientation, entry thunk/ABI/pairing/finalization/cleanup,
current source/update/launcher/backup tooling and SF code are unchanged.
CODEX_NEXT and PROJECT_STATE are preserved.

## Validation

- Windows x64 cross-build: **PASS**, warnings-as-errors.
- Required serial chain: portable **13 PASS**, DLL/actual Windows-filesystem
  logger smoke under Wine **7 PASS**, hooks/native/c19/EDI/workspace **119 PASS**,
  for **139 public PASS / 5 optional private-input SKIP**. All exit0. The six
  prepared EDI tests and13 workspace tests are included, not added again.
- EDI tests retain exact raw32 copies/constants/memory/spills/affine relations,
  both local branches/equal definitions, conflicting origins, partial/chain/
  tombstone/auth/control refusals and later CALL/read blockers with honest coverage.
- Fresh actual source-ZIP extraction: **build PASS**, **54 focused entry/c14-c20
  records**, **7 DLL/logger**, **13 workspace**, all exit0 with no private inputs.
  Native source is unchanged after that tested snapshot. Final packaging also
  preserves concurrent same-branch AGENTS988a7c5, reads DEVELOPMENT_MANUAL, and
  includes that required operational manual in the source documentation allowlist.
  Final packaging/current source bytes are checked again; no main merge occurs.
- Public install/source ZIP CRCs, current source/helpers/analysis/licenses,
  enabled manifest/DLL marker, script/DLL hashes and single virtual active script
  are verified. Source ZIP has no active script or compiled/private game-body
  members. Install/source metadata use the Organization remote and accepted logger.
- Owned A/E orientation fixture: sessions4102/4104ms, verified Deinit/retention/
  Up1/stale0/hard0, Position writes0, clean watch restoration; these are synthetic
  owner fixtures, not current game EDI evidence. The separate owned entry fixture
  retains8entries/7complete maps/XYZ7/sourceW0/destW1 and finalized cleanup.

## Safety / remaining real-game test

Keep writable_source_proven/caller_owned/ownership_exclusive/
source_lifetime_exclusive/downstream_conflicts_proven_absent at0 and
Position/source-position/input/movement writes0. Producer0x351EB00 stays deferred;
no movement/adhesion/camera/animation/physics work. A real Transform+0x10C write
does not gain literal transform_no_write_proven. Unsupported/conflicting/capped
or control effects still refuse; no gate/assertion/deadline was weakened.

**Actual c20 producer/origin/dependency/XYZ/W remain PENDING.** Do not upgrade
owned tests or enabled=1 into an actual-game value or writable-source proof.
Existing c19 A3844ms/E4094ms and accepted shared startup/logs remain historical
accepted evidence. No raw user log or captured game function is redistributed.

Next user action: close game/shared Overstrike, run installed **MSM2-WS/Update-WS.cmd**,
then shared Overstrike **Install mods -> Run modded game**. Desktop Mod-Tools
launchers do not pull/rebuild the project. Run **A/cardinal ~4s -> fully end
WallRun -> move/fast-travel -> E/diagonal ~4s**, then privately return the newest
complete `MSM2-WS/logs/MSM2-WS-v0.1c20-YYYYMMDD-HHMMSS.log`.
Review VALUE_CONTRACT/DEFINITION/CHAIN and exact later WRITE/ALIAS/WRITER blockers,
local versus full coverage, retention/Deinit/Up/stale/hard kills/Position0 and
entry/map finalization/cleanup. Video only for visible regression.

---

# Codex result — shared Mod-Tools manual and specifications (2026-10-24)

Branch: **codex/v01c20-edi-value-provenance**.
Documentation commit: **735c6fe17bd40ec9db5eef53baffedc252a8b913**.
Documented implementation: Mod-Tools v1.1.0, commit
**845e33cafecae352e5194d790358500bb0066081**.

The user requested a specification for each of the three shortcuts as future
modification references, then selected GitHub as the sole documentation
destination. This is a documentation task, not the separate native c20 task.

## Outputs and discoverability

- docs/mod-tools/README.md: index, operation selection, installation/update,
  common layout/dependencies/settings/locking, moved shortcut behavior and
  maintenance rules.
- OVERSTRIKE.md: start/initial setup/layout checks and manager reuse contract.
- PLAY_MODDED.md: profile/installed-DLL gates, -scripts handling, existing-game
  reuse and post-restore installation acknowledgement.
- BACKUP_MODS.md: complete private capture scope, manifest/publication/verification,
  recovery/rollback, exclusions, limitations and format compatibility.
- MANUAL.md: complete combined edition with anchors and implementation links
  pinned to the documented source commit; individual and combined editions are
  maintained together.
- Root README links the manual; AGENTS directs future agents to the relevant
  specifications before shared-tool changes. This delivery updates CODEX_RESULT.

Each specification lists inputs/outputs, exact entrypoints/functions, operation
order, side effects/refusals, modification points and regression/human checks.
The documents distinguish a launch request from actual application readiness,
shortcut presence from execution acceptance, and metadata change detection from
an atomic live snapshot. No private user log or backup is included.

## Verification and next use

Reviewed the current Python/CMD/PowerShell implementation and existing test report.
Checked six manual/root Markdown files, all29 local links/anchors, balanced code
fences, whitespace, referenced Python function existence and the AGENTS discovery
path. Git whitespace checks pass. Existing27 Python/16 PowerShell results are
identified as prior v1.1.0 evidence; no runtime suite or Windows action was rerun
for authored Markdown. Source/runtime/KIT/version/release metadata and ZIP bytes,
CODEX_NEXT and PROJECT_STATE are unchanged. Output is the Markdown manual set;
no new binary package/version is produced.

Future agents should begin at docs/mod-tools/README.md and select the relevant
specification; use MANUAL.md for comprehensive browsing. Actual full backup and
Vanilla recovery remain separate Windows acceptance tasks. GitHub is the user's
selected destination; no ChatGPT library registration is requested or claimed.

---

# Codex result — one-click shared Overstrike startup (2026-10-24)

Branch: **codex/v01c20-edi-value-provenance**.
Implementation: **845e33cafecae352e5194d790358500bb0066081**, based on the
owner-confirmed Organization baseline 5192f05 and recorded branch policy e884fed.
Output: **MSM2-Mod-Tools-v1.1.0.zip**, 41244 bytes,
SHA256 `24262407787c3c757e4d1240435e6d842c288c3d23e3ce12796e789838b0d219`.

The user requested double-click preparation through Overstrike startup and chose
ordinary environment/layout checks, rather than updating/building WS at launch.
The existing actual-game installation/startup/log acceptance remains accepted.

## Change / use

Run `_source/environment/Start-Overstrike.cmd` once after pulling this branch
with game and Overstrike closed. It derives the game root from the authoritative
source layout (or asks for it outside that layout), checks shared EXE/loaders,
the MSM2 profile and WS layout, installs/refreshes only its own Mod-Tools files
and three owned shortcuts, then opens shared Overstrike with cwd=game.
Existing backup destination/profile and post-restore confirmation are preserved;
first-time backup defaults to home/MSM2-Mod-Backups without making a snapshot.

Thereafter double-click **MSM2 - Overstrike** on the desktop. It validates the
same paths, requires one root versioned WS script, refuses visible WS archives,
links and update/Git locks, and launches/reuses only the matching shared manager.
It does not pull Git, build/test/deploy WS, edit SF/ordinary mods, change profiles,
stop processes or start the game. Use Install mods / Run modded game in Overstrike.
Wrong layouts preserve all files and display an error instead of cleaning them.
Matching running Overstrike is activated; a foreign process path is refused.

Files: new environment/Start-Overstrike.cmd; environment engine/Windows adapter,
Python/PowerShell tests, README/TEST_REPORT; root README and shared recovery docs.
This delivery updates CODEX_RESULT, toolkit ZIP and MOD_TOOLS_RELEASE metadata.

## Validation / limits

- **27 Python tests PASS** (owned real filesystem/Git): existing backup/restore
  regressions plus new first-time setup/upgrade, preserved settings/dirty work,
  repeated startup without reinstallation/full scan/Git, path inference, and
  duplicate/archive/lock/loader/profile/unrelated-toolkit refusals.
- **16 PowerShell checks PASS** with fake launch/process/COM boundaries: exact
  EXE/CWD, same-manager reuse, three shortcut targets/icon/CWD, collision refusal
  before saving any shortcut, existing game protections and syntax parsing.
- Seven CRLF CMD entrypoint contracts and Python syntax pass. Distribution CRC,
  payload hashes/allowlist pass; standalone extraction rebuilds identical ZIP.
- Actual Windows desktop COM and this new launcher remain user acceptance checks.
  Linux/Core fake-host tests do not prove the user's shortcut was installed.

No main merge, SF code change or native runtime/version/binary package change.
WS stays v0.1c20/package0.1.38; EDI remains disabled, ownership/exact-build gates
and zero Position/source/input/movement writes are preserved. During publication,
same-branch upstream commits 013cbfa/2e7ac57 updated CODEX_NEXT/PROJECT_STATE to
reauthorize the separate c20 diagnostic task. Those files were read and retained
verbatim by merging only this development branch, without merging main or
overwriting the upstream task. This user's current launcher request does not
execute that separate native implementation. No native139-suite rerun was needed for shared
launcher-only changes. Next human step: one-time launcher setup, then confirm
the desktop shortcut opens the existing shared ordinary+SF+WS Overstrike UI.

---

# Codex result — Organization development baseline accepted (2026-10-24)

Repository: **deluxxxe-dev/MSM2-WallSprint**.
Branch: **codex/v01c20-edi-value-provenance**.
Owner-confirmed implementation baseline:
**5192f05f32313919fdf4049bd734b971655d163f**.

## Result and files

Fetched only the designated development branch from the Organization remote and
fast-forwarded the cloud checkout from 13c76f0 to the exact supplied baseline.
Updated this checkout's origin to the canonical Organization URL. No main merge,
history rewrite, reset or removal of existing untracked artifacts occurred.
The baseline's only change since 13c76f0 is workspace.py's canonical remote.

AGENTS.md now persists the owner's branch/source-of-truth, baseline and no-main-
merge instructions for later isolated tasks. This CODEX_RESULT entry records the
review and verification. No source, runtime, version or package changes were
needed in this handoff; existing output names remain MSM2-WS-v0.1c20.zip,
MSM2-WS-v0.1c20-source.zip and MSM2-Mod-Tools-v1.0.0.zip. Existing archives were
not regenerated and should not be represented as new baseline builds.

## Reviewed state and validation

- Read AGENTS, DEV_RULES, LOCAL_WORKSPACE, PROJECT_STATE and CODEX_NEXT in order,
  plus LOGGING_CONVENTION, BUILD, current result, c19 runtime evidence, c20
  prepared research/gate and the shared-runtime acceptance analysis.
- GitHub's designated branch resolves to the exact supplied full baseline SHA;
  source/tools/workspace.py uses the Organization remote.
- `python3 source/tools/test-workspace.py`: **13 PASS records, exit 0**. Owned
  filesystem/Git fixtures cover migration, fast-forward updates, rollback,
  independent archive-excluding clones, one active script, protected sibling
  preservation and fail-closed checks. They do not launch the real game.
- Native runtime files are unchanged. No new native build/gameplay regression or
  actual Windows run is claimed for this documentation handoff.

## Next work and unchanged gates

The latest derived actual-game evidence in
analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md accepts shared startup,
loading and versioned WS logs. PROJECT_STATE/CODEX_NEXT still describe the older
acceptance wait; this handoff preserves them as instructed by AGENTS rather than
silently rewriting the gameplay task. Both c20 research documents require an
explicit CODEX_NEXT reauthorization before research resumes. Production
writerAlias.edi.enabled remains false; the unresolved edge is EDI at 0x2B2358E,
not an authorized movement source. Preserve exact-build and all ownership gates,
zero Position/source/input/movement writes, and deferred producer 0x351EB00.

Continue future work from this branch and its subsequent commits. The next
gameplay task needs an explicit updated CODEX_NEXT using the accepted workspace
evidence. Separately, Mod-Tools' real Windows shortcut/backup/restore acceptance
remains pending; preserve the healthy environment. SF development stays with
SF Codex. No new runtime test or local migration is requested by this handoff.

---

# Codex result — shared launch and private environment recovery

Branch: **codex/v01c20-edi-value-provenance**. Toolkit implementation commit:
**5d089e628b5eb6ef60c22e1133720abcd1e4b68e**.
Output: **MSM2-Mod-Tools-v1.0.0.zip**, independent of unchanged WSv0.1c20/package0.1.38.

The user delegated SF development back to SF Codex and explicitly requested
simpler future startup plus a complete private shared-environment backup/restore
for the Vanilla game at the original path. The user selected full scope including
ordinary mods, Overstrike/settings, SF/WS source/Git histories/logs and loaders,
excluding game data. No gameplay/EDI/SF implementation advancement is authorized.

## Result

`environment/` adds six CMD entry points, a standard-library Python engine and a
small Windows host adapter. Install copies only its own toolkit/local config to
<game>/Mod-Tools and creates owned desktop Play/Backup shortcuts. Fast launch
validates the selected MSM2 profile, loader and installed DLL list, starts the
game with Overstrike-equivalent CWD/-scripts behavior and reuses a matching game
instance. Open-Overstrike uses only the existing shared manager with cwd=game.
No normal launch pulls Git, rebuilds, updates either project or kills processes.

Explicit shared backup preserves documented whole mod roots, hidden files,
empty directories, independent Git storage, dirty/untracked work, ordinary mods
and logs. It writes a new external timestamped folder, inventories/hash-checks
all files and verifies the archive before publishing it. The folder includes a
standalone recovery engine. Shared operation locking, process checks, source
stat/list checks and external-location enforcement protect against known changing
captures and recursively scanned backup archives. Private snapshots/rollback
folders are also ignored by Git; the utility performs no uploads.

Restore validates the original absolute game path and EXE hash, verifies/stages
before replacing selected roots, preserves current data in a sibling rollback
folder and reverses replacement failures. Incomplete rollback preserves both
locations and reports failure. There is no game-EXE/asset/save/WSL/toolchain copy,
relocation guessing or third-party content in the public toolkit ZIP. Complete
Install mods in restored Overstrike to reapply asset modifications; one explicit
installation confirmation then releases the fast-launch gate. No current user
backup has been created by the cloud and no live PC restore is claimed.

## Files / scope

New environment/{mod_environment.py,windows.ps1, six CMD entry points,README,
TEST_REPORT,LICENSE,build_distribution.py, two tests}; docs/MOD_ENVIRONMENT_BACKUP.
AGENTS/DEV_RULES distinguish this explicitly user-authorized shared utility from
WS-only updater cleanup; LOCAL_WORKSPACE/README link its optional sibling layout.
This delivery adds private-backup Git ignores, this result and release metadata/ZIP.
The existing source/ subtree, WS runtime/version/public WS packages,
CODEX_NEXT/PROJECT_STATE and the SF repository are unchanged by this task.

## Validation / remaining human check

- **21 Python filesystem/Git tests** pass: complete dirty/history/data round trip,
  recovery with the original game removed/recreated as Vanilla at the same path,
  source/target/archive symlink and malformed/CRC-valid-but-hash-invalid ZIP refusals,
  wrong build/path/process/lock refusals, concurrent change detection, synchronous
  rollback and incomplete-rollback preservation, readonly/mtime/empty-directory
  recovery, preserved newer work and explicit post-restore installation gate.
- **11 PowerShell launch-contract checks** pass with owned mocked OS boundaries,
  plus parsing; correct EXE/CWD/-scripts, commandline handling, same-instance reuse,
  active/foreign-process protection. Actual Windows COM/shortcuts/process launch
  and long-path restoration remain pending, not proven by Core/Linux checks.
- Six CMD wrappers are CRLF, use absolute engine paths, forward arguments and move
  cwd outside replaced toolkit directories. Python syntax checks pass.
- Public ZIP CRC/full-file hashes/allowlist pass; a fresh standalone extraction
  rebuilds the exact same37782-byte ZIP without the repository. The manifest records
  source commit and SHA256. Only authored source/text/license/tests are included.
- Existing WS native source and binary assets are byte-identical to the accepted
  runtime delivery. No native139-suite rerun is justified by these separate tools.

Next user step: close game/Overstrike, fast-forward the existing preparation clone
and run environment/Install-Mod-Tools.cmd for the known game root. Choose an
external backup folder, verify shortcut creation/ordinary launch, then create and
verify the first real snapshot. Keep the healthy environment; do not delete it
merely to test restoration. User-facing procedure and limitations are in
[environment/README.md](../environment/README.md); tests are in
[environment/TEST_REPORT.md](../environment/TEST_REPORT.md). SF work stays in SF Codex.

---

# Codex result — final local WS workspace migration

Branch: **codex/v01c20-edi-value-provenance**.
Implementation commit: **a4ec9c0f2635e9f2594bf5972d1765332de73766**.
Latest instruction base: **4402729a53eceeede6476c24d814bfd8469978fa**, merged without history rewrite. Prepared c20 research is preserved in **9057d24** and remains production-disabled; accepted c19 implementationb3185cd/deliveryba0b1d7/runtime evidence retained. No main changes/force push. CODEX_NEXT is preserved from the latest infrastructure task; AGENTS/DEV_RULES/LOCAL_WORKSPACE/LOGGING_CONVENTION/PROJECT_STATE updated as explicitly authorized.

## Outcome

**v0.1c20 / package0.1.38: the latest real-PC diagnostic identifies44.69ms pre-installation thread-list setup against a30ms deadline. The test-only correction prepares a real OS snapshot before the synthetic commit, retains real hardware capture/deadline checks, and adds slow-setup success plus delayed-snapshot refusal controls. The full external pipeline passes139 public tests /5 optional private-input SKIP. The corrected real-PC focused run now passes22 records /1 optional SKIP with exit0. The full corrected real-PC pipeline now passes139 tests /5 optional private-input SKIP and deploys successfully; shared Overstrike shows exactly1 WallSprint alongside ordinary mods. The subsequent successful SF update and shared screenshots confirm exactly1 checked SocialFeedProbe and1 checked WallSprint alongside ordinary mods. The user now confirms game launch/WS operation and supplies both new versioned logs: installation, shared loading and workspace logging are accepted. WS short sessions1594/2234ms pass retention/Deinit, zero hard kills and Position writes; SF bootstrap works but handle candidates remain0. EDI/gameplay research remains paused.**

The authoritative shape is game-root Mods Library/Mods-Developing/MSM2-WS with one root MSM2-WS-v<version>.script, Update-WS.cmd, logs, independent _source clone and disabled _archive. A one-time Migrate-WS.cmd is included. Shared Overstrike is only game-root/Overstrike/Overstrike.exe with game-root WorkingDirectory. No AppData/project-specific copy/profile/library is created. No old D-drive environment, SF or ordinary mod is cleaned, backed up, moved, renamed or overwritten by WS tooling.

## Changes / files

- New root and source/tools/windows Migrate-WS.cmd / Update-WS.cmd call repository-owned source/tools/workspace.py via Windows Python3.11+. Migration accepts an explicit validated game-root/branch, creates only WS folders and clones the intended WallSprint remote with no local/shared-object optimization. Existing correct clones are validated, never replaced by a legacy .git copy.
- Independent clone checks reject root/nested symlink/junction/reparse points, worktree/gitdir files, external common-dir/alternates/core.worktree, wrong remote, detached/unexpected branch, dirty tree and active source scripts. Expected branch is explicit/stored in local Git config; matching upstream and git pull --ff-only are mandatory. No hard-coded future branch or personal drive dependency. Git long paths enabled for the clone.
- Update validates the exact workspace/game/shared paths, stops only the validated game/shared Overstrike using reviewed PowerShell, and refuses a different running Overstrike instead of killing it. Shared Overstrike must exist; Overstrike/winmm.dll and scripts_proxy.dll duplicates refuse; game-root scripts_proxy.dll and winmm.dll are mandatory. Shared binaries are never repaired/copied by WS tooling.
- WindowsSystem invokes configured WSL2 and source/tools/windows/workspace-build.sh. The same pinned Linux cross-build, portable tests, actual DLL/logger smoke, full hooks/native plus new workspace suites, and package/release validation run in a temporary WSL source copy outside Mods Library. Missing tools fail; no MSVC/CMake/prebuilt/skip-tests shortcut. Toolchain uses the existing WSL /workspace/toolchains prefix, not a legacy checkout. Inherited private capture/toolchain overrides do not become old-drive dependencies.
- Transactional deployment stages validated .script.disabled under _archive/_build, verifies authoritative header/manifest/ZIP/DLL hashes, source cleanliness/HEAD and layout, then archives only positively WS-owned old root scripts as .script.disabled and atomically deploys one versioned root script. Postcondition asserts exactly one active script recursively, zero in _source/logs/_archive. Failed deployment/launch rolls back prior active bytes/helper where possible; failed preflight/build/test/package never starts replacement. Update lock rejects concurrent/stale update state. Source Git changes are not discarded.
- Tracked root WallSprint.script removed without history rewrite; .gitignore/output rules prevent live script materialization. Existing generated cloud cache scripts were renamed to disabled names preserving bytes. package.py writes only versioned .script.disabled; release.py virtualizes the install ZIP's one active versioned member, never unpacking it into source. Historical ZIPs remain. Source ZIP contains no active script.
- Runtime GameInterop/log_file.hpp now appends exact Mods Library/Mods-Developing/MSM2-WS/logs to its executable root, creates/checks each directory and rejects blocked/reparse components. Filename MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log comes from authoritative version/local time, numeric -01..bounded9999 collisions use CREATE_NEW, no overwrite/fallback. Same loaded-module A/E/re-enable append retained. Errors keep stage/path/Win32 code via debug output; telemetry payload/UTC line clock/native policy unchanged.
- log_file_smoke.cpp/smoke_windows.sh test new actual Windows Unicode/space nested directories, version/timestamp/collision/reopen/failure behavior and real DLL integration. test-workspace.py adds8groups of actual owned Git/filesystem fixtures including ff updates/divergence, independent clone, active/disabled invariants, rollback, protected siblings and alias/lock/reparse/dirty/preflight failures. Existing suites retained. New Windows wrappers, LF/CRLF attributes, source/build docs, README_TEST and audit synchronize the final workflow.
- Prepared EDI code/offline tests remain preserved but writer_entry_snapshot.cpp explicitly leaves edi.enabled=false. No EDI/gameplay/producer advancement is claimed. Superseded UpdateForTest/AppData tooling was removed from the current tree and remains in Git history. Mod description now identifies infrastructure-only behavior.

## Validation

Final Windows x64 warnings-as-errors build PASS. Serial public results:

```text
Portable ASan/UBSan                        13 PASS
DLL smoke + actual Windows logger          7 PASS
Full hooks/native/prepared/workspace      111 PASS /5 private-input SKIP
Total public PASS records                 131
Entry/ABI/hardware/finalization/ranges      11 PASS (included)
C14 contract                               4 PASS (included)
C15 writer branch                          6 PASS (included)
C16 call/effect                            7 PASS (included)
C17 alias                                  7 PASS (included)
C18 local callee CFG                        6 PASS (included)
C19 write contract                         7 PASS (included)
Prepared EDI offline                        6 PASS (included; production disabled)
Workspace Git/filesystem                    8 PASS (included)
Logger filesystem                          3 PASS (included in DLL smoke)
Separate historical private replay          4 PASS records
```

Historical replay contains1duplicate baseline and separately resolves1optional public SKIP; other4 absent historical private inputs stay skipped. Those444/128byte reset-copy functions are not the current137/183byte callee/writer. No raw capture is executed/redistributed.

Workspace fixtures use temporary owned Unicode/space game/remote/legacy/SF/ordinary-mod trees. Names, hashes, sizes and mtimes of sibling SF/ordinary/legacy data remain unchanged. Fresh no-reference clone and fast-forward next-version deployment, archive collision/idempotence and exact one-active postcondition pass. Detached/dirty/unexpected/diverged/reparse/worktree/alternates/active-source/foreign-script/duplicate shared DLL/lock failures pass. Stop/build/test/package/script hash/DLL hash/launch failure leaves or restores prior active script bytes; no real game process or user C:/D: folder is touched. Real Windows OS process stopping/WSL/GUI remains a local acceptance item.

Owned entry ABI still preserves GPR/YMM/XMM/x87/MXCSR/flags/stack/nonvolatiles/LastError/returns;8captures/7complete concurrent+nested maps/28paired stores/two threads, XYZ7/sourceW0/destW1 and finalized1/0 summaries. Owned A/E integration retains >=4s verified Deinit/retention/Up1/hard0/Position0/12events and clean DR restoration; measured counters in analysis/V01C20_RELEASE.json. Fixtures cannot pass actual-game admission.

Staging wrapper: **PASS: full real staging wrapper ran unchanged required build/portable/DLL/hooks/workspace/package chain from an owned Unicode/space source path and returned validated disabled payload/ZIP/manifests outside the checkout.**

Standalone source ZIP: **PASS: fresh source ZIP rebuilt DLL and passed54 focused +7 DLL/logger +8 workspace records, preserved finalized1/0 logs; no builds/private inputs or active scripts in source.** Archive CRC/current complete code/docs/helpers/licenses, nested script/DLL hashes, no-live-source and private-body exclusion are recorded in V01C20_RELEASE.json. Published ZIP/metadata are verified through fresh GitHub clone, not an anonymous HTTP/browser test.

## Safety / research preserved

Accepted c19 actual A3844ms/E4094ms, normal Deinit/retention/Up1/stale0/hard0/Position0,8entries/maps each and XYZ7/sourceW0/destW1 remain authoritative. Exact blocker is still MOV0x2B2358E [RCX+0x10C],EDI with unresolved EDI provenance. Prepared research9057d24 is inactive; c20 EDI is the next gameplay-research task only after migration acceptance and explicit CODEX_NEXT reauthorization.

Exact exe2.810.0.0/SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9, callee0x2B23560..0x2B235E9/hash2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d/current full code/boundary and all lifecycle/orientation/stale/DR/direct-call/source/alias gates retained. Native source/lifecycle/c12 analyzer/raw thunk/ABI/pairing/stop/cleanup/old State/Slice/caller/producer are unchanged. No gate/assertion/timing weakening. Existing snapshot non-atomicity/incomplete hardware/close-overlap/bounded refresh limits remain.

writable_source_proven/caller_owned/ownership_exclusive/source_lifetime_exclusive/downstream_conflicts_proven_absent0; Position/source/input/movement writes0. No producer0x351EB00 traversal, new game call/observation/PAGE_GUARD/data-page mutation, movement/adhesion/camera/animation/physics work. Runtime changes are limited to log destination/name.

## Outputs / first local action

- MSM2-WS-v0.1c20.zip (virtual versioned .script + complete migration bootstrap)
- MSM2-WS-v0.1c20-source.zip (complete source/docs/tests; no active script)
- Migrate-WS.cmd / Update-WS.cmd; non-active MSM2-WS-v0.1c20.script.disabled; BUILD_MANIFEST.json; analysis/V01C20_RELEASE.json

Extract the install ZIP **outside Mods Library**, keep its tools directory together with Migrate-WS.cmd, prepare Windows Python/Git/PowerShell/configured WSL2 and shared game-root Overstrike/DLL prerequisites described in README_TEST/source/BUILD, then replace the placeholder with the real game root and run:

```bat
Migrate-WS.cmd --game-root "<actual game root>" --branch codex/v01c20-edi-value-provenance
```

Thereafter double-click installed MSM2-WS/Update-WS.cmd. Confirm one shared UI sees ordinary mods + SF + WS, choose Install mods -> Run modded game, and verify a new versioned log in MSM2-WS/logs. Keep the old environment until all real-PC checks pass; it is never automatically deleted. Normal human A~4s -> fully end -> move/fast-travel -> E~4s can then verify regression/new logging; privately send newest complete runtime log, video only on visible anomaly. Real migration is not claimed complete by cloud fixture success.


## First-run WSL bootstrap follow-up

Follow-up implementation commit: **556e67e9404dc75fa78230ef8e19ba5710ce2e2a**. Latest instruction base: **d50bd29afae075791717e5491b82d98bf0744588** (real-PC acceptance gate), fast-forwarded without rewriting history. PROJECT_STATE and CODEX_NEXT are preserved from that instruction base. The original cloud regression and standalone69 counts above are the b191708 baseline; the additional checks below specifically exercise this bootstrap repair.

User-confirmed local prerequisites: Windows Git2.55/Python3.13; Ubuntu24.04 WSL2 x86_64, Python3.12/glibc2.39; basic/support packages; pinned GCC14/Clang19 setup and Capstone5.0.6 analysis environment completed. Game-root winmm.dll was restored by the user and is absent from shared Overstrike. This is prerequisite evidence, not completed migration/UI/runtime acceptance.

A first-use defect was found before asking the user to migrate: smoke_windows.sh attempted to create the NLS symlink inside its own nonexistent destination (usr/share/wine/wine/wine). Cached cloud toolchains already had usr/share/wine/wine -> ., masking that branch. The corrected command creates the link at usr/share/wine/wine. source/BUILD.md documents the first-run requirement.

Validation: bash syntax PASS; the exact checked-in Wine setup prelude with the NLS link absent creates the correct link; a fresh owned isolated Wine prefix runs real DLL load/SEH/repeated enable-disable/cleanup and diagnostic formatter (2 PASS), followed by the full existing DLL/logger smoke (7 PASS). The original cloud link was preserved/restored and only the owned fresh-prefix server was stopped. Wine emitted missing auxiliary winedevice/rundll32 messages, recorded in the test log; the checked programs returned0. No Wine GUI, actual game or real-PC success is inferred. Test logs are source/logs/workspace_fresh_wine.log and workspace_bootstrap_smoke.log.

Runtime DLL/version remain v0.1c20/package0.1.38; this change repairs test-environment setup only. EDI remains production-disabled and no native safety/admission/timing gate changes. Current ZIP source/docs are refreshed; prior pinned b191708 download links still identify the original snapshots. Migration uses the current GitHub branch for its independent _source clone and full validation. Real Windows migration/shared Overstrike/UI/new runtime log acceptance remains PENDING.

Next local step: leave Ubuntu with exit, fast-forward the Windows preparation checkout, then run its Migrate-WS.cmd with the actual game root and codex/v01c20-edi-value-provenance. The helper runs the preserved full build/test/package pipeline before deployment and never deletes the old environment or changes SF/ordinary mods.


## Real-PC first migration failure / focused reset diagnostics

Diagnostic implementation commit: **569b0e0e37b00022a7e1c38047728f1d3c533682**. The user's first real-PC attempt at9815793 completed the Windows/independent-clone/branch/WSL preflights and Windows x64 build, then portable13 and DLL/logger7. The supplied terminal excerpt contains47 PASS records and3 optional private-input SKIP records before failing in immediate_reset_smoke.cpp at assert(windows[0].hits==1). The full native suite/package/deploy/Overstrike-launch stages did not complete. Migration is NOT accepted; this is not a gameplay failure or actual-game test. No full terminal dump or private game body is committed.

The root cause is UNCONFIRMED: the old output lacks hit/drop/capture/timing state. Wine auxiliary missing-process messages alone do not identify it. The failing legacy fixture has a real30ms commit-relative capture deadline; host delay is one hypothesis, not an established fact.

The owned fixture now records QPC just before its store and, only before the original first-hit fatal assertion fails, prints RESET_SMOKE_FAILURE with hits/drops/capture/primary-installed, arm/store timing from commit, unchanged deadline and stop reason plus its synthetic diagnostic output. No production source, deadline, assertion, budget, retry, skip or admission gate was changed. source/BUILD.md documents focused diagnosis. Runtime DLL/artifact/version and the latest CODEX_NEXT/PROJECT_STATE remain unchanged.

Validation on cloud: focused reset suite9 PASS /1 optional private SKIP. A separate owned negative variant introduces40ms delay and is required to fail the original hits==1 assertion: observed store41.3498ms, deadline30.0000ms, hits0, stop_reason=deadline. That confirms the added failure output and preserved deadline, not the cause on the user's PC. Logs: source/logs/workspace_reset_diagnostic_tests.log and workspace_reset_diagnostic_negative.log. Original131/5SKIP and standalone69 above remain baseline records, not a successful real-PC migration.

Next human action: dismiss the migration failure pause, fast-forward the Windows preparation checkout, and invoke source/tools/windows/reset-diagnostic.sh through WSL so source is staged to Linux before build/reset tests. This collects the precise failure without deploying to the game. Share RESET_SMOKE_FAILURE and following assertion, or the final focused PASS output. Full migration/build/tests/package and real shared Overstrike/UI/new runtime log acceptance are still required afterward. Keep EDI paused and all Position/source/input/movement writes0.


## Corrected focused diagnostic filesystem / runner

Staging helper implementation commit: **bc050b27d9cf1cdd01fda6208b4e735a2a549c25**. The user's focused diagnostic attempt at8bf261c failed before running the reset fixture: Windows-mounted /mnt/c source compilation reported missing VOID/LPVOID/BOOL in MinHook. The prior direct-build guidance was incorrect. tools/compat/Windows.h contains an include of lowercase windows.h; a case-insensitive source filesystem can resolve that to the same shim, hiding the real MinGW header. An owned header-alias fixture reproduces the missing-type error. No vendor/compat/production source is changed.

New source/tools/windows/reset-diagnostic.sh derives its preparation source folder, copies it without build/log/output caches into a uniquely owned Linux /tmp/msm2-ws-reset-diagnostic.* tree, creates the required log directory, checks header-case separation, then runs the unchanged build_windows.sh and test_reset_trace.sh. It retains that diagnostic tree/logs for inspection and never deploys to the game. Source/BUILD.md now requires a case-sensitive Linux build tree and routes focused diagnosis through this helper. The normal migration pipeline already stages to Linux and is unchanged.

Validation: shell syntax/default source resolution PASS; actual default helper in a fresh Linux staging tree completed Windows DLL build and9 focused PASS /1 optional private-input SKIP (source/logs/workspace_case_sensitive_reset.log). Owned Windows.h/windows.h alias reproduces the compiler error. The original capture assertion, real30ms deadline, existing suites, production DLL/manifest and latest CODEX_NEXT/PROJECT_STATE remain unchanged. The first real-PC capture failure is still UNCONFIRMED because this second attempt did not reach the fixture. No claim of successful migration, shared Overstrike/UI or new runtime log acceptance is made.

Next user step: fast-forward the Windows preparation checkout, then invoke its source/tools/windows/reset-diagnostic.sh using wsl -d Ubuntu-24.04 --exec bash. Send RESET_SMOKE_FAILURE plus the following assertion, or final focused PASS output. Full migration acceptance remains pending afterward. No old environment/SF/ordinary mods are deleted or changed by focused diagnosis.


## User-confirmed focused and full migration pipeline PASS

The user supplied terminal output from the corrected Linux staging helper: the Windows DLL build and focused reset suite completed9 PASS /1 optional private-input SKIP. The original first-capture assertion did not recur. Its original cause remains unconfirmed; no real30ms deadline, assertion or gate was relaxed.

The subsequent real Windows Migrate-WS.cmd attempt completed the full preserved WSL pipeline and returned to PowerShell. The test portion contains exactly131 PASS /5 optional private-input SKIP records; package validation and final staging each add a separate PASS line. The18 optional maintainer-log SKIP messages are absent cloud report files, not omitted build/test stages. Final actual-helper messages after the fixture tests are: required full WSL build/test/package completed; WS update complete: MSM2-WS-v0.1c20.script; Local migration checks passed. Fixture-only v0.1c21 messages are not the deployed user version.

This supports real helper preflight/independent-checkout/update/build/tests/package, versioned deployment and its one-active-script postcondition. Shared Overstrike launch was requested successfully, but no GUI observation has yet been supplied. The user has not yet confirmed ordinary mods + SF + WS together, Install mods / Run modded game, or the actual new versioned runtime log. Overall acceptance remains PENDING, EDI stays paused, and the old environment must remain until the remaining checks pass.

User-local generated ZIP reports: install3332536bytes/SHA256236e9088014b244a0c7e044ef5dca0a135d922543de4ccc55b300246a9775374; source764618bytes/SHA25617a0263dd6d604cf15cec5b95a2b9a96e272b4be27cd65e3b709cb34de24b974. These describe the local build. Public ZIP snapshots remain pinned at2735e500e7bee17ce6d3016736439f2bd5edad0c; this follow-up only records acceptance evidence and does not republish packages or change runtime/source.

Next human step: confirm that the shared Overstrike window opened and that ordinary mods, SocialFeed and WallSprint appear together in the same MOD list. Then proceed to installation/game launch and new runtime-log checks one step at a time. No gameplay research is authorized by this partial infrastructure result.


## Archive-aware duplicate correction — implementation a2faf166851270a78bd5ca30c67cd55bcc59cc5c

The user supplied shared-Overstrike screenshots showing24 checked WallSprint entries and ordinary mods; SF is not identifiable in that list. This is a failed UI acceptance gate despite the old131-test pipeline/deployment PASS. Upstream extension dispatch recursively discovers `.script` members in ZIP/RAR/7z without extraction. The repository's44 tracked ZIPs contain22 install scripts; source22 + build-cache1 + root active1 predicts the observed24. Individual entry paths were not captured. analysis/OVERSTRIKE_ARCHIVE_DISCOVERY.md records the evidence and independently implemented model.

Changes: workspace.py adds archive-aware guards, managed non-cone Git sparse checkout (all code retained, archive working-tree paths excluded), clean/tracked-source upgrade checks and positive legacy build-ZIP validation/quarantine. It refuses untracked/foreign/corrupt/misplaced archives and custom sparse configuration. Git archive blobs/commit history remain intact. Old WS build outputs move only from _archive/_build to collision-safe disabled names under _archive with exact bytes/mtime. New build ZIPs return only as .zip.disabled. Strict checks run after upgrade, after build and before launch. release.py refuses direct ZIP output within Mods Library; workspace-build.sh also rejects an unsafe old-bootstrap source tree and output archives. Windows root entry names, independent-clone/upstream/ff-only/no-links checks, old active-script rollback and sibling isolation remain.

Existing8 workspace groups retained plus5 archive groups =13. They cover recursive nested ZIP discovery without extraction, disabled-container exclusion, tracked archive/history/code preservation, root/nested/mixed-case sparsity and clean ff updates, old full-checkout/build-cache upgrade with modeled3 entries ->1, bytes/mtime/collision preservation, unknown archive refusal, custom sparse preservation, direct-release/old-bootstrap guards and fresh build output refusing a raw ZIP. A separate actual independent repository clone reduced44 visible ZIP files to0 while retaining all44 tracked blobs, code bytes, HEAD and clean state in an owned Unicode/space path. Public ZIP filenames/downloads and historical artifacts remain available outside Mods Library; no force/history rewrite or main changes.

Validation: actual full external staging pipeline completed build/portable/DLL/native/prepared/workspace/package with136 public PASS /5 optional private-input SKIP (portable13, DLL/logger7, hooks116 including13workspace). Both returned archives are .zip.disabled with valid CRC; manifest/script/DLL hashes agree and no visible archive/script remains in returned build artifacts. Latest13 workspace groups also passed with the old-bootstrap guard. An initial cloud attempt hung in an unchanged owned Up-watch fixture; only stale/owned synthetic test processes were stopped and the prior cloud Wine prefix was preserved under /workspace/toolchains/ws-archive-wine-isolation-sprs4yir/previous-prefix. The full suite then completed with a fresh cloud-only prefix. The original hang cause is unconfirmed; no test, deadline, assertion or native gate was relaxed. User-PC WSL state was not changed by cloud tools.

Every native source/header and version file is unchanged from d19fa36. The exact tested DLL from the successful full pipeline is retained for release. EDI remains disabled; all ownership/write safety gates and c19 evidence remain. No SF/ordinary-mod/old-D/shared-cache/profile cleanup is performed. The earlier real-PC physical one-script postcondition is insufficient and does not count as whole-workspace acceptance.

Current standalone source ZIP verification: PASS74 records (54 focused native +7 DLL/logger +13 workspace), no private inputs or previous builds/logs at extraction. Verification exposed a test-fixture manifest referring to upstream ZIP bytes rather than the actual active fixture ZIP (creation timestamps can differ). The fixture now hashes the actual payload; a valid-CRC archive with a mismatched manifest is explicitly refused without moving either archive. The61 native checks were retained only after proving all runtime sources/headers/build/native-test scripts byte-identical across this fixture-only correction, then the corrected13 workspace groups ran successfully. Original standalone69 above is historical baseline only. Current artifact validation covers CRC/current complete sources/docs/helpers/licenses/script and DLL hashes/private-body exclusion; details are in V01C20_RELEASE.json.

Next user action: close the current Overstrike without installing, update the Windows preparation checkout, rerun that updated Migrate-WS.cmd for the same game root/branch, then confirm exactly one WallSprint in shared Overstrike. Use the updated preparation helper to upgrade the old installed engine. SF/ordinary visibility, Install mods / Run modded game and a fresh actual versioned runtime log remain required. Keep the old environment and keep gameplay research paused.

Fixture consistency and validation-log refinement commit: **31065f8228f415ef0f464aef6b88b727adafa63a**. No production archive ownership/hash check was relaxed.


## Real-PC archive-upgrade rerun — reset arm rejected

Diagnostic implementation commit: **6fcf33e45e49523a456a75016e267f7717713e79** (branch codex/v01c20-edi-value-provenance). The supplied user output at delivery890d23c reports46 public PASS and3 optional private-input SKIP, then immediate_reset_smoke.cpp:156 asserting beginReset(r,snapshot). The earlier failure was hits==1 after arming; this is a different rejection point. The terminal does not identify which synthetic session failed or whether rejection happened before window allocation or during thread enumeration/debug-register installation. Scheduling/deadline and installation failure are possibilities, not established causes. winedevice messages also appeared in successful preceding tests; they do not establish this failure's cause. No raw user terminal dump is committed.

Only the test fixture's failure output changes: every rejected arm now prints RESET_SMOKE_ARM_FAILURE with session, elapsed commit time, admission/identity/alignment/debugger state, before/after window counts, active leases and an explicitly unattributed LastError. It prints existing production skip/arm records and, only if a new window was allocated, that window's timing/hit/stop diagnostic. The original fatal arm assertion remains. No absent-window indexing, retry-to-pass, clock substitution, deadline extension or production admission change is introduced. QueryPerformanceCounter failure is explicitly asserted. Runtime sources/headers/config/version and the validated distributed DLL/script are unchanged from890d23c.

The case-sensitive reset-diagnostic.sh helper captures both stdout and stderr, prints its retained folder and exit status even on failure, runs the existing focused suite once and repeats the unchanged short-window fixture five times without rebuilding. It stops at the first failure through pipefail. It neither runs the full migration nor deploys to the game; SF/ordinary mods/old D/shared cache and user Wine state are not changed. README_TEST (both copies) and source/BUILD.md describe this path. release.py includes only the three named independent diagnostic validation logs.

Validation: focused9 PASS /1 optional private SKIP; the actual helper in a fresh Linux stage rebuilt the DLL and completed19 PASS /1 optional SKIP (focused9 plus five repetitions with2 PASS each). Two separate owned negative variants deterministically backdate the commit by50ms or supply no candidate thread: both produce the new diagnostic, preserve the fatal assertion and return nonzero. The pre-allocation case reports windows0->0 and the failed-installation case0->1. No production/game source was changed. Baseline full136/5 and source74 results above belong to890d23c; they are not rerun or relabeled as successful real-PC acceptance for this diagnostic-only change. Public ZIPs are refreshed with current code/docs and the same validated runtime payload.

Next action: fast-forward the Windows preparation checkout and invoke its source/tools/windows/reset-diagnostic.sh through WSL Ubuntu-24.04. Send RESET_SMOKE_ARM_FAILURE / RESET_SMOKE_FAILURE and subsequent records, or the final exit line if all five repetitions pass. Do not repeat the full migration or install mods yet. Actual cause and the full real-PC acceptance gate remain unresolved; c20 EDI/gameplay research stays paused.


## Confirmed real-PC setup latency / test-only snapshot correction

Implementation commit: **79d2acff84574b7c794abe980560dad1e3e9052a**, branch codex/v01c20-edi-value-provenance. The user's focused result on6547ec4 now identifies session1: all printed admission/identity/alignment gates pass, no debugger, window start44.692600ms after commit and observed return44.706700ms against the real30ms deadline. installed/skipped_busy/failed/hits/drops are all0; no lease remains. Production enumerates the thread list before assigning window.start, then correctly skips installation after expiration. Thus the failure occurs in pre-installation setup, not a failed debug-register write. Individual snapshot/enumeration API costs are not separately measured. LastError18 is unattributed; the cleanup stop label does not establish identity loss. analysis/RESET_SMOKE_SNAPSHOT_TIMING.md records measured facts and limits without publishing the raw terminal dump.

immediate_reset_smoke.cpp now prepares a real OS thread snapshot before the synthetic commit QPC for the direct legacy and async-helper positive tests. A test-translation-unit-only atomic adapter transfers that real handle once to unchanged production enumeration/filtering/close logic. QPC, thread entries, filtering, suspension/context APIs, actual owned-memory store/VEH/MinHook and the30ms capture/deadline assertions remain real. No API adapter enters the runtime DLL. Prearmed and process-expansion tests use actual snapshot acquisition through fallback. No production function or game guard changes.

The required reset suite adds --slow-setup (50ms preparation before commit, original positive hardware assertions must succeed) and --slow-snapshot (50ms delivery delay after commit, arm must fail with no installed watch/hit/lease, unchanged memory and debug registers). This tests both the repaired fixture boundary and preserved fail-closed behavior. Existing failure diagnostics remain fatal. There is no retry-to-pass, clock substitution, new gameplay write or deadline extension.

Validation: focused12 PASS /1 optional private SKIP. The complete fresh-stage diagnostic helper also rebuilt the DLL and passed22 records /1 optional SKIP, including all five repeated short capture executions. The actual full external workspace-build pipeline rebuilds DLL, runs portable13 + DLL/logger7 + hooks119 =139 public PASS /5 optional private-input SKIP, creates validated packages and returns only .script.disabled / .zip.disabled artifacts with correct CRC/hashes. Latest full output is source/logs/workspace_reset_snapshot_full.log; the exact DLL from that successful pipeline is used in the final release. The previously recorded source74 validation is historical (890d23c), not a newly repeated standalone suite; current ZIP integrity and complete current source/docs/helper contents are checked separately. Native source/header/version bytes are unchanged from6547ec4 except the owned smoke-test file; CODEX_NEXT/PROJECT_STATE and all gameplay/ownership/write gates are preserved. Documentation, packaging's required analysis list and selected independent log list are updated. SF/ordinary mods/old D/shared Overstrike caches and profiles remain untouched.

Next human action: update the Windows preparation checkout and run its reset-diagnostic.sh through configured Ubuntu-24.04 WSL, without deploying. If it passes, proceed next to full migration and then shared UI/log acceptance one step at a time; if it fails, retain the diagnostic and subsequent production records. Real-PC acceptance is not claimed from cloud tests.


## Real-PC corrected focused reset run — PASS, full migration next

The user supplied Windows PowerShell/Ubuntu-24.04 WSL output after fast-forwarding to delivery **f2be95b40ea22c8568d7c39489ba0e75461f426e** (snapshot correction implementation **79d2acff84574b7c794abe980560dad1e3e9052a**, branch codex/v01c20-edi-value-provenance). The diagnostic helper rebuilt successfully and completed **22 public PASS records /1 optional private-input SKIP**, all five repeated legacy captures, and **Reset diagnostic exit:0**. The added50ms-before-commit positive and50ms-after-commit arm-refusal cases both passed, together with the coordinator/prearm/multithread cases. No arm/capture assertion failed. This confirms the corrected focused fixture on the actual user machine. Auxiliary winedevice messages remain in that successful run; they do not invalidate the completed checks.

This diagnostic did not deploy to the game. The complete corrected migration, archive duplicate resolution in shared Overstrike, SF visibility, installation/game launch and a new actual runtime log are still unconfirmed. Do not mark the overall workspace gate complete or resume EDI/gameplay research. No raw user terminal output is committed.

This acceptance update changes only docs/CODEX_RESULT.md and analysis/V01C20_RELEASE.json. Source, helpers, tests, runtime payload, version and public release ZIPs remain at f2be95b; the ZIPs retain the documentation captured at that release. Validation for this record checks the supplied22/1 counts, all5 repetition labels, exit0, absence of fatal diagnostics, JSON parsing and existing public ZIP sizes/hashes. No executable changes require a test rerun.

Next human action: close shared Overstrike, update the Windows preparation checkout, then run its Migrate-WS.cmd for the same actual game root and development branch with Ubuntu-24.04 selected. After successful full pipeline/deployment, inspect the reopened shared Overstrike for exactly one WallSprint and ordinary mods; confirm SF visibility separately. Do not Install mods until that UI check is reviewed. Preserve the old environment.


## Real-PC corrected migration and duplicate resolution — PASS, SF visibility next

The user supplied the complete real Windows/Ubuntu-24.04 migration output after updating the preparation checkout to **f1917b1d52175173ccae8c5ef04b2bcd896efa51**. The preserved pipeline completed **139 public test PASS /5 optional private-input SKIP**, package generation/validation, disabled artifact staging, and final MSM2-WS-v0.1c20.script deployment/Overstrike launch. The terminal includes2 additional packaging/staging PASS records (141 total PASS lines), and27 optional maintainer-log absences separate from the5 private-input SKIPs. These missing cloud validation-log copies do not skip local build/test execution. No fatal helper/test error appears.

Two supplied screenshots jointly cover the shared MSM2 (Steam) list1..25 at the known game root. Ordinary mods are visible; WallSprint appears once at24 and is checked; Suits Menu follows at25. The previous24 WallSprint entries are reduced to1 on the real PC, confirming UI duplicate resolution after the archive-aware correction. SocialFeed is not identifiable anywhere in this supplied list; do not infer whether its folder or active script exists without filesystem evidence. Install mods / Run modded game and a fresh WS runtime log are still unconfirmed. The overall ordinary+SF+WS/log acceptance gate remains open; gameplay/EDI work stays paused.

This is a documentation/derived-evidence update only to docs/CODEX_RESULT.md and analysis/V01C20_RELEASE.json. No raw terminal dump/screenshots, source changes or new release binaries are committed. Public release assets remain at f2be95b (their existing sizes/hashes verified), and CODEX_NEXT/PROJECT_STATE are preserved. The local generated install ZIP reported3362929 bytes/SHA2562310334098ebeb9996f054488ac563b244fa5e9331777fd616ae0596ecb38d20; its source ZIP reported782588 bytes/SHA256a35e6852bf2c6db3400e616c65a932fbeb2a3dc8a1a4f58a5d38efc88a58f8d3. These are user-local outputs, not replacements for the public download hashes.

Next action: read-only list Mods-Developing project folders and recursively list script/script.disabled paths to identify the SF placement. WallSprint tooling must not repair/clean/mutate SF, ordinary mods or shared Overstrike profiles. Do not Install mods until the SF visibility check is resolved or the user explicitly chooses a WS-only test. Preserve the old environment.


## Shared SF+WS recognition accepted after successful SF updater

The user completed the SF repository's updater at **3ca46be4260048663ae8afc741678ffd435a8afd**: native54+40, actual CMD15, workflow87 and Core11 passed, its staging package verified and MSM2-SF-v0.1.3.script Prepared. This was SF-owned tooling, not cleanup/mutation by WS tooling. Two screenshots cover the shared MSM2 (Steam) list1..26: ordinary mods, exactly one checked SocialFeedProbe at24 and one checked WallSprint at25, followed by Suits Menu. SF visibility is now confirmed together with the previously accepted139-test WS migration and duplicate resolution.

The shared-list recognition gate passes. Install mods / Run modded game, a fresh versioned WS log and the SF log remain pending. No real game/EDI/player-signal inference or gameplay research reauthorization follows. Preserve old sources. Next: click Install mods once with the shown SF/WS selection and desired ordinary mods; review completion/error before continuing launch/log acceptance.

Only this derived acceptance record and V01C20_RELEASE metadata change. No raw user output/screenshots, native source/helper/runtime/version/ZIP changes. Existing public ZIP hashes remain at f2be95b; CODEX_NEXT and PROJECT_STATE are preserved. No test rerun is needed for documentation-only evidence updates.


## Actual shared game launch / new versioned logs — workspace gate PASS

The user reports successful game startup and visible WS operation and supplies both 20261024-180229 logs. analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md records derived evidence only. WS accepts the expected2.810.0.0 executable hash, installs the existing verified hooks and writes the canonical project log with no shared fallback. Two1594/2234ms WallRun sessions retain orientation (dot1), finish by verified Deinit with source active to exit, stale0/hard-kill0, and both watches report restored cleanup/no pending cleanup and Position writes0. Session2 finalizes8 entries/maps; this does not prove source ownership or replace the prior4s A/E acceptance. One diagnostic-log drop is explicitly writes_unaffected; no whole-process shutdown is claimed from truncated tails. EDI tracing remains disabled.

SF logs prove script_enable/module pin/worker/diagnosis and canonical new SF logging.20 summaries (18 ready) remain valid0/range_pass0 with Transform closed, so loader/logging acceptance does not establish a working player handle or a visual feed. The current probe has no visual UI. The user-confirmed launch plus both real logs completes shared installation/launch/logging acceptance after the previously passed helpers/UI. The infrastructure gate passes; no new gameplay/EDI research is authorized until CODEX_NEXT is explicitly changed. No further setup rerun is needed from this evidence.

Only analysis/docs/structured metadata change; raw logs/game bodies/process data are not committed. Source/helpers/runtime/version/public assets and CODEX_NEXT/PROJECT_STATE are preserved. Derived-summary parsing validates session counts, retention/Deinit/cleanup/Position gates and SF zero-candidate status; existing ZIP hashes remain unchanged. No binary regression rerun is required for this log review. Old sources are not deleted.
