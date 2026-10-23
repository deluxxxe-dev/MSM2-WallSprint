/* Owned offline decoder fixtures. These functions are never game gates. */
.intel_syntax noprefix
.text
.globl ws_rdx_slice,ws_rdx_slice_end,ws_rdx_slice_x,ws_rdx_slice_y,ws_rdx_slice_z,ws_rdx_slice_w
ws_rdx_slice:
 .rept 10
 nop
 .endr
 vmovdqu xmm0,XMMWORD PTR [rdx]
ws_rdx_slice_x:
 vmovss DWORD PTR [rcx+0x30],xmm0
 vshufps xmm1,xmm0,xmm0,0x55
ws_rdx_slice_y:
 vmovss DWORD PTR [rcx+0x34],xmm1
 vshufps xmm2,xmm0,xmm0,0xaa
ws_rdx_slice_z:
 vmovss DWORD PTR [rcx+0x38],xmm2
ws_rdx_slice_w:
 mov DWORD PTR [rcx+0x3c],0x3f800000
 ret
ws_rdx_slice_end:
.globl ws_rdx_scalar,ws_rdx_scalar_end
ws_rdx_scalar:
 mov eax,DWORD PTR [rdx]
 mov DWORD PTR [rcx+0x30],eax
 mov eax,DWORD PTR [rdx+4]
 mov DWORD PTR [rcx+0x34],eax
 mov eax,DWORD PTR [rdx+8]
 mov DWORD PTR [rcx+0x38],eax
 mov DWORD PTR [rcx+0x3c],0x3f800000
 ret
ws_rdx_scalar_end:
.globl ws_rdx_half,ws_rdx_half_end
ws_rdx_half:
 vmovsd xmm0,QWORD PTR [rdx]
 vmovss DWORD PTR [rcx+0x30],xmm0
 vshufps xmm1,xmm0,xmm0,0x55
 vmovss DWORD PTR [rcx+0x34],xmm1
 vmovss xmm2,DWORD PTR [rdx+8]
 vmovss DWORD PTR [rcx+0x38],xmm2
 mov DWORD PTR [rcx+0x3c],0x3f800000
 ret
ws_rdx_half_end:
.globl ws_rdx_local,ws_rdx_local_call,ws_rdx_local_end
ws_rdx_local:
 sub rsp,0x58
 mov DWORD PTR [rsp+0x20],0x41200000
 mov DWORD PTR [rsp+0x24],0x41a00000
 mov DWORD PTR [rsp+0x28],0x41f00000
 lea rdx,[rsp+0x20]
ws_rdx_local_call:
 call ws_rdx_slice
 add rsp,0x58
 ret
ws_rdx_local_end:
.globl ws_rdx_inherited,ws_rdx_inherited_call,ws_rdx_inherited_end
ws_rdx_inherited:
 sub rsp,0x28
ws_rdx_inherited_call:
 call ws_rdx_slice
 add rsp,0x28
 ret
ws_rdx_inherited_end:
