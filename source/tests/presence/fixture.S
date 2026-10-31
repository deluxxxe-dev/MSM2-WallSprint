.intel_syntax noprefix
.ifndef PRESENCE_LEGACY_UNWIND
 .set PRESENCE_LEGACY_UNWIND,0
.endif
.text
# Independently authored effect-equivalent prologues, not native bodies.
.macro BODY delta
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
 mov rax,[rsp+\delta]
 mov QWORD PTR ws_test_gprs+64[rip],rax
 mov rax,[rsp+\delta+40]
 mov QWORD PTR ws_test_gprs+72[rip],rax
 mov rax,[rsp+\delta+48]
 mov QWORD PTR ws_test_gprs+80[rip],rax
 .irp n,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
 vmovdqu YMMWORD PTR ws_test_vectors+32*\n[rip],ymm\n
 .endr
 mov rax,0x77aabbcc11223344
.endm
.globl presence_request
.seh_proc presence_request
presence_request:
 mov r11,rsp
 mov [r11+8],rbx
 mov [r11+16],rbp
 mov [r11+24],rsi
 push rdi
 .seh_pushreg rdi
 sub rsp,1120
 .seh_stackalloc 1120
 .seh_savereg rbx,1136
 .seh_savereg rbp,1144
 .seh_savereg rsi,1152
 .seh_endprologue
 BODY 1128
 mov rbx,[rsp+1136]
 mov rbp,[rsp+1144]
 mov rsi,[rsp+1152]
 add rsp,1120
 pop rdi
 ret
.seh_endproc
.globl presence_gravity
.seh_proc presence_gravity
presence_gravity:
 mov [rsp+8],rbx
 mov [rsp+16],rsi
 push rdi
 .seh_pushreg rdi
 sub rsp,48
 .seh_stackalloc 48
 .seh_savereg rbx,64
 .seh_savereg rsi,72
 .seh_endprologue
 BODY 56
 mov rbx,[rsp+64]
 mov rsi,[rsp+72]
 add rsp,48
 pop rdi
 ret
.seh_endproc
# Test-only proposed observer: no function call, no vector instructions,
# no pointee reads. One bounded CAS reservation, no retry/wait/hot IO.
.macro OBS name, index
.globl \name
.seh_proc \name
\name:
 pushfq
 .seh_stackalloc 8
 .irp r,rax,rcx,rdx,r8,r9,r10,r11
 push \r
 .seh_stackalloc 8
 .endr
 .seh_endprologue
 cmp DWORD PTR presence_enabled[rip],0
 je 9f
 mov eax,DWORD PTR presence_count[rip]
 cmp eax,16
 jae 8f
 lea ecx,[rax+1]
 lock cmpxchg DWORD PTR presence_count[rip],ecx
 jne 8f
 shl rax,6
 lea r11,presence_records[rip]
 add r11,rax
 mov rax,[rsp+40]
 mov [r11],rax
 mov rax,[rsp+32]
 mov [r11+8],rax
 mov rax,[rsp+24]
 mov [r11+16],rax
 mov rax,[rsp+16]
 mov [r11+24],rax
 mov rax,[rsp+64]
 mov [r11+32],rax
 mov rax,QWORD PTR gs:0x48
 mov [r11+40],rax
 rdtsc
 shl rdx,32
 or rax,rdx
 mov [r11+48],rax
 mov QWORD PTR [r11+56],\index
 jmp 9f
8:
 lock inc DWORD PTR presence_suppressed[rip]
9:
 .if !PRESENCE_LEGACY_UNWIND
 .seh_endproc
 .endif
 .set depth,64
 .irp n,0,1,2,3,4,5,6
 .globl \name\()_restore\n
 .if !PRESENCE_LEGACY_UNWIND
 .seh_proc \name\()_restore\n
 .endif
\name\()_restore\n:
 .if !PRESENCE_LEGACY_UNWIND
 .seh_stackalloc depth
 .seh_endprologue
 .endif
 .if \n==0
 pop r11
 .elseif \n==1
 pop r10
 .elseif \n==2
 pop r9
 .elseif \n==3
 pop r8
 .elseif \n==4
 pop rdx
 .elseif \n==5
 pop rcx
 .else
 pop rax
 .endif
 .if !PRESENCE_LEGACY_UNWIND
 .seh_endproc
 .endif
 .set depth,depth-8
 .endr
 .globl \name\()_restore7
 .if !PRESENCE_LEGACY_UNWIND
 .seh_proc \name\()_restore7
 .endif
\name\()_restore7:
 .if !PRESENCE_LEGACY_UNWIND
 .seh_stackalloc 8
 .seh_endprologue
 .endif
 popfq
 .if !PRESENCE_LEGACY_UNWIND
 .seh_endproc
 .endif
 .globl \name\()_restore8
 .if !PRESENCE_LEGACY_UNWIND
 .seh_proc \name\()_restore8
 .endif
\name\()_restore8:
 .if !PRESENCE_LEGACY_UNWIND
 .seh_endprologue
 .endif
 jmp QWORD PTR presence_originals+8*\index[rip]
.seh_endproc
.endm
OBS presence_observer_request,0
OBS presence_observer_gravity,1
.globl presence_race_target
.seh_proc presence_race_target
presence_race_target:
 .seh_endprologue
 lock inc DWORD PTR presence_forwarded[rip]
 mov rax,0x77aabbcc11223344
 ret
.seh_endproc
# Valid OWNED call-return control for StackWalk differential diagnostics.
.globl presence_unwind_caller
.seh_proc presence_unwind_caller
presence_unwind_caller:
 sub rsp,40
 .seh_stackalloc 40
 .seh_endprologue
 call presence_race_target
.globl presence_unwind_return
presence_unwind_return:
 add rsp,40
 ret
.seh_endproc
