# One flat-wall lateral route — bounded original Update investigation

2026-10-31 JST. Baseline d7cb968297478aa751633abda75d56d6ff359a95,
branch codex/v01c20-edi-value-provenance. Research/docs only. Runtime remains
v0.1c38r4/package0.1.60; no new experiment admitted or requested here.

## VERIFIED-RUNTIME / supplied owner evidence

V01C38R4_OWNER_RUNTIME_20261031.md accepts r4's normal5s-expiry removal in THIS
owner test: session3 eleven completed reversals,27.031s source override through
verified native Deinit,1819 A/1822 B commits, retention pass, no hard kill. One40ms
stale episode recovered; no claim of uninterrupted freshness or unlimited lifetime.
Video/log/input clocks are not synchronized here. Earlier facing START is deferred
by explicit owner decision, not fixed. Independent4096-per-path budgets remain.

## ONE selected route, not a guessed callable API

Original authenticated WallRun_Update (RVA0x1424530) -> shared direct branch
continuation RVA0x14248f9 -> original motion/steering consumer (not yet identified).
These are statically reachable edges; actual branch outcomes were not measured.
This is the best AVAILABLE original-route continuation to inspect before claiming
an input/motion API. No pure planar/controller inventory repeated; no low-level
Position producer/c73 reopened. RequestSimpleMotion/UpdateMotion reference names
remain possible architecture hints, not names assigned to any target below.

### Existing evidence actually available

The already attached c20 log MSM2-WS-v0.1c20-20261024-180229.log includes exact-build
ACCEPTED and on-disk EXE SHA c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
for2.810.0.0. Its Update-only original code capture has:

- begin RVA0x1424530, end RVA0x14246d6,422 bytes;
- SHA256 e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f;
- source=admitted_native_function_before_MinHook, runtime-function boundary.

This matches source/include/static_observation_plan.hpp's Update pin (422/89).
These are authenticated PRIOR captures for the same intended build, not new live
r4 byte readback. No raw log/body is committed. Only Update metadata and14 code
chunks were selected into /tmp, not other functions/object dumps.

Existing review_surface_source.py verifies full chunk coverage/no overlap/SHA.
Pinned Capstone5.0.6/static_access_v2.analyze decodes all422 bytes into89 instructions.
HOWEVER:100% linear decode is NOT complete logical-function or CFG coverage.
Conditional branches at0x142468f,0x142469b,0x14246a4,0x14246b2 target0x14248f9,
OUTSIDE the captured range. Last instruction at0x14246d4 ends exactly at0x14246d6
and is not RET/control termination; fallthrough bytes also absent. No RET occurs
in this capture. Existing analyzer reports outside_span_branches=[0x14248f9];
it deliberately omits a successor exactly at span end, so the fallthrough gap
was separately checked. A single RUNTIME_FUNCTION range is not proof of an entire
source-level function. Split/chained layout is plausible, not established without
associated unwind data. Do not “repair” this by concatenating guessed bytes.

### What the available prefix establishes, and does not

Four direct CALL edges are present, at0x1424578->0x113ab70,
0x14245c3->0x367dd10,0x14245f6->0xf51d80,0x1424630->0x29bd5f0.
Their bodies/types are not in this selected capture. No one is authenticated as
input, RequestSimpleMotion, contact query or submission. Register/field use alone
does not create an ABI. Branches/field comparisons are not interpreted as named
input/contact flags. The prefix saves incoming RCX as an owner alias and reads
owner-relative storage; one nested object value feeds a helper. This is static
operand provenance, not authorization to chase those pointers or read new fields.

| Route contract | Available evidence / exact missing part |
| --- | --- |
| Caller/build identity | Existing unique AOB/prefix/runtime boundary in sdk_bridge.cpp/wallrun_hooks.cpp and original422byte SHA pin. Concrete Update branch edges above; reference DLL offsets not used. |
| Input type/coordinates | UNKNOWN. No typed requested-direction or consumed wall-tangent motion record in these422 bytes. Earlier INPUT inventory remains accepted; this investigation follows the original consumer route instead. |
| State/owner/lifetime | Existing verified Init/Entry/Update/Deinit/session owner/generation/thread gates; incoming Update owner alias visible. No helper/object field lifetime/type or motion-request ownership inferred. |
| Contact handling | Existing normal/plane evidence supports WS's planar orientation gates, not a contact/collision contract for a motion API. Prefix checks cannot be named contact logic without consumer proof. |
| Ordinary exit/restoration | r4 verified native Deinit closes source publication/stores; normal native WallRun can continue when overrides close. No lateral motion is injected, so there is no new restoration claim. Side effects/undo semantics of a future request API remain UNKNOWN. |
| Tangent motion submission | UNKNOWN: no verified request vector/coordinate frame/consumer or safe invocation in captured entry fragment. Shared continuation missing before route can be linked. |

