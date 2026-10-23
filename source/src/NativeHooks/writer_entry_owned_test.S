.intel_syntax noprefix
.text
.globl ws_owned_entry_writer
.def ws_owned_entry_writer; .scl 2; .type 32; .endef
.seh_proc ws_owned_entry_writer
ws_owned_entry_writer:
 sub rsp,0x48
 .seh_stackalloc 0x48
 .seh_endprologue
 .rept 32
 nop
 .endr
 mov QWORD PTR [rsp+0x20],rcx
 mov QWORD PTR [rsp+0x28],rdx
 test r8,r8
 je .Lowned_values
 xor r8d,r8d
 call ws_owned_entry_caller
 mov rcx,QWORD PTR [rsp+0x20]
 mov rdx,QWORD PTR [rsp+0x28]
.Lowned_values:
 movss xmm0,DWORD PTR [rdx]
 movss DWORD PTR [rcx+0x30],xmm0
 movss xmm0,DWORD PTR [rdx+4]
 movss DWORD PTR [rcx+0x34],xmm0
 movss xmm0,DWORD PTR [rdx+8]
 movss DWORD PTR [rcx+0x38],xmm0
 movss xmm0,DWORD PTR [rdx+12]
 movss DWORD PTR [rcx+0x3c],xmm0
 mov rax,0x123455667788abcd
 add rsp,0x48
 ret
.seh_endproc
.globl ws_owned_entry_caller
.globl ws_owned_entry_return
.def ws_owned_entry_caller; .scl 2; .type 32; .endef
.seh_proc ws_owned_entry_caller
ws_owned_entry_caller:
 sub rsp,0x28
 .seh_stackalloc 0x28
 .seh_endprologue
 call ws_owned_entry_writer
ws_owned_entry_return:
 add rsp,0x28
 ret
.seh_endproc
