# c34 — pinned SHL count/data/flag classification and conditional constant GPR rule

## Evidence and scope

Task base c32c1fc, branch codex/v01c20-edi-value-provenance. VERIFIED-RUNTIME
V01C33_RUNTIME_RESULT.md: MOVSXD0x2CF7FF4 is exactly reg64 R8 <- reg32 R8D,
modeled without values, tags0. Next SHL0x2CF7FF7/path0, edge0x2CF7FE7 ->
0x2CF7FE9; 6instructions/6visits/0returns/identity/effects. Incoming RAX7,
R8=0, YMM0 low/high7, above256zero, flags0 are retained. Actual SHL operands,
width/count and memory use UNKNOWN. Owned SHL fixture bytes are not actual bytes.
No private current-code snapshot is available in cloud. The production decoder
classifies its authenticated snapshot directly, without requiring a body dump.

## VERIFIED-STATIC — eligibility

Only reachable SHL0x2CF7FF7, exact group2 /4 D0/D1 (implicit1), C0/C1 (imm8)
or D2/D3 (CL) classification. Emit encoding/length/width/destination/count/access,
parent/address/alias/tags, full64 vs zero-ext32 vs partial, decoded flags/effects,
snapshot/current source-code checks. Only canonical no-legacy-prefix register
32/64 implicit1/imm8 forms with matching READ|WRITE destination, sole full GPR
write plus EFLAGS, destination non-SP, no other/vector/memory effect are modeled.
RSP/ESP, partial8/16, CL, memory RMW, wrong site/encoding/mnemonic/refused forms
cannot borrow the rule. Production enable is explicit, fixtures default disabled.
All other instructions still use the prior rules or stop at the exact boundary.

## Width and count

Architectural effective count = encoded count & (width64 ? 63 : 31).
Implicit1 has no encoded immediate bytes; explicit immediate is checked against
its actual imm8 and decoder offset/width. No live count/value observation.
For8/16 metadata the mask remains31, not width-1; counts at/above operand width
are classified with CF undefined, and partial forms always refuse execution.
CL classification records RCX parent/read tags and dynamic possible flag
classes; it is never admitted, even when parent tags0. Tags0 does not prove a
numerical count. Decoder canonical encoding/layout and complete register effects
are independently checked before any state change.

Effective count0: operand unchanged, arithmetic flags unchanged. A register64
parent and pointer provenance remain intact. Register32 still has architectural
parent zero-extension: preserve conservative source-parent tags, replace parent
origin with opaque unknown/non-pointer (do not keep a possibly64bit pointer).
The operand remains unchanged within32bits; do not confuse this with preserving
the old upper32 parent bits. Tests distinguish those effects.

Nonzero supported counts (1..31 for32 /1..63 for64): result tags = incoming
destination-parent tags, constant count adds0. SHL is read/modify/write, so its
own input dependency must remain; there is no unrelated stale destination to
union. Full destination is replaced once, origin opaque unknown/non-pointer.
Conservative parent granularity may retain upper-only input dependencies for
32bit operands; no unsound bit/value narrowing. Only destination changes.

## Flags are part of the effect

Arithmetic masks use RFLAGS bit positions: CF1 PF4 AF16 ZF64 SF128 OF2048,
all6=0x8D5. Remaining flags are architecturally preserved.

- effective0: defined/undefined masks0, preserved0x8D5, aggregate flags unchanged.
- effective1: CF is last shifted-out bit; OF is result MSB XOR CF; SF/ZF/PF
  derive from result. All conservatively use destination input tags. Defined
  mask0x8C5, AF undefined mask0x10.
- effective>1 within width: CF/result/SF/ZF/PF dependence as above; OF/AF
  undefined mask0x810, defined mask0xC5.
- partial width counts >= width: CF additionally undefined; never modeled.

No numerical flags are computed. Existing flag lattice is one aggregate, not
per-flag values. For EVERY admitted nonzero count inject may-top7 because AF
is undefined (and OF additionally for count>1). Defined-flag tags are reported
separately; aggregate top is a sound upper bound for all possible downstream
consumers and avoids claiming undefined=constant/tag0. This is intentionally
coarser than carrying CF/PF/ZF/SF/OF separately. Disable logical-TEST shortcuts
when replacing flags. Count0 preserves the old flag-state/producer shortcuts.
The prior evaluator refuses flag-consuming SETcc/ADC/LAHF and any additional
branch; returns/path unions retain top7. Existing proven full flag producers
may overwrite it under their original rules. No new generic arithmetic consumer
or partial-flag update is enabled. Thus no future consumer can exploit omitted
undefined flags to remove a selected dependence under this extension.

## Diagnostics, replay, safety

SHL / SHL_FLAGS / SHL_OPERAND / SHL_ADDRESS are compact, no numeric register/
memory/vector values. Immediate count/displacement are instruction metadata.
Memory base/index/scale/signed displacement/segment/address size and conditional
Hero/field/source/home/below-SP/image/unknown aliases are diagnostic only; memory
RMW never executes abstract semantics. Existing actual-parent replay supplies
current tags/aliases under the same cumulative4096 and per-walk64 visit budget,
merges tag/alias unions, and loses exactness on conflicts. No second traversal.
Pre/post flag tags are candidate modeled effects, with explicit semantic-modeled
status; decoded potential writes are not claims of actual memory writes.

C30 qword top7, c32 vector low/high7/above256zero and c33 sign extension are
preserved except a legitimately decoded destination overwrite. Both JA legs,
128bytes/32unique/64visits/1branch/2paths, current code/source/PE/runtime and all
normal RET checks remain. One refused path denies whole target identity. No
next new instruction is opportunistically supported. No raw private log/body,
new hook, guessed ABI/type, vector emulation or gameplay change. All ownership/
lifetime/exclusivity/writability and Position/source/input/movement writes0.
Producer0x351EB00 remains deferred. SF/ordinary mods/shared tools unchanged.

## Owned validation / missing live evidence

11 new groups (identity87/read-call21), full build and required portable/DLL/
hook/contracts/workspace chain, independent extracted source build/focused tests,
package CRC/hash/current source/docs/helper/version validation in CODEX_RESULT
and V01C34_RELEASE.json. Retained c19-c33 and native assertions/timeouts.

Actual SHL form/count and resulting next exact barrier remain UNKNOWN until
one shortest A~3s/end/move/E~3s capture of SHL/FLAGS/OPERAND/ADDRESS plus existing
MOVSXD/VECTOR256/barrier/path/GPR/vector low-high/CFG/identity/safety/cleanup.
No numerical count or raw body is requested. Conditional owned rule PASS is
not actual-game SHL acceptance or whole writer/effect/ownership proof.
