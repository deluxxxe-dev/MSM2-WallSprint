.intel_syntax noprefix
.text
.globl ws_test_probe_target
.def ws_test_probe_target; .scl 2; .type 32; .endef
.seh_proc ws_test_probe_target
ws_test_probe_target:
 .seh_endprologue
 .rept 32
 nop
 .endr
 mov QWORD PTR ws_test_gprs[rip],rax
 mov QWORD PTR ws_test_gprs+8[rip],rcx
 mov QWORD PTR ws_test_gprs+16[rip],rdx
 mov QWORD PTR ws_test_gprs+24[rip],r8
 mov QWORD PTR ws_test_gprs+32[rip],r9
 mov QWORD PTR ws_test_gprs+40[rip],r10
 mov QWORD PTR ws_test_gprs+48[rip],r11
 pushfq
 pop rax
 mov QWORD PTR ws_test_gprs+56[rip],rax
 mov rax,QWORD PTR [rsp]
 mov QWORD PTR ws_test_gprs+64[rip],rax
 mov rax,QWORD PTR [rsp+40]
 mov QWORD PTR ws_test_gprs+72[rip],rax
 mov rax,QWORD PTR [rsp+48]
 mov QWORD PTR ws_test_gprs+80[rip],rax
 vmovdqu YMMWORD PTR ws_test_vectors+0[rip],ymm0
 vmovdqu YMMWORD PTR ws_test_vectors+32[rip],ymm1
 vmovdqu YMMWORD PTR ws_test_vectors+64[rip],ymm2
 vmovdqu YMMWORD PTR ws_test_vectors+96[rip],ymm3
 vmovdqu YMMWORD PTR ws_test_vectors+128[rip],ymm4
 vmovdqu YMMWORD PTR ws_test_vectors+160[rip],ymm5
 vmovdqu YMMWORD PTR ws_test_vectors+192[rip],ymm6
 vmovdqu YMMWORD PTR ws_test_vectors+224[rip],ymm7
 vmovdqu YMMWORD PTR ws_test_vectors+256[rip],ymm8
 vmovdqu YMMWORD PTR ws_test_vectors+288[rip],ymm9
 vmovdqu YMMWORD PTR ws_test_vectors+320[rip],ymm10
 vmovdqu YMMWORD PTR ws_test_vectors+352[rip],ymm11
 vmovdqu YMMWORD PTR ws_test_vectors+384[rip],ymm12
 vmovdqu YMMWORD PTR ws_test_vectors+416[rip],ymm13
 vmovdqu YMMWORD PTR ws_test_vectors+448[rip],ymm14
 vmovdqu YMMWORD PTR ws_test_vectors+480[rip],ymm15
 mov rax,0x77aabbcc11223344
 ret
