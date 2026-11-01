.intel_syntax noprefix
.text
# Capture path remains pass-through. Optional admitted writer calls local C++
# before native traversal, saving x87/MXCSR and YMM0-15. Native return address
# and all stack arguments remain untouched; disabled writer makes no call.
.macro WS_THUNK symbol, kind
.globl \symbol
.def \symbol; .scl 2; .type 32; .endef
.seh_proc \symbol
\symbol:
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
 cmp DWORD PTR ws_hook_recording[rip],0
 je 9f
 # Bounded, nonblocking single-CAS attempt. Contention/full queues drop, never stall gameplay.
 mov rax,QWORD PTR ws_hook_head[rip]
 mov r8,rax
 mov r9,rax
 and r9,1023
 shl r9,6
 lea r10,ws_hook_slots[rip]
 add r10,r9
 cmp QWORD PTR [r10],rax
 jne 8f
 lea rdx,[rax+1]
 lock cmpxchg QWORD PTR ws_hook_head[rip],rdx
 jne 8f
 # rdtsc uses only EDX:EAX, saved above. Worker calibrates to a monotonic clock.
 lfence
 rdtsc
 shl rdx,32
 or rax,rdx
 mov QWORD PTR [r10+8],rax
 mov rax,QWORD PTR [rsp+0x428+40]
 mov QWORD PTR [r10+16],rax
 mov rax,QWORD PTR [rsp+0x428+64]
 mov QWORD PTR [r10+24],rax
 mov eax,DWORD PTR gs:[0x48]
 mov DWORD PTR [r10+36],eax
 mov DWORD PTR [r10+32],\kind
 lea rax,[r8+1]
 # x86-64 store-release publishes payload; C++ consumer uses acquire.
 mov QWORD PTR [r10],rax
 jmp 9f
8:
 lock inc QWORD PTR ws_hook_dropped[rip]
9:
 mov eax,DWORD PTR ws_write_dispatch_enabled[rip]
 or eax,DWORD PTR ws_source_lifecycle_enabled[rip]
 je 7f
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
 cmp DWORD PTR ws_source_lifecycle_enabled[rip],0
 je 6f
 mov ecx,\kind
 mov rdx,QWORD PTR [rsp+0x428+40]
 call QWORD PTR ws_source_lifecycle_callback[rip]
6:
 cmp DWORD PTR ws_write_dispatch_enabled[rip],0
 je 5f
 mov ecx,\kind
 mov rdx,QWORD PTR [rsp+0x428+40]
 call ws_write_dispatch
5:
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
7:
 add rsp,0x428
 pop r11
 pop r10
 pop r9
 pop r8
 pop rdx
 pop rcx
 pop rax
 popfq
 # Tail-jump through the MinHook trampoline: original arguments and original return address.
 jmp QWORD PTR ws_hook_originals+8*\kind[rip]
.seh_endproc
.endm
WS_THUNK ws_hook_init,0
WS_THUNK ws_hook_entry,1
WS_THUNK ws_hook_deinit,3

# Update alone uses a standard Win64 call-through wrapper. Fixed unwindable frame
# contains a bounded clone of the WHOLE original caller frame, so stack argument
# arity is not guessed. Bridge rejects missing/oversized unwind metadata and uses
# the unchanged tail-jump path. No return-address substitution, TLS return trap,
# shadow-stack bypass. R15 is a standard saved/restored unwind frame pointer.
.globl ws_hook_update
.def ws_hook_update; .scl 2; .type 32; .endef
.seh_proc ws_hook_update
ws_hook_update:
 push r15
 .seh_pushreg r15
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
 sub rsp,0x1000
 .seh_stackalloc 0x1000
 test BYTE PTR [rsp],0
 sub rsp,0x480
 .seh_stackalloc 0x480
 mov r15,rsp
 .seh_setframe r15,0
 .seh_endprologue
 cmp DWORD PTR ws_hook_recording[rip],0
 je 9f
 # Bounded, nonblocking single-CAS attempt. Contention/full queues drop, never stall gameplay.
 mov rax,QWORD PTR ws_hook_head[rip]
 mov r8,rax
 mov r9,rax
 and r9,1023
 shl r9,6
 lea r10,ws_hook_slots[rip]
 add r10,r9
 cmp QWORD PTR [r10],rax
 jne 8f
 lea rdx,[rax+1]
 lock cmpxchg QWORD PTR ws_hook_head[rip],rdx
 jne 8f
 # rdtsc uses only EDX:EAX, saved above. Worker calibrates to a monotonic clock.
 lfence
 rdtsc
 shl rdx,32
 or rax,rdx
 mov QWORD PTR [r10+8],rax
 mov rax,QWORD PTR [rsp+0x1480+40]
 mov QWORD PTR [r10+16],rax
 mov rax,QWORD PTR [rsp+0x1480+72]
 mov QWORD PTR [r10+24],rax
 mov eax,DWORD PTR gs:[0x48]
 mov DWORD PTR [r10+36],eax
 mov DWORD PTR [r10+32],2
 lea rax,[r8+1]
 # x86-64 store-release publishes payload; C++ consumer uses acquire.
 mov QWORD PTR [r10],rax
 jmp 9f
