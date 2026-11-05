.intel_syntax noprefix
.text
# M5b: HeroCameraManager::UpdateCameraRefMatrix entry (exact build 2.810.0.0, RVA 0x1065680).
# Leaf tail-jump thunk: records RCX (the manager `this`) and continues into the MinHook trampoline.
# No stack or argument-register changes, so the original prologue/unwind state is unchanged.
.globl ws_camref_thunk
.def ws_camref_thunk; .scl 2; .type 32; .endef
# M5e-3c: several manager instances call this entry; the last caller is not always the active camera.
# Record every caller into an 8-slot ring (R10/R11 are volatile scratch at function entry; flags may change,
# which is allowed at a call boundary). The worker then selects the live camera (unit forward, position near hero).
ws_camref_thunk:
 mov QWORD PTR ws_camref_raw[rip],rcx
 mov r10,QWORD PTR ws_camref_seq[rip]
 lea r11,[r10+1]
 mov QWORD PTR ws_camref_seq[rip],r11
 and r10,7
 lea r11,ws_camref_ring[rip]
 mov QWORD PTR [r11+r10*8],rcx
 jmp QWORD PTR ws_camref_original[rip]
