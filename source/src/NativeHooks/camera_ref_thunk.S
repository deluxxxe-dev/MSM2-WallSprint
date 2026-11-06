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

# M6r (read-only diagnostic): LookCameraMover reference-matrix setter entry (exact build RVA 0x3350220, .pdata begin
# checked by the caller). Saves every volatile GPR and XMM0-5, calls ws_camset_record(rcx, rdx, r8), restores them and
# tail-jumps into the MinHook trampoline. Stack: entry rsp = 8 mod 16; 7 pushes (56) + 0x80 keeps 16-byte alignment.
.globl ws_camset_thunk
.def ws_camset_thunk; .scl 2; .type 32; .endef
ws_camset_thunk:
 push rcx
 push rdx
 push r8
 push r9
 push rax
 push r10
 push r11
 sub rsp,0x80
 movdqu XMMWORD PTR [rsp+0x20],xmm0
 movdqu XMMWORD PTR [rsp+0x30],xmm1
 movdqu XMMWORD PTR [rsp+0x40],xmm2
 movdqu XMMWORD PTR [rsp+0x50],xmm3
 movdqu XMMWORD PTR [rsp+0x60],xmm4
 movdqu XMMWORD PTR [rsp+0x70],xmm5
 call ws_camset_record
 movdqu xmm0,XMMWORD PTR [rsp+0x20]
 movdqu xmm1,XMMWORD PTR [rsp+0x30]
 movdqu xmm2,XMMWORD PTR [rsp+0x40]
 movdqu xmm3,XMMWORD PTR [rsp+0x50]
 movdqu xmm4,XMMWORD PTR [rsp+0x60]
 movdqu xmm5,XMMWORD PTR [rsp+0x70]
 add rsp,0x80
 pop r11
 pop r10
 pop rax
 pop r9
 pop r8
 pop rdx
 pop rcx
 jmp QWORD PTR ws_camset_original[rip]
