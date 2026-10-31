.intel_syntax noprefix
.text

.extern ws_request_presence_enabled
.extern ws_request_presence_active_callbacks
.extern ws_request_presence_count
.extern ws_request_presence_overflow
.extern ws_request_presence_contention
.extern ws_request_presence_records
.extern ws_request_presence_original

.globl ws_request_presence_observer
.def ws_request_presence_observer; .scl 2; .type 32; .endef
.seh_proc ws_request_presence_observer
ws_request_presence_observer:
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
    .seh_endprologue

    lock inc DWORD PTR ws_request_presence_active_callbacks[rip]
    cmp DWORD PTR ws_request_presence_enabled[rip], 0
    je ws_request_presence_restore0

    mov eax, DWORD PTR ws_request_presence_count[rip]
    cmp eax, 16
    jae .Loverflow
    lea ecx, [rax + 1]
    lock cmpxchg DWORD PTR ws_request_presence_count[rip], ecx
    jne .Lcontention

    mov r10d, eax
    shl rax, 6
    lea r11, ws_request_presence_records[rip]
    add r11, rax

    mov rax, QWORD PTR 40[rsp]
    mov QWORD PTR 0[r11], rax
    mov rax, QWORD PTR 32[rsp]
    mov QWORD PTR 8[r11], rax
    mov rax, QWORD PTR 24[rsp]
    mov QWORD PTR 16[r11], rax
    mov rax, QWORD PTR 16[rsp]
    mov QWORD PTR 24[r11], rax
    mov rax, QWORD PTR 64[rsp]
    mov QWORD PTR 32[r11], rax
    mov rax, QWORD PTR gs:72
    mov QWORD PTR 40[r11], rax
    mov QWORD PTR 56[r11], r10
    rdtsc
    shl rdx, 32
    or rax, rdx
    mov QWORD PTR 48[r11], rax
    jmp ws_request_presence_restore0

.Loverflow:
    lock inc DWORD PTR ws_request_presence_overflow[rip]
    jmp ws_request_presence_restore0

.Lcontention:
    lock inc DWORD PTR ws_request_presence_contention[rip]
    jmp ws_request_presence_restore0
.seh_endproc

.globl ws_request_presence_restore0
.def ws_request_presence_restore0; .scl 2; .type 32; .endef
.seh_proc ws_request_presence_restore0
ws_request_presence_restore0:
    .seh_stackalloc 64
    .seh_endprologue
    lock dec DWORD PTR ws_request_presence_active_callbacks[rip]
    pop r11
    jmp ws_request_presence_restore1
.seh_endproc

.globl ws_request_presence_restore1
.def ws_request_presence_restore1; .scl 2; .type 32; .endef
.seh_proc ws_request_presence_restore1
ws_request_presence_restore1:
    .seh_stackalloc 56
    .seh_endprologue
    pop r10
    jmp ws_request_presence_restore2
.seh_endproc

.globl ws_request_presence_restore2
.def ws_request_presence_restore2; .scl 2; .type 32; .endef
.seh_proc ws_request_presence_restore2
ws_request_presence_restore2:
    .seh_stackalloc 48
    .seh_endprologue
    pop r9
    jmp ws_request_presence_restore3
.seh_endproc

.globl ws_request_presence_restore3
.def ws_request_presence_restore3; .scl 2; .type 32; .endef
.seh_proc ws_request_presence_restore3
ws_request_presence_restore3:
    .seh_stackalloc 40
    .seh_endprologue
    pop r8
    jmp ws_request_presence_restore4
.seh_endproc

.globl ws_request_presence_restore4
.def ws_request_presence_restore4; .scl 2; .type 32; .endef
.seh_proc ws_request_presence_restore4
ws_request_presence_restore4:
    .seh_stackalloc 32
    .seh_endprologue
    pop rdx
    jmp ws_request_presence_restore5
.seh_endproc

.globl ws_request_presence_restore5
.def ws_request_presence_restore5; .scl 2; .type 32; .endef
.seh_proc ws_request_presence_restore5
ws_request_presence_restore5:
    .seh_stackalloc 24
    .seh_endprologue
    pop rcx
    jmp ws_request_presence_restore6
.seh_endproc

.globl ws_request_presence_restore6
.def ws_request_presence_restore6; .scl 2; .type 32; .endef
.seh_proc ws_request_presence_restore6
ws_request_presence_restore6:
    .seh_stackalloc 16
    .seh_endprologue
    pop rax
    jmp ws_request_presence_restore7
.seh_endproc

.globl ws_request_presence_restore7
.def ws_request_presence_restore7; .scl 2; .type 32; .endef
.seh_proc ws_request_presence_restore7
ws_request_presence_restore7:
    .seh_stackalloc 8
    .seh_endprologue
    popfq
    jmp ws_request_presence_restore8
.seh_endproc

.globl ws_request_presence_restore8
.def ws_request_presence_restore8; .scl 2; .type 32; .endef
.seh_proc ws_request_presence_restore8
ws_request_presence_restore8:
    .seh_endprologue
    jmp QWORD PTR ws_request_presence_original[rip]
.seh_endproc