.seh_endproc
.globl ws_test_invoke_probe
.def ws_test_invoke_probe; .scl 2; .type 32; .endef
.seh_proc ws_test_invoke_probe
ws_test_invoke_probe:
 sub rsp,456
 .seh_stackalloc 456
 vmovdqu YMMWORD PTR [rsp+64],ymm6
 .seh_savexmm xmm6,64
 vmovdqu YMMWORD PTR [rsp+96],ymm7
 .seh_savexmm xmm7,96
 vmovdqu YMMWORD PTR [rsp+128],ymm8
 .seh_savexmm xmm8,128
 vmovdqu YMMWORD PTR [rsp+160],ymm9
 .seh_savexmm xmm9,160
 vmovdqu YMMWORD PTR [rsp+192],ymm10
 .seh_savexmm xmm10,192
 vmovdqu YMMWORD PTR [rsp+224],ymm11
 .seh_savexmm xmm11,224
 vmovdqu YMMWORD PTR [rsp+256],ymm12
 .seh_savexmm xmm12,256
 vmovdqu YMMWORD PTR [rsp+288],ymm13
 .seh_savexmm xmm13,288
 vmovdqu YMMWORD PTR [rsp+320],ymm14
 .seh_savexmm xmm14,320
 vmovdqu YMMWORD PTR [rsp+352],ymm15
 .seh_savexmm xmm15,352
 mov QWORD PTR [rsp+384],rbx
 .seh_savereg rbx,384
 mov QWORD PTR [rsp+392],rbp
 .seh_savereg rbp,392
 mov QWORD PTR [rsp+400],rsi
 .seh_savereg rsi,400
 mov QWORD PTR [rsp+408],rdi
 .seh_savereg rdi,408
 mov QWORD PTR [rsp+416],r12
 .seh_savereg r12,416
 mov QWORD PTR [rsp+424],r13
 .seh_savereg r13,424
 mov QWORD PTR [rsp+432],r14
 .seh_savereg r14,432
 mov QWORD PTR [rsp+440],r15
 .seh_savereg r15,440
 .seh_endprologue
 mov r11,rcx
 mov rbx,0x2222000011110000
 mov rbp,0x2222000011110001
 mov rsi,0x2222000011110002
 mov rdi,0x2222000011110003
 mov r12,0x2222000011110004
 mov r13,0x2222000011110005
 mov r14,0x2222000011110006
 mov r15,0x2222000011110007
 vmovdqu ymm0,YMMWORD PTR ws_test_vector_input+0[rip]
 vmovdqu ymm1,YMMWORD PTR ws_test_vector_input+32[rip]
 vmovdqu ymm2,YMMWORD PTR ws_test_vector_input+64[rip]
 vmovdqu ymm3,YMMWORD PTR ws_test_vector_input+96[rip]
 vmovdqu ymm4,YMMWORD PTR ws_test_vector_input+128[rip]
 vmovdqu ymm5,YMMWORD PTR ws_test_vector_input+160[rip]
 vmovdqu ymm6,YMMWORD PTR ws_test_vector_input+192[rip]
 vmovdqu ymm7,YMMWORD PTR ws_test_vector_input+224[rip]
 vmovdqu ymm8,YMMWORD PTR ws_test_vector_input+256[rip]
 vmovdqu ymm9,YMMWORD PTR ws_test_vector_input+288[rip]
 vmovdqu ymm10,YMMWORD PTR ws_test_vector_input+320[rip]
 vmovdqu ymm11,YMMWORD PTR ws_test_vector_input+352[rip]
 vmovdqu ymm12,YMMWORD PTR ws_test_vector_input+384[rip]
 vmovdqu ymm13,YMMWORD PTR ws_test_vector_input+416[rip]
 vmovdqu ymm14,YMMWORD PTR ws_test_vector_input+448[rip]
 vmovdqu ymm15,YMMWORD PTR ws_test_vector_input+480[rip]
 mov rax,0x1111222233334444
 mov rcx,0x1122334455667788
 mov rdx,0x2233445566778899
 mov r8,0x33445566778899aa
 mov r9,0x445566778899aabb
 mov r10,0x5566778899aabbcc
 mov QWORD PTR [rsp+32],12345
 mov QWORD PTR [rsp+40],67890
 cmp rax,rax
 stc
 call r11
 mov QWORD PTR ws_test_return_gprs[rip],rax
 mov QWORD PTR ws_test_return_gprs+8[rip],rdx
 pushfq
 pop QWORD PTR ws_test_return_gprs+16[rip]
 mov QWORD PTR ws_test_nonvolatile+0[rip],rbx
 mov QWORD PTR ws_test_nonvolatile+8[rip],rbp
 mov QWORD PTR ws_test_nonvolatile+16[rip],rsi
 mov QWORD PTR ws_test_nonvolatile+24[rip],rdi
 mov QWORD PTR ws_test_nonvolatile+32[rip],r12
 mov QWORD PTR ws_test_nonvolatile+40[rip],r13
 mov QWORD PTR ws_test_nonvolatile+48[rip],r14
 mov QWORD PTR ws_test_nonvolatile+56[rip],r15
 vmovdqu YMMWORD PTR ws_test_returns+0[rip],ymm0
 vmovdqu YMMWORD PTR ws_test_returns+32[rip],ymm1
 vmovdqu YMMWORD PTR ws_test_returns+64[rip],ymm2
 vmovdqu YMMWORD PTR ws_test_returns+96[rip],ymm3
 vmovdqu YMMWORD PTR ws_test_returns+128[rip],ymm4
 vmovdqu YMMWORD PTR ws_test_returns+160[rip],ymm5
 vmovdqu YMMWORD PTR ws_test_returns+192[rip],ymm6
 vmovdqu YMMWORD PTR ws_test_returns+224[rip],ymm7
 vmovdqu YMMWORD PTR ws_test_returns+256[rip],ymm8
 vmovdqu YMMWORD PTR ws_test_returns+288[rip],ymm9
 vmovdqu YMMWORD PTR ws_test_returns+320[rip],ymm10
 vmovdqu YMMWORD PTR ws_test_returns+352[rip],ymm11
 vmovdqu YMMWORD PTR ws_test_returns+384[rip],ymm12
 vmovdqu YMMWORD PTR ws_test_returns+416[rip],ymm13
 vmovdqu YMMWORD PTR ws_test_returns+448[rip],ymm14
 vmovdqu YMMWORD PTR ws_test_returns+480[rip],ymm15
 mov QWORD PTR ws_test_result[rip],rax
 vmovdqu ymm6,YMMWORD PTR [rsp+64]
 vmovdqu ymm7,YMMWORD PTR [rsp+96]
 vmovdqu ymm8,YMMWORD PTR [rsp+128]
 vmovdqu ymm9,YMMWORD PTR [rsp+160]
 vmovdqu ymm10,YMMWORD PTR [rsp+192]
 vmovdqu ymm11,YMMWORD PTR [rsp+224]
 vmovdqu ymm12,YMMWORD PTR [rsp+256]
 vmovdqu ymm13,YMMWORD PTR [rsp+288]
 vmovdqu ymm14,YMMWORD PTR [rsp+320]
 vmovdqu ymm15,YMMWORD PTR [rsp+352]
 mov rbx,QWORD PTR [rsp+384]
 mov rbp,QWORD PTR [rsp+392]
 mov rsi,QWORD PTR [rsp+400]
 mov rdi,QWORD PTR [rsp+408]
 mov r12,QWORD PTR [rsp+416]
 mov r13,QWORD PTR [rsp+424]
 mov r14,QWORD PTR [rsp+432]
 mov r15,QWORD PTR [rsp+440]
 vzeroupper
 add rsp,456
 ret
.seh_endproc
