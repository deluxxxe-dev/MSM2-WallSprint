.intel_syntax noprefix
.text
.globl ws_reset_copy_thunk
.def ws_reset_copy_thunk; .scl 2; .type 32; .endef
.seh_proc ws_reset_copy_thunk
ws_reset_copy_thunk:
 pushfq
 .seh_stackalloc 8
 push rax
 .seh_stackalloc 8
 push rcx
 .seh_stackalloc 8
 push rdx
 .seh_stackalloc 8
 push r8
 .seh_stackalloc 8
 push r9
 .seh_stackalloc 8
 push r10
 .seh_stackalloc 8
 push r11
 .seh_stackalloc 8
 sub rsp,0x428
 .seh_stackalloc 0x428
 .seh_endprologue
 mov eax,DWORD PTR ws_reset_copy_recording[rip]
 or eax,DWORD PTR ws_native_source_enabled[rip]
 je .Lcopy_tail
 fxsave64 [rsp+0x20]
 vmovdqu YMMWORD PTR [rsp+0x220],ymm0
 vmovdqu YMMWORD PTR [rsp+0x240],ymm1
 vmovdqu YMMWORD PTR [rsp+0x260],ymm2
 vmovdqu YMMWORD PTR [rsp+0x280],ymm3
 vmovdqu YMMWORD PTR [rsp+0x2a0],ymm4
 vmovdqu YMMWORD PTR [rsp+0x2c0],ymm5
 vmovdqu YMMWORD PTR [rsp+0x2e0],ymm6
 vmovdqu YMMWORD PTR [rsp+0x300],ymm7
 vmovdqu YMMWORD PTR [rsp+0x320],ymm8
 vmovdqu YMMWORD PTR [rsp+0x340],ymm9
 vmovdqu YMMWORD PTR [rsp+0x360],ymm10
 vmovdqu YMMWORD PTR [rsp+0x380],ymm11
 vmovdqu YMMWORD PTR [rsp+0x3a0],ymm12
 vmovdqu YMMWORD PTR [rsp+0x3c0],ymm13
 vmovdqu YMMWORD PTR [rsp+0x3e0],ymm14
 vmovdqu YMMWORD PTR [rsp+0x400],ymm15
 lea rcx,[rsp+0x428]
 call ws_reset_copy_capture
 vmovdqu ymm0,YMMWORD PTR [rsp+0x220]
 vmovdqu ymm1,YMMWORD PTR [rsp+0x240]
 vmovdqu ymm2,YMMWORD PTR [rsp+0x260]
 vmovdqu ymm3,YMMWORD PTR [rsp+0x280]
 vmovdqu ymm4,YMMWORD PTR [rsp+0x2a0]
 vmovdqu ymm5,YMMWORD PTR [rsp+0x2c0]
 vmovdqu ymm6,YMMWORD PTR [rsp+0x2e0]
 vmovdqu ymm7,YMMWORD PTR [rsp+0x300]
 vmovdqu ymm8,YMMWORD PTR [rsp+0x320]
 vmovdqu ymm9,YMMWORD PTR [rsp+0x340]
 vmovdqu ymm10,YMMWORD PTR [rsp+0x360]
 vmovdqu ymm11,YMMWORD PTR [rsp+0x380]
 vmovdqu ymm12,YMMWORD PTR [rsp+0x3a0]
 vmovdqu ymm13,YMMWORD PTR [rsp+0x3c0]
 vmovdqu ymm14,YMMWORD PTR [rsp+0x3e0]
 vmovdqu ymm15,YMMWORD PTR [rsp+0x400]
 fxrstor64 [rsp+0x20]
.Lcopy_tail:
 add rsp,0x428
 pop r11
 pop r10
 pop r9
 pop r8
 pop rdx
 pop rcx
 pop rax
 popfq
 jmp QWORD PTR ws_reset_copy_original[rip]
.seh_endproc