## Interaction with existing A/B orientation override

source/src/NativeHooks/native_source_override.cpp and GameInterop/stack_source.cpp
replace caller-local A orientation48 bytes and B compact36 bytes with one coherent
published basis; Position/W and B metadata/scales are preserved. This does not
submit tangent displacement. source/include/Math/source_orientation.hpp sets the
entry wall frame; reversal_heading.hpp follows displacement later. A proposed
motion consumer might use current Actor/source axes; WS overrides could therefore
change its frame. Independence is UNKNOWN, not assumed. Later admission must
prove which frame the requested vector uses relative to the existing override,
without guessed Position/velocity/gravity writes or unrelated whole-engine proof.

## ONE exact missing acquisition / next bounded action

Need ONE original code window starting RVA0x14248f9, maximum128 bytes, with exact
build/file SHA, block SHA, executable/file-backed section and available covering
runtime-function/unwind metadata. Stop at the first relevant control boundary or
128byte cap; no recursive callee/whole-image scan. It is selected because four
observed Update branches converge there, not because a reference name guessed it.
The separate end-of-span fallthrough remains unresolved; this window does not
promise complete-function coverage or automatic movement admission.

Smallest existing read-only method: use the SAME authenticated on-disk EXE locally,
not a running-game patch. source/tools/slot_publication.py::parse_pe(data) already
validates bounded PE/runtime metadata and exposes file_range(rva,n) for checked
file-backed reads. After independently requiring the pinned EXE SHA and executable
section, read only this window (bounded by the next known metadata/section boundary
if earlier). Decode with existing Capstone/static_access_v2; retain data privately.
Use only this metadata/range API, NOT slot_publication's analyze_image/CLI sweep
or c73. No authenticated game EXE is available among the selected evidence, so that read
was NOT executed.
No full EXE upload or raw full-log re-analysis is requested.

Important acquisition limit: current surface_source.hpp::analyzeNative is called
only for four existing lifecycle starts, requires lookup begin==address and emits
that ONE entry's span before hooking. A normal existing startup recapture would
return the same422bytes, not automatically0x14248f9. review_surface_source.py can
validate supplied captures but cannot acquire missing live bytes. No existing
turnkey “capture this successor” CLI was found; no invented command/hook/helper
is proposed or added. Chat can arrange the single local offline extraction from
the existing PE/range primitives; owner instructions are outside this handoff.

No motion-write experiment admitted yet. Next deliverable is only this bounded
continuation window, not arbitrary input discovery, camera, animation, adhesion,
corners or the earlier START backlog.

## Evidence and validation references

AGENTS/manuals/current state; V01C38R4_OWNER_RUNTIME_20261031.md;
WALLRUN_MOVEMENT_FACING_SOURCE_REVIEW_20261030.md is historical pre-r1 behavior,
NOT current proof of “no reversal branch”. INPUT_START_ADAPTER_INVENTORY.md and
AFTER_TURN_PLAN.md are scope boundaries. Reference architecture:
REFERENCE_MOD_WALLRUN_REVIEW_20261029.md (labels only); Wisdoms
WALLSPRINT_REFERENCE_MODS.md / ENGINE_INTERNALS.md lifecycle/motion sections read
only. New reusable lesson (span is not whole logical CFG) preserved locally;
Wisdoms synchronization deferred because this task authorizes local docs only.

Exact commands/outputs in CODEX_RESULT. No helper/source/config/package/version
change, no new watch/API call/pointer traversal. All stale/ownership/build/C52 and
4096 write/attempt gates unchanged. Raw body/disassembly/log remain private /tmp.
