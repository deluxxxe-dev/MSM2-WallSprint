# M19 — face WR start diagnostic and conditional stop (2026-11-13 JST)
Baseline8b46e87; latest CODEX_NEXT, WS_GROUND_SPRINT_RESULT and DIRECTION read in
full. The supplied files describe full owner video/log review; no raw attachment
or game run exists in this cloud turn. Unchanged repository/CLAUDE operational
rules apply. Seamless motion and retained face gravity remain mandatory. Owner
verified sideways native sprint/stop, upward sprint starting WR; neither new
suppression nor a new gameplay acceptance is established by cloud tests.

## Step1 implementation
The existing Init/Entry native queue records the original caller/self/thread/TSC.
Add a read-only tag in its reserved pad[0] at native capture. Tag references one
of64 coherent atomic native face-frame records. Publish only admitted hold,
pinned, not face air/stop hop, no current WS; invalidate on new WS/summary/F8/
shutdown. Data is finite native raw game displacement over completed frame time,
face normal, raw stick and RT (-1 when no WGI reader), generation/Transform/time.
Worker dispatch occurs only after the original complete verified-hook/epoch/
snapshot checks, BEFORE duplicate lifecycle filtering. It can therefore recover
pre-WR face evidence even if WR/WS already changed the current hold by receipt.
No callback is added on the native path; no new engine hook/typed signature,
argument/return/control-flow edit or game field write. Original Init/Entry always
tail-jump. Update/Deinit tags are zero. Slot size/offsets retained. Queue loss
continues the existing lifecycle reset; overwritten/missing/older-than100ms
records refuse rather than borrowing a newer frame. Calibrated event timing can
precede coarse sample ms by at most5ms; negative small age is explicitly visible.

WS_FACE_WR_START <=40 per process: t=held model time, planar game speed,
unit move_dir_up/side, stick, RT, init|entry, caller_rva+validity, observed owner,
Hero Transform/generation, native thread and face sample age. Owner correlation
is not proven by equal timings: do not call event.self a verified Hero pointer.
Current runtime caller RVA remains UNKNOWN until owner run; no guessed RVA.

Actual helper ASan/UBSan tests cover projection/normal separation and finite/
fresh evidence. Owned Windows ABI fixture also checks tagged Init/Entry and zero
Update/Deinit while retaining existing register/vector/stack/return assertions.
Production Werror C++ compile and assembly compile pass. No assertions relaxed.

## Conditional step2: missing evidence — stop after step1
Reviewed wallrun_hooks.cpp (start BEFORE_ENABLE log, complete-hook admission),
hook_thunks.S (WS_THUNK final mandatory tail jump), hook_capture.hpp/.cpp,
update_bridge.cpp and repository ABI/refusal references. The existing observer
has NO evidenced Init/Entry refusal/skip route. It intentionally has no typed
game signature. Update's guarded call-through fallback tail-jumps; it is not a
WR-start veto. Disabling recording/hooks/writer dispatch does not cancel native
WR. No authenticated native early-return branch, return value/width, required
initialization/cleanup side effects, or caller's rejection contract was supplied.
An invented return0/RET/flag patch could leave native lifecycle half initialized.

CODEX_NEXT explicitly says: "If no evidenced refusal path exists, do not invent
one: stop after step 1 and report the call site and what evidence is missing."
Therefore step2 and step3 are NOT implemented. No FACE_NO_WR INI or REFUSED log
pretends suppression exists. The next evidence is WS_FACE_WR_START hook and
caller_rva from this owner run, followed by the exact authenticated caller/callee
rejection/early-exit contract; the diagnostic cannot issue that permission.
No jump/zip/fall/camera/mapping/stop-hop changes. Read-only observation only.

Wisdoms WALLSPRINT_INTERNALS3817f5e consulted; no new runtime engine fact or
cross-repository edit is claimed. Runtime effect of WR refusal remains UNKNOWN
because refusal is deliberately absent under the task's conditional instruction.