8:
 lock inc QWORD PTR ws_hook_dropped[rip]
9:
 mov eax,DWORD PTR ws_write_dispatch_enabled[rip]
 or eax,DWORD PTR ws_source_lifecycle_enabled[rip]
 or eax,DWORD PTR ws_update_probe_enabled[rip]
 je .Lupdate_tail
 fxsave64 [rsp+0x1000]
 vmovdqu YMMWORD PTR [rsp+0x1200],ymm0
 vmovdqu YMMWORD PTR [rsp+0x1220],ymm1
 vmovdqu YMMWORD PTR [rsp+0x1240],ymm2
 vmovdqu YMMWORD PTR [rsp+0x1260],ymm3
 vmovdqu YMMWORD PTR [rsp+0x1280],ymm4
 vmovdqu YMMWORD PTR [rsp+0x12a0],ymm5
 vmovdqu YMMWORD PTR [rsp+0x12c0],ymm6
 vmovdqu YMMWORD PTR [rsp+0x12e0],ymm7
 vmovdqu YMMWORD PTR [rsp+0x1300],ymm8
 vmovdqu YMMWORD PTR [rsp+0x1320],ymm9
 vmovdqu YMMWORD PTR [rsp+0x1340],ymm10
 vmovdqu YMMWORD PTR [rsp+0x1360],ymm11
 vmovdqu YMMWORD PTR [rsp+0x1380],ymm12
 vmovdqu YMMWORD PTR [rsp+0x13a0],ymm13
 vmovdqu YMMWORD PTR [rsp+0x13c0],ymm14
 vmovdqu YMMWORD PTR [rsp+0x13e0],ymm15
 cmp DWORD PTR ws_source_lifecycle_enabled[rip],0
 je .Lsource_lifecycle_done
 mov ecx,2
 mov rdx,QWORD PTR [rsp+0x1480+40]
 call QWORD PTR ws_source_lifecycle_callback[rip]
.Lsource_lifecycle_done:
 cmp DWORD PTR ws_update_probe_enabled[rip],0
 je .Lupdate_probe_done
 mov rcx,QWORD PTR [rsp+0x1480+40]
 call ws_update_probe_sample
