# V01C29 actual runtime result — exact image qword source is writable

Source evidence: owner-supplied private log `MSM2-WS-v0.1c29-20261025-130401.log`.

## Exact build

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
target_build_gate=ACCEPTED
```

This is the same admitted executable identity used by the accepted c25-c28 chain.

## Accepted predecessor

C28 established the exact fallthrough instruction and address:

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
width=8
memory_read=1
memory_write=0
memory_RMW=0
pure_load_form=1
flags_preserved=1
full64 RAX write
source_rva=0xC2D2580
address_component_tags=0
alias=image-only for the analyzed state
```

The remaining c29 question was whether this exact image-backed qword could satisfy the narrow non-writable/current-stable rule and therefore receive loaded tags0.

## Actual c29 source metadata

The exact source range is authenticated to one PE section:

```text
source_rva=0xC2D2580
width=8
metadata_valid=1
section_unique=1
section_overlaps=1
section=.data
section_begin_rva=0x9B24000
section_end_rva=0xC565A28
section_characteristics=0xC0000040
readable=1
writable=1
executable=0
loader_ranges_checked=1
loader_overlap=0
policy_passed=0
reason=image_load_section_not_readonly
```

The current mapped page is also writable:

```text
allocation_base_matches_image=1
range_in_image=1
region_begin_rva=0xC2D2000
region_bytes=2703360
state=0x1000        # committed
 type=0x1000000     # image
protection=0x4      # PAGE_READWRITE
```

Therefore the c29 immutable/current-stable admission policy fails structurally before any source-value read.

## No value observation occurred

The fail-closed rule behaved as designed:

```text
MOVEMENT_CALLEE_READ_IMAGE_VALUE
read_attempts=0
max_reads=2
first_read=0
final_read=0
PE_metadata_equal=0
page_metadata_equal=0
bytes_equal=0
target_code_rechecked=0
admitted=0
loaded_tags_known=0
raw_value_logged=0
pointer_chase=0
reason=image_load_section_not_readonly
```

Do not interpret zero-valued diagnostic fields above as equality/failure of an attempted value read; the read stage was never entered.

## Target / parent status

The local target remains incomplete at the same exact MOV:

```text
TARGET_IDENTITY admitted=0
barrier_rva=0x2CF7FE9
barrier_instruction=mov
reason=image_load_section_not_readonly

LEAF_CFG complete=0
return_paths=0
unique_instructions=3
visits=3
final_code_rechecked=0
effect_certificate=0
```

The parent CALL also remains unadmitted and fail-closed at that MOV. C27 JA geometry/fan-out and c26 CMP remain accepted; no branch outcome is numerically selected.

## Interpretation

C29 resolves the exact question posed by the prepared image-load proof:

- the source is definitely image-backed;
- but it resides in writable `.data`;
- the live image mapping is currently `PAGE_READWRITE`;
- therefore it cannot be assigned loaded tags0 under the non-writable image rule;
- the implementation correctly refused before observing or logging the qword value.

This is a refusal of the **tag0/immutable-source proof**, not evidence that the qword actually carries selected/postwrite/other-prewrite dependence.

A sound successor may still model this one exact full64 load conservatively by replacing RAX with the existing worst-case loaded tag upper bound `0x7`, provided it makes no claim that all tags are actually present, does not inspect the numerical qword, and does not generalize the rule to arbitrary memory loads.

## Runtime safety

Observed WallRun sessions retained the existing closed safety state:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
Position_writes=0
verified_deinit=1
retention_pass=1
first_hard_kill=none
hard_kill_calls=0
```

No movement/input/Position/source-position permission is granted.

## Next bounded question

Can the exact pinned load at `0x2CF7FE9` be represented as a conservative unknown-memory read effect:

```text
RAX tags <- 0x7   # may-depend upper bound only
RAX origin <- unknown exact-source load
all other GPR/XMM/flags/memory/SP unchanged
```

and then allow the existing c27 bounded two-path proof to continue to the next exact unsupported effect without claiming tag0, immutability, value identity, or generic memory semantics?
