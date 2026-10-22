# TEST ONLY, independent owned-function fixture. No captured game code.
.intel_syntax noprefix
.text
.globl ws_test_stack_caller
.def ws_test_stack_caller; .scl 2; .type 32; .endef
.seh_proc ws_test_stack_caller
ws_test_stack_caller:
 sub rsp,0x68
 .seh_stackalloc 0x68
 .seh_endprologue
 movdqu xmm0,XMMWORD PTR [rdx]
 movdqu xmm1,XMMWORD PTR [rdx+16]
 movdqu xmm2,XMMWORD PTR [rdx+32]
 movdqu xmm3,XMMWORD PTR [rdx+48]
 movdqu XMMWORD PTR [rsp+0x20],xmm0
 movdqu XMMWORD PTR [rsp+0x30],xmm1
 movdqu XMMWORD PTR [rsp+0x40],xmm2
 movdqu XMMWORD PTR [rsp+0x50],xmm3
 lea rdx,[rsp+0x20]
 xor r8d,r8d
 .fill 0x9e-(.-ws_test_stack_caller),1,0x90
 call ws_test_native_source_writer
 add rsp,0x68
 ret
.seh_endproc
.globl ws_test_native_source_writer
.def ws_test_native_source_writer; .scl 2; .type 32; .endef
.seh_proc ws_test_native_source_writer
ws_test_native_source_writer:
 .seh_endprologue
 .rept 32
 nop
 .endr
 movdqu xmm0,XMMWORD PTR [rdx]
 movdqu xmm1,XMMWORD PTR [rdx+16]
 movdqu xmm2,XMMWORD PTR [rdx+32]
 movdqu xmm3,XMMWORD PTR [rdx+48]
 movdqu XMMWORD PTR ws_test_source_received[rip],xmm0
 movdqu XMMWORD PTR ws_test_source_received+16[rip],xmm1
 movdqu XMMWORD PTR ws_test_source_received+32[rip],xmm2
 movdqu XMMWORD PTR ws_test_source_received+48[rip],xmm3
 movdqu XMMWORD PTR [rcx],xmm0
 movdqu XMMWORD PTR [rcx+16],xmm1
 movdqu XMMWORD PTR [rcx+32],xmm2
 movdqu XMMWORD PTR [rcx+48],xmm3
 inc QWORD PTR ws_test_source_calls[rip]
 mov rax,0xabcdef0011223344
 ret
.seh_endproc
