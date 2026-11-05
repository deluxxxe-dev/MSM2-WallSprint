.intel_syntax noprefix
.text
# M5b: HeroCameraManager::UpdateCameraRefMatrix entry (exact build 2.810.0.0, RVA 0x1065680).
# Leaf tail-jump thunk: records RCX (the manager `this`) and continues into the MinHook trampoline.
# No stack, flag or other register changes, so the original prologue/unwind state is unchanged.
.globl ws_camref_thunk
.def ws_camref_thunk; .scl 2; .type 32; .endef
ws_camref_thunk:
 mov QWORD PTR ws_camref_this[rip],rcx
 jmp QWORD PTR ws_camref_original[rip]
