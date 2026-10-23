# TEST ONLY, independent owned-function fixture. No captured game code.
.intel_syntax noprefix
.text
.globl ws_test_copy_caller
.def ws_test_copy_caller; .scl 2; .type 32; .endef
.seh_proc ws_test_copy_caller
ws_test_copy_caller:
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
 mov r8d,0x1234
 mov r9d,0x5678
 .fill 0x9e-(.-ws_test_copy_caller),1,0x90
 call ws_test_copy_writer
 add rsp,0x68
 ret
.seh_endproc
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
.globl ws_test_copy_writer
.def ws_test_copy_writer; .scl 2; .type 32; .endef
.seh_proc ws_test_copy_writer
ws_test_copy_writer:
 .seh_endprologue
 .rept 32
 nop
 .endr
 # Retain the independent input for byte-range assertions, then model the
 # proven compact3x3 -> padded Hero orientation copy map.
 movdqu xmm0,XMMWORD PTR [rdx]
 movdqu xmm1,XMMWORD PTR [rdx+16]
 movdqu xmm2,XMMWORD PTR [rdx+32]
 movdqu xmm3,XMMWORD PTR [rdx+48]
 movdqu XMMWORD PTR ws_test_copy_received[rip],xmm0
 movdqu XMMWORD PTR ws_test_copy_received+16[rip],xmm1
 movdqu XMMWORD PTR ws_test_copy_received+32[rip],xmm2
 movdqu XMMWORD PTR ws_test_copy_received+48[rip],xmm3
 mov eax,DWORD PTR [rdx]
 mov DWORD PTR [rcx],eax
 mov eax,DWORD PTR [rdx+4]
 mov DWORD PTR [rcx+4],eax
 mov eax,DWORD PTR [rdx+8]
 mov DWORD PTR [rcx+8],eax
 mov eax,DWORD PTR [rdx+12]
 mov DWORD PTR [rcx+16],eax
 mov eax,DWORD PTR [rdx+16]
 mov DWORD PTR [rcx+20],eax
 mov eax,DWORD PTR [rdx+20]
 mov DWORD PTR [rcx+24],eax
 mov eax,DWORD PTR [rdx+24]
 mov DWORD PTR [rcx+32],eax
 mov eax,DWORD PTR [rdx+28]
 mov DWORD PTR [rcx+36],eax
 mov eax,DWORD PTR [rdx+32]
 mov DWORD PTR [rcx+40],eax
 inc QWORD PTR ws_test_copy_calls[rip]
 mov rax,0xabcdef0011223344
 ret
.seh_endproc