.Lupdate_probe_done:
 cmp DWORD PTR ws_write_dispatch_enabled[rip],0
 je .Lupdate_restore_tail
 lea rax,[rsp+0x1480+72]
 mov QWORD PTR [rsp+0x1400],rax
 mov QWORD PTR [rsp+0x1408],rbx
 mov QWORD PTR [rsp+0x1410],rbp
 mov QWORD PTR [rsp+0x1418],rsi
 mov QWORD PTR [rsp+0x1420],rdi
 mov QWORD PTR [rsp+0x1428],r12
 mov QWORD PTR [rsp+0x1430],r13
 mov QWORD PTR [rsp+0x1438],r14
 mov rax,QWORD PTR [rsp+0x1480+64]
 mov QWORD PTR [rsp+0x1440],rax
 mov rcx,QWORD PTR [rsp+0x1480+40]
 lea rdx,[rsp+0x1400]
 mov r8,rsp
 call ws_update_prepare
 mov QWORD PTR [rsp+0x1448],rax
 test rax,rax
 je .Lupdate_restore_tail
 # Local callbacks may use outgoing home slots. Restore all four homes too.
 mov r10,QWORD PTR [rsp+0x1400]
 mov rax,QWORD PTR [r10+8]
 mov QWORD PTR [rsp],rax
 mov rax,QWORD PTR [r10+16]
 mov QWORD PTR [rsp+8],rax
 mov rax,QWORD PTR [r10+24]
 mov QWORD PTR [rsp+16],rax
 mov rax,QWORD PTR [r10+32]
 mov QWORD PTR [rsp+24],rax
 vmovdqu ymm0,YMMWORD PTR [rsp+0x1200]
 vmovdqu ymm1,YMMWORD PTR [rsp+0x1220]
 vmovdqu ymm2,YMMWORD PTR [rsp+0x1240]
 vmovdqu ymm3,YMMWORD PTR [rsp+0x1260]
 vmovdqu ymm4,YMMWORD PTR [rsp+0x1280]
 vmovdqu ymm5,YMMWORD PTR [rsp+0x12a0]
 vmovdqu ymm6,YMMWORD PTR [rsp+0x12c0]
 vmovdqu ymm7,YMMWORD PTR [rsp+0x12e0]
 vmovdqu ymm8,YMMWORD PTR [rsp+0x1300]
 vmovdqu ymm9,YMMWORD PTR [rsp+0x1320]
 vmovdqu ymm10,YMMWORD PTR [rsp+0x1340]
 vmovdqu ymm11,YMMWORD PTR [rsp+0x1360]
 vmovdqu ymm12,YMMWORD PTR [rsp+0x1380]
 vmovdqu ymm13,YMMWORD PTR [rsp+0x13a0]
 vmovdqu ymm14,YMMWORD PTR [rsp+0x13c0]
 vmovdqu ymm15,YMMWORD PTR [rsp+0x13e0]
 fxrstor64 [rsp+0x1000]
 push QWORD PTR [rsp+0x1480+56]
 popfq
 mov r11,QWORD PTR [rsp+0x1480+0]
 mov r10,QWORD PTR [rsp+0x1480+8]
 mov r9,QWORD PTR [rsp+0x1480+16]
 mov r8,QWORD PTR [rsp+0x1480+24]
 mov rdx,QWORD PTR [rsp+0x1480+32]
 mov rcx,QWORD PTR [rsp+0x1480+40]
 mov rax,QWORD PTR [rsp+0x1480+48]
 call QWORD PTR ws_hook_originals+16[rip]
.globl ws_update_after_native
ws_update_after_native:
 mov QWORD PTR [rsp+0x1480+0],r11
 mov QWORD PTR [rsp+0x1480+8],r10
 mov QWORD PTR [rsp+0x1480+16],r9
 mov QWORD PTR [rsp+0x1480+24],r8
 mov QWORD PTR [rsp+0x1480+32],rdx
 mov QWORD PTR [rsp+0x1480+40],rcx
 mov QWORD PTR [rsp+0x1480+48],rax
 pushfq
 pop QWORD PTR [rsp+0x1480+56]
 fxsave64 [rsp+0x1000]
 vmovdqu YMMWORD PTR [rsp+0x1200],ymm0
 vmovdqu YMMWORD PTR [rsp+0x1220],ymm1
 vmovdqu YMMWORD PTR [rsp+0x1240],ymm2
 vmovdqu YMMWORD PTR [rsp+0x1260],ymm3
 vmovdqu YMMWORD PTR [rsp+0x1280],ymm4
 vmovdqu YMMWORD PTR [rsp+0x12a0],ymm5
 vmovdqu YMMWORD PTR [rsp+0x12c0],ymm6
 vmovdqu YMMWORD PTR [rsp+0x12e0],ymm7
 vmovdqu YMMWORD PTR [rsp+0x1300],ymm8
 vmovdqu YMMWORD PTR [rsp+0x1320],ymm9
 vmovdqu YMMWORD PTR [rsp+0x1340],ymm10
 vmovdqu YMMWORD PTR [rsp+0x1360],ymm11
 vmovdqu YMMWORD PTR [rsp+0x1380],ymm12
 vmovdqu YMMWORD PTR [rsp+0x13a0],ymm13
 vmovdqu YMMWORD PTR [rsp+0x13c0],ymm14
 vmovdqu YMMWORD PTR [rsp+0x13e0],ymm15
 mov rcx,QWORD PTR [rsp+0x1450]
 mov rdx,QWORD PTR [rsp+0x1448]
 call ws_write_update_post
 vmovdqu ymm0,YMMWORD PTR [rsp+0x1200]
 vmovdqu ymm1,YMMWORD PTR [rsp+0x1220]
 vmovdqu ymm2,YMMWORD PTR [rsp+0x1240]
 vmovdqu ymm3,YMMWORD PTR [rsp+0x1260]
 vmovdqu ymm4,YMMWORD PTR [rsp+0x1280]
 vmovdqu ymm5,YMMWORD PTR [rsp+0x12a0]
 vmovdqu ymm6,YMMWORD PTR [rsp+0x12c0]
 vmovdqu ymm7,YMMWORD PTR [rsp+0x12e0]
 vmovdqu ymm8,YMMWORD PTR [rsp+0x1300]
 vmovdqu ymm9,YMMWORD PTR [rsp+0x1320]
 vmovdqu ymm10,YMMWORD PTR [rsp+0x1340]
 vmovdqu ymm11,YMMWORD PTR [rsp+0x1360]
 vmovdqu ymm12,YMMWORD PTR [rsp+0x1380]
 vmovdqu ymm13,YMMWORD PTR [rsp+0x13a0]
 vmovdqu ymm14,YMMWORD PTR [rsp+0x13c0]
 vmovdqu ymm15,YMMWORD PTR [rsp+0x13e0]
 fxrstor64 [rsp+0x1000]
 push QWORD PTR [rsp+0x1480+56]
 popfq
 mov r11,QWORD PTR [rsp+0x1480+0]
 mov r10,QWORD PTR [rsp+0x1480+8]
 mov r9,QWORD PTR [rsp+0x1480+16]
 mov r8,QWORD PTR [rsp+0x1480+24]
 mov rdx,QWORD PTR [rsp+0x1480+32]
 mov rcx,QWORD PTR [rsp+0x1480+40]
 mov rax,QWORD PTR [rsp+0x1480+48]
 lea rsp,[r15+0x14c0]
 pop r15
 ret
.Lupdate_restore_tail:
 vmovdqu ymm0,YMMWORD PTR [rsp+0x1200]
 vmovdqu ymm1,YMMWORD PTR [rsp+0x1220]
 vmovdqu ymm2,YMMWORD PTR [rsp+0x1240]
 vmovdqu ymm3,YMMWORD PTR [rsp+0x1260]
 vmovdqu ymm4,YMMWORD PTR [rsp+0x1280]
 vmovdqu ymm5,YMMWORD PTR [rsp+0x12a0]
 vmovdqu ymm6,YMMWORD PTR [rsp+0x12c0]
 vmovdqu ymm7,YMMWORD PTR [rsp+0x12e0]
 vmovdqu ymm8,YMMWORD PTR [rsp+0x1300]
 vmovdqu ymm9,YMMWORD PTR [rsp+0x1320]
 vmovdqu ymm10,YMMWORD PTR [rsp+0x1340]
 vmovdqu ymm11,YMMWORD PTR [rsp+0x1360]
 vmovdqu ymm12,YMMWORD PTR [rsp+0x1380]
 vmovdqu ymm13,YMMWORD PTR [rsp+0x13a0]
 vmovdqu ymm14,YMMWORD PTR [rsp+0x13c0]
 vmovdqu ymm15,YMMWORD PTR [rsp+0x13e0]
 fxrstor64 [rsp+0x1000]
.Lupdate_tail:
 push QWORD PTR [rsp+0x1480+56]
 popfq
 mov r11,QWORD PTR [rsp+0x1480+0]
 mov r10,QWORD PTR [rsp+0x1480+8]
 mov r9,QWORD PTR [rsp+0x1480+16]
 mov r8,QWORD PTR [rsp+0x1480+24]
 mov rdx,QWORD PTR [rsp+0x1480+32]
 mov rcx,QWORD PTR [rsp+0x1480+40]
 mov rax,QWORD PTR [rsp+0x1480+48]
 lea rsp,[r15+0x14c0]
 pop r15
 jmp QWORD PTR ws_hook_originals+16[rip]
.seh_endproc
